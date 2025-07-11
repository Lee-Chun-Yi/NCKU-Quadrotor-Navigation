# MATLAB Control System

This folder contains the full MATLAB/Simulink implementation of the control system used in our Crazyflie indoor navigation project. The control system is responsible for receiving real-time Vicon data, processing it, and generating stable RPYT (Roll, Pitch, Yaw, Throttle) commands to achieve hover and point-to-point flight.

---

## Folder Structure

* `controller/`  
  Main Simulink models for autonomous flight control, including position and velocity PID loops.

* `data_analysis/`  
  MATLAB scripts for 2D/3D trajectory plotting (single-point and multi-point), as well as response analysis (e.g., rise time, overshoot, steady-state error).

## Core Control Logic

### Position-Velocity Dual PID

* **Outer Loop**: Position PID generates desired velocity
* **Inner Loop**: Velocity PID outputs Roll, Pitch, and Throttle
* **Yaw Control**: PID tracking of yaw angle (rot\_z) with S-curve smoothing and activation delay

### Features

* Smooth takeoff using cosine-based target height ramping
* Velocity-based attitude limiter to avoid over-tilt
* Low-pass filtering on noisy channels (rot\_z)
* Unit conversions from mm → m and radians → degrees


## Execution

* Simulink runs at 100Hz (Fixed step = 0.01s)
* Output control signals are sent via UDP to Python at each timestep
* Uses user-defined functions to package \[Roll, Pitch, Yaw, Throttle] into float32 arrays



## Requirements

* MATLAB R2023a or later
* Simulink
* Instrument Control Toolbox (for UDP communication)

