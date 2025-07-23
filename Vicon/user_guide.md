## Vicon Quick Start Guide 

This section provides a basic introduction to setting up object tracking and UDP streaming with the Vicon system.

---

###  1. Creating an Object in Vicon Tracker 

To define an object for tracking:

1. If you are streaming live data, first **pause streaming** by clicking **Pause** under the **Objects** tab in the **Resources** pane.
2. **Hold the `Alt` key** and use **left-click + drag** to select the markers that define the object.
3. Enter a name in the **Create Object** box and click **Create**.

More information: [Creating an object – Vicon Help](https://help.vicon.com/space/Tracker310/13926929/Creating+an+object) 

---

###  2. Configure UDP Streaming 

Before launching your MATLAB/Simulink or Python script, ensure UDP settings are correctly configured:

1. Go to the **Properties pane** for the **Local Vicon System**.
2. Verify the following settings:

   * **Destination IP**: `127.0.0.1` (or your machine IP)
   * **Destination Port**: `51001`
   * **Enable the correct object(s)** to be tracked.
   * **Set the streaming rate**, typically **100 Hz** for real-time control.

 More information: [Properties pane for Local Vicon System – Vicon Help](https://help.vicon.com/space/Tracker310/13926747/Properties+pane+for+Local+Vicon+System) 


