#!/usr/bin/env python3
"""Linux-oriented runner for high-rate Crazyflie PWM streaming."""

import argparse
import csv
import os
import socket
import struct
import sys
import threading
import time
import subprocess

# ===== Try import Crazyflie helper functions =====
try:
    from Crazyflie_4PID_control import (
        send_4pwm_packet,
        get_desired_pwm_tuple,
        set_desired_pwm_tuple,
    )
except Exception as exc:
    print(
        "Failed to import Crazyflie_4PID_control helpers:",
        exc,
        file=sys.stderr,
    )
    sys.exit(1)

try:  # optional helper for enabling/disabling motors
    from Crazyflie_4PID_control import try_set_enable  # type: ignore
except Exception:  # pragma: no cover - optional
    try_set_enable = None  # type: ignore

# ===== Setup snippet for --print-setup =====
SETUP_SNIPPET = """# 1) Python & cflib
sudo apt update
sudo apt install -y python3-pip python3-venv
python3 -m pip install --upgrade pip
python3 -m pip install cflib

# 2) udev rule for Crazyradio (VID:PID 1915:7777)
cat <<'EOF' | sudo tee /etc/udev/rules.d/99-crazyradio.rules
SUBSYSTEM=="usb", ATTR{idVendor}=="1915", ATTR{idProduct}=="7777", MODE="0666"
EOF
sudo udevadm control --reload-rules && sudo udevadm trigger

# 3) Run with RT & CPU isolation (example: core 3, RT prio 20)
sudo chrt -f 20 taskset -c 3 python3 Python/4PID/cf4pwm_linux_runner.py --rate 500 --affinity 3 --rt
"""


def print_setup() -> None:
    """Print Linux setup and tuning instructions."""
    print(SETUP_SNIPPET)
    print(
        "\nLightweight desktop tips: run from a TTY (no compositor), "
        "disable animations/effects, set CPU governor to performance, "
        "plug AC power, close heavy apps."
    )


# ===== Utility functions =====

def clamp_u16(v):
    return max(0, min(0xFFFF, int(v)))


def parse_cpu_list(spec: str):
    try:
        return {int(x.strip()) for x in spec.split(',') if x.strip()}
    except Exception:
        return set()


def apply_process_settings(args):
    if args.use_chrt:
        try:
            if os.sched_getscheduler(0) != os.SCHED_FIFO:
                subprocess.call(
                    ["chrt", "-f", str(args.rt_prio), "python3", *sys.argv]
                )
                sys.exit(0)
        except Exception:
            pass

    if args.nice is not None:
        try:
            os.nice(args.nice)
        except Exception as e:
            print(f"Warning: nice {args.nice} failed: {e}", file=sys.stderr)

    if args.affinity:
        cores = parse_cpu_list(args.affinity)
        if cores:
            try:
                os.sched_setaffinity(0, cores)
            except Exception as e:
                print(f"Warning: affinity {cores} failed: {e}", file=sys.stderr)

    if args.rt:
        try:
            os.sched_setscheduler(0, os.SCHED_FIFO, os.sched_param(args.rt_prio))
        except Exception as e:
            print(f"Warning: SCHED_FIFO failed: {e}", file=sys.stderr)


# ===== UDP consumer thread =====

def udp_receiver_thread():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sock.bind(("", 8888))
    except Exception:
        return
    sock.setblocking(False)
    while True:
        try:
            while True:
                data, _ = sock.recvfrom(1024)
                if len(data) == 8:
                    m1, m2, m3, m4 = struct.unpack("<HHHH", data)
                elif len(data) == 16:
                    f1, f2, f3, f4 = struct.unpack("<4f", data)
                    m1 = clamp_u16(f1)
                    m2 = clamp_u16(f2)
                    m3 = clamp_u16(f3)
                    m4 = clamp_u16(f4)
                else:
                    continue
                set_desired_pwm_tuple(m1, m2, m3, m4)
        except BlockingIOError:
            time.sleep(0.001)
        except Exception:
            break


# ===== Metrics utilities =====

def percentile(sorted_list, pct):
    if not sorted_list:
        return 0.0
    k = int((len(sorted_list) - 1) * pct / 100.0)
    return float(sorted_list[k])


# ===== Probe mode =====

def run_probe(args):
    try:
        from cflib.drivers.crazyradio import Crazyradio
    except Exception as exc:
        print("cflib Crazyradio driver required for probe mode:", exc, file=sys.stderr)
        return

    if args.scan_all:
        channels = list(range(0, 126))
    else:
        channels = [int(x) for x in args.channels.split(',') if x]
    arc_list = [int(x) for x in args.arc_list.split(',') if x]
    ard_list = [int(x) for x in args.ard_list.split(',') if x]

    results = []
    radio = Crazyradio()
    radio.set_data_rate(Crazyradio.DR_2MPS)
    packet = bytes([0xA0]) + b"\x00" * 8
    for ch in channels:
        try:
            radio.set_channel(ch)
        except Exception:
            continue
        for arc in arc_list:
            if hasattr(radio, "set_arc"):
                try:
                    radio.set_arc(arc)
                except Exception:
                    pass
            for ard in ard_list:
                if hasattr(radio, "set_ard_time"):
                    try:
                        radio.set_ard_time(ard)
                    except Exception:
                        pass
                sent = 0
                acked = 0
                for _ in range(1000):
                    try:
                        res = radio.send_packet(packet)
                        sent += 1
                        if isinstance(res, tuple):
                            ok = bool(res[0])
                        else:
                            ok = bool(res)
                        if ok:
                            acked += 1
                    except Exception:
                        sent += 1
                results.append((ch, arc, ard, sent, acked))
    radio.close()

    results.sort(key=lambda r: (r[4] / r[3]) if r[3] else 0.0, reverse=True)
    print("channel arc ard sent acked ratio")
    for ch, arc, ard, sent, acked in results:
        ratio = acked / sent if sent else 0.0
        print(f"{ch:3d} {arc:3d} {ard:3d} {sent:5d} {acked:5d} {ratio:6.3f}")

    if args.csv:
        with open(args.csv, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(["channel", "arc", "ard", "sent", "acked", "success_ratio"])
            for ch, arc, ard, sent, acked in results:
                ratio = acked / sent if sent else 0.0
                w.writerow([ch, arc, ard, sent, acked, f"{ratio:.6f}"])


# ===== Run mode =====

def run_stream(args):
    try:
        import cflib.crtp as crtp
        from cflib.crazyflie import Crazyflie
        from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
        from cflib.utils import uri_helper
    except Exception as exc:
        print("cflib is required for run mode:", exc, file=sys.stderr)
        return

    crtp.init_drivers()
    uri = args.uri or uri_helper.uri_from_env(default="radio://0/90/2M")

    threading.Thread(target=udp_receiver_thread, daemon=True).start()

    with SyncCrazyflie(uri, cf=Crazyflie(rw_cache="./cache")) as scf:
        cf = scf.cf
        if try_set_enable:
            try:
                try_set_enable(cf, 1)
            except Exception:
                pass

        link = getattr(cf, "_link", None) or getattr(cf, "link", None)
        radio = getattr(link, "radio", None)
        if radio:
            if args.arc is not None and hasattr(radio, "set_arc"):
                try:
                    radio.set_arc(args.arc)
                except Exception:
                    pass
            if args.ard is not None and hasattr(radio, "set_ard_time"):
                try:
                    radio.set_ard_time(args.ard)
                except Exception:
                    pass

        rate = float(args.rate)
        period = 1.0 / rate
        now = time.perf_counter
        next_t = now()
        ticks = 0
        late = 0
        jitter_all = []
        csv_rows = []
        status_int = float(args.status_interval)
        last_report = now()
        interval_ticks = 0
        interval_late = 0
        interval_jitter = []

        running = True
        try:
            while running:
                m1, m2, m3, m4 = get_desired_pwm_tuple()
                send_4pwm_packet(cf, m1, m2, m3, m4)
                ticks += 1
                interval_ticks += 1

                next_t += period
                t = now()
                slack = next_t - t
                if slack > 0.0006:
                    time.sleep(slack - 0.0003)
                    t = now()
                    slack = next_t - t
                if args.spin_tail:
                    while (t := now()) < next_t:
                        pass
                else:
                    t = now()

                overrun = t > next_t
                if overrun:
                    late += 1
                    interval_late += 1
                period_us = (t - (next_t - period)) * 1e6
                jitter_us = abs(period_us - period * 1e6)
                jitter_all.append(jitter_us)
                interval_jitter.append(jitter_us)
                if args.csv:
                    csv_rows.append((time.time(), ticks, period_us, jitter_us, int(overrun)))

                if t - last_report >= status_int:
                    dur = t - last_report
                    rate_now = interval_ticks / dur if dur > 0 else 0.0
                    late_pct = (interval_late / interval_ticks * 100.0) if interval_ticks else 0.0
                    sj = sorted(interval_jitter)
                    p50 = percentile(sj, 50)
                    p90 = percentile(sj, 90)
                    p99 = percentile(sj, 99)
                    pmax = sj[-1] if sj else 0.0
                    print(
                        f"rate={rate_now:.1f} Hz | late={late_pct:.1f}% | "
                        f"jitter_us p50={int(p50)} p90={int(p90)} p99={int(p99)} max={int(pmax)}"
                    )
                    last_report = t
                    interval_ticks = 0
                    interval_late = 0
                    interval_jitter.clear()
        except KeyboardInterrupt:
            running = False
        finally:
            for _ in range(3):
                send_4pwm_packet(cf, 0, 0, 0, 0)
                time.sleep(0.002)
            if try_set_enable:
                try:
                    try_set_enable(cf, 0)
                except Exception:
                    pass

        total_time = now()
        rate_avg = ticks / (total_time - (next_t - ticks * period)) if ticks else 0.0
        late_pct = (late / ticks * 100.0) if ticks else 0.0
        sj_all = sorted(jitter_all)
        p50 = percentile(sj_all, 50)
        p90 = percentile(sj_all, 90)
        p99 = percentile(sj_all, 99)
        pmax = sj_all[-1] if sj_all else 0.0
        print(
            f"Summary: ticks={ticks} late={late} ({late_pct:.2f}%) | "
            f"jitter_us p50={int(p50)} p90={int(p90)} p99={int(p99)} max={int(pmax)}"
        )

        if args.csv:
            with open(args.csv, "w", newline="") as f:
                w = csv.writer(f)
                w.writerow(["timestamp", "loop_no", "period_us", "jitter_us", "overrun"])
                w.writerows(csv_rows)


# ===== Main =====

def main():
    p = argparse.ArgumentParser(description="Linux Crazyflie 4PWM streamer")
    p.add_argument("--uri", default=None)
    p.add_argument("--rate", type=float, default=500.0)
    p.add_argument("--affinity")
    p.add_argument("--rt", action="store_true")
    p.add_argument("--use-chrt", action="store_true")
    p.add_argument("--rt-prio", type=int, default=10)
    p.add_argument("--nice", type=int)
    p.add_argument("--arc", type=int)
    p.add_argument("--ard", type=int)
    p.add_argument("--probe", action="store_true")
    p.add_argument("--channels", default="0,40,80,120")
    p.add_argument("--scan-all", action="store_true")
    p.add_argument("--arc-list", default="0,1,3")
    p.add_argument("--ard-list", default="0,5,10")
    p.add_argument("--csv")
    p.add_argument("--spin-tail", action="store_true")
    p.add_argument("--status-interval", type=float, default=1.0)
    p.add_argument("--print-setup", action="store_true")
    args = p.parse_args()

    if args.print_setup:
        print_setup()
        return

    apply_process_settings(args)

    if args.probe:
        run_probe(args)
    else:
        run_stream(args)


if __name__ == "__main__":
    main()
