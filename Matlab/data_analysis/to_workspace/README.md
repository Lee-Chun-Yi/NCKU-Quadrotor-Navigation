

# MATLAB Module – Data Analysis

This folder contains MATLAB scripts for visualizing and evaluating Crazyflie flight data. It supports both real-time workspace logging and post-experiment CSV data analysis.

---

## Folder Structure

* `csv/`
  Contains analysis scripts for **CSV-based post-flight data**, typically exported from logging sessions.

  > ⚠️ To use this folder, please ensure the required **`.csv` files exist**.
  
  > If not, you may generate them using `log_csv.m` under the `to_workspace/` directory.

  * `2D_plot.m`
    Plots **multi-axis 2D trajectories** from CSV files (e.g., X/Y/Z/RotZ). Includes:

    * Mean ±1σ visualization
    * Unified time interpolation across flights
    * Unified legend and subplot formatting

  * `3D_plot.m`
    Generates **3D flight trajectory** visualization. Includes:

    * Velocity-based color gradient
    * Start/end point markers and velocity quiver
    * True global coordinate preservation

  * `dynamic_response.m`
    Performs **step response analysis** using `stepinfo` metrics:

    * Rise time, settling time, overshoot
    * Supports X, Y, Z, and Rot Z axes
    * Target tracking error estimation

* `to_workspace/`
  Contains analysis scripts for **live-flight evaluation using MATLAB workspace signals**.

  * `2D_plot.m`
    Plots all axes (X/Y/Z/Rot Z) from `Simulink` or logging output in `out` structure.

  * `2D_plot_single_dir.m`
    Focused 2D plot for a **single direction** (`roll`, `pitch`, or `throttle`) with displacement correction.

  * `3D_plot.m`
    3D trajectory visualization using `out.trans_*` variables in real time.

  * `dynamic_response.m`
    Step response analysis from workspace signals (same output style as CSV version).

  * `log_csv.m`
    Converts workspace variables into `.csv` files for persistent storage or batch comparison. Supports:

    * Automatic filename generation
    * Column formatting (`Time`, `Value`)
    * Compatibility with the `csv/` module

---

## Usage Notes

* All scripts are organized for **modular reuse** – you may mix workspace and CSV analyses depending on your logging configuration.
* Script headers follow a consistent section structure:

  * `User Input`
  * `Data Processing`
  * `Plotting`
  * `Evaluation`
* Time filtering, interpolation, and visualization are all tuned for **Crazyflie-scale indoor UAV testing**.

> These scripts are designed to assist both control debugging and performance analysis in a repeatable, human-readable format.


