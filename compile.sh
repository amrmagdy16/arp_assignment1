#!/bin/bash
# Description: Compiles ALL modules including Watchdog
# Usage: ./compile.sh

echo "Building Full Drone Simulator (with Watchdog)..."
mkdir -p bin log config

# Compile Common Obj
gcc -c src/common_utils.c -o bin/common_utils.o -Iinclude

# Compile Modules
gcc src/blackboard.c bin/common_utils.o -o blackboard -Iinclude
gcc src/server.c bin/common_utils.o -o server -Iinclude
gcc src/input_window.c bin/common_utils.o -o input_window -lncurses -Iinclude
gcc src/output_window.c bin/common_utils.o -o output_window -lncurses -lm -Iinclude
gcc src/obstacle.c bin/common_utils.o -o obstacle -Iinclude
gcc src/target.c bin/common_utils.o -o target -Iinclude

# Compile Watchdog (Specific to Full Project)
gcc src/watchdog.c bin/common_utils.o -o watchdog -lncurses -Iinclude

echo "Full Build Complete. Run ./run.sh to start."
