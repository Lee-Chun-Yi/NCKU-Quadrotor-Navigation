This guide describes the step-by-step configuration required to stream UDP motion data from a Windows-based Vicon Tracker host to a **Linux machine running MATLAB Simulink** via Ethernet, since the **Vicon Tracker GUI is not supported on Linux** platforms and thus UDP streaming is used as an alternative.

---

##  Windows Side (Vicon Host)

### 1. Set Static Ethernet IP

1. Go to: Control Panel → Network and Sharing Center → Change adapter settings
2. Find the **wired Ethernet interface connected to Linux**, right-click → Properties
3. Double-click **Internet Protocol Version 4 (TCP/IPv4)**
4. Set a static IP:

   * **IP Address**: `192.168.10.2`
   * **Subnet Mask**: `255.255.255.0`

---

### 2. Verify IP Configuration

Open `cmd` and run:

```cmd
ipconfig
```

Check for the interface used for Vicon streaming. You should see:

* IPv4 Address: `192.168.10.2`
* Subnet Mask: `255.255.255.0`

---

### 3. (Optional) Add Static Route

If needed, force Windows to route traffic to the Linux target:

```cmd
route -p add 192.168.10.3 mask 255.255.255.0 192.168.10.2 metric 1 if <InterfaceIndex>
```

Find your interface index with:

```cmd
route print
```

Or filter results:

```cmd
route print | find "192.168.10"
```

---

### 4. Verify Connectivity

```cmd
ping 192.168.10.3
```

You should receive successful replies from the Linux machine.

---

##  Vicon Tracker Configuration

In **Vicon Tracker**, navigate to **UDP Object Stream** settings:

* ✅ **Enable**: checked
* **IP Address**: `192.168.10.2`
* **Port**: `51001`
* **Data Block Size**: `1024`

> Ensure the Tracker machine’s IP matches the **Local IP** set earlier.

---

## Linux Side (MATLAB Simulink + UDP Receiver)

### 1. Set Static IP for Ethernet

Check your network interface name:

```bash
ip a
```

Assuming your Ethernet interface is `enp5s0`, run:

```bash
sudo ip addr flush dev enp5s0
sudo ip addr add 192.168.10.3/24 dev enp5s0
ip a  # Confirm the settings
```

---

### 2. Allow Incoming UDP Port (Optional)

If `ufw` (firewall) is enabled:

```bash
sudo ufw allow 51001/udp
```

---

### 3. Check UDP Traffic with tcpdump

Use `tcpdump` to monitor incoming Vicon data:

```bash
sudo tcpdump -i enp5s0 udp port 51001
```

Expected output:

```
IP 192.168.10.2.XXXX > 192.168.10.3.51001: UDP, length 1024
```

If no packets appear, double-check Windows firewall and routing settings.

---

### 4. Simulink: UDP Receive Block Settings

Configure the **UDP Receive** block in Simulink:

| Parameter            | Value                  |
| -------------------- | ---------------------- |
| Local address        | `0.0.0.0` (accept all) |
| Local port           | `51001`                |
| Remote address       | `192.168.10.2`         |
| Remote port          | `51001`                |
| Output latest data   | ✅ checked              |
| Data size            | `[1024, 1]`            |
| Source data type     | `uint8`                |
| Enable blocking mode | ✅ checked (optional)   |
| Timeout              | `5` (seconds)          |
| Block sample time    | `0.01` (adjustable)    |

> ⚠️ Make sure the Simulink model **starts only after** the Vicon UDP stream is active.


