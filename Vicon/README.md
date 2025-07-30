# Vicon User Guide

This folder contains setup instructions and reference materials for streaming **motion capture data from Vicon Tracker (Windows)** to a **Linux-based Simulink system** via UDP.
Because the **Vicon Tracker GUI is not supported on Linux**, all communication is established over a custom Ethernet setup using static IP and the built-in UDP Object Stream.

---

## Folder Structure

* `Create_Object.md`
  Step-by-step instructions for creating and naming rigid bodies (Trackable Objects) in Vicon Tracker. Covers:

  * Creating labeled markers
  * Defining coordinate frames
  * Exporting VSK files for reuse

* `recv_vicon_linux.md`
  Full bidirectional configuration guide for enabling **UDP streaming from Windows to Linux**. Includes:

  * Static IP configuration on both systems
  * Vicon UDP port settings
  * Linux-side network setup and UDP debugging
  * Simulink UDP Receive block configuration


