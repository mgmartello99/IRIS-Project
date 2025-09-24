#ifndef __IP_TYPES_H_
#define __IP_TYPES_H_

// Error values
#define IPAPI_ERR_NONE             0		// no errors
#define IPAPI_ERR_OSI_FNC         -1		// operating system is too old, the funtion is not available
#define IPAPI_ERR_NULL            -1000		// invalid pointer (NIL-pointer)
#define IPAPI_ERR_IFACE           -1001		// ip-interface not available
#define IPAPI_ERR_IFACE_FNC       -1002		// ip-interface function not available
#define IPAPI_ERR_ETHER_IFACE     -1003		// ethernet Interface not available
#define IPAPI_ERR_NETWORK         -1004		// invalid network config. (same logical network on second ethernet interface)
#define IPAPI_ERR_ADDRESS         -1005		// error set/change ip-address
#define IPAPI_ERR_GATEWAY         -1006		// error set/change gateway address
#define IPAPI_ERR_PARAM           -1007     // error invalid parameter
#define IPAPI_ERR_OPTION          -1008     // error invalid option
#define IPAPI_ERR_IP_NOT_READY    -1009     // error, ethernet interface available but not ready (no IP-address set)
#define IPAPI_ERR_PORT_NOT_SET    -1010     // TCP Server Shutdown failed - Client(s) still connected.

// Errors for SetHostname
#define IPAPI_ERR_SETNAME_INVALID -1100
#define IPAPI_ERR_SETNAME_LONG	  -1101
#define IPAPI_ERR_SETNAME_CAP	  -1102
#define IPAPI_ERR_SETNAME_OTHER   -1103

// Options for GetIPInfo
#define IPAPI_OPT_ADDR             1
#define IPAPI_OPT_SUBNETMASK       2
#define IPAPI_OPT_ETHERNET_ADDR    3
#define IPAPI_OPT_GATEWAY          4
#define IPAPI_OPT_PORT             5

// Only available >= version 0x000A
#define IPAPI_OPT_DNS1              6
#define IPAPI_OPT_DNS2              7
#define IPAPI_OPT_DNS3              8
#define IPAPI_OPT_DNS4              9

// Identifiers for Changes, InstallChgEventCallback - Callback
#define IPAPI_CHG_ID_IP            1
#define IPAPI_CHG_ID_GW_ADD        2
#define IPAPI_CHG_ID_GW_REMOVE     3
#define IPAPI_CHG_ID_ROUTE_ADD     4
#define IPAPI_CHG_ID_ROUTE_REMOVE  5


// InstallChgEventCallback - CALLBACK Type
//
// FUNCTION ChgEventCallback
// VAR_INPUT
//   udChgID : UDINT;
//   dIfaceNum : DINT;
//   dReserved : DINT;
//   pThis : pVoid;
// END_VAR
// VAR_OUTPUT
//   dRC : DINT
// END_VAR

// DNS states
#define DNS_OK                  0
#define DNS_LIST_FULL           0x1000
#define DNS_ALREADY_IN_LIST     0x2000
#define DNS_TOO_MANY_SERVERS    0x3000
#define DNS_NOT_INITIALIZED     0x4000
#define DNS_SERVER_NOT_FOUND    0x5000
#define DNS_NOT_ENOUGH_SPACE    0x6000
#define DNS_ENTRY_IDX_FOUND     0x7000
#define DNS_ENTRY_IDX_NOTFOUND  0x8000
#define DNS_POINTER_ERROR       0x9000


#endif // __IP_TYPES_H_