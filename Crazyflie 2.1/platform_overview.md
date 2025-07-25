# 1. Hardware Platform
## Crazyflie 2.1
🔗 [Bitcraze - Crazyflie 2.1](https://www.bitcraze.io/products/crazyflie-2-1-plus/)
🔗 [Bitcraze - Crazyflie platform overview](https://www.bitcraze.io/documentation/system/platform/)

A lightweight, modular quadrotor platform designed for indoor research and development.
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-03-02%20030438.png)
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-07-23%20000637.png)


**Key Features:**
- Size: 92 mm motor-to-motor  
- Weight: ~27g (with battery)  
- Open-source hardware & firmware  
- Bluetooth & radio communication  
- Support for sensor decks and extensions

##  Motion Capture Marker Deck 
🔗 [Motion Capture Marker Deck – Bitcraze](https://www.bitcraze.io/products/motion-capture-marker-deck/)

An add-on deck for motion capture systems like **Vicon** or **OptiTrack**, allowing sub-millimeter tracking accuracy.

**Used in this project for:**
- Real-time 6DoF position feedback  
- Replacing the onboard IMU  
- Accurate external pose estimation

##  Crazyradio PA 
🔗 [Crazyradio PA – Bitcraze](https://www.bitcraze.io/products/crazyradio-pa/)

A long-range 2.4GHz USB radio dongle used for communicating with Crazyflie over the **CRTP protocol**.

**Used in this project for:**
- Reliable wireless communication between Crazyflie and control PC  
- Supports up to 1Mbit/s data rate  
- Essential for real-time command and feedback when not using Bluetooth
  
# 2. Commander Framework
🔗 [Commander setpoints documentation](https://www.bitcraze.io/documentation/repository/crazyflie-firmware/master/functional-areas/sensor-to-control/commanders_setpoints/)

🔗 [Controllers in the Crazyflie](https://www.bitcraze.io/documentation/repository/crazyflie-firmware/master/functional-areas/sensor-to-control/controllers/)

The **Commander** is the firmware layer responsible for translating external setpoints (e.g., velocity, position, attitude) into flight control actions.  
This project bypasses the Crazyflie onboard controller and sends **PWM signals directly**, forming a fully custom outer-loop system.
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-07-11%20130726.png)

# 3. Firmware and Software Resources
The following open-source Bitcraze repositories were used and referenced in this project:

-  [Crazyflie Firmware (C)](https://github.com/bitcraze/crazyflie-firmware/tree/master)  
  Handles onboard attitude estimation, motor mixing, and stabilization.  
  Referenced for motor PWM logic and commander structure.

-  [Crazyflie Clients (Python GUI)](https://github.com/bitcraze/crazyflie-clients-python)  
  Official PC GUI for controlling Crazyflie.  
  Used for early testing and packet format reference.

-  [Crazyflie Python Library](https://github.com/bitcraze/crazyflie-lib-python)  
  Python API to send setpoints, read logs, and access low-level communication.  
  Extensively used in this project to build real-time control loops and interface with Vicon.

# 4. How to Control Crazyflie

This section introduces three common ways to control the Crazyflie 2.1 quadrotor, ranging from manual operation to full programmatic control.

>  Note: The `python/` `reference` folder contains frequently used Python functions used in this project, including communication handlers, control pipelines, and safety utilities.

## 1. Using a Remote Controller

Crazyflie supports manual flight using various types of RC input:

* **Bluetooth gamepad** (e.g., PS4/PS5, Xbox Controller)
* **USB Joystick** connected to PC
* **Custom RC transmitters** via radio modules (e.g., Devo 7E)

These controllers map user input directly to thrust and attitude setpoints. Suitable for:

* Pilot training
* Emergency override
* Basic flight testing



## 2. Using Crazyflie Client (cfclient)

🔗 [Bitcraze – cfclient GitHub](https://github.com/bitcraze/crazyflie-clients-python/blob/master/docs/installation/install.md)

🔗 [Userguide– cfclient GitHub](https://github.com/bitcraze/crazyflie-clients-python/tree/master/docs/userguides)

To monitor logs or perform radio scan and configuration, you may install the official GUI: **Crazyflie Client (cfclient)**.

**Main Functions:**

* Manual control via keyboard/gamepad
* Real-time sensor data monitoring (IMU, battery, altitude)
* Parameter tuning and logging
* Flight mode selection

**Installation:**

Open ```CMD```

```bash
pip3 install cfclient
```
Enter ```cfclient``` to launch.



## 3. Using Python Scripts (cflib)

🔗 [Crazyflie Python Library](https://github.com/bitcraze/crazyflie-lib-python)

For advanced users and custom control, Python + cflib offers full programmatic access to the Crazyflie via CRTP protocol.

**Control Options:**

* Low-Level: Send raw RPYT commands (`send_setpoint()`)
* High-Level: Use `MotionCommander` or `PositionHlCommander` for movement commands like `takeoff()`, `go_to()`, `circle()`
* Logging: Read real-time battery, position, attitude data

**Advantages:**

* Full autonomy possible
* Integration with external systems (e.g., Vicon, MATLAB)
* Suitable for algorithm testing and research

**Quick Start Example:**

```python
from cflib.crazyflie import Crazyflie
cf = Crazyflie()
cf.commander.send_setpoint(roll, pitch, yaw_rate, thrust)
```




