#ifndef __LSL_C_PRINT
#pragma once
 #define __LSL_C_PRINT

unsigned long LslPrint_Register();


typedef struct 
{
    unsigned long (* pSetPrinterIOBase)(unsigned long port0, unsigned long IOBase0);
    unsigned long (* pPrinterInit)(unsigned long port0);
    unsigned long (* pPrinterStatus)(unsigned long port0);
    unsigned long (* pPrintByte)(unsigned long port0, unsigned long data0, unsigned long timeout0);

    //**HH
    // USB Printer support
    // Achtung:
    // Parameter "PrinterIndex" muss im bereich 0...3 liegen

    // liefert eine Zeiger auf eine String, der den Drucker beschreibt
    char *        (* pUSBPrinterInfo)(int PrinterIndex);

    // initialisiert Printer, liefert Status
    unsigned char (* pUSBPrinterInit)(int PrinterIndex);

    // liefert Status
    unsigned char (* pUSBPrinterStatus)(int PrinterIndex);

    // druckt Daten,liefert Status
    unsigned char (* pUSBPrintData)(int PrinterIndex, const void * Data, int Len, int Timeout);

    // beendet aktuellen Ausdruck, liefert Status
    unsigned char (* pUSBPrintCancel)(int PrinterIndex);

} LSL_PRINT;


#define Printer_Init       LSL_POS->piPrinter->pPrinterInit
#define Printer_SetIOBase  LSL_POS->piPrinter->pSetPrinterIOBase
#define Printer_Status     LSL_POS->piPrinter->pPrinterStatus
#define Printer_Byte       LSL_POS->piPrinter->pPrintByte

// Printer port status bits
#ifndef USB_PRN_OUT_OF_PAPER
  #define USB_PRN_OUT_OF_PAPER 0x20
#endif
#ifndef USB_PRN_SELECTED
  #define USB_PRN_SELECTED     0x10
#endif
#ifndef USB_PRN_IO_ERROR
  #define USB_PRN_IO_ERROR     0x08
#endif
#ifndef USB_PRN_TIMEOUT
  #define USB_PRN_TIMEOUT      0x01
#endif

// Funktionen
#define USBPrinter_Info    LSL_POS->piPrinter->pUSBPrinterInfo
#define USBPrinter_Init    LSL_POS->piPrinter->pUSBPrinterInit
#define USBPrinter_Status  LSL_POS->piPrinter->pUSBPrinterStatus
#define USBPrint_Data      LSL_POS->piPrinter->pUSBPrintData
#define USBPrint_Cancel    LSL_POS->piPrinter->pUSBPrintCancel


#endif //__LSL_C_PRINT
