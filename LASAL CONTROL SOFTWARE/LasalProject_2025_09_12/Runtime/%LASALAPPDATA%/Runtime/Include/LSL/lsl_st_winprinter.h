#pragma once

#ifndef __LSL_ST_WINPRINTER_H_
#define __LSL_ST_WINPRINTER_H_

#pragma pack (push,1)

TYPE

  LSL_WINPRINTER : STRUCT

    Version                     : UDINT;

    pStartJob                   : pvoid;
    pStartPage                  : pvoid;
    pSetData                    : pvoid;
    pEndPage                    : pvoid;
    pEndJob                     : pvoid;
    pPrinterReset               : pvoid;
    pPrinterReady               : pvoid;
    pGetPrinterStatus           : pvoid;

  END_STRUCT;

END_TYPE

#pragma pack (pop)


FUNCTION GLOBAL __CDECL P_StartJob
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_StartPage
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_SetData
VAR_INPUT
  pData           : pvoid;
  pWidth          : UINT;
  pHeight         : UINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_EndPage
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_EndJob
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PrinterReset
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PrinterReady
VAR_OUTPUT
  dRC             : BOOL;
END_VAR;

FUNCTION GLOBAL __CDECL P_GetPrinterStatus
VAR_OUTPUT
  dRC             : DINT;
END_VAR;


#define OS_WPVersion                   lsl_piWinPrinter^.udVersion

#define OS_WPStartJob()                lsl_piWinPrinter^.pStartJob $ P_StartJob()
#define OS_WPStartPage()               lsl_piWinPrinter^.pStartPage $ P_StartPage()
#define OS_WPSetData(p1,p2, p3)        lsl_piWinPrinter^.pSetData $ P_SetData(p1,p2,p3)
#define OS_WPEndPage()                 lsl_piWinPrinter^.pEndPage $ P_EndPage()
#define OS_WPEndJob()                  lsl_piWinPrinter^.pEndJob $ P_EndJob()
#define OS_WPPrinterReset()            lsl_piWinPrinter^.pPrinterReset $ P_PrinterReset()
#define OS_WPPrinterReady()            lsl_piWinPrinter^.pPrinterReady $ P_PrinterReady()
#define OS_WPGetPrinterStatus()        lsl_piWinPrinter^.pPrinterReady $ P_GetPrinterStatus()

#endif // __LSL_ST_WINPRINTER_H_
