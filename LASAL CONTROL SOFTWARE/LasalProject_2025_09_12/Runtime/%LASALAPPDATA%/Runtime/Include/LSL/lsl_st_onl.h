#ifndef _LSL_ST_ONL_H
#pragma once
#define _LSL_ST_ONL_H

// Defines
#define ONLTYPE_NONE    0
#define ONLTYPE_CAN     1
#define ONLTYPE_TCPIP   2

// Errors
#define LSLONL_NOERROR                0
#define LSLONLERR_INITFAILED          -200
#define LSLONLERR_INVONLTYPE          -201    // invalid online type
#define LSLONLERR_ADDRINUSE           -202    // address is already in use
#define LSLONLERR_NOMEM               -203    // out of memory
#define LSLONLERR_NOINTERFACE         -204    // no interface available
#define LSLONLERR_RESPTOOSMALL        -205    // number of bytes in response is too small
#define LSLONLERR_TIMEOUTCPUSTAT      -206    // timeout waiting for a cpu status
#define LSLONLERR_INVADDRESS          -207    // invalid address specified
#define LSLONLERR_SYSTEM              -208    // system error
#define LSLONLERR_MAXCONNECTIONS      -209    // max. number of connections reached
#define LSLONLERR_INVDESCRNUM         -210    // invalid descriptor number
#define LSLONLERR_INVPARAM            -211    // invalid parameter
#define LSLONLERR_FCNUNAVAILABLE      -212    // function not available (e.g. wrong interface version)

#pragma pack( push, 1 )
TYPE
    OS_LSLONL_ADDR : STRUCT
        OnlType                     : UDINT;
        InterfaceNbr                : UDINT;
        reserved1                   : UDINT;
        Filler1                     : UDINT;
        Filler2                     : UDINT;
        Filler3                     : UDINT;
        Filler4                     : UDINT;
        Filler5                     : UDINT;
    END_STRUCT;

    OS_LSLONL_ADDR_CAN : STRUCT
        OnlType                     : UDINT;
        InterfaceNbr                : UDINT;
        reserved1                   : UDINT;
        CanDestStation              : UDINT;
        Filler2                     : UDINT;
        Filler3                     : UDINT;
        Filler4                     : UDINT;
        Filler5                     : UDINT;
    END_STRUCT;

    OS_LSLONL_ADDR_TCPIP : STRUCT
        OnlType                     : UDINT;
        InterfaceNbr                : UDINT;
        reserved1                   : UDINT;
        IP1                         : USINT;
        IP2                         : USINT;
        IP3                         : USINT;
        IP4                         : USINT;
        Port                        : UDINT;
        Filler3                     : UDINT;
        Filler4                     : UDINT;
        Filler5                     : UDINT;
    END_STRUCT;

    OS_LSLONL : STRUCT
        udVersion                   : UDINT;
        OsError                     : pVoid;
        Open                        : pVoid;
        Close                       : pVoid;
        CancelRequest               : pVoid;
        String2Onladdr              : pVoid;
        OS_reserved3                : pVoid;
        GetCpuStatus                : pVoid;
        ResetSPS                    : pVoid;
        RunSPS                      : pVoid;
        GetPlcInfo                  : pVoid;

        // ab Version 2
        ServiceProvider             : pVoid;
        FileInfo                    : pVoid;
        FileSave                    : pVoid;
        LoadOsImage                 : pVoid;
        BootOsImage                 : pVoid;
        LoadPrj                     : pVoid;
        LoadModul                   : pVoid;
        LinkModules                 : pVoid;
        SetData                     : pVoid;
        GetData                     : pVoid;
        GetOsVersion                : pVoid;
        GetUserData                 : pVoid;
        
        // ab Version 3
        SetBreakPoint               : pVoid;
        Request                     : pVoid; //SteAnd --> for Lasal32 improvement
        SetSendInterval             : pVoid; //SteAnd --> for Lasal32 improvement

        // ab Version 4
        LoadModulClsName            : pVoid;

        // ab Version 5
        OS_reserved21               : pVoid;

        // ab Version 6
        GetObject                   : pVoid;
        ReadServer                  : pVoid;
        WriteServer                 : pVoid;

        // ab Version 7
        FileLoad                   : pVoid;

        // ab Version 8
        OS_reserved4                : pVoid;
        OS_reserved5                : pVoid;

    END_STRUCT;
END_TYPE
#pragma pack(pop)

FUNCTION GLOBAL __cdecl P_LSLONL_OPEN
VAR_INPUT
    addr        : ^OS_LSLONL_ADDR;
END_VAR
VAR_OUTPUT
    descr_num   : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_CLOSE
VAR_INPUT
    descr_num   : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_STRING2ONLADDR
VAR_INPUT
    string      : ^CHAR;
    addr        : ^OS_LSLONL_ADDR;
    rc          : ^DINT;
END_VAR
VAR_OUTPUT
    rest        : ^CHAR;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_GETCPUSTATUS
VAR_INPUT
    descr_num   : DINT;
    status      : ^USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_RESETSPS
VAR_INPUT
    descr_num   : DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_RUNSPS
VAR_INPUT
    descr_num   : DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_LINKMODULES
VAR_INPUT
    descr_num   : DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_CANCELREQUEST
VAR_INPUT
    descr_num   : DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_GETUSRDATA
VAR_INPUT
    descr_num       : DINT;
    UserDataStart   : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_GETOSVERSION
VAR_INPUT
    descr_num       : DINT;
    ver_major       : ^DINT;
    ver_minor       : ^DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_FILESAVE
VAR_INPUT
    descr_num       : DINT;
    dest            : ^CHAR;
    pData           : ^CHAR;
    len             : UDINT;
    LenAll          : UDINT;
    ffirst          : USINT;
    flast           : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;


FUNCTION GLOBAL __cdecl P_LSLONL_SETBREAKPOINT
VAR_INPUT
    descr_num       : DINT;
    nr              : DINT;
    addr            : UDINT;
    addrlen         : UDINT;
    typ             : DINT;
    stackoffset     : UDINT;
    stacksize       : UDINT;
    Counter         : UDINT;
    aktion          : DINT;
    pCode           : ^CHAR;
    codelen         : UDINT;

END_VAR
VAR_OUTPUT
    retval          : DINT;

END_VAR;


FUNCTION GLOBAL __cdecl P_LSLONL_FILEINFO
VAR_INPUT
    descr_num         : DINT;
    rxbuf             : ^CHAR;
    dest              : ^CHAR;
    buflen            : UDINT;
    BytesReturned     : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;


FUNCTION GLOBAL __cdecl P_LSLONL_LOADOSIMAGE
VAR_INPUT
    descr_num       : DINT;
    pData           : ^CHAR;
    len             : UDINT;
    LenAll          : UDINT;
    ffirst          : USINT;
    flast           : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_LOADMODUL
VAR_INPUT
    descr_num       : DINT;
    pProg           : ^USINT;
    len             : DINT;
    LenAll          : DINT;
    ffirst          : USINT;
    flast           : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;


FUNCTION GLOBAL __cdecl P_LSLONL_LOADMODULCLSNAME
VAR_INPUT
    descr_num       : DINT;
    pProg           : ^USINT;
    len             : DINT;
    LenAll          : DINT;
    ffirst          : USINT;
    flast           : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;


FUNCTION GLOBAL __cdecl P_LSLONL_BOOTOSIMAGE
VAR_INPUT
    descr_num       : DINT;

END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_LOADPRJ
VAR_INPUT
    descr_num       : DINT;
    prjname           : ^USINT;

END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_ERROR
VAR_INPUT
    rc          : DINT;
END_VAR
VAR_OUTPUT
    rest        : ^CHAR;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_GETOBJECT
VAR_INPUT
    descr_num       : DINT;
    objName         : ^CHAR;
    objAddr         : ^UDINT;
    pMode           : ^CHMODE;
    clsName         : ^CHAR;
    dataBufferFlag  : ^CHAR;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_READSERVER
VAR_INPUT
    descr_num       : DINT;
    objAddr         : UDINT;
    data            : ^UDINT;
    dwsize          : UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_REQUEST
VAR_INPUT
  descr_num       : DINT;
  Cmd             : USINT;
  InBuffer1       : ^USINT;
  InBuffer1Size   : UDINT;
  InBuffer2       : ^USINT;
  InBuffer2Size   : UDINT;
  InBufferSizeAll : UDINT;
  OutBuffer       : ^USINT;
  OutBufferSize   : UDINT;
  BytesReturend   : ^UDINT;
  CpuStatus       : ^USINT;
  fFirstRecPart   : USINT;
  fLastRecPart     : USINT;
END_VAR
VAR_OUTPUT
  retval : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_SETSENDINTERVAL
VAR_INPUT
  waittime_ms : DINT;
END_VAR
VAR_OUTPUT
  retval : DINT;
END_VAR;




FUNCTION GLOBAL __cdecl P_LSLONL_WRITESERVER
VAR_INPUT
    descr_num       : DINT;
    objAddr         : UDINT;
    data            : UDINT;
    result          : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_GETPLCINFO
VAR_INPUT
    descr_num         : DINT;
    infostr0          : ^CHAR;
    maxsize0          : UDINT;
    BytesReturned     : ^UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_SETDATA
VAR_INPUT
    descr_num         : DINT;
    pBuffer           : ^USINT;
    addr0             : UDINT;
    nCount            : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_GETDATA
VAR_INPUT
    descr_num         : DINT;
    pBuffer           : ^USINT;
    addr0             : UDINT;
    nCount            : USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_FILELOAD
VAR_INPUT
    descr_num         : DINT;
    pSrc              : ^CHAR;
    pBuffer           : ^CHAR;
    addr0             : UDINT;
    nCount            : UDINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLONL_SERVICEPROVIDER
VAR_INPUT
    descr_num         : DINT;
    cmd          : ^CHAR;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

#define OS_ONL_ERROR(p1)                                              pLslOnl^.OsError$ P_LSLONL_ERROR(p1)
#define OS_ONL_OPEN(p1)                                               pLslOnl^.open $ P_LSLONL_OPEN(p1)
#define OS_ONL_CLOSE(p1)                                              pLslOnl^.close $ P_LSLONL_CLOSE(p1)
#define OS_ONL_CANCELREQUEST(p1)                                      pLslOnl^.CancelRequest$ P_LSLONL_CANCELREQUEST(p1)
#define OS_ONL_STRING2ONLADDR(p1,p2,p3)                               pLslOnl^.string2onladdr $ P_LSLONL_STRING2ONLADDR(p1,p2,p3)
// OS_reserved3 for OS_ONL_RESET
#define OS_ONL_GETCPUSTATUS(p1,p2)                                    pLslOnl^.getcpustatus $ P_LSLONL_GETCPUSTATUS(p1,p2)
#define OS_ONL_RESETSPS(p1)                                           pLslOnl^.resetsps $ P_LSLONL_RESETSPS(p1)
#define OS_ONL_RUNSPS(p1)                                             pLslOnl^.runsps $ P_LSLONL_RUNSPS(p1)
#define OS_ONL_GETPLCINFO(p1,p2,p3,p4)                                pLslOnl^.GetPlcInfo $ P_LSLONL_GETPLCINFO(p1,p2,p3,p4)

// ab Version 2:
#define OS_ONL_SERVICEPROVIDER(p1,p2)                                 pLslOnl^.ServiceProvider $ P_LSLONL_SERVICEPROVIDER(p1,p2)
#define OS_ONL_FILEINFO(p1,p2,p3,p4,p5)                               pLslOnl^.FileInfo $ P_LSLONL_FILEINFO(p1,p2,p3,p4, p5)
#define OS_ONL_FILESAVE(p1,p2,p3,p4,p5,p6,p7)                         pLslOnl^.FileSave $ P_LSLONL_FILESAVE(p1,p2,p3,p4,p5,p6,p7)
#define OS_ONL_LOADOSIMAGE(p1,p2,p3,p4,p5, p6)                        pLslOnl^.LoadOsImage $ P_LSLONL_LOADOSIMAGE(p1,p2,p3,p4,p5, p6)
#define OS_ONL_BOOTOSIMAGE(p1)                                        pLslOnl^.BootOsImage $ P_LSLONL_BOOTOSIMAGE(p1)
#define OS_ONL_LOADPRJ(p1, p2)                                        pLslOnl^.LoadPrj $ P_LSLONL_LOADPRJ(p1, p2)
#define OS_ONL_LOADMODUL(p1,p2,p3,p4,p5, p6)                          pLslOnl^.LoadModul $ P_LSLONL_LOADMODUL(p1,p2,p3,p4,p5, p6)
#define OS_ONL_LINKMODULES(p1)                                        pLslOnl^.LinkModules $ P_LSLONL_LINKMODULES(p1)
#define OS_ONL_SETDATA(p1,p2,p3,p4)                                   pLslOnl^.SetData $ P_LSLONL_SETDATA(p1,p2,p3,p4)
#define OS_ONL_GETDATA(p1,p2,p3,p4)                                   pLslOnl^.GetData $ P_LSLONL_GETDATA(p1,p2,p3,p4)
#define OS_ONL_GETOSVERSION(p1,p2,p3)                                 pLslOnl^.GetOsVersion $ P_LSLONL_GETOSVERSION(p1,p2,p3)
#define OS_ONL_GETUSRDATA(p1, p2)                                     pLslOnl^.GetUserData $ P_LSLONL_GETUSRDATA(p1,p2)

// ab Version 3:
#define OS_ONL_SETBREAKPOINT(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)      pLslOnl^.SetBreakPoint $ P_LSLONL_SETBREAKPOINT(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
#define OS_ONL_REQUEST(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)    pLslOnl^.Request $ P_LSLONL_Request(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)
#define OS_ONL_SETSENDINTERVAL(p1)                                    pLslOnl^.SetSendInterval $ P_LSLONL_SETSENDINTERVAL(p1)

// ab Version 4:
#define OS_ONL_LOADMODULCLSNAME(p1,p2,p3,p4,p5, p6)                   pLslOnl^.LoadModulClsName $ P_LSLONL_LOADMODULCLSNAME(p1,p2,p3,p4,p5, p6)

// ab Version 5:
// OS_reserved21 for OS_ONL_REQUESTEXT

// ab Version 6:
#define OS_ONL_GETOBJECT(p1,p2,p3,p4,p5,p6)                           pLslOnl^.getobject $ P_LSLONL_GETOBJECT(p1,p2,p3,p4,p5,p6)
#define OS_ONL_READSERVER(p1,p2,p3,p4)                                pLslOnl^.readserver $ P_LSLONL_READSERVER(p1,p2,p3,p4)
#define OS_ONL_WRITESERVER(p1,p2,p3,p4)                               pLslOnl^.writeserver $ P_LSLONL_WRITESERVER(p1,p2,p3,p4)

//ab Version 7
#define OS_ONL_FILELOAD(p1,p2,p3,p4,p5)                            	  pLslOnl^.FileLoad $ P_LSLONL_FILELOAD(p1,p2,p3,p4,p5)

//HFILTER:1
#endif // _LSL_ST_ONL_H
//HFILTER:



