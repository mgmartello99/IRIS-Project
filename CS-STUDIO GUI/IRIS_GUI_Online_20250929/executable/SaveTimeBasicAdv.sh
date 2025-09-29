#!/bin/bash

# Log file location
LOGFILE="./timelog/logTimers$(date +%Y-%m-%d).log"

# Get current system time in readable format
CURRENT_TIME=$(date +"%Y-%m-%d %H:%M:%S")

# Append the time to the log file
echo " "  >> "$LOGFILE"
echo "CURRENT_TIME:" >> "$LOGFILE"
echo "$CURRENT_TIME"  >> "$LOGFILE"

echo "Irradiation time in [ms]">> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerIrrAdv1_R >> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerIrrAdv2_R >> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerIrrAdv3_R >> "$LOGFILE"
echo "" >> "$LOGFILE"
echo "Delayed time in [s]">> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerMov1_R >> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerMov2_R >> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerMov3_R >> "$LOGFILE"
echo " "  >> "$LOGFILE"
echo "Measurement time in [s]">> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerDis1_R >> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerDis2_R >> "$LOGFILE"
caget mc:a:maincontrol:VisualTimerDis3_R >> "$LOGFILE"


#cd /home/isolpharm/CSS/IRIS-GUI-latest/IRIS_remote
#python IRIS_auto_activity.py 2>&1
