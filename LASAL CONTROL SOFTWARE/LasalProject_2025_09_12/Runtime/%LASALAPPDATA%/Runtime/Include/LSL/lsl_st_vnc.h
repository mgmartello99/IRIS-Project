(****************************************************************************
 *                        INTERFACE zum VNC - CLIENT                        *
 *                                                                          *
 *   Originalversion: 28.07.2004, MITTERBAUER Ferdinand                     *
 *   Letztes Update : 24.09.2004, MITTERBAUER Ferdinand                     *
 *                                                                          *
 ****************************************************************************)

#ifndef	__VNC_CLI__
#pragma once
#define __VNC_CLI__

TYPE
	_VNCHANDLE	: ^void;
END_TYPE
//
// Interface
//
TYPE
	OS_VNCCLI	: STRUCT
		udSize					: UDINT;
		udVersion				: UDINT;
		udVersionVNC			: UDINT;
		VNCInit					: pvoid;
		VNCExit					: pvoid;
		VNCConnect				: pvoid;
		VNCDisconnect			: pvoid;
		VNCDisconnectHandle		: pvoid;
		VNCGetSize				: pvoid;
		VNCReset				: pvoid;
		VNCSendKbrdEvent		: pvoid;
		VNCSendHIDEvent			: pvoid;
		VNCDraw					: pvoid;
		VNCForceRepaint			: pvoid;
		// Ab Version 1.1.3
		VNCSendKeyboardEvents	: pvoid;
		VNCSendPointerEvents	: pvoid;
		// Ab Version 1.1.16
		VNCSetCliSleep	      : pvoid;
		VNCFreeBuffer         : pvoid;
		// Ab Version 1.1.18
		VNCSetTaskDelayTime   : pvoid;
		// Ab Version 1.1.19
		VNCFreeOldSockets   : pvoid;
		// Salamander ARM
		VNCDraw2            : pvoid;
		// ab Version 1.1.23
		VNCSetKaValues        : pvoid;
		// ab Version 1.1.27
		VNCSetConnectTimeout  : pvoid;
		// ab Version 1.1.28
		VNCSetOptions  : pvoid;
	END_STRUCT;
END_TYPE

TYPE
	OS_VNCSVR	: STRUCT
		udSize						: UDINT;
		udVersion					: UDINT;
		udVersionVNC				: UDINT;
		VNCSVRInit					: pvoid;
		VNCSVRExit					: pvoid;
		VNCSVRReset					: pvoid;
		VNCSVRSetPass				: pvoid;
		VNCSVREnumConnections		: pvoid;
		VNCSVRDisconnectAllClients	: pvoid;
		VNCSVRDisconnectClient		: pvoid;
		VNCSVRFullRefreshCycle		: pvoid;
		VNCSVRTimeoutInactiveClient	: pvoid;
		VNCSVRAcceptKeyboardEvents	: pvoid;
		VNCSVRAcceptPointerEvents	: pvoid;
		VNCSVRSetFilter				: pvoid;
		VNCSVRDisconnectClientsOnNonsharedConnection	: pvoid;
		VNCSVRNeverShared			: pvoid;
		VNCSVRAlwaysShared			: pvoid;
		VNCSVRMaxConnections		: pvoid;
		VNCSVRClientWaitTimeMillis	: pvoid;
		VNCSVRCompareFB				: pvoid;
		VNCSVRSetDisplayOffset		: pvoid;
		VNCSVRUpdateRect			: pvoid;
		VNCSVRInitialized			: pvoid;
		VNCSVRConnectedClients		: pvoid;
		VNCSVRReserved0				: pvoid;
		VNCSVRReserved1				: pvoid;
		VNCSVRDisableSharedMemory	: pvoid;
		// ab Version 1.1.38
		VNCSVREnumConnections2		: pvoid;
		VNCSVRConnectedClients2		: pvoid;

	END_STRUCT;
END_TYPE

(*
 *  PARAMETER DER LASAL - CALLBACK - Funktion
 *)
#define VNCMSG_CONNECT				1
#define VNCMSG_CONNECT_STARTED		0
#define VNCMSG_CONNECT_DONE  		1
#define VNCMSG_CONNECT_FAILED  		2
#define VNCMSG_CONNECT_AUTHFAILED   3

#define VNCMSG_DISCONNECT			2
#define VNCMSG_DISCONNECT_STARTED	0
#define VNCMSG_DISCONNECT_DONE		1
#define VNCMSG_DISCONNECT_EXIT		2

#define VNCMSG_UPDATE				3
#define VNCMSG_UPDATE_SCREEN		0

#define VNCMSG_TRAFFIC				4	(* Der Parameter gibt dann den Netzwerktraffic in kBit/s an *)

#define VNCMSG_TIMEOUT				5	// func   param: 0 - call disconnect und connect

#define VNCMSG_RFB_READ_ERROR		6	// func, Error in function ReadFromRFBServer

(*
 *  VNC-Errorcodes (OS)
 *)
#define VNC_OK						0
#define VNC_NO_VIEWMANGER			1
#define VNC_NO_THREAD				2
#define VNC_NO_BUFFER				3
#define VNC_NO_ATTACHED_VIEW		4
#define VNC_FAILED_BUFFER_LOCK		5
#define VNC_FAILED_BUFFER_UNLOCK	6
#define VNC_INIT_FAILED				7   (* VNC-Client Service konnte nicht initialisiert werden.           *)
#define VNC_NOT_DRAWN				8	(* Es wurde nichts gezeichnet - Passiert, wenn der Client (noch)   *)
										(* 	keinen zugewiesenen Pixelpuffer hat, bzw. kein Pixelpuffer     *)
										(*	angelegt wurde, da zuwenig Speicher vorhanden war.             *)

(*
 *  VNC-Errorcodes (LASAL: _VncClientService - Start ab 16#100, damit noch Platz fuer zuk�nftige OS-Fehlermeldungen ist)
 *)
#define VNC_NOT_INITIALIZED		   16#100		// you have to call VNCInit first!
#define VNC_NO_OS_INTERFACE		   16#101
#define VNC_INVALID_HANDLE		   16#102
#define VNC_CONNECT_FAILED		   16#103

(*
 *  VNC-Server ERRORCODES 
 *)
#define VNC_NO_SUCH_CLIENT			16#0050	
#define	VNC_NO_SCREENUPDATE_MANAGER 16#0051

#define VNC_INVALID_PWDFILE			16#0052
#define VNC_OLDPWD_INVALID			16#0053
#define VNC_FAILED_OPEN_PWDFILE		16#0054
#define VNC_FAILED_WRITING_PWDFILE	16#0055

#define	VNC_NO_SCREENBUFFER			16#0056
#define	VNC_NOT_IMPLEMENTED			16#0057

(*
 *  VNC-Client Options 
 *)
#define VNCCLI_OPTION_USE_REMOTE_CURSOR       1
#define VNCCLI_OPTION_COLOR_DEPTH             2
#define VNCCLI_OPTION_ENCODING_FORMAT         3
#define VNCCLI_OPTION_CLEAR_SCREEN_AT_CONNECT 4
#define VNCCLI_OPTION_SCALING_FACTOR          5
#define VNCCLI_OPTION_USE_COLOR_DEPTH_FROM_SERVER 6

(* ========== DEFINITION der FUNKTIONSPROTOTYPEN ========== *)


FUNCTION __CDECL GLOBAL G_VNCCallback
VAR_INPUT
	myThis		: pvoid;
	func		: UDINT;
	param		: UDINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCInit
VAR_INPUT
	logStr		: ^CHAR;
	logFileName	: ^CHAR;
	Callback	: pvoid;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCExit;

FUNCTION __CDECL GLOBAL G_VNCConnect
VAR_INPUT
	hostinfo	: ^CHAR;
	username	: ^CHAR;
	password	: ^CHAR;

	myThis		: pvoid;
	// Ab Version 1.1.12, Default 5
	prio		  : UDINT;
	// Ab Version 1.1.15
  dontfree  : UDINT; 
END_VAR
VAR_OUTPUT
	retval		: _VNCHANDLE;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCDisconnect
VAR_INPUT
	hostinfo	: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: _VNCHANDLE;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCDisconnectHandle
VAR_INPUT
	hVNC		: _VNCHANDLE;
END_VAR
VAR_OUTPUT
	retval		: _VNCHANDLE;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCGetSize
VAR_INPUT
	hVNC		: _VNCHANDLE;
	width		: ^DINT;
	height		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSendKbrdEvent
VAR_INPUT
	_hVNC		: _VNCHANDLE;
	eventtype	: UDINT;
	scancode	: UINT;
	modifier	: UINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSendHIDEvent
VAR_INPUT
	_hVNC		: _VNCHANDLE;
	eventtype	: UDINT;
	button  	: UINT;
	x 			: INT;
	y 			: INT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCDraw2
VAR_INPUT
	_hVNC		: _VNCHANDLE;
	clientRect	: ^_ROOM;
	scroll_x	: DINT;
	scroll_y	: DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
#ifndef _LSL_TARGETARCH_ARM
FUNCTION __CDECL GLOBAL G_VNCDraw
VAR_INPUT
	_hVNC		: _VNCHANDLE;
	clientRect	: _ROOM;
	scroll_x	: DINT;
	scroll_y	: DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
#endif

FUNCTION __CDECL GLOBAL G_VNCForceRepaint
VAR_INPUT
	_hVNC		: _VNCHANDLE;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSendKeyboardEvents
VAR_INPUT
	_hVNC		: _VNCHANDLE;
	set			: DINT;
	pSend		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSendPointerEvents
VAR_INPUT
	_hVNC		: _VNCHANDLE;
	set			: DINT;
	pSend		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSetCliSleep
VAR_INPUT
	_hVNC		: _VNCHANDLE;
	sstate  	: UINT;
	kalive  	: UINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL G_VNCFreeBuffer
VAR_INPUT
	bufnum  	: UINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL G_VNCSetTaskDelayTime
VAR_INPUT
	_hVNC		: _VNCHANDLE;
	delayTime	: UINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL G_VNCFreeOldSockets
VAR_INPUT
	enable	: UINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSetKaValues
VAR_INPUT
	hVNC		  : _VNCHANDLE;
	Interval  : UINT;             // Zeit in Sek. bis erste KA-Paket gesendet wird
	Retry	    : UINT;             // Zeit in Sek. zwischen zwei KA-Paketen
	Timeout	  : UINT;             // Zeit in Sek. wie lange KA-Pakete gesendet werden
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSetConnectTimeout
VAR_INPUT
	Timeout	  : UDINT;            // Timeout in Sekunden
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSetOptions
VAR_INPUT
	optionId	: DINT;
	optionAddr	: ^void;
	optionSize	: DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVRInit
VAR_INPUT
	logStr		: ^CHAR;
	logFileName	: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRExit;


FUNCTION __CDECL GLOBAL G_VNCSVRReset
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRSetPass
VAR_INPUT
	oldPassword	: ^CHAR;
	newPassword	: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVREnumConnections
VAR_INPUT
	pThis		: pvoid;
	Callback	: pvoid;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVREnumConnections2
VAR_INPUT
	pThis		: pvoid;
	Callback	: pvoid;
  listenerPort : DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVRDisconnectAllClients
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVRDisconnectClient
VAR_INPUT
	strClient	: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRFullRefreshCycle
VAR_INPUT
	set			: DINT;
	pSeconds	: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRTimeoutInactiveClient
VAR_INPUT
	set			: DINT;
	pSeconds	: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRAcceptKeyboardEvents
VAR_INPUT
	set			: DINT;
	pAccept		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRAcceptPointerEvents
VAR_INPUT
	set			: DINT;
	pAccept		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRSetFilter
VAR_INPUT
	strFilter	: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRDisconnectClientsOnNonsharedConnection
VAR_INPUT
	set			: DINT;
	pDisconnect	: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRNeverShared
VAR_INPUT
	set			: DINT;
	pValue		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRAlwaysShared
VAR_INPUT
	set			: DINT;
	pValue		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRMaxConnections
VAR_INPUT
	set			: DINT;
	pValue		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRClientWaitTimeMillis
VAR_INPUT
	set			: DINT;
	pValue		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRCompareFB
VAR_INPUT
	set			: DINT;
	pValue		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL G_VNCSVRSetDisplayOffset
VAR_INPUT
	Offset		: DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVRUpdateRect
VAR_INPUT
	x,y,w,h		: DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVRInitialized
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVRConnectedClients
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVRConnectedClients2
VAR_INPUT
  listenerPort : DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

FUNCTION __CDECL GLOBAL G_VNCSVRDisableSharedMemory
VAR_INPUT
	set			: DINT;
	pValue		: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;

#endif // __VNC_CLI__
