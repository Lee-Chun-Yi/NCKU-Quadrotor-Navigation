# Troubleshooting Guide

## 1. Wireless Communication Failure or Parameter Errors (Reset to Default)

When attempting to connect, check if the **green LED** on the Crazyflie lights up:

- ✅ If the green LED is on but the drone does not respond:
  - This means the connection was successful, but **initialization was not completed**.
  - Try **unplugging and replugging** the Crazyflie battery.

- ❌ If the green LED does **not** light up:
  1. Verify that the correct **radio URI** in the Python code is being used.
  2. Unplug and replug the **Crazyradio PA** dongle.
  3. Use **Zadig** to check and install the correct USB driver:  
     🔗 [Zadig USB Driver Setup – Bitcraze Documentation](https://www.bitcraze.io/documentation/repository/crazyradio-firmware/master/building/usbwindows/)



## 2. Crazyflie Unbalanced During Flight

### 1. If the Crazyflie crashes immediately after takeoff while RPYT outputs appear normal, this indicates a **hardware fault**.
  🔍 Step-by-step hardware inspection:

  1. **Propellers** – Check for physical damage or cracks.
  2. **Motors** – Listen for abnormal noise during rotation.
  3. **Motor mounts** – Ensure they are securely fixed to the frame.

  To assist with diagnosis, use the `cfclient` software:
  Open `Console`
  Select **Propeller Test** to check individual motor response

### 2. If the Crazyflie lifts off briefly and then crashes:

- It may be caused by **excessive attitude output (RPYT)** from the controller.
- If this happens **repeatedly**, it's recommended to re-check the **hardware integrity** as above.



### 3. If the Crazyflie **drifts after takeoff**, this is **considered normal** and expected due to external disturbances and sensor limitations.


## 3. Forcibly Closing a UDP Port (e.g., 52001 Occupied)

If a UDP port remains occupied due to a crashed process:

1. Open Command Prompt.
2. Run the following to find the process ID (PID) using port 52001:
```
netstat -ano | findstr :52001
```
3. Use the PID value from the output (e.g., 52112) to terminate the process:
```
taskkill /PID  52112 /F
```
