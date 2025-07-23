# MATLAB Control System

This folder contains the full MATLAB/Simulink implementation of the control system used in our Crazyflie indoor navigation project. The control system is responsible for receiving real-time Vicon data, processing it, and generating stable RPYT (Roll, Pitch, Yaw, Throttle) commands to achieve hover and point-to-point flight.

---

## Folder Structure

* `controller/`
  
  Main Simulink control models for indoor flight with Crazyflie using a **dual-loop PID architecture**.

  * `2PID/controller.slx`
    
    The core Simulink model implementing:

    * Position PID (outer loop) and velocity PID (inner loop) control
    * Real-time RPYT command generation based on 6-DoF Vicon input

  * `2PID/user_guide.md`

    * Simulink model structure and module descriptions

    * UDP setup and input/output signal configuration

    * PID tuning procedure
      

* `data_analysis/`
  
  MATLAB scripts for evaluating and visualizing Crazyflie flight performance.

  * `csv/`
    
    Used for **flight analysis** based on logged csv data.
    Includes:

    * 2D and 3D trajectory visualization across multiple flights
    * Step response evaluation with `stepinfo` (rise time, overshoot, steady-state error)

  * `workspace/`
    
    Used for data analysis from Simulink's out structure.
    Includes:

    * logging data into csv files
    * Single and multi-axis plotting
    * Response evaluation
    
## Core Control Logic

### Position-Velocity Dual PID

* **Outer Loop**: Position PID generates desired velocity
* **Inner Loop**: Velocity PID outputs Roll, Pitch, and Throttle
* **Yaw Control**: PID tracking of yaw angle (rot\_z) with S-curve smoothing and activation delay


## Requirements

* MATLAB R2025a or later
* Simulink

