# Part1.  Crazyflie 2.1 Custom Control System Overview
## 1. Hardware Platform
### [Crazyflie 2.1](https://www.bitcraze.io/products/crazyflie-2-1-plus/)

A lightweight, modular quadrotor platform designed for indoor research and development.
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-03-02%20030438.png)

**Key Features:**
- Size: 92 mm motor-to-motor  
- Weight: ~27g (with battery)  
- Open-source hardware & firmware  
- Bluetooth & radio communication  
- Support for sensor decks and extensions

###  [Motion Capture Marker Deck](https://www.bitcraze.io/products/motion-capture-marker-deck/)
An add-on deck for motion capture systems like **Vicon** or **OptiTrack**, allowing sub-millimeter tracking accuracy.

**Used in this project for:**
- Real-time 6DoF position feedback  
- Replacing the onboard IMU  
- Accurate external pose estimation

###  [Crazyradio PA](https://www.bitcraze.io/products/crazyradio-pa/)
A long-range 2.4GHz USB radio dongle used for communicating with Crazyflie over the **CRTP protocol**.

**Used in this project for:**
- Reliable wireless communication between Crazyflie and control PC  
- Supports up to 1Mbit/s data rate  
- Essential for real-time command and feedback when not using Bluetooth
  
### 2. Commander Framework
🔗 [Commander setpoints documentation](https://www.bitcraze.io/documentation/repository/crazyflie-firmware/master/functional-areas/sensor-to-control/commanders_setpoints/)

The **Commander** is the firmware layer responsible for translating external setpoints (e.g., velocity, position, attitude) into flight control actions.  
This project bypasses the Crazyflie onboard controller and sends **PWM signals directly**, forming a fully custom outer-loop system.

### 3. Firmware and Software Resources
The following open-source Bitcraze repositories were used and referenced in this project:

-  [Crazyflie Firmware (C)](https://github.com/bitcraze/crazyflie-firmware/tree/master)  
  Handles onboard attitude estimation, motor mixing, and stabilization.  
  Referenced for motor PWM logic and commander structure.

-  [Crazyflie Clients (Python GUI)](https://github.com/bitcraze/crazyflie-clients-python)  
  Official PC GUI for controlling Crazyflie.  
  Used for early testing and packet format reference.

-  [Crazyflie Python Library](https://github.com/bitcraze/crazyflie-lib-python)  
  Python API to send setpoints, read logs, and access low-level communication.  
  Extensively used in this project to build real-time control loops and interface with Vicon.


# Part2. 故障排除
### 1. 無線通訊異常、飛行參數錯誤、恢復初廠設置
1. 拔掉你的 Crazyradio
2. 使用 USB 線將 Crazyflie 連接到 PC
3. 在 CMD 執行
```
  python3 write-eeprom.py
```

### 2. Crazyflie 不平衡
1. 確保電池位於中央。
2. 檢查螺旋槳是否能自由旋轉。
3. 檢查螺旋槳是否平衡，[平衡螺旋槳](https://www.bitcraze.io/documentation/tutorials/balancing-propellers/)

### 3. UDP port 強制關閉
1. 開啟 CMD

2. 根據欲關閉的port(52001)，填入
```
netstat -ano | findstr :52001
```
3. 根據CMD顯示的值(52112)，填入
```
taskkill /PID  52112 /F
```

