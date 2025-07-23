---

# Simulink User Guide

This document provides a step-by-step guide on how to operate the control system for this project using MATLAB Simulink.

> The file `controller.slx` requires **MATLAB R2025a or newer** to function properly.

> See the file `theory.md` for a **complete explanation** of the MATLAB-based control system theory.

---

## 1. Overview

This system implements a full-stack control architecture for a Crazyflie UAV, integrating:

* External Vicon motion capture data
* Position and velocity PID controllers
* Real-time RPYT command generation
* UDP-based command transmission

The system is modeled in Simulink and consists of five main modules:

1. **Vicon Data Input** (Gray)
2. **Position Controller** (Violet)
3. **Velocity Controller** (Blue)
4. **Yaw Rate Controller**
5. **RPYT Command Output** (Gray)

![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/Matlab_structure_page-0001.jpg)

---

## 2. Model Structure

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

   * Three PID controllers for velocity in x, y, z
   * Generates output thrust commands
   * Output is normalized and routed to the command mixer

4. **Yaw Rate Controller**

   * Computes yaw rate from yaw signal derivative
   * Uses a first-order low-pass filter
   * Includes a dedicated yaw PID controller

5. **Command Output**

   * Collects RPYT values: roll, pitch, yaw, throttle
   * Packs commands into binary format
   * Sends them via UDP to the target port (default: 8888)

6. **Data Output Scope**

   * Logs real-time signals for debugging
   * Records reference and measured values for all axes

---

## 3. Pre-Flight Setup

These parameters define runtime behavior and must be configured in Simulink or the MATLAB base workspace.

### 3.1 Set UDP Ports

Check and configure the following UDP ports:

* **Vicon Data Input**

  ```
  Local Port: 51001  
  Remote Port: 9090
  ```

  > Adjust as needed to match your Vicon tracker system settings.

* **RPYT Command Output**

  ```
  Destination IP: 127.0.0.1  
  Port: 8888
  ```

  > Match these with the corresponding settings in your Python control script.

### 3.2 Position and Velocity Limits

Throttle, roll, pitch, and yaw values are constrained as follows:

```matlab
roll, pitch ∈ [-5, 5] degrees
yaw ∈ unrestricted
throttle ∈ [0, 20000]
```

> For indoor flights using Crazyflie 2.1, the roll and pitch are safety-limited to ±5°.
> A throttle offset of 40000 is added in the Python layer and should not be re-applied here.

### 3.3 Simulation Rate

Set the fixed simulation step size to match the intended control frequency:

```matlab
Fixed-step size: 0.01   % corresponds to 100 Hz
```

To change:

1. Open **Simulation Settings**
2. Set solver type to **Fixed-step**
3. Set step size to `0.01`

More details: [Control Frequency Considerations](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/troubleshooting.md#5-control-frequency-considerations)

---

## 4. Flight Operation

1. **Set the target** `(x, y, z, yaw)` in MATLAB workspace
2. **Start the Python control script**

   * The model will begin listening for UDP input and preparing control outputs
3. **Run the Simulink simulation**

   * The system will automatically initiate takeoff and fly toward the target

---

## 5. Troubleshooting

### No Input Data

* Confirm the sender (Vicon system or simulator) is broadcasting to the correct port (`51000`)
* Use packet sniffers like **Wireshark** to verify UDP packet reception

### No Output Commands

* Ensure the input signal is arriving and triggering the control loop
* Check for saturation blocks or zeroed PID gains blocking outputs

### Oscillations or Overshoot

* Re-tune the PID gains:

  * Try lowering **Kp**
  * Increase **Kd** to add damping
* Confirm that the simulation step size is correct and consistent

---

## 6. Model Customization

You may customize the following components to adapt the model to your use case:

* **UDP decoder**: Modify unpacking logic if your UDP packet structure differs
* **PID gains**: Use the **Simulink PID Tuner App** for automatic tuning
* **Yaw filter**: Adjust the low-pass filter time constant for better smoothing
* **Output format**: Modify the command packing logic to match your actuator or embedded system format

---

## 7. Additional Resources

* [PID Tuning Basics (Simulink)](https://www.mathworks.com/help/slcontrol/ug/pid-tuning-basics.html)
* [UDP Communication in Simulink](https://www.mathworks.com/help/instrument/udp.html)
* [MATLAB Copilot (AI Assistant)](https://www.mathworks.com/products/matlab-copilot.html)

---

## 8. File Reference

* `main_control_model.slx` — Main Simulink file for system operation
* `udp_decode.m` — Optional script for interpreting incoming UDP payload
* `README.md` — Project summary and link to this user guide

---
