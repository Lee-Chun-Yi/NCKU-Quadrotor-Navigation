import socket
import struct
import threading
import time
from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO

# 設定 UDP 端口
LOCAL_IP = "127.0.0.1"
PORT1 = 8888  # Roll, Pitch, Yaw, Thrust
PORT2 = 8889  # tran_x, tran_y, tran_z, rot_x, rot_y, rot_z
BUFFER_SIZE = 1024

# 建立 Flask 應用
app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*", async_mode="threading")

# 初始化變數
running = threading.Event()
running.clear()  # 讓程式啟動時不接收 UDP 數據

@app.route('/start', methods=['POST'])
def start_control():
    running.set()
    print("✅ 系統開始運行")
    return jsonify({"status": "started", "message": "系統已開始運行"})

@app.route('/stop', methods=['POST'])
def stop_control():
    running.clear()
    print("🛑 系統已停止")
    return jsonify({"status": "stopped", "message": "系統已停止運行"})

def parse_udp_data(data):
    """解析來自 UDP 的二進制數據 (float32)"""
    length = len(data)
    if length == 24:  # 8889 應該是 24 bytes (6 * float32)
        return struct.unpack("<6f", data)
    elif length == 16:  # 8888 應該是 16 bytes (4 * float32)
        return struct.unpack("<4f", data)
    else:
        return None  # 長度錯誤時，直接返回 None

def udp_listener(ip, port, update_func):
    """監聽 UDP 端口，解析數據"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((ip, port))
    sock.settimeout(5)  # 設定超時，避免程式卡住

    while True:
        if running.is_set():
            try:
                data, addr = sock.recvfrom(BUFFER_SIZE)
                parsed_data = parse_udp_data(data)
                if parsed_data:
                    update_func(parsed_data)
            except socket.timeout:
                continue
            except Exception as e:
                continue
        time.sleep(0.1)

def update_vicon_data(data):
    """更新 Vicon 數據"""
    vicon_data = {
        "tran_x": round(data[0], 3),
        "tran_y": round(data[1], 3),
        "tran_z": round(data[2], 3),
        "rot_x": round(data[3], 3),
        "rot_y": round(data[4], 3),
        "rot_z": round(data[5], 3),
    }
    print(f"✅ [Port 8889] {vicon_data}")
    socketio.emit("vicon_data", vicon_data)

def update_control_data(data):
    """更新控制數據"""
    control_data = {
        "roll": round(data[0], 3),
        "pitch": round(data[1], 3),
        "yaw": round(data[2], 3),
        "thrust": round(data[3], 3),
    }
    print(f"✅ [Port 8888] {control_data}")
    socketio.emit("control_data", control_data)

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == "__main__":
    try:
        # 啟動 UDP 監聽執行緒
        thread1 = threading.Thread(target=udp_listener, args=(LOCAL_IP, PORT1, update_control_data), daemon=True)
        thread2 = threading.Thread(target=udp_listener, args=(LOCAL_IP, PORT2, update_vicon_data), daemon=True)
        thread1.start()
        thread2.start()

        print("✅ 伺服器啟動，按 Ctrl + C 停止")
        socketio.run(app, host="0.0.0.0", port=5000, allow_unsafe_werkzeug=True)

    except KeyboardInterrupt:
        print("\n🛑 伺服器已停止")