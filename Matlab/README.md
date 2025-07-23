# MATLAB Control System

This folder contains the full MATLAB/Simulink implementation of the control system used in our Crazyflie indoor navigation project. The control system is responsible for receiving real-time Vicon data, processing it, and generating stable RPYT (Roll, Pitch, Yaw, Throttle) commands to achieve hover and point-to-point flight.

---

## Folder Structure

* `controller/`

  Main Simulink control models for:

  * Position PID and velocity PID dual-loop architecture
  * RPYT (attitude) command generation

* `data_analysis/`
  MATLAB scripts for evaluating and visualizing Crazyflie flight performance.
  Divided into two submodules based on data source:

  * `csv/`
    Used for **post-flight analysis** based on logged `.csv` data.
    Includes:

    * 2D and 3D trajectory visualization across multiple flights
    * Step response evaluation with `stepinfo` (rise time, overshoot, steady-state error)

  * `workspace/`
    Used for **real-time analysis** from Simulink's `out` structure during experiments.
    Includes:

    * Single and multi-axis plotting
    * Response evaluation and CSV export for persistent logging
    
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

* MATLAB R2025a or later
* Simulink

