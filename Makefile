# Makefile for Drone Simulator Project
# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lncurses -lm

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

# Source Files
COMMON_SRC = $(SRC_DIR)/common_utils.c
SRCS = $(SRC_DIR)/blackboard.c $(SRC_DIR)/server.c $(SRC_DIR)/input_window.c \
       $(SRC_DIR)/output_window.c $(SRC_DIR)/obstacle.c $(SRC_DIR)/target.c

# Object Files
COMMON_OBJ = $(OBJ_DIR)/common_utils.o
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executables
TARGETS = blackboard server input_window output_window obstacle target

# Default Target
all: directories $(TARGETS)

# Rule to create directories
directories:
	@mkdir -p $(OBJ_DIR)

# Compile Common Utils
$(COMMON_OBJ): $(COMMON_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Rules for Executables
blackboard: $(OBJ_DIR)/blackboard.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

server: $(OBJ_DIR)/server.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

input_window: $(OBJ_DIR)/input_window.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

output_window: $(OBJ_DIR)/output_window.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

obstacle: $(OBJ_DIR)/obstacle.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

target: $(OBJ_DIR)/target.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Generic Rule for Object Files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Up
clean:
	rm -rf $(OBJ_DIR) $(TARGETS)
	rm -rf log/*.txt

# Phony Targets
.PHONY: all clean directories
