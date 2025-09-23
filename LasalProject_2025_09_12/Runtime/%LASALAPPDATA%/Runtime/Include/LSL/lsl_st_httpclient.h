(****************************************************************************
 *                      INTERFACE for the HTTP - CLIENT                     *
 *                                                                          *
 *   Original Version: 19.05.2010, THALMEIER Michael                        *
 *                                                                          *
 *   Version 2: 28.05.2010, HTTPUrlEncode + HTTPUrlDecode                   *
 *                                                                          *
 ****************************************************************************)

 //HFILTER:1
#ifndef __LSL_ST_HTTPCLIENT_H_
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define __LSL_ST_HTTPCLIENT_H_

// HTTP CLIENT Error values
#define HTTPCLIENT_ERR_NONE                    0
#define HTTPCLIENT_ERR_PARAM                  -1000
#define HTTPCLIENT_ERR_INVALID                -1001
#define HTTPCLIENT_ERR_MEMORY                 -1002
#define HTTPCLIENT_ERR_NO_CURL_HANDLE         -1003
#define HTTPCLIENT_ERR_INTERNAL_DATA_STRUCT   -1004

// Curl Engine Error Codes
#define CURLE_OK 								0
#define CURLE_UNSUPPORTED_PROTOCOL 				1
#define CURLE_FAILED_INIT 						2
#define CURLE_URL_MALFORMAT 					3
#define CURLE_NOT_BUILT_IN 						4
#define CURLE_COULDNT_RESOLVE_PROXY 			5
#define CURLE_COULDNT_RESOLVE_HOST 				6
#define CURLE_COULDNT_CONNECT 					7
#define CURLE_FTP_WEIRD_SERVER_REPLY 			8
#define CURLE_REMOTE_ACCESS_DENIED 				9
#define CURLE_FTP_ACCEPT_FAILED 				10
#define CURLE_FTP_WEIRD_PASS_REPLY 				11
#define CURLE_FTP_ACCEPT_TIMEOUT 				12
#define CURLE_FTP_WEIRD_PASV_REPLY 				13
#define CURLE_FTP_WEIRD_227_FORMAT 				14
#define CURLE_FTP_CANT_GET_HOST 				15
#define CURLE_FTP_COULDNT_SET_TYPE 				17
#define CURLE_PARTIAL_FILE 						18
#define CURLE_FTP_COULDNT_RETR_FILE 			19
#define CURLE_QUOTE_ERROR 						21
#define CURLE_HTTP_RETURNED_ERROR 				22
#define CURLE_WRITE_ERROR 						23
#define CURLE_UPLOAD_FAILED 					25
#define CURLE_READ_ERROR 						26
#define CURLE_OUT_OF_MEMORY 					27
#define CURLE_OPERATION_TIMEDOUT 				28
#define CURLE_FTP_PORT_FAILED 					30
#define CURLE_FTP_COULDNT_USE_REST 				31
#define CURLE_RANGE_ERROR 						33
#define CURLE_HTTP_POST_ERROR 					34
#define CURLE_SSL_CONNECT_ERROR 				35
#define CURLE_BAD_DOWNLOAD_RESUME 				36
#define CURLE_FILE_COULDNT_READ_FILE 			37
#define CURLE_LDAP_CANNOT_BIND 					38
#define CURLE_LDAP_SEARCH_FAILED 				39
#define CURLE_FUNCTION_NOT_FOUND 				41
#define CURLE_ABORTED_BY_CALLBACK 				42
#define CURLE_BAD_FUNCTION_ARGUMENT 			43
#define CURLE_INTERFACE_FAILED 					45
#define CURLE_TOO_MANY_REDIRECTS 				47
#define CURLE_UNKNOWN_OPTION 					48
#define CURLE_TELNET_OPTION_SYNTAX 				49
#define CURLE_PEER_FAILED_VERIFICATION 			51
#define CURLE_GOT_NOTHING 						52
#define CURLE_SSL_ENGINE_NOTFOUND 				53
#define CURLE_SSL_ENGINE_SETFAILED 				54
#define CURLE_SEND_ERROR 						55
#define CURLE_RECV_ERROR 						56
#define CURLE_SSL_CERTPROBLEM 					58
#define CURLE_SSL_CIPHER 						59
#define CURLE_SSL_CACERT 						60
#define CURLE_BAD_CONTENT_ENCODING 				61
#define CURLE_LDAP_INVALID_URL 					62
#define CURLE_FILESIZE_EXCEEDED 				63
#define CURLE_USE_SSL_FAILED 					64
#define CURLE_SEND_FAIL_REWIND 					65
#define CURLE_SSL_ENGINE_INITFAILED 			66
#define CURLE_LOGIN_DENIED 						67
#define CURLE_TFTP_NOTFOUND 					68
#define CURLE_TFTP_PERM 						69
#define CURLE_REMOTE_DISK_FULL 					70
#define CURLE_TFTP_ILLEGAL 						71
#define CURLE_TFTP_UNKNOWNID 					72
#define CURLE_REMOTE_FILE_EXISTS 				73
#define CURLE_TFTP_NOSUCHUSER 					74
#define CURLE_CONV_FAILED 						75
#define CURLE_CONV_REQD 						76
#define CURLE_SSL_CACERT_BADFILE 				77
#define CURLE_REMOTE_FILE_NOT_FOUND 			78
#define CURLE_SSH 								79
#define CURLE_SSL_SHUTDOWN_FAILED 				80
#define CURLE_AGAIN 							81
#define CURLE_SSL_CRL_BADFILE 					82
#define CURLE_SSL_ISSUER_ERROR 					83
#define CURLE_FTP_PRET_FAILED 					84
#define CURLE_RTSP_CSEQ_ERROR 					85
#define CURLE_RTSP_SESSION_ERROR 				86
#define CURLE_FTP_BAD_FILE_LIST 				87
#define CURLE_CHUNK_FAILED 						88
// #define CURLE_NO_CONNECTION_AVAILABLE		89 (For internal use only,
// will never be returned by libcurl)
#define CURLE_SSL_PINNEDPUBKEYNOTMATCH			90
#define CURLE_SSL_INVALIDCERTSTATUS				91

// HTTP CLIENT Options
#define HTTPCLIENT_OPT_PROXY            1
#define HTTPCLIENT_OPT_SSL_VERIFYPEER   2
#define HTTPCLIENT_OPT_SSL_VERIFYHOST   3
#define HTTPCLIENT_OPT_CACERT           4
#define HTTPCLIENT_OPT_USERAGENT        5
#define HTTPCLIENT_OPT_CONNECTTIMEOUT   6
#define HTTPCLIENT_OPT_AUTHMETHOD       7
#define HTTPCLIENT_OPT_AUTHUSERPWD      8
#define HTTPCLIENT_OPT_DNSCACHETIME     9
#define HTTPCLIENT_OPT_SMTPUSERNAME     10
#define HTTPCLIENT_OPT_SMTPPASSWORD     11
#define HTTPCLIENT_OPT_SMTPSERVER       12
#define HTTPCLIENT_OPT_DEBUGFILE        13
#define HTTPCLIENT_OPT_SSLUSAGE         14
// supported from interface version 00.00.005
#define HTTPCLIENT_OPT_TIMEOUT          15
// supported since interface version 6 (HTTP client version 01.01.009)
#define HTTPCLIENT_OPT_PINNEDPUBLICKEY  16
#define HTTPCLIENT_OPT_CONTENTTYPE 17
#define HTTPCLIENT_OPT_LOGIN_OPTIONS 18

// Authentification options
#define HTTPCLIENT_AUTH_NONE			0
#define HTTPCLIENT_AUTH_BASIC			1
#define HTTPCLIENT_AUTH_DIGEST			2
#define HTTPCLIENT_AUTH_GSSNEGOTIATE	4
#define HTTPCLIENT_AUTH_NTLM			8

// HTTP Client Request types
#define HTTPCLIENT_REQ_GET      "GET"
#define HTTPCLIENT_REQ_POST     "POST"
#define HTTPCLIENT_REQ_PUT      "PUT"
#define HTTPCLIENT_REQ_DELETE   "DELETE"

// SSL Options
#define USESSL_NONE   0       // Do not use SSL
#define USESSL_TRY    1       // Try SSL
#define USESSL_ALL    3       // Use every time SSL or die

// minimal size in bytes of error message buffer allocated by caller of function
// GetLastErrorMessage and pointed to by parameter errMsgBuf
#define CURL_ERROR_SIZE 256

#pragma pack (push,1)

TYPE

  OS_HTTP_CLIENT : STRUCT

    udVersion                    : UDINT;
    udSize                       : UDINT;

    HTTPGet                      : pvoid;
    HTTPPost                     : pvoid;
    HTTPRequest                  : pvoid;
    HTTPSetOption                : pvoid;
    HTTPGetOption                : pvoid;
    HTTPUrlEncode                : pvoid;
    HTTPUrlDecode                : pvoid;

    HTTPMulitpartFileAdd         : pvoid;
    HTTPMultipartFieldAdd        : pvoid;

    MailInit                     : pvoid;
    CurlEasySetOptWrapper        : pvoid;
    CurlEasyPerformWrapper       : pvoid;
    MailAddReceipt               : pvoid;
    MailConfirmReceipt           : pvoid;
    MailDeInit                   : pvoid;
    HTTPClientReset              : pvoid;
    GetLastErrorMessage          : pvoid; 
  END_STRUCT;

END_TYPE

#pragma pack (pop)


FUNCTION GLOBAL __CDECL G_HTTPGet
VAR_INPUT
  strUrl      :^CHAR;
  pOutBuffer  :^CHAR;
  OutSize     : UDINT;
  code        :^DINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPPost
VAR_INPUT
  strUrl      :^CHAR;
  pOutBuffer  :^CHAR;
  OutSize     : UDINT;
  pInBuffer   :^CHAR; 
  InSize      : UDINT;
  code        :^DINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPRequest
VAR_INPUT
  strUrl      :^CHAR;
  strType     :^CHAR;
  pOutBuffer  :^CHAR;
  OutSize     : UDINT;
  pInBuffer   :^CHAR; 
  InSize      : UDINT;
  code        :^DINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPSetOption
VAR_INPUT
  udOption        : UDINT;
  optVal          :^VOID;
  optValLen       :^UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPGetOption
VAR_INPUT
  udOption        : UDINT;
  optVal          :^VOID;
  optValLen       :^UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPUrlEncode
VAR_INPUT
  pInBuffer   :^CHAR; 
  pOutBuffer  :^CHAR;
  OutSize     : UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPUrlDecode
VAR_INPUT
  pInBuffer   :^CHAR; 
  pOutBuffer  :^CHAR;
  OutSize     : UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPMulitpartFileAdd
VAR_INPUT
  pcFieldName   :^CHAR; 
  pcFileName  :^CHAR;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPMultipartFieldAdd
VAR_INPUT
  pcFieldName   :^CHAR; 
  pcFieldContent  :^CHAR;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_MailInit
VAR_INPUT
  userfunc   	: pvoid;
  useSSL		: UDINT;
  FromAddr		: ^CHAR;
  curlhandle	: ^pvoid;
  objptr		: pvoid;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_CurlEasySetOptWrapper
VAR_INPUT
  objptr		: pvoid;
  CurlParam		: DINT;
  paramptr		: pvoid;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_CurlEasyPerformWrapper
VAR_INPUT
  curlhandle	: pvoid;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_MailAddReceipt
VAR_INPUT
  listhandle	: ^pvoid;
  receipt		: ^CHAR;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_MailConfirmReceipt
VAR_INPUT
  listhandle	: pvoid;
  receipt		: pvoid;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_MailDeInit
VAR_INPUT
  curlhandle		: pvoid;
  receiptshandle 	: pvoid;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL G_HTTPClientReset;

FUNCTION GLOBAL __CDECL G_GetLastErrorMessage
VAR_INPUT
  errMsgBuf     : ^CHAR;
  bufSize       : UDINT;
END_VAR
VAR_OUTPUT
  dRC           : DINT;
END_VAR;

//HFILTER:1
#endif // __LSL_ST_HTTPCLIENT_H_
//HFILTER:
