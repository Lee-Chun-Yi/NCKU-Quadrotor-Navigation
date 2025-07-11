# NCKU-Quadrotor-Navigation

The following repository encompasses both completed and ongoing research projects under the guidance of **Professor Woei-Leong Chen**.

## Indoor Navigation of Quadrotors Using a Motion Capture System

This study focuses on the development of an autonomous navigation and control system for quadrotors operating in indoor environments. The objective is to achieve **stable and controllable hovering and point-to-point flight** without human intervention. The control strategy is primarily implemented in MATLAB, employing a **dual-loop PID controller** that independently handles position and velocity errors to generate corresponding attitude control outputs (Roll, Pitch, Yaw, Throttle).

The overall system consists of several key modules: a Vicon motion capture system using five cameras operating at 100 Hz provides real-time 3D position and attitude data of the quadrotor. This data is transmitted via UDP to **MATLAB/Simulink**, where it undergoes preprocessing such as unit conversion and low-pass filtering.

Within the control logic, **position errors** are first processed by a position PID controller to compute the **desired velocity**, which is then passed to a velocity PID controller to produce the appropriate Roll, Pitch, and Throttle commands. Yaw is separately adjusted based on the yaw angle error.

The resulting control commands are sent from MATLAB to a Python script, which then transmits them to the Crazyflie 2.1 quadrotor via Crazyradio PA for execution.

##  Folder Structure

- `Crazyflie 2.1/`  
  Contains basic usage documentation for Crazyflie and a troubleshooting guide for hardware and connectivity issues.

- `Python/`  
  Includes the main flight control script and various utility scripts such as motor testing and communication handlers.

- `Matlab/`  
  Contains the core flight control system built in Simulink, as well as plotting and response analysis scripts (e.g., trajectory visualization, PID tuning).
  
 ## 
 
![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-07-11%20204155.png)
