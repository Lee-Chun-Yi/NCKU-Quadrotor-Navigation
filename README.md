# NCKU-Quadrotor-Navigation

This repository contains a collection of research projects completed under the guidance of [**Professor Woei Leong Chan**](https://iaa.ncku.edu.tw/p/412-1104-29048.php?Lang=en).


## Indoor Navigation of Quadrotors Using a Motion Capture System

This study focuses on the development of an autonomous navigation and control system for quadrotors operating in indoor environments. The objective is to achieve **stable and controllable hovering and point-to-point flight** without human intervention. The control strategy is primarily implemented in MATLAB, employing a **dual-loop PID controller** that independently handles position and velocity errors to generate corresponding attitude control outputs (Roll, Pitch, Yaw, Throttle).

The current system has successfully implemented a dual-loop PID controller for position and velocity control. Building upon this foundation, the architecture will be progressively extended into a **full four-layer control framework** by incorporating additional attitude and attitude rate PID loops.

The overall system consists of several key modules: a Vicon motion capture system using five cameras operating at 100 Hz provides real-time 6DOF data of the quadrotor. This data is transmitted via UDP to **MATLAB/Simulink**.

Within the control logic, position errors are first processed by a **position PID controller** to compute the desired velocity, which is then passed to a **velocity PID controller** to produce the appropriate Roll, Pitch, and Throttle commands.

The resulting control commands are sent from MATLAB to a Python script, which then transmits them to the Crazyflie 2.1 quadrotor.

##  Folder Structure

* `Crazyflie 2.1/`
  Setup guides for Crazyflie operation.

* `Matlab/`
  **Simulink-based control systems** (2PID & 4PID), with tools for tuning, trajectory plotting, and performance analysis.

* `Python/`
  **Main flight control scripts** with direct RPYT or PWM output (2PID & 4PID respectively).

  
 ## 
 
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-07-11%20204155.png)


---

## Project Timeline – Crazyflie UAV Control Research

### 2025-04-15 — Python Control Code Implementation

* Added UDP reception for RPYT commands
* Implemented smooth landing procedures
* Added battery voltage logging
* Added emergency landing routines
* Enabled logging of both 6DoF Vicon data and RPYT command history
  Details in [`Python/`](./Python)

  
---

  
### **2025-05-20 — Our First GUI**

* Completed integration between Python control code, MATLAB, and Crazyflie
* Designed first GUI version for operator-friendly control
  ![](https://github.com/Lee-Chun-Yi/crazyflie-GUI-python/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-08-11%20005757.png)

Complete GUI information: [crazyflie-GUI-python](https://github.com/Lee-Chun-Yi/crazyflie-GUI-python/tree/main)


---


### 2025-06-17 — 2PID Single-Point Flight Test
> Flight Test Records: [Flight_Data](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/tree/main/experiment_results/flight_data)

#### Experiment Result


>  17 consecutive flights (Battery replacement was performed) were conducted toward a fixed target position. Among these, 15 flights were successfully completed, while 2 flights resulted in crashes. 

The control system was tested by performing 15 repeated flights toward a fixed target position. The quadrotor initialized from
**(x, y, z) = (35, 350, 40) mm**, with an initial heading angle **ψ = 45°**, and was commanded to fly to the destination **(500, 500, 800) mm**, with a yaw angle aligned to **0°**.


| **Axis / DOF**                    | **Overshoot (±σ)**     | **Steady-State Error (±σ)**     | **Rise Time (±σ)**     |
| --------------------------------- | ---------------------- | ------------------------------- | ---------------------- |
| **X-axis (Pitch control)**        | 82.75 ± 29.91 %        | 41.38 ± 30.46 mm                | —                      |
| **Y-axis (Roll control)**         | 29.66 ± 0.42 %         | 44.78 ± 21.00 mm                | —                      |
| **Z-axis (Throttle control)**     | 17.42 ± 6.06 %         | 13.75 ± 13.38 mm                | 2.52 ± 0.19 s          |
| **Rot Z (Yaw control)**           | 56.62 deg              | 2.78 ± 2.59 deg                 | —                      |

Table 1. Flight response metrics under dual-loop PID control (mean ± standard deviation over 15 flights)


To visualize the results, the figure below illustrates the **2D trajectory** of each flight attempt (solid lines), along with the **mean trajectory** (dashed blue line). The shaded region denotes the ±1σ envelope across trials.

> * `X vs Time`, `Y vs Time`, and `Z vs Time` represent spatial trajectory tracking.
> * `Rotation Z vs Time` shows the yaw angle correction behavior.

![Figure 2. Single-target trajectory](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-07-23%20114535.png)

---

### 2025-07-15 — Linux System Deployment


Migrated the full MATLAB–Python control system from Windows to **Linux**.
Vicon motion capture data reception remains on the Windows host, with **6DoF data streamed via UDP over a private LAN** to the Linux machine.

For detailed setup steps, refer to the `Vicon/`  `linux` directory.
