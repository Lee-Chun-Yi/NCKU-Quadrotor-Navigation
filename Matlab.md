# Indoor Navigation of Quadrotors Using a Motion Capture System

# Vicon Data Receiver

The Vicon Data Receiver is a module designed to interface with the Vicon motion capture system, which provides precise 3D positional and orientation data of objects in real-time. This module acts as the entry point for the data stream from the Vicon system, decoding the incoming data from Quadrotor.

- **Remote IP port**: Set it to `51001`.
![image](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-01-23%20024135.png)
  
# Connect Matlab and Python

UDP is a lightweight communication protocol, suitable for applications with high real-time requirements but a higher tolerance for data loss (such as control command transmission). 

## 1.Matlab

Configuration of the UDP Send module:

- **Remote IP address**: Set it to `127.0.0.1` (for local communication) .
- **Remote IP port**: Set it to `52001`.
- **Input format**: Based on the toarray function code, it is an array containing three `int16` integers, representing `desiredroll`, `desiredpitch`, `desiredyaw`, and `desiredThrottle`. These data will be transmitted to Python via Simulink using the UDP Send module. 
![image](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/%E8%9E%A2%E5%B9%95%E6%93%B7%E5%8F%96%E7%95%AB%E9%9D%A2%202025-01-23%20021659.png)


