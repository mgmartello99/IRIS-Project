# Microsoft Developer Studio Project File - Name="LASAL_INCLUDE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LASAL_INCLUDE - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "LASAL_INCLUDE.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "LASAL_INCLUDE.mak" CFG="LASAL_INCLUDE - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "LASAL_INCLUDE - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "LASAL_INCLUDE - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LASAL_INCLUDE - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FAcs /YX /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LASAL_INCLUDE - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "LASAL_INCLUDE - Win32 Release"
# Name "LASAL_INCLUDE - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\lsl.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_can.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_et261.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_c_ifFTP.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_iflinker.h
# End Source File
# Begin Source File

SOURCE=.\LSL_C_IFLSE.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_ifssr.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_iftcp.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_c_kernel.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_meas.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_melkusram.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_mt.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_panel.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_c_print.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_serial.h
# End Source File
# Begin Source File

SOURCE=.\lsl_c_systrace.h
# End Source File
# Begin Source File

SOURCE=.\LSL_CITask.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_can.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_cdias.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_cmdif.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_et261.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_ethercat_slave.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_st_excpt.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_hardwaretree.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_httpclient.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_ifftp.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_iflinker.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_st_ifssr.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_st_kernel.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_keypad.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_st_lslfile.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_st_mt.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_onl.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_osfile.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_st_panel.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_pci.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_pdfview.h
# End Source File
# Begin Source File

SOURCE=.\Lsl_st_print.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_safety.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_safetydll.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_sdias.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_serial.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_spi.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_sramdisk.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_sysenv.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_sysmsg.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_syssernum.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_systrace.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_tcp_user.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_Usb.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_varan.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_varantocdias.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_VNC.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_webserver.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_winprinter.h
# End Source File
# Begin Source File

SOURCE=.\lsl_st_xtimer.h
# End Source File
# Begin Source File

SOURCE=.\LSL_STIFile.h
# End Source File
# Begin Source File

SOURCE=.\LSL_STITask.h
# End Source File
# Begin Source File

SOURCE=.\lsl_string.h
# End Source File
# Begin Source File

SOURCE=.\RTOS_C_interfaces.h
# End Source File
# Begin Source File

SOURCE=.\RTOS_CGlobals.h
# End Source File
# Begin Source File

SOURCE=.\Rtos_interfaces.h
# End Source File
# Begin Source File

SOURCE=.\SysCRT.h
# End Source File
# Begin Source File

SOURCE=.\T_grc.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\hcopy.cmd
# End Source File
# Begin Source File

SOURCE=.\hfilter.exe
# End Source File
# Begin Source File

SOURCE=.\lsl_c_can.txt
# End Source File
# Begin Source File

SOURCE=.\lsl_st_can.txt
# End Source File
# Begin Source File

SOURCE=.\lsl_st_mt.txt
# End Source File
# End Target
# End Project
