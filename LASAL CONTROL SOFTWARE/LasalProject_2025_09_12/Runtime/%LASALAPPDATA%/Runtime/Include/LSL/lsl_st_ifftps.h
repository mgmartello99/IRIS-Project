#ifndef __LSL_ST_IFFTPS
#pragma once
#define __LSL_ST_IFFTPS

//Static options for SetOption functions:
#define	FTPS_CCC_NONE				0
#define	FTPS_CCC_PASSIVE			1
#define	FTPS_CCC_ACTIVE				2
#define	FTPS_IMPLICIT				0
#define	FTPS_EXPLICIT				1
#define FTPS_TRANSFER_BINARY		0
#define FTPS_TRANSFER_ASCII			1
#define FTPS_VERIFYHOST_LENIENT		0
#define FTPS_VERIFYHOST_STRICT		1
#define FTPS_VERIFYPEER_LENIENT		0
#define FTPS_VERIFYPEER_STRICT		1
#define FTPS_TCPPROBES_OFF			0
#define FTPS_TCPPROBES_ON			1
#define FTPS_CLOSERESPONSE_OFF		0
#define FTPS_CLOSERESPONSE_ON		1
#define FTPS_CONNTYPE_PASSIVE		0
#define FTPS_CONNTYPE_ACTIVE		1
#define FTPS_TYPE_IMPLICIT			0
#define FTPS_TYPE_EXPLICIT			1

// FTPS error codes
#define FTPSCLI_ERR_NONE		 	0	//no error
#define FTPSCLI_ERR_UNDEF			-1	//not yet defined error
#define FTPSCLI_ERR_LOGIN_DENIED	-3	//login denied
#define FTPSCLI_ERR_COULDNT_CONNECT	-4	//could not connect
#define FTPSCLI_ERR_ABORT			-5	//aborted by user callback
#define FTPSCLI_ERR_WRITE			-7	//error from write file
#define FTPSCLI_ERR_NOCLIENT		-20	//no client
#define FTPSCLI_ERR_FILENAME		-21	//filename error
#define FTPSCLI_ERR_POINTER			-1000   //invalid pointer
#define FTPSCLI_ERR_CFG_OPTION		-1001   //wrong config option
#define FTPSCLI_ERR_CFG_VALUE		-1002   //wrong parameter
#define FTPSCLI_ERR_INVALID_THIS	-1003	//this pointer is empty
#define FTPSCLI_ERR_INVALID_CB		-1004	//invalid callback function
#define FTPSCLI_ERR_IS_CONNECTED	-1005	//client is already connected
#define FTPSCLI_ERR_NOT_CONNECTED	-1006	//client is not connected
#define FTPSCLI_ERR_MEMORY			-1007	//memory error



#pragma pack (push,1)

TYPE  
  
  //SetOption Enumerator
  ftps_Options :  
  (
    _FTPS_Username,
    _FTPS_Password,
    _FTPS_URL,
    _FTPS_Port,
    _FTPS_SSL_Certificate,
    _FTPS_SSL_Key,
    _FTPS_SSL_KeyPassword,
    _FTPS_SSL_CABundle,
    _FTPS_SSL_VerifyHost,
    _FTPS_SSL_VerifyPeer,
    _FTPS_SSL_CCC,
    _FTPS_TCP_KeepAlive,
    _FTPS_TCP_KeepIdle,
    _FTPS_TCP_KeepInterval,
    _FTPS_TIMEOUT_Connect,
    _FTPS_TIMEOUT_Send,
    _FTPS_TIMEOUT_Receive,
    _FTPS_TLS_Username,
    _FTPS_TLS_Password,
    _FTPS_CloseResponse,
    _FTPS_ConnType,
    _FTPS_EPSV,
    _FTPS_FTPS_Type,
    _FTPS_TransferType
  )$UDINT;
  

  //CIL Declaration for the FTPS API
  LSL_FTPS_API_CLIENT : STRUCT    
    version:                UDINT;
    size:                   UDINT;      
    pftps_CreateConn:       pvoid;
    pftps_SetOptionString:  pvoid;
    pftps_SetOptionInt:     pvoid;
    pftps_GetOptionString:  pvoid;
    pftps_GetOptionInt:     pvoid;
    pftps_Connect:          pvoid;
    pftps_XConnect:         pvoid;
    pftps_CWD:              pvoid;
    pftps_PWD:              pvoid;
    pftps_MKDIR:            pvoid;
    pftps_RMDIR:            pvoid;
    pftps_DELE:             pvoid;
    pftps_BYE:              pvoid;
    pftps_LIST:             pvoid;
    pftps_NLIST:            pvoid;
    pftps_PUT:              pvoid;
    pftps_APPEND:           pvoid;
    pftps_GET:              pvoid;
    pftps_REN:              pvoid;
    pftps_CloseAllSessions: pvoid;    
  END_STRUCT;
  
END_TYPE

#pragma pack (pop)




//CreateConn:	Currently, max. 4 connections are simultaneously possible.
//			This function looks for an empty connection slot and
//			returns, if one is available, the handle as address.
//			If none is available or an error occured, returns NULL.
//
//void **(*pftps_CreateConn)(void);
FUNCTION GLOBAL __cdecl P_FTPS_CreateConn
VAR_OUTPUT 
	ppHandle	: ^pVoid;
END_VAR;

//SetOptionString:	Set options that require a string for the specified handle.
//			Choose the option in the above declared enum. Pass the value
//			as string in double quotes. Returns FTPSCLI_ERR_NOCLIENT if
//			connection is faulty or not available, returns
//			FTPSCLI_ERR_IS_CONNECTED if a connection is already established,
//			returns FTPSCLI_ERR_CFG_OPTION if a wrong option is specified.
//
//long (*pftps_SetOptionString)(void **handle, ftps_Options option, const char* value);
FUNCTION GLOBAL __cdecl P_FTPS_SetOptionString
VAR_INPUT
    ppHandle : ^pVoid;
	option : ftps_Options;
    pValue : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//SetOptionInt:		Set options that require an integer for the specified handle.
//			Choose the option in the above declared enum. Pass the value
//			as integer. Returns FTPSCLI_ERR_NOCLIENT if
//			connection is faulty or not available, returns
//			FTPSCLI_ERR_IS_CONNECTED if a connection is already established,
//			returns FTPSCLI_ERR_CFG_OPTION if a wrong option is specified.
//
//long (*pftps_SetOptionInt)(void **handle, ftps_Options option, const long value);
FUNCTION GLOBAL __cdecl P_FTPS_SetOptionInt
VAR_INPUT
    ppHandle : ^pVoid;
	option : ftps_Options;
    Value : DINT;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//GetOptionString:	Read the value of an option for the specified handle. Choose the
//			option in the above declared enum. Pass a buffer to store the
//			value in and specify the maximum amount of characters to be
//			written. Returns FTPSCLI_ERR_NOCLIENT if
//			connection is faulty or not available,
//			returns FTPSCLI_ERR_CFG_OPTION if a wrong option is specified.
//
//long (*pftps_GetOptionString)(void **handle, ftps_Options option, void *buffer, long nBuffer);
FUNCTION GLOBAL __cdecl P_FTPS_GetOptionString
VAR_INPUT
    ppHandle : ^pVoid;
	option : ftps_Options;
    pBuffer : ^char;
	nBuffer : DINT;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//GetOptionInt:		Read the value of an option for the specified handle. Choose the
//			option in the above declared enum. Pass the address of an integer
//			variable to store the value in. Returns FTPSCLI_ERR_NOCLIENT if
//			connection is faulty or not available, returns
//			FTPSCLI_ERR_CFG_OPTION if a wrong option is specified.
//
//long (*pftps_GetOptionInt)(void **handle, ftps_Options option, long *var);
FUNCTION GLOBAL __cdecl P_FTPS_GetOptionInt
VAR_INPUT
    ppHandle : ^pVoid;
	option : ftps_Options;
    pValue : ^DINT;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//Connect:	Connect the specified handle with the defined parameters. Returns
//			the socket number if executed properly, returns
//			FTPSCLI_ERR_LOGIN_DENIED if login was denied, returns
//			FTPSCLI_ERR_UNDEF if an other error occured. For error details,
//			view salamander-log.txt
//long (*pftps_Connect)(void **handle)
FUNCTION GLOBAL __cdecl P_FTPS_Connect
VAR_INPUT
    ppHandle : ^pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//XConnect:	Connect the specified handle with the defined parameters. Possibility
//			to pass a callback function with parameters. Returns the socket number
//			if executed properly, returns FTPSCLI_ERR_NOCLIENT if connection
//			is faulty or not available,returns FTPSCLI_ERR_LOGIN_DENIED if login was
//			denied, returns	FTPSCLI_ERR_UNDEF if an other error occured. For error
//			details, view salamander-log.txt
//long (*pftps_XConnect)(void **handle,	void (*response_func)(void* user_arg, char* wasned, int wasnix), void* user_arg);
FUNCTION GLOBAL __cdecl P_FTPS_XConnect
VAR_INPUT
    ppHandle : ^pVoid;
    response_func : pVoid;
    user_arg : pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//CWD:		Change working directory while connected. Pass the directory name to
//			switch to in double quotes. Returns FTPSCLI_ERR_NONE if working
//			correctly, returns FTPSCLI_ERR_NOCLIENT if connection is faulty or not
//			available, returns FTPSCLI_ERR_NOT_CONNECTED if not connected, returns
//			FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_CWD)(void **handle, char* dir);
FUNCTION GLOBAL __cdecl P_FTPS_CWD
VAR_INPUT
    ppHandle : ^pVoid;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//PWD:		Writes current directory to specified buffer. Max. nBuf characters are
//			copied. Returns FTPSCLI_ERR_NONE if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected.
//long (*pftps_PWD)(void **handle, char *buf);
FUNCTION GLOBAL __cdecl P_FTPS_PWD
VAR_INPUT
    ppHandle : ^pVoid;
    buf   : ^CHAR;
    nBuf  : DINT;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//MKDIR:	Creates a directory with the specified name in the working directory.
//			Returns FTPSCLI_ERR_NONE if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns
//			FTPSCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_MKDIR)(void **handle, char *dirname);
FUNCTION GLOBAL __cdecl P_FTPS_MKDIR
VAR_INPUT
    ppHandle : ^pVoid;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//RMDIR:	Deletes the directory with the specified name in the working directory.
//			Returns FTPSCLI_ERR_NONE if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns
//			FTPSCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_RMDIR)(void **handle, char *dirname);
FUNCTION GLOBAL __cdecl P_FTPS_RMDIR
VAR_INPUT
    ppHandle : ^pVoid;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//DELE:		Deletes the file with the specified name in the working directory.
//			Returns FTPSCLI_ERR_NONE if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns
//			FTPSCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_DELE)(void **handle, char *filename);
FUNCTION GLOBAL __cdecl P_FTPS_DELE
VAR_INPUT
    ppHandle : ^pVoid;
    file : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//BYE:		Closes the connection and frees the connection (parameters are lost).
//			Returns FTPSCLI_ERR_NONE if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns
//			FTPSCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_BYE)(void **handle);
FUNCTION GLOBAL __cdecl P_FTPS_BYE
VAR_INPUT
    ppHandle : ^pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//LIST:		Lists all files in the specified directory by executing the user_function.
//			Returns FTPSCLI_ERR_NONE if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns FTPSCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns FTPSCLI_ERR_INVALID_CB if callback
//			function is invalid, returns FTPSCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_LIST)(void **handle, char* path, int (*user_function)(char*, int));
FUNCTION GLOBAL __cdecl P_FTPS_LIST
VAR_INPUT
    ppHandle : ^pVoid;
    path : ^CHAR;
    userfunc : pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//NLIST:	Lists all files and directories in the specified directory by executing
//			the user_function. Returns FTPSCLI_ERR_NONE if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns FTPSCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns FTPSCLI_ERR_INVALID_CB if callback
//			function is invalid, returns FTPSCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_NLIST)(void **handle, char * path, int (*user_function)(char *, int));
FUNCTION GLOBAL __cdecl P_FTPS_NLIST
VAR_INPUT
    ppHandle : ^pVoid;
    path : ^CHAR;
    userfunc : pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//PUT:		Copies file with specified filename to the working directory by executing the
//			user_function. Returns filesize if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns FTPSCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns FTPSCLI_ERR_INVALID_CB if callback
//			function is invalid, returns FTPSCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns FTPSCLI_ERR_ABORT if the user aborted the transfer,
//			returns FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_PUT)(void **handle, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
FUNCTION GLOBAL __cdecl P_FTPS_PUT
VAR_INPUT
    ppHandle : ^pVoid;
    file : ^CHAR;
    userfunc : pVoid;
    userpoint : pVoid;
END_VAR
VAR_OUTPUT 
	ret0      : DINT;
END_VAR;

//APPEND:	Appends file with specified filename to the working directory by executing the
//			user_function. Returns filesize if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns FTPSCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns FTPSCLI_ERR_INVALID_CB if callback
//			function is invalid, returns FTPSCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns FTPSCLI_ERR_ABORT if the user aborted the transfer,
//			returns FTPSCLI_ERR_UNDEF if another error occured.
//long (*pftps_APPEND)(void **handle, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
FUNCTION GLOBAL __cdecl P_FTPS_APPEND
VAR_INPUT
    ppHandle : ^pVoid;
    file : ^CHAR;
    userfunc : pVoid;
    userpoint : pVoid;
END_VAR
VAR_OUTPUT 
	ret0      : DINT;
END_VAR;

//GET:		Downloads the file with specified filename to the working directory by executing
//			the user_function. Returns filesize if executed correctly, returns//
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns FTPSCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns FTPSCLI_ERR_INVALID_CB if callback
//			function is invalid, returns FTPSCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns FTPSCLI_ERR_ABORT if the user aborted the transfer, returns
//			FTPSCLI_ERR_WRITE if write callback returned an error, returns FTPSCLI_ERR_UNDEF
//			if another error occured.
//long (*pftps_GET)(void **handle, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
FUNCTION GLOBAL __cdecl P_FTPS_GET
VAR_INPUT
    ppHandle : ^pVoid;
    file : ^CHAR;
    userfunc : pVoid;
    userpoint : pVoid;
END_VAR
VAR_OUTPUT 
	ret0      : DINT;
END_VAR;

//REN:		Renames the file with specified filename to the new filename Returns
//			FTPSCLI_ERR_NONE if executed correctly, returns	FTPSCLI_ERR_NOCLIENT
//			if connection is faulty or not available, returns FTPSCLI_ERR_NOT_CONNECTED
//			if not connected. Returns FTPSCLI_ERR_FILENAME if a filename is invalid
//			returns FTPSCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			FTPSCLI_ERR_UNDEF if another error occured.
//
//long (*pftps_REN)(void **handle, char *from0, char *to0);
FUNCTION GLOBAL __cdecl P_FTPS_REN
VAR_INPUT
  ppHandle : ^pVoid;
  from0     :^CHAR;
  to0       :^CHAR;
END_VAR
VAR_OUTPUT
  ret0      : DINT;
END_VAR;

//CloseAllSessions:	Closes all connections and frees the connections (parameters are lost).
//			Returns FTPSCLI_ERR_NONE if executed correctly, returns
//			FTPSCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			FTPSCLI_ERR_NOT_CONNECTED if not connected. Returns
//			FTPSCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			FTPSCLI_ERR_UNDEF if another error occured.
//
//long (*pftps_CloseAllSessions)(void);
FUNCTION GLOBAL __cdecl P_FTPS_CLOSE
VAR_OUTPUT
  ret0      : DINT;
END_VAR;


//
//  INTERFACE MAKROS for the FTPS API
//
#define OS_FTPS_CREATE_CONN()                piFTPS^.pftps_CreateConn $ P_FTPS_CreateConn()
#define OS_FTPS_SET_OPT_STR(p1,p2,p3)        piFTPS^.pftps_SetOptionString $ P_FTPS_SetOptionString(p1,p2,p3)
#define OS_FTPS_SET_OPT_INT(p1,p2,p3)        piFTPS^.pftps_SetOptionInt $ P_FTPS_SetOptionInt(p1,p2,p3)
#define OS_FTPS_GET_OPT_STR(p1,p2,p3,p4)     piFTPS^.pftps_GetOptionString $ P_FTPS_GetOptionString(p1,p2,p3,p4)
#define OS_FTPS_GET_OPT_INT(p1,p2,p3)        piFTPS^.pftps_GetOptionInt $ P_FTPS_GetOptionInt(p1,p2,p3)
#define OS_FTPS_Connect(p1)                  piFTPS^.pftps_Connect $ P_FTPS_Connect(p1)
#define OS_FTPS_XConnect(p1,p2,p3)           piFTPS^.pftps_XConnect $ P_FTPS_XConnect(p1,p2,p3)
#define OS_FTPS_CWD(p1,p2)                   piFTPS^.pftps_CWD $ P_FTPS_CWD(p1,p2)
#define OS_FTPS_PWD(p1,p2,p3)                piFTPS^.pftps_PWD $ P_FTPS_PWD(p1,p2,p3)
#define OS_FTPS_MKDIR(p1,p2)                 piFTPS^.pftps_MKDIR $ P_FTPS_MKDIR(p1,p2)
#define OS_FTPS_RMDIR(p1,p2)                 piFTPS^.pftps_RMDIR $ P_FTPS_RMDIR(p1,p2)
#define OS_FTPS_DELE(p1,p2)                  piFTPS^.pftps_DELE $ P_FTPS_DELE(p1,p2)
#define OS_FTPS_BYE(p1)                      piFTPS^.pftps_BYE $ P_FTPS_BYE(p1)
#define OS_FTPS_LIST(p1,p2,p3)               piFTPS^.pftps_LIST $ P_FTPS_LIST(p1,p2,p3)
#define OS_FTPS_NLIST(p1,p2,p3)              piFTPS^.pftps_NLIST $ P_FTPS_NLIST(p1,p2,p3)
#define OS_FTPS_PUT(p1,p2,p3,p4)             piFTPS^.pftps_PUT $ P_FTPS_PUT(p1,p2,p3,p4)
#define OS_FTPS_APPEND(p1,p2,p3,p4)          piFTPS^.pftps_APPEND $ P_FTPS_APPEND(p1,p2,p3,p4)
#define OS_FTPS_GET(p1,p2,p3,p4)             piFTPS^.pftps_GET $ P_FTPS_GET(p1,p2,p3,p4)

#define OS_FTPS_REN(p1,p2,p3)                piFTPS^.pftps_REN $ P_FTPS_REN(p1,p2,p3)
#define OS_FTPS_CLOSE()                      piFTPS^.pftps_CloseAllSessions $ P_FTPS_CLOSE()

#define OS_FTPS_SIZE                         piFTPS^.size
#define OS_FTPS_VERSION                      piFTPS^.version

#endif
