// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Version     Date    Author     Comment
//     1.0     09042002   Menk      Creating this file
//     2.0     08072002   Menk      New Release Version 5.22, overall change
//
// ------------------------------------------------------------
#ifndef __LSL_IF_PANEL_H
#pragma once
#define __LSL_IF_PANEL_H

#define PANELLEDID_1   0x01
#define PANELLEDID_2   0x02
#define PANELLEDID_3   0x03
#define PANELLEDID_4   0x04
#define PANELLEDID_5   0x05
#define PANELLEDID_6   0x06
#define PANELLEDID_7   0x07
#define PANELLEDID_8   0x08

#define PANELLED_OFF   0x00
#define PANELLED_ON    0x01
#define PANELLED_BLINK 0x02

#define PANELKEY_F1    16#01000000
#define PANELKEY_F2    16#02000000
#define PANELKEY_F3    16#04000000
#define PANELKEY_F4    16#08000000
#define PANELKEY_F5    16#10000000
#define PANELKEY_F6    16#20000000
#define PANELKEY_F7    16#40000000
#define PANELKEY_F8    16#80000000
#define PANELKEY_DOWN  16#00001000
#define PANELKEY_UP    16#00080000
#define PANELKEY_LEFT  16#00002000
#define PANELKEY_RIGHT 16#00040000
#define PANELKEY_PLUS  16#00020000
#define PANELKEY_MINUS 16#00004000


////////////////////////////////////////////////////////////////////////////
//
//  P A N E L   Type Definitions
//
////////////////////////////////////////////////////////////////////////////
#pragma pack (push, 1)
TYPE
  PANEL_CONFIGTYPE               : STRUCT
        sizeoftype    : UDINT;
        version       : UDINT;
        plattform     : UDINT;
        IO_PORT       : UDINT;
        WD_PORT       : UDINT;
        charperline   : UDINT;
        linenumber    : UDINT;
        displaysize   : UDINT;
        keypuffersize : UDINT;
  END_STRUCT;

  PANEL_APITYPE                  : STRUCT
        IO_BitMap    : ^USINT;
        Watchdog     : pVoid;
        GetConfig    : pVoid;
        GetKey       : pVoid;
        SetLed       : pVoid;
        SetLedSpeed  : pVoid;
        WriteDirect  : pVoid;
        WriteXY      : pVoid;
        ReadXY       : pVoid;
        PtrToDisplay : pVoid;
        ClearDisplay : pVoid;
        SetContrast  : pVoid;
        GetADC       : pVoid;
  END_STRUCT;
END_TYPE
#pragma pack (pop)




////////////////////////////////////////////////////////////////////////////
//
//  P A N E L   Function Prototypes
//
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//  void          (*Watchdog)(void);
FUNCTION GLOBAL __cdecl P_PAN_WATCHDOG;
	
//--------------------------------------------------------------------------
//  unsigned long (*GetConfig)(PANEL_CONFIGTYPE * config0);
FUNCTION GLOBAL __cdecl P_PAN_GETCONFIG
VAR_INPUT
  config0  : ^PANEL_CONFIGTYPE;
END_VAR
VAR_OUTPUT 
  ret0     : UDINT;
END_VAR;

//--------------------------------------------------------------------------
//  unsigned long (*GetKey)(unsigned char *keybuf0);
FUNCTION GLOBAL __cdecl P_PAN_GETKEY
VAR_INPUT
  keybuf0  : ^UDINT;
END_VAR
VAR_OUTPUT 
  ret0     : UDINT;
END_VAR;

//--------------------------------------------------------------------------
//  long          (*SetLed)(unsigned long id0, unsigned long status0);
FUNCTION GLOBAL __cdecl P_PAN_SETLED
VAR_INPUT
  id0      : UDINT;
  status   : UDINT;
END_VAR
VAR_OUTPUT 
  ret0     : DINT;
END_VAR;

//--------------------------------------------------------------------------
//  void          (*SetLedSpeed)(unsigned long speed0);
FUNCTION GLOBAL __cdecl P_PAN_SETLEDSPEED
VAR_INPUT
  speed0   : UDINT;
END_VAR;

//--------------------------------------------------------------------------
//  void          (*WriteDirect)(char *buf0);
FUNCTION GLOBAL __cdecl P_PAN_WRITEDIRECT
VAR_INPUT
  text0    : ^CHAR;
END_VAR;

//--------------------------------------------------------------------------
//  long          (*WriteXY)(long x0, long y0, char *s0);
FUNCTION GLOBAL __cdecl P_PAN_WRITEXY
VAR_INPUT
  x0       : DINT;
  y0       : DINT;
  text0    : ^CHAR;
END_VAR
VAR_OUTPUT 
  ret0     : DINT;
END_VAR;

//--------------------------------------------------------------------------
//  unsigned long (*ReadXY)(long x0, long y0, char *s0, unsigned long len0);
FUNCTION GLOBAL __cdecl P_PAN_READXY
VAR_INPUT
  x0       : DINT;
  y0       : DINT;
  text0    : ^CHAR;
  rdlen0   : UDINT;
END_VAR
VAR_OUTPUT 
  ret0     : UDINT;
END_VAR;

//--------------------------------------------------------------------------
//  char         *(*PtrToDisplay)(void);
FUNCTION GLOBAL __cdecl P_PAN_PTRTODISPLAY
VAR_OUTPUT 
  ret0     : ^USINT;
END_VAR;

//--------------------------------------------------------------------------
//  void          (*ClearDisplay)(unsigned long chr0);
FUNCTION GLOBAL __cdecl P_PAN_CLEARDISPLAY
VAR_INPUT
  fillchar0: UDINT;
END_VAR;

//--------------------------------------------------------------------------
//  void          (*SetContrast)(long dir0);
FUNCTION GLOBAL __cdecl P_PAN_SetContrast
VAR_INPUT
  dir0: DINT;
END_VAR;

//--------------------------------------------------------------------------
//  int         (*GetADC)(unsigned int);
FUNCTION GLOBAL __cdecl P_PAN_GetADC
VAR_INPUT
  dir0: UDINT;
END_VAR
VAR_OUTPUT 
  ret0: DINT;
END_VAR;


////////////////////////////////////////////////////////////////////////////
//
//  P A N E L   Interface Macros
//
////////////////////////////////////////////////////////////////////////////

VAR_PRIVATE
  _PANEL_API : ^PANEL_APITYPE;
END_VAR


#define OSPANEL_WATCHDOG            _PANEL_API^.WATCHDOG     $ P_PAN_WATCHDOG
#define OSPANEL_GETCONFIG(p1)       _PANEL_API^.GETCONFIG    $ P_PAN_GETCONFIG(p1)
#define OSPANEL_GETKEY(p1)          _PANEL_API^.GETKEY       $ P_PAN_GETKEY(p1)
#define OSPANEL_SETLED(p1,p2)       _PANEL_API^.SETLED       $ P_PAN_SETLED(p1, p2)
#define OSPANEL_SETLEDSPEED(p1)     _PANEL_API^.SETLEDSPEED  $ P_PAN_SETLEDSPEED(p1)
#define OSPANEL_WRITEDIRECT(p1)     _PANEL_API^.WRITEDIRECT  $ P_PAN_WRITEDIRECT(p1)
#define OSPANEL_WRITEXY(p1,p2,p3)   _PANEL_API^.WRITEXY      $ P_PAN_WRITEXY(p1, p2, p3)
#define OSPANEL_READXY(p1,p2,p3,p4) _PANEL_API^.READXY       $ P_PAN_READXY(p1, p2, p3 p4)
#define OSPANEL_PTRTODISPLAY        _PANEL_API^.PTRTODISPLAY $ P_PAN_PTRTODISPLAY
#define OSPANEL_CLEARDISPLAY(p1)    _PANEL_API^.CLEARDISPLAY $ P_PAN_CLEARDISPLAY(p1)
#define OSPANEL_SETCONTRAST(p1)     _PANEL_API^.SETCONTRAST  $ P_PAN_SETCONTRAST(p1)
#define OSPANEL_GETADC(p1)          _PANEL_API^.GETADC       $ P_PAN_GETADC(p1)






#endif // __LSL_IF_PANEL_H
