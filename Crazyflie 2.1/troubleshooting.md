# Troubleshooting Guide

## 1. Wireless Communication Failure or Parameter Errors (Reset to Default)

If Crazyflie is not responding correctly due to communication or parameter issues:

1. Unplug your Crazyradio PA.
2. Connect the Crazyflie to your PC via USB.
3. Open Command Prompt and run:

This will reset the EEPROM to factory defaults.

---

## 2. Crazyflie Unbalanced During Flight

If Crazyflie tilts or spins uncontrollably:

1. Ensure the battery is mounted at the center of the frame.
2. Check that the propellers can rotate freely without obstruction.
3. Check for imbalance and follow this tutorial:  
🔗 [Balancing Propellers – Bitcraze](https://www.bitcraze.io/documentation/tutorials/balancing-propellers/)

---

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
