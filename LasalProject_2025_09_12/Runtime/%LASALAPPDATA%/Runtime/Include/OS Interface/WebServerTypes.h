#ifndef __WEB_SERVER_TYPES_H_
#define __WEB_SERVER_TYPES_H_

// error values
#define WEBS_E_NONE              0           // no error
#define WEBS_E_TASKHANDLE       -1000        // error creating web server thread, create returned invalid handle
#define WEBS_E_LISTFULL         -1001        // list full (e.g. list of acceptable IP addresses, browser list)
#define WEBS_E_MEMORY           -1002        // not enough memory
#define WEBS_E_NOT_FOUND        -1003        // not found (e.g. entry not found in list, browser list)
#define WEBS_E_POINTER          -1004        // invalid pointer, NIL-pointer
#define WEBS_E_EXISTS           -1005        // already exists
#define WEBS_E_ACCESS           -1006        // access denied (e.g. delete a default entry from auth list)
#define WEBS_E_BUSY             -1007        // function in use by another task
#define WEBS_E_FLAGS            -1008        // unknown flags
#define WEBS_E_FNCDLL           -1009        // could not load/unload webfnc.dll

// reserved return value - this error value will be returned from every method of the WebServer
// class, if no WebServer interface is available.
// Starting the application without the WebServer is possible
#define WEBS_ERR_NO_INTERFACE   -1015        // error, no WebServer Interface available

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
#define WEBS_PUT_QUE   1					 // que data
#define WEBS_PUT_SEND  2					 // send queued data (if no data queued, error)

// dType flags for OS_WEB_GetLineFromBrowser
#define WEBS_GET_LINE  1					 // get line (reads data to the next end of line)
#define WEBS_GET_BUF   2					 // get buffer


#endif // __WEB_SERVER_TYPES_H_