#!/bin/bash
# Description: Launches the Master Process
# Usage: ./run.sh

# Ensure logs and config exist
mkdir -p log config
touch log/error_log.txt

echo "Starting Full Simulator..."
./blackboard
