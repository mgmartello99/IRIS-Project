#!/bin/bash

# Log file location
LOGFILE="./timelog/detection_time_$(date +%Y-%m-%d).log"

# Get current system time in readable format
CURRENT_TIME=$(date +"%Y-%m-%d %H:%M:%S")

# Append the time to the log file
echo "Detection stops at: $CURRENT_TIME" >> "$LOGFILE"
echo "" >> "$LOGFILE"

echo "Detection stops at: $CURRENT_TIME"
