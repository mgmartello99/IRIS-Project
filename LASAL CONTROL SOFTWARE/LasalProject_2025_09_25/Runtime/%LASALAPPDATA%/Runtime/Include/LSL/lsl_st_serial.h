//HFILTER:1
#ifndef __LSL_ST_SERIAL
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
 #define __LSL_ST_SERIAL

#pragma pack( push, 1 )
TYPE
  LSLAPI_SERIALINFO     : STRUCT
	   initialized : USINT;
	   comportnum : USINT;
	   IRQNum : USINT;
	   IOPort : UINT;
	   Baudrate : UINT;
	   Ptr_RecvBuffer : pVoid;
	   Ptr_SendBuffer : pVoid;
     Ptr_RecvBufferTime : pVoid;
  END_STRUCT;
END_TYPE
#pragma pack(pop)


#define SERUSERCOM_1 1
#define SERUSERCOM_2 2
#define SERUSERCOM_3 3
#define SERUSERCOM_4 4

#define SERUSERBAUD_300     0
#define SERUSERBAUD_600     1
#define SERUSERBAUD_1200    2
#define SERUSERBAUD_2400    3
#define SERUSERBAUD_4800    4
#define SERUSERBAUD_9600    5
#define SERUSERBAUD_14400   6
#define SERUSERBAUD_19200   7
#define SERUSERBAUD_38400   8
#define SERUSERBAUD_56000   9
#define SERUSERBAUD_115200 10

#define SERUSERPARITY_NONE  0
#define SERUSERPARITY_ODD   1
#define SERUSERPARITY_EVEN  2
#define SERUSERPARITY_MARK  3
#define SERUSERPARITY_SPACE 4



//----------------------------------
// Errors from GetError
#define SERERROR_NONE        0
#define SERERROR_COMNUM     -1
#define SERERROR_BAUDTABLE  -2
#define SERERROR_BAUDRATE   -3
#define SERERROR_PARITY     -4
#define SERERROR_STOPBIT    -5
#define SERERROR_WORDLEN    -6
#define SERERROR_INUSE     -10
#define SERERROR_OSINUSE   -11
#define SERERROR_NOTAVAIL  -12
#define SERERROR_NOMEM     -13
#define SERERROR_NOHANDLE  -14
#define SERERROR_PARAMETER -15
#define SERERROR_RECVBUF   -16
#define SERERROR_SENDBUF   -17
#define SERERROR_TOOLONG   -18
#define SERERROR_RECVERROR -19
#define SERERROR_SENDERROR -20
#define SERERROR_GENERAL   -21
#define SERERROR_PARITY_E  -22
#define SERERROR_FRAMING_E -23



// bit masks which may be written to the MCR using COMModemControl
#define SERUSER_DTR          16#01      // Data Terminal Ready
#define SERUSER_RTS          16#02      // Request To Send
#define SERUSER_OUT1         16#04
#define SERUSER_OUT2         16#08
#define SERUSER_LOOPBACK     16#10



TYPE
  SERUSER_FIFOSTATUS  : (
    SERUSER_FIFOUNKNOWN,        // No error, OK
    SERUSER_FIFOPRESENT,        // Not enough memory
    SERUSER_FIFONOTPRESENT      // Interface exists
    )$UDINT;
END_TYPE


//--------------------------------------------------------
// void *(*pInitSerial)(unsigned short comnum0, unsigned short combaud0, unsigned short parity0, unsigned short stopbits0, unsigned short wordlength0);
FUNCTION GLOBAL __cdecl P_SerUsr_Init
VAR_INPUT
    comnum0 : UINT;
    combaud0 : UINT;
    parity0 : UINT;
    stopbits0 : UINT;
    wordlength0 : UINT;
END_VAR
VAR_OUTPUT 
    ret0     : pVoid;
END_VAR;

//--------------------------------------------------------
// long  (*pGetError)(void* handle0);
FUNCTION GLOBAL __cdecl P_SerUsr_GetError
VAR_INPUT
    handle0 : pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// void *(*pGetHandle)(unsigned short comnum0);
FUNCTION GLOBAL __cdecl P_SerUsr_GetHandle
VAR_INPUT
    comnum0 : UINT;
END_VAR
VAR_OUTPUT 
    ret0     : pVoid;
END_VAR;

//--------------------------------------------------------
// unsigned long (*pIsInitialized)(void *handle0);
FUNCTION GLOBAL __cdecl P_SerUsr_IsInitialized
VAR_INPUT
    handle0 : pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : UDINT;
END_VAR;

//--------------------------------------------------------
// long  (*pGetInfo)(void *handle0, LSLAPI_SERIALINFO *info0);
FUNCTION GLOBAL __cdecl P_SerUsr_GetInfo 
VAR_INPUT
    handle0 : pVoid;
    info0   : ^LSLAPI_SERIALINFO;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long  (*pSetBufferRecv)(void *handle0, void *recvbuffer0, unsigned long bufferlength0);
FUNCTION GLOBAL __cdecl P_SerUsr_SetBufferRecv
VAR_INPUT
    handle0 : pVoid;
    recvbuffer0: pVoid;
    bufferlength0 : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// unsigned long (*pSend)(void *handle0, void *buffer0, unsigned long bufferlength0);
FUNCTION GLOBAL __cdecl P_SerUsr_Send
VAR_INPUT
    handle0 : pVoid;
    buffer0: pVoid;
    bufferlength0 : UDINT;
    wrlen0 : ^UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long  (*pRecvChar)(void *handle0, unsigned char *buffer0);
FUNCTION GLOBAL __cdecl P_SerUsr_RecvChar
VAR_INPUT
    handle0 : pVoid;
    buffer0: pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long  (*pRecvBlock)(void *handle0, void *buffer0, unsigned long rdlength0, unsigned long *rdlen0);
FUNCTION GLOBAL __cdecl P_SerUsr_RecvBlock
VAR_INPUT
    handle0 : pVoid;
    buffer0: pVoid;
    rdlength0 : UDINT;
    rdlen0 : ^UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;


//--------------------------------------------------------
// unsigned long (*pGetSendStatus)(void* handle0);
FUNCTION GLOBAL __cdecl P_SerUsr_GetSendStatus
VAR_INPUT
    handle0 : pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : UDINT;
END_VAR;

//--------------------------------------------------------
// unsigned long (*pGetRecvStatus)(void *handle0);
FUNCTION GLOBAL __cdecl P_SerUsr_GetRecvStatus
VAR_INPUT
    handle0 : pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : UDINT;
END_VAR;

//--------------------------------------------------------
// long  (*pClearRecvBuffer)(void *handle0);
FUNCTION GLOBAL __cdecl P_SerUsr_ClearRecvBuffer
VAR_INPUT
    handle0 : pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// void *(*pGetRecvPointer)(void *handle0);
FUNCTION GLOBAL __cdecl P_SerUsr_GetRecvPointer
VAR_INPUT
    handle0 : pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : pVoid;
END_VAR;

//--------------------------------------------------------
// void  (*pClose)(void *handle0);
FUNCTION GLOBAL __cdecl P_SerUsr_Close
VAR_INPUT
    handle0 : pVoid;
END_VAR;


//--------------------------------------------------------
// long  (*pSetOnline)(unsigned long action0);
FUNCTION GLOBAL __cdecl P_SerUsr_SetOnline
VAR_INPUT
    action0 : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;


//--------------------------------------------------------
// long SIGSERIAL_SetUserFunction(void *handle0, void *fct0, void *param0)
FUNCTION GLOBAL __cdecl P_SerUsr_SetFunction
VAR_INPUT
    handle0   : pVoid;
    function0 : pVoid;
    param0    : pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;



//--------------------------------------------------------
// long SIGSERIAL_SetModemControl(void *handle0, unsigned long SetToOneZero0, unsigned long NewValue0)
FUNCTION GLOBAL __cdecl P_SerUsr_SetModemControl
VAR_INPUT
    handle0       : pVoid;
    SetToOneZero0 : UDINT;
    NewValue0     : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long SIGSERIAL_GetModemControl(void *handle0, unsigned char *Value0)
FUNCTION GLOBAL __cdecl P_SerUsr_GetModemControl
VAR_INPUT
    handle0       : pVoid;
    Value0        : ^USINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long SIGSERIAL_Set422Mode(void *handle0, unsigned long OnOff0)
FUNCTION GLOBAL __cdecl P_SerUsr_Set422Mode
VAR_INPUT
    handle0       : pVoid;
    OnOff0        : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long SIGSERIAL_Get422Mode(void *handle0, unsigned long *OnOff0)
FUNCTION GLOBAL __cdecl P_SerUsr_Get422Mode
VAR_INPUT
    handle0       : pVoid;
    OnOff0        : ^UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;


//--------------------------------------------------------
// long  (*pGetModemStatus)(void *handle0, unsigned char *Value0);
FUNCTION GLOBAL __cdecl P_SerUsr_GetModemStatus
VAR_INPUT
    handle0       : pVoid;
    Value0        : ^USINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;


//--------------------------------------------------------
//long  SIGSERIAL_HasFIFO(void *handle0, SERUSER_FIFOStatus *FifoStatus0);
FUNCTION GLOBAL __cdecl P_SerUsr_HasFIFO
VAR_INPUT
    handle0       : pVoid;
    FifoStatus0   : ^SERUSER_FIFOStatus;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
//long SIGSERIAL_EnableFIFO(void *handle0, unsigned long Trigger0);
FUNCTION GLOBAL __cdecl P_SerUsr_EnableFIFO
VAR_INPUT
    handle0       : pVoid;
    Trigger0      : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
//long SIGSERIAL_SetBreak(void *handle0, unsigned long Value);
FUNCTION GLOBAL __cdecl P_SerUsr_SetBreak
VAR_INPUT
    handle0       : pVoid;
    Value         : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long SIGSERIAL_SetMDPMode(void *handle0, unsigned long OnOff0)
FUNCTION GLOBAL __cdecl P_SerUsr_SetMDPMode
VAR_INPUT
    handle0       : pVoid;
    OnOff0        : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long SIGSERIAL_Set485Mode(void *handle0, unsigned long OnOff0, unsigned long EchoOnOff0)
FUNCTION GLOBAL __cdecl P_SerUsr_Set485Mode
VAR_INPUT
    handle0       : pVoid;
    OnOff0        : UDINT;
    EchoOnOff0    : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
FUNCTION __CDECL GLOBAL P_SerUsr_GetNrByPath
VAR_INPUT
    master      : UDINT;
    path        : ^CHAR;
    index       : UDINT;
END_VAR
VAR_OUTPUT
    retval      : UINT;
END_VAR;

//--------------------------------------------------------
// long  (*pRecvTimingChar)(void *handle0, unsigned long *buffer0);
FUNCTION GLOBAL __cdecl P_SerUsr_RecvTimingChar
VAR_INPUT
    handle0 : pVoid;
    buffer0: pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;


//--------------------------------------------------------
// long  (*pRecvTimingBlock)(void *handle0, void *buffer0, unsigned long rdlength0, unsigned long *rdlen0);
FUNCTION GLOBAL __cdecl P_SerUsr_RecvTimingBlock
VAR_INPUT
    handle0 : pVoid;
    buffer0: pVoid;
    rdlength0 : UDINT;
    rdlen0 : ^UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long  (*pClearTimingBuffer)(void *handle0);
FUNCTION GLOBAL __cdecl P_SerUsr_ClearTimingBuffer
VAR_INPUT
    handle0 : pVoid;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;

//--------------------------------------------------------
// long (*SetTimingBufferRecv)(void *handle0, void *recvbuffer0, unsigned long bufferlength0);
FUNCTION GLOBAL __cdecl P_SerUsr_SetTimingBufferRecv
VAR_INPUT
    handle0 : pVoid;
    recvbuffer0: pVoid;
    bufferlength0 : UDINT;
END_VAR
VAR_OUTPUT 
    ret0     : DINT;
END_VAR;


#define SERUSER_Init(p1,p2,p3,p4,p5)    _LSL_POS^.piSerial^.pInitSerial      $ P_SerUsr_Init(p1,p2,p3,p4,p5)
#define SERUSER_GetError(p1)            _LSL_POS^.piSerial^.pGetError        $ P_SerUsr_GetError(p1)
#define SERUSER_GetHandle(p1)           _LSL_POS^.piSerial^.pGetHandle       $ P_SerUsr_GetHandle(p1)
#define SERUSER_IsInitialized(p1)       _LSL_POS^.piSerial^.pIsInitialized   $ P_SerUsr_IsInitialized(p1)
#define SERUSER_GetInfo(p1,p2)          _LSL_POS^.piSerial^.pGetInfo         $ P_SerUsr_GetInfo(p1,p2)
#define SERUSER_SetBufferRecv(p1,p2,p3) _LSL_POS^.piSerial^.pSetBufferRecv   $ P_SerUsr_SetBufferRecv(p1,p2,p3)
#define SERUSER_Send(p1,p2,p3,p4)       _LSL_POS^.piSerial^.pSend            $ P_SerUsr_Send(p1,p2,p3,p4)
#define SERUSER_RecvChar(p1,p2)         _LSL_POS^.piSerial^.pRecvChar        $ P_SerUsr_RecvChar(p1,p2)
#define SERUSER_RecvBlock(p1,p2,p3,p4)  _LSL_POS^.piSerial^.pRecvBlock       $ P_SerUsr_RecvBlock(p1,p2,p3,p4)
#define SERUSER_GetSendStatus(p1)       _LSL_POS^.piSerial^.pGetSendStatus   $ P_SerUsr_GetSendStatus(p1)
#define SERUSER_GetRecvStatus(p1)       _LSL_POS^.piSerial^.pGetRecvStatus   $ P_SerUsr_GetRecvStatus(p1)
#define SERUSER_ClearRecvBuffer(p1)     _LSL_POS^.piSerial^.pClearRecvBuffer $ P_SerUsr_ClearRecvBuffer(p1)
#define SERUSER_GetRecvPointer(p1)      _LSL_POS^.piSerial^.pGetRecvPointer  $ P_SerUsr_GetRecvPointer(p1)
#define SERUSER_Close(p1)               _LSL_POS^.piSerial^.pClose           $ P_SerUsr_Close(p1)
#define SERUSER_SetOnline(p1)           _LSL_POS^.piSerial^.pSetOnline       $ P_SerUsr_SetOnline(p1)
#define SERUSER_UserFunction(p1,p2,p3)  _LSL_POS^.piSerial^.pSetFunction     $ P_SerUsr_SetFunction(p1,p2,p3)
#define SERUSER_SetModemControl(p1,p2,p3) _LSL_POS^.piSerial^.pSetModemControl $ P_SerUsr_SetModemControl(p1,p2,p3)
#define SERUSER_GetModemControl(p1,p2)  _LSL_POS^.piSerial^.pGetModemControl $ P_SerUsr_GetModemControl(p1,p2)
#define SERUSER_Set422Mode(p1,p2)       _LSL_POS^.piSerial^.pSet422Mode      $ P_SerUsr_Set422Mode(p1,p2)
#define SERUSER_Get422Mode(p1,p2)       _LSL_POS^.piSerial^.pGet422Mode      $ P_SerUsr_Get422Mode(p1,p2)
#define SERUSER_GetModemStatus(p1,p2)   _LSL_POS^.piSerial^.pGetModemStatus  $ P_SerUsr_GetModemStatus(p1,p2)
#define SERUSER_HasFIFO(p1,p2)          _LSL_POS^.piSerial^.pHasFIFO         $ P_SerUsr_HasFIFO(p1,p2)
#define SERUSER_EnableFIFO(p1,p2)       _LSL_POS^.piSerial^.pEnableFIFO      $ P_SerUsr_EnableFIFO(p1,p2)
#define SERUSER_SetBreak(p1,p2)         _LSL_POS^.piSerial^.pSetBreak        $ P_SerUsr_SetBreak(p1,p2)
#define SERUSER_SetMDPMode(p1,p2)       _LSL_POS^.piSerial^.pSetMDPMode      $ P_SerUsr_SetMDPMode(p1,p2)
#define SERUSER_Set485Mode(p1,p2,p3)    _LSL_POS^.piSerial^.pSet485Mode      $ P_SerUsr_Set485Mode(p1,p2,p3)
// ab OS Version 1.1.227
#define SERUSER_GETNRBYPATH(p1,p2,p3)   _LSL_POS^.piSerial^.pGetNrByPath     $ P_SerUsr_GetNrByPath(p1,p2,p3)

// ab OS Version 01.02.195
#define SERUSER_RecvTimingChar(p1,p2)   _LSL_POS^.piSerial^.pRecvTimingChar  $ P_SerUsr_RecvTimingChar(p1,p2)
#define SERUSER_RecvTimingBlock(p1,p2,p3,p4)  _LSL_POS^.piSerial^.pRecvTimingBlock $ P_SerUsr_RecvTimingBlock(p1,p2,p3,p4)
#define SERUSER_ClearTimingBuffer(p1)     _LSL_POS^.piSerial^.pClearTimingBuffer $ P_SerUsr_ClearTimingBuffer(p1)
#define SERUSER_SetTimingBufferRecv(p1,p2,p3) _LSL_POS^.piSerial^.pSetTimingBufferRecv   $ P_SerUsr_SetTimingBufferRecv(p1,p2,p3)


//----------------------------------
// UserFunction: 
//
// FUNCTION __CDECL MyFunction
// VAR_INPUT
//    UserParam0 : pVoid;
//    ComNum     : UDINT;     // see comnum defines
//    BasePort   : UDINT;     // base port address of com interface, e.g. COM1 = 16#3F8
// END_VAR       
//     ....
// END_FUNCTION    // int. quit is done by OS
//
//



//HFILTER:1
#endif //__LSL_ST_SERIAL
//HFILTER:
