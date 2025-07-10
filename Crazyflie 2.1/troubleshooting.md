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

### Case I. Crashes Immediately After Takeoff

If the Crazyflie crashes as soon as it lifts off and RPYT outputs appear normal, this likely indicates a **hardware fault**.

🔍 **Step-by-step hardware inspection:**

1. **Propellers** – Check for physical damage or cracks.  
2. **Motors** – Listen for abnormal noise during rotation.  
3. **Motor mounts** – Ensure they are securely fixed to the frame.  

To assist with diagnosis, use the `cfclient` software:

- Open the `Console` tab  
- Select **Propeller Test** to check individual motor response  


### Case II. Lifts Off Briefly Then Crashes

- This may be caused by **excessive RPYT output** from the controller.
- If this occurs **repeatedly**, it is recommended to re-check all hardware components as listed above.

### Case III. Drifts After Takeoff

- This behavior is **considered normal**, often due to external disturbances or lack of position correction (e.g., no external tracking).
