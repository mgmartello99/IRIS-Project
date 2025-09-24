#ifndef __LSL_ST_IFFTP
#pragma once
#define __LSL_ST_IFFTP

// FTP Client Configuration Types
#define FTPCLI_CFG_CONNECT_TMO      1   // to set the timeout for connect
#define FTPCLI_CFG_SEND_TMO         2   // to set the timeout for send
#define FTPCLI_CFG_RECV_TMO         3   // to set the timeout for receive
#define FTPCLI_CFG_CLOSE_NO_RESP    4   // to set the socket to close without a communication to the server
#define FTPCLI_CFG_FREEOLDSOCKETS   5   // free sockets after closesocket (xn_abort) // 22.11.10
#define FTPCLI_CFG_CONNTYPE         6   // to set connection type: 0=PASSIVE, 1=ACTIVE
#define FTPCLI_CFG_EPSV             7   // to enable/disable use of EPSV: 0=PSV, 1=EPSV

// FTP error codes
#define FTPCLI_ERR_POINTER          -1000
#define FTPCLI_ERR_CFG_OPTION       -1001
#define FTPCLI_ERR_CFG_VALUE        -1002


//long (* pftp_Connect)(unsigned long host, char *username, char *password);
FUNCTION GLOBAL __cdecl P_FTP_Connect
VAR_INPUT
	ftphost    : UDINT;
    user : ^CHAR;
    pass : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_XConnect)(unsigned long host, unsigned short port, char *username, char *password);
FUNCTION GLOBAL __cdecl P_FTP_XConnect
VAR_INPUT
	ftphost    : UDINT;
  	ftpport    : UINT;
    user : ^CHAR;
    pass : ^CHAR;
    response_func : pVoid;
    user_arg : pVoid;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_CWD)(long ftpsocket, char* dir);
FUNCTION GLOBAL __cdecl P_FTP_CWD
VAR_INPUT
	ftpsock    : DINT;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_PWD)(long ftpsocket, char *buf);
FUNCTION GLOBAL __cdecl P_FTP_PWD
VAR_INPUT
	ftpsock    : DINT;
    buf : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_MKDIR)(long ftpsocket, char *dirname);
FUNCTION GLOBAL __cdecl P_FTP_MKDIR
VAR_INPUT
	ftpsock    : DINT;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_RMDIR)(long ftpsocket, char *dirname);
FUNCTION GLOBAL __cdecl P_FTP_RMDIR
VAR_INPUT
	ftpsock    : DINT;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_DELE)(long ftpsocket,  char *filename);
FUNCTION GLOBAL __cdecl P_FTP_DELE
VAR_INPUT
	ftpsock    : DINT;
    file : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_BYE)(long ftpsocket);
FUNCTION GLOBAL __cdecl P_FTP_BYE
VAR_INPUT
	ftpsock    : DINT;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_LIST)(long ftpsocket, char* path, int (*user_function)(char*, int));
FUNCTION GLOBAL __cdecl P_FTP_LIST
VAR_INPUT
	ftpsock    : DINT;
    path : ^CHAR;
    userfunc : pVoid;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_NLIST)(long ftpsocket, int list, char * path, int (*user_function)( char *, int));
FUNCTION GLOBAL __cdecl P_FTP_NLIST
VAR_INPUT
	ftpsock    : DINT;
    path : ^CHAR;
    userfunc : pVoid;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_PUT)(long ftpsocket, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
FUNCTION GLOBAL __cdecl P_FTP_PUT
VAR_INPUT
	ftpsock    : DINT;
    file : ^CHAR;
    userfunc : pVoid;
    userpoint : pVoid;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_FTP_APPEND
VAR_INPUT
	ftpsock    : DINT;
    file : ^CHAR;
    userfunc : pVoid;
    userpoint : pVoid;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_GET)(long ftpsocket, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
FUNCTION GLOBAL __cdecl P_FTP_GET
VAR_INPUT
	ftpsock    : DINT;
    file : ^CHAR;
    userfunc : pVoid;
    userpoint : pVoid;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long (* pftp_TRANSFTYPE)(long ftpsocket, char type);
FUNCTION GLOBAL __cdecl P_FTP_TRANSFTYPE
VAR_INPUT
	ftpsock    : DINT;
    type0 : SINT;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//long ftp_REN(long ftpsocket, char *from0, char *to0)
FUNCTION GLOBAL __cdecl P_FTP_REN
VAR_INPUT
  ftpsock   : DINT;
  from0     :^CHAR;
  to0       :^CHAR;
END_VAR
VAR_OUTPUT
  ret0      : DINT;
END_VAR;

//long ftp_ClientCfg(unsigned long ulCfgOption, void * pValue, void * ppValue)
FUNCTION GLOBAL __cdecl P_FTP_CLI_CONFIG
VAR_INPUT
  udCfgOption : UDINT;
  pValue      : PVOID;
  ppValue     :^PVOID;
END_VAR
VAR_OUTPUT
  ret0        : DINT;
END_VAR;

//
//  INTERFACE MAKROS FOR the FTP API
//
#define OS_FTP_Connect(p1,p2,p3)            _LSL_POS^.piFTP^.pftp_Connect $ P_FTP_Connect(p1,p2,p3)
#define OS_FTP_XConnect(p1,p2,p3,p4,p5,p6)  _LSL_POS^.piFTP^.pftp_XConnect $ P_FTP_XConnect(p1,p2,p3,p4,p5,p6)
#define OS_FTP_CWD(p1,p2)                   _LSL_POS^.piFTP^.pftp_CWD $ P_FTP_CWD(p1,p2)
#define OS_FTP_PWD(p1,p2)                   _LSL_POS^.piFTP^.pftp_PWD $ P_FTP_PWD(p1,p2)
#define OS_FTP_MKDIR(p1,p2)                 _LSL_POS^.piFTP^.pftp_MKDIR $ P_FTP_MKDIR(p1,p2)
#define OS_FTP_RMDIR(p1,p2)                 _LSL_POS^.piFTP^.pftp_RMDIR $ P_FTP_RMDIR(p1,p2)
#define OS_FTP_DELE(p1,p2)                  _LSL_POS^.piFTP^.pftp_DELE $ P_FTP_DELE(p1,p2)
#define OS_FTP_BYE(p1)                      _LSL_POS^.piFTP^.pftp_BYE $ P_FTP_BYE(p1)
#define OS_FTP_LIST(p1,p2,p3)               _LSL_POS^.piFTP^.pftp_LIST $ P_FTP_LIST(p1,p2,p3)
#define OS_FTP_NLIST(p1,p2,p3)              _LSL_POS^.piFTP^.pftp_NLIST $ P_FTP_NLIST(p1,p2,p3)
#define OS_FTP_PUT(p1,p2,p3,p4)             _LSL_POS^.piFTP^.pftp_PUT $ P_FTP_PUT(p1,p2,p3,p4)
#define OS_FTP_APPEND(p1,p2,p3,p4)          _LSL_POS^.piFTP^.pftp_APPEND $ P_FTP_APPEND(p1,p2,p3,p4)
#define OS_FTP_GET(p1,p2,p3,p4)             _LSL_POS^.piFTP^.pftp_GET $ P_FTP_GET(p1,p2,p3,p4)
#define OS_FTP_XFERTYPE(p1,p2)              _LSL_POS^.piFTP^.pftp_TRANSFTYPE $ P_FTP_TRANSFTYPE(p1,p2)

#define OS_FTP_SIZE                         _LSL_POS^.piFTP^.size
#define OS_FTP_VERSION                      _LSL_POS^.piFTP^.version

// ab version 1
#define OS_FTP_REN(p1,p2,p3)                _LSL_POS^.piFTP^.pftp_REN $ P_FTP_REN(p1,p2,p3)

// ab version 2
#define OS_FTP_ClientConfig(p1,p2,p3)       _LSL_POS^.piFTP^.pftp_ClientCfg $ P_FTP_CLI_CONFIG(p1,p2,p3)


#endif
