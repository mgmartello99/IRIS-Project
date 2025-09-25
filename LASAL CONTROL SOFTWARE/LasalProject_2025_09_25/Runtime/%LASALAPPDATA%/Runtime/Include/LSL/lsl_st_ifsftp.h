#ifndef __LSL_ST_IFSFTP
#pragma once
#define __LSL_ST_IFSFTP

//Static options for SetOption functions:
#define	SFTP_SSH_AUTH_ANY		0
#define	SFTP_SSH_AUTH_PASSWORD		1
#define	SFTP_SSH_AUTH_KEYBOARD		2
#define	SFTP_SSH_AUTH_AGENT		3
#define	SFTP_SSH_AUTH_PUBLICKEY		4
#define	SFTP_COMPRESSION_OFF		0
#define	SFTP_COMPRESSION_REQUEST	1
#define SFTP_KH_FINE_ADD_TO_FILE	0
#define SFTP_KH_FINE			1
#define SFTP_KH_REJECT			2
#define SFTP_KH_DEFER			3
#define SFTP_TCPPROBES_OFF		0
#define SFTP_TCPPROBES_ON		1
#define SFTP_CLOSERESPONSE_OFF		0
#define SFTP_CLOSERESPONSE_ON		1

// SFTP error codes
#define SFTPCLI_ERR_NONE		 0	//no error
#define SFTPCLI_ERR_UNDEF		-1	//not yet defined error
#define SFTPCLI_ERR_LOGIN_DENIED	-3	//login denied
#define SFTPCLI_ERR_COULDNT_CONNECT	-4	//could not connect
#define SFTPCLI_ERR_ABORT		-5	//aborted by user callback
#define SFTPCLI_ERR_WRITE		-7	//error from write file
#define SFTPCLI_ERR_NOCLIENT		-20	//no client
#define SFTPCLI_ERR_FILENAME		-21	//filename error
#define SFTPCLI_ERR_POINTER		-1000   //invalid pointer
#define SFTPCLI_ERR_CFG_OPTION		-1001   //wrong config option
#define SFTPCLI_ERR_CFG_VALUE		-1002   //wrong parameter
#define SFTPCLI_ERR_INVALID_THIS	-1003	//this pointer is empty
#define SFTPCLI_ERR_INVALID_CB		-1004	//invalid callback function
#define SFTPCLI_ERR_IS_CONNECTED	-1005	//client is already connected
#define SFTPCLI_ERR_NOT_CONNECTED	-1006	//client is not connected
#define SFTPCLI_ERR_MEMORY		-1007	//memory error



#pragma pack (push,1)

TYPE  
  
  //SetOption Enumerator
  sftp_Options :  
  (
	_SFTP_Username,
	_SFTP_Password,
	_SFTP_URL,
	_SFTP_Port,
	_SFTP_SSH_AuthTypes,
	_SFTP_SSH_Compression,
	_SFTP_SSH_PubKeyMD5,
	_SFTP_SSH_PubKey,
	_SFTP_SSH_PrivKey,
	_SFTP_SSH_PrivKeyPassword,
	_SFTP_SSH_KnownHosts,
	_SFTP_SSH_KH_MATCH,
	_SFTP_SSH_KH_MISMATCH,
	_SFTP_SSH_KH_MISSING,
	_SFTP_TCP_KeepAlive,
	_SFTP_TCP_KeepIdle,
	_SFTP_TCP_KeepInterval,
	_SFTP_TIMEOUT_Connect,
	_SFTP_TIMEOUT_Send,
	_SFTP_TIMEOUT_Receive,
	_SFTP_CloseResponse
  )$UDINT;
  

  //CIL Declaration for the SFTP API
  LSL_SFTP_API_CLIENT : STRUCT    
    version:                UDINT;
    size:                   UDINT;      
    psftp_CreateConn:       pvoid;
    psftp_SetOptionString:  pvoid;
    psftp_SetOptionInt:     pvoid;
    psftp_GetOptionString:  pvoid;
    psftp_GetOptionInt:     pvoid;
    psftp_Connect:          pvoid;
    psftp_XConnect:         pvoid;
    psftp_CWD:              pvoid;
    psftp_PWD:              pvoid;
    psftp_MKDIR:            pvoid;
    psftp_RMDIR:            pvoid;
    psftp_DELE:             pvoid;
    psftp_BYE:              pvoid;
    psftp_LIST:             pvoid;
    psftp_NLIST:            pvoid;
    psftp_PUT:              pvoid;
    psftp_APPEND:           pvoid;
    psftp_GET:              pvoid;
    psftp_REN:              pvoid;
    psftp_CloseAllSessions: pvoid;    
  END_STRUCT;
  
END_TYPE

#pragma pack (pop)




//CreateConn:	Currently, max. 4 connections are simultaneously possible.
//			This function looks for an empty connection slot and
//			returns, if one is available, the handle as address.
//			If none is available or an error occured, returns NULL.
//
//void **(*psftp_CreateConn)(void);
FUNCTION GLOBAL __cdecl P_SFTP_CreateConn
VAR_OUTPUT 
	ppHandle	: ^pVoid;
END_VAR;

//SetOptionString:	Set options that require a string for the specified handle.
//			Choose the option in the above declared enum. Pass the value
//			as string in double quotes. Returns SFTPCLI_ERR_NOCLIENT if
//			connection is faulty or not available, returns
//			SFTPCLI_ERR_IS_CONNECTED if a connection is already established,
//			returns SFTPCLI_ERR_CFG_OPTION if a wrong option is specified.
//
//long (*psftp_SetOptionString)(void **handle, sftp_Options option, const char* value);
FUNCTION GLOBAL __cdecl P_SFTP_SetOptionString
VAR_INPUT
    ppHandle : ^pVoid;
	option : sftp_Options;
    pValue : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//SetOptionInt:		Set options that require an integer for the specified handle.
//			Choose the option in the above declared enum. Pass the value
//			as integer. Returns SFTPCLI_ERR_NOCLIENT if
//			connection is faulty or not available, returns
//			SFTPCLI_ERR_IS_CONNECTED if a connection is already established,
//			returns SFTPCLI_ERR_CFG_OPTION if a wrong option is specified.
//
//long (*psftp_SetOptionInt)(void **handle, sftp_Options option, const long value);
FUNCTION GLOBAL __cdecl P_SFTP_SetOptionInt
VAR_INPUT
    ppHandle : ^pVoid;
	option : sftp_Options;
    Value : DINT;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//GetOptionString:	Read the value of an option for the specified handle. Choose the
//			option in the above declared enum. Pass a buffer to store the
//			value in and specify the maximum amount of characters to be
//			written. Returns SFTPCLI_ERR_NOCLIENT if
//			connection is faulty or not available,
//			returns SFTPCLI_ERR_CFG_OPTION if a wrong option is specified.
//
//long (*psftp_GetOptionString)(void **handle, sftp_Options option, void *buffer, long nBuffer);
FUNCTION GLOBAL __cdecl P_SFTP_GetOptionString
VAR_INPUT
    ppHandle : ^pVoid;
	option : sftp_Options;
    pBuffer : ^char;
	nBuffer : DINT;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//GetOptionInt:		Read the value of an option for the specified handle. Choose the
//			option in the above declared enum. Pass the address of an integer
//			variable to store the value in. Returns SFTPCLI_ERR_NOCLIENT if
//			connection is faulty or not available, returns
//			SFTPCLI_ERR_CFG_OPTION if a wrong option is specified.
//
//long (*psftp_GetOptionInt)(void **handle, sftp_Options option, long *var);
FUNCTION GLOBAL __cdecl P_SFTP_GetOptionInt
VAR_INPUT
    ppHandle : ^pVoid;
	option : sftp_Options;
    pValue : ^DINT;
END_VAR
VAR_OUTPUT 
	ret0          : DINT;
END_VAR;

//Connect:	Connect the specified handle with the defined parameters. Returns
//			the socket number if executed properly, returns
//			SFTPCLI_ERR_LOGIN_DENIED if login was denied, returns
//			SFTPCLI_ERR_UNDEF if an other error occured. For error details,
//			view salamander-log.txt
//long (*psftp_Connect)(void **handle)
FUNCTION GLOBAL __cdecl P_SFTP_Connect
VAR_INPUT
    ppHandle : ^pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//XConnect:	Connect the specified handle with the defined parameters. Possibility
//			to pass a callback function with parameters. Returns the socket number
//			if executed properly, returns SFTPCLI_ERR_NOCLIENT if connection
//			is faulty or not available,returns SFTPCLI_ERR_LOGIN_DENIED if login was
//			denied, returns	SFTPCLI_ERR_UNDEF if an other error occured. For error
//			details, view salamander-log.txt
//long (*psftp_XConnect)(void **handle,	void (*response_func)(void* user_arg, char* wasned, int wasnix), void* user_arg);
FUNCTION GLOBAL __cdecl P_SFTP_XConnect
VAR_INPUT
    ppHandle : ^pVoid;
    response_func : pVoid;
    user_arg : pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//CWD:		Change working directory while connected. Pass the directory name to
//			switch to in double quotes. Returns SFTPCLI_ERR_NONE if working
//			correctly, returns SFTPCLI_ERR_NOCLIENT if connection is faulty or not
//			available, returns SFTPCLI_ERR_NOT_CONNECTED if not connected, returns
//			SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_CWD)(void **handle, char* dir);
FUNCTION GLOBAL __cdecl P_SFTP_CWD
VAR_INPUT
    ppHandle : ^pVoid;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//PWD:		Writes current directory to specified buffer. Max. nBuf characters are
//			copied. Returns SFTPCLI_ERR_NONE if executed correctly, returns
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected.
//long (*psftp_PWD)(void **handle, char *buf);
FUNCTION GLOBAL __cdecl P_SFTP_PWD
VAR_INPUT
    ppHandle : ^pVoid;
    buf   : ^CHAR;
    nBuf  : DINT;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//MKDIR:	Creates a directory with the specified name in the working directory.
//			Returns SFTPCLI_ERR_NONE if executed correctly, returns
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns
//			SFTPCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_MKDIR)(void **handle, char *dirname);
FUNCTION GLOBAL __cdecl P_SFTP_MKDIR
VAR_INPUT
    ppHandle : ^pVoid;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//RMDIR:	Deletes the directory with the specified name in the working directory.
//			Returns SFTPCLI_ERR_NONE if executed correctly, returns
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns
//			SFTPCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_RMDIR)(void **handle, char *dirname);
FUNCTION GLOBAL __cdecl P_SFTP_RMDIR
VAR_INPUT
    ppHandle : ^pVoid;
    dir : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//DELE:		Deletes the file with the specified name in the working directory.
//			Returns SFTPCLI_ERR_NONE if executed correctly, returns
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns
//			SFTPCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_DELE)(void **handle, char *filename);
FUNCTION GLOBAL __cdecl P_SFTP_DELE
VAR_INPUT
    ppHandle : ^pVoid;
    file : ^CHAR;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//BYE:		Closes the connection and frees the connection (parameters are lost).
//			Returns SFTPCLI_ERR_NONE if executed correctly, returns
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns
//			SFTPCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_BYE)(void **handle);
FUNCTION GLOBAL __cdecl P_SFTP_BYE
VAR_INPUT
    ppHandle : ^pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//LIST:		Lists all files in the specified directory by executing the user_function.
//			Returns SFTPCLI_ERR_NONE if executed correctly, returns
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns SFTPCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns SFTPCLI_ERR_INVALID_CB if callback
//			function is invalid, returns SFTPCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_LIST)(void **handle, char* path, int (*user_function)(char*, int));
FUNCTION GLOBAL __cdecl P_SFTP_LIST
VAR_INPUT
    ppHandle : ^pVoid;
    path : ^CHAR;
    userfunc : pVoid;
END_VAR
VAR_OUTPUT 
	ret0     : DINT;
END_VAR;

//NLIST:	Lists all files and directories in the specified directory by executing
//			the user_function. Returns SFTPCLI_ERR_NONE if executed correctly, returns
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns SFTPCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns SFTPCLI_ERR_INVALID_CB if callback
//			function is invalid, returns SFTPCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_NLIST)(void **handle, char * path, int (*user_function)(char *, int));
FUNCTION GLOBAL __cdecl P_SFTP_NLIST
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
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns SFTPCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns SFTPCLI_ERR_INVALID_CB if callback
//			function is invalid, returns SFTPCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns SFTPCLI_ERR_ABORT if the user aborted the transfer,
//			returns SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_PUT)(void **handle, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
FUNCTION GLOBAL __cdecl P_SFTP_PUT
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
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns SFTPCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns SFTPCLI_ERR_INVALID_CB if callback
//			function is invalid, returns SFTPCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns SFTPCLI_ERR_ABORT if the user aborted the transfer,
//			returns SFTPCLI_ERR_UNDEF if another error occured.
//long (*psftp_APPEND)(void **handle, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
FUNCTION GLOBAL __cdecl P_SFTP_APPEND
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
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns SFTPCLI_ERR_INVALID_THIS
//			if this-pointer is invalid, returns SFTPCLI_ERR_INVALID_CB if callback
//			function is invalid, returns SFTPCLI_ERR_COULDNT_CONNECT if connection is
//			lost, returns SFTPCLI_ERR_ABORT if the user aborted the transfer, returns
//			SFTPCLI_ERR_WRITE if write callback returned an error, returns SFTPCLI_ERR_UNDEF
//			if another error occured.
//long (*psftp_GET)(void **handle, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
FUNCTION GLOBAL __cdecl P_SFTP_GET
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
//			SFTPCLI_ERR_NONE if executed correctly, returns	SFTPCLI_ERR_NOCLIENT
//			if connection is faulty or not available, returns SFTPCLI_ERR_NOT_CONNECTED
//			if not connected. Returns SFTPCLI_ERR_FILENAME if a filename is invalid
//			returns SFTPCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			SFTPCLI_ERR_UNDEF if another error occured.
//
//long (*psftp_REN)(void **handle, char *from0, char *to0);
FUNCTION GLOBAL __cdecl P_SFTP_REN
VAR_INPUT
  ppHandle : ^pVoid;
  from0     :^CHAR;
  to0       :^CHAR;
END_VAR
VAR_OUTPUT
  ret0      : DINT;
END_VAR;

//CloseAllSessions:	Closes all connections and frees the connections (parameters are lost).
//			Returns SFTPCLI_ERR_NONE if executed correctly, returns
//			SFTPCLI_ERR_NOCLIENT if connection is faulty or not available, returns
//			SFTPCLI_ERR_NOT_CONNECTED if not connected. Returns
//			SFTPCLI_ERR_COULDNT_CONNECT if connection is lost, returns
//			SFTPCLI_ERR_UNDEF if another error occured.
//
//long (*psftp_CloseAllSessions)(void);
FUNCTION GLOBAL __cdecl P_SFTP_CLOSE
VAR_OUTPUT
  ret0      : DINT;
END_VAR;


//
//  INTERFACE MAKROS for the SFTP API
//
#define OS_SFTP_CREATE_CONN()                piSFTP^.psftp_CreateConn $ P_SFTP_CreateConn()
#define OS_SFTP_SET_OPT_STR(p1,p2,p3)        piSFTP^.psftp_SetOptionString $ P_SFTP_SetOptionString(p1,p2,p3)
#define OS_SFTP_SET_OPT_INT(p1,p2,p3)        piSFTP^.psftp_SetOptionInt $ P_SFTP_SetOptionInt(p1,p2,p3)
#define OS_SFTP_GET_OPT_STR(p1,p2,p3,p4)     piSFTP^.psftp_GetOptionString $ P_SFTP_GetOptionString(p1,p2,p3,p4)
#define OS_SFTP_GET_OPT_INT(p1,p2,p3)        piSFTP^.psftp_GetOptionInt $ P_SFTP_GetOptionInt(p1,p2,p3)
#define OS_SFTP_Connect(p1)                  piSFTP^.psftp_Connect $ P_SFTP_Connect(p1)
#define OS_SFTP_XConnect(p1,p2,p3)           piSFTP^.psftp_XConnect $ P_SFTP_XConnect(p1,p2,p3)
#define OS_SFTP_CWD(p1,p2)                   piSFTP^.psftp_CWD $ P_SFTP_CWD(p1,p2)
#define OS_SFTP_PWD(p1,p2,p3)                piSFTP^.psftp_PWD $ P_SFTP_PWD(p1,p2,p3)
#define OS_SFTP_MKDIR(p1,p2)                 piSFTP^.psftp_MKDIR $ P_SFTP_MKDIR(p1,p2)
#define OS_SFTP_RMDIR(p1,p2)                 piSFTP^.psftp_RMDIR $ P_SFTP_RMDIR(p1,p2)
#define OS_SFTP_DELE(p1,p2)                  piSFTP^.psftp_DELE $ P_SFTP_DELE(p1,p2)
#define OS_SFTP_BYE(p1)                      piSFTP^.psftp_BYE $ P_SFTP_BYE(p1)
#define OS_SFTP_LIST(p1,p2,p3)               piSFTP^.psftp_LIST $ P_SFTP_LIST(p1,p2,p3)
#define OS_SFTP_NLIST(p1,p2,p3)              piSFTP^.psftp_NLIST $ P_SFTP_NLIST(p1,p2,p3)
#define OS_SFTP_PUT(p1,p2,p3,p4)             piSFTP^.psftp_PUT $ P_SFTP_PUT(p1,p2,p3,p4)
#define OS_SFTP_APPEND(p1,p2,p3,p4)          piSFTP^.psftp_APPEND $ P_SFTP_APPEND(p1,p2,p3,p4)
#define OS_SFTP_GET(p1,p2,p3,p4)             piSFTP^.psftp_GET $ P_SFTP_GET(p1,p2,p3,p4)

#define OS_SFTP_REN(p1,p2,p3)                piSFTP^.psftp_REN $ P_SFTP_REN(p1,p2,p3)
#define OS_SFTP_CLOSE()                      piSFTP^.psftp_CloseAllSessions $ P_SFTP_CLOSE()

#define OS_SFTP_SIZE                         piSFTP^.size
#define OS_SFTP_VERSION                      piSFTP^.version

#endif
