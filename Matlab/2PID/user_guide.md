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

More information: 🔗 [Control Frequency Considerations](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/troubleshooting.md#5-control-frequency-considerations)

---

## 4. Flight Operation

1. **Set the target** `(x, y, z, yaw)` in MATLAB workspace
2. **Start the Python control script**

   * The model will begin listening for UDP input and preparing control outputs
3. **Run the Simulink simulation**

   * The system will automatically initiate takeoff and fly toward the target

---

## 5. PID Tuning

This Simulink control system adopts a **two-layer PID architecture**, consisting of an inner velocity control loop and an outer position control loop.

> Always tune the **inner loop first**, followed by the **outer loop**.



### 5.1 Velocity PID (Inner Loop)

The velocity controller is responsible for making the actual velocity follow the desired velocity setpoint.
Ideally, the output velocity waveform should closely track the reference.
It is recommended to plot the following during tuning:

* `desired_velocity`
* `measured_velocity`
* `position`

> Set the outer (position) PID to **P=1, I=0, D=0** during tuning of the inner loop.

#### Step 1: Tune P (Proportional only)

```matlab
Kp > 0, Ki = 0, Kd = 0
```

* If **Kp is too small** → sluggish response
* If **Kp is too large** → may cause overshoot or oscillation

Gradually increase `Kp` until the response is fast but not overly oscillatory.

#### Step 2: Add D (Derivative)

```matlab
Kd > 0
```

* Derivative control adds damping to suppress oscillation.
* Start with a small `Kd` and increase slowly.
* Useful in dynamic systems like UAVs to reduce overshoot and stabilize approach.

#### Step 3: Add I (Integral)

```matlab
Ki > 0
```

* Integral control helps eliminate steady-state error.
* Be cautious: too much `Ki` may cause **integrator windup**.

Mitigation strategies:

* Enable **Anti-Windup** in Simulink PID block
* Set upper and lower limits using **Saturation** blocks


### 5.2 Position PID (Outer Loop)

The position controller outputs the **desired velocity** to help the drone reach its target point.

> Objectives:
>
> * Fast convergence to the desired position
> * Minimal overshoot and oscillation
> * Zero steady-state error

#### Step 1: Tune P (Proportional only)

```matlab
Kp > 0, Ki = 0, Kd = 0
```

* Increase `Kp` gradually.
* If position error reduces quickly → working.
* If excessive overshoot or oscillation → reduce `Kp`.

> At this stage, the controller behaves like a spring: responsive but with residual error.

#### Step 2: Add D (Derivative)

```matlab
Kd > 0
```

* Helps suppress overshoot and oscillation
* Adds damping to stabilize the trajectory near the target
* Smoother convergence and less aggressive corrections

#### Step 3: Add I (Integral)

```matlab
Ki > 0
```

* Eliminates long-term position drift or offset
* Be careful with high `Ki` as it may cause instability or oscillation

Recommended safeguards:

* Add **Saturation limits** to position output
* Enable **Anti-Windup** in Simulink PID blocks


### 5.3 Performance Example

![](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-07-23%20152124.png)

This scope output shows tuning results for the **X-axis controller**:

| Subplot          | Signal Name          | Description                            |
| ---------------- | -------------------- | -------------------------------------- |
| **Top Left**     | `x_velocity_current` | Actual velocity measured by Vicon      |
| **Bottom Left**  | `x_velocity_desired` | Desired velocity from position PID     |
| **Top Right**    | `pitch_out`          | Roll command output by velocity PID    |
| **Bottom Right** | `trans_x`            | Actual position response in the X axis |


