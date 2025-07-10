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

If Crazyflie tilts or spins uncontrollably:

1. Ensure the battery is mounted at the center of the frame.
2. Check that the propellers can rotate freely without obstruction.
3. Check for imbalance and follow this tutorial:  
🔗 [Balancing Propellers – Bitcraze](https://www.bitcraze.io/documentation/tutorials/balancing-propellers/)

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
