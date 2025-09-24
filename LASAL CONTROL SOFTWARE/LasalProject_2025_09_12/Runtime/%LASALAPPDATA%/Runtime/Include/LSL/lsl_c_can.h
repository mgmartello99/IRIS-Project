// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Version  Date      Author    Comment
//   6.0      17082006  pr        new function:
//                                  TxRmtFrame
//   5.0      24012005  pr        new function:
//                                  InstallOnlCmdHook
//   4.0      13122004  pr        new functions:
//                                  DelCanObj
//                                  DelBasicCanObj
//   3.0      20012004  pr        new functions:
//                                  CAN_GetBlockPG
//                                  CAN_SetBlockPG
//                                  CAN_GetDataPG
//                                  CAN_SetDataPG
//   2.0      17092002  pr        new functions:
//                                  CAN_CancelReq
//                                  CAN_SetReqTimout
//                                  CAN_GetCpuStatus
//                                  CAN_GetSysvar
//                                  CAN_GetData
//                                  CAN_SetData
//   1.0      17052002  pr        initial version
// ------------------------------------------------------------

#pragma once
#define __LSL_IF_CAN_H

#define LSLAPI_CAN_VERSION   0x0007

#pragma pack (push, 1)

// Antwort vom OnlCmdHook
typedef struct OnlCmdResponse
{
  unsigned short sizeOfStruct;
  unsigned short toStation;
  unsigned char  txData[8];
} OnlCmdResponse;

typedef struct
{
  unsigned long version;
  // CanNr: 1=CAN1, 2=CAN2, etc.
  // Station: 0-31
  // Baudrate: 
  long (*CAN_IsInstalled)( unsigned char CanNr );
  long (*CAN_GetMyStation)( unsigned char CanNr, unsigned char *Station );
  long (*CAN_SetMyStation)( unsigned char CanNr, unsigned char Station );
  long (*CAN_GetBaudrate)( unsigned char CanNr, unsigned char *Baudrate );
  long (*CAN_SetBaudrate)( unsigned char CanNr, unsigned char Baudrate );
  // ab Version 2
  long (*CAN_CancelReq)( unsigned char CanNr, unsigned char Station );
  long (*CAN_SetReqTimout)( unsigned char CanNr, unsigned char Station,
                            unsigned long tv_ms );
  long (*CAN_GetProtocolVersion)( unsigned char CanNr, 
                                  unsigned char Station,
                                  unsigned char *CanProtocolVersion,
                                  unsigned char ReqType);
  long (*CAN_GetCpuStatus)( unsigned char CanNr, unsigned char Station,
                            unsigned char *CpuStatus, unsigned char ReqType);
  long (*CAN_GetSysvar)( unsigned char CanNr, 
                         unsigned char Station,
                         unsigned long *Sysvar,
                         unsigned char ReqType);
  long (*CAN_GetData)( unsigned char CanNr, 
                       unsigned char Station,
                       unsigned long Adr,
                       unsigned long Len,
                       unsigned char *bData,
                       unsigned char ReqType);
  long (*CAN_SetData)( unsigned char CanNr, 
                       unsigned char Station,
                       unsigned long Adr,
                       unsigned long Len,
                       unsigned char *bData,
                       unsigned char ReqType);
  // ab Version 3
  long (* CAN_GetBlockPG)( unsigned char CanNr, 
                           unsigned char Station,
                           unsigned short SegAdr,
                           unsigned short OfsAdr,
                           unsigned short Len,
                           unsigned char *bData,
                           unsigned char ReqType);
  long (* CAN_SetBlockPG)( unsigned char CanNr, 
                           unsigned char Station,
                           unsigned short SegAdr,
                           unsigned short OfsAdr,
                           unsigned short Len,
                           unsigned char *bData,
                           unsigned char ReqType);
  long (* CAN_GetDataPG)(  unsigned char CanNr, 
                           unsigned char Station,
                           unsigned short SegAdr,
                           unsigned short OfsAdr,
                           unsigned short Len,
                           unsigned char *bData);
  long (* CAN_SetDataPG)(  unsigned char CanNr, 
                           unsigned char Station,
                           unsigned short SegAdr,
                           unsigned short OfsAdr,
                           unsigned short Len,
                           unsigned char *bData);
  // ab Version 4
  int (* DelCanObj)(unsigned char cannr, int handle);
  int (* DelBasicCanObj)(unsigned char cannr);
  // ab Version 5
  int (* InstallOnlCmdHook)(unsigned char cannr, void *fnHook, void *pThis);
  // ab Version 6
  int (* TxRmtFrame)(unsigned char cannr, int handle);
  // ab Version 7
  unsigned char (* GetNrByPath)(unsigned int master, unsigned char *path, unsigned long index);
}LSLAPI_CAN;
#pragma pack (pop)

// Defines
#define REQTYPE_SENDWAIT      0   // send request and wait for the response
#define REQTYPE_SENDNOWAIT    1   // send request in background
#define REQTYPE_WAITRESP      2   // wait for the response
#define REQTYPE_POLLRESP      3   // poll for a response

// Errors
#define CANERROR_NONE               0     // o.k.
#define CANERROR_INVALIDPARAM       -1    // invalid parameter
#define CANERROR_NOBIOS             -2    // BIOS-Interface is not installed
#define CANERROR_NOTENABLED         -3    // CAN is not enabled
#define CANERROR_NOTINSTALLED       -4    // CAN-Interface is not installed
#define CANERROR_INIT               -5    // Initializing of CAN-Interface failed
#define CANERROR_ONLINE             -98   // Active Online Connection (e.g. SetBaudrate failed because of an online conn.)
#define CANERROR_FCNNOTAVAILABLE    -99   // Function is not available
// V2:
#define CANERROR_SYSERROR           -6    // system error
#define CANERROR_SENDFAILED         -100  // send request failed
#define CANERROR_REQINUSE           -101  // request is in use by another task
#define CANERROR_SENDREQ_PENDING    -102  // send not possible, because a previous request has not completed
#define CANERROR_IO_PENDING         -103  // no response available, because request is pending
#define CANERROR_REQ_IDLE           -104  // no response available, because no request was launched
#define CANERROR_REQ_CANCELED       -105  // no response available, because request was canceled
#define CANERROR_REQ_TIMEOUT        -106  // no response available, because request timed out
#define CANERROR_REQ_NACKRECEIVED   -107  // no response available, because a neg. acknowledgment was received



