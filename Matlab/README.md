# MATLAB Control System

This folder contains the full MATLAB/Simulink implementation of the control system used in our Crazyflie indoor navigation project. The control system is responsible for receiving real-time Vicon data, processing it, and generating stable RPYT (Roll, Pitch, Yaw, Throttle) commands to achieve hover and point-to-point flight.

---

## Folder Structure

* `controller/`
  Dual-loop PID controllers for position and velocity control.

* `utilities/`
  Helper functions for velocity smoothing, attitude limiting, and thrust mapping.

* `vicon/`
  UDP decoding functions for 6DoF motion capture data.

* `analysis/`
  Post-processing tools for flight data plotting and error analysis.

* `model/`
  Simulink system model and subsystem references.

---

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

---

## Execution

* Simulink runs at 100Hz (Fixed step = 0.01s)
* Output control signals are sent via UDP to Python at each timestep
* Uses user-defined functions to package \[Roll, Pitch, Yaw, Throttle] into float32 arrays

---

## Example Functions

### `takeoff_smooth.m`

Generates a smooth target height trajectory using:

```
z_target(t) = z_max * (1 - cos(pi*t/T)) / 2
```

### `accel_to_pitch_deg.m`

Limits pitch angle based on acceleration input, with defined saturation bounds.

### `scale_error.m`

Converts velocity error to normalized thrust using:

```
Throttle = (error + g) * (m / Fmax * 20000)
```

---

## Requirements

* MATLAB R2023a or later
* Simulink
* Instrument Control Toolbox (for UDP communication)

---

## License

MIT License.
