(****************************************************************************************)
(*                                                                                      *)
(* lsl_st_xtimer.h                                                                      *)
(*                                                                                      *)
(* enthält die Definitionen für den Timer                                               *)
(*                                                                                      *)
(* HH                                                                                   *)
(*                                                                                      *)
(*                                                                                      *)
(****************************************************************************************)
#ifndef __LSL_ST_SXTIMER
#pragma once
#define __LSL_ST_SXTIMER

// mögliche Modi des Timers (zu setzen über XTimerSetMode)

// einmaliger Start des Timers
#define SXTIMER_SINGLE_RUN   1
// wiederholtes Starten des Timers
#define SXTIMER_CONT_RUN     3

//
// Interface
//
TYPE
  LSL_SXTIMER : STRUCT

  //
  // Version
  //
  udVersion : UDINT;

  //
  // Timer
  //

  // Initialisiere Timer
  XTimerInit : pVoid;

  // Setze Intervall
  XTimerSetInterval : pVoid;

  // Starte Timer
  XTimerStart : pVoid;

  // Stoppe Timer
  XTimerStop : pVoid;

  // Reset Timer
  XTimerReset : pVoid;

  // Hole aktuellen Wert
  XTimerValue : pVoid;

  // intern
  Reserved1 : pVoid;

  // Stoppe alle Timer
  XTimerStopAllUser : pVoid;

  // ab Interface Version 01.01.001

  // Modus festlegen (Single oder Continuous run)
  XTimerSetMode : pVoid;

  END_STRUCT;
END_TYPE




// XTimerInit
FUNCTION __CDECL GLOBAL P_XTimerInit
VAR_INPUT
  ulNum     : UDINT;
  ulVal     : UDINT;
  pThis     : ^VOID;
  pCallback : ^VOID;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define SXTIMER_XTIMERINIT(p1,p2,p3,p4)  pSXTimerInfo^.XTimerInit $ P_XTimerInit(p1,p2,p3,p4)


// XTimerSetInterval
FUNCTION __CDECL GLOBAL P_XTimerSetInterval
VAR_INPUT
  ulNum     : UDINT;
  ulVal     : UDINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define SXTIMER_XTIMERSETINTERVAL(p1,p2)  pSXTimerInfo^.XTimerSetInterval $ P_XTimerSetInterval(p1,p2)


// XTimerStart
FUNCTION __CDECL GLOBAL P_XTimerStart
VAR_INPUT
  ulNum     : UDINT;
END_VAR
VAR_OUTPUT
END_VAR;
#define SXTIMER_XTIMERSTART(p1)  pSXTimerInfo^.XTimerStart $ P_XTimerStart(p1)


// XTimerStop
FUNCTION __CDECL GLOBAL P_XTimerStop
VAR_INPUT
  ulNum     : UDINT;
END_VAR
VAR_OUTPUT
END_VAR;
#define SXTIMER_XTIMERSTOP(p1)  pSXTimerInfo^.XTimerStop $ P_XTimerStop(p1)


// XTimerReset
FUNCTION __CDECL GLOBAL P_XTimerReset
VAR_INPUT
  ulNum     : UDINT;
END_VAR
VAR_OUTPUT
END_VAR;
#define SXTIMER_XTIMERRESET(p1)  pSXTimerInfo^.XTimerReset $ P_XTimerReset(p1)


// XTimerValue
FUNCTION __CDECL GLOBAL P_XTimerValue
VAR_INPUT
  ulNum     : UDINT;
END_VAR
VAR_OUTPUT
  Value : UDINT;
END_VAR;
#define SXTIMER_XTIMERVALUE(p1)  pSXTimerInfo^.XTimerValue $ P_XTimerValue(p1)


// XTimerStopAllUser
FUNCTION __CDECL GLOBAL P_XTimerStopAllUser
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR;
#define SXTIMER_XTIMERSTOPALLUSER()  pSXTimerInfo^.XTimerStopAllUser $ P_XTimerStopAllUser()


// XTimerSetMode
FUNCTION __CDECL GLOBAL P_XTimerSetMode
VAR_INPUT
  ulNum     : UDINT;
  usMode    : USINT;
END_VAR
VAR_OUTPUT
  retval    : DINT;
END_VAR;
#define SXTIMER_XTIMERSETMODE(p1,p2)  pSXTimerInfo^.XTimerSetMode $ P_XTimerSetMode(p1,p2)




//
// Interface
//
TYPE
  LSL_CDIASWATCHDOG : STRUCT

  //
  // Version
  //
  udVersion : UDINT;

  //
  // CDias Watchdog
  //

  // Setze Interval
  CDWSetInterval : pVoid;

  // Starte Watchdog
  CDWStart : pVoid;

  // Stoppe Watchdog
  CDWStop : pVoid;

  // triggere Watchdog
  CDWTrigger : pVoid;

  END_STRUCT;
END_TYPE


// CDWSetInterval
FUNCTION __CDECL GLOBAL P_CDWSetInterval
VAR_INPUT
  ulVal     : UDINT;
END_VAR
VAR_OUTPUT
END_VAR;
#define SCDW_CDWSETINTERVAL(p1)  pSCDW^.CDWSetInterval $ P_CDWSetInterval(p1)


// CDWStart
FUNCTION __CDECL GLOBAL P_CDWStart
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR;
#define SCDW_CDWSTART()  pSCDW^.CDWStart $ P_CDWStart()


// CDWStop
FUNCTION __CDECL GLOBAL P_CDWStop
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR;
#define SCDW_CDWSTOP()  pSCDW^.CDWStop $ P_CDWStop()


// CDWTrigger
FUNCTION __CDECL GLOBAL P_CDWTrigger
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR;
#define SCDW_CDWTRIGGER()  pSCDW^.CDWTrigger $ P_CDWTrigger()


#endif
