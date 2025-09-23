#ifndef _LSL_ST_CMDIF_H
#pragma once
#define _LSL_ST_CMDIF_H

#define LSLCMD_RECVBLOCKSIZE    1000
#define LSLCMD_SENDBLOCKSIZE    1000

#define LSLCMD_COMMID_LOADER   	   4

#define PRECMD_ERROR       -5    // Error preparing to Rx command
#define PRECMD_DATA         1    // Rx prepared, expect command data next
#define PRECMD_POST         0    // Rx prepared, expect message postfix next
#define PRECMD_DATA_REJECT -3    // Rx prepared, expect data to reject next
#define PRECMD_MOREDATA     6

#pragma pack( push, 1 )
TYPE
    OS_LSLCMDIF : STRUCT
        size        : UDINT;
        version     : UDINT;
        open        : pVoid;
        close       : pVoid;
        preCmd      : pVoid;
        collectData : pVoid;
        postCmd     : pVoid;
    END_STRUCT;
END_TYPE
#pragma pack(pop)

VAR_PRIVATE
	OS_pLslCmd : ^OS_LSLCMDIF;
END_VAR

//
// LSLCMD_Open
//
FUNCTION GLOBAL __cdecl P_LSLCMD_Open
VAR_INPUT
    commID     : UDINT;
    userData   : pVoid;
    rxCounter  : ^UDINT;
    txBuffer   : ^USINT;
    maxTXlen   : UDINT;
    maxSendBlockSize : UDINT;
    maxRecvBlockSize : UDINT;
    fnSend     : pVoid;
END_VAR
VAR_OUTPUT
    cmdIf : pVoid;
END_VAR;

//
// LSLCMD_Close
//
FUNCTION GLOBAL __cdecl P_LSLCMD_Close
VAR_INPUT
    pCmdIF     : pVoid;
END_VAR;

//
// LSLCMD_PRECmd
//
FUNCTION GLOBAL __cdecl P_LSLCMD_PRECmd
VAR_INPUT
    pCmdIF       : pVoid;
    defRxBuf     : ^USINT;
    defRxBufSize : UDINT;
    cmd          : USINT;
    dataLen      : UDINT;
END_VAR
VAR_OUTPUT
    retVal : DINT;
END_VAR;

//
// LSLCMD_CollectData
//
FUNCTION GLOBAL __cdecl P_LSLCMD_CollectData
VAR_INPUT
    pCmdIF       : pVoid;
    pData        : ^USINT;
    dataLen      : UDINT;
END_VAR
VAR_OUTPUT
    retVal : DINT;
END_VAR;

//
// LSLCMD_POSTCmd
//
FUNCTION GLOBAL __cdecl P_LSLCMD_POSTCmd
VAR_INPUT
	pCmdIF : pVoid;
	fError : UDINT;
END_VAR
VAR_OUTPUT
    retVal : DINT;
END_VAR;

#define OS_LSLCMD_Open(p1,p2,p3,p4,p5,p6,p7,p8) OS_pLslCmd^.open $ P_LSLCMD_Open(p1,p2,p3,p4,p5,p6,p7,p8)
#define OS_LSLCMD_Close(p1) OS_pLslCmd^.close $ P_LSLCMD_Close(p1)
#define OS_LSLCMD_PRECmd(p1,p2,p3,p4,p5) OS_pLslCmd^.preCmd $ P_LSLCMD_PRECmd(p1,p2,p3,p4,p5)
#define OS_LSLCMD_CollectData(p1,p2,p3) OS_pLslCmd^.collectData $ P_LSLCMD_CollectData(p1,p2,p3)
#define OS_LSLCMD_POSTCmd(p1,p2) OS_pLslCmd^.postCmd $ P_LSLCMD_POSTCmd(p1,p2)

#endif // _LSL_ST_CMDIF_H
