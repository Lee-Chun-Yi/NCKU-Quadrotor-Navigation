import time
import socket
import struct
import threading
import logging
from threading import Event, Lock

from cflib.crazyflie import Crazyflie
from cflib.crazyflie.log import LogConfig
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.utils import uri_helper
import cflib.crtp as crtp
from cflib.utils.power_switch import PowerSwitch
from cflib.crtp.crtpstack import CRTPPacket 

# ========= 全域參數（集中管理） =========
STREAM_HZ  = 200.0   # Python 發送 xyz target 的頻率(Hz)，也用於降落時 PWM 轉發頻率
TAKEOFF_Z  = 700.0   # 起飛目標高度（單位請與模型一致：m 或 mm）
TAKEOFF_T  = 4.0     # 起飛插值秒數（S-curve）
LAND_T     = 4.0     # 降落插值秒數（S-curve）
UDP_TARGET_PORT = 51002  # 你的 Simulink UDP Receive 埠號

# ========= 連線與 CRTP 4PWM 設定 =========
crtp.init_drivers()
URI = uri_helper.uri_from_env(default="radio://0/90/2M")

PORT_PWM = 0x0A
CHAN_PWM = 0

ENABLE_PARAM_CANDIDATES = [
    "crtp_pwm.enable",
    "pwm.enable",
    "motorPowerSet.enable",
]

# ========= 共享狀態 =========
vicon_data = None
battery_voltage = None
stop_event = Event()
stop_flight = Event()
lock = Lock()

# ===== PWM desired value (thread-safe) =====
_desired = (0, 0, 0, 0)
_desired_lock = Lock()

def set_desired_pwm_tuple(m1, m2, m3, m4):
    m1 = clamp_u16(m1)
    m2 = clamp_u16(m2)
    m3 = clamp_u16(m3)
    m4 = clamp_u16(m4)
    global _desired
    with _desired_lock:
        _desired = (m1, m2, m3, m4)

def get_desired_pwm_tuple():
    with _desired_lock:
        return _desired

# ========= XYZ 目標輸出（只傳「點」，UDP，<3f> 12 bytes） =========
_xyz_lock = Lock()
_xyz_target = [0.0, 0.0, 0.0]
_xyz_stream_evt = Event()

def set_xyz_target(x: float, y: float, z: float) -> None:
    with _xyz_lock:
        _xyz_target[0] = float(x)
        _xyz_target[1] = float(y)
        _xyz_target[2] = float(z)

def get_xyz_target():
    with _xyz_lock:
        return tuple(_xyz_target)

def start_xyz_stream(host: str = "127.0.0.1", port: int = UDP_TARGET_PORT, rate_hz: float = STREAM_HZ) -> None:
    """以固定頻率把 XYZ 目標送到 Simulink 的 UDP Receive。"""
    if _xyz_stream_evt.is_set():
        return
    _xyz_stream_evt.set()

    def _worker():
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        dt = 1.0 / max(1.0, float(rate_hz))
        next_t = time.time()
        while _xyz_stream_evt.is_set():
            x, y, z = get_xyz_target()
            payload = struct.pack("<3f", float(x), float(y), float(z))
            try:
                sock.sendto(payload, (host, port))
            except Exception:
                pass
            next_t += dt
            slp = next_t - time.time()
            if slp > 0:
                time.sleep(slp)
            else:
                next_t = time.time()
        try:
            sock.close()
        except Exception:
            pass

    threading.Thread(target=_worker, daemon=True).start()

def stop_xyz_stream() -> None:
    _xyz_stream_evt.clear()

# ========= 工具函式 =========
def clamp_u16(v):
    return max(0, min(0xFFFF, int(v)))

def send_packet_compat(cf, pkt):
    # 盡量相容不同 cflib 版本
    try:
        cf.link.send_packet(pkt); return
    except AttributeError:
        pass
    try:
        cf.cf.link.send_packet(pkt); return
    except AttributeError:
        pass
    try:
        cf._link.send_packet(pkt); return  # noqa: SLF001
    except AttributeError:
        pass
    raise RuntimeError("No compatible send_packet on Crazyflie link")

def send_4pwm_packet(cf, m1, m2, m3, m4):
    pk = CRTPPacket()
    pk.port = PORT_PWM
    pk.channel = CHAN_PWM
    pk.data = struct.pack("<HHHH", m1, m2, m3, m4)
    send_packet_compat(cf, pk)


def try_lower_radio_retries(cf):
    try:
        link = getattr(cf, 'link', getattr(cf, '_link', None))
        radio = getattr(link, 'radio', None)
        if radio is not None:
            if hasattr(radio, 'set_arc'):
                radio.set_arc(0)
            if hasattr(radio, 'set_ard_time'):
                radio.set_ard_time(0)
    except Exception:
        pass

def try_set_enable(cf, state: int) -> str:
    for pname in ENABLE_PARAM_CANDIDATES:
        try:
            cf.param.set_value(pname, str(int(bool(state))))
            time.sleep(0.05)
            print(f"[ENABLE] Tried set {pname}={int(bool(state))}")
            return pname
        except Exception:
            continue
    return ""

# ======== Cosine S-curve（0→1）：(1 - cos(pi*t/T))/2，限制 t ∈ [0,T] ========
def _s_curve01(t: float, T: float) -> float:
    if T <= 0:
        return 1.0
    if t <= 0:
        return 0.0
    if t >= T:
        return 1.0
    from math import cos, pi
    return (1.0 - cos(pi * t / T)) * 0.5

# ========= High-rate PWM loop =========
SPIN_TAIL = False  # set True to busy-wait for final microseconds

def pwm_forward_loop(cf, duration_sec: float, rate_hz: float, spin_tail: bool = SPIN_TAIL):
    period = 1.0 / max(1.0, float(rate_hz))
    now = time.perf_counter
    sleep = time.sleep
    send = send_4pwm_packet
    getp = get_desired_pwm_tuple
    is_stop = stop_flight.is_set

    next_t = now()
    end_t = next_t + max(0.0, float(duration_sec))
    tick_count = 0
    late_count = 0
    max_late = 0.0

    while now() < end_t and not is_stop():
        m1, m2, m3, m4 = getp()
        send(cf, m1, m2, m3, m4)
        tick_count += 1

        next_t += period
        t = now()
        slack = next_t - t
        if slack > 0.0006:
            sleep(slack - 0.0003)
            t = now()
            slack = next_t - t
        if slack > 0:
            if spin_tail:
                while (t := now()) < next_t:
                    pass
            else:
                sleep(slack)
                t = now()
        else:
            t = now()
        if t > next_t:
            late_count += 1
            diff = t - next_t
            if diff > max_late:
                max_late = diff

    return tick_count, late_count, max_late

# ========= UDP 接收 =========
def listen_port_8889():
    """Vicon: 6×float => (x,y,z,rotX,rotY,rotZ)"""
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
                        vicon_data = struct.unpack("<6f", data)
        except BlockingIOError:
            time.sleep(0.01)

def listen_port_8888():
    """
    4PWM：支援兩種封包
      - 8 bytes:  <HHHH>  -> 4×uint16
      - 16 bytes: <4f>    -> 4×float 轉為 uint16
    （若收到 12 bytes 的 <3f>，將被忽略—那是 XYZ 目標輸出格式）
    """
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("127.0.0.1", 8888))
    sock.setblocking(False)

    while not stop_event.is_set():
        try:
            while True:
                data, _ = sock.recvfrom(1024)
                if len(data) == 8:
                    m1, m2, m3, m4 = struct.unpack("<HHHH", data)
                elif len(data) == 16:
                    f1, f2, f3, f4 = struct.unpack("<4f", data)
                    m1 = clamp_u16(f1); m2 = clamp_u16(f2)
                    m3 = clamp_u16(f3); m4 = clamp_u16(f4)
                else:
                    continue
                set_desired_pwm_tuple(m1, m2, m3, m4)
        except BlockingIOError:
            time.sleep(0.001)

# ========= 電池紀錄 =========
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

# ========= 降落（S-curve：鎖 XY=Vicon、Z 平滑到 0；期間持續轉發 PWM） =========
def land_via_target_lock_xy_and_ramp_z(cf, used_param_name: str,
                                       duration_sec: float,
                                       pwm_forward_rate_hz: float):
    with lock:
        v = vicon_data
    if v is not None:
        x_now, y_now, z0 = float(v[0]), float(v[1]), float(v[2])
    else:
        x_now, y_now, z0 = get_xyz_target()

    print(f"🛬 Landing via S-curve: lock xy=({x_now:.3f},{y_now:.3f}), z:{z0:.3f} → 0 in {duration_sec:.1f}s")

    t0 = time.time()
    dt = 1.0 / max(1.0, pwm_forward_rate_hz)
    while True:
        el = time.time() - t0
        if el >= duration_sec:
            break

        s = _s_curve01(el, duration_sec)    # 0→1
        z_t = z0 * (1.0 - s)                # 平滑下降
        set_xyz_target(x_now, y_now, z_t)

        m1, m2, m3, m4 = get_desired_pwm_tuple()
        send_4pwm_packet(cf, m1, m2, m3, m4)

        time.sleep(dt)

    # 時間到：PWM=0，並嘗試 disable
    for _ in range(3):
        send_4pwm_packet(cf, 0, 0, 0, 0)
        time.sleep(0.002)
    if used_param_name:
        try_set_enable(cf, 0)
        print(f"[DISABLE] Set {used_param_name}=0")
    print("✅ Landing complete (S-curve → hard stop).")

# ========= 主流程（含：起飛 S-curve、巡航、降落 S-curve） =========
def run_4pwm_flight(scf,
                    duration_sec=120.0,
                    rate_hz=STREAM_HZ,
                    takeoff_T: float = TAKEOFF_T,
                    takeoff_forward_pwm: bool = True,
                    z_final_cmd: float = TAKEOFF_Z,
                    land_T: float = LAND_T):
    cf = scf.cf

    used_param = try_set_enable(cf, 1)
    if not used_param:
        print("[WARN] 未能設定任何已知的 PWM enable 參數，請確認韌體端已允許 CRTP 4PWM。")

    # 電池量測 1 秒
    print("🔋 Measuring battery voltage (2 sec)...")
    samples = []
    t0 = time.time()
    while time.time() - t0 < 1.0:
        with lock:
            if battery_voltage:
                samples.append(battery_voltage)
        time.sleep(0.01)
    avg_v = sum(samples) / len(samples) if samples else 4.0
    print(f"Avg battery ≈ {avg_v:.2f} V")

    # ===== 起飛：S-curve 將 z 從 0 → z_final（xy 取目前 target 的 xy） =====
    x_final, y_final, _ = get_xyz_target()
    z_final = float(z_final_cmd)
    z0 = 0.0
    print(f"🚀 Takeoff S-curve: z {z0:.3f} → {z_final:.3f} in {takeoff_T:.1f}s (xy target = {x_final:.3f},{y_final:.3f})")
    t0 = time.time()
    dt_pwm = 1.0 / max(1.0, float(rate_hz))
    while True:
        el = time.time() - t0
        if el >= takeoff_T:
            break
        s = _s_curve01(el, takeoff_T)               # 0→1
        z_t = z0 + (z_final - z0) * s               # 平滑上升
        set_xyz_target(x_final, y_final, z_t)

        if takeoff_forward_pwm:
            m1, m2, m3, m4 = get_desired_pwm_tuple()
            send_4pwm_packet(cf, m1, m2, m3, m4)

        time.sleep(dt_pwm)

    # 到達最終高度（再寫一次保險）
    set_xyz_target(x_final, y_final, z_final)

    # ===== 巡航：高頻率 4PWM 轉發 =====
    print(f"✈️ 進入巡航（{rate_hz:.1f} Hz, {duration_sec:.1f} s）")
    ticks, late, max_late = pwm_forward_loop(cf, duration_sec, rate_hz)
    print(f"⏱️ Loop ticks:{ticks} late:{late} max_late:{max_late*1000:.3f} ms")

    # ===== 降落（S-curve，使用 land_T；持續以 rate_hz 轉發 PWM）=====
    print("Normal landing sequence (S-curve while forwarding PWM).")
    land_via_target_lock_xy_and_ramp_z(cf, used_param,
                                       duration_sec=land_T,
                                       pwm_forward_rate_hz=rate_hz)

# ========= 其他工具：電源 =========
def power_cycle_crazyflie():
    print("Power cycling Crazyflie...")
    ps = PowerSwitch(URI)
    ps.stm_power_down()
    time.sleep(2)
    ps.stm_power_up()
    time.sleep(2)
    print("Power cycle complete")

# ========= Main =========
if __name__ == "__main__":
    logging.basicConfig(level=logging.ERROR)
    stop_event.clear()
    stop_flight.clear()

    # 先把 target 設為地面 0，再開始對 MATLAB 送資料（確保起飛在 scope 能看到插值）
    set_xyz_target(0.0, 0.0, 0.0)
    start_xyz_stream(port=UDP_TARGET_PORT, rate_hz=STREAM_HZ)

    threading.Thread(target=listen_port_8889, daemon=True).start()
    threading.Thread(target=listen_port_8888, daemon=True).start()

    with SyncCrazyflie(URI, cf=Crazyflie(rw_cache='./cache')) as scf:
        BatteryLogger(scf.cf)
        try_lower_radio_retries(scf.cf)
        try:
            run_4pwm_flight(
                scf,
                duration_sec=120.0,
                rate_hz=STREAM_HZ,         # 巡航與降落 PWM 轉發頻率
                takeoff_T=TAKEOFF_T,       # 起飛插值秒數
                takeoff_forward_pwm=True,
                z_final_cmd=TAKEOFF_Z,     # 起飛高度
                land_T=LAND_T              # 降落插值秒數
            )
        except KeyboardInterrupt:
            try:
                print("🛑 Emergency stop! Landing (S-curve, forwarding PWM)...")
                stop_flight.set()  # 結束巡航，但 listener 繼續收
                land_via_target_lock_xy_and_ramp_z(
                    scf.cf,
                    used_param_name="",
                    duration_sec=LAND_T,
                    pwm_forward_rate_hz=STREAM_HZ
                )
            except KeyboardInterrupt:
                for _ in range(3):
                    send_4pwm_packet(scf.cf, 0, 0, 0, 0)
                    time.sleep(0.002)

        print("Flight mission completed.")

    stop_event.set()
    stop_xyz_stream()
    print("Program terminated.")
