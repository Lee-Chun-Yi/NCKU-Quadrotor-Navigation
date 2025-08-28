# NCKU-Quadrotor-Navigation

This repository contains a collection of research projects completed under the guidance of [**Professor Woei Leong Chan**](https://iaa.ncku.edu.tw/p/412-1104-29048.php?Lang=en).

Contact us: 

* Lee-Chunyi: F44111106@gs.ncku.edu.tw

* Ken-Huang: e84116201@gs.ncku.edu.tw

## Indoor Navigation of Quadrotors Using a Motion Capture System

This study focuses on the development of an autonomous navigation and control system for quadrotors operating in indoor environments. The objective is to achieve **stable and controllable hovering and point-to-point flight** without human intervention. The control strategy is primarily implemented in MATLAB, employing a **dual-loop PID controller** that independently handles position and velocity errors to generate corresponding attitude control outputs (Roll, Pitch, Yaw, Throttle).

The current system has successfully implemented a dual-loop PID controller for position and velocity control. Building upon this foundation, the architecture will be progressively extended into a **full four-layer control framework** by incorporating additional attitude and attitude rate PID loops.

The overall system consists of several key modules: a Vicon motion capture system using five cameras operating at 100 Hz provides real-time 6DOF data of the quadrotor. This data is transmitted via UDP to **MATLAB/Simulink**.

Within the control logic, position errors are first processed by a **position PID controller** to compute the desired velocity, which is then passed to a **velocity PID controller** to produce the appropriate Roll, Pitch, and Throttle commands.

The resulting control commands are sent from MATLAB to a Python script, which then transmits them to the Crazyflie 2.1 quadrotor.

##  Folder Structure

* `Matlab/`
  **Simulink-based control systems** (2PID & 4PID), with tools for tuning, trajectory plotting, and performance analysis.

* `Python/`
  **Main flight control scripts** with direct RPYT or PWM output (2PID & 4PID respectively).

  
 ## 
 
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-07-11%20204155.png)


---

## Project Timeline – Crazyflie UAV Control Research

### 2025-04-15 — Python Control Code Complete

   > Details in [`Python/2PID`](./Python/2PID)

This python version is based on 2PID Matlab structure, and it's job is to transfer RPYT commands from Matlab to Crazyflie.
  
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-08-11%2002153.png)  

---

### **2025-05-13 — GUI Project Start**

 Began development of the GUI project

---


### **2025-05-20 — Our First GUI**

> Details in [crazyflie-GUI-python](https://github.com/Lee-Chun-Yi/crazyflie-GUI-python/tree/main). Future work and updates will be documented here.
> 
> Since the GUI wasn't stable enough, the experiment was still rely on the python code we had completed

* Start to work on GUI project
* Completed integration between Python control code, MATLAB, and Crazyflie
* Designed first GUI version for operator-friendly control

  ![](https://github.com/Lee-Chun-Yi/crazyflie-GUI-python/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-08-11%20005757.png)

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


### **2025-07-20 — 4PID & Crazyflie Bolt Project Start**

* Began development of the entire four PID loops (Unfinished: **Attitude** and **Attitude Rate**)
* Transitioned from Crazyflie 2.1 to **Crazyflie Bolt** due to PWM command limitations on CF 2.1
  
#### Short-Term Goals
1. Implement **single-packet 4PWM transmission** (requires firmware modification).  
2. Configure SD card logging for PWM data.  
3. Perform **linear calibration of each motor** while flying with RPYT inputs.  

#### Long-Term Goals
- Develop **automated RPYT flight with per-motor linear calibration**, where compensation coefficients are obtained automatically during flight.  

 Hardware Used:
> Crazyflie Bolt 1.1
> 
> HQProp T3X2 HQ3020 3-inch bi-blade propellers (racing toothpick style)
> 
> Happymodel EX1103 KV11000 brushless motors (3-hole mount, for Mobula8)
> 
> Tarot 6A BLHeli ESC (multirotor) TL300G5

![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-08-08%20041930.png)

---


### 2025-07-15 — Linux System Deployment

> Details in [`Vicon/linux`](./Vicon/linux.md)


Migrated the full MATLAB–Python control system from Windows to **Linux**.
Vicon motion capture data reception remains on the Windows host, with **6DoF data streamed via UDP over a private LAN** to the Linux machine.

---

### 2025-08-28 — Build and Flash Firmware

> 📄 Details in [`custom_cf_firmware.md`](./custom_cf_firmware.md)

Since the official Crazyflie firmware does not support sending all **m1–m4 PWM values in a single packet**,  
we designed and built a **custom firmware** (based on the official Crazyflie firmware) to achieve this functionality.

