import time
import socket
import struct
import threading
import logging
import os
from threading import Event, Lock
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.log import LogConfig
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.utils import uri_helper
import cflib.crtp
from cflib.utils.power_switch import PowerSwitch

# Initialize Crazyflie drivers
cflib.crtp.init_drivers()

# Set Crazyflie connection URI
URI = uri_helper.uri_from_env(default="radio://0/80/2M")

# Shared variables
vicon_data = None
control_data = {'roll': 0.0, 'pitch': 0.0, 'yaw': 0.0, 'throttle': 0}
battery_voltage = None
stop_event = Event()
stop_flight = Event()
lock = Lock()

def listen_port_8889():
    global vicon_data
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("127.0.0.1", 8889))
    sock.setblocking(False)
    while not stop_event.is_set():
        try:
            while True:
                data, _ = sock.recvfrom(1024)
                if len(data) == 24:
                    with lock:
                        vicon_data = struct.unpack('<6f', data)
        except BlockingIOError:
            time.sleep(0.05)

def listen_port_8888():
    global control_data
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("127.0.0.1", 8888))
    sock.setblocking(False)

    loop_rate = 100  # Hz
    dt = 1.0 / loop_rate
    next_time = time.time()

    while not stop_event.is_set():
        try:
            while True:
                data, _ = sock.recvfrom(1024)
                if len(data) == 16:
                    roll, pitch, yaw, throttle_float = struct.unpack('<4f', data)
                    with lock:
                        control_data['roll'] = max(-5.0, min(5.0, roll))
                        control_data['pitch'] = max(-5.0, min(5.0, pitch))
                        control_data['yaw'] = yaw
                        control_data['throttle'] = max(0, min(20000, int(throttle_float))) + 40000
        except BlockingIOError:
            pass  # Non-blocking, continue if no data

        # Precise loop timing
        next_time += dt
        sleep_duration = next_time - time.time()
        if sleep_duration > 0:
            time.sleep(sleep_duration)
        else:
            next_time = time.time()  # Reset to avoid drift

class BatteryLogger:
    def __init__(self, cf):
        self._cf = cf
        self._lg_battery = LogConfig(name="Battery", period_in_ms=100)
        self._lg_battery.add_variable("pm.vbat", "FP16")
        try:
            self._cf.log.add_config(self._lg_battery)
            self._lg_battery.data_received_cb.add_callback(self._log_data)
            self._lg_battery.start()
        except KeyError:
            print("Logging configuration error")

    def _log_data(self, timestamp, data, logconf):
        global battery_voltage
        with lock:
            battery_voltage = data["pm.vbat"]

def send_start_signal_to_matlab():
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(b'start', ('127.0.0.1', 51001))
        print("🚀 Sent 'start' signal to MATLAB (port 51001)")
        return True
    except Exception as e:
        print(f"⚠️ Failed to send 'start' signal: {e}")
        return False
    finally:
        sock.close()

def send_attitude_commands(scf, duration):
    commander = scf.cf.commander
    commander.send_setpoint(0.0, 0.0, 0.0, 0)
    print("Motors armed, waiting for takeoff...")
    time.sleep(1)
    stop_flight.clear()

    print("🔋 Measuring battery voltage (2 sec)...")
    samples = []
    start_time = time.time()
    while time.time() - start_time < 2.0:
        with lock:
            if battery_voltage:
                samples.append(battery_voltage)
        commander.send_setpoint(0.0, 0.0, 0.0, 0)
        time.sleep(0.01)  # 100Hz sampling

    avg_voltage = sum(samples) / len(samples) if samples else 4.0

    if not send_start_signal_to_matlab():
        print("❌ Aborting flight: Failed to send 'start' signal to MATLAB")
        return

    print("✈️ Starting flight control loop (100Hz)")

    flight_start_time = time.time()
    last_print_time = 0

    loop_rate = 100  # Hz
    dt = 1.0 / loop_rate
    next_time = time.time()

    while time.time() - flight_start_time < duration:
        current_time = time.time()

        if stop_flight.is_set():
            break

        with lock:
            roll, pitch, yaw, throttle = control_data.values()
            if throttle <= 38000:
                roll, pitch, yaw = 0.0, 0.0, 0.0
            v_data = vicon_data if vicon_data else (0, 0, 0, 0, 0, 0)
            battery = battery_voltage if battery_voltage else 4.0

        commander.send_setpoint(roll, pitch, yaw, throttle)

        # Print info every 0.1 sec (10Hz)
        if current_time - last_print_time >= 0.1:
            print(f"X: {v_data[0]:.3f}, Y: {v_data[1]:.3f}, Z: {v_data[2]:.3f}, "
                  f"RotX: {v_data[3]:.3f}, RotY: {v_data[4]:.3f}, RotZ: {v_data[5]:.3f} | "
                  f"R: {roll:.3f}, P: {pitch:.3f}, Y: {yaw:.3f}, T: {throttle:>5d} | B: {battery:.2f}V")
            last_print_time = current_time

        # Precise timing to prevent drift
        next_time += dt
        sleep_duration = next_time - time.time()
        if sleep_duration > 0:
            time.sleep(sleep_duration)
        else:
            next_time = time.time()

    print("Normal landing sequence initiated.")
    smooth_landing(commander)


def smooth_landing(commander):
    print("Thrust cut (Thrust=0)")
    commander.send_setpoint(0.0, 0.0, 0.0, 0)
    time.sleep(0.1)
    print(" Ramping down to 42000")
    ramp_time_1 = 1.5
    steps = 10
    current_thrust = 45000
    for i in range(steps + 1):
        progress = i / steps
        thrust_level = int(current_thrust - progress * (current_thrust - 35000))
        commander.send_setpoint(0.0, 0.0, 0.0, thrust_level)
        time.sleep(ramp_time_1 / steps)
    print(" Ramping down to 0")
    ramp_time_2 = 0.5
    for i in range(steps + 1):
        progress = i / steps
        thrust_level = int(40000 - progress * 40000)
        commander.send_setpoint(0.0, 0.0, 0.0, thrust_level)
        time.sleep(ramp_time_2 / steps)
    print("🛬 Landing complete.")

def power_cycle_crazyflie():
    print("Power cycling Crazyflie...")
    ps = PowerSwitch(URI)
    ps.stm_power_down()
    time.sleep(3)
    ps.stm_power_up()
    time.sleep(3)
    print("Power cycle complete")

def clear_udp_ports(ports):
    for port in ports:
        result = os.popen(f'netstat -ano | findstr :{port}').read()
        if result:
            lines = result.splitlines()
            for line in lines:
                parts = line.split()
                if len(parts) >= 5:
                    pid = parts[-1]
                    print(f"Terminating process {pid} using port {port}...")
                    os.system(f'taskkill /PID {pid} /F')

if __name__ == "__main__":
    logging.basicConfig(level=logging.ERROR)
    stop_event.clear()
    stop_flight.clear()

    threading.Thread(target=listen_port_8889, daemon=True).start()
    threading.Thread(target=listen_port_8888, daemon=True).start()

    with SyncCrazyflie(URI, cf=Crazyflie(rw_cache='./cache')) as scf:
        BatteryLogger(scf.cf)
        try:
            send_attitude_commands(scf, duration=120)
        except KeyboardInterrupt:
            try:
                print("🛑 Emergency stop triggered from main! Executing smooth landing...")
                stop_flight.set()
                smooth_landing(scf.cf.commander)
            except KeyboardInterrupt:
                commander = scf.cf.commander
                commander.send_setpoint(0.0, 0.0, 0.0, 0)

        print("Flight mission completed.")

    power_cycle_crazyflie()
    clear_udp_ports([8888, 8889])
    stop_event.set()
    print("Program terminated.")
