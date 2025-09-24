// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Version  Date      Author    Comment
//   6.0      17082006  pr        new function:
//                                  CanTxRmtFrame
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

#ifndef __LSL_ST_CANUSER
#pragma once
#define __LSL_ST_CANUSER

FUNCTION __CDECL GLOBAL P_Canuser_IsInstalled
VAR_INPUT
    CanNR       : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetMyStation
VAR_INPUT
    CanNR       : USINT;
    Station     : ^USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_SetMyStation
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetBaudrate
VAR_INPUT   
    CanNR       : USINT;
    Baudrate    : ^USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_SetBaudrate
VAR_INPUT
    CanNR       : USINT;
    Baudrate    : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

//
// since version 2:
//
FUNCTION __CDECL GLOBAL P_Canuser_CancelReq
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_SetReqTimout
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    tv_ms       : UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetProtocolVersion
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    CanProtocolVersion : ^USINT;
    ReqType     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetCpuStatus
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    CpuStatus   : ^USINT;
    ReqType     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetSysvar
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    Sysvar      : ^UDINT;
    ReqType     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetData
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    Adr         : UDINT;
    Len         : UDINT;
    bData       : ^USINT;
    ReqType     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_SetData
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    Adr         : UDINT;
    Len         : UDINT;
    bData       : ^USINT;
    ReqType     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetBlockPG
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    SegAdr      : UINT;
    OfsAdr      : UINT;
    Len         : UINT;
    bData       : ^USINT;
    ReqType     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_SetBlockPG
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    SegAdr      : UINT;
    OfsAdr      : UINT;
    Len         : UINT;
    bData       : ^USINT;
    ReqType     : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetDataPG
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    SegAdr      : UINT;
    OfsAdr      : UINT;
    Len         : UINT;
    bData       : ^USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_SetDataPG
VAR_INPUT
    CanNR       : USINT;
    Station     : USINT;
    SegAdr      : UINT;
    OfsAdr      : UINT;
    Len         : UINT;
    bData       : ^USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_DelCanObj
VAR_INPUT
    CanNR       : USINT;
    Handle      : DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_DelBasicCanObj
VAR_INPUT
    CanNR       : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_InstallOnlCmdHook
VAR_INPUT
    canNR       : USINT;
    fnHook      : pVoid;
    pThis       : pVoid;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_TxRmtFrame
VAR_INPUT
    CanNR       : USINT;
    Handle      : DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Canuser_GetNrByPath
VAR_INPUT
    master      : UDINT;
    path        : ^CHAR;
    index       : UDINT;
END_VAR
VAR_OUTPUT
    retval      : USINT;
END_VAR;

#define CANUSER_ISINSTALLED(p1)         pCanuser^.IsInstalled $ P_Canuser_IsInstalled(p1)
#define CANUSER_GETMYSTATION(p1,p2)    	pCanuser^.GetMyStation $ P_Canuser_GetMyStation(p1,p2)
#define CANUSER_SETMYSTATION(p1,p2)    	pCanuser^.SetMyStation $ P_Canuser_SetMyStation(p1,p2)
#define CANUSER_GETBAUDRATE(p1,p2)    	pCanuser^.GetBaudrate $ P_Canuser_GetBaudrate(p1,p2)
#define CANUSER_SETBAUDRATE(p1,p2)    	pCanuser^.SetBaudrate $ P_Canuser_SetBaudrate(p1,p2)
// since version 2:
#define CANUSER_CANCELREQ(p1,p2)        pCanuser^.CancelReq $ P_Canuser_CancelReq(p1,p2)
#define CANUSER_SETREQTIMEOUT(p1,p2,p3)	pCanuser^.SetReqTimout $ P_Canuser_SetReqTimout(p1,p2,p3)
#define CANUSER_GETPROTOCOLVERSION(p1,p2,p3,p4)	pCanuser^.GetProtocolVersion $ P_Canuser_GetProtocolVersion(p1,p2,p3,p4)
#define CANUSER_GETCPUSTATUS(p1,p2,p3,p4) pCanuser^.GetCpuStatus $ P_Canuser_GetCpuStatus(p1,p2,p3,p4)
#define CANUSER_GETSYSVAR(p1,p2,p3,p4)  pCanuser^.GetSysvar $ P_Canuser_GetSysvar(p1,p2,p3,p4)
#define CANUSER_GETDATA(p1,p2,p3,p4,p5,p6) pCanuser^.GetData $ P_Canuser_GetData(p1,p2,p3,p4,p5,p6)
#define CANUSER_SETDATA(p1,p2,p3,p4,p5,p6) pCanuser^.SetData $ P_Canuser_SetData(p1,p2,p3,p4,p5,p6)
// since version 3:
#define CANUSER_GETBLOCKPG(p1,p2,p3,p4,p5,p6,p7) pCanuser^.GetBlockPG $ P_Canuser_GetBlockPG(p1,p2,p3,p4,p5,p6,p7)
#define CANUSER_SETBLOCKPG(p1,p2,p3,p4,p5,p6,p7) pCanuser^.SetBlockPG $ P_Canuser_SetBlockPG(p1,p2,p3,p4,p5,p6,p7)
#define CANUSER_GETDATAPG(p1,p2,p3,p4,p5,p6) pCanuser^.GetDataPG $ P_Canuser_GetDataPG(p1,p2,p3,p4,p5,p6)
#define CANUSER_SETDATAPG(p1,p2,p3,p4,p5,p6) pCanuser^.SetDataPG $ P_Canuser_SetDataPG(p1,p2,p3,p4,p5,p6)
// since version 4:
#define CANUSER_DELCANOBJ(p1,p2)        pCanuser^.DelCanObj $ P_Canuser_DelCanObj(p1,p2)
#define CANUSER_DELBASICCANOBJ(p1)      pCanuser^.DelBasicCanObj $ P_Canuser_DelBasicCanObj(p1)
// since version 5:
#define CANUSER_INSTALL_ONLCMDHOOK(p1,p2,p3) pCanuser^.InstallOnlCmdHook $ P_Canuser_InstallOnlCmdHook(p1,p2,p3)
// since version 6:
#define CANUSER_TXRMTFRAME(p1,p2)       pCanuser^.TxRmtFrame $ P_Canuser_TxRmtFrame(p1,p2)
// since version 7:
#define CANUSER_GETNRBYPATH(p1,p2,p3)   pCanuser^.GetNrByPath $ P_Canuser_GetNrByPath(p1,p2,p3)

#pragma pack (push, 1)
TYPE
    OnlCmdResponse : STRUCT
      sizeOfStruct  : UINT;         // Größe der Struktur (inkl. sizeOfStruct)
      toStation     : UINT;         // CAN-Station, an die gesendet werden soll
      txData        : ARRAY[0..7] OF USINT; // Daten der zu sendenden CAN-Message
	  END_STRUCT;

    OS_CANUSER : STRUCT
          udVersion            : UDINT;
        // CanNr: 1=CAN1, 2=CAN2, etc.
        IsInstalled         : pVoid;
        GetMyStation        : pVoid;
        SetMyStation        : pVoid;
        GetBaudrate         : pVoid;
        SetBaudrate         : pVoid;
        // since Version 2
        CancelReq           : pVoid;
        SetReqTimout        : pVoid;
        GetProtocolVersion  : pVoid;
        GetCpuStatus        : pVoid;
        GetSysvar           : pVoid;
        GetData             : pVoid;
        SetData             : pVoid;
        // since Version 3
        GetBlockPG          : pVoid;
        SetBlockPG          : pVoid;
        GetDataPG           : pVoid;
        SetDataPG           : pVoid;
        // since Version 4
        DelCanObj           : pVoid;
        DelBasicCanObj      : pVoid;
        // since Version 5
        InstallOnlCmdHook   : pVoid;
        // since Version 6
        TxRmtFrame          : pVoid;
        // since Version 7
        GetNrByPath         : pVoid;
    END_STRUCT;
END_TYPE
#pragma pack (pop)

// Defines
#define REQTYPE_SENDWAIT      0   // send request and wait for the response
#define REQTYPE_SENDNOWAIT    1   // send request in background
#define REQTYPE_WAITRESP      2   // wait for the response
#define REQTYPE_POLLRESP      3   // poll for a response

// Errors
(*#define CANERROR_NONE               0     // o.k.
#define CANERROR_INVALIDPARAM       -1    // invalid parameter
#define CANERROR_NOBIOS             -2    // BIOS-Interface is not installed
#define CANERROR_NOTENABLED         -3    // CAN is not enabled
#define CANERROR_NOTINSTALLED       -4    // CAN-Interface is not installed
#define CANERROR_INIT               -5    // Initializing of CAN-Interface failed*)
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

#endif
