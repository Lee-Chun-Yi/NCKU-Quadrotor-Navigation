# Connect Matlab and python

UDP is a lightweight communication protocol, suitable for applications with high real-time requirements but a higher tolerance for data loss (such as control command transmission). 

## Matlab

Configuration of the UDP Send module:

- **Remote IP address**: Set it to `127.0.0.1` (for local communication) .
- **Remote IP port**: Set it to `52001`.
- **Input format**: Based on the toarray function code, it is an array containing three `int16` integers, representing `desiredroll`, `desiredpitch`, and `desiredyaw`. These data will be transmitted to Python via Simulink using the UDP Send module. 
![image](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/123.png)

## Python

The UDP Recieve code:
 ```
  import socket
  import struct
  udp_ip = "127.0.0.1"  
  udp_port = 52001

  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
  sock.bind((udp_ip, udp_port))

  while True:
        # Receive data with a maximum buffer size of 1024 bytes
        data, addr = sock.recvfrom(1024)

        # Verify if the data length matches the expected size (6 bytes)
        if len(data) != 6:
            print(f"Unexpected data length: {len(data)} bytes. Skipping...")
            continue

         # Decode the data: 3 int16 values (little-endian format)
        desired_roll, desired_pitch, desired_yaw = struct.unpack('<hhh', data)
```
