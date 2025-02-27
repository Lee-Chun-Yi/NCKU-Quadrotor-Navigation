# 1. 控制方法
### 軌跡飛行
- [建立 Craztflie 的虛擬坐標系 執行 多點飛行](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/positioning/initial_position.py)
- [利用 RPYT 執行 懸停+8字飛行](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/positioning/flowsequenceSync.py)

### 懸停
- [利用水平速度與高度 執行 懸停](https://github.com/ataffanel/crazyflie-push-demo/blob/8b9b2e8/src/push.c#L105-L106)

### 懸停
- [利用水平速度與高度 執行 懸停](https://github.com/ataffanel/crazyflie-push-demo/blob/8b9b2e8/src/push.c#L105-L106)

# 2. Crazyflie Python Library
### Cflib Library
- [RPYT控制 & 座標系控制 指令](https://github.com/bitcraze/crazyflie-lib-python/blob/master/cflib/crazyflie/commander.py)

### MotionCommander
- [利用速度 or 座標系 執行上升, 下降, 圓型 飛行](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/autonomy/motion_commander_demo.py)
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/positioning/motion_commander/)

### PositionHlCommander
- [起飛, 降落, 移動到指定位置, 獲取當前座標](https://github.com/bitcraze/crazyflie-lib-python/blob/master/cflib/positioning/position_hl_commander.py)
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/positioning/position_hl_commander/)

### 電池電量管理
- [官網](https://github.com/bitcraze/crazyflie-firmware/blob/crazyflie2/hal/src/pm_f405.c#L172)

### Crazyflie 關閉, 重啟, 
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/utils/power_switch/)

### 遠端連線驅動程式
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/crtp/crtpdriver/)

### 開源資料分析器
- [Wireshark](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/development/wireshark/)

### 無線通訊異常、飛行參數錯誤、恢復初廠設置
1. 拔掉你的 Crazyradio
2. 使用 USB 線將 Crazyflie 連接到 PC
3. 在 CMD 執行
```
  python3 write-eeprom.py
```

### 回報當前位置
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/crazyflie/extpos/)
