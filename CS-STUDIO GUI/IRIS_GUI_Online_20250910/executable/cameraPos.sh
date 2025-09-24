#!/bin/bash

# use --video-rotate=90 instead of --vf=rotate=PI/2, it gives better border control
# use $(()) instead of $ only cuz in the second way it does not work properly

# The window geometry setting depends on the resolution of the monitor
# Ratio of original video - H:W = 9:5, 4:3 (fisheye)

# When deal with the multi-display setup, the default monitor where is the first one according to "xrandr --query", and the resolution needs to be adjusted accordingly, as well as the compensation pixels for the bottom windows placement

MonitorResX=$(xrandr | grep 'HDMI-1' | awk '{print $3}' | cut -d'x' -f1)
MonitorResY=$(xrandr | grep 'HDMI-1' | awk '{print $3}' | cut -d'x' -f2 | cut -d'+' -f1)

screenid=1

CamLeftH=$MonitorResY
CamLeftW=$((CamLeftH * 5 / 9))

CamTopH=$((MonitorResY / 2))
CamTopW=$(((MonitorResX - CamLeftW) / 2))
CamTopLPosX=$CamLeftW
CamTopRPosX=$((CamLeftW + CamTopW))

# The 75 pixels in CamBotH and CamBotPosY are meant to compensate the space mislignment along y axis caused by the title bar
CamBotH=$((MonitorResY / 2 - 40))
CamBotW=$CamTopW
CamBotLPosX=$CamLeftW
CamBotRPosX=$CamTopRPosX
CamBotPosY=$((MonitorResY - CamTopH + 45))

# Left window
mpv --video-rotate=90 --geometry=${CamLeftW}x${CamLeftH}+0+0 --title="CCTV2 10.5.0.203 - Discharge front view" --screen=${screenid} rtsp://isolpharm:isolpharm@localhost:8203/hsub&

# Top-left window
# Video crop - 1) The option acts before geometry, so the pixels in video-crop are those streamed by the cctv, not the one of the monitor. The option will be invalid if the crop rectangle is out of the original video. 2) The video crop parameter is hard coded here, the default resolution in streamed video is 640x360 / 640x480 (fisheye)
mpv --video-crop=480x360+80+0 --video-rotate=180 --geometry=${CamTopW}x${CamTopH}+${CamTopLPosX}+0 --title="CCTV1 10.5.0.202 - Charge front view" --screen=${screenid} rtsp://isolpharm:isolpharm@localhost:8202/hsub&

# Top-right window
mpv --video-rotate=180 --geometry=${CamTopW}x${CamTopH}+${CamTopRPosX}+0 --title="CCTV5 10.5.0.206 fisheye - Charge side view" --screen=${screenid} rtsp://isolpharm:isolpharm@localhost:8206/hsub&

# Bottom-left window
mpv --video-rotate=180 --geometry=${CamBotW}x${CamBotH}+${CamBotLPosX}+${CamBotPosY} --title="CCTV3 10.5.0.204 fisheye - Inbox back view" --screen=${screenid} rtsp://admin:123456@localhost:8204/hsub&

# Bottom-right window
mpv --video-crop=480x360+80+0 --video-rotate=180 --geometry=${CamBotW}x${CamBotH}+${CamBotRPosX}+${CamBotPosY} --title="CCTV4 10.5.0.205 - Detection position" --screen=${screenid} rtsp://isolpharm:isolpharm@localhost:8205/hsub&

# Typically all the streams open in 6 s, in particular 10.5.0.205 takes from the 4th to the 8th second
# Use 'killall mpv' command to kill all mpv open streams
# Do not open more times the same stream, after two-three times they could crash
