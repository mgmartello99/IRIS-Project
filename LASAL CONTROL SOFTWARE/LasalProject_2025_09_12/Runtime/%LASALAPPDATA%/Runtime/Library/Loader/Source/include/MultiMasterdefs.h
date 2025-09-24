#pragma once

TYPE

  // Mögliche Kommandos
  _MMThreadCmd : 
  (
    MM_NONE,
    MM_CONNECT_STATION,
    MM_DISCONNECT_STATION,
    MM_SETCONNSTR,
    MM_WRITE,
    MM_NEWINST,
    MM_DELFROMRFL
  )$UDINT;

  // benötigte Information damit ein User-Kommando ausgeführt werden kann
  _MMThreadCmdLine : STRUCT  
	    cmd         : _MMThreadCmd; // Art des Kommandos
	    udStationId : UDINT;  // Stations id
      SvrHandle   : UDINT;  // Server handle	    
	    caConn      : ARRAY [0..16#FF] OF CHAR; // Verbindungsstring
      pVoid       : UDINT;  // adresse auf zusätzliche Info
      udSeqNb     : UDINT;  // laufende nummer des cmd
	 END_STRUCT;
       
// struktur für die newInst kommandos
sNewInst : STRUCT  //! <Type Public="true" Name="sNewInst"/>
  Para : CmdStruct;
  Results : results;
  IprState : iprStates;
  origparaAdr : UDINT;
  origresAdr : UDINT;
  next : ^sNewInst;
  used :
  (
    unusedNewInst,      // struktur wird nicht verwendet
    fromNewInst,        // Kommando kommt von NewInst
    sentNewInst,        // NewInst Kommando wurde gesendet (derzeit nicht verwendet)
    fromGetState,       // Kommando kommt von GetState (derzeit nicht verwendet)
    sentGetState,       // NewInst Kommando wurde gesendet (derzeit nicht verwendet)
    finishedNewInst     // NewInst Kommando vollständig abgearbeitet
  )$UDINT;
END_STRUCT;
   
END_TYPE

// Mutex
#define MM_MUTEX_COMDEF        2


// Default Comlink ServerPort
#define MM_DEFAULT_TCP_PORT  1955 
// Pfad für Datei zum Speichern für die StationsFiles bzgl LasalIds
#define MM_FILE_PATH      "C:\lsldata\" 
// Dateiendung
#define MM_FILE_EXT       ".mmb"
#define MM_FILE_SEARCH    "C:\lsldata\*.mmb"

// Datei in der die MMStations aufgelistet sind
#define MM_STATIONCFG_FILE "c:\lsldata\Stations.txt" 
// aktuelle Dateiversion
#define MM_STATIONCFG_FILE_VERSION   "1.00"

#define MM_STATIONCFG_FILE_DEFVERSION      "[VERSION]"
#define MM_STATIONCFG_FILE_DEFCONNECTIONS  "[CONNECTIONS]"

// Datei in der die überschriebenen Connectionstrings der MMStations aufgelistet sind
#define MM_STATIONCFG_OVERWRITE_FILE "c:\lsldata\Station2.txt" 

// Maximale Anzahl der Stationen, die unterstützt wird
#define MM_MAX_STATIONS   256 

// Name der Tabelle wo alle Server aufgelistet sind
#define MM_TableName "CONFIG_MULTIMASTER" 

// Defines für MMServer Priority
#define MM_NoRefreshlistEntry    16#0000
#define MM_GetValueOnce          16#FFFF

//#define MM_DEBUG

#ifdef MM_DEBUG
FUNCTION GLOBAL LDR_MM_Trace
VAR_INPUT
  fmt : ^CHAR;
  p1 : UDINT;
  p2 : UDINT;
  p3 : UDINT;
END_VAR;
FUNCTION GLOBAL LDR_MM_Trace_s
VAR_INPUT
  fmt : ^CHAR;
  s1_ : ^CHAR;
  p1 : UDINT;
  p2 : UDINT;
END_VAR;
#define MM_TRACE(fmt,p1,p2,p3)     LDR_MM_Trace(fmt,p1$UDINT,p2$UDINT,p3$UDINT);
#define MM_TRACE_S(fmt,s1_,p1,p2)   LDR_MM_Trace_s(fmt,s1_,p1$UDINT,p2$UDINT);
#else
#define MM_TRACE(fmt,p1,p2,p3)
#define MM_TRACE_S(fmt,s1_,p1,p2)
#endif

FUNCTION GLOBAL __CDECL MM_ComdefMutexData
VAR_INPUT
  state : BOOL; 
END_VAR;

FUNCTION GLOBAL MM_GetStation
VAR_INPUT
  id    : UDINT; 
END_VAR
VAR_OUTPUT
  pStation : ^void;
END_VAR;

FUNCTION GLOBAL MM_GetServer
VAR_INPUT
  StationId   : UDINT;
  pszSvrName  : ^Char;  
END_VAR
VAR_OUTPUT
  pSrv : ^void;
END_VAR;

FUNCTION GLOBAL CreateMMServerObjName
VAR_INPUT
  udStation       : UDINT; 
  pszSvrName      : ^CHAR;
  pOut_szObjName  : ^CHAR;
END_VAR;

FUNCTION __CDECL GLOBAL MMThread_AddCmd
VAR_INPUT
  pCmd : ^_MMThreadCmdLine;
END_VAR
VAR_OUTPUT
  retCode : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL _LookUpMMServer
VAR_INPUT
	pObj            : ^VirtualBase;
	pText           : pChar;
	pObj_ch         : ^pVoid;
	ppObj           : ^pVirtualBase;
END_VAR
VAR_OUTPUT
	chMode    : CHMODE;
END_VAR;

FUNCTION GLOBAL MM_InvokeUserCallback
VAR_INPUT
  udStationId     : UDINT;
  udServerHandle	: UDINT;
  udState 	      : UDINT; 
END_VAR;

FUNCTION GLOBAL MM_InvokeUserCallbackServer
VAR_INPUT
  udServerHandle	: UDINT;
  udState 	      : UDINT; 
END_VAR;

FUNCTION GLOBAL MM_AreAllReqInitialised 
VAR_INPUT
  udStartTime_ms : UDINT; // Startzeitpunkg für die Timeoutprüfung
END_VAR
VAR_OUTPUT
  allReqInitialised : BOOL;
END_VAR;

FUNCTION GLOBAL MM_PrepareForFirstInit;
FUNCTION GLOBAL MM_FirstInitFinished;


