(****************************************************************************************)
(*                                                                                      *)
(* lsl_st_keypad.h                                                                      *)
(*                                                                                      *)
(* enthält die Definitionen für das Keypad                                              *)
(*                                                                                      *)
(* HH                                                                                   *)
(*                                                                                      *)
(*                                                                                      *)
(****************************************************************************************)
#ifndef __LSL_ST_SKEYPAD
#pragma once
#define __LSL_ST_SKEYPAD



//
// Interface
//
TYPE
  LSL_SKEYPAD : STRUCT

  //
  // Version
  //
  udVersion : UDINT;

  //
  // Timer
  //

  // Hole Tastendruck
  GetKeyCode : pVoid;

  // Setze Delay
  SetKeyDelay : pVoid;

  // Setze repeat rate in CPS (character per second), 1...25
  SetKeyCPS : pVoid;

  // Setze Tabelle
  SetKeyTable : pVoid;

  // Setze Modus
  SetKeyMode : pVoid;

  // Setze Tabelle mit Character Codes
  SetKeyTableCharCode : pVoid;

  // setze Callback
  SetKeyCallback : pVoid;

// since LSL_SKEYPAD version 0x00001021

  // setze Callback, mit dem die Applikation über die Verarbeitung des 
  // Events über die Standard-Queue entscheiden kann
  SetKeyCallbackFiltered : pVoid;

  END_STRUCT;
END_TYPE


// values used by the callback function which is set with SetKeyCallbackFiltered
// scancode for shuttle increment, increment value is set in the parameter udAction
#define KEYPAD_SCANCODE_SHUTTLE_INC     16#10000
// scancode for shuttle decrement, decrement value is set in the parameter udAction
#define KEYPAD_SCANCODE_SHUTTLE_DEC     16#10001
// action code for key pressed event
#define KEYPAD_ACTIONCODE_KEYPRESS      1
// action code for key released event
#define KEYPAD_ACTIONCODE_KEYRELEASE    0

// GetKeyCode
FUNCTION __CDECL GLOBAL P_GetKeyCode
VAR_INPUT
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define KEYPAD_GETKEYCODE()  pSKeyPad^.GetKeyCode $ P_GetKeyCode()


// SetKeyDelay
FUNCTION __CDECL GLOBAL P_SetKeyDelay
VAR_INPUT
  ulDelay   : UDINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define KEYPAD_SETKEYDELAY(p1)  pSKeyPad^.SetKeyDelay $ P_SetKeyDelay(p1)


// SetKeyCPS
FUNCTION __CDECL GLOBAL P_SetKeyCPS
VAR_INPUT
  ulCPS     : UDINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define KEYPAD_SETKEYCPS(p1)  pSKeyPad^.SetKeyCPS $ P_SetKeyCPS(p1)


// SetKeyTable
FUNCTION __CDECL GLOBAL P_SetKeyTable
VAR_INPUT
  puiTab    : ^UDINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define KEYPAD_SETKEYTABLE(p1)  pSKeyPad^.SetKeyTable $ P_SetKeyTable(p1)


// SetKeyMode
FUNCTION __CDECL GLOBAL P_SetKeyMode
VAR_INPUT
  uiKey     : UDINT;
  ucMode    : USINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define KEYPAD_SETKEYMODE(p1,p2)  pSKeyPad^.SetKeyMode $ P_SetKeyMode(p1,p2)


// SetKeyTableCharCode
FUNCTION __CDECL GLOBAL P_SetKeyTableCharCode
VAR_INPUT
  puiTab    : ^UINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define KEYPAD_SETKEYTABLECHARCODE(p1)  pSKeyPad^.SetKeyTableCharCode $ P_SetKeyTableCharCode(p1)


// SetKeyCallback
FUNCTION __CDECL GLOBAL P_SetKeyCallback
VAR_INPUT
  uiFnc  : UDINT;
  uiThis : UDINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define KEYPAD_SETKEYCALLBACK(p1,p2)  pSKeyPad^.SetKeyCallback $ P_SetKeyCallback(p1,p2)


// SetKeyCallbackFiltered
(* example of a correct declaration of the callback function which is set by SetKeyCallbackFiltered
FUNCTION GLOBAL __CDECL Callback
VAR_INPUT
  pUserPara   : pVoid;
  udScanCode  : UDINT;
  udAction    : UDINT;
END_VAR
VAR_OUTPUT
  retval      : DINT;  
END_VAR
*)
FUNCTION __CDECL GLOBAL P_SetKeyCallbackFiltered
VAR_INPUT
  pUserFct    : ^VOID;
  pUserParam  : ^VOID;
END_VAR
VAR_OUTPUT
  retval      : DINT;
END_VAR;
#define KEYPAD_SETKEYCALLBACKFILTERED(p1,p2)  pSKeyPad^.SetKeyCallbackFiltered $ P_SetKeyCallbackFiltered(p1,p2)

#endif
