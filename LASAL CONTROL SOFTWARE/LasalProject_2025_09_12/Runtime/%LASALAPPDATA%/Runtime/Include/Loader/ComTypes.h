#ifndef __RTOS__COM__TYPES__434EA8E2_956E_4AC8_B865_D46665446068__INCLUDED_
#define __RTOS__COM__TYPES__434EA8E2_956E_4AC8_B865_D46665446068__INCLUDED_

#include "ComLink.h"

#define COMLINK_API_VERSION		1

// =========================================================================================
// Data-Types for the Comlink-communication
// =========================================================================================
TYPE
	comlinkReason : UINT;
END_TYPE

// Reason codes
#define COMLINK_OK							16#0000	// no error
#define COMLINK_IPR							16#0001	// the status code comes from the interpreter
#define COMLINK_ERR_MAXENTRIES				16#FFEF	// Maximum number of entries in list reached
#define COMLINK_ERR_GENERAL                 16#FFF0	// general unrecoverable error, without information of the cause.
#define COMLINK_ERR_TIMEOUT                 16#FFF1	// timeout while waiting for a response
#define COMLINK_ERR_BUF_TOO_SMALL			16#FFF2	// the size of the specified buffer is too small
#define COMLINK_ERR_WOULDBLOCK				16#FFF3	// the function failed because a call would block (i.e. another task is in this function)
#define COMLINK_ERR_LOGIN_REQUIRED          16#FFF8	// a login is required
#define COMLINK_ERR_INVALID_RESPONSE        16#FFF9	// server sent an invalid response
#define COMLINK_ERR_ADDRINUSE               16#FFFA	// address is already in use
#define COMLINK_ERR_CLIENT_NOTREADY         16#FFFB	// the client interface functions are not available 
#define COMLINK_ERR_UNSUPPORTED_INTERFACE   16#FFFC	// the specified interface type is not supported
#define COMLINK_ERR_MAXCONN                 16#FFFD	// maximum number of connections exceeded
#define COMLINK_ERR_NOCONNECTION            16#FFFE	// no connection to the server
#define COMLINK_ERR_INVALID_PARAM           16#FFFF	// invalid parameter specified

// interface numbers
#define COMLINK_IFNUM_LOCAL			0
#define COMLINK_IFNUM_INTERN		1
#define COMLINK_IFNUM_COM1			2
#define COMLINK_IFNUM_COM2			3
#define COMLINK_IFNUM_COM3			4
#define COMLINK_IFNUM_COM4			5
#define COMLINK_IFNUM_CAN1			6
#define COMLINK_IFNUM_CAN2			7	// not implemented
#define COMLINK_IFNUM_TCP1_OLD		8	// deprecated
#define COMLINK_IFNUM_TCP1   	   10	
#define COMLINK_IFNUM_TCP2   	   11	// not implemented
#define COMLINK_IFNUM_COM5			12
#define COMLINK_IFNUM_COM6			13
#define COMLINK_IFNUM_COM7			14
#define COMLINK_IFNUM_COM8			15
#define COMLINK_IFNUM_COM9			16
#define COMLINK_IFNUM_COM10			17
// Nummern für 10 ansteckbare CAN Devicese
#define COMLINK_IFNUM_CAN20         20
#define COMLINK_IFNUM_CAN21         21
#define COMLINK_IFNUM_CAN22         22
#define COMLINK_IFNUM_CAN23         23
#define COMLINK_IFNUM_CAN24         24
#define COMLINK_IFNUM_CAN25         25
#define COMLINK_IFNUM_CAN26         26
#define COMLINK_IFNUM_CAN27         27
#define COMLINK_IFNUM_CAN28         28
#define COMLINK_IFNUM_CAN29         29


// =========================================================================================
// Functions for the Comlink-communication
// =========================================================================================
FUNCTION GLOBAL ComLink;


FUNCTION GLOBAL ComlinkCyWork;		
FUNCTION GLOBAL ComlinkInit;

// LOGIN
FUNCTION GLOBAL __CDECL LOGIN
VAR_INPUT
	pComdef  :^COMDEF; 
END_VAR
VAR_OUTPUT
	result   :UINT;
END_VAR;

// LOGOUT
FUNCTION GLOBAL __CDECL LOGOUT
VAR_INPUT
	pComdef  :^COMDEF; 
END_VAR
VAR_OUTPUT
	result   :UINT;
END_VAR;

// TXCOMMAND
FUNCTION GLOBAL __CDECL TXCOMMAND
VAR_INPUT
	Command  :  UDINT;
	length   :  UDINT;
	charptr  : ^USINT;
	pComdef  : ^COMDEF;
	Presu	 : ^UDINT;	
END_VAR
VAR_OUTPUT
	Status   : IPRSTATES;
END_VAR;

// TXCOMMANDEX
FUNCTION GLOBAL __CDECL TXCOMMANDEX
VAR_INPUT
	Command  	:  UDINT;  	// command IPRStates
	length   	:  UDINT;  	// length of command 
	charptr  	: ^USINT;  	// pointer on the command line (must be copied) 
	pComdef  	: ^COMDEF; 
	pResuBuf	: ^RESULTS; // result buffer
	sizeResuBuf	: UDINT;	// size of result buffer
	pReason		: ^comlinkReason;	// reason-code in the case of an error
END_VAR
VAR_OUTPUT
	Status   : IPRSTATES;
END_VAR;

// TXUPD
FUNCTION GLOBAL __CDECL TXUPD2
VAR_INPUT
	pLslcommregdata : ^Lslcommregdata;
	pComdef         : ^COMDEF;
  uiflags         : UINT;   // befüllt die freien Bits in COMSTCELL.udUpdateRate
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR;
FUNCTION GLOBAL __CDECL TXUPD
VAR_INPUT
	pLslcommregdata : ^Lslcommregdata;
	pComdef  : ^COMDEF;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR;

// PrototypeCallback
FUNCTION GLOBAL __CDECL PrototypeCallback
VAR_INPUT
    pComdef  : ^COMDEF;
	pData    : ^d2LSE;
END_VAR;

// StartStopRefresh
FUNCTION GLOBAL __CDECL StartStopRefresh 
VAR_INPUT
	pComdef : ^COMDEF;
	count   : UINT;
	typ	  	: UINT; 	
END_VAR;

// INSTALLCALLBACK: nur für LSE
FUNCTION GLOBAL __CDECL INSTALLCALLBACK
VAR_INPUT
	PCallback : ^void;
END_VAR;
// Install callback zu einem COMDATA
FUNCTION GLOBAL __CDECL INSTALLCALLBACK_DYN
VAR_INPUT
   PCallback : ^void;
   pThis     : ^void;
   pComdata  : ^COMDATA;
END_VAR;

// LDR_SetCanWait //ws001
FUNCTION GLOBAL __CDECL LDR_SetCanWait
VAR_INPUT
	us_wait : usint;
END_VAR
VAR_OUTPUT
  old : usint;
END_VAR;  // /ws001

FUNCTION GLOBAL __CDECL LDR_SetCanWaitRemote
VAR_INPUT
	pComdef : ^COMDEF;
  wait : INT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL LDR_SetIamAliveTime
VAR_INPUT
  pComdef : ^COMDEF;
  remoteMaxIdleTime_ms : UDINT;
  maxNetworkDelay_ms : UDINT;
  remoteMaxCyclicTime_ms : UDINT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL LDR_isRemotePlcAlive
VAR_INPUT
  pComdef : ^COMDEF;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL LDR_InitRs232Interface
VAR_INPUT
  pThis : pVoid;
	interfaceNbr : UDINT;
  hComm : pVoid;
END_VAR
VAR_OUTPUT
  // 1 : initialized
  // 0 : init in progress
  //<0 : init failed
  initStatus : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL LDR_SetRs232ComlinkParams
VAR_INPUT
	interface : UDINT;
  baudrate : UINT;
  startServer : DINT;
  fnInitInterface : pVoid;
  pThis : pVoid;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL LDR_SetWait4Visu  //ZP000
VAR_INPUT
  bWait : BOOL;
  timeout_ms : UDINT;
END_VAR;

#endif // __RTOS__COM__TYPES__434EA8E2_956E_4AC8_B865_D46665446068__INCLUDED_
