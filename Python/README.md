# Python Module

This folder contains the Python-side implementation for real-time communication with Crazyflie 2.1. It receives control commands from MATLAB and sends them to the drone via Crazyradio PA using the `cflib` library.

---

## Folder Structure

* `2PID/`
  Contains the control system for the **two-layer PID architecture**, which sends RPYT commands directly to the Crazyflie.

  * [`controller.py`](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/2PID/controller.py)
    Main control script responsible for:

    * Receiving RPYT commands from MATLAB over UDP 
    * Receiving 6DoF Vicon motion data 
    * Sending commands to Crazyflie at **100 Hz** using `cf.commander.send_setpoint()`

  * [`user_guide.md`](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/2PID/user_guide.md)
    Step-by-step documentation for setup and operation. Includes:

    * System usage guide
    * Function summaries and customization tips
    * Sample log output and data flow explanations

* `4PID/`
  Contains the control system for the **four-layer PID architecture**, which directly controls Crazyflie motors via PWM.

  * `controller.py` 

    * Attitude and attitude-rate PID implemented in Simulink or Python
    * Direct PWM output via `motorPowerSet()` or equivalent
    * Vicon-based position sensing without IMU
    * Support for control loop frequencies up to **500 Hz**

  * `user_guide.md`

    * Low-level control system usage
    * PWM output and safety routines
    * Logging and filtering strategies

* [`reference.md`](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/reference.md)
  Reference document for **Crazyflie-related Python APIs** and tools. Covers:

  * Usage of `MotionCommander`, `PositionHlCommander`
  * Logging and telemetry handling
  * Power management and direct PWM / low-level setpoint strategies


## Dependencies

To ensure the Python control system runs properly, the following software components must be installed:

* **Python ≥ 3.10**
  Recommended version: 3.11–3.12, 3.13 may be too new to ensure compatibility with `cflib`.

  > Ensure Python is properly added to your system PATH and that `pip` is available.

* **Zadig (Windows only)**
  Required to install USB drivers for the Crazyradio PA dongle.

  > Used to assign the correct WinUSB driver if the dongle is not recognized.
  > Download: [https://zadig.akeo.ie/](https://zadig.akeo.ie/)

* **MATLAB**
  Used as the upstream controller that sends commands and receives motion capture data.

  > Requires proper UDP communication setup and signal timing coordination.
