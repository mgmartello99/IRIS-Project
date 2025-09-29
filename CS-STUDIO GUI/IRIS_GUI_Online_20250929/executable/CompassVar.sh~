#!/bin/bash

#log file location
LOGFILE="./CompassVar/Compass.log"

#ask the terminal the value from pv_IOC and write the output of caget in the logFile
caget AcquisitionTimeCoMPASS > "$LOGFILE"
caget RadioIsotopeCoMPASS >> "$LOGFILE"

cd /home/isolpharm/CSS/IRIS-GUI-latest/IRIS_remote
python IRIS_auto_activity.py 2>&1
