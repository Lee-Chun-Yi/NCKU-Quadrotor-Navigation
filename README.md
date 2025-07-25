# NCKU-Quadrotor-Navigation

The following repository encompasses completed research projects under the guidance of [**Professor Woei Leong Chan**](https://iaa.ncku.edu.tw/p/412-1104-29048.php?Lang=en).

## Indoor Navigation of Quadrotors Using a Motion Capture System

This study focuses on the development of an autonomous navigation and control system for quadrotors operating in indoor environments. The objective is to achieve **stable and controllable hovering and point-to-point flight** without human intervention. The control strategy is primarily implemented in MATLAB, employing a **dual-loop PID controller** that independently handles position and velocity errors to generate corresponding attitude control outputs (Roll, Pitch, Yaw, Throttle).

The current system has successfully implemented a dual-loop PID controller for position and velocity control. Building upon this foundation, the architecture will be progressively extended into a **full four-layer control framework** by incorporating additional attitude and attitude rate PID loops. This enhancement aims to enable high-frequency, low-latency closed-loop control, thereby improving flight stability and supporting more complex navigation tasks.


The overall system consists of several key modules: a Vicon motion capture system using five cameras operating at 100 Hz provides real-time 6DOF data of the quadrotor. This data is transmitted via UDP to **MATLAB/Simulink**.

Within the control logic, position errors are first processed by a **position PID controller** to compute the desired velocity, which is then passed to a **velocity PID controller** to produce the appropriate Roll, Pitch, and Throttle commands. Yaw is separately adjusted based on the yaw angle error.

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

Figure 1. Overview of the experimental setup

---

## Experiment Result

### 2PID: Single-Point Flight Trajectory

The control system was tested by performing 15 repeated flights toward a fixed target position. The quadrotor initialized from
**(x, y, z) = (35, 350, 40) mm**, with an initial heading angle **ψ = 45°**, and was commanded to fly to the destination **(500, 500, 800) mm**, with a yaw angle aligned to **0°**.

During each test, the Crazyflie’s flight position was logged in real time for 0–30 seconds. A sample trajectory and corresponding performance indicators were summarized and analyzed to quantify control effectiveness.

The table below reports the average **Overshoot**, **Steady-State Error**, and **Rise Time** for each axis, based on 15 flight trials.

> Notably, the Z-axis (altitude) exhibited the most stable behavior, with an average overshoot of only **17.42 ± 6.06%**, corresponding to about **0.139 m**.

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

Figure 2. Single-target trajectory

