# cf4pwm Runner (Windows)

High-rate PWM runner for Crazyflie, sending exactly one CRTP packet per tick.

## Build

```powershell
cmake -S cpp -B cpp/build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build cpp/build --config Release -j
```

## Run (500 Hz on core 3, high priority)

```powershell
cpp\build\Release\cf4pwm_runner_win.exe --uri "radio://0/80/2M" --rate 500 --affinity 3 --prio high --spin-tail
```
