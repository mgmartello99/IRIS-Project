#ifndef  __DHCPSVR_H
#pragma once
#define  __DHCPSVR_H

// Interface
#define INTERFACE_DHCPSVR                       "LSL_DHCPSVR"


// Commands to read/write the configuration file
#define LSL_DHCPSVR_CMD_IP_RANGE				0x0001
#define LSL_DHCPSVR_CMD_DEFAULT_LEASE_TIME		0x0002
#define LSL_DHCPSVR_CMD_MAX_LEASE_TIME			0x0003

// list type for the OS_DHCPSVR_GET_LEASES function
#define LSL_DHCPSVR_LT_IP_ADDR		1

#define  LSL_DHCPSVR_ERROR_BUF_TOO_SMALL    16#90000011

TYPE
  #pragma pack (push, 1)
  LSL_DHCPSVR_TYPE : STRUCT
    version	        	: UDINT;
    DHCPSVR_Start        : pVoid;
    DHCPSVR_Stop         : pVoid;
    DHCPSVR_Restart      : pVoid;
	DHCPSVR_Installed	: pVoid;
	DHCPSVR_IsRunning	: pVoid;
    DHCPSVR_GetConfig    : pVoid;
    DHCPSVR_SetConfig    : pVoid;
    // Since version 0x00010001
    DHCPSVR_ConfigureInterfaces : pVoid;
    // Since version 0x00010002
    DHCPSVR_GetConfig2   : pVoid;
    DHCPSVR_SetConfig2   : pVoid;
    // Since version 0x00010003
    DHCPSVR_GetLeases   : pVoid;
    DHCPSVR_GetLeasesChange : pVoid;
  END_STRUCT;
  #pragma pack (pop)
END_TYPE

FUNCTION __CDECL GLOBAL P_DHCPSVR_Start
  VAR_OUTPUT
  	retcode     : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_Stop
  VAR_OUTPUT
  	retcode     : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_Restart
 VAR_OUTPUT
   	retcode     : DINT;
 END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_Installed
  VAR_OUTPUT
   	retcode     : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_IsRunning
 VAR_OUTPUT
   	retcode     : DINT;
 END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_GetConfig
  VAR_INPUT
    cmd       	: UDINT;
    stringVal 	: ^CHAR;
    intVal		: ^DINT;
    strLen		: UDINT;
  END_VAR
  VAR_OUTPUT
  	retcode     : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_GetConfig2
  VAR_INPUT
  	idx         : DINT;
    cmd       	: UDINT;
    stringVal 	: ^CHAR;
    intVal		: ^DINT;
    strLen		: UDINT;
  END_VAR
  VAR_OUTPUT
  	retcode     : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_SetConfig
  VAR_INPUT
    cmd       	: UDINT;
    stringVal 	: ^CHAR;
    intVal		: DINT;
  END_VAR
  VAR_OUTPUT
  	retcode     : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_SetConfig2
  VAR_INPUT
	  idx         : DINT;
    cmd       	: UDINT;
    stringVal 	: ^CHAR;
    intVal		: DINT;
  END_VAR
  VAR_OUTPUT
  	retcode     : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_DHCPSVR_ConfigureInterfaces
  VAR_INPUT
    cnt       	: UDINT;
    interfaceArray	: ^CHAR;
  END_VAR
  VAR_OUTPUT
  	retcode     : DINT;
  END_VAR;

(* Abfrage der vom DHCP Server aktuell vergebenen IP Adressen *)
FUNCTION __CDECL GLOBAL P_DHCPSVR_GetLeases
  VAR_INPUT
    buffer : ^CHAR; // Buffer in den die abgefragten Werte reingeschrieben werden
    bufSize : UDINT; // Größe des Buffers
    listType : UDINT; // abzufragender Typ, z.B. LSL_DHCPSVR_LT_IP_ADDR für IP-Adressen
  END_VAR
  VAR_OUTPUT
  	retcode     : DINT; // >=0: Anzahl der zurückgegeben Elemente, <0: Fehler
                        // Wenn LSL_DHCPSVR_ERROR_BUF_TOO_SMALL zurückgegeben wird, dann war der Buffer 
                        // zu klein und der Aufruf muss mit einem größeren Buffer wiederholt werden.
  END_VAR;

(* informiert, wenn im DHCP Server Änderungen an den vergebenen IP Adressen stattgefunden haben *)
FUNCTION __CDECL GLOBAL P_DHCPSVR_GetLeasesChange
  VAR_INPUT
    oldChgCnt : ^UDINT; // Zeiger auf eine Variable, in der die Anzahl der erkannten Änderungen
                        // seit dem letzten Aufruf gespeichert werden. Muss bei wiederholten 
                        // Aufrufen immer auf die gleiche Variable zeigen.
  END_VAR
  VAR_OUTPUT
  	retcode     : DINT; // 1=Änderung, 0=keine Änderung, <0=Fehler.
  END_VAR;


#define OS_DHCPSVR_CHECK_VERSION(pDHCPSVR)				pDHCPSVR^.version <> LSL_DHCPSVR_TYPE_VERSION
#define OS_DHCPSVR_START(pDHCPSVR)              		pDHCPSVR^.DHCPSVR_Start 		$ P_DHCPSVR_Start()
#define OS_DHCPSVR_STOP(pDHCPSVR)              			pDHCPSVR^.DHCPSVR_Stop 			$ P_DHCPSVR_Stop()
#define OS_DHCPSVR_RESTART(pDHCPSVR)              		pDHCPSVR^.DHCPSVR_Restart 		$ P_DHCPSVR_Restart()
#define OS_DHCPSVR_INSTALLED(pDHCPSVR)					pDHCPSVR^.DHCPSVR_Installed		$ P_DHCPSVR_Installed();
#define OS_DHCPSVR_ISRUNNING(pDHCPSVR)					pDHCPSVR^.DHCPSVR_IsRunning		$ P_DHCPSVR_IsRunning();
#define OS_DHCPSVR_GETCONFIG(pDHCPSVR,p1,p2,p3,p4)     pDHCPSVR^.DHCPSVR_GetConfig 		$ P_DHCPSVR_GetConfig(p1,p2,p3,p4)
#define OS_DHCPSVR_SETCONFIG(pDHCPSVR,p1,p2,p3)        pDHCPSVR^.DHCPSVR_SetConfig 		$ P_DHCPSVR_SetConfig(p1,p2,p3)
#define OS_DHCPSVR_GETCONFIG2(pDHCPSVR,p1,p2,p3,p4,p5)     pDHCPSVR^.DHCPSVR_GetConfig2 		$ P_DHCPSVR_GetConfig2(p1,p2,p3,p4,p5)
#define OS_DHCPSVR_SETCONFIG2(pDHCPSVR,p1,p2,p3,p4)        pDHCPSVR^.DHCPSVR_SetConfig2 		$ P_DHCPSVR_SetConfig2(p1,p2,p3,p4)
#define OS_DHCPSVR_CONFIGURE_INTERFACES(pDHCPSVR,p1,p2) pDHCPSVR^.DHCPSVR_ConfigureInterfaces $ P_DHCPSVR_ConfigureInterfaces(p1,p2)
#define OS_DHCPSVR_GET_LEASES(pDHCPSVR,p1,p2,p3) pDHCPSVR^.DHCPSVR_GetLeases $ P_DHCPSVR_GetLeases(p1,p2,p3)
#define OS_DHCPSVR_GET_LEASES_CHANGE(pDHCPSVR,p1) pDHCPSVR^.DHCPSVR_GetLeasesChange $ P_DHCPSVR_GetLeasesChange(p1)
#endif
