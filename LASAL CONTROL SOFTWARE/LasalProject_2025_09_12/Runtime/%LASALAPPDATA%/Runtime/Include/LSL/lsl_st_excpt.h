//****************************************************************************/
//*                                                                          */
//*  File: LSL_ST_EXCPT.H                                                    */
//*  Date: JAN 17, 2005                                                      */
//*  Description:                                                            */
//*		This file contains definitions for the LASAL OS exception handler      */
//*                                                                          */
//****************************************************************************/
//* $$!ENDCMD */
#ifndef LSL_ST_EXCPT_H
#pragma once
#define LSL_ST_EXCPT_H

// Handler Typen - für Funktion OS_SSR_SetHandler
#define HANDLERTYPE_EXCEPTION   0

#define EXCEPTION_ACCESS_VIOLATION          16#C0000005
#define EXCEPTION_INT_DIVIDE_BY_ZERO        16#C0000094
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        16#C000008E
#define EXCEPTION_ARRAY_OUT_OF_BOUND        16#00000011

// possible return codes from the exception handler
#define EXCEPTION_OSHANDLER                 0
#define EXCEPTION_CONTINUE_EXECUTION       -1
#define EXCEPTION_SKIP_CONTINUE             1

TYPE

  FLOATING_SAVE_AREA : STRUCT

    ControlWord   : UDINT;
    StatusWord    : UDINT;
    TagWord       : UDINT;
    ErrorOffset   : UDINT;
    ErrorSelector : UDINT;
    DataOffset    : UDINT;
    DataSelector  : UDINT;
#define SIZE_OF_80387_REGISTERS      80
    RegisterArea  : ARRAY[0..SIZE_OF_80387_REGISTERS-1] OF USINT;
    Cr0NpxState   : UDINT;

  END_STRUCT;

  CONTEXT86 : STRUCT

    ContextFlags : UDINT;

    rDr0 : UDINT;
    rDr1 : UDINT;
    rDr2 : UDINT;
    rDr3 : UDINT;
    rDr6 : UDINT;
    rDr7 : UDINT;

    FloatSave : FLOATING_SAVE_AREA;

    rSegGs : UDINT;
    rSegFs : UDINT;
    rSegEs : UDINT;
    rSegDs : UDINT;

    rEdi : UDINT;
    rEsi : UDINT;
    rEbx : UDINT;
    rEdx : UDINT;
    rEcx : UDINT;
    rEax : UDINT;

    rEbp    : UDINT;
    rEip    : UDINT;
    rSegCs  : UDINT;
    rEFlags : UDINT;
    rEsp    : UDINT;
    rSegSs  : UDINT;

  END_STRUCT;

  EXCEPTION_POINTERS : STRUCT

    ExceptionRecord : pVoid;
    ContextRecord   : ^CONTEXT86;

  END_STRUCT;

END_TYPE

#endif  // end of #define LSL_ST_EXCPT_H
