# 1. Control Methods

### 1. Trajectory Flight
- [Create a virtual coordinate frame for multi-point flight](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/positioning/initial_position.py)
- [Use RPYT to perform hover + figure-eight trajectory](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/positioning/flowsequenceSync.py)

### 2. Hovering
- [Perform hover using horizontal velocity and altitude](https://github.com/ataffanel/crazyflie-push-demo/blob/8b9b2e8/src/push.c#L105-L106)



# 2. Crazyflie Python Library

### 1. Cflib Library
- [RPYT control & coordinate-based control commands](https://github.com/bitcraze/crazyflie-lib-python/blob/master/cflib/crazyflie/commander.py)

### 2. MotionCommander
- [Perform upward/downward/circular flight using velocity or coordinates](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/autonomy/motion_commander_demo.py)
- [Official Docs](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/positioning/motion_commander/)

### 3. PositionHlCommander
- [Takeoff, land, go to position, get current coordinates](https://github.com/bitcraze/crazyflie-lib-python/blob/master/cflib/positioning/position_hl_commander.py)
- [Official Docs](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/positioning/position_hl_commander/)

### 4. Logging: Battery voltage, current position, attitude
- [Use logging to record flight data](https://github.com/bitcraze/crazyflie-lib-python/blob/master/examples/logging/basiclog.py)
- [Official Docs: External position reporting](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/crazyflie/extpos/)

### 5. Crazyflie Power Control
- [Official Docs: Shutdown / Restart Crazyflie](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/utils/power_switch/)

### 6. Remote CRTP Driver
- [Official Docs](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/api/cflib/crtp/crtpdriver/)

### 7. Open Protocol Analyzer
- [Wireshark Integration Guide](https://www.bitcraze.io/documentation/repository/crazyflie-lib-python/master/development/wireshark/)

### 8. Local Path to Crazyflie Library
```
C:\Users\PC\AppData\Local\Programs\Python\Python312\Lib\site-packages\cflib\crazyflie
```
### 9. Python to Excel
- [Using openpyxl for automated spreadsheet processing](https://hackmd.io/@howkii-studio/python_autoporcessing_xl)
