# Python User Guide

This document provides a step-by-step guide on how to operate the control system for this project.
> Please ensure that all required [dependencies](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/README.md#dependencies) are properly installed on your system before executing the control code. 
---

##  Computer Setup

This section guides you through the environment setup required to run the Python control system for this project.

### 1. Check Python Version

The control scripts are tested with **Python 3.11 ~ 3.12**. You can verify your version using:

```bash
python --version
```

If you are using `python3`, use:

```bash
python3 --version
```

We recommend using a virtual environment (e.g., `venv`) to manage your dependencies.

### 2. Install `cflib`

`cflib` is the official Python library from Bitcraze to interface with the Crazyflie platform.

Install it using pip:

```bash
pip install cflib
```

If using Python 3:

```bash
pip3 install cflib
```

For source installation or updates, see:
🔗 [Bitcraze – cflib GitHub](https://github.com/bitcraze/crazyflie-lib-python)

---

##  Pre-flight Setup

>   Click the title to jump to the corresponding code in `controller.py`.

1. [**Set the URI**](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/2PID/controller.py#L19)

   The Crazyflie uses a unique URI format for communication via the Crazyradio PA using the **CRTP (Crazyradio Real-Time Protocol)**. The URI defines:

   * **Medium**: Communication type (e.g., radio)
   * **Channel**: Communication channel (e.g., 80)
   * **Speed**: Communication speed (e.g., 2M)
   * **Address**: Target device address, using for swarm. (e.g., E7E7E7E7E7)

   Example URI:

   ```text
   radio://0/80/2M/E7E7E7E7E7
   ```

2. [**Confirm port configuration and limits** ](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/2PID/controller.py#L44)

   * Port `8888`: default for receiving RPYT commands from MATLAB
   * Port `8889`: default for receiving 6DoF Vicon data
   * RPYT value defaul limits:

     ```python
     roll, pitch ∈ [-5, 5] degrees
     yaw ∈ unrestricted (depends on control)
     throttle ∈ [40000, 60000]
     ```

3. [**Set the send rate** ](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/2PID/controller.py#L133)

   * Default control loop operates at `100Hz` (every 0.01s)

4. [**Set throttle safety condition** ](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/2PID/controller.py#L145)

   ```python
   with lock:
       roll, pitch, yaw, throttle = control_data.values()
       if throttle <= 38000:
           roll, pitch, yaw = 0.0, 0.0, 0.0
   ```

5. [**Set flight duration** ](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/Python/2PID/controller.py#L225)

   * Configure duration in seconds:

     ```python
     send_attitude_commands(scf, duration=120)
     ```

---

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

     *  At high altitude, pressed **once**: cut off motors for 0.1s, and begins a smooth landing.
     *  At low altitude, pressed **twice**: immediately cuts off motors.
     * This behavior is intentional to avoid mid-air drop during low-altitude startup.


For full source code and all function references, see:

* `controller.py`
* `reference.md`
