# Simulink User Guide

This document provides a step-by-step guide on how to operate the control system for this project using MATLAB Simulink.
> The file `controller.slx` requires **MATLAB R2025a or newer** to function properly.

---

## Computer Setup

This section guides you through the environment setup required to run the Simulink control system for this project.

### 1. Check MATLAB and Simulink Version

This project has been tested with **MATLAB R2024a or later**. Verify your version using:

```matlab
ver
````

You must have the following toolboxes installed:

* Simulink
* Instrument Control Toolbox
* Control System Toolbox

You can check installed toolboxes with:

```matlab
ver
```

If required, install missing toolboxes via MATLAB Add-On Explorer or contact your administrator.

### 2. Launch Simulink Model

Open the main control model:

```matlab
open('main_control_model.slx')
```

This will load the full control architecture with UDP input/output, PID controllers, yaw filter, and simulation scopes.

---

## Model Structure

The Simulink model is organized into functional subsystems. Each block performs a dedicated task in the control pipeline.

1. **6DoF Data Input**

   * Receives data from a UDP port (default: 51000)
   * Parses Vicon or external pose data (x, y, z, roll, pitch, yaw)
   * Applies scaling and moving average filters

2. **Position Controller**

   * Three PID controllers for x, y, z axes
   * Converts position errors into velocity setpoints
   * Includes saturation blocks and manual setpoints for testing

3. **Velocity Controller**

   * Three PID controllers for x, y, z velocity
   * Generates output thrust commands
   * Output is normalized and routed to command mixer

4. **Yaw Rate Controller**

   * Computes yaw rate from yaw signal derivative
   * Uses a first-order low-pass filter
   * Includes a dedicated yaw PID controller

5. **Command Output**

   * Collects RPYT values: roll, pitch, yaw, throttle
   * Packs commands into binary format
   * Sends over UDP to target (default port: 8888)

6. **Data Output Scope**

   * Stores real-time values for debugging
   * Logs reference and measured signals for all axes

---

## Simulation Parameters

These parameters define runtime and control behavior. Configure them in Simulink or MATLAB base workspace.

### 1. Set UDP Ports

Check the configured ports in the UDP blocks:

```text
Input Port: 51000
Output Port: 8888
```

Adjust these values if needed to match your network configuration.

### 2. Position and Velocity Limits

Throttle, roll, pitch, yaw values are constrained as follows:

```matlab
roll, pitch ∈ [-5, 5] degrees
yaw ∈ unrestricted
throttle ∈ [40000, 60000]
```

These constraints are applied using Saturation blocks in the velocity controller subsystem.

### 3. Simulation Rate

Set the model simulation step size to match control frequency:

```matlab
Fixed-step size: 0.01 (corresponds to 100Hz)
```

To modify:

1. Open Simulation Settings
2. Set solver type to "Fixed-step"
3. Set step size to `0.01`

---

## Operation Guide

### 1. Start Simulation

Start Simulink simulation using the "Run" button or:

```matlab
sim('main_control_model')
```

The model will begin listening for incoming data and preparing control outputs.

### 2. Send UDP Data

Start the external system (e.g., Python script or Vicon server) to send position/orientation data over UDP.

Once input is received, the position and velocity controllers will activate and begin producing control signals.

### 3. Observe Behavior

Use Scopes to monitor:

* Position vs. Setpoint (x, y, z)
* Velocity feedback and command signals
* Yaw rate estimation
* Throttle command stability

### 4. Stop and Reset

To stop the simulation:

```matlab
set_param('main_control_model','SimulationCommand','stop')
```

Reset all scopes and blocks using:

```matlab
set_param('main_control_model','SimulationCommand','update')
```

---

## Troubleshooting

### No input data

* Verify the UDP sender is using the correct port (51000)
* Use a packet sniffer (e.g., Wireshark) to confirm data transmission

### No output commands

* Ensure input data triggers controllers
* Check saturation or zero gain in PID blocks

### Oscillations or overshoot

* Tune PID gains: try reducing Kp or increasing D gain
* Confirm sampling rate stability and no aliasing

---

## Model Customization

You can modify the following components to adapt the system to your own use case:

* **UDP decoder**: adjust unpacking logic if the payload format differs
* **PID gains**: use the PID Tuner App for automated tuning
* **Yaw filter**: update the time constant or order for more aggressive smoothing
* **Output format**: change the command packer to match your actuator’s input type

---

## Additional Resources

* Simulink PID Tuner:
  [https://www.mathworks.com/help/slcontrol/ug/pid-tuning-basics.html](https://www.mathworks.com/help/slcontrol/ug/pid-tuning-basics.html)

* Simulink UDP Communication:
  [https://www.mathworks.com/help/instrument/udp.html](https://www.mathworks.com/help/instrument/udp.html)

* MATLAB Copilot (code assistant):
  [https://www.mathworks.com/products/matlab-copilot.html](https://www.mathworks.com/products/matlab-copilot.html)

---

## File Reference

* `main_control_model.slx`: Primary Simulink file
* `udp_decode.m`: Optional decoding logic for debugging
* `README.md`: Project summary and link to this guide
