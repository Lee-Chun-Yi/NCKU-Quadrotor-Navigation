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

### 4. 紀錄：電池電壓, Cureent position, attitude 
- [使用logging 紀錄](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/logging/basiclog.py)
- [官網：回報當前位置](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/crazyflie/extpos/)

### 5. Crazyflie 關閉, 重啟, 
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/utils/power_switch/)

### 6. 遠端連線驅動程式
- [官網](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/crtp/crtpdriver/)

### 7. 開源資料分析器
- [Wireshark](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/development/wireshark/)

### 8. Crazyflie library 本地地址
```
C:\Users\PC\AppData\Local\Programs\Python\Python312\Lib\site-packages\cflib\crazyflie
```

