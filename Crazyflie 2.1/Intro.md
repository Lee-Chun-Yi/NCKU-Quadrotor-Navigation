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

### 2. Commander Framework

### 3. Firmware and Software Resources


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

