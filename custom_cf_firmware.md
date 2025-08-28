

# Crazyflie Custom Firmware Flashing Guide

This guide describes the step-by-step procedure to **build and flash custom firmware** onto a Crazyflie 2.1 / Bolt platform.
The setup uses **WSL + Docker Toolbelt** as the build environment to ensure a consistent toolchain.

---

## 1. Windows Side (Environment Preparation)

### 1.1 Install WSL (Ubuntu)

Open **PowerShell** as Administrator and run:

```powershell
wsl --install -d Ubuntu
wsl -d Ubuntu
cd ~
sudo apt update
sudo apt install -y git
exit
```

> ⚠️ Restart may be required after WSL installation.



### 1.2 Install Docker Desktop

Download and install:

🔗 [Docker Desktop](https://www.docker.com/products/docker-desktop/?utm_source=chatgpt.com)

Enable **WSL Integration** for **Ubuntu** in Docker Desktop settings.



### 1.3 Verify Docker from WSL

Re-open Ubuntu (WSL):

```bash
wsl -d Ubuntu
docker --version
```

Expected output shows Docker version number.

---

## 2. Docker Toolbelt Setup

The Bitcraze team provides a **Docker Toolbelt image** containing all required build tools.

Run:

```bash
docker run --rm -it bitcraze/toolbelt
```

Inside this container you will have access to the Crazyflie firmware toolchain.

---

## 3. Build Crazyflie Firmware

### 3.1 Clone Repository

Inside the Docker container:

```bash
git clone https://github.com/bitcraze/crazyflie-firmware.git
cd crazyflie-firmware
```

### 3.2 Modify Firmware (Optional)

Edit source code to implement your custom features:

```bash
nano src/<your_file>.c
```

### 3.3 Compile

```bash
make
```

The generated binary will be located in:

```
build/cf2.bin
```

---

## 4. Flash Firmware

### 4.1 Put Crazyflie in DFU Mode

1. Power off Crazyflie.
2. Hold the **power button** for \~3 seconds until the blue LED blinks.
3. Connect via USB.



### 4.2 Install `dfu-util` (if not included)

Inside Ubuntu:

```bash
sudo apt install dfu-util
```



### 4.3 Flash Firmware

```bash
dfu-util -a 0 -d 0483:df11 -D build/cf2.bin
```

Expected output:

```
Download    [=========================] 100% ...
File downloaded successfully
```

---

## 5. Verify

1. Disconnect and power cycle Crazyflie.
2. Use **cfclient** to connect via Crazyradio.
3. Check that the custom firmware is running.

---

## 6. Troubleshooting

| Issue                             | Possible Fix                                       |
| --------------------------------- | -------------------------------------------------- |
| `dfu-util: No DFU capable USB`    | Re-enter DFU mode, check USB cable.                |
| Build errors (`make`)             | Run inside Toolbelt container, check dependencies. |
| Crazyflie not booting after flash | Flash with official prebuilt firmware, then retry. |


