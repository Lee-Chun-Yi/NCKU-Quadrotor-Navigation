# C++ User Guide

This document shows how to set up and run the **high-frequency 4PWM C++ runner** on Windows.

> Requires custom firmware: [crazyflie-firmware-pwm](https://github.com/Lee-Chun-Yi/crazyflie-firmware-pwm).

---

## 1. Install Environment

Install Git

```powershell
winget install --id Git.Git -e --source winget
```

Install CMake

```
winget install --id Kitware.CMake -e --source winget
```

Install Visual Studio 2022 (Community Edition)

```
winget install --id Microsoft.VisualStudio.2022.Community -e --source winget
```

Verify installation:

```powershell
git --version
cmake --version
```

---

## 2. Clone Repositories

```powershell
git clone https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation.git
cd NCKU-Quadrotor-Navigation
```

---

## 3. Build the C++ Runner

```powershell
cmake -S cpp -B cpp/build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build cpp/build --config Release -j
```

Executable path:
`cpp\build\Release\cf4pwm_runner_win.exe`


---

## 6. Flight Workflow

```text
1. Power on Crazyflie + Crazyradio PA
2. Run cf4pwm_runner_win.exe (see example above)
3. External program sends PWM packets
4. Motors spin and follow external control
5. Ctrl+C = Landing
```


