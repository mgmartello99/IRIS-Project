// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Version     Date    Author     Comment
//     1.0     04092002   Menk      Creating this file
//
// ------------------------------------------------------------

#pragma once
#define __LSL_IF_ET261_H




#pragma pack (push, 1)
typedef struct
{
   unsigned long ShuttleKeyVersion;
   void (*_SetRepeatKey)(unsigned long pressdelayms0, unsigned long charpersec0);
   void (*_SetShuttle)(unsigned long eventspersecond0, unsigned long releasetimeout0);
   unsigned long (*_SetShuttleEnabled)(unsigned long enabled0);
   long (*_GetShuttleWheel)(void);
   long (*_SetLed)(unsigned long ledcode0, unsigned long op0);
   void (*_Pfeiferl)(unsigned long delayms0);
}ET261_INTERFACETYPE;

#pragma pack (pop)

