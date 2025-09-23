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

#pragma pack (push, 1)
typedef struct
{
  unsigned long  sizeoftype;
  unsigned long  version;
  unsigned long  plattform;
  unsigned long  IO_PORT;
  unsigned long  WD_PORT;
  unsigned long  charperline;
  unsigned long  linenumber;
  unsigned long  displaysize;
  unsigned long  keypuffersize;
}PANEL_CONFIGTYPE;

typedef struct
{
  unsigned char *IO_BitMap;
  void          (*Watchdog)(void);
  unsigned long (*GetConfig)(PANEL_CONFIGTYPE * config0);
  unsigned long (*GetKey)(unsigned char *keybuf0);
  long          (*SetLed)(unsigned long id0, unsigned long status0);
  void          (*SetLedSpeed)(unsigned long speed0);
  void          (*WriteDirect)(char *buf0);
  long          (*WriteXY)(long x0, long y0, char *s0);
  unsigned long (*ReadXY)(long x0, long y0, char *s0, unsigned long len0);
  char         *(*PtrToDisplay)(void);
  void          (*ClearDisplay)(unsigned long chr0);
  void          (*SetContrast)(long dir0);
  int           (*GetADC)( unsigned int iChannel );
  void          (*pReset_AO)(void);
}PANEL_APITYPE;

#pragma pack (pop)

#endif // __LSL_IF_PANEL_H
