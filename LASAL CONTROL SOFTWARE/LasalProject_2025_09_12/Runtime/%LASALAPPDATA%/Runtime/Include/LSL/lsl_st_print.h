#ifndef __LSL_ST_PRINT
#pragma once
 #define __LSL_ST_PRINT


// define ST prototypes for API

//unsigned long (* pSetPrinterIOBase)(unsigned long port0, unsigned long IOBase0);
FUNCTION GLOBAL __cdecl P_Print_SetIOBase
VAR_INPUT
    port0    : UDINT;
    IOBase0  : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : UDINT;
END_VAR;

//unsigned long (* pPrinterInit)(unsigned long port0);
FUNCTION GLOBAL __cdecl P_Print_Init
VAR_INPUT
    port0    : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : UDINT;
END_VAR;

//unsigned long (* pPrinterStatus)(unsigned long port0);
FUNCTION GLOBAL __cdecl P_Print_Status
VAR_INPUT
    port0    : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : UDINT;
END_VAR;

//unsigned long (* pPrintByte)(unsigned long port0, unsigned long data0, unsigned long timeout0);
FUNCTION GLOBAL __cdecl P_Print_Byte
VAR_INPUT
    port0    : UDINT;
    data0    : UDINT;
    timeout0 : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : UDINT;
END_VAR;


//
// USB Printer support
//

//char *        (* pUSBPrinterInfo)(int PrinterIndex);
FUNCTION GLOBAL __cdecl P_USBPrinter_Info
VAR_INPUT
    PrinterIndex: DINT;
END_VAR
VAR_OUTPUT 
    ret0        : ^USINT;
END_VAR;

//unsigned char (* pUSBPrinterInit)(int PrinterIndex);
FUNCTION GLOBAL __cdecl P_USBPrinter_Init
VAR_INPUT
    PrinterIndex: DINT;
END_VAR
VAR_OUTPUT 
    ret0        : USINT;
END_VAR;

//unsigned char (* pUSBPrinterStatus)(int PrinterIndex);
FUNCTION GLOBAL __cdecl P_USBPrinter_Status
VAR_INPUT
    PrinterIndex: DINT;
END_VAR
VAR_OUTPUT 
    ret0        : USINT;
END_VAR;

//unsigned char (* pUSBPrintData)(int PrinterIndex, const void * Data, int Len, int Timeout);
FUNCTION GLOBAL __cdecl P_USBPrint_Data
VAR_INPUT
    PrinterIndex: DINT;
    Data        : PVOID;
    Len         : DINT;
    Timeout     : DINT;
END_VAR
VAR_OUTPUT 
    ret0        : USINT;
END_VAR;

//unsigned char (* pUSBPrintCancel)(int PrinterIndex);
FUNCTION GLOBAL __cdecl P_USBPrint_Cancel
VAR_INPUT
    PrinterIndex: DINT;
END_VAR
VAR_OUTPUT 
    ret0        : USINT;
END_VAR;



//
//    Printer API interface Macros
//
#define Printer_SetIOBase(p1)  _LSL_POS^.piPrinter^.pSetPrinterIOBase $ P_Print_SetIOBase(p1)
#define Printer_Init(p1)       _LSL_POS^.piPrinter^.pPrinterInit $ P_Print_Init(p1)
#define Printer_Status(p1)     _LSL_POS^.piPrinter^.pPrinterStatus $ P_Print_Status(p1)
#define Printer_Byte(p1,p2,p3) _LSL_POS^.piPrinter^.pPrintByte $ P_Print_Byte(p1,p2,p3)

//
//    USB Printer API interface Macros
//
#define USBPrinter_Info(p1)         _LSL_POS^.piPrinter^.pUSBPrinterInfo $ P_USBPrinter_Info(p1)
#define USBPrinter_Init(p1)         _LSL_POS^.piPrinter^.pUSBPrinterInit $ P_USBPrinter_Init(p1)
#define USBPrinter_Status(p1)       _LSL_POS^.piPrinter^.pUSBPrinterStatus $ P_USBPrinter_Status(p1)
#define USBPrint_Data(p1,p2,p3,p4)  _LSL_POS^.piPrinter^.pUSBPrintData $ P_USBPrint_Data(p1,p2,p3,p4)
#define USBPrint_Cancel(p1)         _LSL_POS^.piPrinter^.pUSBPrintCancel $ P_USBPrint_Cancel(p1)


#endif //__LSL_ST_PRINT
