# Compiler and Flags
CC = gcc
CFLAGS = -I./include -Wall -g
LDFLAGS = -lncurses -lm

# Executables to generate
TARGETS = blackboard server input_window output_window obstacle target watchdog network

# Source directory for organization
SRC_DIR = src

# Component mapping
all: $(TARGETS)

# 1. Blackboard (Master Process)
blackboard: $(SRC_DIR)/blackboard.c $(SRC_DIR)/common_utils.c
	$(CC) $(CFLAGS) -o blackboard $(SRC_DIR)/blackboard.c $(SRC_DIR)/common_utils.c

# 2. Server (The Brain - handles multiplexing)
server: $(SRC_DIR)/server.c $(SRC_DIR)/common_utils.c
	$(CC) $(CFLAGS) -o server $(SRC_DIR)/server.c $(SRC_DIR)/common_utils.c $(LDFLAGS)

# 3. Input Window (Keyboard manager)
input_window: $(SRC_DIR)/input_window.c $(SRC_DIR)/common_utils.c
	$(CC) $(CFLAGS) -o input_window $(SRC_DIR)/input_window.c $(SRC_DIR)/common_utils.c $(LDFLAGS)

# 4. Output Window (Physics engine and display)
output_window: $(SRC_DIR)/output_window.c $(SRC_DIR)/common_utils.c
	$(CC) $(CFLAGS) -o output_window $(SRC_DIR)/output_window.c $(SRC_DIR)/common_utils.c $(LDFLAGS)

# 5. Obstacle Generator (Standalone mode only)
obstacle: $(SRC_DIR)/obstacle.c $(SRC_DIR)/common_utils.c
	$(CC) $(CFLAGS) -o obstacle $(SRC_DIR)/obstacle.c $(SRC_DIR)/common_utils.c

# 6. Target Generator (Standalone mode only)
target: $(SRC_DIR)/target.c $(SRC_DIR)/common_utils.c
	$(CC) $(CFLAGS) -o target $(SRC_DIR)/target.c $(SRC_DIR)/common_utils.c

# 7. Watchdog (Process monitor - standalone mode only)
watchdog: $(SRC_DIR)/watchdog.c $(SRC_DIR)/common_utils.c
	$(CC) $(CFLAGS) -o watchdog $(SRC_DIR)/watchdog.c $(SRC_DIR)/common_utils.c $(LDFLAGS)

# 8. Network Bridge (New Assignment 3 Bridge)
network: $(SRC_DIR)/network.c $(SRC_DIR)/common_utils.c
	$(CC) $(CFLAGS) -o network $(SRC_DIR)/network.c $(SRC_DIR)/common_utils.c $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -f $(TARGETS)
	rm -rf log/*.txt
