# Part1. 基本操作

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
1. 開啟cmd

2. 根據欲關閉的port(52001)，填入
```
netstat -ano | findstr :52001
```
3. 根據CMD顯示的值(52112)，填入
```
taskkill /PID  52112 /F
```
