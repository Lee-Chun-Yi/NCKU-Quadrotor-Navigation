# Quadrotor Control System with Real-Time Feedback

# Connect Matlab and Python

UDP is a lightweight communication protocol, suitable for applications with high real-time requirements but a higher tolerance for data loss (such as control command transmission). 

## 1.Matlab

Configuration of the UDP Send module:

- **Remote IP address**: Set it to `127.0.0.1` (for local communication) .
- **Remote IP port**: Set it to `52001`.
- **Input format**: Based on the toarray function code, it is an array containing three `int16` integers, representing `desiredroll`, `desiredpitch`, `desiredyaw`, and `desiredThrottle`. These data will be transmitted to Python via Simulink using the UDP Send module. 
![image](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-01-23%20021659.png)

## 2.Python

The UDP Recieve code:
 ```
 import socket
import struct

udp_ip = "127.0.0.1"  
udp_port = 52001       

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((udp_ip, udp_port))  

print(f"Listening on {udp_ip}:{udp_port}...")

while True:
        # receive data
        data, addr = sock.recvfrom(1024)

        if len(data) != 8:
            print(f"Unexpected data length: {len(data)} bytes. Skipping...")
            continue

        # Decode data: 4 int16 values (little-endian format)
        desired_roll, desired_pitch, desired_yaw, desired_throttle = struct.unpack('<hhhh', data)

        # Print the decoded values
        print(f"Decoded values - Roll: {desired_roll}, Pitch: {desired_pitch}, Yaw: {desired_yaw}, Throttle: {desired_throttle}")

```
