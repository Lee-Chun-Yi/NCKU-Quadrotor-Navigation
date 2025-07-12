# Python Module

This folder contains the Python-side implementation for real-time communication with Crazyflie 2.1. It receives control commands from MATLAB and sends them to the drone via Crazyradio PA using the `cflib` library.

---

## Folder Structure

* `control_system.py`

  Main control script, responsible for:

  * Receiving RPYT commands from MATLAB over UDP (port 8888)
  * Receiving 6DoF Vicon motion data (port 8889)
  * Sending control commands to Crazyflie at 100Hz via `cf.commander.send_setpoint()`
  * Handling startup signals, battery voltage monitoring, emergency landing, and cleanup

* `user_guide/`

  A dedicated folder providing usage instructions and documentation for the Python control system. It includes:

  * Step-by-step usage guide for running the system
  * Troubleshooting common connection and flight issues
  * Function explanations and customization tips
  * Data flow diagrams and log examples

* `control_reference.md`

  A reference document summarizing key Python functions and links to relevant **Crazyflie libraries** and control examples. It provides guidance on using:

  * MotionCommander / PositionHlCommander
  * Logging and real-time data retrieval
  * Power management and low-level setpoint control



## Features

* Dual UDP input pipeline for Vicon and MATLAB
* Timing control for precise 100Hz signal dispatch
* Safety modules: smooth landing, port cleanup, battery check
* Easily extendable for future path tracking, logging, and data saving



## Dependencies

* Python ≥ 3.7
* `cflib` (Crazyflie Python library)
* `socket`, `struct`, `threading`, `time`, `os`


