# Drone Simulator Project Report (Assignment 1)

## 1. Sketch of the Architecture

The system utilizes a **Blackboard Server** architecture (Process/Pipes/Select model). A central Server acts as the core Blackboard, coordinating communication between all components via 6 unidirectional pipes.



## 2. Short Definitions of Active Components

| Component | Role | Algorithms & Primitives Used |
| :--- | :--- | :--- |
| **Blackboard (Master)** | **Initializes and Manages System** | `fork()`, `exec()`, `pipe()`, `signal(SIGINT)`. Ensures clean shutdown of all children. |
| **Server** | **Central Data Hub (Blackboard)** | `select()` for non-blocking I/O multiplexing. Aggregates data from all sources (I, T, O, D) and broadcasts the updated Game State (Pipe 5). |
| **Input Window (I)** | **User Control** | `ncurses` for UI/input, `write()` to send commands (Pipe 0). Uses robust signal masking (`SIGINT` blocked). |
| **Output Window (D)** | **Physics Engine & Display** | **Euler Integration** (numerical solution of motion equation). **Khatib Potential Fields** (repulsion). **Vector Projection** (converts repulsion into key steps). `ncurses` for drawing. |
| **Target Generator (T)** | **Game Logic** | `rand()`, `sleep()`. Generates targets that must be reached sequentially. |
| **Obstacle Generator (O)** | **Game Logic** | `rand()`, `sleep()`. Generates temporary static obstacles. |

## 3. List of Components, Directories, and Files

* **Directories:** `src/`, `include/`, `config/`, `obj/` (created by Makefile).
* **Source Files (.c):** `blackboard.c`, `server.c`, `input_window.c`, `output_window.c`, `obstacle.c`, `target.c`, `common_utils.c`.
* **Header Files (.h):** `common.h`, `handlers.h`.
* **Build/Run Files:** `Makefile`, `run.sh`.
* **Parameter File:** `config/config.txt`.

## 4. Instructions for Installing and Running

1.  **Installation:** Ensure `gcc` and `libncurses5-dev` are installed (`sudo apt-get install libncurses5-dev`).
2.  **Compilation:** Execute `make` in the project root directory.
3.  **Execution:** Run the shell script `./run.sh`.

## 5. Operational Instructions

* **Goal:** Guide the blue drone (`+`) to the green targets (1, 2, 3...) sequentially.
* **Controls (W/E/R, S/D/F, X/C/V):** Use the 9-key cluster to incrementally apply force to the drone. `D` acts as the brake (zeroes command forces).
* **Scoring:** Calculated using a weighted formula (Targets Hit * 100 - Time Elapsed Penalty).
* **Quit:** Press **Ctrl+C** in the original terminal where the program was launched (`blackboard`) to cleanly terminate all processes.

## 6. What Else is Useful (Code Quality & Compliance)
* **Signal Masks:** All child processes block `SIGINT` to ensure only the Master handles termination.
* **Pipe Hygiene:** Useless file descriptors are closed immediately after `fork()/exec()` in `blackboard.c`.
* **Help Feature:** All executables support `-h` or `--help` arguments.
* **Aesthetics:** The Input Window features an attractive, color-coded key cluster to enhance user experience.
