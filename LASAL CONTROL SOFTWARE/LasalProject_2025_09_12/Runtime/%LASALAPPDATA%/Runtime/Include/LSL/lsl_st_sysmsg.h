#ifndef _LSL_ST_SYSMSG_H
#pragma once
#define _LSL_ST_SYSMSG_H



TYPE
  LUSER_EXPORT_RESULT   : STRUCT
    rc            : DINT;
    LogFileID     : DINT;
    IsBackupFile  : USINT;
    pNext         :^LUSER_EXPORT_RESULT;
  END_STRUCT;
  PLUSER_EXPORT_RESULT  :^LUSER_EXPORT_RESULT;
END_TYPE


#pragma pack( push, 1 )
TYPE
    OS_LSLSYSMSG : STRUCT
        udVersion                 : UDINT;
        lcreate                   : pVoid;
        lopen                     : pVoid;
        lclose                    : pVoid;
        OS_reserved0              : pVoid;
        lwrite                    : pVoid;
        lwriteln                  : pVoid;
        lprintfln                 : pVoid;
        lwrite_i                  : pVoid;
        OS_reserved1              : pVoid;
        lflush                    : pVoid;
        OS_reserved2              : pVoid;
        OS_reserved3              : pVoid;
        linfo                     : pVoid;
        // USER LOG
        plprintfln_userlog        : pVoid;
        plwrite_event_userlog     : pVoid;
        plflush_userlog           : pVoid;
        plfileheader_userlog      : pVoid;
        plexport_userlog          : pVoid;
        plastexporterror_userlog  : pVoid;
        OS_reserved4			        : pVoid;
    END_STRUCT;
END_TYPE
#pragma pack(pop)

// USER LOG
#define LUSER_LOW_PRIO_ID        1
#define LUSER_HIGH_PRIO_ID       2

// USER LOG ERROR CODES
#define LUSER_NO_ERR             0
#define LUSER_INVALID_HANDLE    -1
#define LUSER_INVALID_ID        -2
#define LUSER_NO_IFACE          -3
#define LUSER_INTERN_ERR        -4
#define LUSER_FLUSH_ERR         -5
#define LUSER_EXPORT_ERR        -6
#define LUSER_FILE_ERR          -7
#define LUSER_NO_PATH           -8
#define LUSER_PTR_NIL           -9
#define LUSER_BUSY              -10
#define LUSER_BUFFTOBIG         -11

// Reserved IDs for Userlog-Files (nID Parameter OS_SYSMSG_LCREATE)
//  9   (event19.log) .. used by Loader
//  8,7 (event18.log, event17.log) .. reserved by Loader
//  6   (event16.log) .. reserved by software paket 'industry 4.0'
FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LCREATE
VAR_INPUT
    nID         : DINT;
    Buffer      : ^CHAR;
    BufferSize  : UDINT;
    file_quota  : DINT;
    flags       : UDINT;
END_VAR
VAR_OUTPUT
    hLog        : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LOPEN
VAR_INPUT
    nID         : DINT;
END_VAR
VAR_OUTPUT
    hLog        : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LCLOSE
VAR_INPUT
    hLog        : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LWRITE
VAR_INPUT
    hLog          : UDINT;
    fAddTimestamp : UDINT;
    data          : ^CHAR;                  
    size          : UDINT;
END_VAR
VAR_OUTPUT
    result      : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LWRITELN
VAR_INPUT
    hLog          : UDINT;
    fAddTimestamp : UDINT;
    txt           : ^CHAR;                  
END_VAR
VAR_OUTPUT
    result      : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LPRINTFLN
VAR_INPUT
    hLog          : UDINT;
    fAddTimestamp : UDINT;
    msg           : ^CHAR;                  
    lpar1         : UDINT;
    lpar2         : UDINT;
    lpar3         : UDINT;
    lpar4         : UDINT;
END_VAR
VAR_OUTPUT
    result      : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LWRITE_I
VAR_INPUT
    hLog          : UDINT;
    txt           : ^CHAR;                  
    lpar1         : UDINT;
    lpar2         : UDINT;
    lpar3         : UDINT;
    lpar4         : UDINT;
END_VAR
VAR_OUTPUT
    result      : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LFLUSH
VAR_INPUT
    hLog          : UDINT;
END_VAR
VAR_OUTPUT
    result      : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LINFO
VAR_INPUT
    hLog          : UDINT;
    buf_addr      : ^UDINT;
    pos           : ^UDINT;
    bufsize       : ^UDINT;
    n_unflushed   : ^UDINT;
END_VAR
VAR_OUTPUT
    result      : UDINT;
END_VAR;

// USER LOG
FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LUSER_PRINTFLN
VAR_INPUT
    msg       :^CHAR;
    param1    : DINT;
    param2    : DINT;
END_VAR
VAR_OUTPUT
    rc        : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LUSER_WRITEEVENT
VAR_INPUT
    hID0      : USINT;
    scancode  : UINT;
    picnum    : UINT;
    X         : UINT;
    Y         : UINT;
END_VAR
VAR_OUTPUT
    rc        : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LUSER_FLUSH
VAR_INPUT
    LogFileID     : DINT;
    bNonBlocking  : USINT;
END_VAR
VAR_OUTPUT
    rc            : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LUSER_FILEHEADER
VAR_INPUT
    str           :^CHAR;
    LogFileID     : DINT;
    bNonBlocking  : USINT;
END_VAR
VAR_OUTPUT
    rc            : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LUSER_EXPORT
VAR_INPUT
    path      :^CHAR;
END_VAR
VAR_OUTPUT
    rc        : DINT;
END_VAR;


FUNCTION GLOBAL __cdecl P_LSLSYSMSG_LUSER_LAST_EXPORT_RESULT
VAR_INPUT
    PtrExpResult  :^PLUSER_EXPORT_RESULT;
END_VAR
VAR_OUTPUT
    errcount  : DINT;
END_VAR;

#define OS_SYSMSG_LCREATE(p1,p2,p3,p4,p5)  OS_pLslSysMsg^.lcreate $ P_LSLSYSMSG_LCREATE(p1,p2,p3,p4,p5)
#define OS_SYSMSG_LOPEN(p1)             OS_pLslSysMsg^.lopen $ P_LSLSYSMSG_LOPEN(p1)
#define OS_SYSMSG_LCLOSE(p1)            OS_pLslSysMsg^.lclose $ P_LSLSYSMSG_LCLOSE(p1)
#define OS_SYSMSG_LWRITE(p1,p2,p3,p4)   OS_pLslSysMsg^.lwrite $ P_LSLSYSMSG_LWRITE(p1,p2,p3,p4)
#define OS_SYSMSG_LWRITELN(p1,p2,p3)    OS_pLslSysMsg^.lwriteln $ P_LSLSYSMSG_LWRITELN(p1,p2,p3)
#define OS_SYSMSG_LPRINTFLN1(p1,p2,p3,p4)          OS_pLslSysMsg^.lprintfln $ P_LSLSYSMSG_LPRINTFLN(p1,p2,p3,p4,0 ,0 ,0 )
#define OS_SYSMSG_LPRINTFLN2(p1,p2,p3,p4,p5)       OS_pLslSysMsg^.lprintfln $ P_LSLSYSMSG_LPRINTFLN(p1,p2,p3,p4,p5,0 ,0 )
#define OS_SYSMSG_LPRINTFLN3(p1,p2,p3,p4,p5,p6)    OS_pLslSysMsg^.lprintfln $ P_LSLSYSMSG_LPRINTFLN(p1,p2,p3,p4,p5,p6,0 )
#define OS_SYSMSG_LPRINTFLN4(p1,p2,p3,p4,p5,p6,p7) OS_pLslSysMsg^.lprintfln $ P_LSLSYSMSG_LPRINTFLN(p1,p2,p3,p4,p5,p6,p7)
#define OS_SYSMSG_LWRITE_I0(p1,p2)              OS_pLslSysMsg^.lwrite_i $ P_LSLSYSMSG_LWRITE_I(p1,p2,0 ,0 ,0 ,0 )
#define OS_SYSMSG_LWRITE_I1(p1,p2,p3)           OS_pLslSysMsg^.lwrite_i $ P_LSLSYSMSG_LWRITE_I(p1,p2,p3,0 ,0 ,0 )
#define OS_SYSMSG_LWRITE_I2(p1,p2,p3,p4)        OS_pLslSysMsg^.lwrite_i $ P_LSLSYSMSG_LWRITE_I(p1,p2,p3,p4,0 ,0 )
#define OS_SYSMSG_LWRITE_I3(p1,p2,p3,p4,p5)     OS_pLslSysMsg^.lwrite_i $ P_LSLSYSMSG_LWRITE_I(p1,p2,p3,p4,p5,0 )
#define OS_SYSMSG_LWRITE_I4(p1,p2,p3,p4,p5,p6)  OS_pLslSysMsg^.lwrite_i $ P_LSLSYSMSG_LWRITE_I(p1,p2,p3,p4,p5,p6)
#define OS_SYSMSG_LFLUSH(p1)            OS_pLslSysMsg^.lflush $ P_LSLSYSMSG_LFLUSH(p1)
#define OS_SYSMSG_LINFO(p1,p2,p3,p4,p5) OS_pLslSysMsg^.linfo $ P_LSLSYSMSG_LINFO(p1,p2,p3,p4,p5)
// USER LOG
#define OS_USERLOG_PRINTFLN(p1)        OS_pLslSysMsg^.plprintfln_userlog $ P_LSLSYSMSG_LUSER_PRINTFLN(p1, 0, 0)
#define OS_USERLOG_PRINTFLN1(p1,p2)    OS_pLslSysMsg^.plprintfln_userlog $ P_LSLSYSMSG_LUSER_PRINTFLN(p1,p2, 0)
#define OS_USERLOG_PRINTFLN2(p1,p2,p3) OS_pLslSysMsg^.plprintfln_userlog $ P_LSLSYSMSG_LUSER_PRINTFLN(p1,p2,p3)
#define OS_USERLOG_WRITEEVENT(p1,p2,p3,p4,p5) OS_pLslSysMsg^.plwrite_event_userlog $ P_LSLSYSMSG_LUSER_WRITEEVENT(p1,p2,p3,p4,p5)
#define OS_USERLOG_FLUSH(p1,p2) OS_pLslSysMsg^.plflush_userlog $ P_LSLSYSMSG_LUSER_FLUSH(p1,p2)
#define OS_USERLOG_FILEHEADER(p1,p2,p3) OS_pLslSysMsg^.plfileheader_userlog $ P_LSLSYSMSG_LUSER_FILEHEADER(p1,p2,p3)
#define OS_USERLOG_EXPORT(p1) OS_pLslSysMsg^.plexport_userlog $ P_LSLSYSMSG_LUSER_EXPORT(p1)
#define OS_USERLOG_LAST_EXPORT_RESULT(p1) OS_pLslSysMsg^.plastexporterror_userlog $ P_LSLSYSMSG_LUSER_LAST_EXPORT_RESULT(p1)

#endif
