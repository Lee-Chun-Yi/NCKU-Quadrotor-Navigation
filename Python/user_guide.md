# Python User Guide

This document provides a step-by-step guide on how to prepare and operate the Python control system for Crazyflie 2.1.

---

##  Pre-flight Setup

>  🔗 Click the title to jump to the corresponding code in `control_system.py`.

1. [**Set the URI**](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/control_system.py#L19)

   ```python
   URI = uri_helper.uri_from_env(default="radio://0/80/2M")
   ```

2. [**Confirm port configuration and limits** ](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/control_system.py#L44)

   * Port `8888`: receives RPYT commands from MATLAB
   * Port `8889`: receives 6DoF Vicon data
   * RPYT value limits:

     ```python
     roll, pitch ∈ [-5, 5] degrees
     yaw ∈ unrestricted (depends on control)
     throttle ∈ [40000, 60000]
     ```

3. [**Set the send rate** ](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/control_system.py#L133)

   * Control loop operates at `100Hz` (every 0.01s)

4. [**Set throttle safety condition** ](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/control_system.py#L145)

   ```python
   with lock:
       roll, pitch, yaw, throttle = control_data.values()
       if throttle <= 38000:
           roll, pitch, yaw = 0.0, 0.0, 0.0
   ```

5. [**Set flight duration** ](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/control_system.py#L225)

   * Configure duration in seconds:

     ```python
     send_attitude_commands(scf, duration=120)
     ```



##  Flight Operation

1. **Start the Python script**

   * The system will begin execution and attempt to connect to Crazyflie.
   * Motors will remain off until MATLAB begins sending commands.

2. **Run MATLAB**

   * MATLAB sends the `start` signal and RPYT commands.
   * Motors will activate and flight will begin.

3. **Autonomous flight**

   * Once running, MATLAB fully controls the drone.
   * The drone will land automatically after the specified duration.

4. **Emergency stop**

   * Press `Ctrl+C`:

     * If **pressed once at high altitude**: begins a smooth landing.
     * If **pressed twice or at low altitude**: immediately cuts off motors.
     * This behavior is intentional to avoid mid-air drop during low-altitude startup.


For full source code and all function references, see:

* `control_system.py`
* `control_reference.md`

All safety mechanisms, logging, and communication routines are integrated in a modular structure for easy customization.
