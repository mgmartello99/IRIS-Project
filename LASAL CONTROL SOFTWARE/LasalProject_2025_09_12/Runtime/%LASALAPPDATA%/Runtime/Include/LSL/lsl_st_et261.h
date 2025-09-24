#ifndef _LSL_ST_ET261_H
#pragma once
#define _LSL_ST_ET261_H

#pragma pack( push, 1 )
TYPE
    OS_ET261_INTERFACETYPE : STRUCT
        ShuttleKeyVersion        : UDINT;
        _SetRepeatKey        : pVoid;
        _SetShuttle       : pVoid;
        _SetShuttleEnabled      : pVoid;
        _GetShuttleWheel : pVoid;
        _SetLed     : pVoid;
        _Pfeiferl     : pVoid;
    END_STRUCT;
END_TYPE
#pragma pack(pop)

VAR_PRIVATE
	OS_pET261 : ^OS_ET261_INTERFACETYPE;
END_VAR

//
// SetRepeatKey
//
FUNCTION GLOBAL __cdecl P_OS_ET261_SetRepeatKey
VAR_INPUT
    pressdelayms0     : UDINT;
    charpersec0   : UDINT;
END_VAR;

//
// SetShuttle
//
FUNCTION GLOBAL __cdecl P_OS_ET261_SetShuttle
VAR_INPUT
    eventspersecond0     : UDINT;
    releasetimeout0   : UDINT;
END_VAR;

//
// SetShuttleEnabled
//
FUNCTION GLOBAL __cdecl P_OS_ET261_SetShuttleEnabled
VAR_INPUT
    enabled0     : UDINT;
END_VAR
VAR_OUTPUT
    result : UDINT;
END_VAR;

//
// GetShuttleWheel
//
FUNCTION GLOBAL __cdecl P_OS_ET261_GetShuttleWheel
VAR_OUTPUT
    result : DINT;
END_VAR;

//
// SetLed
//
FUNCTION GLOBAL __cdecl P_OS_ET261_SetLed
VAR_INPUT
    ledcode0     : UDINT;
    op0   : UDINT;
END_VAR
VAR_OUTPUT
    result : DINT;
END_VAR;

//
// Pfeiferl
//
FUNCTION GLOBAL __cdecl P_OS_ET261_Pfeiferl
VAR_INPUT
    delayms0     : UDINT;
END_VAR;


#define OS_ET261_SetRepeatKey(p1,p2)    OS_pET261^._SetRepeatKey      $ P_OS_ET261_SetRepeatKey(p1,p2)
#define OS_ET261_SetShuttle(p1,p2)      OS_pET261^._SetShuttle        $ P_OS_ET261_SetShuttle(p1,p2)
#define OS_ET261_SetShuttleEnabled(p1)  OS_pET261^._SetShuttleEnabled $ P_OS_ET261_SetShuttleEnabled(p1)
#define OS_ET261_GetShuttleWheel()      OS_pET261^._GetShuttleWheel   $ P_OS_ET261_GetShuttleWheel()
#define OS_ET261_SetLed(p1,p2)          OS_pET261^._SetLed            $ P_OS_ET261_SetLed(p1,p2)
#define OS_ET261_Pfeiferl(p1)           OS_pET261^._Pfeiferl          $ P_OS_ET261_Pfeiferl(p1)

#endif // _LSL_ST_ET261_H
