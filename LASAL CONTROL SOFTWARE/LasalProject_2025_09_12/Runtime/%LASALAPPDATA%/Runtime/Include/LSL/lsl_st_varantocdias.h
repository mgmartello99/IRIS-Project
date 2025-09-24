(****************************************************************************************)
(*                                                                                      *)
(* lsl_st_varan.h                                                                       *)
(*                                                                                      *)
(* enthält die Definitionen für das VARAN Interface                                     *)
(*                                                                                      *)
(* HH                                                                                   *)
(*                                                                                      *)
(*                                                                                      *)
(****************************************************************************************)
#ifndef __LSL_ST_VARANTOCDIAS
#pragma once
#define __LSL_ST_VARANTOCDIAS



//
// Defines
//

// Interface
#define INTERFACE_VARANTOCDIAS                "IVARANTOCDIAS"

// Ergebniscodes
#define VARANTOCDIAS_OK                       0



//
// Interface
//
TYPE
  LSL_VARANTOCDIAS : STRUCT

  //
  // Version
  //
  udVersion : UDINT;

  //
  // Funcs
  //

  // Init I2C
  iInitI2C  : pVoid;

  // Exit I2C
  iExitI2C  : pVoid;

  // Read I2C
  iReadI2C  : pVoid;

  // Write I2C
  iWriteI2C  : pVoid;

  END_STRUCT;
END_TYPE

  

// iInitI2C
FUNCTION __CDECL GLOBAL iInitI2C
VAR_INPUT
  uiMaster  : UDINT;
  pucNode   : ^USINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define VARANTOCDIAS_iInitI2C(p1,p2)  pVaranToCDIAS^.iInitI2C $ iInitI2C(p1,p2)



// iExitI2C
FUNCTION __CDECL GLOBAL iExitI2C
VAR_INPUT
  uiMaster  : UDINT;
  pucNode   : ^USINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define VARANTOCDIAS_iExitI2C(p1,p2)  pVaranToCDIAS^.iExitI2C $ iExitI2C(p1,p2)



// iReadCDIASEEprom
FUNCTION __CDECL GLOBAL iReadI2C
VAR_INPUT
  uiMaster  : UDINT;
  pucNode   : ^USINT;
  uiStation : UDINT;
  uiOffset  : UDINT;
  uiLen     : UDINT;
  pucData   : ^USINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define VARANTOCDIAS_iReadI2C(p1,p2,p3,p4,p5,p6)  pVaranToCDIAS^.iReadI2C $ iReadI2C(p1,p2,p3,p4,p5,p6)



// iWriteCDIASEEprom: "uiOffset" von 16#B0 - 16#FF
FUNCTION __CDECL GLOBAL iWriteI2C
VAR_INPUT
  uiMaster  : UDINT;
  pucNode   : ^USINT;
  uiStation : UDINT;
  uiOffset  : UDINT;
  uiLen     : UDINT;
  pucData   : ^USINT;
END_VAR
VAR_OUTPUT
    retval  : DINT;
END_VAR;
#define VARANTOCDIAS_iWriteI2C(p1,p2,p3,p4,p5,p6)  pVaranToCDIAS^.iWriteI2C $ iWriteI2C(p1,p2,p3,p4,p5,p6)



#endif
