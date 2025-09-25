//HFILTER:1
#ifndef __LSL_C_SERIAL
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define __LSL_C_SERIAL

#pragma pack(push, 1)
typedef struct
{
   unsigned char  initialized;
   unsigned char  comportnum;
   unsigned char  IRQNum;
   unsigned short IOPort;
   unsigned short Baudrate;
   void          *Ptr_RecvBuffer;
   void          *Ptr_SendBuffer;
}LSLAPI_SERIALINFO;


typedef enum { SERUSER_FIFOUnknown, SERUSER_FIFOPresent, SERUSER_FIFONotPresent } SERUSER_FIFOStatus;


typedef struct 
{
    void *(*pInitSerial)(unsigned short comnum0, 
                         unsigned short combaud0, 
                         unsigned short parity0, 
                         unsigned short stopbits0, 
                         unsigned short wordlength0);
    long  (*pGetError)(void* handle0);
    void *(*pGetHandle)(unsigned short comnum0);
    unsigned long (*pIsInitialized)(void *handle0);
    long  (*pGetInfo)(void *handle0, LSLAPI_SERIALINFO *info0);
    long  (*pSetBufferRecv)(void *handle0, void *recvbuffer0, unsigned long bufferlength0);
    long  (*pSend)(void *handle0, void *buffer0, unsigned long bufferlength0, unsigned long *wrlen0);
    long  (*pRecvChar)(void *handle0, unsigned char *buffer0);
    long  (*pRecvBlock)(void *handle0, void *buffer0, unsigned long rdlength0, unsigned long *rdlen0);
    unsigned long (*pGetSendStatus)(void* handle0);
    unsigned long (*pGetRecvStatus)(void *handle0);
    long  (*pClearRecvBuffer)(void *handle0);
    void *(*pGetRecvPointer)(void *handle0);
    void  (*pClose)(void *handle0);
    void  (*pReset)(void *appl0);
    long  (*pSetOnline)(unsigned long action0);
    long  (*pSetFunction)(void *handle0, void *fct0, void *param0);
    long  (*pSetModemControl)(void *handle0, unsigned long SetToOneZero0, unsigned long NewValue0);
    long  (*pGetModemControl)(void *handle0, unsigned char *Value0);
    long  (*pSet422Mode)(void *handle0, unsigned long OnOff0);
    long  (*pGet422Mode)(void *handle0, unsigned long *OnOff0);
    long  (*pGetModemStatus)(void *handle0, unsigned char *Value0);
    long  (*pHasFIFO)(void *handle0, SERUSER_FIFOStatus *FifoStatus0);
    long  (*pEnableFIFO)(void *handle0, unsigned long Trigger0);
    long  (*pSetBreak)(void *handle0, unsigned long Value);
    long  (*pSetMDPMode)(void *handle0, unsigned long OnOff0);
    long  (*pSet485Mode)(void *handle0, unsigned long OnOff0, unsigned long EchoOnOff0);
    // ab OS Version 1.1.227
    unsigned short (*pGetNrByPath)(unsigned int master_, unsigned char *path, unsigned long index);
	// ab OS Version 01.02.078
    long  (*pClearSendBuffer)(void *handle0);
    long  (*pSetBufferSend)(void *handle0, void *recvbuffer0, unsigned long bufferlength0);

    // ab 01.02.195
    long (*pRecvTimingChar)(void *handle0, unsigned long *val0);
    long (*pRecvTimingBlock)(void *handle0, unsigned long *val0, unsigned long len0, unsigned long *rdlen0);
    long (*pClearTimingBuffer)(void *handle0);
    long (*SetTimingBufferRecv)(void *handle0, void *recvbuffer0, unsigned long bufferlength0);
}LSLAPI_SERIAL;
#pragma pack(pop)


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
#define SERUSER_DTR          0x01      // Data Terminal Ready
#define SERUSER_RTS          0x02      // Request To Send
#define SERUSER_OUT1         0x04
#define SERUSER_OUT2         0x08
#define SERUSER_LOOPBACK     0x10



#define SERUSER_Init            LSL_POS->piSerial->pInitSerial
#define SERUSER_GetError        LSL_POS->piSerial->pGetError
#define SERUSER_GetHandle       LSL_POS->piSerial->pGetHandle
#define SERUSER_IsInitialized   LSL_POS->piSerial->pIsInitialized
#define SERUSER_GetInfo         LSL_POS->piSerial->pGetInfo
#define SERUSER_SetBufferRecv   LSL_POS->piSerial->pSetBufferRecv
#define SERUSER_Send            LSL_POS->piSerial->pSend
#define SERUSER_RecvChar        LSL_POS->piSerial->pRecvChar
#define SERUSER_RecvBlock       LSL_POS->piSerial->pRecvBlock
#define SERUSER_GetSendStatus   LSL_POS->piSerial->pGetSendStatus
#define SERUSER_GetRecvStatus   LSL_POS->piSerial->pGetRecvStatus
#define SERUSER_ClearRecvBuffer LSL_POS->piSerial->pClearRecvBuffer
#define SERUSER_GetRecvPointer  LSL_POS->piSerial->pGetRecvPointer
#define SERUSER_Close           LSL_POS->piSerial->pClose

//HFILTER:1
#endif //__LSL_C_SERIAL
//HFILTER:
