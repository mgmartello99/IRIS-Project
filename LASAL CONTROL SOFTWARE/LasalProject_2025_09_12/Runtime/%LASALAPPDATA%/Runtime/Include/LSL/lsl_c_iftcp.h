#ifndef __LSL_TCP_API
#pragma once
#define __LSL_TCP_API

#define TCP_NOT_READY           -2000
#define TCP_INVALID_SOCKSTATE   -2001
#define TCP_NOMEM_ERROR         -2002
#define TCP_BUFFER_TO_SMALL     -2003
#define TCP_MAXSOCKETS_ERROR    -2004
#define TCP_INVALID_SOCKNUM     -2005
#define TCP_INVALID_NUMBER      -2007
#define TCP_SYSTEM_ERROR        -2008
#define TCP_NOIF_ERROR          -2009


//getlasterror codes
#define TCP_ADDR_NOT_AVAIL      -3000   //endpoint address not available
#define TCP_ADDR_IN_USE         -3001   //address in use
#define TCP_AF_NO_SUPPORT       -3002   //family not supported
#define TCP_ARP_TABLE_FULL      -3003   //arp table full
#define TCP_INVALID_BAUD        -3004   //invalid baud rate 
#define TCP_INVALID_COMM_PORT   -3005   //invalid comm port number
#define TCP_INVALID_DEVICE      -3006   //invalid device type
#define TCP_INVALID_IFACE       -3007   //invalid interface number
#define TCP_INVALID_MASK        -3008   //invalid mask (ether must not be all fs)
#define TCP_INVALID_PING        -3009   //invalid ping response
#define TCP_CONN_REFUSED        -3010   //endpoint refused connection
#define TCP_DEST_ADDR_REQ       -3011   //destination address is required
#define TCP_DEST_UNREACH        -3012   //destination unreachable (ICMP)
#define TCP_INVALID_PARAM       -3013   //invalid parameter (pointer is 0)
#define TCP_IFACE_CLOSED        -3014   //interface closed
#define TCP_IFACE_TABLE_FULL    -3015   //interface table full
#define TCP_IFACE_OPEN_FAIL     -3016   //interface open failed
#define TCP_IN_PROGRESS         -3017   //operation (connect) is in progress
#define TCP_INVALID_FUNC        -3018   //invalid function call (parameter)
#define TCP_SOCKET_CONNECTED    -3019   //socket is already connected
#define TCP_MC_TABLE_FULL       -3020   //multicast table full
#define TCP_MC_ADDR_NOT_FOUND   -3021   //multicast address not found
#define TCP_OUT_OF_PORTS        -3022   //out of ports 
#define TCP_NET_DOWN            -3023   //network is down (send failed)
#define TCP_NET_UNREACH         -3024   //network unreachable (keepalive failed)
#define TCP_OUT_OF_DCUS         -3025   //out of DCUs (packets)
#define TCP_OPTPARAM_INVALID    -3026   //option parameter is invalid
#define TCP_SOCK_NOT_CONNECTED  -3027   //socket is not connected
#define TCP_RTIP_NOT_INST       -3028   //RTIP not initialized (i.e. xn_rtip_init not called)
#define TCP_INVALID_SOCKET      -3029   //invalid socket descriptor
#define TCP_NUM_DEVICE          -3030   //not enough devices (see CFG_NUM_ED,CFG_NUM_SMCX, NUM_3C, NUM_RS232 etc)
#define TCP_OP_NOT_SUPPORT      -3031   //socket type or specified operation not supported for this function
#define TCP_OUTPUT_FULL         -3032   //send failed due to output list being full
#define TCP_PROBE_FAIL          -3033   //could not determine device
#define TCP_RENTRANT            -3034   //non-reentrancy error
#define TCP_ROUTE_NOT_FOUND     -3035   //routing table entry not found
#define TCP_ROUTE_FULL          -3036   //routing table full
#define TCP_RSC_INIT_FAIL       -3037   //resource initialization failed
#define TCP_SHUTDOWN            -3038   //illegal operation due to socket shutdown
#define TCP_TIMEOUT             -3039   //timeout
#define TCP_TYPE_NOT_SUPPORT    -3040   //type not supported (only SOCK_STREAM and SOCK_DGRAM are supported)
#define TCP_WOULD_ARP           -3041   //send needs to ARP but ARP is disabled (see INCLUDE_ARP)
#define TCP_WOULD_BLOCK         -3042   //socket non-blocking but function would block
#define TCP_UNKNOWN_ERROR       -3043

#define SELECT_OPT_READ          1
#define SELECT_OPT_WRITE         2
#define SELECT_OPT_EXCEPTION     3

#ifndef COMPILE_RTK

//
// Defines and type for setsockopt()
//

// Level numbers
// Number to indicate options to be interpreted by socket API itself
#define SOL_SOCKET        0x7fff
// Protocol number for IP; used to indicate options to be interpreted by IP implementation
#define IPPROTO_IP        0x0008

// Structure used to request membership in IP multicast group via socket option IP_ADD_MEMBERSHIP
typedef struct
{
    unsigned long mc_group_addr;  /* IP address of multicast group */
    unsigned long iface_addr;     /* IP address of local interface used to join multicast group */
} IP_MC_REQ;

// Option flags per-socket
#define SO_NAGLE           0x0001     // NAGLE algorithm enabled/disabled
#define SO_REUSEADDR       0x0004     // allow local address reuse
#define SO_KEEPALIVE       0x0008     // enables/disables sending of TCP keepalive packets
#define SO_DELAYED_ACK     0x0400     // delay sending ACK
#define IP_MULTICAST_IF    0x0009     // RTK: specify default local multicast IP interface valid *system-wide*
                                      // Salamander: specify local IP interface for multicast packets sent via a particular socket
#define IP_MULTICAST_LOOP  0x000b     // enable/disable loop-back of sent multicast IP packets to local sockets
#define IP_ADD_MEMBERSHIP  0x000c     // join ip multicast group

#endif // #ifndef COMPILE_RTK

typedef struct
{
    unsigned long open_sessions;

    void* (* tcp_Init)(void* callback, unsigned char prot);
    long  (* tcp_Open)(void* handle, unsigned long remote_ip, unsigned short remote_port);
    long  (* tcp_Listen)(void* handle, unsigned short listen_port);
    void* (* tcp_Accept)(void* handle, void* callback);
    long  (* tcp_SendData)(void* handle, void* data, unsigned long datalen);
    long  (* tcp_ReceiveData)(void* handle, void* data, unsigned long len);
    long  (* tcp_Close)(void* handle);
    long  (* tcp_GetStatus)();

} LSL_TCP_API;

#define LSLAPI_TCP_USER_VERSION   0x000C

typedef struct
{
           unsigned long version;

           int   (* tcp_user_socket)();
           int   (* tcp_user_closesocket)(int snum, unsigned long type);
           int   (* tcp_user_connect)(int snum, unsigned long localport0, char *IPAddress0, unsigned long port0, unsigned long timeout_ms);
           int   (* tcp_user_listen)(int snum, unsigned long localport0, unsigned long backlogsize0);
           int   (* tcp_user_accept)(int snum, unsigned long timeout_ms);
           int   (* tcp_user_recv)(int snum, char *buffer0, unsigned long buflen0, unsigned long flags0, unsigned long timeout_ms);
           int   (* tcp_user_send)(int snum, char *buffer0, unsigned long buflen0, unsigned long flag0, unsigned long timeout_ms);
           int   (* tcp_user_shutdown)(int snum, unsigned long how);
           long  (* tcp_user_getpeerip)(int snum, unsigned long *sin_addr);
           long  (* tcp_user_getpeerport)(int snum, unsigned long *sin_port);
           long  (* tcp_user_getsockip)(int snum, unsigned long *sin_addr);
           long  (* tcp_user_getsockport)(int snum, unsigned long *sin_port);
           long  (* tcp_user_toip)(char *buffer0, unsigned long buflen0, unsigned long ID10, unsigned long ID20, unsigned long ID30, unsigned long ID40);
  unsigned long  (* tcp_user_strtoulong)(char *buffer0);
           long  (* tcp_user_ulongtostr)(char *buffer0, unsigned long buflen0, unsigned long IPAddr0);
  unsigned long  (* tcp_user_getCOMLinkPort)();
           long  (* tcp_user_geterrorstring)(char *buffer0, unsigned long buflen0, long errvalue0);
           long  (* tcp_user_netstat)(char *buf0, long bufsize0);
           int   (* tcp_user_nread_available)(int snum);
           // since version 2
           int   (* tcp_user_socket_ex)(unsigned long iface0);
           // since version 3
           unsigned long (* tcp_user_getservbyname)(char *name, char *proto);
           // since version 4
           long  (* tcp_user_ipinfo)(int iface0, int opt0, unsigned long *p0);
           // since version 5
           int   (* tcp_user_setsockopt)(int snum, int level, int option_name, char *option_value, int optionlen);
           // since version 6
           long  (* tcp_user_ping)(char *ipaddr0, unsigned long  bytes0, unsigned long ttl0, unsigned long wait0);
           // since version 7
           int   (* udp_user_socket)();
           int   (* udp_user_bind)(int snum, unsigned long ipaddr, unsigned long port);
           int   (* tcp_user_recvfrom)(int snum, char *buffer0, unsigned long buflen0, unsigned long flag0, unsigned long timeout_ms, unsigned long * pIPaddr, unsigned long * pPort);
           int   (* tcp_user_sendto)(int snum, char *buffer0, unsigned long buflen0, unsigned long flag0, unsigned long timeout_ms, unsigned long ipaddr, unsigned long port);
           // since version 8
           long  (* tcp_user_ipinfoIntern)(int iface0, int opt0, unsigned long *p0);
           int   (* tcp_user_select)(int count0, int *sockets0, unsigned long select_type0, unsigned long flag0, unsigned long timeout_ms);
           // since version 9
           int   (* tcp_user_ioctlsocket)(int snum, unsigned long select_type0, unsigned long onoff);
           // since version a
           // since version b
           long  (* tcp_user_initFastInterface)(long iFace, /*FN_USERCALLBACK*/void *pUserFct, void *pUserParam);
           long  (* tcp_user_sendfast)(long interface_handle,char *buffer0,unsigned long buflen0);
           long  (* tcp_user_getLinkStatus)(long iFace, unsigned long *pulLinkStatus);
           // since version c
           long (* tcp_user_strtoulong_asy)(char* buffer, /*FN_STRTOULONG_USERCALLBACK*/void *pUserFct, void *pUserParam);
} LSL_TCP_USER;

#endif