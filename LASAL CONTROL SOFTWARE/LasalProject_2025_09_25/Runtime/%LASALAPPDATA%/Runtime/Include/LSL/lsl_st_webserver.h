#ifndef	__WEB_SERVER_H_
#pragma once
#define __WEB_SERVER_H_

// error values
#define WEBS_E_NONE              0           // no error
#define WEBS_E_TASKHANDLE       -1000        // error creating web server thread, create returned invalid handle
#define WEBS_E_LISTFULL         -1001        // list full (e.g. list of acceptable IP addresses, browser list)
#define WEBS_E_MEMORY           -1002        // not enough memory
#define WEBS_E_NOT_FOUND        -1003        // not found (e.g. entry not found in list, browser list)
#define WEBS_E_POINTER          -1004        // invalid pointer, NULL-pointer
#define WEBS_E_EXISTS           -1005        // already exists
#define WEBS_E_ACCESS           -1006        // access denied (e.g. delete a default entry from auth list)
#define WEBS_E_BUSY             -1007        // function in use by another task
#define WEBS_E_FLAGS            -1008        // unknown flags
#define WEBS_E_FNCDLL           -1009        // could not load/unload webfnc.dll

// flags for browser list (filter) entries
#define WEBS_BL_ADD              0x00000001  // add browser list (filter) entry
#define WEBS_BL_REMOVE           0x00000002  // remove filter entry
#define WEBS_BL_BLOCKING         0x00000004  // blocking mode
#define WEBS_BL_REMOVEALL        0x00000008  // remove all entries

// flags for authentication entries
#define WEBS_AUTH_ADD            0x00000001  // add authentication entry
#define WEBS_AUTH_REMOVE         0x00000002  // remove authentication entry
#define WEBS_AUTH_BLOCKING       0x00000004  // blocking mode
#define WEBS_AUTH_REMOVEALL      0x00000008  // remove all entries

// dType flags for OS_WEB_SendLineToBrowser
#define WEBS_PUT_QUE   1
#define WEBS_PUT_SEND  2

// dType flags for OS_WEB_GetLineFromBrowser
#define WEBS_GET_LINE  1
#define WEBS_GET_BUF   2


#pragma pack (push, 1)


TYPE

  LSL_WEBSERVER_API     : STRUCT

    udVersion               : UDINT;
    udSize                  : UDINT;

    pRegisterGetCallback    : PVOID;
    pRegisterPostCallback   : PVOID;
    pGetLineFromBrowser     : PVOID;
    pSendLineToBrowser      : PVOID;
    pFindStringInBuffer     : PVOID;
    pSetBrowserList         : PVOID;
    pSetAuthentication      : PVOID;
    pInetStrToByte          : PVOID;
    pGetErrorStringByValue  : PVOID;

  END_STRUCT;

  LSL_WEB_IO_CONTEXT    : STRUCT

    handle                  : PVOID;
    buffer_out              :^PCHAR;
    length_out              :^UDINT;

  END_STRUCT;

END_TYPE

#pragma pack (pop)


FUNCTION GLOBAL __CDECL P_RegisterGetCallback
VAR_INPUT
  pGetCallback  : PVOID;
  pThis         : PVOID;
END_VAR;

FUNCTION GLOBAL __CDECL P_RegisterPostCallback
VAR_INPUT
  pPostCallback : PVOID;
  pThis         : PVOID;
END_VAR;

FUNCTION GLOBAL __CDECL P_GetLineFromBrowser
VAR_INPUT
  pHandle       : PVOID;
  ppBuffer      :^PCHAR;
  dWait         : DINT;
  dType         : DINT;
END_VAR
VAR_OUTPUT
  dRetval       : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_SendLineToBrowser
VAR_INPUT
  pHandle       : PVOID;
  dWait         : DINT;
  dType         : DINT;
END_VAR
VAR_OUTPUT
  dRetval       : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_FindStringInBuffer
VAR_INPUT
  pBuffer       :^CHAR;
  pMatch        :^CHAR;
  dBufLen       : DINT;
END_VAR
VAR_OUTPUT
  pRetval       :^CHAR;
END_VAR;

FUNCTION GLOBAL __CDECL P_SetBrowserList
VAR_INPUT
  pHost         :^CHAR;
  pNet          :^CHAR;
  udFlags       : UDINT;
END_VAR
VAR_OUTPUT
  dRetval       : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_SetAuthentication
VAR_INPUT
  pFilename     :^CHAR;
  pRealm        :^CHAR;
  pUserPwd      :^CHAR;
  pErr401File   :^CHAR;
  udFlags       : UDINT;
END_VAR
VAR_OUTPUT
  dRetval       : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_InetStrToByte
VAR_INPUT
  pStrAddr      :^CHAR;
  pByteAddr     :^CHAR;
  udSize        : UDINT;
END_VAR
VAR_OUTPUT
  pRetval       :^CHAR;
END_VAR;

FUNCTION GLOBAL __CDECL P_GetErrorStringByValue
VAR_INPUT
  dErrValue     : DINT;
END_VAR
VAR_OUTPUT
  pErrString    :^CHAR;
END_VAR;


#define OS_WEB_RegisterGetCallback(p1,p2)         lsl_webserver^.pRegisterGetCallback $ P_RegisterGetCallback(p1,p2)
#define OS_WEB_RegisterPostCallback(p1,p2)        lsl_webserver^.pRegisterPostCallback $ P_RegisterPostCallback(p1,p2)
#define OS_WEB_GetLineFromBrowser(p1,p2,p3,p4)    lsl_webserver^.pGetLineFromBrowser $ P_GetLineFromBrowser(p1,p2,p3,p4)
#define OS_WEB_SendLineToBrowser(p1,p2,p3)        lsl_webserver^.pSendLineToBrowser $ P_SendLineToBrowser(p1,p2,p3)
#define OS_WEB_FindStringInBuffer(p1,p2,p3)       lsl_webserver^.pFindStringInBuffer $ P_FindStringInBuffer(p1,p2,p3)
#define OS_WEB_SetBrowserList(p1,p2,p3)           lsl_webserver^.pSetBrowserList $ P_SetBrowserList(p1,p2,p3)
#define OS_WEB_SetAuthentication(p1,p2,p3,p4,p5)  lsl_webserver^.pSetAuthentication $ P_SetAuthentication(p1,p2,p3,p4,p5)
#define OS_WEB_InetStrToByte(p1,p2,p3)            lsl_webserver^.pInetStrToByte $ P_InetStrToByte(p1,p2,p3)
#define OS_WEB_GetErrorStringByValue(p1)          lsl_webserver^.pGetErrorStringByValue $ P_GetErrorStringByValue(p1)


#endif // __WEB_SERVER_H_
