# 🚀 Assignment 3: Networked Drone Simulator

This repository contains the implementation for the **Advanced Robotics Programming** course (Assignment 3). It extends the previous interactive simulator with a **Distributed Network Architecture**, allowing two distinct instances of the simulator to communicate over TCP/IP.

## 🌐 New in Assignment 3: Distributed Operation

The major update in this version is the introduction of a **Network Bridge** that allows two simulators to exchange data in real-time.

### 1. TCP Socket Communication (`network.c`)
* **Architecture:** Implemented a **Client-Server** model using standard Berkeley Sockets (`AF_INET`, `SOCK_STREAM`).
* **Dual-Mode Operation:** The network process can start as a **Server** (listens for incoming connections) or a **Client** (connects to a specific IP).
* **Reliability:** Uses a **Stop-and-Wait** synchronization protocol. The simulator pauses its physics cycle until it receives an acknowledgement (ACK) from the peer, ensuring that both simulations remain perfectly synchronized in time.

### 2. Remote Entity Mapping
* **Concept:** The simulator treats the "Remote Drone" as a "Local Obstacle".
* **Implementation:** The network process receives coordinates from the remote peer, packages them into an `Obstacle` struct, and injects them into the local **Server** process via an IPC pipe.
* **Result:** The local player sees the remote player as a moving obstacle to be avoided.

### 3. Coordinate Standardization
* **Problem:** Different terminal window sizes could lead to desynchronized positions.
* **Fix:** Implemented `to_virtual_y()` in `network.c` to normalize coordinates (converting from ncurses Top-Left origin to a standard Cartesian Bottom-Left origin) before transmission.

---

## 🛠️ Assignment 2 Features (Retained & Refined)

### 🛡️ Watchdog & Reliability
* **Watchdog Process:** A dedicated process (`watchdog.c`) monitors the heartbeat of all system components.
* **Mechanism:** It checks the `st_mtime` (last modification time) of log files. If a process fails to write to its log for >2 seconds, the Watchdog triggers a visual alert.
* **Safe Logging:** All logs use `flock()` (File Locking) to ensure thread-safe writing, preventing data corruption when multiple processes log simultaneously.

### ⚛️ Physics Engine Stability
* **Anti-Teleportation:** Clamped the maximum repulsive force to **20.0** and set a minimum distance threshold to **1.0** in `output_window.c`.
    * *Result:* Prevents the drone from being "ejected" across the screen when close to obstacles.
* **Resize Handling:** Enhanced the `KEY_RESIZE` handler to immediately re-clamp drone coordinates to the new window boundaries, preventing visual artifacts.

---

## 📂 File Structure

### Network Module (New)
* **`network.c`**: The core socket handler. Manages connection establishment and the data transmission loop.
* **`network.h`**: Defines the strict protocol headers and helper functions for blocking I/O.
* **`network_bridge.c`**: Alternative implementation for different testing scenarios.

### Core System
* **`blackboard.c`**: The process orchestrator. Spawns children and manages IPC pipes.
* **`server.c`**: The central data hub. Multiplexes inputs from UI, Physics, and Network using `select()`.
* **`output_window.c`**: The physics engine and renderer. Calculates dynamics based on the Mass-Spring-Damper model.
* **`input_window.c`**: Captures user keystrokes and displays telemetry.

---

## 🔧 How to Run

### 1. Build the Project
You can compile the project using either the provided Makefile or the shell script.

**Option A: Using Make (Recommended)**
```bash
make
```

**Option B: Using Compile Script**
```bash
chmod +x compile.sh
./compile.sh
```

### 2. Execute (Network Mode)
The system now requires you to specify whether this instance is the **Server** (Host) or **Client** (Remote).

**Host A (Server):**
Start this instance first. It will listen for incoming connections.
```bash
./run.sh 2
```
*(Note: Mode 2 typically designates the Server)*

**Host B (Client):**
Start this instance second. You must provide the IP address of Host A.
```bash
./run.sh 1 <IP_ADDRESS_OF_HOST_A>
```
*(Example: `./run.sh 1 192.168.1.5`)*

### 3. Dependencies
Ensure you have the `ncurses` library installed:
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```
