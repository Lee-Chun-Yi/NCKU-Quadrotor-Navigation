# Part1. 控制方法
### 1. 軌跡飛行
- [建立 Craztflie 的虛擬坐標系 執行 多點飛行](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/positioning/initial_position.py)
- [利用 RPYT 執行 懸停+8字飛行](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/positioning/flowsequenceSync.py)

### 2. 懸停
- [利用水平速度與高度 執行 懸停](https://github.com/ataffanel/crazyflie-push-demo/blob/8b9b2e8/src/push.c#L105-L106)

### 3. 懸停
- [利用水平速度與高度 執行 懸停](https://github.com/ataffanel/crazyflie-push-demo/blob/8b9b2e8/src/push.c#L105-L106)

# Part2. Crazyflie Python Library
### 1. Cflib Library
- [RPYT控制 & 座標系控制 指令](https://github.com/bitcraze/crazyflie-lib-python/blob/master/cflib/crazyflie/commander.py)

### 2. MotionCommander
- [利用速度 or 座標系 執行上升, 下降, 圓型 飛行](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/autonomy/motion_commander_demo.py)
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/positioning/motion_commander/)

### 3. PositionHlCommander
- [起飛, 降落, 移動到指定位置, 獲取當前座標](https://github.com/bitcraze/crazyflie-lib-python/blob/master/cflib/positioning/position_hl_commander.py)
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/positioning/position_hl_commander/)

### 4. 電池電量管理
- [官網](https://github.com/bitcraze/crazyflie-firmware/blob/crazyflie2/hal/src/pm_f405.c#L172)

### 5. Crazyflie 關閉, 重啟, 
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/utils/power_switch/)

### 6. 遠端連線驅動程式
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/crtp/crtpdriver/)

### 7. 開源資料分析器
- [Wireshark](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/development/wireshark/)

### 8. 回報當前位置
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/crazyflie/extpos/)

# Part3.故障排除
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

