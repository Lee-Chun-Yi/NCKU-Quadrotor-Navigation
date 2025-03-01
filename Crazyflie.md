# Part1. 基本操作
### 1. The Coordinate System of the Crazyflie
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-03-02%20030438.png)
### 2. PID tuning

增益 ( Kp )（比例增益）、( Ki )（積分增益）和 ( Kd )（微分增益）用於調整 PID 控制器：

1. **( Kp )：** 調整對當前誤差的響應靈敏度。較高的 ( Kp ) 會提高響應速度，但可能導致overshoot。  
2. **( Ki )：** 根據過去誤差的累積來調整響應。較高的 ( Ki ) 能夠消除穩態誤差，但可能導致系統不穩定。  
3. **( Kd )：** 根據誤差變化率來調整響應。較高的 ( Kd ) 能夠提供阻尼效果，減少超調和震盪。  

調整 PID 控制器的關鍵在於平衡這些增益，以實現穩定且精確的控制。  

[官網PID tuning 操作](https://www.bitcraze.io/documentation/tutorials/pid-tuning-guide/)

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
