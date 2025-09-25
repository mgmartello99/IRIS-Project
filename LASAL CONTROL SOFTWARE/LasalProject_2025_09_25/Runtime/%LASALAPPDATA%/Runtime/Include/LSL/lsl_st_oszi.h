//HFILTER:1
#ifndef _LSL_ST_OSZI_H
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define _LSL_ST_OSZI_H

//
// Defines
//

// Ergebniscodes
#define LSLOSZI_OK                 0  
#define LSLOSZI_ERROR             -1
#define LSLOSZI_CHANNEL_NOTFOUND  -2
#define LSLOSZI_CHANNEL_CORRUPT   -3

//
// Status
//
#define LSLOSZI_STOPPED           0
#define LSLOSZI_WAIT_FOR_TRIGGER  1
#define LSLOSZI_RUNNING           2


//
// Oszi Konstanten
//

// Channel managment
// ChannelType
#define LSLOS_OSZI_CHANNEL_SERVER         0
#define LSLOS_OSZI_CHANNEL_CLIENT         1
#define LSLOS_OSZI_CHANNEL_MEM_1          2
#define LSLOS_OSZI_CHANNEL_MEM_2          3
#define LSLOS_OSZI_CHANNEL_MEM_4          4
#define LSLOS_OSZI_CHANNEL_SERVER_DD      5
#define LSLOS_OSZI_CHANNEL_CLIENT_DD      6
#define LSLOS_OSZI_CHANNEL_MEM_8          7
// ChannelValueType
#define LSLOS_OSZI_CHANNEL_VALUE_UNSIGNED 0
#define LSLOS_OSZI_CHANNEL_VALUE_SIGNED   1
#define LSLOS_OSZI_CHANNEL_VALUE_REAL     2

// Trigger
#define LSLOS_OSZI_TRIGGER_START        0
#define LSLOS_OSZI_TRIGGER_STOP         1
#define LSLOS_OSZI_TRIGGER_COND_EQ      0
#define LSLOS_OSZI_TRIGGER_COND_NEQ     1
#define LSLOS_OSZI_TRIGGER_COND_GT      2
#define LSLOS_OSZI_TRIGGER_COND_GTE     3
#define LSLOS_OSZI_TRIGGER_COND_LT      4
#define LSLOS_OSZI_TRIGGER_COND_LTE     5
#define LSLOS_OSZI_TRIGGER_COND_RISING  6
#define LSLOS_OSZI_TRIGGER_COND_FALLING 7
#define LSLOS_OSZI_TRIGGER_COND_LOW     8
#define LSLOS_OSZI_TRIGGER_COND_HIGH    9

TYPE
    LSL_OSZI    :STRUCT
        udVersion                 : UDINT;
        LslOsziAddChannel         : pVoid;
        LslOsziReplaceChannel     : pVoid;
        LslOsziRemoveAllChannels  : pVoid;
        LslOsziGetChannels        : pVoid;
        LslOsziGetStatus          : pVoid;
        LslOsziSetBufferSize      : pVoid;
        LslOsziResetBuffer        : pVoid;
        LslOsziIsBufferEmpty      : pVoid;
        LslOsziGetUsedBufferSize  : pVoid;
        LslOsziGetUsedBuffer      : pVoid;
        LslOsziStart              : pVoid;
        LslOsziStop               : pVoid;
        LslOsziSetSampleRate      : pVoid;
        LslOsziGetSampleRate      : pVoid;
        LslOsziSetTrigger         : pVoid;
        LslOsziClearTrigger       : pVoid;
        LslOsziSetCRC             : pVoid;
        LslOsziGetCRC             : pVoid;
        LslOsziInit               : pVoid;
    END_STRUCT;
END_TYPE

FUNCTION GLOBAL __cdecl P_OSZI_ADDCHANNEL
VAR_INPUT
    uiChannelType  : UDINT;
    uiChannelValueType  : UDINT;
    pChannelDescriptor  : pVoid;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_REPLACECHANNEL
VAR_INPUT
    uiChannel  : UDINT;
    uiChannelType  : UDINT;
    uiChannelValueType  : UDINT;
    pChannelDescriptor : pVoid;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_REMOVEALLCHANNELS
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_GETCHANNELS
VAR_INPUT
    puiChannels  : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_GETSTATUS
VAR_INPUT
    puiStatus  : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_SETBUFFERSIZE
VAR_INPUT
    uiBufferSize  : UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_RESETBUFFER
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_ISBUFFEREMPTY
VAR_INPUT
    pbEmpty  : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_GETUSEDBUFFERSIZE
VAR_INPUT
    puiUsedBufferSize   : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_GETUSEDBUFFER
VAR_INPUT
    uiMaxSize  : UDINT;
    pBuffer : pVoid;
    puiLen  : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_START
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_STOP
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_SETSAMPLERATE
VAR_INPUT
    uiSampleRateMuSec  : UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_GETSAMPLERATE
VAR_INPUT
    puiSampleRate   : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_SETTRIGGER
VAR_INPUT
    uiTriggerType  : UDINT;
    uiChannel  : UDINT;
    uiTriggerCond  : UDINT;
    pValue : pVoid;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_CLEARTRIGGER
VAR_INPUT
    uiTriggerType  : UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_SETCRC
VAR_INPUT
    uiLasalCRC  : UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_GETCRC
VAR_INPUT
    puiCRC   : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_OSZI_INIT
VAR_OUTPUT
    retval      : DINT;
END_VAR;


#define OS_OSZI_VERSION lsl_oszi^.udVersion

#define OS_OSZI_ADDCHANNEL(p1,p2,p3) lsl_oszi^.LslOsziAddChannel $ P_OSZI_ADDCHANNEL(p1,p2,p3)
#define OS_OSZI_REPLACECHANNEL(p1,p2,p3,p4) lsl_oszi^.LslOsziReplaceChannel $ P_OSZI_REPLACECHANNEL(p1,p2,p3,p4)
#define OS_OSZI_REMOVEALLCHANNELS() lsl_oszi^.LslOsziRemoveAllChannels $ P_OSZI_REMOVEALLCHANNELS()
#define OS_OSZI_GETCHANNELS(p1) lsl_oszi^.LslOsziGetChannels $ P_OSZI_GETCHANNELS(p1)
#define OS_OSZI_GETSTATUS(p1) lsl_oszi^.LslOsziGetStatus $ P_OSZI_GETSTATUS(p1)
#define OS_OSZI_SETBUFFERSIZE(p1) lsl_oszi^.LslOsziSetBufferSize $ P_OSZI_SETBUFFERSIZE(p1)
#define OS_OSZI_RESETBUFFER() lsl_oszi^.LslOsziResetBuffer $ P_OSZI_RESETBUFFER()
#define OS_OSZI_ISBUFFEREMPTY(p1) lsl_oszi^.LslOsziIsBufferEmpty $ P_OSZI_ISBUFFEREMPTY(p1)
#define OS_OSZI_GETUSEDBUFFERSIZE(p1) lsl_oszi^.LslOsziGetUsedBufferSize $ P_OSZI_GETUSEDBUFFERSIZE(p1)
#define OS_OSZI_GETUSEDBUFFER(p1,p2,p3) lsl_oszi^.LslOsziGetUsedBuffer $ P_OSZI_GETUSEDBUFFER(p1,p2,p3)
#define OS_OSZI_START() lsl_oszi^.LslOsziStart $ P_OSZI_START()
#define OS_OSZI_STOP() lsl_oszi^.LslOsziStop $ P_OSZI_STOP()
#define OS_OSZI_SETSAMPLERATE(p1) lsl_oszi^.LslOsziSetSampleRate $ P_OSZI_SETSAMPLERATE(p1)
#define OS_OSZI_GETSAMPLERATE(p1) lsl_oszi^.LslOsziGetSampleRate $ P_OSZI_GETSAMPLERATE(p1)
#define OS_OSZI_SETTRIGGER(p1, p2, p3,p4) lsl_oszi^.LslOsziSetTrigger $ P_OSZI_SETTRIGGER(p1, p2, p3,p4)
#define OS_OSZI_CLEARTRIGGER(p1) lsl_oszi^.LslOsziClearTrigger $ P_OSZI_CLEARTRIGGER(p1)
#define OS_OSZI_SETCRC(p1) lsl_oszi^.LslOsziSetCRC $ P_OSZI_SETCRC(p1)
#define OS_OSZI_GETCRC(p1) lsl_oszi^.LslOsziGetCRC $ P_OSZI_GETCRC(p1)
#define OS_OSZI_INIT() lsl_oszi^.LslOsziInit $ P_OSZI_INIT()


//HFILTER:1
#endif //ifndef _LSL_ST_OSZI_H
//HFILTER:
