#!/bin/bash

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}Cleaning previous builds...${NC}"
make clean

echo -e "${GREEN}Compiling Assignment 3 Drone Simulator...${NC}"
make

if [ $? -eq 0 ]; then
    echo -e "${GREEN}Build Successful!${NC}"
    
    # Ensure log directory exists
    if [ ! -d "log" ]; then
        mkdir log
        echo "Created log directory."
    fi
    
    echo "------------------------------------------------"
    echo "To run:"
    echo "  1. Start Server: ./blackboard (Select Option 2)"
    echo "  2. Start Client: ./blackboard (Select Option 3)"
    echo "------------------------------------------------"
else
    echo -e "${RED}Build Failed. Please check the errors above.${NC}"
    exit 1
fi
