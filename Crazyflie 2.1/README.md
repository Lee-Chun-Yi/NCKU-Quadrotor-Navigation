# Crazyflie 2.1

This folder provides hardware reference and usage notes for the **Crazyflie 2.1** quadrotor used in our indoor navigation project.

---

## Overview

[Crazyflie 2.1 – Bitcraze](https://www.bitcraze.io/products/crazyflie-2-1-plus/)

Crazyflie 2.1 is a lightweight, modular, and open-source nano quadcopter platform designed for indoor research and educational use.

**Key Specs:**

* Size: 92 mm motor-to-motor
* Weight: \~27g with battery
* Interfaces: Bluetooth, 2.4GHz radio (via Crazyradio PA)
* Fully open-source hardware & firmware



## Used in this project for:

* Receiving low-level PWM commands from an external PC
* Executing attitude and rate control onboard
* Interfacing with the Vicon-based external navigation system



## Add-on Decks

* **Motion Capture Marker Deck**
  Enables precise position and orientation tracking via Vicon system

* **Crazyradio PA**
  Used for high-speed wireless control via CRTP protocol



## Notes

* Internal IMU is unused in this project (Vicon replaces it)
* Motor PWM values are set via Python over CRTP
* Nominal takeoff thrust: 40000 (approx. 33g lift)


