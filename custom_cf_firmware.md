# Crazyflie Custom Firmware Build & Flash Guide - Windows

This guide describes how to **build and flash a custom Crazyflie firmware** using **WSL (Ubuntu)** and **Docker Toolbelt** on Windows.

---

## 1. Install Required Tools

### 1.1 Download Docker Desktop

🔗 [Docker Desktop](https://www.docker.com/products/docker-desktop/?utm_source=chatgpt.com)

During installation, enable **WSL 2 Backend**.



### 1.2 Install WSL (Ubuntu)

Open **PowerShell** and run:

```powershell
wsl --install -d Ubuntu
```

```powershell
wsl -d Ubuntu
```

```powershell
cd ~
sudo apt update
```

```powershell
sudo apt install -y git
```



### 1.3 Enable WSL Integration in Docker

1. Open **Docker Desktop** → **Settings**
2. Go to **Resources → WSL Integration**
3. Enable checkbox for **Ubuntu**


### 1.4 Verify Docker in WSL

Re-open **PowerShell** and run:

```powershell
exit
```

```powershell
wsl -d Ubuntu
```

```powershell
docker --version   # confirm Docker is running
```

```powershell
docker run --rm -it bitcraze/toolbelt
```

If successful, you will enter the **Bitcraze Toolbelt** container.

---

## 2. Clone Custom Firmware Repository

Inside the Toolbelt container:

```bash
cd ~
git clone --recursive https://github.com/Lee-Chun-Yi/crazyflie-firmware-pwm.git   # replace with your GitHub link
```

```bash
cd crazyflie-firmware-pwm   # replace with your repo name
```

---

## 3. Build Firmware

Run the following commands inside the container:

```bash
tb make cf2_defconfig
tb make -j$(nproc)
```

The compiled firmware will be located at:

```
build/cf2.bin
```

---

## 4. Flash Firmware to Crazyflie

### 4.1 Enter DFU Mode

1. Disconnect USB and power off Crazyflie
2. Hold **power button** for \~3 seconds until the blue LED blinks
3. Connect Crazyflie via USB



### 4.2 Flash Using `dfu-util`

Inside Ubuntu (not Docker container):

```bash
sudo apt install dfu-util
dfu-util -a 0 -d 0483:df11 -D build/cf2.bin
```

Expected output:

```
Download    [=========================] 100%
File downloaded successfully
```

---

## 5. Verify Firmware

1. Disconnect and reboot Crazyflie
2. Connect with **cfclient**
3. Confirm that the custom firmware is running
