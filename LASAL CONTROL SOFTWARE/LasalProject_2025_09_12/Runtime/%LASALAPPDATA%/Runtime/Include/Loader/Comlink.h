#ifndef __RTOS__COM__LINK__34F77956_E5CD_48FC_8BD7_DF713450220B__INCLUDED_
#define __RTOS__COM__LINK__34F77956_E5CD_48FC_8BD7_DF713450220B__INCLUDED_

//The loader has to be compiled with this option anyway, to
//provide the functions (e.g. TxCommand). But users do not want
//to have this define in their project's global header file.
//#ifdef COMLINK_LASAL

#define maxDynCOMcell  600
#define maxStatCOMcell 300


#include "RTOS_IprIntern.h"
#include <LSL_ST_MT.H>

//++comlinkt
#ifdef COMLINK_TCP_SERVER
 #define COMLINK_TCP
#else

 #ifdef COMLINK_TCP_CLIENT
  #define COMLINK_TCP
 #else
  #undef COMLINK_TCP
 #endif
#endif


TYPE
#pragma pack (push,1)

  LslCommRegData :struct
	LASALID   :UDINT;
	channel	  :UINT;
	VarPos	  :UINT;   
	uiTIME    :UINT; 
	VarlistID :UDINT;	// from LSE a number give back at callback (not interpreted)
  END_STRUCT;	

  comData                         : STRUCT

	// ---- muß immer da vorne sein (wegen TCP/IP loginContext Structur) ----
	pCallback					  : ^void;   	   
	pThis						  : UDINT;	 	   //
    hComchMutex                   : MT_SEMAHANDLE; //
	txCommandCnt				  : UINT;		   //
	sizeofDynList				  : UINT;		   //
	sizeofStatList				  : UINT;		   //
	sizeofRemoteDynList			  : UINT;		   //
	sizeofRemoteStatList		  : UINT;		   //
	remoteMaxCyclicTime_ms 		  : UDINT;		   //
	pNode 						  : ^MonitorNode;  //
 	pDynCallbacks       		  : ^sDynCallbackList; // Liste der Callbacks (ua. für Multimaster)
	// ---------------------------------------------------------

    HisTxObj                      : INT;
    HisHandle                     : UINT;
    OurTxObj                      : INT;
    OurHandle                     : UINT;
    CanHandle                     : dint;
    isClient					  : BOOL;
    udLastUpdate                  : UDINT;
    Com                           : DebugIp;
    OldTabsolute				  : UDINT;
    updssw						  : UINT;	
    nStateS                       : UINT;
    nActS                         : UINT;  // no of actual cells to update
    nState                        : UINT;
    nAct                          : UINT;  // no of actual cells to update
    nTx                           : INT;        // total bytes to transmit
    xTx                           : INT;           // counts up while tx
    reqSN                         : UINT;  // Sequence-Number für einen Request, um fehlende CAN Msgs zu erkennen
    respSN                        : UINT;  // Sequence-Number für eine Response, um fehlende CAN Msgs zu erkennen
	maxIdleTime_ms                : UDINT;		   
	lastSendTime_ms               : UDINT;
	m4004						  : UDINT;	// Inhalt der CAN 4004er Meldung (Refreshlistengröße)
	m4005						  : UDINT;	// Inhalt des CAN 4005er Meldung (_swruntime)
	maxIdlTim_new				  : UINT;	// Inhalt des CAN 4006er Meldung (_swruntime)
	maxIdlTim_old				  : UINT;
	m4007						  : UDINT;	// Inhalt des CAN 4007er Meldung (I am Alive Message)
	canBremseSvr                  : UINT;	// Can Bremse serverseitig
	canNr                         : USINT;	// Can Schnittstellen-Nummer
    pComdef						  : ^COMDEF;	
	paDynList                     : ^COMSTCELL;
	paStatList                    : ^COMSTCELL; 
	aDynList                      : ARRAY [0..maxDynCOMcell-1] OF COMSTCELL;
	aStatList                     : ARRAY [0..maxStatCOMcell-1] OF COMSTCELL; 
  END_STRUCT;

comStCell                       : STRUCT
  pCh                           : ^SVRCH;
// Die oberen Bits in udUpdateRate haben eine spezielle Bedeutung: 
#define UPDRATE_FLAG_IS_VBI         16#80000000     // gibt an, dass es sich um ein VBI Objekt handelt
#define UPDRATE_FLAG_IS_GLOBALVAR   16#40000000     // gibt an, dass es sich um eine globale Variable handelt
#define UPDRATE_FLAG_IS_MERKEREX    16#20000000     // gibt an, dass es sich um ein MerkerEx Objekt handelt
#define UPDRATE_FLAG_FORCE_CHANGE   16#10000000     // bewirkt, dass eine Änderung der Zelle festgestellt wird, auch wenn alt und neu gleich sind
  udUpdateRate                  : UDINT;
  dData                         : DINT;
  udLastUpdate                  : UDINT;
  VarlistID						: UDINT; // a number for LSE interpreter
 END_STRUCT;

comObjStr                       : STRUCT
  iLng                          : INT;
  iOff                          : INT;
  dData                         : DINT;
 END_STRUCT;

 // .......................................
//        übergabe bei login 
// .......................................

Comdef : STRUCT
	Interface: UDINT;
	Adress   : UINT;
	pt_COM   : ^COMDATA;
END_STRUCT; 

d2LSE : STRUCT
	VarlistID : UDINT;
	uiOffs    : UINT;
	Data      : DINT;
END_STRUCT; 

  // a structure for a socket with a rx- and tx- buffer
  bufferedSocket : STRUCT
    sock         : DINT;                    // socket number
    pndSock      : DINT;                    // socket for pending connections
    rxBuf        : ARRAY[0..9999] OF CHAR;  // receive buffer
    #ifdef DEBUG
    rxBufChk     : UDINT;
    #endif
    txBuf        : ARRAY[0..9999] OF CHAR;  // send buffer
    #ifdef DEBUG
    txBufChk     : UDINT;
    #endif
    bytesInRxBuf : UDINT;                   // number of bytes in rxBuf
    rxBufStrtInd : UDINT;                   // zero-based index of first byte in rxBuf
    sizeOfRxBuf  : UDINT;                   // size of rxBuf
    bytesInTxBuf : UDINT;                   // number of bytes in txBuf
    txBufStrtInd : UDINT;                   // zero-based index of first byte in txBuf
    sizeOfTxBuf  : UDINT;                   // size of txBuf
    bytesToSend  : UDINT;                   // number of bytes in the buffer that are marked as unsent
    fAckRcvdData : UDINT;                   // send something when data received from the tcpip stack to avoid delayed ack
    fTxBufferShortage : UDINT;              // this flag is set when a tx buffer shortage exists
    lastSendTime : UDINT;                   // time of last send without buffering, in ms
    isInstIdVerified : UDINT;               // a flag that indicates whether the instance ID is already verified
    lastError    : DINT;
	lastSendTime_ms : UDINT;
    lastUpdTime  : UDINT;                   // time of last UpdateListProcessor call
    refrMsgListenInhibitCnt : UDINT;        // Zähler wie oft der Aufruf von RefrMsgListener verhindert wurde
	isDataAvailable : BOOL;
  END_STRUCT;

  // a data structure for an update list
  updateList : STRUCT
    pFirstCell   : ^comStCell;  // pointer to first cell
    pActCell     : ^comStCell;  // pointer to actual cell
    actInd       : UDINT;       // index of actual cell
    nbrCells     : UDINT;       // total number of cell entries (size of list)
    nbrRefrCells : UDINT;       // number of cells to refresh
  END_STRUCT;
  
  // a data structure that is created by the client for each login
  loginContext : STRUCT
	// ---- muß immer da vorne sein (wegen CAN comData Structur) ----
    pCallback                     : ^void;
	pThis		                  : UDINT;
    hComchMutex                   : MT_SEMAHANDLE; //
	txCommandCnt                  : UINT;		   //
	sizeofDynList			      : UINT;	       //
	sizeofStatList			      : UINT;	       //
	sizeofRemoteDynList		      : UINT;	       //
	sizeofRemoteStatList	      : UINT;	       //
	remoteMaxCyclicTime_ms 		  : UDINT;		   //
	pNode 						  : ^MonitorNode;  //
	pDynCallbacks       		  : ^sDynCallbackList; // Liste der Callbacks (ua. für Multimaster)
	// ---------------------------------------------------------
    ipAddrSrvr   : UDINT;           // ip-address of the server
    cmdPortSrvr  : UDINT;           // port number of the servers command port
    versionOfPeer: UDINT;           // version number of the peer
    instIdOfPeer : UDINT;           // instance id of the peer
    cmdSockClnt  : bufferedSocket;  // command socket of the client
    refrSockClnt : bufferedSocket;  // refresh socket of the client
    befiprResp   : msgBefiprResp;   // the response message from command CMD_BEFIPR
    statList     : updateList;      // static list
    dynList      : updateList;      // dynamic list
    pComdefT     : ^comdefT;        // a pointer to a structure that was given by the caller of login
    hMutex       : MT_SEMAHANDLE;   // Mutex for accessing refrSockClnt (is accessed by LSE-task/background and CyWork of loader)
    refCnt       : DINT;            // >0 means that the loginContext is in use and so it cannot be released
    accelerateCellUpdates : BOOL;   // TRUE means call UpdateCellListener more often
  END_STRUCT; 

  // a data structure for a command connection block
  cmdConnData : STRUCT
    com          : DebugIp;         // object for the BefIpr
    instIdOfPeer : UDINT;           // instance id of the peer
    sessIdOfPeer : UDINT;           // session id of the peer
  END_STRUCT; 

  // a data structure for a refresh connection block
  refrConnData : STRUCT
    instIdOfPeer : UDINT;           // instance id of the peer
    sessIdOfPeer : UDINT;           // session id of the peer
    ipAddrPeer   : UDINT;           // ip-address of the peer
    ApplicationID : UDINT;          // application ID: individual number given by user, default = 0
    statList     : updateList;      // static list
    dynList      : updateList;      // dynamic list
	maxIdleTime_ms  : UDINT;		   
	lastSendTime_ms : UDINT;
    next         : ^refrConnData;   // pointer to next element in linked list
  END_STRUCT; 
  
  // a generic data structure for a connection
  connBlock : STRUCT
    typ          : UDINT;           // identifies the type of this data structure
    bsock        : bufferedSocket;  // command socket of the server
    ipAddrPeer   : UDINT;           // ip-address of the peer
    connectTime  : UDINT;           // timestamp of connect [ms]
    next         : ^connBlock;      // pointer to next element in linked list
    ptr          : ^void;           // a pointer to to non-generic data
    ApplicationID : UDINT;          // application ID: individual number given by user, default = 0
  END_STRUCT; 

  // a non-generic data structure for a command connection
  cmdConnBlock : STRUCT
    typ          : UDINT;           // identifies the type of this data structure
    bsock        : bufferedSocket;  // command socket of the server
    ipAddrPeer   : UDINT;           // ip-address of the peer
    connectTime  : UDINT;           // timestamp of connect [ms]
    next         : ^cmdConnBlock;   // pointer to next element in linked list
    ptr          : ^cmdConnData;    // a pointer to to non-generic data
    ApplicationID : UDINT;          // application ID: individual number given by user, default = 0
  END_STRUCT; 

  // a non-generic data structure for a refresh connection
  refrConnBlock : STRUCT
    typ          : UDINT;           // identifies the type of this data structure
    bsock        : bufferedSocket;  // command socket of the server
    ipAddrPeer   : UDINT;           // ip-address of the peer
    connectTime  : UDINT;           // timestamp of connect [ms]
    next         : ^refrConnBlock;  // pointer to next element in linked list
    ptr          : ^refrConnData;   // a pointer to to non-generic data
    ApplicationID : UDINT;          // application ID: individual number given by user, default = 0
    flushNextSends : BOOL;           // Flush the next Send calls 
  END_STRUCT; 

  // The elements in this structure must match the elements of Comdef 
  // defined in Comlink.h
  comdefT : STRUCT
    interface   : UDINT;            // interface type (TCPIP1 or TCPIP2)
    notUsed     : UINT;             // not used in tcp/ip
    pLogin      : ^loginContext;    // connection context
    ipAddress   : UDINT;            // ip-address of the comlink server
    port        : UDINT;            // port number of the comlink server
    res         : UDINT;            // reserverd and must be set to 0
  END_STRUCT; 

  // ---   messages sent over the command/refresh channel   ---

  // Variable records:
  //  Every message sent over the command/refresh channel is preceeded by a 
  //  variable record header (vrec-header). The header consists of the length 
  //  of the data following the vrec-header and an id that is always 0xFACE.
  //  The id is used to protect the comlink software from a malicious peer:
  //  When the id is not 0xFACE, the the connection is closed.
  //
  //  0  1  2  3  4  5  6  7  8
  //  +-----------------------+------------------------------------------------+
  //  !       vrecHdr         ! variable length data                           !
  //  ! dataLen   ! res ! id  !                                                !
  //  +-----------+-----+-----+                                                !
  //                          ! cmd       ! command-specific data              !
  //                          +-----------+------------------------------------+
  //                          8  9  A  B  C
  vrecHdr : STRUCT
    dataLen     : UDINT;        // length of the variable data following the vrec-header
    res         : UINT;         // Reserved, must be set to zero; a server does not 
                                // interpret this field in the moment, it can be 
                                // used in future versions to transport add. info 
                                // with a vrec (e.g.some sort of status codes).
    id          : UINT;         // for safety - must be 0xFACE, the server closes 
                                // the connection when this field does not match
  END_STRUCT;

  // generic variable record  
  vRecord : STRUCT
    vHdr        : vrecHdr;
    data        : UDINT;  
  END_STRUCT;
   
  sDynCallback : STRUCT
    pCallBack : ^void;
    pThis     : ^void;
  END_STRUCT;
  
  sDynCallbackList : STRUCT
    pCallback     : ^sDynCallback;
    pNext         : ^sDynCallbackList;
  END_STRUCT;
  
  
  
  // Command messages:
  //  Command messages start with a 32 bit command, the rest of the message is 
  //  command-specific data. Commands are layed out as follows
  //
  //   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  //   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
  //  +-+-+---------------------------+-------------------------------+
  //  |T!A!Res                        !            Code               |
  //  +-+-+---------------------------+-------------------------------+
  //  where
  //      T - Type, a request or a response
  //          0 - Request
  //          1 - Response
  //      A - Acknowledge field       
  //        a) for a request
  //          0 - no response is required
  //          1 - a response is required
  //        b) for a response
  //          0 - command is not supported
  //          1 - command has been processed
  //      Res - reserved, must be set to zero, not interpreted by the server.
  //      Code - the 16 bit command code
#define MASK_CMD_ISRESP         0x80000000
#define MASK_CMD_ACK            0x40000000
#define MASK_CMD_CODE           0x0000FFFF

#define CMD_TYPE_REQ            1
#define CMD_TYPE_RESP           2
  
  cmdMsgHdr : STRUCT
    vHdr        : vrecHdr;
    cmd         : UDINT;
  END_STRUCT;
  
  // generic command messages
  cMessage : STRUCT
    cHdr        : cmdMsgHdr;
    data        : UDINT;  
  END_STRUCT;
  
  // a generic command message with 256 bytes for data
  cMessage256 : STRUCT
    cHdr        : cmdMsgHdr;
    data        : ARRAY[0..255] OF CHAR;  
  END_STRUCT;
  
  // a generic command message with 512 bytes for data
  cMessage512 : STRUCT
    cHdr        : cmdMsgHdr;
    data        : ARRAY[0..511] OF CHAR;  
  END_STRUCT;
  
  // LOGOUT request
  msgLOGOUTReq : STRUCT
  	cHdr        : cmdMsgHdr;
  END_STRUCT;
  
  // Request/Response structure for command CMD_SESSINFO
  msgSessInfoData : STRUCT
    version     : UDINT;
    instId      : UDINT;
    sessId      : UDINT;
    sizeofDynList  : UINT;
    sizeofStatList : UINT;
  END_STRUCT;
  msgSessInfo : STRUCT
    cHdr        : cmdMsgHdr;
    version     : UDINT;
    instId      : UDINT;
    sessId      : UDINT;
	sizeofDynList  : UINT;
	sizeofStatList : UINT;
  END_STRUCT;
  msgSessInfo2 : STRUCT
    cHdr        : cmdMsgHdr;
    version     : UDINT;
    instId      : UDINT;
    sessId      : UDINT;
	  sizeofDynList  : UINT;
	  sizeofStatList : UINT;
    ApplicationID : UDINT;
  END_STRUCT;
  
  // Request structure for command CMD_BEFIPR
  msgBefiprReqData : STRUCT
    workState   : iprStates;
    instr       : instruct;
  END_STRUCT;
  msgBefiprReq : STRUCT
    cHdr        : cmdMsgHdr;
    workState   : iprStates;
    instr       : instruct;
  END_STRUCT;
  
  // Response structure for command CMD_BEFIPR
  msgBefiprRespData : STRUCT
    workState   : iprStates;
    resu        : results;
  END_STRUCT;
  msgBefiprResp : STRUCT
    cHdr        : cmdMsgHdr;
    workState   : iprStates;
    resu        : results;
  END_STRUCT;

  // Request structure for command CMD_TXUPD
  msgTxUpdReqData : STRUCT
    iOff        : UINT;
    comcell     : comStCell;
  END_STRUCT;
  msgTxUpdReq : STRUCT
    cHdr        : cmdMsgHdr;
    iOff        : UINT;
    comcell     : comStCell;
  END_STRUCT;

  // Request structure for command CMD_MAXIDLETIME
  msgIdlTimReqData : STRUCT
    maxIdleTime_ms : UDINT;
  END_STRUCT;
  msgIdlTimReq : STRUCT
    cHdr        : cmdMsgHdr;
	maxIdleTime_ms : UDINT;
  END_STRUCT;

  // Request structure for command CMD_SSREFR
  msgSsRefrReqData : STRUCT
    count       : UINT;
    typ         : UINT;
  END_STRUCT;
  msgSsRefrReq : STRUCT
    cHdr        : cmdMsgHdr;
    count       : UINT;
    typ         : UINT;
  END_STRUCT;
  
  // Request structure for command CMD_UPDCELL
  msgUpdCellReqData : STRUCT
    pCh         : ^SVRCH;
    iOff        : UINT;
    dData       : DINT;
  END_STRUCT;
  msgUpdCellReq : STRUCT
    cHdr        : cmdMsgHdr;
    pCh         : ^SVRCH;
    iOff        : UINT;
    dData       : DINT;
  END_STRUCT;

  // Request structure for command CMD_IAMALIVE
  msgIamAliveReq : STRUCT
    cHdr        : cmdMsgHdr;
  END_STRUCT;

  // Request structure for command CMD_NOP
  msgNopReq : STRUCT
    cHdr        : cmdMsgHdr;
  END_STRUCT;
  
#pragma pack (pop)
END_TYPE

//#endif // COMLINK_LASAL
#endif // __RTOS__COM__LINK__34F77956_E5CD_48FC_8BD7_DF713450220B__INCLUDED_
