# Connect Matlab and python

UDP is a lightweight communication protocol, suitable for applications with high real-time requirements but a higher tolerance for data loss (such as control command transmission). 

Configuration of the UDP Send module:

- **Remote IP address**: Set it to `127.0.0.1` (for local communication) .
- **Remote IP port**: Set it to `52001`.
- **Input format**: Based on the toarray function code, it is an array containing three `int16` integers, representing `desiredroll`, `desiredpitch`, and `desiredyaw`. These data will be transmitted to Python via Simulink using the UDP Send module.
- 
![image](https://github.com/Lee-Chun-Yi/NCKU-Quadrotor-Navigation/blob/main/image/123.png)
