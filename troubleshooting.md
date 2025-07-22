# Troubleshooting Guide

## Table of Contents

1. [Wireless Communication Failure or Parameter Errors](#1-wireless-communication-failure-or-parameter-errors-reset-to-default)
2. [Crazyflie LED Indicators](#2-crazyflie-led-indicators)
3. [Firmware Recovery (M2 Blue LED Solid ON)](#3-firmware-recovery-m2-blue-led-solid-on)
4. [Crazyflie Not Responding to RPYT Commands](#4-crazyflie-not-responding-to-rpyt-commands)
5. [Control Frequency Considerations](#5-control-frequency-considerations)
6. [Crazyflie Cannot Fly Again After One Flight](#6-crazyflie-cannot-fly-again-after-one-flight)
7. [Crazyflie Unbalanced During Flight](#7-crazyflie-unbalanced-during-flight)

---

## 1. Wireless Communication Failure or Parameter Errors (Reset to Default)

When attempting to connect, check if the **green LED** on the Crazyflie lights up:

* If the green LED is on but the drone does not respond:

  * This means the connection was successful, but **initialization was not completed**.
  * Try **unplugging and replugging** the Crazyflie battery.

* If the green LED does **not** light up:

  1. Verify that the correct **radio URI** in the Python code is being used.
  2. Unplug and replug the **Crazyradio PA** dongle.
  3. Use **Zadig** to check and install the correct USB driver.
     More information: [Zadig USB Driver Setup – Bitcraze Documentation](https://www.bitcraze.io/documentation/repository/crazyradio-firmware/master/building/usbwindows/)

---

## 2. Crazyflie LED Indicators

| **Mode**                 | **LED(s)**              | **State**                                     | **Meaning**                                              |
| ------------------------ | ----------------------- | --------------------------------------------- | -------------------------------------------------------- |
| **Standby Mode**         | M2, M3: Blue<br>M1: Red | Blue LEDs solid ON<br>Red LED slowly blinking | Crazyflie is powered and idle, ready to receive commands |
| **Low Battery**          | M1: Red                 | Red LED solid ON                              | Battery voltage is low — charging required               |
| **Crazyradio Connected** | M4: Green               | Green LED solid ON                            | Crazyradio PA is successfully connected                  |
| **Firmware Corruption**  | M2: Blue                | Blue LED solid ON                             | Firmware is missing or corrupted — recovery needed       |

More information: [Bitcraze Documentation – CF2 Components](https://www.bitcraze.io/documentation/system/platform/cf2-components/)

---

## 3. Firmware Recovery (M2 Blue LED Solid ON)

If your Crazyflie shows a **solid blue LED on M2 after power-up**, this means the firmware is corrupted and the system **cannot boot or connect wirelessly**.

To recover the firmware, follow the **cold boot procedure** using the official `cfclient`.

### Cold Boot (Recovery Mode)

1. **Disconnect and power off** the Crazyflie.

2. Launch `cfclient` and go to:

   ```
   Crazyflie → Bootloader
   ```

3. In the bootloader dialog, select the **"Cold boot (recovery)"** tab.

4. **Press and hold** the power button on the Crazyflie.

   * After a few seconds, the blue LED (M2) will **blink slowly** → wireless bootloader stage.
   * Then it will **blink rapidly** → USB bootloader stage.
   * **Release the button immediately once the fast blink begins.**

5. Once the blue LED is blinking rapidly, the bootloader is active.

6. In the dialog, click **"Initiate bootloader cold boot."**

7. Select the firmware:

   * Either the **latest official release**, or
   * Manually browse to a `.bin` file from the [GitHub Releases](https://github.com/bitcraze/crazyflie-firmware/releases)

8. Click **"Program"**, wait until flashing completes.

9. Click **"Restart in firmware mode"** to reboot and exit recovery.

More information: [Bitcraze-Recovery firmware flashing](https://www.bitcraze.io/documentation/repository/crazyflie-clients-python/master/userguides/recovery-mode/)

---

## 4. Crazyflie Not Responding to RPYT Commands

If the Crazyflie does not respond when RPYT commands are being sent, ensure that a proper arming signal has been issued beforehand.

### Required Actions Before Sending Commands:

1. **For standard Crazyflie (brushed motors):**
   Ensure you send the following command once before sending any attitude/thrust control:

   ```python
   commander.send_setpoint(0, 0, 0, 0)
   ```

   This acts as a signal to activate the motor system.

2. **For Crazyflie Bolt (brushless motors):**
   Send an arming request to enable motor output:

   ```python
   scf.cf.platform.send_arming_request(True)
   ```

---

## 5. Control Frequency Considerations

Different layers of the PID control architecture require different recommended update rates to ensure stable flight and responsive control.

### Recommended Frequencies:

* **Two-layer PID architecture** (e.g. sending RPYT commands directly to Crazyflie):

  * Maintain a global control loop frequency of at least **100 Hz**
  * This ensures position and velocity control layers are sufficiently responsive

* **Four-layer PID architecture** (e.g. outputting raw PWM to each motor):

  * For attitude and attitude rate controllers, maintain at least **500 Hz** update frequency
  * Lower frequencies may cause instability due to the fast dynamics of inner-loop control

More information: [Commander Framework](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Crazyflie%202.1/platform_overview.md#2-commander-framework)

---

## 6. Crazyflie Cannot Fly Again After One Flight

After completing a flight, the Crazyflie may not respond to further commands unless a **power cycle** is performed.

### Solution:

Perform a manual power cycle by unplugging and reconnecting the battery, or use the following software-based method via the `PowerSwitch` API:

```python
def power_cycle_crazyflie():
    print("Power cycling Crazyflie...")
    ps = PowerSwitch(URI)
    ps.stm_power_down()
    time.sleep(3)
    ps.stm_power_up()
    time.sleep(3)
    print("Power cycle complete")
```

---

## 7. Crazyflie Unbalanced During Flight

### Case I. Crashes Immediately After Takeoff

If the Crazyflie crashes as soon as it lifts off and RPYT outputs appear normal, this likely indicates a **hardware fault**.

Step-by-step hardware inspection:

1. **Propellers** – Check for physical damage or cracks.
2. **Motors** – Listen for abnormal noise during rotation.
3. **Motor mounts** – Ensure they are securely fixed to the frame.

To assist with diagnosis, use the `cfclient` software:

* Open the `Console` tab
* Select **Propeller Test** to check individual motor response

### Case II. Lifts Off Briefly Then Crashes

* This may be caused by **excessive RPYT output** from the controller.
* If this occurs repeatedly, re-check all hardware components as listed above.

### Case III. Drifts After Takeoff

* This behavior is considered normal, often due to pre-existing roll/pitch angle.
