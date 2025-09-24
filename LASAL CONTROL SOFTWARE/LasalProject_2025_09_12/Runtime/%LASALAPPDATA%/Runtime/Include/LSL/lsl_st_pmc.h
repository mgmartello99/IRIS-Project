#pragma once

(* Funktionen zur Kommunikation im dem PMC(Power Management Controller) - des HGW1033 *)

#define INTERFACE_PMC "PMC"

(* Fehlercodes der PMC Funkionen *)
#define PMCERROR_INVALID_HANDLE   16#9000000A
#define PMCERROR_NOT_INIT         16#9000000E
#define PMCERROR_BUF_TOO_SMALL    16#90000011
#define PMCERROR_READ_FAILED			16#9000001C
#define PMCERROR_WRITE_FAILED			16#9000001D
#define PMCERROR_CRC_WRONG				16#90000026
#define PMCERROR_BUSY             16#9000002C
#define PMCERROR_COMM_TIMEOUT		  16#90000090

TYPE
	LSL_PMC :STRUCT
// version 0
		version             : UDINT;
    ExecRequest         : pVoid;
    StartAsyncRequest   : pVoid;
    PollAsyncRequest    : pVoid;
// verison 1 (needs FW version 5.0 or higher)
    TransportMode       : pVoid;
    DeepSleepMode       : pVoid;
    PowerLoad           : pVoid;
    PowerLoadState      : pVoid;
    TransportModeLevel  : pVoid;
	END_STRUCT;
END_TYPE

(*
  Führt einen synchronen Request zum PMC aus
 
  \type_ Request Typ (Kommando)
  \reqData Daten des Requests
  \reqLen Größe der Requestdaten
  \rspBuf Response Buffer - wird mit der Antwort beschrieben
  \rspBufLen Größe des Response Buffers
  \return >= 0: Anzahl der in den Response Buffer geschriebenen Daten,
  		bei einem Fehler wird ein neg. Wert zurückgegeben.
*)
FUNCTION GLOBAL __cdecl P_PMC_ExecRequest
VAR_INPUT
  type_ : USINT;
  reqData : ^USINT;
  reqLen : UDINT;
  rspBuf : ^USINT;
  rspBufLen : UDINT;
END_VAR
VAR_OUTPUT
	retval      : DINT;
END_VAR;

(*
  Startet einen asynchronen Request zum PMC.
  Der Status des Requests kann dann mit der Funktion OS_PMC_POLL_ASYNC_REQUEST
  gepollt werden. OS_PMC_POLL_ASYNC_REQUEST muss so lange aufgerufen werden, bis 
  ein Wert ungleich PMCERROR_BUSY zurückgeliefert wird, sonst wird der Request 
  nicht entfernt und der Speicher für aktive Requests kann voll werden.
 
  \type_ Request Typ (Kommando)
  \reqData Daten des Requests
  \reqLen Größe der Requestdaten
  \return >= 0: Handle des asynchronen Requests,
  		bei einem Fehler wird ein neg. Wert zurückgegeben.
*)
FUNCTION GLOBAL __cdecl P_PMC_StartAsyncRequest
VAR_INPUT
  type_ : USINT;
  reqData : ^USINT;
  reqLen : UDINT;
END_VAR
VAR_OUTPUT
	retval      : DINT;
END_VAR;

(*
  Pollt einen asynchronen Request zum PMC.
  Ein Request, der mit OS_PMC_START_ASYNC_REQUEST gestartet wurde, muss so lange 
  gepollt werden, bis ein Wert ungleich PMCERROR_BUSY zurückgeliefert wird, 
  sonst wird der Request nicht entfernt und der Speicher für aktive Requests kann 
  voll werden.
 
  \handle Handle des asynchronen Requests (=Rückgabewert von OS_PMC_START_ASYNC_REQUEST)
  \rspBuf Response Buffer - wird mit der Antwort beschrieben
  \rspBufLen Größe des Response Buffers
  \return >= 0: Anzahl der in den Response Buffer geschriebenen Daten,
  		bei einem Fehler wird ein neg. Wert zurückgegeben,
  		bzw. wenn der Request noch nicht fertig ist, der Wert PMCERROR_BUSY.
*)
FUNCTION GLOBAL __cdecl P_PMC_PollAsyncRequest
VAR_INPUT
  handle : DINT;
  rspBuf : ^USINT;
  rspBufLen : UDINT;
END_VAR
VAR_OUTPUT
	retval      : DINT;
END_VAR;



// mode: 0 = abort, 1 = enable (OS handling to get to 30%), 2 = enable at current battery level
FUNCTION GLOBAL __cdecl P_PMC_TransportMode
VAR_INPUT
  mode : UDINT;
END_VAR
VAR_OUTPUT
	retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_PMC_DeepSleepMode
VAR_OUTPUT
	retval      : DINT;
END_VAR;

// activate: 0 = disable charging power, 1 = enable charging power
FUNCTION GLOBAL __cdecl P_PMC_PowerLoad
VAR_INPUT
  activate : UDINT;
END_VAR
VAR_OUTPUT
	retval      : DINT;
END_VAR;

// Bit0: 0=not charging, 1=charging
FUNCTION GLOBAL __cdecl P_PMC_PowerLoadState
VAR_INPUT
  pState : ^UDINT;
END_VAR
VAR_OUTPUT
	retval      : DINT;
END_VAR;

// returns the battery level in [%] the OS uses for the transport mode (e.g. 30 for 30%)
FUNCTION GLOBAL __cdecl P_PMC_TransportModeLevel
VAR_OUTPUT
	retval      : DINT;
END_VAR;

// Version 0
#define OS_PMC_EXEC_REQUEST(pPMC,p1,p2,p3,p4,p5)  pPMC^.ExecRequest       $ P_PMC_ExecRequest(p1,p2,p3,p4,p5)
#define OS_PMC_START_ASYNC_REQUEST(pPMC,p1,p2,p3) pPMC^.StartAsyncRequest $ P_PMC_StartAsyncRequest(p1,p2,p3)
#define OS_PMC_POLL_ASYNC_REQUEST(pPMC,p1,p2,p3)  pPMC^.PollAsyncRequest  $ P_PMC_PollAsyncRequest(p1,p2,p3)

// Version 1
#define OS_PMC_TRANSPORT_MODE(pPMC,p1)            pPMC^.TransportMode     $ P_PMC_TransportMode(p1)
#define OS_PMC_DEEP_SLEEP_MODE(pPMC)              pPMC^.DeepSleepMode     $ P_PMC_DeepSleepMode()
#define OS_PMC_POWER_LOAD(pPMC,p1)                pPMC^.PowerLoad         $ P_PMC_PowerLoad(p1)
#define OS_PMC_POWER_LOAD_STATE(pPMC,p1)          pPMC^.PowerLoadState    $ P_PMC_PowerLoadState(p1)
#define OS_PMC_TRANSPORT_MODE_LEVEL(pPMC)         pPMC^.TransportModeLevel$ P_PMC_TransportModeLevel()
