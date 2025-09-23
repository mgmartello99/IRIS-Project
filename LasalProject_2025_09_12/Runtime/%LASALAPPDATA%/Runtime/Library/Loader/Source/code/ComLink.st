
//  Changes:
//
//    25.8.2005
//      - Rueckgabewerte von CanTxObj werden beruecksichtigt, da eventuell ohne die 
//        CAN-Bremse ein Bufferüberlauf auftreten kann (Änderungsmarkierung: //ws003)
//
//    4.8.2005
//      - neue globale Funktion LDR_SetCanWait( usint ): ermöglicht das Setzen der 
//        bislang fixen Waitstates bei der CAN-Kommunikation.
//        Der Parameter gibt die Wartezeit in ms an. (Änderungsmarkierung: //ws001)
//
//      4.5.2005
//      - TXCOMMAND initialisiert jetzt vor dem Aufruf der Befipr Methode das 
//        Längenfeld der Antwort mit -1 (Com._m.resu.uilng)
//
//      14.10.2004
//          - Prüfung auf Wiedereintritt bei TXCOMMAND_ts entfernt, da sie bei bestehenden 
//            Anwendungen Probleme bereitet
//            Mit '#define COMLINK_CHECK_REENTRANT' kann die Prüfung wieder aktiviert werden.
//
//      23.09.2004
//          - Neue Funktion TXCOMMANDEX. 
//              Unterschied zu TXCOMMAND: Dem Aufrufer wird nicht der Zeiger mitgeteilt, 
//              wo sich das Ergebnis befindet, sondern der Aufrufer muß einen Buffer 
//              zur Verfügung stellen, der dann das Ergebnis aufnimmt. TXCOMMANDEX ist 
//              threadsafe. Wenn TXCOMMANDEX einen anderen TXCOMMANDEX Aufruf des selben 
//              Kanals unterbricht, dann wird BUSY zurückgegeben.
//              TXCOMMAND hat den Nachteil, daß man nur den Zeiger auf den Ergebnisbuffer 
//              erhält. Pro Kanal gibt es einen Ergebnisbuffer. Wenn man das Ergebnis aus-
//              wertet kann man von einem anderen Task unterbrochen werden, der dann 
//              ebenfalls TXCOMMAND aufruft und das Ergebnis des ersten Aufrufs überschreibt 
//              bevor es ausgewertet worden ist.
//              TXCOMMAND darf daher nicht verwendet werden, wenn es von mehreren 
//              Tasks benutzt wird und der Zugriff auf den Ergebnisbuffer nicht irgendwie 
//              anders geschützt wird !!!
//
//      24.08.2004
//          - Login Funktion beim CAN und beim TCP/IP
//              Wenn ein Login für eine bestimmte Adresse ein 2.Mal aufgerufen 
//              wird, dann wird bei ungleichem pLogin ein neuer Kanal geöffnet, 
//              bei identischem pLogin wird ein Re-Connect durchgeführt.
//              (bisher wurde in beiden Fällen ein Re-Connect durchgeführt)
//
//      30.07.2004
//          -  Mutex bei den Comlink-Funktionen wieder entfernt (außer bei TCP/IP)
//             Grund: 
//              TxCommand ist aufgrund des Parameters pResu nicht dafür geeignet,  
//              daß es von mehreren Tasks aufgerufen wird:
//
//              Bsp:
//              pResu wird von TxCommand auf den Ergebnisbuffer gesetzt. Wenn dann 
//              das Ergebnis ausgewertet wird, kann es passieren, daß der nächste 
//              Aufruf von TxCommand von einem anderen Task das Ergebnis überschreibt, 
//              bevor der erste Aufrufer es ausgewertet hat.
//
//              -> Es hat also keinen Sinn, die Comlink-Funktionen in der derzeitigen 
//              Form mit einem Mutex zu schützten. 
//
//              *** Die Comlink-Funktionen dürfen daher nur aus einem Task aufgerufen werden ***
//
//      29.06.2004
//          -  Login, TxCommand, TxUpd und StartStopRefresh Thread-Safe gemacht.
//             Anm. zu Login:
//             Es wird nur der Zugriff auf den ComCh geschützt, d.h. Login darf 
//             nicht durch ein anderes Login unterbrochen werden, da noch immer 
//             globale Variablen verwendet werden, die Probleme verursachen können 
//             (z.B. login_anz).
//             Es ist aber jetzt erlaubt, daß ein TxCommand aufgerufen wird, während 
//             das Login läuft. Dieses Situation kann z.B. bei einen Re-Connect 
//             Versuch eintreten.
//
//      22.06.2004
//          -  Reconnect over CAN resulted in an access exception when the 
//             server did not respond to the login command.
//             (++bugfix040622)
//
//      27.01.2004
//          1. InstallCallBack changed (docu look at function)
//             on CallBack ThisPointer(ESI) is loaded
//             important if more than one lokal login (for example LSE + LasalText on one CPU)      
//      17.12.2003
//          1. TCP/IP added
//
//      14.03.2003:
//          1. we take our canbus (station * 10) and add the number off logins 
//             because in the old version it is impossible to login vice versa  
//             you will get the same CAN Objekt so you will get corrupted DATA  
//          3. important "command struct" for Data is online allocatet 
//             --> in old version fixed (in init) was reserved for max_anz  
//
//      17.06.2001:
//
//          1.  in a single CPU version the updatelist is done in one cycle
//              not every 2 ms so the screen buildup time is faster  
//              
//          2.  LSE needs an additional number on callback "comStCell" extended on a 4 byte ID
//              the ID is given from LSE on TXUPD and given back at Callback
//              the so called valistID is never interprted or touched in the programm
//
//
//


#ifdef COMLINK_LASAL

#ifdef LOADER_4_LASAL2
#pragma using VirtualBaseInit
#pragma using MerkerEx
#else
TYPE
    SVRCHCMD_UDINT : SvrChCmd;
END_TYPE
#include "..\..\memory\header files\virtualbaseinit.h"
#endif

#include "RTOS_Mem.h"
#include "ComTypes.h"
#include "RTOS_IprIntern.h"
#include <LSL_ST_IFSSR.h>
#include <lsl_string.h>
#include <RTOS_Variables.h>
#include <RTOS_Header.h>
#include "RTOS_PrivHeader.h"
#include <LSL_ST_TCP_USER.h>
#include <LSL_ST_CAN.H>
#include <LSL_ST_MT.H>
#include <lsl_st_cmdif.h>
#include <lsl_st_serial.h>
#include <lsl_st_systrace.h>
#include <lsl_st_can.h>
#include <LSL_ST_SYSENV.H>

#define TREC_USER_0           ((TRECGRP_USER0 SHL 16)          OR 16#0000)

//                  1954      // ComPort for LASAL Class    
#define ServerPort  1955      // standard com I_GET befipr
#define RefreshPort 1956      // refresh port


#define DFLT_CANBREMSE_SVR 2	// 2ms CAN Bremse serverseitig

#define COMLINKR_VERSION          0x0100  // version major, version minor

// Define the following constants to write debug messages to event18.log
//#define DEBUG
#undef DEBUG
//#define DEBUG_VERBOSE
#undef DEBUG_VERBOSE

#ifdef DEBUG
#include <LSL_ST_SYSMSG.h>
#define DEBUGLOG0(txt)              OS_SYSMSG_LPRINTFLN1(hLog0,1,txt,ops.tAbsolute);
#define DEBUGLOG1(txt,p1)           OS_SYSMSG_LPRINTFLN2(hLog0,1,txt,ops.tAbsolute,p1);
#define DEBUGLOG2(txt,p1,p2)        OS_SYSMSG_LPRINTFLN3(hLog0,1,txt,ops.tAbsolute,p1,p2);
#define DEBUGLOG3(txt,p1,p2,p3)     OS_SYSMSG_LPRINTFLN4(hLog0,1,txt,ops.tAbsolute,p1,p2,p3);
#else
#define DEBUGLOG0(txt)              
#define DEBUGLOG1(txt,p1)           
#define DEBUGLOG2(txt,p1,p2)        
#define DEBUGLOG3(txt,p1,p2,p3)     
#endif

#ifdef DEBUG_VERBOSE
#define DEBUG_VERBOSE0 DEBUGLOG0
#define DEBUG_VERBOSE1 DEBUGLOG1
#define DEBUG_VERBOSE2 DEBUGLOG2
#define DEBUG_VERBOSE3 DEBUGLOG3
#else
#define DEBUG_VERBOSE0(txt)             
#define DEBUG_VERBOSE1(txt,p1)           
#define DEBUG_VERBOSE2(txt,p1,p2)        
#define DEBUG_VERBOSE3(txt,p1,p2,p3)     
#endif

#define arraysize(p)                (sizeof(p)/sizeof(p[0]))
#ifdef LOADER_4_LASAL2
#ifndef DUMMY_LOADER
#pragma using DebugIp
#endif
#endif

// max. mögliche Anzahl von internen + CAN Comlink Kanälen
#define COMLINK_COMCHS	15

// Max. mögliche Anzahl von LOGINs für TCPIP.
// Dieser Wert ist die technische Obergrenze. Das aktuelle Limit kann mit 
// der Umgebungsvariable COMLINK_MAX_LOGINS gesetzt werdenn.
#define MAX_LOGINS_ARRAYSIZE    64
// Defaultwert der Variablen g_max_logins, kann mit der Umgebungsvariable 
// COMLINK_MAX_LOGINS geändert werden
#define MAX_LOGINS_DEFAULT	     16


// Wenn COMLINK_TRACEaktiviert wird, werden TRACE-View Einträge generiert
//#define COMLINK_TRACE
#ifdef COMLINK_TRACE
#define TRACE_ENTRY(value)   LDR_MakeTraceEntry(value)
#else
#define TRACE_ENTRY(value)
#endif

#define STRUCT_TYPE_COMDATA         0
#define STRUCT_TYPE_LOGINCONTEXT    1
#define STRUCT_TYPE_LOGINCONTEXTR   2

// default value for max_conns (when env.var COMLINK_MAX_SERVER_CONNECTIONS is not set)
#define MAX_CONNS_DFLT           32   

TYPE
    MonitorNode : STRUCT
        pt_COM            : pVoid;        // zeigt auf den Kanal, über den die IamAlive Messages geschickt werden
        lastRcvdTime_ms   : UDINT;
        maxSilentTime_ms  : UDINT;
        remoteMaxIdleTime_ms : UDINT;     // die maxIdleTime der Gegenstelle
        isRemotePlcAlive  : BOOL;
    END_STRUCT;
    MonitorNodeList : STRUCT
        pNode : MonitorNode;
        pNext : ^MonitorNodeList;
    END_STRUCT;

    // Das ist die gemeinsame Basis der Strukturen comdata/loginContext/loginContextR.
    // D.h. diese 3 Strukturen müssen am Anfang so aufgebaut sein wie ComData_Base.
    // Eigentlich sollte ComData_Base und comdata/loginContext/loginContextR im gleichen 
    // File deklariert werden und am Anfang von comdata/loginContext/loginContextR 
    // sollte dann ein Element vom Typ ComData_Base existieren, um Doppelwartung zu 
    // vermeiden, aber dann hat man eine Abhängigkeit zu den Includefiles, die nicht 
    // Teil des Loaderprojekts sind.
    ComData_Base                  : STRUCT
      pCallback                     : ^void;          
      pThis                         : UDINT;
      hComchMutex                   : MT_SEMAHANDLE;
      typeOfStruct                  : UINT;           // STRUCT_TYPE_...
      sizeofDynList                 : UINT;
      sizeofStatList                : UINT;
      sizeofRemoteDynList           : UINT;
      sizeofRemoteStatList          : UINT;
      remoteMaxCyclicTime_ms        : UDINT;
      pNode                         : ^MonitorNode;
      pDynCallbacks                 : ^sDynCallbackList; // Liste der Callbacks (ua. für Multimaster)
    END_STRUCT;
    
END_TYPE

VAR_PRIVATE
    pSystrace : ^OS_SYSTRACE;
    ComlinkT_wasTraceMsgDisplayed : UDINT;
#ifdef DEBUG
    OS_pLslSysMsg : ^OS_LSLSYSMSG;
    LogBuf0       : ARRAY[0..499999] OF CHAR;
    hLog0         : UDINT;
#endif
    canCltCh_anz    : UINT;
    mt_api          : ^LSL_MT_TYPE; // multitask interface (needed for semaphores)
    myComChs        : ARRAY [0..COMLINK_COMCHS-1] OF ^COMDATA;	// Comlink Kanäle für LOCAL + CAN
    monitorNodes    : ^MonitorNodeList;
    LDR_canwait     : USINT;  //ws001
    pCanuser        : ^OS_CANUSER;
    g_RS232activity : BOOL;
    canNrSvr        : USINT;	// CAN Nr. die vom Comlink Server verwendet wird
#ifdef COMLINK_PG       // #FA1204 
    MutexPG50       : MT_SEMAHANDLE;
#endif   
#ifdef LOADER_TCP_MODERATE
    taskhandle      : MT_TASKHANDLE;
#endif
    monitorNodeMutex : MT_SEMAHANDLE;  // Mutex for monitorNodeList (alive check)
    max_conns : UDINT; // max number of comlink server connections (minus 1)
    g_max_logins : UDINT; // max. Anzahl von LOGINs über TCPIP
    loginArray    : ARRAY[0..(MAX_LOGINS_ARRAYSIZE - 1)] of ^loginContext;
END_VAR

// nur mal zum test noch nicht genau definiert wo hin damit
VAR_GLOBAL
   login_anz           : INT;
   usedsocks : ARRAY[0..127] OF USINT;  // max number of TCP_USER sockets is 120
#ifdef LOADER_TCP_MODERATE
   TCPCltCntr : USINT;
   ComlinkCntr : USINT;
#endif
    // Anzahl der verbundenen Clients (ConnBlock's - AllocConnBlock)
    g_nbrConnBlk : UDINT;
    // Anzahl der allokierten Refresh-Listen (dyn., stat.)
    g_nbrDynList : UDINT;
    g_nbrStatList : UDINT;
    // Anzahl der Verbindungen zu Servern (Login Contexte - AllocateLoginContext)
    g_nbrLoginCtx : UDINT;
END_VAR

//ZP000
VAR_EXTERNAL
  bWait4Visu  : BOOL;
  wait4Visu_timeout_ms : UDINT;
END_VAR
//ZP000

#IF arraysize(myComChs) < (1 + COMLINK_CAN_COMCHS)
# PRAGMA message ("*** COMLINK_CAN_COMCHS too big:" COMLINK_CAN_COMCHS);
raise_syntax_error
#ENDIF


FUNCTION GLOBAL __CDECL GetDataBufferFlag
VAR_INPUT
    pObj        : ^VirtualBase;
END_VAR
VAR_OUTPUT
    bSet        : BOOL;
END_VAR;

FUNCTION RemoveMonitorNode
VAR_INPUT
  pNode : ^MonitorNode;
END_VAR;

FUNCTION CheckRefreshlist
VAR_INPUT
  ptc : ^COMDATA;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

FUNCTION INI_ONE_COM_IF
VAR_INPUT
  Number    :INT;
END_VAR
VAR_OUTPUT
  pCOMDATA  : ^COMDATA;  
END_VAR;

FUNCTION FREE_ONE_COM_IF
VAR_INPUT
  Number  :INT;
END_VAR;

FUNCTION GLOBAL __CDECL PrototypeCallback

VAR_INPUT
    pComdef  : ^COMDEF;
    pData    : ^d2LSE;
END_VAR;


FUNCTION CRX
VAR_INPUT
    piCanptr        : ^SINT;
    rxDataptr       : ^sint;
    thisp       : Udint;
END_VAR;

FUNCTION SendAliveMessage_CAN
VAR_INPUT
  pCom : ^COMDATA;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

FUNCTION AliveChecker;
FUNCTION INI_COM_IF;
//function AWL loadlist;
function getspecialbyteSTR
VAR_INPUT
    pVirtual : ^virtualbase; 
END_VAR
VAR_OUTPUT
    retcode   : USINT;
END_VAR;
//++comlinkt:begin
FUNCTION ComlinkT_MissingClientCode;

FUNCTION ComlinkT_Init;

FUNCTION ComlinkT_Login
VAR_INPUT
    pComdef   : ^COMDEF; 
END_VAR
VAR_OUTPUT
    retval    : UINT;
END_VAR;

FUNCTION ComlinkT_Logout
VAR_INPUT
    pComdef   : ^COMDEF; 
END_VAR;

FUNCTION ComlinkT_TxCommandEx
VAR_INPUT
    opCode    : UDINT;
    length    : UDINT;
    aPara     : ^USINT;
    pComdef   : ^comdef; 
    pResu     : ^UDINT;
    pStatus   : ^IPRSTATES;
    res       : UDINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR;

FUNCTION ComlinkT_TxUpd
VAR_INPUT
    pLslcommregdata : ^lslCommregData;
    pComdef         : ^COMDEF; 
    res             : UDINT;
    uiflags         : UINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR;

FUNCTION ComlinkT_InstallCallback
VAR_INPUT
    pCallback : ^void;
END_VAR;

FUNCTION ComlinkT_StartStopRefresh
VAR_INPUT
    pComdef  : ^COMDEF; 
    count    : UINT;
    typ      : UINT;
    res      : UDINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR;

FUNCTION ComlinkT_CyWork;

FUNCTION ComlinkR_Init;

FUNCTION ComlinkR_Login
VAR_INPUT
    pComdef       : ^comdef; 
END_VAR
VAR_OUTPUT
    retval        : UINT;
END_VAR;

FUNCTION ComlinkR_Logout
VAR_INPUT
    pComdef       : ^comdef; 
END_VAR
VAR_OUTPUT
    retval        : UINT;
END_VAR;

FUNCTION ComlinkR_TxCommandEx
VAR_INPUT
    opCode    : UDINT;
    length    : UDINT;
    aPara     : ^USINT;
    pComdef   : ^comdef; 
    pResu     : ^UDINT;
    pStatus   : ^IPRSTATES;
    res       : UDINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR;

FUNCTION ComlinkR_TxUpd
VAR_INPUT
    pLslcommregdata : ^lslCommregData;
    pComdef         : ^comdef; 
    res             : UDINT;
    uiflags         : UINT;
END_VAR
VAR_OUTPUT
    retval          : comlinkReason;
END_VAR;

FUNCTION ComlinkR_InstallCallback
VAR_INPUT
    pCallback : ^void;
END_VAR;

FUNCTION ComlinkR_StartStopRefresh
VAR_INPUT
    pComdef  : ^comdef; 
    count    : UINT;
    typ      : UINT;
    res      : UDINT;
END_VAR
VAR_OUTPUT
    retval   : comlinkReason;
END_VAR;

FUNCTION ComlinkR_CyWork;

FUNCTION ComlinkR_SerTransfer
VAR_INPUT
  pItf : ^RS232_ITF;
END_VAR
VAR_OUTPUT
  event : BOOL;
END_VAR;

FUNCTION SendMaxIdleTime_RS232
VAR_INPUT
  pComdefT : ^comdefT;
  remoteMaxIdleTime_ms : UDINT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

FUNCTION SendLogout
VAR_INPUT
  pComdefT : ^comdefT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

//ws001
FUNCTION GLOBAL __CDECL LDR_SetCanWait
VAR_INPUT
  us_wait : usint;
END_VAR
VAR_OUTPUT
  old : usint;
END_VAR;

FUNCTION GLOBAL LDR_DebugLog0
VAR_INPUT
  fmt : ^CHAR;
END_VAR;

FUNCTION GetAndLockComdata
VAR_INPUT
  pComdef : ^COMDEF;
END_VAR
VAR_OUTPUT
	pt_COM   : ^ComData_Base;
END_VAR;

FUNCTION ReleaseComdata
VAR_INPUT
	pt_COM   : ^ComData_Base;
END_VAR;

//------------------------------------------------------------------------------
// MakeResourceName
//
//  Erstellt einen Resourcenamen (z.für ein Mutex) anhand eines fixen Strings 
//  und einer eindeutigen Nummer (z.B. ein Zeiger auf den Block, in dem die 
//  Resource gespeichert ist).
//  Der als Parameter übergebene Name muß groß genug sein, damit die 9 Ascii 
//  Zeichen der Nummer angehängt werden können
//------------------------------------------------------------------------------
FUNCTION MakeResourceName
VAR_INPUT
  number : UDINT;
  name : ^CHAR;
END_VAR
VAR
  len : UDINT;
  i : INT;
END_VAR

  len := _strlen(name);
  FOR i:= 0 TO 8 DO
    (name + len + i)^ := 0;
  END_FOR;
  _itoa(number, name + len);

END_FUNCTION

//=== SEMAPOOL: Anfang
#define SEMA_POOL_SIZE 16

VAR_PRIVATE
  poolMutex : MT_SEMAHANDLE;
  hMutexPool : ARRAY[0..SEMA_POOL_SIZE-1] OF MT_SEMAHANDLE;
  hCountingSemaPool : ARRAY[0..SEMA_POOL_SIZE-1] OF MT_SEMAHANDLE;
END_VAR

FUNCTION InitSemaPool
VAR
  i : UDINT;
  mutexName : ARRAY[0..30] OF CHAR;
END_VAR

  poolMutex := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                     1,    // initvalue
                                     0,    // flags: 0 means create unconitionally
                                     "COMLINK_SEMAPOOL"
                                     );

  _strcpy(#mutexName[0], "COMLINK_P_MUTEX__");
  FOR i := 0 TO SEMA_POOL_SIZE-1 DO
    mutexName[_strlen(#mutexName[0])-1] := 'A' + i$USINT;
    hMutexPool[i] := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                        1,    // initvalue
                                        0,    // flags: 0 means create unconitionally
                                        #mutexName[0]
                                        );
  END_FOR;

  _strcpy(#mutexName[0], "COMLINK_P_COUNTING__");
  FOR i := 0 TO SEMA_POOL_SIZE-1 DO
    mutexName[_strlen(#mutexName[0])-1] := 'A' + i$USINT;
    hCountingSemaPool[i] := OS_MT_CreateSemaphore(MTSEMATYPE_COUNTING, 
                                            1,    // initvalue
                                            0,    // flags: 0 means create unconitionally
                                            #mutexName[0]
                                            );
  END_FOR;

END_FUNCTION

FUNCTION GetMutexFromPool
VAR_INPUT
  counting : BOOL;
END_VAR
VAR_OUTPUT
  hSema : MT_SEMAHANDLE;
END_VAR
VAR
  i : UDINT;
END_VAR

  hSema := NIL;

  OS_MT_Wait(poolMutex);
  FOR i := 0 TO SEMA_POOL_SIZE-1 DO
    IF counting THEN
      IF hCountingSemaPool[i] THEN
        hSema := hCountingSemaPool[i];
        hCountingSemaPool[i] := NIL;
        EXIT;
      END_IF;
    ELSE
      IF hMutexPool[i] THEN
        hSema := hMutexPool[i];
        hMutexPool[i] := NIL;
        EXIT;
      END_IF;
    END_IF;
  END_FOR;
  OS_MT_Signal(poolMutex);

END_FUNCTION

FUNCTION PutMutexToPool
VAR_INPUT
  counting : BOOL;
  hSema : MT_SEMAHANDLE;
END_VAR
VAR_OUTPUT
  pooled : BOOL;
END_VAR
VAR
  i : UDINT;
END_VAR

  pooled := FALSE;

  OS_MT_Wait(poolMutex);
  FOR i := 0 TO SEMA_POOL_SIZE-1 DO
    IF counting THEN
      IF hCountingSemaPool[i] = NIL THEN
        hCountingSemaPool[i] := hSema;
        pooled := TRUE;
        EXIT;
      END_IF;
    ELSE
      IF hMutexPool[i] = NIL THEN
        hMutexPool[i] := hSema;
        pooled := TRUE;
        EXIT;
      END_IF;
    END_IF;
  END_FOR;
  OS_MT_Signal(poolMutex);

END_FUNCTION
//=== SEMAPOOL: Ende

FUNCTION NewComchMutex
VAR_INPUT
  Number    : UDINT;
  id        : USINT;
END_VAR
VAR_OUTPUT
  hMutex    : MT_SEMAHANDLE;
END_VAR
VAR
  mutexName : ARRAY[0..30] OF CHAR;
END_VAR

    hMutex := GetMutexFromPool(FALSE(*counting*));
    IF hMutex = NIL THEN
      IF mt_api <> NIL THEN
          _strcpy(#mutexName[0], "COMCH_MUTEX__");
          mutexName[_strlen(#mutexName[0])-1] := id;
          MakeResourceName(Number, #mutexName[0]);
          hMutex := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                          1,    // initvalue
                                          0,    // flags: 0 means create unconitionally
                                          #mutexName[0]
                                          );
      END_IF;
    END_IF;

END_FUNCTION

FUNCTION DeleteComchMutex
VAR_INPUT
  hMutex    : ^MT_SEMAHANDLE;
END_VAR

    IF (mt_api <> NIL) & (hMutex^ <> NIL) THEN
        IF !!PutMutexToPool(FALSE(*counting*), hMutex^) THEN
          OS_MT_DeleteSemaphore(hMutex^);
        END_IF;
        hMutex^ := NIL;
    END_IF;

END_FUNCTION

FUNCTION _Lock
VAR_INPUT
  hMutex    : MT_SEMAHANDLE;
END_VAR

    IF (mt_api <> NIL) & (hMutex <> NIL) THEN
        //DEBUGLOG1("%010d-_Lock: hMutex=%d", hMutex$UDINT);
        OS_MT_Wait(hMutex);
    END_IF;

END_FUNCTION
FUNCTION _Unlock
VAR_INPUT
  hMutex    : MT_SEMAHANDLE;
END_VAR

    IF (mt_api <> NIL) & (hMutex <> NIL) THEN
        //DEBUGLOG1("%010d-_Unlock: hMutex=%d", hMutex$UDINT);
        OS_MT_Signal(hMutex);
    END_IF;

END_FUNCTION
FUNCTION GLOBAL ComlinkInit
VAR
  pSysenv : ^OS_SYSENV;
  value : ARRAY[0..9] OF CHAR;
  temp, i : UDINT;
END_VAR

    g_nbrConnBlk := 0;
    g_nbrDynList := 0;
    g_nbrStatList := 0;
    g_nbrLoginCtx := 0;
    OPS.CH16buf.res1$int :=-1;  // loesche commando über can !! login 
    
   #ifdef _LSL_TARGETARCH_ARM
    if((#OPS.CH16buf.res1)$udint and 3) then
      // diese addresse muss auf 4 byte ausgerichtet sein
      // ansonsten problem bei function ComplexSwap im modul Comlink.st
      TRACE_WARN("ComLink with can-bus will not work...");      
    end_if;
   #endif
    
    login_anz  :=1;             // nur lokal ist vorhanden
    canCltCh_anz := 0;
    LDR_canwait := 2; //ws001
    canNrSvr := 1; // Default CAN 1
    OS_CILGet("SYSENV", #pSysenv$void);
    IF pSysenv & OS_GETENVVAR(pSysenv, "COMLINKCAN", #value[0], SIZEOF(value)) THEN
      temp := 0;
      FOR i := 0 TO _strlen(#value[0]) - 1 DO
        IF value[i] < '0' | value[i] > '9' THEN
          temp := 0;
          EXIT;
        END_IF;
        temp := temp * 10 + value[i] - '0';
      END_FOR;
      IF temp > 0 & temp < 256 THEN
        canNrSvr := temp$USINT;
      END_IF;
    END_IF;

    g_max_logins := MAX_LOGINS_DEFAULT;
    IF pSysenv & OS_GETENVVAR(pSysenv, "COMLINK_MAX_LOGINS", #value[0], SIZEOF(value)) THEN
      temp := 0;
      FOR i := 0 TO _strlen(#value[0]) - 1 DO
        IF value[i] < '0' | value[i] > '9' THEN
          temp := 0;
          EXIT;
        END_IF;
        temp := temp * 10 + value[i] - '0';
      END_FOR;
      IF temp < 1  THEN
        g_max_logins := 1;
      ELSIF temp > arraysize(loginArray) THEN
        g_max_logins := arraysize(loginArray);
      ELSE
        g_max_logins := temp;
      END_IF;
    END_IF;
    
    _MEMSET(#myComChs[0], 0, SIZEOF(myComChs));
    _memset(#usedsocks[0], ' ', sizeof(usedsocks));

    IF OS_CILGet("LSL_MULTITASK", #mt_api$void) THEN
        mt_api := NIL;
   #ifdef COMLINK_PG 
        MutexPg50 := NIL;
    else
        MutexPG50 := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                           1,    // initvalue
                                           0,    // flags: 0 means create unconitionally
                                           "MutexPG50");
   #endif
    END_IF;

    OS_CILGet("SYSTRACE", #pSystrace$void);

#ifdef DEBUG
  IF OS_CILGet("SYSMSG", #OS_pLslSysMsg$void) THEN
    RETURN;
  END_IF;
  // write to C:\SYSMSG\EVENT18.LOG
  hLog0 := OS_SYSMSG_LCREATE(8, #LogBuf0[0], arraysize(LogBuf0), 200000, 0);
#endif

  OS_CILGET("CANUSER", #pCanuser);

    InitSemaPool();

    INI_COM_IF();               // speicher reservieren für 1 object (lokal)
   
   #ifdef COMLINK_TCP
    ComlinkT_Init(); //++comlinkt
   #endif   

    ComlinkR_Init();

    monitorNodes := NIL;
    monitorNodeMutex := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                           1,    // initvalue
                                           0,    // flags: 0 means create unconitionally
                                           "COMLINKMN"
                                           );
    
    g_RS232activity := FALSE;
    
    max_conns := MAX_CONNS_DFLT;
    IF pSysenv & OS_GETENVVAR(pSysenv, "COMLINK_MAX_SERVER_CONNECTIONS", #value[0], SIZEOF(value)) THEN
      temp := 0;
      FOR i := 0 TO _strlen(#value[0]) - 1 DO
        IF value[i] < '0' | value[i] > '9' THEN
          temp := 0;
          EXIT;
        END_IF;
        temp := temp * 10 + value[i] - '0';
      END_FOR;
      IF temp > 0 & temp < 256 THEN
        max_conns := temp - 1;
      END_IF;
    END_IF;
    
END_FUNCTION // Comlink::Comlink

#ifdef _LSL_TARGETARCH_X86
function AWL callback
 VAR
   data2LSE                     : d2LSE;            
 END_VAR

// --> EDI points on comdata

    push        esi                         // save the this-pointer

    s.bx        data2lse.uioffs
    s.eax       data2lse.data
    s.ecx       data2lse.varlistID
    lea.ebx     data2Lse
    l.esi       (edi+comdata.pThis)         // also the this pointer to function
    push        ebx
    l.ebx       (edi+comdata.pComdef)
    push        ebx
    call        (edi+comdata.pcallback)     // pointer on callback
    add.esp     8

    pop         esi                         // restore this

end_function
#else
function callback
  var_input
    pcd  : ^comData;
  	pd2LSE : ^d2LSE;
  end_var
  
  if(pcd^.pCallback <> nil) then
    esi := pcd^.pThis;
    pcd^.pCallBack$PrototypeCallback(pcd^.pcomdef, pd2LSE);
  end_if;  
  
end_function
#endif
//------------------------------------------------------------------------------
// InvalidateRefListEntry
//
//  Sucht in einer Refresh-Liste einen Eintrag, wenn ergefunden wird, dann 
//  wird dData auf INVALID (16#80000001) und in udUpdateRate wird das Bit 
//  UPDRATE_FLAG_FORCE_CHANGE gesetzt. Das soll bewirken, dass bei der nächsten 
//  Bearbeitung der Refreshliste der Wert übertragen wird.
//------------------------------------------------------------------------------
FUNCTION InvalidateRefListEntry
VAR_INPUT
    pUpd        : ^COMSTCELL;
    nbrOfElems  : UINT;
    obj         : pVoid;
END_VAR

  while(nbrOfElems <> 0) do
    if(obj = pUpd^.pCh) then
      pUpd^.dData := 16#80000001$DINT; // gefunden -> dData auf INVALID ändern
      pUpd^.udUpdateRate := pUpd^.udUpdateRate or UPDRATE_FLAG_FORCE_CHANGE;
    end_if;

    nbrOfElems -= 1;
    pUpd       += sizeof(comStCell);
  end_while;

END_FUNCTION
//------------------------------------------------------------------------------
// SyncRefLists
//
//  Prüft, ob durch einen Befipr Befehl ein Eintrag in der Refreshliste nicht 
//  mehr mit dem tatsächlichen Wert übereinstimmt. Wenn das der Fall ist, dann 
//  wird dData des Refreshlisteneintrags auf INVALID gesetzt, damit bei der 
//  nächsten Abtastung der Wert an den Client (Visu) gesendet.
//  Diese Funktion ist notwendig, weil die Visu sich nicht nur von der Refresh-
//  liste über den tatsächlichen Wert informieren läßt, sondern weil sie von 
//  sich aus den angezeigten Wert nach einem I_WRITE Befehl ändert.
//  Das funktioniert dann nicht mehr, wenn sich der Serverwert nach dem I_WRITE 
//  und vor der nächsten Abtastung wieder auf den alten Wert ändert, weil die 
//  Refreshliste dann keine Änderung mitkriegt.
//------------------------------------------------------------------------------
FUNCTION SyncRefLists
VAR_INPUT
    pCom    : ^COMDATA;
END_VAR
VAR
    obj     : pVoid;
    pHead   : ^IprHead; 
    instr   : ^instruct;
END_VAR
    
#ifndef DUMMY_LOADER
    pHead := pCom^.Com.GetHead();
#endif    
    instr := pHead^.pCode;

    IF instr^.opCode = I_WRITE THEN

        obj := instr^.aPara[0]$^pVoid;

        // statische Refresh-Liste durchsuchen
        InvalidateRefListEntry(pCom^.paStatList, pCom^.nStateS, obj);

        // dynamische Refresh-Liste durchsuchen
        InvalidateRefListEntry(pCom^.paDynList,  pCom^.nState,  obj);

    END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// CanTxObj - senden einer CAN Message
//------------------------------------------------------------------------------
FUNCTION CanTxObj
VAR_INPUT
  pCom : ^COMDATA;
  msgBuf : PVOID;
END_VAR
VAR_OUTPUT
  retVal : INT;
END_VAR

  retVal := OS_SSR_CanTxObj(pCom^.canNr, pCom^.OurTxObj, 8, msgBuf);
  IF retVal = 0 THEN
    pCom^.lastSendTime_ms := ops.tAbsolute;
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// this function waits for the can bremse (usually 2 ms)
//------------------------------------------------------------------------------
FUNCTION GLOBAL WaitForCan
VAR_INPUT
  pt_com   : ^COMDATA;
END_VAR

 #ifdef _LSL_USECLISTI
  STI();
 #endif 
  WHILE (OPS.tAbsolute - pt_com^.OldTabsolute) < LDR_canwait (*ws001 2 *) DO //ws001: removed fixed wait state
    OS_SSR_Delay(1);  // 1ms blockieren spart CPU Resourcen
  END_WHILE;  
  
  pt_com^.OldTabsolute :=OPS.tAbsolute;

END_FUNCTION

//------------------------------------------------------------------------------
// CanTxObj_WithBremse - senden einer CAN Message mit Einhaltung der CAN-Bremse
//------------------------------------------------------------------------------
FUNCTION CanTxObj_WithBremse
VAR_INPUT
  pCom : ^COMDATA;
  msgBuf : PVOID;
END_VAR
VAR_OUTPUT
  retVal : INT;
END_VAR
VAR
  us_oldwait : USINT;
END_VAR

  WaitForCan(pCom);
  retVal := CanTxObj(pCom, msgBuf);
  IF (retVal = 4) THEN // 4 = buffer full
  
    us_oldwait := LDR_SetCANWait( 2 );  // 2ms sollte ausreichen, bis wieder Platz im Buffer ist
    WaitForCan( pCom );
    LDR_SetCANWait( us_oldwait );
    retVal := CanTxObj(pCom, msgBuf);
    
  END_IF;
        
END_FUNCTION
     
FUNCTION AWL ComplexSwap
	VAR_INPUT
		address 	: ^void;
	END_VAR
  VAR_OUTPUT
    retcode : int;
  END_VAR
  
#ifdef _LSL_TARGETARCH_ARM
  // hier wird der inhalt von OPS.CH16buf.res1$INT gegen -1 "atomar geswappt"
  // arm kann aber kein swap16, nur swap8 und swap32 -> deshalb etwas komplexer
  // vorraussetzung: OPS.CH16buf.res1$INT muss auf einer durch 4 teilbaren adresse liegen
  // die funktion retouniert den inhalt von OPS.CH16buf.res1$INT vor dem swap
  // info: auf swp-hi-word liegen die unteren 16 bit von descCrc, diese ändern sich während betrieb nicht

  MOVW     r11, #16#FFFF 
  LDRH     retcode, [address]
  CMP      retcode, r11
  BEQ      ComplexSwapEnd
    
  STMDB    sp!, {r6}  
  LDR      r6, [address]
  ORR      r6, r6, r11
  SWP      r6, r6, [address]
  AND      retcode, r6, r11
  LDMIA    sp!, {r6}  
  
ComplexSwapEnd    
#else
  l.edi    address
  l.ax     16#FFFF
  xch.ax   (edi)
  s.ax     retcode
#endif

END_FUNCTION
      
//------------------------------------------------------------------------------
// Check4NewConnects
//
//  Prüft die OS-Variable OPS.CH16buf.res1 ob sie ungleich -1 ist.
//  Wenn ja, dann heißt das, daß sich jemand über CAN verbinden möchte.
//  Es wird dann ein neues Empfangsobjekt eingerichtet und (ab V1.1.3) der 
//  Befehl 4004 (Refreshlisten-Größen) und 4003 (Loginbestätigung) 
//  zurückgeschickt.
//------------------------------------------------------------------------------
FUNCTION Check4NewConnects
VAR_INPUT
    pCom    : ^COMDATA;
END_VAR
VAR
    txinfo4003  : COMOBJSTR;
    txinfo4004  : COMOBJSTR;
    txinfo4005  : COMOBJSTR;
    i           : INT;
    pirq        : ^VOID;
END_VAR

   #ifndef _LSL_USECLISTI
    txinfo4003.ilng := ComplexSwap(#OPS.CH16buf.res1);
    if(txinfo4003.ilng <> -1) then
   #else
    CLI();
    IF OPS.CH16buf.res1$INT <> -1 THEN      
      txinfo4003.ilng := OPS.CH16buf.res1$INT;
      OPS.CH16buf.res1$INT := -1;
      STI();
   #endif
      txinfo4003.ioff := 4003;
 
      // ---------------------------------------
      //  might a restart on the other station 
      // --------------------------------------- 
      // --> check if already exists

      FOR i:=0 TO (login_anz-1) DO
        pCom := myComChs[i];
        IF pCom <> NIL & pCom^.HisTxObj = txinfo4003.ilng THEN
          txinfo4003.dData := pCom$DINT;
          DEBUGLOG2("%010d-NewCo: found existing ComCh #%d for HisTxObj 16#%X", TO_UDINT(i), TO_UDINT(pCom^.HisTxObj));
          GOTO is_on;   // YES --> so just clear everything
        END_IF;
      END_FOR;

      pcom := INI_ONE_COM_IF(login_anz);
      IF pcom <> NIL THEN
        IF CheckRefreshlist(pcom) <> 0 THEN
          pcom := NIL;
        END_IF;
      END_IF;
      IF pcom <> NIL THEN

        DEBUGLOG1("%010d-NewCo: allocated ComCh #%d", TO_UDINT(login_anz));

        pirq := #CRX();
        login_anz += 1;
        txinfo4003.dData := pcom$DINT;
        pCom^.HisTxObj := txinfo4003.ilng;
        pCom^.OurTxObj := txinfo4003.ilng + 1;

        DEBUGLOG3("%010d-NewCo: using ComCh #%d, HisTxObj 16#%X, OurTxObj 16#%X", TO_UDINT(login_anz-1), TO_UDINT(pCom^.HisTxObj), TO_UDINT(pCom^.OurTxObj));

      //  cannr0: USINT;objnr0: DINT;length0: USINT;mode0: USINT;actionptr0: PVOID;
      //  thisptr0 : PVOID;
        pCom^.canNr := canNrSvr;
        pCom^.hishandle := to_usint(OS_SSR_AddCanObj(pCom^.canNr, pCom^.HisTxObj, 8, 1, pirq, pCom)); 
        IF pCom^.hishandle >= 0 THEN
is_on:
          pCom^.updssw     := 0;
          pCom^.nStateS    := 0;
          pCom^.nActS      := 0;
          pCom^.nState     := 0;
          pCom^.nAct       := 0;
          pCom^.pThis      := 0;      // this = 0 
    
          // Meldung 4004 enthält die Größen der eigenen Refreshlisten
          txinfo4004.iLng  := 0;
          txinfo4004.iOff  := 4004;
          txinfo4004.dData$UDINT := (pCom^.sizeofDynList SHL 16) + pCom^.sizeofStatList;

          // Meldung 4005 enthält in den unteren 2 Bytes den SW-Runtime-Wert (in 10ms Einheiten)
          txinfo4005.iLng  := 0;
          txinfo4005.iOff  := 4005;
          txinfo4005.dData := TO_DINT(_swruntime);

          // Bevor die Login Bestätigung geschickt wird, das Feld m4005 auf 16#FFFFffff 
          // setzen, damit eine eingehende 4005 Meldung erkannt wird.
          pCom^.m4005 := 16#FFFFffff;

          //ws003
          i := CanTxObj(pCom, #txinfo4005$void);
          i += CanTxObj(pCom, #txinfo4004$void);
          i += CanTxObj(pCom, #txinfo4003$void);
          if( i <> 0 )then
            DEBUGLOG1("%010d-NewCo: *** AddCanObj failed for ComCh #%d (maybe buffer is full)", TO_UDINT(login_anz-1));
          end_if;
          // /ws003
        ELSE
          DEBUGLOG1("%010d-NewCo: *** AddCanObj failed for ComCh #%d", TO_UDINT(login_anz-1));
          pCom^.hishandle := -1$UINT;
        END_IF;

      ELSE
        DEBUGLOG1("%010d-NewCo: *** INI_ONE_COM_IF failed for ComCh #%d", TO_UDINT(login_anz));
      END_IF;

    END_IF;
   #ifdef _LSL_USECLISTI
    STI();
   #endif 

END_FUNCTION


//------------------------------------------------------------------------------
//  Funktion prüft globale Variablen auf Änderungen
//  die max. Grösse einer glob.variable ist 255 Byte. 
//  ist bei der Grösse 0 Byte eingetragen ist so handelt es sich immer um eine 4 byte Variable
//------------------------------------------------------------------------------
FUNCTION UpDateGlobalVar
VAR_INPUT
  psrc : ^void; 
  udUpdataRate : udint; 
END_VAR 
VAR_OUTPUT
  retcode : dint; 
END_VAR
VAR
  size : udint;
END_VAR

  size := (udUpdataRate shr 16) and 16#000000FF; 
  
  if(size = 0) then
    retcode$udint := psrc$^udint^;
  elsif(size = 1) then
    retcode$udint := psrc$^usint^;
  elsif(size = 2) then
    retcode$udint := psrc$^uint^;
  elsif(size = 3) then // ! zb: 3 byte struktur 
    retcode$udint := (psrc$^udint^) AND 16#00FFFFFF;
  elsif(size = 4) then
    retcode$udint := psrc$^udint^;
  else
    retcode$udint := CRC32_Buffer(psrc, size);
  end_if;
  
END_FUNCTION


//------------------------------------------------------------------------------
// Check4CanTransmits
//
//  Prüft ob etwas über den CAN geschickt werden muß.
//  Diese Funktion wird zyklisch aufgerufen.
//------------------------------------------------------------------------------
FUNCTION Check4CanTransmits
VAR_INPUT
  pCom : ^COMDATA;
  pHead : ^IprHead; 
  pResu : ^Results;
END_VAR
VAR
 txinfo                          : COMOBJSTR;
 rest                            : INT;
 pUpd                            : ^COMSTCELL;
 data2LSE                        : d2LSE;
 original_this                   : UDINT;
#ifdef COMLINK_PG 
 retry                           : UINT;
#endif 
END_VAR

start:
  //------------------------------------------------------------------
  // chk for comand answer
  //------------------------------------------------------------------
  IF pCom^.nTx <> 0 THEN

    IF pCom^.nTx = - 1 THEN
com_qu:
      txinfo.iLng := 4;
      txinfo.iOff := 0; // absolut 0 because we have different struct
      txinfo.dData := pHead^.workstate$DINT;

      //DEBUGLOG0("%010d-Check4CanTransmits:CanTxObj 1");//++pr:test
      IF CanTxObj(pCom, #txinfo$void) <> 4 THEN // 4 = buffer full
        pCom^.nTx := 0;
        IF pCom^.canBremseSvr = 0 THEN
          GOTO start;
        END_IF;
      ELSE
        //DEBUGLOG0("%010d-Check4CanTransmits:***buffer full***");//++pr:test
      END_IF;

    ELSE
      rest  := (pResu^.uiLng - pCom^.xTx$UINT)$INT;

      IF rest = 0 THEN
        pCom^.nTx:=-1;
        GOTO com_qu;
      ELSE
        IF rest < 4 THEN
          txinfo.ilng := rest;
        ELSE
          txinfo.ilng := 4;
        END_IF;
        txinfo.ioff  := 1000 + pCom^.xTx; // we are absolut
        txinfo.dData := (pResu + TO_UDINT(pcom^.xTx))^$DINT;

        //DEBUGLOG0("%010d-Check4CanTransmits:CanTxObj 2");//++pr:test
        IF CanTxObj(pCom, #txinfo$void) <> 4 THEN // 4 = buffer full
          pCom^.xTx += txinfo.ilng;
          IF pCom^.canBremseSvr = 0 THEN
            GOTO start;
          END_IF;
        ELSE
         //DEBUGLOG0("%010d-Check4CanTransmits:***buffer full***");//++pr:test
        END_IF;

      END_IF;
    END_IF;

    //------------------------------------------------------------------
    // do updates of statelist if there is no answer to tx
    //------------------------------------------------------------------
  ELSE

    IF (pCom^.nState <> 0) | (pCom^.nStateS <> 0) THEN
upd_lp:
      // check if we have already done the dynlist the do the staticlist
      IF pCom^.nAct >= pCom^.nState THEN
        // check if we are at the end of the static begin with the dynamic
        IF pCom^.nActS >= pCom^.nStateS THEN
          pCom^.nAct  := 0;  // start both of them
          pCom^.nActS := 0;
             GOTO upd_end;
        END_IF;

        pUpd := pCom^.pastatList + (pCom^.nActS * SIZEOF(comStCell));
        IF OPS.TAbsolute - pUpd^.udLastUpdate >= (pUpd^.udUpdateRate AND 16#ffff) THEN
          pUpd^.udLastUpdate := OPS.TAbsolute;

#ifdef COMLINK_PG
          // jetzt prüfen wir ob lokal oder doch über die Pg50 funktionen
          IF pCom^.HisTxObj < 1 THEN
            IF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_VBI) THEN
              txinfo.dData:=(pUpd^.pCh$^virtualbaseinit^.getcrc())$DINT;
			ELSIF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_MERKEREX) THEN
      			txinfo.dData := (pUpd^.pCh$^merkerex^.getcrc())$DINT;
            ELSIF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_GLOBALVAR) THEN
              txinfo.dData := (pUpd^.pCh$^DINT)^;
            ELSE
              txinfo.dData:=RD(pUpd^.pCh);
            END_IF;
          ELSE
            IF (pUpd^.udUpdateRate and UPDRATE_FLAG_IS_VBI) THEN
              // ist immer ungleich !! geht dann mit txcommand,I_CMD
              txinfo.dData := pUpd^.dData+1; 
            ELSE
              DEBUGLOG1("%010d - CyWork CAN1, Chk-Stat, seg/ofs=0x%08X", pUpd^.pCH$UDINT);
              _lock(MutexPG50);  // #FA1204
              FOR retry:=0 TO 3 DO  
                IF CANUSER_GetBlockPG(CanNr:=1,
                                      Station:=pCom^.hishandle$USINT,
                                      SegAdr:= (pUpd^.pCH$UDINT SHR 16)$UINT,
                                      OfsAdr:= (pUpd^.pCH$UDINT AND 16#ffff)$UINT,
                                      len:=4,
                                      #txinfo.dData$USINT,
                                      ReqType:=0) = 0 THEN
                  DEBUGLOG1("%010d - CyWork CAN1, Chk-Stat, data=0x%08X", txinfo.dData$UDINT);
                  EXIT;
                END_IF;  
              END_FOR;
              _Unlock(hMutex:=MutexPg50);
            END_IF;
          END_IF;
#else
          IF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_VBI) THEN
            txinfo.dData := (pUpd^.pCh$^virtualbaseinit^.getcrc())$DINT;
		  ELSIF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_MERKEREX) THEN
      		txinfo.dData := (pUpd^.pCh$^merkerex^.getcrc())$DINT;
          ELSIF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_GLOBALVAR) THEN
//            txinfo.dData := (pUpd^.pCh$^DINT)^;
            txinfo.dData := UpDateGlobalVar(pUpd^.pCh, pUpd^.udUpdateRate);
          ELSE
            txinfo.dData := RD(pUpd^.pCh);
          END_IF;
#endif          

          IF txinfo.dData <> pUpd^.dData THEN
            IF pCom^.HisTxObj < 1 THEN 
              // call the callback function we are internally connected
              pUpd^.dData:=txinfo.dData; 
              data2LSE.data:=txinfo.dData;
              data2LSE.uiOffs:=pCom^.nActS;
              data2LSE.varlistID := pUpd^.varlistID;  // just for LSE 

              // es wird auch der ThisPointer übergeben (neu seit 27.01.2004)
              original_this := ESI;
              ESI := pCom^.pThis;
              pCom^.pCallBack$PrototypeCallback(pCom^.pcomdef,#data2LSE);
              ESI := original_this;
              // fault +2 #FA141103 pCom^.nActS+=1;
              // faster screen #AF17062001 GOTO upd_end;
            ELSIF pCom^.HisTxObj = 1 THEN
            ELSE 
#ifdef COMLINK_PG
              // call the callback function 
              pUpd^.dData:=txinfo.dData;
              data2LSE.data:=txinfo.dData;
              data2LSE.uiOffs := pCom^.nActS;
              data2LSE.varlistID := pUpd^.varlistID;	// just for LSE 
      
              // es wird auch der ThisPointer übergeben (neu seit 27.01.2004)
              original_this := ESI;
              ESI := pCom^.pThis;
              pCom^.pCallBack$PrototypeCallback(pCom^.pcomdef,#data2LSE);
              ESI:=original_this; 
      
              pCom^.nActS+=1;
              GOTO upd_end;
#else
              pUpd^.dData:=txinfo.dData;
              txinfo.ilng:=4;
              txinfo.ioff:=5000+pCom^.nActS$INT;
              CanTxObj(pCom, #txinfo$void);
              pCom^.nActS+=1;
              GOTO upd_end;
#endif              
            END_IF;
          END_IF;
        END_IF;
        pCom^.nActS+=1;
        GOTO upd_lp;
         
      ELSE

        pUpd:=pCom^.paDynList+(pCom^.nAct*SIZEOF(comStCell));
        IF OPS.TAbsolute -pUpd^.udLastUpdate >= (pUpd^.udUpdateRate AND 16#FFFF) THEN
          pUpd^.udLastUpdate:=OPS.TAbsolute;

#ifdef COMLINK_PG
          txinfo.ddata := -1;
          // test ob pg50 oder lokal
          IF pCom^.HisTxObj < 1 THEN
            IF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_VBI) THEN
              txinfo.dData:=(pUpd^.pCh$^virtualbaseinit^.getcrc())$DINT;
			ELSIF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_MERKEREX) THEN
      		  txinfo.dData := (pUpd^.pCh$^merkerex^.getcrc())$DINT;
            ELSIF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_GLOBALVAR) THEN
              txinfo.dData := (pUpd^.pCh$^DINT)^;
            ELSE
              txinfo.dData:=RD(pUpd^.pCh);
            END_IF;
          ELSE
            IF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_VBI) THEN
              // ist immer ungleich !! geht dann mit txcommand,I_CMD
              txinfo.dData := pUpd^.dData+1; 
            ELSE
              DEBUGLOG1("%010d - CyWork CAN1, Chk-Dyn , seg/ofs=0x%08X", pUpd^.pCH$UDINT);
              _lock(MutexPG50);
              FOR retry:=0 TO 3 DO
                IF CANUSER_GetBlockPG(CanNr:=1,
                                      Station:=pCom^.hishandle$USINT,
                                      SegAdr:= (pUpd^.pCH$UDINT SHR 16)$UINT,
                                      OfsAdr:= (pUpd^.pCH$UDINT and 16#ffff)$UINT,
                                      len:=4,
                                      #txinfo.dData$USINT,
                                      ReqType:=0) = 0 THEN
                  DEBUGLOG1("%010d - CyWork CAN1, Chk-Dyn , data=0x%08X", txinfo.dData$UDINT);
                  EXIT;
                END_IF;
              END_FOR;
              _Unlock(hMutex:=MutexPG50);
            END_IF;
          END_IF;
#else
          IF (pUpd^.udUpdateRate and UPDRATE_FLAG_IS_VBI) THEN
            txinfo.dData:=(pUpd^.pCh$^virtualbaseinit^.getcrc())$DINT;
		  ELSIF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_MERKEREX) THEN
      	 	txinfo.dData := (pUpd^.pCh$^merkerex^.getcrc())$DINT;
          ELSIF (pUpd^.udUpdateRate AND UPDRATE_FLAG_IS_GLOBALVAR) THEN
//            txinfo.dData := (pUpd^.pCh$^DINT)^;
            txinfo.dData := UpDateGlobalVar(pUpd^.pCh, pUpd^.udUpdateRate);
          ELSE
            txinfo.dData:=RD(pUpd^.pCh);
          END_IF;
#endif          

          IF txinfo.dData <> pUpd^.dData THEN

            // ** check if CAN / TCPIP / Intern Lokal
            IF pCom^.HisTxObj < 1 THEN 
             // call the callback function 
              pUpd^.dData:=txinfo.dData;
              data2LSE.data:=txinfo.dData;
              data2LSE.uiOffs:=pCom^.nAct+1000;
              data2LSE.varlistID := pUpd^.varlistID;  // just for LSE 

              // es wird auch der ThisPointer übergeben (neu seit 27.01.2004)
              original_this := ESI;
              ESI := pCom^.pThis;
              pCom^.pCallBack$PrototypeCallback(pCom^.pcomdef,#data2LSE);
              ESI:=original_this; 

            ELSIF pCom^.HisTxObj = 1 THEN
            ELSE 
#ifdef COMLINK_PG
              // call the callback function 
              pUpd^.dData:=txinfo.dData;
              data2LSE.data:=txinfo.dData;
              data2LSE.uiOffs:=pCom^.nAct+1000;
              data2LSE.varlistID := pUpd^.varlistID;	// just for LSE 
      
              // es wird auch der ThisPointer übergeben (neu seit 27.01.2004)
              original_this := ESI;
              ESI := pCom^.pThis;
              pCom^.pCallBack$PrototypeCallback(pCom^.pcomdef,#data2LSE);
              ESI:=original_this; 
      
              pCom^.nAct+=1;
              GOTO upd_end;
#else
              pUpd^.dData := txinfo.dData;
              txinfo.ilng := 4;
              txinfo.ioff := 6000+pCom^.nAct$INT;
              CanTxObj(pCom, #txinfo$void);
              pCom^.nAct += 1;
              GOTO upd_end;
#endif              
            END_IF;
          END_IF;
        END_IF;
        pCom^.nAct += 1;
          GOTO upd_lp;
        END_IF;
      END_IF;
    END_IF;

upd_end:

END_FUNCTION
//------------------------------------------------------------------------------
// Check4AliveCmds
//
//  Prüft ob ein neuer Wert für die maxIdleTime eingetroffen ist.
//------------------------------------------------------------------------------
FUNCTION Check4AliveCmds
VAR_INPUT
  pCom : ^COMDATA;
END_VAR

  IF pCom^.maxIdlTim_new <> pCom^.maxIdlTim_old THEN
    pCom^.maxIdleTime_ms := TO_UDINT(pCom^.maxIdlTim_new);
    pCom^.maxIdlTim_old := pCom^.maxIdlTim_new;
    pCom^.lastSendTime_ms := ops.tAbsolute - pCom^.maxIdleTime_ms; // damit gleich was geschickt wird
  END_IF;

END_FUNCTION

FUNCTION GLOBAL LDR_MakeTraceEntry
VAR_INPUT
  value : UDINT;
END_VAR
  OS_MakeTraceEntry(pSystrace, TREC_USER_0, value, value);      
END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkCyWork
//------------------------------------------------------------------------------
FUNCTION GLOBAL ComlinkCyWork
VAR
  pCom                            : ^COMDATA;
  i                               : INT;
  bNext                          : UDINT;
  befipr_state                    : IPRSTATES;
  pHead                          : ^IprHead;    
  pResu                          : ^Results;
END_VAR

  TRACE_ENTRY(0);

#ifdef COMLINK_TCP
  #ifdef LOADER_TCP_MODERATE
    if (ComlinkCntr) then
  #endif
      ComlinkT_CyWork(); //TCP/IP
  #ifdef LOADER_TCP_MODERATE
      ComlinkCntr := 0;
    else
      ComlinkCntr := 1;
    end_if;
  #endif
#endif

  ComlinkR_CyWork(); //RS232

  FOR i:= 0 TO (login_anz-1) DO
    pCom := myComChs[i];
    IF pCom <> NIL & pCom^.HisTxObj <> - 1 THEN

      Check4AliveCmds(pCom);
      IF pCom^.maxIdleTime_ms > 0 THEN
        IF ops.tAbsolute - pCom^.lastSendTime_ms > pCom^.maxIdleTime_ms THEN
          SendAliveMessage_CAN(pCom); // der Gegenstelle mitteilen, daß man noch lebt
        END_IF;
      END_IF;

      IF (OPS.TAbsolute - pCom^.udLastUpdate) >= TO_UDINT(pCom^.canBremseSvr) THEN // dont overload Canbus
        pCom^.udLastUpdate := OPS.TAbsolute;

#ifndef DUMMY_LOADER
        pHead := pCom^.Com.GetHead();
        pResu := pCom^.Com.GetResult();
#endif        

        Check4CanTransmits(pCom, pHead, pResu);
        //--------------------------------------------------------------------
        // chk if there is some action to do
        //--------------------------------------------------------------------

        IF pHead^.workstate = BUSY THEN
          // this is to check if we are logged in or we are the slave
          // because txcomand is alos writing to workstate
          IF (pCom^.isClient = FALSE) & (pCom^.OurTxObj <> -1) THEN
            IF (pCom^.reqSN <> 16#ffff) & (pCom^.reqSN - 1 = pHead^.pCode^.uiLng) THEN 
              pHead^.funkSt := READY;
  
#ifndef DUMMY_LOADER
  #ifdef LOADER_4_LASAL2          
              pCom^.Com._m.resu.uilng := 0;
  #else
              pCom^.Com.resu.uilng := 0;
  #endif
              befipr_state := pCom^.Com.BEFIPR(#bNext);
#endif              
              SyncRefLists(pCom);
              pHead^.workstate:=befipr_state;
              pCom^.reqSN := 0;
  
              // check if it is intern then do nothing transmit --> work is done
              IF pCom^.HisTxObj > 1 THEN 
                IF pHead^.workstate <>  ERROR THEN
                  pCom^.nTx := pResu^.uilng$int;
                  pCom^.xTx := 0;
                ELSE 
                   pCom^.nTx:=-1; // send just the status 
                   pCom^.xTx:=0;
                END_IF;
              END_IF;

            ELSE
              // verstümmelten Befehl empfangen -> workstate + reqSN resetten
              pHead^.workstate := READY;
              pCom^.reqSN := 0;
            END_IF;
          END_IF;   // OurHandle
        END_IF;
      END_IF;
    END_IF;
  END_FOR;

  //*************************************************                    
  // chk for new conects CANBUS
  //*************************************************                    
  Check4NewConnects(pCom);

  // Prüfen, ob die Gegenstelle noch aktiv ist
  AliveChecker();
  
  TRACE_ENTRY(9999);

END_FUNCTION //VIRTUAL GLOBAL Comlink::CyWork
// ...................................   
//  prepar list for start 
// ...................................
//
// --> EDX is pointer list
#ifdef _LSL_TARGETARCH_X86

function AWL loadlist
        
         L.CX      AX
         and.ecx   16#FFFF  // weil loop sonst crash    
         or.ECX    ECX
         JNZ       UPDIS1
         CLR       AX
         JMP       UPDI_END // no refresh

UPDIS1
         push       EBX             
         l.ebx      EDX

         L.EDX      OPS.TAbsolute
         SUB.EDX    0000000020*0000000060*0000001000
         
UPDSTL   L.D       (EBX+comStCell.dData),16#80000001 //INVALID
         S.EDX     (EBX+comStCell.udLastUpdate)
         ADD.BX    SIZEOF(comStCell)
         LOOP      UPDSTL
         POP       EBX
UPDI_END
         S.AX      (EDI+ESI)  // write to nstate
         L.W       (EDI+ESI+2), 00000 // nact
         ret
end_function
// Schreibt den Inhalt von EAX, Länge CX auf *EBX
function AWL write2buffer
         CMP.CX    00001
         JNE       CRX2B
         S         (EBX)
         ret
CRX2B
         CMP.CX    00002
         JNE       CRX3B
         S.AX      (EBX)
         ret
CRX3B
         CMP.CX    00003
         JNE       CRX4B
         S.AX      (EBX)
         SHR.EAX   0000000016
         S         (EBX+2)
         ret
CRX4B
         CMP.CX    00004
         JNE       CRXR
         S.EAX     (EBX)
CRXR
         ret

end_function
#else
function loadlist
  var_input
    pcell : ^comStCell;  // edx
    anz : udint;         // ax
  end_var
  VAR
    tix : udint;
  END_VAR
    
  tix := ops.tAbsolute - 20*60*1000; // create extraold timestamp
  while(anz) do
    pcell^.dData$udint := 16#80000001; // invalidate cell
    pcell^.udLastUpdate := tix;
    pcell += sizeof(comStCell);
    anz -= 1;
  end_while;
  
  // auf folgende zeilen nicht vergessen !!!
  // S.AX      (EDI+ESI)  // write to nstate
  // L.W       (EDI+ESI+2), 00000 // nact
    
end_function
#endif

#ifdef COMLINK_PG
// ***************************************************
//   ** in meine eingene Liste laden **
// ***************************************************
//
//
Function AWL loadlist_AWL
VAR_INPUT
 pt_com   : ^COMDATA;
 count    :uint;
 typ      :uint;
END_VAR

    l.edi   pt_com
    l.ax    count

    cmp.w   typ,0
    jne     ll_dynamic

    l.EDX   (EDI + comdata.pastatlist)
    l.esi   comdata.nstates
    cmp.ax  (EDI + comdata.sizeofStatList)
    jbe     llsA
    l.ax    (EDI + comdata.sizeofStatList)
llsA
    call    loadlist
    jmp     loadlist_AWLret


ll_dynamic
    l.EDX   (EDI + comdata.padynlist)
    l.esi   comdata.nstate

// check if max. array is reached
    cmp.ax  (EDI + comdata.sizeofDynList)
    jbe     lldA
    l.ax    (EDI + comdata.sizeofDynList)
lldA
    call    loadlist

loadlist_AWLret
end_function
#endif // COMLINK_PG

// the same like getspecialbyte for STR call
function getspecialbyteSTR
VAR_INPUT
    pVirtual : ^virtualbase; 
END_VAR

VAR_OUTPUT
    retcode   : USINT;
END_VAR
    retcode:=GetdataBufferFlag(pVirtual);
end_function

#ifdef _LSL_TARGETARCH_X86
function getspecialbyte
VAR_INPUT
    eax : udint; 
END_VAR

VAR_OUTPUT
    DL   : USINT;
END_VAR
    dl:=GetdataBufferFlag(EAX$^virtualbase);
end_function
#endif

//    Put data into our cmdbuff
//    
//    
//     offset:0-999 offset in cmdbuff
//            1000 + x .. set updatezell
//               Data: pSVRCH
//               lng : UPdaterate
//
// VERSION 00.00
// this is the IRQ-Function of the CAN 
// no Breakskpoints !!!!

    // 0000 - 0999 = offset into comobj Str command side
    // 1000 - 1999 = offset into comobj Str answer side
    // 2000 - 2999 = put into cyclik refreshlist  1 (this is the static list it is not depending by pict.)
    // 3000 - 3999 = put into dynamik refreshlist 2 (this always depends on pictures)   
    // 4000        = bit 0 =  start/stop statik  
    //               bit 1 =  clear statik
    //               bit 16 = start/stop dynamik
    //               bit 17 = clear dynamik
    // 4001        = numbers of refreshs in statik list 
    // 4002        = numbers of refresh in dynamik list
    // 4003        = Bestätigung des Logins
    // 4004        = Refreshlistengrößen des Servers
    // 4005        = _swruntime
    // 4006        = max.Idle Time
    // 4007        = I am Alive Message
    // 4008        = CAN Bremse Wert serverseitig
    // 5000 - 5999 = changed object from refreshlist cyclik so reflist-2000+5000
    // 6000 - 6999 = changed object from refreshlist cyclik so reflist-3000+6000

#ifdef _LSL_TARGETARCH_X86
FUNCTION AWL       CRXGO

         CLR        ECX
         L.CX       (ESI+comobjstr.ilng)
         L.EAX      (ESI+comobjstr.ddata)
         CLR        EBX

         l.EDX      (EDI + comdata.pNode)
         OR.EDX     EDX
         JZ         CRXGO_NO_ALIVE_CHECK

         // alive check
         L.EBX     OPS.TAbsolute
         S.EBX     (EDX + MonitorNode.lastRcvdTime_ms)
         L.B       (EDX + MonitorNode.isRemotePlcAlive), 1
         CLR        EBX

CRXGO_NO_ALIVE_CHECK
// chk if start update (write to n_status)

         L.BX      (ESI+comobjstr.ioff)

         CMP.BX    5000
         JAE       CRXGO_AE_5000

// ********************************************
//      4000 .. numbers of entry in list
// ********************************************

         CMP.BX    04000
         JB        CRXGO_B_4000
         JNE       CRX00
         push      ESI
            
         l.EDX     (EDI + comdata.pastatlist)
         l.esi     comdata.nstates

// check if max. array is reached
         cmp.ax    (EDI + comdata.sizeofStatList)
         jbe       CRX00B
         l.ax      (EDI + comdata.sizeofStatList)
CRX00B              
         call      loadlist
         
         pop       esi  
         JMP       CRXE



// *********************************************
//     4001 .. numbers of entry in list
// *********************************************
CRX00
         CMP.BX     04001
         JNE        CRX00A
         push       esi
         l.EDX      (EDI + comdata.padynlist)
         l.esi      comdata.nstate
// check if max. array is reached
         cmp.ax     (EDI + comdata.sizeofDynList)
         jbe        CRX00C
         l.ax       (EDI + comdata.sizeofDynList)
CRX00C              
         call       loadlist
         pop        esi 
         JMP        CRXE



// *********************************************
//     4003 .. this is the quit for login
// *********************************************

CRX00A
         CMP.BX     04003
         JNE        CRXO0
         L.W       (EDI+comdata.OurHandle),0  // just quit thats it LOGIN is waiting
         JMP        CRXE


// *********************************************
//     4004 .. enthält die Refreshlistengrößen des Servers
// *********************************************

CRXO0
         CMP.BX     04004
         JNE        CRXO1A
         S.EAX      (EDI + comdata.m4004)
         JMP        CRXE


// *********************************************
//     4005 .. enthält in den unteren 2 Bytes den _swruntime Wert (in 10ms Einheiten)
// *********************************************

CRXO1A
         CMP.BX     04005
         JNE        CRXO1B
         S.EAX      (EDI + comdata.m4005)
         JMP        CRXE


// *********************************************
//     4006 .. max.Idle Time (für Verbindungsüberwachung)
// *********************************************

CRXO1B
         CMP.BX     04006
         JNE        CRXO1C
         S.AX       (EDI + comdata.maxIdlTim_new)
         JMP        CRXE


// *********************************************
//     4007 .. I am Alive Message
// *********************************************

CRXO1C
         CMP.BX     04007
         JNE        CRXO1D
         S.EAX      (EDI + comdata.m4007)
         JMP        CRXE


// *********************************************
//     4008 .. CAN Bremse Wert serverseitig
// *********************************************

CRXO1D
         CMP.BX     04008
         JNE        CRXE
         CMP.AX     16#FFFF
         JNE        CRXO1D1
         L.AX       DFLT_CANBREMSE_SVR
CRXO1D1
         S.AX       (EDI + comdata.canBremseSvr)
         JMP        CRXE


// ***********************************************
// offset < 1000 data for buffer commandbuffer
// ***********************************************

// write to funkstate

CRXGO_B_4000
         O.BX       BX
         jnz        CRX03  // 0 = write to workstate
         CMP.CX     4
         jne        crxerr  // must be 4 byte
// is in the moment 2 byte
//         S.AX      (EDI+comdata.Com.Head.workstate+0)
#ifndef DUMMY_LOADER
         CALL       BaseIp::AWLSetWorkState
#endif         
         jmp        CRXE


// 4 - 999 into the commandbuffer           

CRX03
         cmp.bx     1000
         jae        write2result
         sub.ebx    4 // we work absolut
         
         // Hier wird geprüft, ob die CAN Messages in der richtigen Reihenfolge 
         // eintreffen. Die Offsets der Messages starten mit 0 und sind dann immer 
         // um 4 inkrementiert.
         // pCom^.reqSN enthält den nächsten zu erwartenden Offset. Wenn ein 
         // falscher Offset eintrifft, dann wird pCom^.reqSN auf 16#ffff gesetzt.
         // Wenn ein vollständiger fehlerfreier Befehl eingelangt ist, dann steht  
         // in pCom^.reqSN die Anzahl der empfangenen Daten.
         cmp.bx     (EDI + comdata.reqSN)
         jnz        write2befehl_SNcheckFAILED
         l.dx       (EDI + comdata.reqSN)
         add.dx     cx
         jmp        write2befehl_SNcheckUPD
write2befehl_SNcheckFAILED
         l.dx       16#FFFF
write2befehl_SNcheckUPD
         s.dx       (EDI + comdata.reqSN)
         
//       ADD.EBX    (edi+comdata.Com.Head.pcode) // load the pointer in the buffer
#ifndef DUMMY_LOADER
         CALL       BaseIp::AWLAddpCode
#endif         
         call       write2buffer        // Schreibt den Inhalt von EAX, Länge CX auf *EBX    
         JMP        CRXE


// ***********************************************
// offset < 2000 data for buffer resultbuffer
// ***********************************************

write2result
         cmp.bx     2000
         jae        write2refresh
         sub.ebx    1000    
         
         // Hier wird geprüft, ob die CAN Messages in der richtigen Reihenfolge 
         // eintreffen. Die Offsets der Messages starten mit 0 und sind dann immer 
         // um 4 inkrementiert.
         // pCom^.respSN enthält den nächsten zu erwartenden Offset. Wenn ein 
         // falscher Offset eintrifft, dann wird pCom^.respSN auf 16#ffff gesetzt.
         // Wenn eine vollständige fehlerfreie Antwort eingelangt ist, dann steht  
         // in pCom^.respSN die Anzahl der empfangenen Daten.
         cmp.bx     (EDI + comdata.respSN)
         jnz        write2result_SNcheckFAILED
         l.dx       (EDI + comdata.respSN)
         add.dx     cx
         jmp        write2result_SNcheckUPD
write2result_SNcheckFAILED
         l.dx       16#FFFF
write2result_SNcheckUPD
         s.dx       (EDI + comdata.respSN)
         
//       add.ebx    (edi+comdata.Com.Head.presu) // load the pointer in the buffer
#ifndef DUMMY_LOADER
         CALL       BaseIp::AWLAddpResu
#endif         
         call       write2buffer        // Schreibt den Inhalt von EAX, Länge CX auf *EBX
         jmp        crxe

// ***********************************************
// offset < 3000 data for buffer resultbuffer
// ***********************************************


write2refresh
        cmp.bx      3000
        jae         write2dyn

        
        sub.bx      2000
        cmp.bx      (EDI + comdata.sizeofStatList)
        jae         CRXE            // safety !!
        
        IMUL.BX   BX      , SIZEOF(comStCell)
        ADD.EBX   (EDI + comdata.pastatlist)

        tst.cx      16#4000             // bit 14: 1=globale var.
        jz          write2refA
        o.ecx       16#40000000
        jmp         write2refA2
write2refA

        // test if it is a string
        //anm pr: die info dass es sich um einen string handelt, wird auch vom 
        //client mitgeschickt. wieso man das hier noch einmal prüft und die info 
        //vom client ignoriert, ist mir nicht bekannt. es funktioniert aber auch so.
        push        eax
        push        ecx
        push        ebx
        push        edi
        call        getspecialbyte
        and.al      16#01               // mask the bit
        pop         edi
        pop         ebx
        pop         ecx
        pop         eax

        o.dl        dl
        jz          write2refA2
        o.ecx       16#80000000

write2refA2        
        and.cx      16#3fff             // bit 15 ist kennung von oben (vbi flag)
        S.EAX     (EBX+comStCell.pCh)       
        S.ECX     (EBX+comStCell.udUpdaterate)
        L.D       (EBX+comStCell.dData),16#80000001 // INVALID
        L.EAX     OPS.TAbsolute
        S.EAX     (EBX+comStCell.udLastUpdate)
        JMP       CRXE

// ***********************************************
// offset < 4000 data for buffer resultbuffer
// ***********************************************

write2dyn
        cmp.bx      4000
        jae         WriteUpdReflistStat

        sub.bx      3000
        cmp.bx      (EDI + comdata.sizeofDynList)
        jae         CRXE            // safety !!

        IMUL.BX     BX      , SIZEOF(comStCell)
        ADD.EBX     (EDI + comdata.padynlist)

        tst.cx      16#4000             // bit 14: 1=globale var.
        jz          write2dynA
        o.ecx       16#40000000
        jmp         write2dynA2
write2dynA

// test if it is a string
        //anm pr: die info dass es sich um einen string handelt, wird auch vom 
        //client mitgeschickt. wieso man das hier noch einmal prüft und die info 
        //vom client ignoriert, ist mir nicht bekannt. es funktioniert aber auch so.
        push        eax
        push        ecx
        push        ebx
        push        edi
        call        getspecialbyte
        and.al      16#01               // mask the bit
        pop         edi
        pop         ebx
        pop         ecx
        pop         eax

        o.dl        dl
        jz          write2dynA2
        o.ecx       16#80000000

write2dynA2        
        and.cx      16#3fff             // bit 15 ist kennung von oben (vbi flag)
        S.EAX       (EBX+comStCell.pCh)
        S.ECX       (EBX+comStCell.udUpdaterate)
        L.D         (EBX+comStCell.dData),16#80000001 // INVALID
        L.EAX       OPS.TAbsolute
        S.EAX       (EBX+comStCell.udLastUpdate)
        JMP         CRXE

//***************************************
//  put data into refreshlist cyclik
//***************************************

CRXGO_AE_5000
WriteUpdReflistStat
        cmp.bx      6000
        jae         WriteUpdReflistDyn

        sub.Ebx     5000

        push        ebx
        IMUL.EBX    EBX      , SIZEOF(comStCell)
        ADD.EBX     (EDI + comdata.padynlist)
        S.EAX       (EBX+comStCell.dData)
        l.ecx       (EBX+comStCell.VarlistID)
        pop         ebx
        call        callback
        JMP         CRXE

//***************************************
//  put data into refreshlist dynamik
//***************************************

WriteUpdReflistDyn
        cmp.bx      7000
        jae         CRXERR

        push        ebx
        sub.EBX     6000
        IMUL.EBX    EBX      , SIZEOF(comStCell)
        ADD.EBX     (EDI + comdata.padynlist)

        S.EAX       (EBX+comStCell.dData)
        l.ecx       (EBX+comStCell.VarlistID)
        pop         ebx
        sub.ebx     5000
        call        callback
        JMP         CRXE

CRXERR
CRXE
         RET
END_FUNCTION

FUNCTION AWL CRX
VAR_INPUT
    piCanptr        : ^SINT;
    rxDataptr       : ^sint;
    thisp       : Udint;
END_VAR
         L.ESI    rxDataptr 
         L.EDI    thisp
         CALL       CRXGO
END_FUNCTION //VIRTUAL GLOBAL Comlink::CyWork

#else

FUNCTION CRX
VAR_INPUT
  piCanptr        : ^SINT;
  rxDataptr       : ^sint;
  thisp       : udint;
END_VAR
VAR
  pedi : ^comData;
  pesi : ^comObjStr;
  
  pcell : ^comStCell;
  d2lse : d2LSE;
  
  veax : udint;
  vebx : udint;
  vecx : udint;
  vedx : udint;
END_VAR

  pesi := rxDataptr$^comObjStr;
  pedi := thisp$^comData;

  // CLR        ECX
  // L.CX       (ESI+comobjstr.ilng)
  // L.EAX      (ESI+comobjstr.ddata)
  // CLR        EBX
  vecx := to_udint(pesi^.ilng);
  veax := pesi^.ddata$udint;
  vebx := 0;

  // l.EDX      (EDI + comdata.pNode)
  // OR.EDX     EDX
  // JZ         CRXGO_NO_ALIVE_CHECK
  if(pedi^.pNode <> nil) then
    // alive check
    // L.EBX     OPS.TAbsolute
    // S.EBX     (EDX + MonitorNode.lastRcvdTime_ms)
    pedi^.pNode^.lastRcvdTime_ms := ops.tAbsolute;
    // L.B       (EDX + MonitorNode.isRemotePlcAlive), 1
    pedi^.pNode^.isRemotePlcAlive := 1;
    // CLR        EBX
    // CRXGO_NO_ALIVE_CHECK
  end_if;

  // chk if start update (write to n_status)

  // L.BX      (ESI+comobjstr.ioff)
  // CMP.BX    5000
  // JAE       CRXGO_AE_5000
  if(pesi^.ioff < 5000) then

    // CMP.BX    04000
    // JB        CRXGO_B_4000
    if(pesi^.iOff >= 4000) then

      // JNE       CRX00
      if(pesi^.ioff = 4000) then
        // ********************************************
        //      4000 .. numbers of entry in list
        // ********************************************
        // l.EDX     (EDI + comdata.pastatlist)
        // l.esi     comdata.nstates
        // check if max. array is reached
        // cmp.ax    (EDI + comdata.sizeofStatList)
        // jbe       CRX00B
        if(veax > pedi^.sizeofStatList) then
          // l.ax      (EDI + comdata.sizeofStatList)
          veax := pedi^.sizeofStatList;
          // CRX00B              
        end_if;
        
        // call      loadlist
        loadlist(pedi^.paStatList, veax);
        pedi^.nStateS := veax$uint;
        pedi^.nActS   := 0;

        //JMP       CRXE
        return;
      elsif(pesi^.iOff = 4001) then
        // *********************************************
        //     4001 .. numbers of entry in list
        // *********************************************
        // l.EDX      (EDI + comdata.padynlist)
        // l.esi      comdata.nstate
        // check if max. array is reached
        // cmp.ax     (EDI + comdata.sizeofDynList)
        // jbe        CRX00C
        if(veax > pedi^.sizeofDynList) then
          // l.ax       (EDI + comdata.sizeofDynList)
          veax := pedi^.sizeofDynList;
          // CRX00C
        end_if;
       
        // call       loadlist
        loadlist(pedi^.paDynList, veax);
        pedi^.nState := veax$uint;
        pedi^.nAct   := 0;
           
        // JMP        CRXE
        return;
      elsif(pesi^.iOff = 4003) then
        // *********************************************
        //     4003 .. this is the quit for login
        // *********************************************
        // L.W       (EDI+comdata.OurHandle),0  // just quit thats it LOGIN is waiting
        pedi^.OurHandle := 0;
        // JMP        CRXE
        return;
      elsif(pesi^.iOff = 4004) then
        // *********************************************
        //     4004 .. enthält die Refreshlistengrößen des Servers
        // *********************************************
        // S.EAX      (EDI + comdata.m4004)
        pedi^.m4004 := veax;
        // JMP        CRXE
        return;
      elsif(pesi^.iOff = 4005) then
        // *********************************************
        //     4005 .. enthält in den unteren 2 Bytes den _swruntime Wert (in 10ms Einheiten)
        // *********************************************
        // S.EAX      (EDI + comdata.m4005)
        pedi^.m4005 := veax;
        // JMP        CRXE
        return;
      elsif(pesi^.iOff = 4006) then
        // *********************************************
        //     4006 .. max.Idle Time (für Verbindungsüberwachung)
        // *********************************************
        // S.AX       (EDI + comdata.maxIdlTim_new)
        pedi^.maxIdlTim_new := veax$uint;
        // JMP        CRXE
        return;
      elsif(pesi^.iOff = 4007) then
        // *********************************************
        //     4007 .. I am Alive Message
        // *********************************************
        // S.EAX      (EDI + comdata.m4007)
        pedi^.m4007 := veax;
        // JMP        CRXE
        return;
      elsif(pesi^.iOff = 4008) then
        // *********************************************
        //     4008 .. CAN Bremse Wert serverseitig
        // *********************************************
        // CMP.AX     16#FFFF
        // JNE        CRXO1D1
        if(veax = 16#FFFF) then
          // L.AX       DFLT_CANBREMSE_SVR
          veax := DFLT_CANBREMSE_SVR;
          //CRXO1D1
        end_if;
        // S.AX       (EDI + comdata.canBremseSvr)
        pedi^.canBremseSvr := veax$uint;
        // JMP        CRXE
        return;
      end_if;
    end_if;
    
    // CRXGO_B_4000
    // write to funkstate
    // ***********************************************
    // offset < 1000 data for buffer commandbuffer
    // ***********************************************
    
    // O.BX       BX
    // jnz        CRX03  // 0 = write to workstate
    if(pesi^.iOff = 0) then
      // CMP.CX     4
      if(vecx <> 4) then
        //jne        crxerr  // must be 4 byte
        return; // error, must be 4 byte
      end_if;
     #ifndef DUMMY_LOADER
      //CALL       BaseIp::AWLSetWorkState
      pedi^.Com._m.head.workstate := veax$iprStates;
     #endif         
      // jmp        CRXE
      return;
    end_if;

    // CRX03
    // cmp.bx     1000
    // jae        write2result
    if(pesi^.iOff < 1000) then
      // 4 - 999 into the commandbuffer           
      // sub.ebx    4 // we work absolut
      vebx := to_udint(pesi^.iOff) - 4;
      
      // Hier wird geprüft, ob die CAN Messages in der richtigen Reihenfolge 
      // eintreffen. Die Offsets der Messages starten mit 0 und sind dann immer 
      // um 4 inkrementiert.
      // pCom^.reqSN enthält den nächsten zu erwartenden Offset. Wenn ein 
      // falscher Offset eintrifft, dann wird pCom^.reqSN auf 16#ffff gesetzt.
      // Wenn ein vollständiger fehlerfreier Befehl eingelangt ist, dann steht  
      // in pCom^.reqSN die Anzahl der empfangenen Daten.
      
      // cmp.bx     (EDI + comdata.reqSN)
      // jnz        write2befehl_SNcheckFAILED
      vedx := 16#FFFF;
      if(vebx = pedi^.reqSN) then
        // l.dx       (EDI + comdata.reqSN)
        // add.dx     cx
        vedx := pedi^.reqSN + vecx;
        // jmp        write2befehl_SNcheckUPD
        // write2befehl_SNcheckFAILED
      end_if;
      
      // write2befehl_SNcheckUPD
      // s.dx       (EDI + comdata.reqSN)
      pedi^.reqSN := vedx$uint; 
         
     #ifndef DUMMY_LOADER
      // CALL       BaseIp::AWLAddpCode
      vebx += pedi^.Com._m.head.pCode$udint;
     #endif
  
      //call       write2buffer        // Schreibt den Inhalt von EAX, Länge CX auf *EBX
      _memcpy(vebx$^void, #veax, vecx);
      // JMP        CRXE
      return;
      
    // write2result
    // cmp.bx     2000
    // jae        write2refresh
    elsif(pesi^.iOff < 2000) then
      // ***********************************************
      // offset < 2000 data for buffer resultbuffer
      // ***********************************************

      // sub.ebx    1000    
      vebx := to_udint(pesi^.iOff) - 1000;
      
      // Hier wird geprüft, ob die CAN Messages in der richtigen Reihenfolge 
      // eintreffen. Die Offsets der Messages starten mit 0 und sind dann immer 
      // um 4 inkrementiert.
      // pCom^.respSN enthält den nächsten zu erwartenden Offset. Wenn ein 
      // falscher Offset eintrifft, dann wird pCom^.respSN auf 16#ffff gesetzt.
      // Wenn eine vollständige fehlerfreie Antwort eingelangt ist, dann steht  
      // in pCom^.respSN die Anzahl der empfangenen Daten.
      
      // cmp.bx     (EDI + comdata.respSN)
      // jnz        write2result_SNcheckFAILED
      vedx := 16#FFFF;
      if(vebx = pedi^.respSN) then
        // l.dx       (EDI + comdata.respSN)
        // add.dx     cx
        vedx := pedi^.respSN + vecx;
        // jmp        write2result_SNcheckUPD
        // write2result_SNcheckFAILED
        // l.dx       16#FFFF
      end_if;
      
      // write2result_SNcheckUPD
      // s.dx       (EDI + comdata.respSN)
      pedi^.respSN := vedx$uint;
         
     #ifndef DUMMY_LOADER
      // CALL       BaseIp::AWLAddpResu
      vebx += pedi^.Com._m.head.pResu$udint;
     #endif         
      // call       write2buffer        // Schreibt den Inhalt von EAX, Länge CX auf *EBX
      _memcpy(vebx$^void, #veax, vecx);
      // jmp        crxe
      return;
      
    // write2refresh
    // cmp.bx      3000
    // jae         write2dyn
    elsif(pesi^.iOff < 3000) then
      // ***********************************************
      // offset < 3000 data for buffer resultbuffer
      // ***********************************************
        
      // sub.bx      2000
      vebx := to_udint(pesi^.iOff) - 2000;
      
      // cmp.bx      (EDI + comdata.sizeofStatList)
      // jae         CRXE            // safety !!
      if(vebx >= pedi^.sizeofStatList) then
        return;
      end_if;
      
      // IMUL.BX   BX      , SIZEOF(comStCell)
      // ADD.EBX   (EDI + comdata.pastatlist)
      pcell := pedi^.paStatList + vebx * sizeof(comStCell);

      // tst.cx      16#4000             // bit 14: 1=globale var.
      // jz          write2refA
      if(vecx and 16#4000) then
        // o.ecx       16#40000000
        vecx := vecx or 16#40000000;
        // jmp         write2refA2
      else
        // write2refA
        // test if it is a string
        //anm pr: die info dass es sich um einen string handelt, wird auch vom 
        //client mitgeschickt. wieso man das hier noch einmal prüft und die info 
        //vom client ignoriert, ist mir nicht bekannt. es funktioniert aber auch so.
        
        // push        eax
        // push        ecx
        // push        ebx
        // push        edi
        // call        getspecialbyte
        // and.al      16#01               // mask the bit
        // pop         edi
        // pop         ebx
        // pop         ecx
        // pop         eax

        // o.dl        dl
        // jz          write2refA2
        // o.ecx       16#80000000
        if(getspecialbyteSTR(veax$^VirtualBase)) then
          vecx := vecx or 16#80000000;
        end_if; 
      // write2refA2        
      end_if;
      
      // and.cx      16#3fff             // bit 15 ist kennung von oben (vbi flag)
      vecx := vecx and 16#FFFF3FFF;
      // S.EAX     (EBX+comStCell.pCh)       
      pcell^.pCh := veax$^SvrCh;
      // S.ECX     (EBX+comStCell.udUpdaterate)
      pcell^.udUpdaterate := vecx;
      // L.D       (EBX+comStCell.dData),16#80000001 // INVALID
      pcell^.dData$udint := 16#80000001; // INVALID
      // L.EAX     OPS.TAbsolute
      // S.EAX     (EBX+comStCell.udLastUpdate)
      pcell^.udLastUpdate := ops.tAbsolute;
      // JMP       CRXE
      return;

    // write2dyn
    // cmp.bx      4000
    // jae         WriteUpdReflistStat
    elsif(pesi^.iOff < 4000) then
      // ***********************************************
      // offset < 4000 data for buffer resultbuffer
      // ***********************************************

      // sub.bx      3000
      vebx := to_udint(pesi^.iOff) - 3000;
      
      // cmp.bx      (EDI + comdata.sizeofDynList)
      // jae         CRXE            // safety !!
      if(vebx >= pedi^.sizeofDynList) then
        return;
      end_if;

      // IMUL.BX     BX      , SIZEOF(comStCell)
      // ADD.EBX     (EDI + comdata.padynlist)
      pcell := pedi^.paDynList + vebx * sizeof(comStCell);
      // tst.cx      16#4000             // bit 14: 1=globale var.
      // jz          write2dynA
      if(vecx and 16#4000) then
        // o.ecx       16#40000000
        vecx := vecx or 16#40000000;
        // jmp         write2dynA2
      else

        // write2dynA
        // test if it is a string
        //anm pr: die info dass es sich um einen string handelt, wird auch vom 
        //client mitgeschickt. wieso man das hier noch einmal prüft und die info 
        //vom client ignoriert, ist mir nicht bekannt. es funktioniert aber auch so.
        
        // push        eax
        // push        ecx
        // push        ebx
        // push        edi
        // call        getspecialbyte
        // and.al      16#01               // mask the bit
        // pop         edi
        // pop         ebx
        // pop         ecx
        // pop         eax

        // o.dl        dl
        // jz          write2dynA2
        // o.ecx       16#80000000
        if(getspecialbyteSTR(veax$^VirtualBase)) then
          vecx := vecx or 16#80000000;
        end_if; 
      end_if;

      // write2dynA2        
      // and.cx      16#3fff             // bit 15 ist kennung von oben (vbi flag)
      vecx := vecx and 16#FFFF3FFF;
      // S.EAX       (EBX+comStCell.pCh)
      pcell^.pCh := veax$^SvrCh;  
      // S.ECX       (EBX+comStCell.udUpdaterate)
      pcell^.udUpdaterate := vecx;
      // L.D         (EBX+comStCell.dData),16#80000001 // INVALID
      pcell^.dData$udint := 16#80000001; // INVALID
      // L.EAX       OPS.TAbsolute
      // S.EAX       (EBX+comStCell.udLastUpdate)
      pcell^.udLastUpdate := ops.tAbsolute;
      // JMP         CRXE
      return;
      
    end_if;
  // CRXGO_AE_5000
  end_if;
  
  // WriteUpdReflistStat
  // cmp.bx      6000
  // jae         WriteUpdReflistDyn
  if(pesi^.iOff < 6000) then  
    //***************************************
    //  put data into refreshlist cyclik
    //***************************************

    // sub.Ebx     5000
    vebx  := to_udint(pesi^.iOff) - 5000;
    // push        ebx
    // IMUL.EBX    EBX      , SIZEOF(comStCell)
    // ADD.EBX     (EDI + comdata.padynlist)
    pcell := pedi^.paDynList + vebx * sizeof(comStCell);
    // S.EAX       (EBX+comStCell.dData)
    pcell^.dData := veax$dint;

    // l.ecx       (EBX+comStCell.VarlistID)
    // pop         ebx
    d2lse.uioffs    := vebx$uint;
    d2lse.data      := pcell^.dData;
    d2lse.varlistid := pcell^.VarListID;

    // call        callback
    callback(pedi, #d2lse);
        
    // JMP         CRXE
    return;

  // WriteUpdReflistDyn
  // cmp.bx      7000
  // jae         CRXERR
  elsif(pesi^.iOff < 7000) then
    //***************************************
    //  put data into refreshlist dynamik
    //***************************************

    // push        ebx
    // sub.EBX     6000
    vebx  := to_udint(pesi^.iOff) - 6000;
        
    // IMUL.EBX    EBX      , SIZEOF(comStCell)
    // ADD.EBX     (EDI + comdata.padynlist)
    pcell := pedi^.paDynList + vebx * sizeof(comStCell);

    // S.EAX       (EBX+comStCell.dData)
    pcell^.dData := veax$dint;
    
    // l.ecx       (EBX+comStCell.VarlistID)
    // pop         ebx
    // sub.ebx     5000
    d2lse.uioffs    := 1000 + vebx$uint;
    d2lse.data      := pcell^.dData;
    d2lse.varlistid := pcell^.VarListID;
        
    // call        callback
    callback(pedi, #d2lse);
        
    // JMP         CRXE
    return;
  end_if;

// CRXERR
// CRXE
// RET

END_FUNCTION //VIRTUAL GLOBAL Comlink::CyWork
#endif
// Prüft, ob die Refreshlisten-Zeiger auf NIL stehen. Wenn ja, dann wird die 
// Refreshliste allokiert und die Refreshlisten-Zeiger auf die allokierte Liste 
// gesetzt.
FUNCTION CheckRefreshlist
VAR_INPUT
  ptc : ^COMDATA;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR

  retVal := 0;

  IF ptc^.paDynList = NIL THEN
    DEBUGLOG0("%010d-CheckRefreshlist: allocating dynlist");
    ptc^.paDynList := _alloc(ptc^.sizeofDynList)$^COMSTCELL;
    IF ptc^.paDynList = NIL THEN
      retVal := -1;
    END_IF;
  END_IF;

  IF ptc^.paStatList = NIL THEN
    DEBUGLOG0("%010d-CheckRefreshlist: allocating statlist");
    ptc^.paStatList := _alloc(ptc^.sizeofStatList)$^COMSTCELL;
    IF ptc^.paStatList = NIL THEN
      retVal := -1;
    END_IF;
  END_IF;

END_FUNCTION


FUNCTION INI_ONE_COM_IF
VAR_INPUT
  Number    :INT;
END_VAR
VAR_OUTPUT
  pCOMDATA  : ^COMDATA;  
END_VAR
VAR
  ptc       : ^COMDATA;
  comdataSize : UDINT;
END_VAR

    pCOMDATA := NIL;

// ********************************
//   login_anz is normaly on 1 
// ********************************
// 
    if (Number >= arraysize(myComChs)) then
      TRACE_ERR("LOGIN: No more Array for LOGIN Data");
      LSLCLI_LoaderTracePrint("LOGIN: No more Array for LOGIN Data", 12);
      Return;
    end_if;

    if myComChs[Number] = 0 then 
      if (Number < arraysize(OPS.comchs)) then
        comdataSize := SIZEOF(comdata);
      else
        comdataSize := comData.aDynList;
      end_if;
      DEBUGLOG2("%010d-INI_ONE_COM_IF: allocating %d bytes for ch#%d", TO_UDINT(comdataSize), TO_UDINT(Number));
      ptc :=_alloc(comdataSize)$^COMData;
    else
      ptc := myComChs[Number];
    end_if;

    if ptc <> NIL then
      _MEMSET(ptc,0,comdataSize);
      myComChs[Number]:=ptc;
      if (Number < arraysize(OPS.comchs)) then
        OPS.comchs[Number]:=ptc;
      end_if;
      ptc^.HisTxObj:=-1;
      ptc^.hishandle:=-1$UINT;
      ptc^.OurTxObj:=-1;
      ptc^.ourhandle:=-1$UINT;
      ptc^.nTx:=0;
#ifndef DUMMY_LOADER
      ptc^.Com.InitCode(FALSE);
#endif     
      ptc^.pcallback:=#PrototypeCallback();
      ptc^.hComchMutex := NewComchMutex(TO_UDINT(Number), 'c');
      ptc$^ComData_Base^.typeOfStruct := STRUCT_TYPE_COMDATA;
      ptc^.pDynCallbacks := NIL;

      // 1.1.31, 2.2.31 (Feb.2006):
      // Dynamisches allokieren der Refreshliste bei Bedarf.
      //
      // Es wird nur bei den ersten 6 comdata Elementen der Refreshlisten-Zeiger 
      // auf das Refreshlisten-Array in der comdata Struktur gesetzt. Bei den restlichen 
      // comdata Elementen wird der Refreshlisten-Zeiger auf NIL gesetzt und dynamisch 
      // allokiert, denn hier wurde die comdata Struktur ohne Refreshlisten-Array 
      // angelegt.
      //
      // Das Refreshlisten-Array kann bei der ersten 6 Einträgen nicht durch einen dynamisch 
      // allokierten Speicher ersetzt werden, weil Anwendungen existieren, die über 
      // OPS.comchs (Pointerleiste mit 6 Einträgen) direkt auf die comdata Struktur 
      // zugreifen und fix davon ausgehen, daß das Refreshlisten-Array in der comdata 
      // Struktur existiert. 
      //
      // Bei den restlichen comdata Elementen kann man die Refreshliste dynamsich allokieren. 
      // Man muß nur vor der Benutzung der Refreshlisten-Zeiger prüfen ob diese NIL sind. 
      // wenn ja, dann wird die Refreshliste allokiert und der Refreshlisten-Zeiger darauf 
      // gesetzt.
      // 
      if (Number < arraysize(OPS.comchs)) then
        ptc^.paDynList  := #ptc^.aDynList[0];
        ptc^.paStatList := #ptc^.aStatList[0];

        ptc^.sizeofDynList  := arraysize(ptc^.aDynList);
        ptc^.sizeofStatList := arraysize(ptc^.aStatList);
      else
        // Obwohl die Refreshlisten-Zeiger auf NIL stehen, wird das sizeof Element auf 
        // einen Wert <> 0 gesetzt, da hier nicht die allokierte Größe sondern die 
        // mögliche Größe angegeben wird.
        // Die Größe wird nämlich gleich am Anfang der anderen Station mitgeteilt. 
        // Zu diesem Zeitpunkt muß die Refreshliste aber noch nicht allokiert worden sein.
        ptc^.sizeofDynList  := SIZEOF(COMSTCELL) * maxDynCOMcell;
        ptc^.sizeofStatList := SIZEOF(COMSTCELL) * maxStatCOMcell;
      end_if;

      // Die Refreshlisten-Größen der Remote Station werden vorerst mit der Größe der eigenen 
      // initialisiert. Wenn beim Login ein anderer Wert gemeldet wird, dann wird diese 
      // Variable noch einmal überschrieben
      ptc^.sizeofRemoteDynList  := ptc^.sizeofDynList;
      ptc^.sizeofRemoteStatList := ptc^.sizeofStatList;

      ptc^.canBremseSvr := DFLT_CANBREMSE_SVR;

      pCOMDATA := ptc;
    else
      TRACE_ERR("No More Memory for LOGIN Data (Online --> COMDATA)");
      LSLCLI_LoaderTracePrint("No More Memory for LOGIN Data (Online --> COMDATA)", 12);
    end_if;

END_FUNCTION


FUNCTION FREE_ONE_COM_IF
VAR_INPUT
  Number  :INT;
END_VAR
VAR
  ptc     : ^COMDATA;
  pDynCallback      : ^sDynCallbackList;
  pDynCallbackNext  : ^sDynCallbackList;
END_VAR;

	ptc := myComChs[Number];

	IF ptc <> NIL THEN

	    DEBUGLOG1("%010d-FREE_ONE_COM_IF: freeing comdata for ch#%d", TO_UDINT(Number));

		IF ptc^.paDynList <> NIL & ptc^.paDynList <> #ptc^.aDynList[0] THEN
			_free(ptc^.paDynList);
			ptc^.paDynList := NIL;
      g_nbrDynList -= 1;
		END_IF;

		IF ptc^.paStatList <> NIL & ptc^.paStatList <> #ptc^.aStatList[0] THEN
			_free(ptc^.paStatList);
			ptc^.paStatList := NIL;
      g_nbrStatList -= 1;
		END_IF;

    IF ptc^.hComchMutex THEN
      DeleteComchMutex(#ptc^.hComchMutex);
    END_IF;
     
    IF (ptc^.pDynCallbacks <> NIL) THEN
    
      pDynCallback := ptc^.pDynCallbacks;
      pDynCallbackNext := pDynCallback^.pNext;
      
      // ersten callback freigeben
     _free(pDynCallback^.pCallback);
     _free(pDynCallback);

     // restliche freigeben     
     WHILE (pDynCallbackNext <> NIL) DO
         pDynCallback := pDynCallbackNext;
         pDynCallbackNext := pDynCallback^.pNext;
         _free(pDynCallback^.pCallback);
         _free(pDynCallback);
     END_WHILE;
    
      ptc^.pDynCallbacks := NIL;
    END_IF;

#ifndef DUMMY_LOADER
		ptc^.Com.dtor();
		_free(ptc);
    ptc := NIL;
#endif    

		myComChs[Number] := NIL;
		IF (Number < arraysize(OPS.comchs)) THEN
    		OPS.comchs[Number] := NIL;
		END_IF;

	END_IF;

END_FUNCTION
//    
//    Initialise comunication interface
//VERSION 00.00

FUNCTION INI_COM_IF

    INI_ONE_COM_IF(0);  

// *******************************************************************//
// the first one is for internal use and is always automatic reserved //
// *******************************************************************//

    myComChs[0]^.HisTxObj :=0;

END_FUNCTION


FUNCTION GLOBAL __CDECL LOGOUT
VAR_INPUT
  pComdef : ^COMDEF;
END_VAR
VAR_OUTPUT
  result : UINT;
END_VAR
VAR
  i : INT;
  canNrClt : USINT;
  nbrFound : INT;
END_VAR

  DEBUGLOG1( "%010d-LOGOUT: pComdef=0x%X", pComdef$UDINT );

  result := 0;

  IF (pComdef = NIL ) THEN
    RETURN;
  END_IF;

  StartStopRefresh( pComdef, 0, 1 );
  IF (pComdef^.pt_com <> NIL) THEN
    IF ((pComdef^.pt_com^.paDynList <> NIL) &
       (pComdef^.pt_com^.paDynList <> #pComDef^.pt_com^.aDynList[0])) THEN
      _free( pComdef^.pt_com^.paDynList );
      pComdef^.pt_com^.paDynList := NIL;
      g_nbrDynList -= 1;
    END_IF;
    IF ((pComdef^.pt_com^.paStatList <> NIL) &
       (pComdef^.pt_com^.paStatList <> #pComDef^.pt_com^.aStatList[0])) THEN
      _free( pComdef^.pt_com^.paStatList );
      pComdef^.pt_com^.paStatList := NIL;
      g_nbrStatList -= 1;
    END_IF;
  END_IF;

  CASE pComdef^.interface OF

  // ******************
  //     lokal
  // ******************
  COMLINK_IFNUM_LOCAL,
  COMLINK_IFNUM_INTERN:

      // search for the ComCh
      FOR i := 0 to login_anz - 1 do
        IF (pComdef^.pt_COM = myComChs[i]) THEN
          pComdef^.pt_COM^.pComdef := NIL;
          DEBUGLOG1("%010d-LOGOUT: ComCh #%d for LOCAL", to_udint(i));
        END_IF;
      END_FOR;

      // Anm praerw 16.3.2015:
      // Da die vorherige Schleife immer bis zum Endwert abgearbeitet wird, 
      // hat i immer den Wert login_anz. Beim folgenden Code wird also 
      // immer der Zweig mit 'ELSIF (i = login_anz) THEN' abgearbeitet. 
      // Ich habe hier aber nichts ausgebessert, weil ich die Auswirkung 
      // einer Änderung nicht abschätzen kann.
      
      //no additional ComCh has been allocated
      IF (i = 0) THEN
        RETURN; 
      //no ComCh found
      ELSIF (i = login_anz) THEN
        result := 16#ffff;
        DEBUGLOG0( "%010d-LOGOUT: no ComCh LOCAL" );
        RETURN;
      ELSE
        FREE_ONE_COM_IF(i);
        RETURN; 
      END_IF;

  // ******************
  //     CANBUS CAN1,2 und CAN20..CAN29
  // ******************
  COMLINK_IFNUM_CAN1,
  COMLINK_IFNUM_CAN2,
  COMLINK_IFNUM_CAN20,
  COMLINK_IFNUM_CAN21,
  COMLINK_IFNUM_CAN22,
  COMLINK_IFNUM_CAN23,
  COMLINK_IFNUM_CAN24,
  COMLINK_IFNUM_CAN25,
  COMLINK_IFNUM_CAN26,
  COMLINK_IFNUM_CAN27,
  COMLINK_IFNUM_CAN28,
  COMLINK_IFNUM_CAN29:
      IF pComdef^.interface < COMLINK_IFNUM_CAN20 THEN
        canNrClt := pComdef^.interface$USINT - COMLINK_IFNUM_CAN1 + 1;
      ELSE
        canNrClt := pComdef^.interface$USINT - COMLINK_IFNUM_CAN20 + 20;
      END_IF;
      nbrFound := -1;
      FOR i := 1 to login_anz - 1 do
        IF ((myComChs[i] <> NIL) & 
           (myComChs[i]^.HisHandle = pComdef^.adress) &
           (myComChs[i]^.pComdef = pComdef)) THEN
          nbrFound := i;
          pComdef^.pt_COM^.pComdef := NIL;
          myComChs[i]^.HisHandle := -1$UINT;
          DEBUGLOG1("%010d-LOGOUT: ComCh #%d for CAN", to_udint(i));
        END_IF;
      END_FOR; 

      IF (nbrFound = -1) THEN
        result := 16#ffff;
        DEBUGLOG0( "%010d-LOGOUT: no ComCh CAN" );
        RETURN;
      ELSE
        CANUSER_DelCanObj(canNrClt, pComdef^.pt_COM^.CanHandle);
        FREE_ONE_COM_IF(nbrFound);
      END_IF;

  // ******************
  //       TCPIP 1,2
  // ******************
  COMLINK_IFNUM_TCP1_OLD,
  COMLINK_IFNUM_TCP1,
  COMLINK_IFNUM_TCP2:
      #ifdef COMLINK_TCP_CLIENT  
      ComlinkT_Logout(pComdef);
      #endif

      COMLINK_IFNUM_COM1,
      COMLINK_IFNUM_COM2,
      COMLINK_IFNUM_COM3,
      COMLINK_IFNUM_COM4,
      COMLINK_IFNUM_COM5,
      COMLINK_IFNUM_COM6,
      COMLINK_IFNUM_COM7,
      COMLINK_IFNUM_COM8,
      COMLINK_IFNUM_COM9,
      COMLINK_IFNUM_COM10:
      result := ComlinkR_Logout(pComdef);

  ELSE
      result := COMLINK_ERR_UNSUPPORTED_INTERFACE;

  END_CASE;

END_FUNCTION


FUNCTION GLOBAL __CDECL LOGIN
VAR_INPUT
  pComdef  :^COMDEF; 
END_VAR
VAR_OUTPUT
  result   :UINT;
END_VAR
VAR
  pCom          : ^COMDATA;
 #ifdef COMLINK_PG
  buffer        : UDINT;
  canresu       : DINT;
 #else
  pirq          : ^VOID;
  WaitForResult : DINT;
  StartCanAdr   : UINT;
  MyCanStation  : UINT;
  txinfo4005    : COMOBJSTR;
 #endif 
  i             : INT;  
  comChIdx      : INT; 
  selectedCanChNbr : UINT; 
  bReuse        : BOOL;
  canNrClt      : USINT;
END_VAR

  CASE pComdef^.interface OF

  // ******************
  //     lokal
  // ******************
  COMLINK_IFNUM_LOCAL,
  COMLINK_IFNUM_INTERN:
      pCom := myComChs[0];
      // check first if already the first is    
      IF pCom <> NIL & pCom^.pComdef = NIL THEN   
        // it is the first lokal
        pComdef^.pt_COM := myComChs[0];
        myComChs[0]^.Pcomdef := pComdef;
        pCom := pComdef^.pt_COM;

        DEBUGLOG0("%010d-LOGIN: using preallocated ComCh #0 for LOCAL");

        // stop refresh 
        pCom^.nStateS := 0;
        pCom^.nActS := 0;
        pCom^.nState := 0;              
        pCom^.nAct := 0; 
        result := 0;
      ELSE
        // this is new and can happen if visu screen internal + visu small diplay
        // we search for a already done login (call 2 times)
        // if this happens we do nothing
        FOR i := 0 TO login_anz - 1 DO
          IF myComChs[i] <> NIL THEN
            IF pComdef^.pt_COM <> NIL THEN
              IF myComChs[i]^.Pcomdef = pComdef THEN
                DEBUGLOG1("%010d-LOGIN: re-using ComCh #%d for LOCAL", TO_UDINT(login_anz));
                result := 0;    // called 2 times 
                RETURN;         // back 2 caller
              END_IF;  
            END_IF;
          END_IF;
        END_FOR;        
        // try to get memory for a new one
        pComdef^.pt_COM := INI_ONE_COM_IF(login_anz);

        // test if successful
        IF pComdef^.pt_COM = NIL THEN
          result := COMLINK_ERR_MAXCONN;                 // end of memory
          RETURN;
        END_IF;

        DEBUGLOG1("%010d-LOGIN: allocated ComCh #%d for LOCAL", TO_UDINT(login_anz));

        _Lock(pComdef^.pt_Com^.hComchMutex);
        login_anz += 1;                     // one more
        pCom := pComdef^.pt_COM;
        pCom^.isClient := TRUE;//++bugfix040622
        pCom^.Pcomdef := pComdef;           // keep pointer from LSE

        pCom^.nStateS :=0;                  // stop refresh
        pCom^.nActS:=0;
        pCom^.nState:=0;              
        pCom^.nAct:=0;

        pCom^.HisTxObj:=0;                  // lokal refresh 
        result:=0;
        _Unlock(pComdef^.pt_Com^.hComchMutex);
        RETURN;  
      END_IF;

  // ******************
  //     CANBUS CAN1,2 und CAN20..CAN29
  // ******************
  COMLINK_IFNUM_CAN1,
  COMLINK_IFNUM_CAN2,
  COMLINK_IFNUM_CAN20,
  COMLINK_IFNUM_CAN21,
  COMLINK_IFNUM_CAN22,
  COMLINK_IFNUM_CAN23,
  COMLINK_IFNUM_CAN24,
  COMLINK_IFNUM_CAN25,
  COMLINK_IFNUM_CAN26,
  COMLINK_IFNUM_CAN27,
  COMLINK_IFNUM_CAN28,
  COMLINK_IFNUM_CAN29:
      DEBUGLOG1( "%010d-LOGIN: pComdef=0x%X", pComdef$UDINT );
      IF pComdef^.interface < COMLINK_IFNUM_CAN20 THEN
        canNrClt := pComdef^.interface$USINT - COMLINK_IFNUM_CAN1 + 1;
      ELSE
        canNrClt := pComdef^.interface$USINT - COMLINK_IFNUM_CAN20 + 20;
      END_IF;

      IF Pcomdef^.adress < 31 THEN
        bReuse := FALSE;
        comChIdx := -1;
        
        // search for an old one
        FOR i := 1 TO login_anz - 1 DO
          IF myComChs[i] <> NIL THEN
            IF (myComChs[i]^.HisHandle = Pcomdef^.adress) &
               (myComChs[i]^.Pcomdef = pComdef) THEN          // bei ungleichem pComdef wird ein neuer channel aufgemacht
              pComdef^.pt_COM := myComChs[i]; // already exits
              comChIdx := i;
              bReuse := TRUE;
              DEBUGLOG1("%010d-LOGIN: re-using ComCh #%d", comChIdx$UDINT);
              GOTO loginfound; 
            END_IF;
          END_IF;
        END_FOR;        

        // search for an free one
        FOR i := 1 TO login_anz - 1 DO
          IF myComChs[i] = NIL THEN
            pComdef^.pt_COM := INI_ONE_COM_IF(i);
            IF pComdef^.pt_COM <> NIL THEN
              DEBUGLOG1("%010d-LOGIN: *** INI_ONE_COM_IF failed for ComCh #%d", TO_UDINT(i));
              result := COMLINK_ERR_MAXCONN;
            END_IF;
            comChIdx := i;
            selectedCanChNbr := i$UINT - 1;
            DEBUGLOG2("%010d-LOGIN: re-allocated ComCh #%d for dest %d", comChIdx$UDINT, Pcomdef^.adress);
            GOTO loginfound; 
          END_IF;
        END_FOR;        

        // create a new one
        pComdef^.pt_COM := INI_ONE_COM_IF(login_anz);
        IF pComdef^.pt_COM <> NIL THEN
          comChIdx := login_anz;
          selectedCanChNbr := canCltCh_anz;
          login_anz += 1;                                 // one more
          canCltCh_anz += 1;
          DEBUGLOG2("%010d-LOGIN: allocated ComCh #%d for dest %d", comChIdx$UDINT, Pcomdef^.adress);
          GOTO loginfound;  
        END_IF;

        DEBUGLOG1("%010d-LOGIN: *** INI_ONE_COM_IF failed for ComCh #%d", TO_UDINT(login_anz));
        result := COMLINK_ERR_MAXCONN;
        RETURN;

        // Yes we have free memory to communicate //                
    loginfound:
        _Lock(pComdef^.pt_Com^.hComchMutex);
        pCom := pComdef^.pt_COM;

        pCom^.canNr := canNrClt;
        pCom^.isClient := TRUE;//++bugfix040622
        pCom^.Pcomdef := pComdef;  // keep pointer on otto in my struct //
        pCom^.nStateS := 0;
        pCom^.nActS := 0;
        pCom^.nState := 0;              
        pCom^.nAct := 0; 
        pCom^.HisHandle := Pcomdef^.adress;  
        pCom^.pcallback := myComChs[0]^.pcallback; // only in the first is the callback

       #ifdef COMLINK_PG
        // von den IRQ pointer etwas abholen offset0 segment 0 4 byte
        _Lock(MutexPG50); //#FA1204
        canresu := CANUSER_GetBlockPG(CanNr:=1, Station:=Pcomdef^.adress$USINT, SegAdr:=0, OfsAdr:=0, len:=4, #buffer$USINT, ReqType:=0);
        _Unlock(hMutex:=MutexPG50); //#FA1204
        IF canresu = 0 THEN  
          pCom^.HisTxObj := 2;         // can refresh 
          result := 0;
          DEBUGLOG0("%010d - LOGIN o.k.");
        ELSE
          result := 16#ffff;
          DEBUGLOG0("%010d - LOGIN failed");
        END_IF;
       #else
        IF !!bReuse THEN
          // falls CAN nicht vorhanden (hat es nicht immer gegeben)
          // --> dann verwenden wir das alte  
          IF pCanUser = NIL THEN
            pCom^.HisTxObj := ((Pcomdef^.adress*2) + 16#500 + 1)$INT;
            pCom^.OurTxObj := ((Pcomdef^.adress*2) + 16#500)$INT; 
          ELSE
            IF canCltCh_anz > COMLINK_CAN_COMCHS THEN
              login_anz -= 1;
              canCltCh_anz -= 1;
              TRACE_ERR("LOGIN: no more can comlink channels available");
              LSLCLI_LoaderTracePrint("LOGIN: no more can comlink channels available", 12);
              DEBUGLOG1("%010d-LOGIN: *** no more can comlink channels available (%d used)", TO_UDINT(canCltCh_anz));
              FREE_ONE_COM_IF(login_anz);
              comChIdx := -1;
              result := COMLINK_ERR_MAXCONN;
              GOTO LOGIN_unlock;
            END_IF;

            // we take our station multiply it by _MAX_COMCHS_ *2 and add login_anz-1
            // 
            // 32 Station * 2(rx/tx) * 8 = 16#200
            // 16#500+16#200 = 16#700 (!! we start at 16#700 object with LASAL com)
            // so if we have more than 8 comchannels we start at 16#200
            //   
            // (Mystation+(login_anz-2)) * 2 * StartCanAdr 

           #if (COMLINK_CAN_COMCHS+1) <= 8 
            StartCanAdr := 16#500;
           #else
            StartCanAdr := 16#200;
           #endif

            MyCanStation := 0; // to set the rest of the UINT
            CANUSER_GETMYSTATION(1, #MyCanStation$USINT);
            // pr:
            //  Eigentlich würde es ausreichen, mit COMLINK_CAN_COMCHS anstatt mit (COMLINK_CAN_COMCHS+1) 
            //  zu multiplizieren, aber dann wäre man nicht mehr kompatibel zur früheren Berechnung mit 
            //  _MAX_COMCHS_ (_MAX_COMCHS_ ist die Anzahl des lokalen + des CAN Channels)
            MyCanStation := (MyCanStation * (COMLINK_CAN_COMCHS + 1) + selectedCanChNbr) * 2;
            MyCanStation := MyCanStation + StartCanAdr;

            pCom^.HisTxObj := (MyCanStation + 1)$INT;
            pCom^.OurTxObj := (MyCanStation)$INT; 

            DEBUGLOG3("%010d-LOGIN: using ComCh #%d, HisTxObj 16#%X, OurTxObj 16#%X", TO_UDINT(comChIdx), TO_UDINT(pCom^.HisTxObj), TO_UDINT(pCom^.OurTxObj));
          END_IF;
        END_IF; // !!bReuse

        pCom^.Ourhandle := -1$UINT; // clear the IRQ flag. Wird gesetzt, wenn die 4003er Meldung (Bestätigung des LOGINs) eintrifft
        pCom^.m4004 := 0;           // wird gesetzt, wenn die 4004er Meldung (Refreshlistengrößen) eintrifft.
        pCom^.m4005 := 16#FFFFffff; // wird gesetzt, wenn die 4005er Meldung (_swruntime) eintrifft.
        result := COMLINK_ERR_NOCONNECTION;
        pirq := #CRX();

        pCom^.CanHandle := OS_SSR_AddCanObj(pCom^.canNr, pCom^.HisTxObj, 8, 1, pirq, pCom);
        IF pCom^.CanHandle >= 0 THEN
          OS_SSR_LoginIntoCANNew(pCom^.canNr, Pcomdef^.adress$USINT,pCom^.OurTxObj$UINT);

          // now we wait for max of 200 ms if we get back something       
          WaitForResult := OPS.tAbsolute$DINT;

          WHILE (OPS.tAbsolute$DINT- WaitForResult) < 200 DO
            IF pCom^.Ourhandle <> 16#ffff THEN
              IF pCom^.m4004 <> 0 THEN
                pCom^.sizeofRemoteDynList  := (pCom^.m4004 SHR 16)$UINT;
                pCom^.sizeofRemoteStatList := (pCom^.m4004 AND 16#FFFF)$UINT;
              END_IF;
              IF pCom^.m4005 <> 16#FFFFffff THEN
                pCom^.remoteMaxCyclicTime_ms := (pCom^.m4005 AND 16#FFFF)$UDINT * 10;
              ELSE
                pCom^.remoteMaxCyclicTime_ms := 16#FFFFffff;
              END_IF;
              result := COMLINK_OK;
              EXIT;
            END_IF;
          END_WHILE;

          IF result <> COMLINK_OK THEN
            DEBUGLOG1("%010d-LOGIN: *** timout on ComCh #%d", TO_UDINT(comChIdx));
            CANUSER_DelCanObj(pCom^.canNr, pCom^.CanHandle);
          ELSE
            // Meldung 4005 enthält in den unteren 2 Bytes den SW-Runtime-Wert (in 10ms Einheiten)
            txinfo4005.iLng  := 0;
            txinfo4005.iOff  := 4005;
            txinfo4005.dData := TO_DINT(_swruntime);
            //ws003
            IF (CanTxObj(pCom, #txinfo4005$void)) THEN
              DEBUGLOG1("%010d-LOGIN: *** TX error on ComCh #%d", TO_UDINT(comChIdx));
            END_IF;
            // /ws003
          END_IF;
        ELSE
          DEBUGLOG1("%010d-LOGIN: *** AddCanObj failed for ComCh #%d", TO_UDINT(comChIdx));
        END_IF;

    LOGIN_unlock:
       #endif // !!COMLINK_PG
        _Unlock(pComdef^.pt_Com^.hComchMutex);
         
        // Ab der Loaderversion 02.02.164 wird bei einem Fehler in der LOGIN Funktion der 
        // ComChannel freigegeben. 
        // Grund: Es gibt Codestellen (LSE, Dataexcahnge), die pComdef^.pt_COM auf NIL setzen, 
        // wenn LOGIN einen Fehlercode zurückgibt. Das führt dann zu einer Access-Exception, 
        // wenn bei einem darauffolgenden LOGIN Aufruf ein ComChannel wiederverwendet wird 
        // und pComdef^.pt_COM dereferenziert (siehe SA29877).
        IF (result <> COMLINK_OK) & (comChIdx >= 0) THEN
          pComdef^.pt_COM := NIL;
          myComChs[comChIdx]^.HisHandle := -1$UINT;
          FREE_ONE_COM_IF(comChIdx$INT);
        END_IF;

      ELSE
        result := COMLINK_ERR_INVALID_PARAM;
      END_IF;

  // ******************
  //       TCPIP 1,2
  // ******************
  COMLINK_IFNUM_TCP1_OLD,
  COMLINK_IFNUM_TCP1,
  COMLINK_IFNUM_TCP2:
     #ifdef COMLINK_TCP_CLIENT
      result := ComlinkT_Login(pComdef); //++comlinkt
     #else       
      ComlinkT_MissingClientCode();
      result := COMLINK_ERR_UNSUPPORTED_INTERFACE;
     #endif      

      COMLINK_IFNUM_COM1,
      COMLINK_IFNUM_COM2,
      COMLINK_IFNUM_COM3,
      COMLINK_IFNUM_COM4,
      COMLINK_IFNUM_COM5,
      COMLINK_IFNUM_COM6,
      COMLINK_IFNUM_COM7,
      COMLINK_IFNUM_COM8,
      COMLINK_IFNUM_COM9,
      COMLINK_IFNUM_COM10:
      result := ComlinkR_Login(pComdef);

  ELSE
      result := COMLINK_ERR_UNSUPPORTED_INTERFACE;
      
  END_CASE;
    
END_FUNCTION


#ifdef COMLINK_PG
FUNCTION  CAN_GET_BLOCK
VAR_INPUT
	CanNr		: USINT;
	Station		: USINT;
	SegAdr		: UINT;
	OfsAdr		: UINT;
	Len		: UINT;
	bData		: ^ USINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR
VAR
	lenlokal	: UINT;
END_VAR

   _Lock(hMutex:=MutexPg50); //#FA1204
	 retval := 0 ;
    
	 while len do
	    if len > 6 then
	     lenlokal := 6;
		 len -=6;
	    else
		  lenlokal := len;
		  len :=0;
		end_if;  	  	
		if CANUSER_GetBlockPG(CanNr,
						  Station,
						  SegAdr,
						  OfsAdr,
						  lenlokal,
						  bData,
						  ReqType:=0) = 0 then

         else
		   retval := -1;
		   exit;
		 end_if;
       OfsAdr += lenlokal;
	   bData += lenlokal;
	 end_while;
   
   _Unlock(hMutex:=MutexPg50); //#FA1204
END_FUNCTION // ComlinkL::CAN_GET_BLOCK

FUNCTION  CAN_SET_BLOCK
VAR_INPUT
	CanNr		: USINT;
	Station		: USINT;
	SegAdr		: UINT;
	OfsAdr		: UINT;
	Len		: UINT;
	bData		: ^ USINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR
VAR
	lenlokal	: UINT;
END_VAR

   _Lock(hMutex:=MutexPg50); //#FA1204
	 retval := 0 ;
    
	 while len do
	    if len > 4 then
	     lenlokal := 4;
		 len -=4;
	    else
		  lenlokal := len;
		  len :=0;
		end_if;  	  	
		if CANUSER_SetBlockPG(CanNr,
						  Station,
						  SegAdr,
						  OfsAdr,
						  lenlokal,
						  bData,
						  ReqType:=0) = 0 then

         else
		   retval := -1;
		   exit;
		 end_if;
       OfsAdr += lenlokal;
	   bData += lenlokal;
	 end_while;
   _Unlock(hMutex:=MutexPg50); //#FA1204

END_FUNCTION 
#endif // COMLINK_PG

// ************************************************************
// send a command to the interpreter and wait for answer 
// ************************************************************

#ifdef COMLINK_PG
FUNCTION TXCOMMAND_PG
VAR_INPUT
  pt_Head  :^IprHead;
  pt_com   : ^COMDATA;
  Command  :  UDINT;  // command IPRStates
  length   :  UDINT;  // length of command 
  charptr  : ^USINT;  // pointer on the command line (must be copied) 
  pComdef  : ^COMDEF; 
  Presu	 : ^UDINT;  // pointer on result buffer --> back to caller	
END_VAR
VAR_OUTPUT
  Status   : IPRSTATES;
END_VAR
VAR
  buffer   : UDINT;
  retry    : UINT;  // 190705 Aschl Retry zum 2. 
//dummyCmd : BOOL;
END_VAR

  //DEBUGLOG1("%010d - TXCOMMAND CAN1, cmd=%d", command);
  //dummyCmd := FALSE;
  case command$INSTRSET of
    I_GET_OBJ:
      //DEBUGLOG0("%010d - TXCOMMAND CAN1, I_GET_OBJ");
      //dummyCmd := TRUE;
      
      // wir haben keine namen und die adresse ist dem LSE bekannt
      // ist nur wegen kompatbilität
      
      pt_Head^.presu^.uilng :=  14;   // len(2), 2x Ptr (8), Mode (4), class name (x)  // #FA12042010
      pt_Head^.presu^.aData[0] :=  1;
      pt_Head^.presu^.aData[1] :=  1;
      pt_Head^.presu^.aData[2] :=  1;
      
      pt_Head^.presu^.aData[8]$CHMODE := _CH_SVR;    // #FA12042010
      pt_Head^.presu^.aData[10]$UINT  := 0;          // #FA12042010  
      
      status :=READY;
    
    I_WRITE:
      DEBUGLOG0("%010d - TXCOMMAND CAN1, I_WRITE");
      
      if (charptr+10)^ > 4 then
        status := ERROR;
        return;
      end_if;	
      
      
      _Lock(hMutex:=MutexPg50); //#FA1204

      for retry:=0 to 3 do
        if CANUSER_SetBlockPG(CanNr:=1,
                              Station:=pt_Com^.hishandle$USINT,
                              SegAdr:= (charptr+2)^$UINT,
                              OfsAdr:= (charptr)^$UINT,
                              len:= (charptr+10)^$USINT,
                              (charptr+5)$^USINT,
                              ReqType:=0) = 0 then
          pt_Head^.presu^.uilng :=  6;
          status :=READY;
          exit;
        else
          status := ERROR;
        end_if;
      end_for;
      
      _Unlock(hMutex:=MutexPg50); //#FA1204

    I_READ:
      DEBUGLOG1("%010d - TXCOMMAND CAN1, I_READ, seg/ofs=0x%08X", (charptr+1)^$UDINT);

      //_Lock(hMutex:=MutexPg50); //#FA1204

      for retry:=0 to 3  do
        if CANUSER_GetBlockPG(CanNr:=1,
                              Station:=pt_Com^.hishandle$USINT,
                              SegAdr:= (charptr+3)^$UINT,  // #FA171104 (war +2)
                              OfsAdr:= (charptr+1)^$UINT,  // #FA171104 (war +1)
                              len:= 4,
                              #pt_Head^.presu^.aData[0]$USINT,
                              ReqType:=0) = 0 then
                              
          pt_Head^.presu^.uilng :=  6; // #FA12042010
          status :=READY;
          DEBUGLOG1("%010d - TXCOMMAND CAN1, I_READ, data=0x%08X", pt_Head^.presu^.aData[0]$UDINT);
          exit;
        else
          status := ERROR;
        end_if;
      end_for;
      //_Unlock(hMutex:=MutexPg50); //#FA1204
      if status = ERROR then
        sti();
      end_if;  	
    
    I_GET_DESC_CRC:
      //DEBUGLOG0("%010d - TXCOMMAND CAN1, I_GET_DESC_CRC");
      //dummyCmd := TRUE;
      pt_Head^.pResu^.uiLng :=4;
      pt_Head^.pResu^.aData[0] :=0;
      pt_Head^.pResu^.aData[1] :=0;
      pt_Head^.pResu^.aData[2] :=0;
      pt_Head^.pResu^.aData[3] :=0;
      status :=READY;
    
    I_GET_VERSION:
      DEBUGLOG0("%010d - TXCOMMAND CAN1, I_GET_VERSION");
      
      _Lock(hMutex:=MutexPg50); //#FA1204
      if (CANUSER_GetBlockPG(CanNr:=1,Station:=Pcomdef^.adress$USINT,SegAdr:=0,OfsAdr:=16#1000,len:=4,#buffer$USINT,ReqType:=0)) = 0 then
        pt_Head^.pResu^.uiLng :=sizeof(Ops.uiLoaderVersion)+2;
        pt_Head^.pResu^.aData[0]$UINT :=ops.uiLoaderVersion.usLoRev;
        pt_Head^.pResu^.aData[2]$UINT :=ops.uiLoaderVersion.usHiRev;
        status :=READY;
        _Unlock(hMutex:=MutexPg50); //#FA1204
        return;
      end_if;
      _Unlock(hMutex:=MutexPg50); //#FA1204
      status:= ERROR;
    
    I_CMD:
      DEBUGLOG0("%010d - TXCOMMAND CAN1, I_CMD");
      // string holen
      if (charptr+4)^$UINT = 1 then
      
        if CAN_Get_Block(CanNr:=1,
                         Station:=pt_Com^.hishandle$USINT,
                         SegAdr:= (charptr+2)^$UINT,
                         OfsAdr:= (charptr)^$UINT,
                         len:= (charptr+7)^$UINT,
                         #pt_Head^.presu^.aData[4]$USINT
                        ) = 0 then
        
          pt_Head^.presu^.uiLng := 4+2+(charptr+7)^$UINT;
          pt_Head^.presu^.aData[0]$UDINT := (charptr+7)^$UINT;
        else
          status := ERROR;
          return;
        end_if;
      
        status :=READY;
      
      
      // string schreiben 
      elsif (charptr+4)^$UINT = 4 then
      
        if (charptr+6)^$PREFIX = p_user_stream then		
          if CAN_Set_Block(CanNr:=1,
                           Station:=pt_Com^.hishandle$USINT,
                           SegAdr:= (charptr+2)^$UINT,
                           OfsAdr:= (charptr)^$UINT+(charptr+11)^$UINT,
                           len:= (charptr+7)^$UINT-2,		// weil otto 2 dazuhaut
                           charptr+17
                           ) = 0 then
          
            pt_Head^.presu^.uiLng := 0;
          else
            status := ERROR;
            return;
          end_if;
        
          status :=READY;
        end_if;
      
      
      else
        status:= ERROR;
      end_if;
      
    else
      DEBUGLOG0("%010d - TXCOMMAND CAN1, unsupported cmd");
      status:= ERROR;
  end_case;
  
//  IF !!dummyCmd THEN;
//    DEBUGLOG1("%010d - TXCOMMAND CAN1, status=%d", status$UDINT);
//  END_IF;

END_FUNCTION
#endif // COMLINK_PG

// nicht wiedereintrittsfähige Version von TXCOMMAND
FUNCTION TXCOMMAND_nots
VAR_INPUT
    Command  :  UDINT;  // command IPRStates
    length   :  UDINT;  // length of command 
    charptr  : ^USINT;  // pointer on the command line (must be copied) 
    pComdef  : ^COMDEF; 
    pPresu   : ^UDINT;  // pointer to a pointer on result buffer --> back to caller 
    pReason     : ^comlinkReason;   // reason-code in the case of an error
END_VAR
VAR_OUTPUT
    Status   : IPRSTATES;
END_VAR
VAR
    ulength  :UINT;
#ifndef COMLINK_PG
    count    :UINT;
    i        :int; //ws003
    src_lokal:^USINT;
    buff_of  :UINT;
    cmdbuf   :  COMOBJSTR;
    WaitForResult : DINT;
    uiLng   : UINT;
#endif    
    pt_com   : ^COMDATA;
    pt_code  :^INSTRUCT;
    TxState  : IPRSTATES; 
    pt_Head  :^IprHead;
    bNext    : UDINT;
END_VAR

  CASE pComdef^.interface OF
    COMLINK_IFNUM_TCP1_OLD, 
    COMLINK_IFNUM_TCP1: 
  #ifdef COMLINK_TCP_CLIENT
    pReason^ := ComlinkT_TxCommandEx(Command, length, charptr, pComdef, pPresu, #Status, 0); //++comlinkt
  #else        
    ComlinkT_MissingClientCode();
    Status := ERROR;
    pReason^ := COMLINK_ERR_UNSUPPORTED_INTERFACE;
  #endif       
    RETURN;

    COMLINK_IFNUM_COM1,
    COMLINK_IFNUM_COM2,
    COMLINK_IFNUM_COM3,
    COMLINK_IFNUM_COM4,
    COMLINK_IFNUM_COM5,
    COMLINK_IFNUM_COM6,
    COMLINK_IFNUM_COM7,
    COMLINK_IFNUM_COM8,
    COMLINK_IFNUM_COM9,
    COMLINK_IFNUM_COM10:
      pReason^ := ComlinkR_TxCommandEx(Command, length, charptr, pComdef, pPresu, #Status, 0);
      RETURN;
  END_CASE;
  
  ulength := length$UINT;  
  pt_com := pComdef^.pt_com; 
#ifndef DUMMY_LOADER
  pt_Head  := pt_com^.Com.GetHead();
#endif  
  pPresu^  := pt_Head^.pResu$UDINT;
  
  // load the pointer in the code
  pt_code := pt_Head^.pCode;   

  // now write the instruction into the interpreter
  pt_code^.uiLng   :=ulength + SIZEOF (instruct.uiLng);
  pt_code^.OpCode  :=Command$INSTRSET;
  _memcpy(#pt_code^.aPara,charptr,ulength);
  
  // start the command
  pt_Head^.workstate := BUSY;
  pt_Head^.funkst := READY;
  TxState := pt_Head^.workstate;

//***********************************************************************
// CHECK IF WE HAVE TO TRANSFER IT OR IS IT INTERNAL THEN JUST WAIT
//***********************************************************************

#ifndef DUMMY_LOADER
#ifdef LOADER_4_LASAL2          
  pt_Com^.Com._m.resu.uilng :=0;
#else  
  pt_Com^.Com.resu.uilng :=0;
#endif  
#endif
  
  // 0,1 = LOKAL
  IF pComdef^.interface <= 1 then

#ifndef DUMMY_LOADER
    Status := pt_Com^.Com.BEFIPR(#bNext);
#endif    
    SyncRefLists(pt_Com);
    pReason^ := COMLINK_IPR;
    pt_Head^.workstate:=Status;

  // 6,7 CAN1 and 2
 ELSIF pComdef^.interface <= COMLINK_IFNUM_CAN2 | (pComdef^.interface >= COMLINK_IFNUM_CAN20 & pComdef^.interface <= COMLINK_IFNUM_CAN29)  then

#ifdef COMLINK_PG
    Status := TXCOMMAND_PG(pt_Head, 
                           pt_Com,
                           Command, 
                           length, 
                           charptr, 
                           pComdef, 
                           pPresu);
#else
    // add the length itself
    count:= (ulength +
             SIZEOF (instruct.opCode)+
             SIZEOF (instruct.uiLng))$UINT;

    // first the data start at instr.lng
    src_lokal := pt_Head^.pCode$^usint;
    buff_of   := 4; // we work absolute

    IF ((buff_of+count) < 999)   THEN

      WHILE count <> 0 DO
        IF count > 4 THEN
          count -=4;
          cmdbuf.iLng := 4;
        ELSE
          cmdbuf.iLng := count$INT;
          count       := 0;   // last one
        END_IF;
        cmdbuf.dData := src_lokal^$DINT;
        cmdbuf.iOff  := buff_of$INT; // offset into buffer

        // send the data over the bus !!! //
        i := CanTxObj_WithBremse(pt_Com, #cmdbuf$void);
        IF i <> 0 THEN
          pt_Head^.workstate := ERROR;
          Status := ERROR;
          pReason^ := COMLINK_ERR_GENERAL;
          RETURN;
        END_IF;
     
        src_lokal += 4;
        buff_of += 4;
      END_WHILE;

      //last the command
      cmdbuf.iLng  := 4;
      cmdbuf.dData := pt_Head^.workstate$DINT;
      cmdbuf.iOff  := 0; // offset into buffer absolute

      // Die SequenceNumber (=nächster zu erwartender Offset) der Antwort wird 
      // mit 0 initialisiert.
      pt_Com^.respSN := 0;

      // send the data over the bus !!! //
      i := CanTxObj_WithBremse(pt_Com, #cmdbuf$void);
      IF i <> 0 THEN
        pt_Head^.workstate := ERROR;
        Status := ERROR;
        pReason^ := COMLINK_ERR_GENERAL;
        RETURN;
      END_IF;

    END_IF;

    // load the timer for the answer 
    WaitForResult := OPS.tAbsolute$DINT;
   #ifdef _LSL_USECLISTI 
    STI();         // otherwise we are offline
   #endif 
    // we wait a max time of 1000 ms not more //
    pReason^ := COMLINK_ERR_TIMEOUT;
    WHILE (OPS.tAbsolute$DINT - WaitForResult) < 1000 DO
      IF pt_Head^.workstate <> BUSY THEN
#ifndef DUMMY_LOADER
      #ifdef LOADER_4_LASAL2          
        uiLng := pt_Com^.Com._m.resu.uilng;
      #else
        uiLng := pt_Com^.Com.resu.uilng;
      #endif
#endif      
        IF pt_Com^.respSN = 16#ffff | uiLng <> pt_Com^.respSN THEN
          // es sind nicht alle CAN Messages der Antwort angekommen
          pt_Head^.workstate := ERROR;
          Status := ERROR;
          pReason^ := COMLINK_ERR_GENERAL;
          RETURN;
        ELSE
          pReason^ := COMLINK_IPR;
          EXIT;
        END_IF;
      END_IF;
      IF (OPS.tAbsolute$DINT - WaitForResult) > 5 THEN
        OS_SSR_Delay(1);  // 1ms blockieren spart CPU Resourcen
      END_IF;
    END_WHILE;
    Status := pt_Head^.workstate;
#endif // COMLINK_PG

  ELSE
    pt_Head^.workstate := ERROR;
    Status := ERROR;
    pReason^ := COMLINK_ERR_UNSUPPORTED_INTERFACE;
  END_IF;
 
END_FUNCTION

FUNCTION TXCOMMAND_ts
VAR_INPUT
    Command  :  UDINT;  // command IPRStates
    length   :  UDINT;  // length of command 
    charptr  : ^USINT;  // pointer on the command line (must be copied) 
    pComdef  : ^COMDEF; 
    pPresu      : ^UDINT;   // pointer to a pointer on result buffer --> back to caller 
    pResuBuf    : ^RESULTS; // result buffer
    sizeResuBuf : UDINT;    // size of result buffer
    pReason     : ^comlinkReason;   // reason-code in the case of an error
END_VAR
VAR_OUTPUT
    Status   : IPRSTATES;
END_VAR
VAR
    pResuLocal  : UDINT;
    reasonLocal : comlinkReason;
    pResu       : ^RESULTS;
END_VAR

    IF (pReason = NIL) THEN
        pReason := #reasonLocal;
    END_IF;

    pReason^ := COMLINK_ERR_GENERAL;

    IF GetAndLockComdata(pComdef) = NIL THEN
        Status := ERROR;
        pReason^ := COMLINK_ERR_INVALID_PARAM;
        RETURN;
    END_IF;

    IF pPresu = NIL THEN
        pPresu := #pResuLocal;
    END_IF;

    Status := TXCOMMAND_nots(Command, length, charptr, pComdef, pPresu, pReason);

    pResu := pPresu^$^RESULTS;

    IF (Status <> ERROR) & (pResuBuf <> NIL) & (pResu^.uilng <> 16#ffff) THEN
        IF (sizeResuBuf < pResu^.uilng) THEN
            // zuwenig Platz in pResuBuf
            Status := ERROR;
            pReason^ := COMLINK_ERR_BUF_TOO_SMALL;
        ELSE
            // Daten zum Aufrufer kopieren
            _memcpy(pResuBuf, pResu, pResu^.uilng);
        END_IF;
    END_IF;
    
    ReleaseComdata(pComdef^.pt_Com$^ComData_Base);

END_FUNCTION
FUNCTION GLOBAL __CDECL TXCOMMAND
VAR_INPUT
    Command     :  UDINT;  // command IPRStates
    length      :  UDINT;  // length of command 
    charptr     : ^USINT;  // pointer on the command line (must be copied) 
    pComdef     : ^COMDEF; 
    Presu       : ^UDINT;  // pointer to a pointer on result buffer --> back to caller  
END_VAR
VAR_OUTPUT
    Status   : IPRSTATES;
END_VAR

    Status := TXCOMMAND_ts(Command, length, charptr, pComdef, Presu, NIL, 0, NIL);

END_FUNCTION
FUNCTION GLOBAL __CDECL TXCOMMANDEX
VAR_INPUT
    Command     :  UDINT;   // command IPRStates
    length      :  UDINT;   // length of command 
    charptr     : ^USINT;   // pointer on the command line (must be copied) 
    pComdef     : ^COMDEF; 
    pResuBuf    : ^RESULTS; // result buffer
    sizeResuBuf : UDINT;    // size of result buffer
    pReason     : ^comlinkReason;   // reason-code in the case of an error
END_VAR
VAR_OUTPUT
    Status   : IPRSTATES;
END_VAR

    Status := TXCOMMAND_ts(Command, length, charptr, pComdef, NIL, pResuBuf, sizeResuBuf, pReason);

END_FUNCTION


// command to transmit a Updatecell into the updatebuffer
FUNCTION TXUPD_nots
VAR_INPUT
    pLslcommregdata : ^Lslcommregdata;
    pComdef         : ^COMDEF;
    uiflags         : UINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR
VAR
    pt_com   : ^COMDATA;
    pt_COMSTCELL :^COMSTCELL;
    timx : udint;
#ifndef COMLINK_PG
    cmdbuf   :  COMOBJSTR;
    i         : int; //ws003
#endif    
END_VAR

    retval := COMLINK_ERR_GENERAL;
    
   CASE pComdef^.interface OF
      COMLINK_IFNUM_TCP1_OLD, 
      COMLINK_IFNUM_TCP1: 
   #ifdef COMLINK_TCP_CLIENT
        retVal := ComlinkT_TxUpd(pLslcommregdata, pComdef, 0, uiflags); //++comlinkt
   #else        
        retVal := COMLINK_ERR_UNSUPPORTED_INTERFACE;
      ComlinkT_MissingClientCode();
   #endif       
      RETURN;
  
      COMLINK_IFNUM_COM1,
      COMLINK_IFNUM_COM2,
      COMLINK_IFNUM_COM3,
      COMLINK_IFNUM_COM4,
      COMLINK_IFNUM_COM5,
      COMLINK_IFNUM_COM6,
      COMLINK_IFNUM_COM7,
      COMLINK_IFNUM_COM8,
      COMLINK_IFNUM_COM9,
      COMLINK_IFNUM_COM10:
        retVal := ComlinkR_TxUpd(pLslcommregdata, pComdef, 0, uiflags);
        RETURN;
    END_CASE;

    pt_com := pComdef^.pt_com; 
    IF pt_com = NIL  THEN
      RETURN;
    END_IF;

    IF CheckRefreshlist(pt_com) <> 0 THEN
      RETURN;
    END_IF;

    IF pLslcommregdata^.varpos$INT < 1000 THEN
        // check for overwrite
        IF (pLslcommregdata^.varpos >= pt_com^.sizeofStatList) | 
           (pLslcommregdata^.varpos >= pt_com^.sizeofRemoteStatList) THEN
            TRACE_WARN("sizeof maxStatCOMcell (comlink.st)"); 
            LSLCLI_LoaderTracePrint("sizeof maxStatCOMcell (comlink.st)", 12); 
            retVal := COMLINK_ERR_MAXENTRIES;
            return;         // back to caller
        END_IF;  
    ELSE
        IF ((pLslcommregdata^.varpos - 1000) >= pt_com^.sizeofDynList) | 
           ((pLslcommregdata^.varpos - 1000) >= pt_com^.sizeofRemoteDynList) THEN
            TRACE_WARN("sizeof maxDynCOMcell (comlink.st)"); 
            LSLCLI_LoaderTracePrint("sizeof maxDynCOMcell (comlink.st)", 12);
            retVal := COMLINK_ERR_MAXENTRIES;
            RETURN;         // back to caller
        END_IF;
    END_IF;


    if pLslcommregdata^.varpos$INT < 1000 then
        pt_COMSTCELL := pt_com^.paStatList;
      pt_COMSTCELL += (pLslcommregdata^.varpos$UINT) * SIZEOF (COMSTCELL);
     else
        pt_COMSTCELL := pt_com^.paDynList;
      pt_COMSTCELL += ((pLslcommregdata^.varpos$UINT)-1000) * SIZEOF (COMSTCELL);
    end_if;


    // THR 7/30/01: Was checking only for _LOCAL INTERFACE types,
    // needs to check for LOCAL and INTERN!
    //if pComdef^.interface = 0 then
    if pComdef^.interface <= 1 then
    
        timx                       := pLslcommregdata^.uiTime; // u32 := u16 
        pt_COMSTCELL^.pCh          := pLslcommregdata^.LASALID$^SVRCH;
        pt_COMSTCELL^.udUpdateRate := ((timx and 16#3FFF) or ((timx and 16#C000) shl 16) or (uiflags shl 14));
        pt_COMSTCELL^.dData        := 16#80000001$DINT;                // INVALID  
        pt_COMSTCELL^.udLastUpdate := OPS.tAbsolute;
        pt_COMSTCELL^.VarlistID    := pLslcommregdata^.VarlistID;

    elsif pComdef^.interface <= COMLINK_IFNUM_CAN2 | (pComdef^.interface >= COMLINK_IFNUM_CAN20 & pComdef^.interface <= COMLINK_IFNUM_CAN29) then

        timx                       := pLslcommregdata^.uiTime; // u32 := u16 
        pt_COMSTCELL^.pCh          := pLslcommregdata^.LASALID$^SVRCH;
        pt_COMSTCELL^.udUpdateRate := ((timx and 16#3FFF) or ((timx and 16#C000) shl 16) or (uiflags shl 14));
        pt_COMSTCELL^.VarlistID    := pLslcommregdata^.VarlistID;
#ifdef COMLINK_PG
        pt_COMSTCELL^.dData := 16#80000001$DINT; 				// INVALID	

        DEBUGLOG2("%010d - TXUPD CAN1, lasalid=0x%08X, channel=%d", pLslcommregdata^.LASALID$UDINT, TO_UDINT(pLslcommregdata^.channel));
        DEBUGLOG2("%010d -             varpos=%d, time=%d", pLslcommregdata^.varpos, TO_UDINT(pLslcommregdata^.uiTIME));
#else      

        cmdbuf.iOff := pLslcommregdata^.varpos$INT; 
        cmdbuf.iOff += 2000;  // otto zaehlt + 1000 wenn dynamisch 

        cmdbuf.iLng  := pLslcommregdata^.uiTime$INT;
        cmdbuf.dData := pLslcommregdata^.LASALID$DINT;

        // send the data over the bus !!! //
        i := CanTxObj_WithBremse(pt_Com, #cmdbuf$void); //ws003
        case i of
            0   : retval := COMLINK_OK;
            1   : retval := COMLINK_ERR_UNSUPPORTED_INTERFACE;
            2   : retval := COMLINK_ERR_INVALID_PARAM;
            4   : retval := COMLINK_ERR_MAXENTRIES;
            else  retval := COMLINK_ERR_GENERAL;
        end_case;  // /ws003
#endif        

    end_if;

end_function

(*
  Im Element LslCommRegData.uiTIME ist neben der Update-Rate in ms auch noch folgende 
  Zusatzinformation hineincodiert:
  Bit 15 (16#8000) .. es handelt sich um ein VBI Objekt (Datenbuffer statt 4 Byte Wert)
  Bit 14 (16#4000) .. es handelt sich um eine globale Variable (4 Byte Datenlänge)
 *)
FUNCTION GLOBAL __CDECL TXUPD
VAR_INPUT
    pLslcommregdata : ^Lslcommregdata;
    pComdef         : ^COMDEF;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR

    retval := TXUPD2(pLslcommregdata, pComdef, 0);
    
END_FUNCTION
(*
  Im Element LslCommRegData.uiTIME ist neben der Update-Rate in ms auch noch folgende 
  Zusatzinformation hineincodiert:
  Bit 15 (16#8000) .. es handelt sich um ein VBI Objekt (Datenbuffer statt 4 Byte Wert)
  Bit 14 (16#4000) .. es handelt sich um eine globale Variable (4 Byte Datenlänge)
  
  uiFlags: kodiert die freien bits in COMSTCELL.udUpdateRate
		   Bit 15 (16#8000) .. es handelt sich um ein MerkerEx Objekt (Datenbuffer statt 4 Byte Wert)
 *)
FUNCTION GLOBAL __CDECL TXUPD2
VAR_INPUT
    pLslcommregdata : ^Lslcommregdata;
    pComdef         : ^COMDEF;
    uiflags         : UINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR

    retval := COMLINK_ERR_GENERAL;

    IF GetAndLockComdata(pComdef) = NIL THEN
        retval := COMLINK_ERR_INVALID_PARAM;
        RETURN;
    END_IF;

    retVal := TXUPD_nots(pLslcommregdata, pComdef, uiflags);

    ReleaseComdata(pComdef^.pt_Com$^ComData_Base);

END_FUNCTION


//------------------------------------------------------------------------------
// calloc - allocates memory with elements initialized to 0
//------------------------------------------------------------------------------
FUNCTION calloc
VAR_INPUT
    udSize  : UDINT;
END_VAR
VAR_OUTPUT
    pMem    : ^void;
END_VAR

  pMem := _alloc_resize_LDR(udSize);
  IF pMem <> NIL THEN
    _memset(pMem, 0, udSize);
  END_IF;
  
END_FUNCTION

// *************************************************************
//  ** function to set the callback method for cyclic refresh
// *************************************************************
// till 16.12.2003
//   for all installed comchs only one callback function
// sice 16.12.2003
//   out of backward and forward compatibility we check
//   if this void^ is a pointer into code 
//   1. if YES we do old 
//   2. if NO the pointer points on a struct
//      first pointer in struct is call back
//      second pointer is pointer on handle--> comchs 
//      third pointer is this (esi on Call)
//      pStruct --> 0,1,2,3   pointer on callback code
//                  4,5,6,7   pointer back from login (comdef.pt_com)
//                  8,9,10,11 this pointer (your own this --> so ESI is loaded on call)
//

FUNCTION GLOBAL __CDECL INSTALLCALLBACK

VAR_INPUT
   PCallback : ^void;
END_VAR

VAR
 ptc                             : ^COMDATA;
 i                               : INT;
END_VAR

 // --> check if pointer is on data or on code

 if (pCallback >= _UserProgPointer) & (pCallback < _UserProgPointer+_UserProgSize) then 

  #ifdef COMLINK_TCP_CLIENT
    ComlinkT_InstallCallback(PCallback); //++comlinkt
  #endif    

    ComlinkR_InstallCallback(PCallback);

  FOR i:=0 TO (login_anz-1) DO
   
   ptc:=    myComChs[i];
   
   // old set to all comdata the callback
   if ptc <> NIL then
    if ptc^.pCallback = #PrototypeCallback() then
     ptc^.pCallback:=PCallback;
    end_if;
   end_if;
    
  END_FOR;

 else

    
  // new just set to one comdata the callback
    
  ptc        := (PCallback+4)^$^comdata;    // pointer on struct comdata
  ptc^.pThis := (PCallback+8)^$UDINT;       // thispointer on callback
  ptc^.pCallback := PCallback^$^void;       // callbackadress

 end_if;
end_function

FUNCTION GLOBAL __CDECL INSTALLCALLBACK_DYN
VAR_INPUT
   PCallback : ^void;
   pThis     : ^void;
   pComdata  : ^COMDATA;
END_VAR
VAR
	pEntry : ^sDynCallbackList;
END_VAR


  IF (pComdata = NIL) THEN
    RETURN;
  END_IF;

  pEntry := calloc(SIZEOF(sDynCallbackList))$^sDynCallbackList;
  IF pEntry = NIL THEN
     RETURN;
  END_IF;
  pEntry^.pNext := NIL;
  pEntry^.pCallback := calloc(SIZEOF(sDynCallback))$^sDynCallback;
  IF pEntry^.pCallback = NIL THEN
     RETURN;
  END_IF;
  pEntry^.pCallback^.pCallBack := PCallback;
  pEntry^.pCallback^.pThis := pThis;
    
    
  IF (pComdata^.pDynCallbacks = NIL) THEN
    pComdata^.pDynCallbacks := pEntry;
  ELSE 
    pEntry^.pNext := pComdata^.pDynCallbacks;
    pComdata^.pDynCallbacks := pEntry^.pNext;
  END_IF;

END_FUNCTION

// stehen lassen ist der prototyp zum aufruf von koller //
FUNCTION GLOBAL __CDECL PrototypeCallback

VAR_INPUT
#pragma warning (disable : 73)
    pComdef  : ^COMDEF;
    pData    : ^d2LSE;
#pragma warning (default : 73)
END_VAR
end_function

Function Global __CDECL StartStopRefresh_nots
Var_input
 pComdef  : ^COMDEF;
 count    :uint;
 typ      :uint;
end_var

VAR
    pt_com   : ^COMDATA;
#ifndef COMLINK_PG
    cmdbuf   :  COMOBJSTR;
#endif    
END_VAR

  CASE pComdef^.interface OF
    COMLINK_IFNUM_TCP1_OLD, 
    COMLINK_IFNUM_TCP1: 
     #ifdef COMLINK_TCP_CLIENT
      ComlinkT_StartStopRefresh(pComdef, count, typ, 0); //++comlinkt
     #else        
      ComlinkT_MissingClientCode();
     #endif       
      RETURN;
  
    COMLINK_IFNUM_COM1,
    COMLINK_IFNUM_COM2,
    COMLINK_IFNUM_COM3,
    COMLINK_IFNUM_COM4,
    COMLINK_IFNUM_COM5,
    COMLINK_IFNUM_COM6,
    COMLINK_IFNUM_COM7,
    COMLINK_IFNUM_COM8,
    COMLINK_IFNUM_COM9,
    COMLINK_IFNUM_COM10:
      ComlinkR_StartStopRefresh(pComdef, count, typ, 0);
      RETURN;
  END_CASE;

  pt_com := pComdef^.pt_com; 
  IF(pt_com = NIL) THEN    //++bugfix040622
    RETURN;
  END_IF;

  IF(CheckRefreshlist(pt_com) <> 0) THEN
    RETURN;
  END_IF;

  // THR 7/30/01: Was checking only for _LOCAL INTERFACE types,
  // needs to check for LOCAL and INTERN!
  // if pComdef^.interface = 0 then
  if(pComdef^.interface <= 1) then
    // write in to lokal data       
    // !! NO IRQ in this few lines (ASCHL) //   
    // checked since 20.01.2003 (overflow)
   #ifndef _LSL_USECLISTI 
    if(typ = 0) then
      if(count > pt_com^.sizeofStatList) then
        count := pt_com^.sizeofStatList;
      end_if;
      pt_com^.nActS   := 0;
      pt_com^.nStateS := count;
      pt_com^.nActS   := 0; // zur sicherheit weil nicht mit semaphor geschützt
    else
      if(count > pt_com^.sizeofDynList) then
        count := pt_com^.sizeofDynList;
      end_if;
      pt_com^.nAct   := 0;
      pt_com^.nState := count;
      pt_com^.nAct   := 0; // zur sicherheit weil nicht mit semaphor geschützt
    end_if; 
   #else
    CLI();
    if(typ = 0) then
      if(count < pt_com^.sizeofStatList) then
        pt_com^.nActS   := 0;
        pt_com^.nStateS := count;
      else
        pt_com^.nActS   := 0;
        pt_com^.nStateS := pt_com^.sizeofStatList;
      end_if;
    else
      if(count < pt_com^.sizeofDynList) then
        pt_com^.nAct   := 0;
        pt_com^.nState := count;
      else 
        pt_com^.nAct   := 0;
        pt_com^.nState := pt_com^.sizeofDynList;
      end_if;
    end_if; 
    STI();
   #endif

  elsif(pComdef^.interface <= COMLINK_IFNUM_CAN2) | ((pComdef^.interface >= COMLINK_IFNUM_CAN20) & (pComdef^.interface <= COMLINK_IFNUM_CAN29)) then
        
   #ifdef COMLINK_PG
    DEBUGLOG2("%010d - StartStopRefresh CAN1, count=%d, typ=%d", count, typ);
    loadlist_AWL(pt_com,count,typ);
   #else        
    if(typ = 0) then
      cmdbuf.iOff := 4000;
    else
      cmdbuf.iOff := 4001;  
    end_if; 

    cmdbuf.iLng :=4;
    cmdbuf.dData := to_dint(count);

    // send the data over the bus !!! //

    CanTxObj_WithBremse(pt_Com, #cmdbuf$void); 
   #endif        
  end_if;

end_function
FUNCTION GLOBAL __CDECL StartStopRefresh 
VAR_INPUT
    pComdef : ^COMDEF;
    count   : uint;
    typ     : uint;
END_VAR

    IF GetAndLockComdata(pComdef) = NIL THEN
        RETURN;
    END_IF;

    StartStopRefresh_nots(pComdef, count, typ);

    ReleaseComdata(pComdef^.pt_Com$^ComData_Base);

END_FUNCTION


//++comlinkt:begin
#ifdef COMLINK_TCP
////////////////////////////////////////////////////////////////////////////////
//
// COMLINK over TCP/IP
//
//  Requirements:
//
//    Comlink/TCP client:   LasalOS 5.43 or higher (MT-API bug fixed)
//    Comlink/TCP server:   LasalOS 5.28 or higher (TCP-User API)
//
//  Exported functions:
//    
//    ComlinkT_Init
//    ComlinkT_Login
//    ComlinkT_TxCommand
//    ComlinkT_TxUpd
//    ComlinkT_InstallCallback
//    ComlinkT_StartStopRefresh
//
//    ComlinkT_CyWork
//
//  Modification history:
//
//    pr    03-nov-2003   initial version
//
//
//  Open issues:
//
//  - Function Login does not specify a port number of the remote server,
//    so it is not possible to login to a server that listens to ports other 
//    than 1955/1956
//
//  Functional description:
//
//    Tow connections are used: the command channel and the refresh channel.
//    The comlink server listens on the command server port and on the refresh 
//    server port for incoming connections. 
//    The command server serves BEFIPR commands and the refresh server serves 
//    TXUPD and STARTSTOPREFRESH commands and sends updated cells to the client. 
//    The command channel uses a request/response protocol: The client sends a 
//    BEFIPR request and the server send the answer in the response.
//    On the refresh data is sent without any acknoledge mechanism: The client 
//    sends TXUPD- and SSREFR(StartStopRefresh) requests to the server and the 
//    server sends UPDCELL requests to the client.
//
//      Client                              Server
//      --------                            ----------
//      SESSINFO-Req. -- cmd channel   --> 
//                   <--               --   SESSINFO-Resp.
//      BEFIPR-Req.   -- cmd channel   --> 
//                   <--               --   BEFIPR-Resp.
//
//      SESSINFO-Req. -- refr. channel --> 
//                   <--               --   SESSINFO-Resp.
//      TXUPD-Req.    -- refr. channel -->
//      SSREFR-Req.   -- refr. channel -->
//                   <-- refr. channel --   UPDCELL-Req.
//
//    Requests on the refresh channel are not always immediately sent. The data 
//    can be queued and sent when enough data is available, because tcp/ip 
//    performs better when one large write is used instead of multiple small 
//    writes.
//
//    Reconnects:
//    When the client looses a connection, it initiates a reconnect process. 
//    After a connection could be re-established, the instance-ID of the server 
//    is compared to the instance-ID of the previous session. When the instance-
//    IDs are the same, the reconnection was successful, otherwise no reconnect 
//    is possible because the server was restarted and has lost all of its 
//    session information, e.g. the content of the update list.
//    The server does not discard the content of the update list after a
//    connection failure, it is preserved for a possible reconnect of the client.
//
//  Remarks
//
//    In the current implementation the server closes the connection when it 
//    receives a variable record whose size is greater than the size specified 
//    in function ReadCmdMsg, because there is no mechanism implemented to 
//    discard the unneeded data. This makes the the code faster and simpler.
//    The smallest size specified in any ReadCmdMsg Call of the server is 
//    SIZEOF(cMessage512).
//    As long as no bigger messages are used in the comlink protocol, it works. 
//    When bigger messages are invented, the protocol version number has to be 
//    increased and the version number of the peer has to be checked before 
//    sending bigger messages !
//
////////////////////////////////////////////////////////////////////////////////

#include <LSL_ST_TCP_USER.h>

//
// defines
//

#define COMLINKT_VERSION          0x0100  // version major, version minor
#define INVALID_SOCKET_NUM       -1
#define TCP_NO_ERROR              0
// interface types
#define IF_TYPE_TCPIP1            8
#define IF_TYPE_TCPIP2            9
#define IF_TYPE_TCPIP1_NEW       10
#define IF_TYPE_TCPIP2_NEW       11

// various constants
#define INIT_STATLIST_ENTRIES   300
#define INIT_DYNLIST_ENTRIES    600
#define MAX_STATLIST_ENTRIES   1000
#define MAX_DYNLIST_ENTRIES    15000
#define SMALLEST_UPD_INTERVALL   10       // a smaller update intervall makes no sense
#define HEARTBEAT_INTERVALL    5000      
#define CLIENT_TIMEOUT_REQ     3000       // timeout for requests of a client: 3 sec.
#define CLIENT_TIMEOUT_CONNECT 3000       // timeout for a connect of a client: 3 sec.
// values for isInstIdVerified
#define INSTID_NOT_VERIFIED       0
#define INSTID_CMD_PENDING        1
#define INSTID_MATCH              2
#define INSTID_MISMATCH           3
// type of a connection block
#define TYPE_CMD_CONN             0
#define TYPE_REFR_CONN            1
// commands
#define CMD_BEFIPR                0
#define CMD_TXUPD                 1
#define CMD_SSREFR                2
#define CMD_UPDCELL               3
#define CMD_SESSINFO              4
#define CMD_NOP                   5
#define CMD_MAXIDLETIME           6
#define CMD_IAMALIVE              7
#define CMD_CONNECT               8 // nur für RS232
#define CMD_LOGOUT                9
// error codes at initialization
#define COMLINKT_ERR_NOTCP          -1
#define COMLINKT_ERR_NOMTAPI        -2
#define COMLINKT_ERR_WRONG_OSVER    -3
#define COMLINKT_ERR_API_FAILED     -4
// return codes (must not overlap with tcp-user error codes)
#define ERR_INVALID_VRECHDR     -60000
#define ERR_INVALID_CMSGHDR     -60001
#define ERR_INVALID_CMSGTYPE    -60002
#define ERR_INVALID_CMSGCODE    -60003
#define ERR_INVALID_CMSGDATA    -60004
#define ERR_NACK_RECEIVED       -60005
#define ERR_BUFFER_SHORTAGE     -60006
#define ERR_CONN_BROKEN         -60007
#define ERR_BUF_TOO_SMALL       -60008

#define QUERY_FREE_TXBUF(bsock)     (bsock.sizeOfTxBuf - bsock.bytesInTxBuf)
#define FLUSH_TXBUF(bsock)          SendNBuffered(bsock,NIL,0,0,1)

//
// variables
//
VAR_PRIVATE
  lsl_tcp_user  : ^LSL_TCP_USER;    // tcp user interface pointer
  isClientReady : DINT;             // 0 indicates that client is ready, a neg. value is a error code
  isServerReady : DINT;             // 0 indicates that server is ready, a neg. value is a error code
#ifdef COMLINK_TCP_CLIENT
  loginAnz      : UDINT;
  loginArrayMutex : MT_SEMAHANDLE;  // Mutex for accessing loginArray
  g_pCallback   : ^void;            // callback function
#endif  
#ifdef COMLINK_TCP_SERVER
  cmdSockSrvr   : DINT;             // command socket of the server
  refrSockSrvr  : DINT;             // refresh socket of the server
  nbrCmdConn    : UDINT;
  nbrRefrConn   : UDINT;
  nbrRecycled   : UDINT;
  pRefrConnDataRoot : ^refrConnData;
  tcpServerState : UDINT;
  tcpAcceptState : UDINT;
#endif  
  myInstanceId  : UDINT;            // my instance id; at every project start, the instance id gets a new value
  myApplId  : UDINT;                // Application-ID, die beim Aufbau einer Comlink Verbindung zum Server verwendet wird
END_VAR

VAR_GLOBAL
  s_pCmdConn    : ^cmdConnBlock;
  s_pRefrConn   : ^refrConnBlock;
  pCmdConnRoot  : ^cmdConnBlock;
  pRefrConnRoot : ^refrConnBlock;
END_VAR

//
// forward declarations
//
FUNCTION GLOBAL __CDECL GetDataBufferFlag
VAR_INPUT
    pObj        : ^VirtualBase;
END_VAR
VAR_OUTPUT
    bSet        : BOOL;
END_VAR;

FUNCTION GLOBAL __CDECL PrototypeCallbackT
VAR_INPUT
    pComdefT : ^comdefT;
    pData    : ^d2LSE;
END_VAR;


//------------------------------------------------------------------------------
// GetMyInstanceId
//
//  Get a unique identifier for this instance. This identifier changes when the 
//  application is restarted.
//------------------------------------------------------------------------------
FUNCTION GetMyInstanceId
VAR_OUTPUT
    instId : UDINT;
END_VAR
VAR
    time_ : SYSTIME;
END_VAR

  instId := OS_ReadMicroSec();
  IF instId = 0 THEN
    // OS_ReadMicroSec is not supported under LARS under a standard PC
    OS_SSR_GetSysTime(#time_);
    instId := time_$UDINT;
  END_IF;

END_FUNCTION

(* Scannt einen UDINT Wert aus einem String.
  Das Ergebnis ist > 0 wenn der Wert erfolgreich geparst werden konnte, sonst 0.
*)
FUNCTION ScanUdint
  VAR_INPUT
    pScan 	: pChar;
    pValue 	: ^UDINT;
  END_VAR
  VAR_OUTPUT
    result 	: UDINT;
  END_VAR
  VAR
    value : UDINT;
    oldValue : UDINT;
    pFirstChar 	: pChar;
  END_VAR

  result := 0;
  pFirstChar := pScan;
  
  value := 0;
  WHILE (pScan^ >= '0' & pScan^ <= '9') DO 
    oldValue := value;
    value := value * 10 + pScan^ - '0';
    IF oldValue > value THEN
      (* overflow *)
      RETURN;
    END_IF;
    pScan += 1;
  END_WHILE;
  
  // Am Ende muss der String mit 0 terminiert sein
  IF pScan^ <> 0 THEN 
    RETURN;
  ELSE
    pValue^ := value;
  END_IF;
  
  result := pScan$UDINT - pFirstChar$UDINT;
    
END_FUNCTION
//------------------------------------------------------------------------------
// GetMyApplId
//------------------------------------------------------------------------------
FUNCTION GetMyApplId
VAR_OUTPUT
  applId : UDINT;
END_VAR
VAR
  pSysenv : ^OS_SYSENV;
  value : ARRAY[0..255] OF CHAR;
END_VAR

  applId := 0;
  OS_CILGet("SYSENV", #pSysenv$void);
  IF pSysenv & OS_GETENVVAR(pSysenv, "COMLINK_APPLID", #value[0], SIZEOF(value)) THEN
    ScanUdint(#value[0], #applId);
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// CloseSock
//------------------------------------------------------------------------------
FUNCTION CloseSock
VAR_INPUT
    sock         : ^DINT;
END_VAR

  IF sock^ <> INVALID_SOCKET_NUM THEN
    DEBUGLOG1("%010d-closing socket %d", sock^$UDINT);
    OS_TCP_USER_CLOSESOCKET(sock^, 0);
    IF sock^ < arraysize(usedsocks) THEN
      usedsocks[sock^] := ' ';
    END_IF;
    sock^ := INVALID_SOCKET_NUM;
  END_IF;
  
END_FUNCTION

//------------------------------------------------------------------------------
// InitBufferedSock
//------------------------------------------------------------------------------
FUNCTION InitBufferedSock
VAR_INPUT
    bsock         : ^bufferedSocket;
END_VAR

  _memset(bsock, 0, sizeof(bsock^));
  bsock^.sock         := INVALID_SOCKET_NUM;
  bsock^.pndSock      := INVALID_SOCKET_NUM;
  bsock^.sizeOfRxBuf  := SIZEOF(bsock^.rxBuf);
  bsock^.sizeOfTxBuf  := SIZEOF(bsock^.txBuf);
  
END_FUNCTION

//------------------------------------------------------------------------------
// ResetBufferedSock
//------------------------------------------------------------------------------
FUNCTION ResetBufferedSock
VAR_INPUT
    bsock         : ^bufferedSocket;
END_VAR
VAR
  isInstIdVerified_old : UDINT;
END_VAR

  isInstIdVerified_old := bsock^.isInstIdVerified;
  InitBufferedSock(bsock);
  IF isInstIdVerified_old = INSTID_MATCH THEN
    bsock^.isInstIdVerified := INSTID_NOT_VERIFIED;
  ELSE
    bsock^.isInstIdVerified := isInstIdVerified_old;
  END_IF;
  
END_FUNCTION

//------------------------------------------------------------------------------
// CloseBufferedSock
//------------------------------------------------------------------------------
FUNCTION CloseBufferedSock
VAR_INPUT
    bsock         : ^bufferedSocket;
END_VAR

  IF bsock^.pndSock <> INVALID_SOCKET_NUM THEN
    CloseSock(#bsock^.pndSock);
  END_IF;
  IF bsock^.sock <> INVALID_SOCKET_NUM THEN
    CloseSock(#bsock^.sock);
    ResetBufferedSock(bsock);
  END_IF;
  
END_FUNCTION

//------------------------------------------------------------------------------
// LoadUpdateList
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION LoadUpdateList
VAR_INPUT
    list          : ^updateList;
    nbrRefrCells  : UDINT;
END_VAR

  IF nbrRefrCells > 0 THEN
    IF nbrRefrCells > list^.nbrCells THEN
      nbrRefrCells := list^.nbrCells;
    END_IF;
  END_IF;
  
  list^.pActCell     := list^.pFirstCell;
  list^.actInd       := 0;
  list^.nbrRefrCells := nbrRefrCells;
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// MemoryUpdateList - allocates or reallocate memory for an update list
//------------------------------------------------------------------------------
FUNCTION MemoryUpdateList
VAR_INPUT
    list          : ^updateList;
    nbrCells      : UDINT;
    listType      : UDINT;        // 0=stat, 1=dyn
    fRealloc      : UDINT;        // 0=alloc, 1=realloc
END_VAR
VAR_OUTPUT
    pCells        : ^comStCell;  // pointer to cells
END_VAR

  IF listType = 0 THEN
    IF nbrCells > MAX_STATLIST_ENTRIES THEN
      pCells := NIL;
      RETURN;
    END_IF;
  ELSE
    IF nbrCells > MAX_DYNLIST_ENTRIES THEN
      pCells := NIL;
      RETURN;
    END_IF;
  END_IF;
  
  IF fRealloc = 0 THEN
    _memset(list, 0, sizeof(list^));  
    list^.pFirstCell := _alloc(SIZEOF(list^.pFirstCell^) * nbrCells)$^comStCell;
    IF list^.pFirstCell THEN
      IF listType = 0 THEN
        g_nbrStatList += 1;
      ELSE
        g_nbrDynList += 1;
      END_IF;
    END_IF;
  ELSE
    list^.pFirstCell := _realloc_LDR(list^.pFirstCell, SIZEOF(list^.pFirstCell^) * nbrCells)$^comStCell;
  END_IF;
  IF list^.pFirstCell <> NIL THEN
    IF nbrCells > list^.nbrCells THEN
      // init new allocated memory with zeros
      _memset(list^.pFirstCell + SIZEOF(list^.pFirstCell^) * list^.nbrCells, 
              0,
              SIZEOF(list^.pFirstCell^) * (nbrCells - list^.nbrCells)
              );
    END_IF;
    list^.nbrCells := nbrCells;
  ELSE
    list^.actInd := 0;
    list^.nbrCells := 0;
    list^.nbrRefrCells := 0;
  END_IF;
  list^.pActCell := list^.pFirstCell;
  
  pCells := list^.pFirstCell;
  
END_FUNCTION

//------------------------------------------------------------------------------
// Versucht die Update-List zu vergrößern
//------------------------------------------------------------------------------
FUNCTION IncreaseUpdateList
VAR_INPUT
    list          : ^updateList;
    minNbrCells   : UDINT;        // Mindestgröße der Liste
    listType      : UDINT;        // 0=stat, 1=dyn
END_VAR
VAR_OUTPUT
    pCells        : ^comStCell;  // pointer to cells
END_VAR
VAR
    limit : UDINT;
END_VAR

  IF listType = 0 THEN
    limit := MAX_STATLIST_ENTRIES;
  ELSE
    limit := MAX_DYNLIST_ENTRIES;
  END_IF;

  IF minNbrCells < limit THEN           // wenn das limit noch nicht erreicht ist, 
    minNbrCells := minNbrCells + 300;   // dann wird versucht mehr zu allokieren,
    IF minNbrCells > limit THEN         // aber nicht mehr als das limit
      minNbrCells := limit;
    END_IF;
  END_IF;

  pCells := MemoryUpdateList(list, minNbrCells, listType, fRealloc := 1);
  
END_FUNCTION

//------------------------------------------------------------------------------
// AllocateLoginContext
//
//  Search a free entry in loginArray and allocate a new login context.
//  When no free entry exists or when the specified address is already in use, 
//  the function fails.
//
// Return value
//  If the functin succeeds, a pointer to the context is returned.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION AllocateLoginContext
VAR_INPUT
    ipAddrSrvr  : UDINT;       // ip-address of the server
    cmdPortSrvr : UDINT;       // port number of the servers command port
END_VAR
VAR_OUTPUT
    pLogin       : ^loginContext;
END_VAR
VAR
    pStatCells  : ^comStCell;
    pDynCells   : ^comStCell;
    mutexName   : ARRAY[0..30] OF CHAR;
END_VAR

  pStatCells := NIL;
  pDynCells  := NIL;

  pLogin := calloc(SIZEOF(loginContext))$^loginContext;
  IF pLogin = NIL THEN
    RETURN;
  END_IF;
  
  // initialize the new connection context
  pStatCells := MemoryUpdateList(#pLogin^.statList, INIT_STATLIST_ENTRIES, 0, 0);
  pDynCells  := MemoryUpdateList(#pLogin^.dynList, INIT_DYNLIST_ENTRIES, 1, 0);
  _strcpy(#mutexName[0], "LOGIN_SEMA_");
  MakeResourceName(pLogin$UDINT, #mutexName[0]);
  pLogin^.hMutex := GetMutexFromPool(TRUE(*counting*));
  IF pLogin^.hMutex = NIL THEN
    pLogin^.hMutex := OS_MT_CreateSemaphore(MTSEMATYPE_COUNTING, 
                                            1,    // initvalue
                                            0,    // flags: 0 means create unconitionally
                                            #mutexName[0]
                                            );
  END_IF;                                            
  IF (pStatCells <> NIL) & (pDynCells <> NIL) & (pLogin^.hMutex <> NIL) THEN
    pLogin^.ipAddrSrvr  := ipAddrSrvr;
    pLogin^.cmdPortSrvr := cmdPortSrvr;
    InitBufferedSock(#pLogin^.cmdSockClnt);
    InitBufferedSock(#pLogin^.refrSockClnt);
    pLogin^.pCallback := g_pCallback;
    pLogin^.refCnt := 1;
    pLogin^.hComchMutex := NewComchMutex(pLogin$UDINT, 't');
    pLogin$^ComData_Base^.typeOfStruct := STRUCT_TYPE_LOGINCONTEXT;

    pLogin^.sizeofStatList := MAX_STATLIST_ENTRIES;
    pLogin^.sizeofDynList  := MAX_DYNLIST_ENTRIES;
    
    pLogin^.pDynCallbacks := NIL;

     // Die Refreshlisten-Größen der Remote Station werden vorerst mit der Größe der eigenen 
     // initialisiert. Wenn beim Login ein anderer Wert gemeldet wird, dann wird diese 
     // Variable noch einmal überschrieben
    pLogin^.sizeofRemoteDynList  := pLogin^.sizeofDynList;
    pLogin^.sizeofRemoteStatList := pLogin^.sizeofStatList;

    DEBUGLOG0("%010d-  =====  Login context allocated  =====");
    g_nbrLoginCtx += 1;
  ELSE
    // cleanup
    IF pLogin^.hMutex <> NIL THEN
      IF !!PutMutexToPool(TRUE(*counting*), pLogin^.hMutex) THEN
        OS_MT_DeleteSemaphore(pLogin^.hMutex);
      END_IF;
    END_IF;
    IF pStatCells <> NIL THEN
      _free(pStatCells);
      pStatCells := NIL;
      g_nbrStatList -= 1;
    END_IF;
    IF pDynCells <> NIL THEN
      _free(pDynCells);
      pDynCells := NIL;
      g_nbrDynList -= 1;
    END_IF;
    _free(pLogin);
    pLogin := NIL;
  END_IF;

  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ReleaseLoginContext
//
//  Release all resources of a used login context.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ReleaseLoginContext
VAR_INPUT
    pLogin       : ^loginContext;
END_VAR
VAR
  pDynCallback      : ^sDynCallbackList;
  pDynCallbackNext  : ^sDynCallbackList;
END_VAR

  IF pLogin = NIL THEN
    RETURN;
  END_IF;

  IF pLogin^.hMutex <> NIL THEN
    IF !!PutMutexToPool(TRUE(*counting*), pLogin^.hMutex) THEN
      OS_MT_DeleteSemaphore(pLogin^.hMutex);
    END_IF;
  END_IF;
  DeleteComchMutex(#pLogin^.hComchMutex);

  IF pLogin^.pNode <> NIL THEN
    RemoveMonitorNode(pLogin^.pNode);
  END_IF;

  IF pLogin^.pComdefT THEN
    pLogin^.pComdefT^.pLogin := NIL;
  END_IF;

  // close open sockets
  CloseBufferedSock(#pLogin^.cmdSockClnt);
  CloseBufferedSock(#pLogin^.refrSockClnt);
  
  // free allocated memory
  _free(pLogin^.statList.pFirstCell);
  g_nbrStatList -= 1;
  _free(pLogin^.dynList.pFirstCell);
  g_nbrDynList -= 1;
  pLogin^.statList.pFirstCell := NIL;
  pLogin^.dynList.pFirstCell := NIL;
  
  
  IF (pLogin^.pDynCallbacks <> NIL) THEN
    
     pDynCallback := pLogin^.pDynCallbacks;
     pDynCallbackNext := pDynCallback^.pNext;

     // ersten callback freigeben
     _free(pDynCallback^.pCallback);
     _free(pDynCallback);

     // restliche freigeben     
     WHILE (pDynCallbackNext <> NIL) DO
         pDynCallback := pDynCallbackNext;
         pDynCallbackNext := pDynCallback^.pNext;
         _free(pDynCallback^.pCallback);
         _free(pDynCallback);
     END_WHILE;
    
     pLogin^.pDynCallbacks := NIL;
  END_IF;
  
  _free(pLogin);
  pLogin := NIL;
  
  DEBUGLOG0("%010d-  =====  Login context released  =====");
  g_nbrLoginCtx -= 1;
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// LockLoginContext
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION LockLoginContext
VAR_INPUT
    index       : UDINT;
END_VAR
VAR_OUTPUT
    pLogin      : ^loginContext;
END_VAR

  // Mit dieser Abfrage wird der OVerhead von OS_MT_Wait/OS_MT_Signal vermieden, 
  // wenn niemand angemeldet ist.
  IF loginArray[index] = NIL THEN
    pLogin := NIL;
    RETURN;
  END_IF;

  OS_MT_Wait(loginArrayMutex);
  pLogin := loginArray[index];
  IF pLogin <> NIL THEN
    pLogin^.refCnt += 1;    
  END_IF;
  OS_MT_Signal(loginArrayMutex);

END_FUNCTION

FUNCTION LockLoginContext2
VAR_INPUT
    pLogin      : ^loginContext;
END_VAR

  OS_MT_Wait(loginArrayMutex);
  pLogin^.refCnt += 1;    
  OS_MT_Signal(loginArrayMutex);

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// UnlockLoginContext
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION UnlockLoginContext
VAR_INPUT
    pLogin      : ^loginContext;
END_VAR
VAR
    i           : UDINT;
END_VAR

  // Mit dieser Abfrage wird der OVerhead von OS_MT_Wait/OS_MT_Signal vermieden, 
  // wenn pLogin NIL ist ist.
  IF pLogin = NIL THEN
    RETURN;
  END_IF;

  OS_MT_Wait(loginArrayMutex);
    pLogin^.refCnt -= 1;
    IF pLogin^.refCnt <= 0 THEN
      // search the connection context in loginArray
      FOR i := 0 TO (g_max_logins - 1) DO
        IF pLogin = loginArray[i] THEN
          // found
          loginAnz -= 1;
          loginArray[i] := NIL;
        END_IF;
      END_FOR;
      ReleaseLoginContext(pLogin);
    END_IF;
  OS_MT_Signal(loginArrayMutex);

END_FUNCTION
#endif

(*
  Führt einen _Lock auf das hComchMutex durch, welches verwendet wird um die 
  comdata/loginContext/loginContextR Struktur zu schützen. 
  Wenn es sich um eine loginContext Struktur handelt, die dynamische allokiert 
  wird, dann wird zusätzlich der refCnt erhöht, damit während des Locks der 
  Speicher nicht freigegeben wird.
*)
FUNCTION GetAndLockComdata
VAR_INPUT
  pComdef : ^COMDEF;
END_VAR
VAR_OUTPUT
	pt_COM   : ^ComData_Base;
END_VAR

  pt_COM := NIL;

  IF pComdef = NIL THEN
    RETURN;
  END_IF;

  OS_MT_Wait(loginArrayMutex);
  // Es ist notwendig, dass auf pComdef^.pt_Com innerhalb des mit loginArrayMutex 
  // gesperrten Abschnitts zugegriffen wird, weil in ReleaseLoginContext das pt_Com auf
  // NIL gesetzt werden kann.
  pt_COM := pComdef^.pt_Com$^ComData_Base;
  IF (pt_COM <> NIL) & (pt_COM^.typeOfStruct = STRUCT_TYPE_LOGINCONTEXT) THEN
    pt_COM$^loginContext^.refCnt += 1;    
  END_IF;
  OS_MT_Signal(loginArrayMutex);

  IF pt_COM <> NIL THEN
    _Lock(pt_COM^.hComchMutex);
  END_IF;

END_FUNCTION

(*
  Führt einen _Unlock auf das hComchMutex durch, und macht im Falle einer 
  loginContext Struktur einen UnlockLoginContext Aufruf, in dem die 
  Struktur freigegeben wird, wenn der rerCnt <= 0 wird.
*)
FUNCTION ReleaseComdata
VAR_INPUT
	pt_COM   : ^ComData_Base;
END_VAR

  _Unlock(pt_COM^.hComchMutex);
  IF pt_COM^.typeOfStruct = STRUCT_TYPE_LOGINCONTEXT THEN
    UnlockLoginContext(pt_COM$^loginContext);
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// AllocConnBlock
//
//  Allocate a new data structure for a connection and add it to the linked list.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION AllocConnBlock
VAR_INPUT
    typ         : UDINT;           // type of the data structure
    sock        : DINT;            // socket number
END_VAR
VAR_OUTPUT
    pConn       : ^connBlock;
END_VAR
VAR
    pStatCells  : ^comStCell;
    pDynCells   : ^comStCell;
END_VAR

  // allocate generic connection block
  pConn := calloc(SIZEOF(connBlock))$^connBlock;
  IF pConn = NIL THEN
    DEBUGLOG0("%010d-alloc of connBlock failed");
    RETURN;
  END_IF;
  
  // allocate non-generic connection block
  CASE typ OF
    TYPE_CMD_CONN:
      IF nbrCmdConn > max_conns THEN
        DEBUGLOG0("%010d-# of max.connections exceeded");
        _free(pConn);
        pConn := NIL;
        RETURN;
      END_IF;
      pConn^.ptr := calloc(SIZEOF(cmdConnData))$^cmdConnData;
    TYPE_REFR_CONN:
      IF nbrRefrConn > max_conns THEN
        DEBUGLOG0("%010d-# of max.connections exceeded");
        _free(pConn);
        pConn := NIL;
        RETURN;
      END_IF;
      pConn^.ptr := calloc(SIZEOF(refrConnData))$^refrConnData;
  END_CASE;
  
  IF pConn^.ptr <> NIL THEN
    // initialize the new connection block
    InitBufferedSock(#pConn^.bsock);
    pConn^.bsock.sock := sock;
    
    OS_TCP_USER_GETPEERIP(sock, #pConn^.ipAddrPeer);
    pConn^.connectTime := ops.tAbsolute;

    CASE typ OF
      TYPE_CMD_CONN:
#ifndef DUMMY_LOADER
        pConn^.ptr^$cmdConnData.com.InitCode(FALSE); // initialize ip object
#endif        
      TYPE_REFR_CONN:
        pStatCells := MemoryUpdateList(#pConn^.ptr^$refrConnData.statList, INIT_STATLIST_ENTRIES, 0, 0);
        pDynCells  := MemoryUpdateList(#pConn^.ptr^$refrConnData.dynList,  INIT_DYNLIST_ENTRIES,  1, 0);
        IF (pStatCells = NIL) | (pDynCells = NIL) THEN
          IF pStatCells <> NIL THEN
            DEBUGLOG0("%010d-alloc of dynList failed");
            _free(pStatCells);
            pStatCells := NIL;
            g_nbrStatList -= 1;
          END_IF;
          IF pDynCells <> NIL THEN
            DEBUGLOG0("%010d-alloc of statList failed");
            _free(pDynCells);
            pDynCells := NIL;
            g_nbrDynList -= 1;
          END_IF;
          _free(pConn^.ptr);
          pConn^.ptr := NIL;
        END_IF;
    END_CASE;
  END_IF;

  IF pConn^.ptr = NIL THEN
    // cleanup
    _free(pConn);
    pConn := NIL;
  ELSE
    // add block to the linked list
    CASE typ OF
      TYPE_CMD_CONN:
        pConn^.next := pCmdConnRoot$^connBlock;
        pCmdConnRoot := pConn$^cmdConnBlock;
        nbrCmdConn += 1;
      TYPE_REFR_CONN:
        pConn^.next := pRefrConnRoot$^connBlock;
        pRefrConnRoot := pConn$^refrConnBlock;
        nbrRefrConn += 1;
    END_CASE;
    DEBUGLOG0("%010d-Connection block allocated");
    g_nbrConnBlk += 1;
  END_IF;
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// FreeRefrConnData - free all resources of a refresh connection data block
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION FreeRefrConnData
VAR_INPUT
    pData : ^refrConnData;
END_VAR

  if( pData = NIL )then
    return;
  end_if;

  IF pData^.statList.pFirstCell <> NIL THEN
    _free(pData^.statList.pFirstCell);
    pData^.statList.pFirstCell := NIL;
    g_nbrStatList -= 1;
  END_IF;
  IF pData^.dynList.pFirstCell <> NIL THEN
    _free(pData^.dynList.pFirstCell);
    pData^.dynList.pFirstCell := NIL;
    g_nbrDynList -= 1;
  END_IF;
  _free(pData);
  pData := NIL;
  
END_FUNCTION  
#endif

//------------------------------------------------------------------------------
// ReleaseRefrConnData
//
//  Put a refresh connection data block into the recycle pool for later reuse 
//  when the same client reconnets.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION ReleaseRefrConnData
VAR_INPUT
    pData     : ^refrConnData;
    reuse : dint;
END_VAR
VAR
    pDataWork : ^refrConnData;
    pDataPrev : ^refrConnData;
END_VAR

  if( reuse = 0 )then
    LoadUpdateList(#pData^.statList, pData^.statList.nbrRefrCells);
    LoadUpdateList(#pData^.dynList,  pData^.dynList.nbrRefrCells);
    return; //irgendwas haut da beim reusen nit hin, aber das ist eh egal -> logout
  end_if;
  
  IF nbrRecycled >= max_conns THEN
    // free the oldest connection data block in the recycle pool
    pDataPrev := NIL;
    pDataWork := pRefrConnDataRoot;
    WHILE pDataWork <> NIL DO
      IF pDataWork^.next = NIL THEN
        // this is the last element in the list
        IF pDataPrev = NIL THEN
          pRefrConnDataRoot := NIL;
        ELSE
          pDataPrev^.next := NIL;
        END_IF;
        FreeRefrConnData(pDataWork);
        nbrRecycled -= 1;
        DEBUGLOG0("%010d-Refresh connection data block freed");
        EXIT;
      END_IF;
      pDataPrev := pDataWork;
      pDataWork := pDataWork^.next;
      if( pDataWork = pDataWork^.next )then
        pDataWork^.next := NIL;
      end_if;
    END_WHILE;
  END_IF;
  
  pData^.next := pRefrConnDataRoot;
  pRefrConnDataRoot := pData;
  // invalidate all update-cells
  LoadUpdateList(#pData^.statList, pData^.statList.nbrRefrCells);
  LoadUpdateList(#pData^.dynList,  pData^.dynList.nbrRefrCells);
  nbrRecycled += 1;
  DEBUGLOG0("%010d-Refresh connection data block put into recycle pool");
  DEBUGLOG2("%010d- statList, nbrCells=%d, nbrRefrCells=%d", pData^.statList.nbrCells, pData^.statList.nbrRefrCells);
  DEBUGLOG2("%010d- dynList,  nbrCells=%d, nbrRefrCells=%d", pData^.dynList.nbrCells,  pData^.dynList.nbrRefrCells);
  
END_FUNCTION  
#endif

//------------------------------------------------------------------------------
// ReuseRefrConnData
//
//  Search a refresh connection data block in the recycle pool. When found, 
//  remove the block from the pool and return it to the caller.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION ReuseRefrConnData
VAR_INPUT
    instIdOfPeer : UDINT;           // instance id of the peer
    sessIdOfPeer : UDINT;           // session id of the peer
END_VAR
VAR_OUTPUT
    pData     : ^refrConnData;
END_VAR
VAR
    pDataWork : ^refrConnData;
    pDataPrev : ^refrConnData;
END_VAR

  pData := NIL;
  
  pDataPrev := NIL;
  pDataWork := pRefrConnDataRoot;
  WHILE pDataWork <> NIL DO
    IF (pDataWork^.instIdOfPeer = instIdOfPeer) &
       (pDataWork^.sessIdOfPeer = sessIdOfPeer) THEN
      // found
      pData := pDataWork;
      // remove it from the recycle pool
      IF pDataPrev = NIL THEN
        pRefrConnDataRoot  := pDataWork^.next;
      ELSE
        pDataPrev^.next := pDataWork^.next;
      END_IF;
      nbrRecycled -= 1;
      DEBUGLOG0("%010d-Refresh connection data block removed from recycle pool");
      EXIT;
    END_IF;
    pDataPrev := pDataWork;
    if( pDataWork = pDataWork^.next )then
      pData := NIL;
      exit;
    end_if;
    pDataWork := pDataWork^.next;
  END_WHILE;
  
END_FUNCTION  
#endif

//------------------------------------------------------------------------------
// ReleaseConnBlock - release a connection block
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION ReleaseConnBlock
VAR_INPUT
    typ         : UDINT;           // type of the data structure
    pConn       : ^connBlock;
    reuse       : dint;
END_VAR
VAR
    pConnTmp    : ^connBlock;
    pConnPrev   : ^connBlock;
END_VAR

  if( pConn = NIL )then
    return;
  end_if;

  CASE typ OF
    TYPE_CMD_CONN:
      pConnTmp := pCmdConnRoot$^connBlock;
    TYPE_REFR_CONN:
      pConnTmp := pRefrConnRoot$^connBlock;
  END_CASE;

  pConnPrev := NIL;
  WHILE pConnTmp <> NIL DO
    IF pConnTmp = pConn THEN
      // close open sockets
      CloseBufferedSock(#pConn^.bsock);
      // cleanup of non-generic block
      CASE typ OF
        TYPE_CMD_CONN:
          nbrCmdConn -= 1;
#ifndef DUMMY_LOADER
#ifdef LOADER_4_LASAL2          
          _free(pConn^.ptr^$cmdConnData.com._m.head.pcode);  // todo: A DebugIp object should provide a destruktor !
          pConn^.ptr^$cmdConnData.com._m.head.pcode := NIL;
#else
          _free(pConn^.ptr^$cmdConnData.com.head.pcode);  // todo: A DebugIp object should provide a destruktor !
          pConn^.ptr^$cmdConnData.com.head.pcode := NIL;
#endif
          pConn^.ptr^$cmdConnData.com.dtor();
#endif
          IF pConn$^cmdConnBlock = s_pCmdConn THEN
            s_pCmdConn := NIL;
          END_IF;
          DEBUGLOG0("%010d-  =====  Command connection block released  =====");
        TYPE_REFR_CONN:
          nbrRefrConn -= 1;
          ReleaseRefrConnData(pConn^.ptr$^refrConnData,reuse);
          if( reuse = 0 )then
            if( pConn^.ptr$^refrConnData^.statList$^void <> NIL )then
              _free( pConn^.ptr$^refrConnData^.statList$^void );
              pConn^.ptr$^refrConnData^.statList$^void := NIL;
              g_nbrStatList -= 1;
            end_if;
            if( pConn^.ptr$^refrConnData^.dynList$^void <> NIL )then
              _free( pConn^.ptr$^refrConnData^.dynList$^void );
              pConn^.ptr$^refrConnData^.dynList$^void := NIL;
              g_nbrDynList -= 1;
            end_if;
          end_if;
          IF pConn$^refrConnBlock = s_pRefrConn THEN
            s_pRefrConn := NIL;
          END_IF;
          DEBUGLOG0("%010d-  =====  Refresh connection block released  =====");
      END_CASE;
      // unlink block
      IF pConnPrev = NIL THEN
        CASE typ OF
          TYPE_CMD_CONN:
            pCmdConnRoot  := pConn^.next$^cmdConnBlock;
          TYPE_REFR_CONN:
            pRefrConnRoot := pConn^.next$^refrConnBlock;
        END_CASE;
      ELSE
        pConnPrev^.next := pConn^.next;
      END_IF;
      // free block
      if( reuse = 0 )then
        if( pConn^.ptr <> NIL )then
          _free( pConn^.ptr );
          pConn^.ptr := NIL;
        end_if;
      end_if;
      _free(pConn);
      pConn := NIL;
      g_nbrConnBlk -= 1;
      EXIT;
    END_IF;
    pConnPrev := pConnTmp;
    pConnTmp  := pConnTmp^.next;
  END_WHILE;  
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// Connect
//
//  Establish a connection to a specified port
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION Connect
VAR_INPUT
    sock          : DINT;       // socket number
    ipAddr        : UDINT;      // ip-address
    port          : UDINT;      // port number
    timeout_ms    : UDINT;      // timeout, in milliseconds
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    ipAddrString  : ARRAY[0..15] of CHAR;
END_VAR    

  OS_TCP_USER_TOIP(#ipAddrString[0], 
                   sizeof(ipAddrString), 
                    ipAddr         AND 16#FF, 
                   (ipAddr SHR  8) AND 16#FF, 
                   (ipAddr SHR 16) AND 16#FF, 
                   (ipAddr SHR 24)
                   );
                           
  retval := OS_TCP_USER_CONNECT(sock, 
                                0,
                                #ipAddrString[0], 
                                port, 
                                timeout_ms
                                );
#ifdef DEBUG                                
  IF (retval < 0) & (retval <> TCP_NOT_READY) THEN
    DEBUGLOG3("%010d-OS_TCP_USER_CONNECT(addr:%s,port:%d) failed, rc=%d", (#ipAddrString[0])$UDINT, TO_UDINT(port), TO_UDINT(retval));
  END_IF;
#endif  
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// SendNUnbuffered
//
//  Send n bytes of data (unbuffered).
//
// Return value:
//  If the function succeeds, the return value is the number of bytes sent which 
//  is equal to the number specified in parameter len.
//  
//  If the function fails, the return value is equal or less than zero.
//  A value of zero means that the connection is closed.
//
// Remarks:
//  Do not mix unbuffered (SendNBuffered) and buffered sends (SendNUnbuffered) !
//  Do not use a timeout value of zero, this causes a loop without a delay !
//------------------------------------------------------------------------------
FUNCTION SendNUnbuffered
VAR_INPUT
    bsock         : ^bufferedSocket;
    buf           :^CHAR;
    len           : UDINT;
    timeout_ms    : UDINT;      // timeout, in milliseconds
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    rc            : DINT;
    startTime_ms  : UDINT;
END_VAR;    

  retval := len$DINT;
  startTime_ms := ops.tAbsolute;
  
  WHILE len > 0 DO     
  
    rc := OS_TCP_USER_SEND(bsock^.sock, 
                           buf, 
                           len, 
                           0,
                           timeout_ms
                           );
         
    IF rc = TCP_NOT_READY THEN
      OS_MT_Delay(1);
    END_IF;
                           
    IF rc > 0 THEN
      bsock^.lastSendTime_ms := ops.tAbsolute;
      DEBUG_VERBOSE2("%010d-OS_TCP_USER_SEND(sock %d): %d bytes sent", bsock^.sock$UDINT, TO_UDINT(rc));
      len -= TO_UDINT(rc);
    ELSE
      IF rc <> TCP_NOT_READY THEN
        retval := rc;
        DEBUGLOG2("%010d-SendNUnbuffered: OS_TCP_USER_SEND(sock %d) failed, rc=%d", TO_UDINT(bsock^.sock), TO_UDINT(retval));
        CloseBufferedSock(bsock);
        EXIT;
      
      ELSIF ops.tAbsolute - startTime_ms > timeout_ms THEN
        retval := TCP_TIMEOUT;
        DEBUGLOG2("%010d-SendNUnbuffered: timeout(sock %d), rc=%d", TO_UDINT(bsock^.sock), TO_UDINT(retval));
        CloseBufferedSock(bsock);
        EXIT;
        
      END_IF;
    END_IF;                           
    
  END_WHILE;
  
END_FUNCTION

#ifdef DEBUG
//------------------------------------------------------------------------------
// ChkBuffer
//------------------------------------------------------------------------------
FUNCTION ChkBuffer
VAR_INPUT
    bsock         : ^bufferedSocket;
END_VAR
VAR
    pTmp  : ^UDINT;
END_VAR

  IF (bsock^.bytesInRxBuf > arraysize(bsock^.rxBuf)) | 
     (bsock^.bytesInTxBuf > arraysize(bsock^.txBuf)) THEN
    pTmp$^void := NIL;
    pTmp^ := 1;
  END_IF;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// SendNBuffered
//
//  This function sends or buffers n bytes of data.
//
// Return value
//  If the function succeeds, the return value is the number of bytes sent or 
//  bufferd which is equal to the number specified in parameter len.
//
// Remarks
//  This function never blocks. Either all data or nothing is sent or queued. 
//  The buffering of data is used because TCP/IP performs better when one big 
//  block of data is sent instead of multiple small blocks.
//  Setting len to zero or buf to NIL and fFlush to 1 causes a flush of the 
//  buffer without sending any new data.
//------------------------------------------------------------------------------
FUNCTION SendNBuffered
VAR_INPUT
    bsock         : ^bufferedSocket;
    buf           : ^CHAR;
    len           : UDINT;
    timeout_ms    : UDINT;      // timeout, in milliseconds
    fFlush        : UDINT;      // 0=don't send, write to buffer instead
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    bytesFree     : UDINT;  // total number of free bytes in the buffer
    endInd        : UDINT;  // zero-based index of the first free byte after the used block
    endFree       : UDINT;  // number of free bytes after the used block
    bytesSent     : UDINT;  // number of bytes sent
    rc            : DINT;
END_VAR

  #ifdef DEBUG
  ChkBuffer(bsock);
  #endif

  retval := len$DINT;

  // First check if there is enough free space in the buffer. When the free 
  // space is less than len, then the function fails, because we can not 
  // guarantee that all data can be written into the buffer when tcp-send 
  // indicates that it is not ready.
  bytesFree := bsock^.sizeOfTxBuf - bsock^.bytesInTxBuf;
  IF bytesFree < len THEN
    retval := ERR_BUFFER_SHORTAGE;
    RETURN;
  END_IF;

  endInd  := bsock^.txBufStrtInd + bsock^.bytesInTxBuf;
  endFree := bsock^.sizeofTxBuf - endInd;

  // when the buffer is not empty or fFlush is not set:
  //  - append all data to the buffer.
  //  - send data when fFlush is set
  // otherwise the buffer is empty and fFlush is set:
  //  - try to send data and append the data not sent to the buffer.

  IF (bsock^.bytesInTxBuf > 0) | (fFlush = 0) THEN
    // the buffer is not empty or fFlush is not set:
    
    IF (len <> 0) & (buf <> NIL) THEN
      // append all data to the buffer
      IF endFree < len THEN
        // move the data in txBuf to the front of the buffer
        _memmove(#bsock^.txBuf[0], #bsock^.txBuf[bsock^.txBufStrtInd], bsock^.bytesInTxBuf);
        bsock^.txBufStrtInd := 0;
        endInd  := bsock^.bytesInTxBuf;
        endFree := bytesFree;
      END_IF;
      _memcpy(#bsock^.txBuf[endInd], buf, len);
      bsock^.bytesInTxBuf += len;
      endInd  += len;
      endFree -= len;
    END_IF;

    // try to send all data in buffer when fFlush is set
    IF fFlush <> 0 THEN
      bsock^.bytesToSend := bsock^.bytesInTxBuf;
      rc := OS_TCP_USER_SEND(bsock^.sock, 
                             #bsock^.txBuf[bsock^.txBufStrtInd], 
                             bsock^.bytesInTxBuf, 
                             0,
                             timeout_ms
                             );
      IF rc > 0 THEN
        bsock^.lastSendTime_ms := ops.tAbsolute;
        DEBUG_VERBOSE2("%010d-OS_TCP_USER_SEND(sock %d): %d bytes sent", bsock^.sock$UDINT, TO_UDINT(rc));
        // data sent, adjust buffer variables
        bsock^.lastSendTime := ops.tAbsolute;
        bsock^.bytesInTxBuf -= TO_UDINT(rc);
        IF TO_UDINT(rc) > bsock^.bytesToSend THEN
          bsock^.bytesToSend := 0;
        ELSE
          bsock^.bytesToSend  -= TO_UDINT(rc);
        END_IF;
        IF bsock^.bytesInTxBuf = 0 THEN
          bsock^.txBufStrtInd := 0;
        ELSE
          bsock^.txBufStrtInd += TO_UDINT(rc);
        END_IF;
      ELSE
        // no data sent, check for an error
        IF rc <> TCP_NOT_READY THEN
          retval := rc;
          DEBUGLOG2("%010d-SendNBuffered: OS_TCP_USER_SEND(sock %d) failed, rc=%d", TO_UDINT(bsock^.sock), TO_UDINT(retval));
          CloseBufferedSock(bsock);
          RETURN;
        END_IF;
      END_IF;                             
    END_IF;                           
    
  ELSE
    // the buffer is empty and fFlush is set:
    
    IF (len = 0) | (buf = NIL) THEN
      // no data to send
      RETURN;
    END_IF;
    
    // try to send data
    rc := OS_TCP_USER_SEND(bsock^.sock, 
                           buf, 
                           len, 
                           0,
                           timeout_ms
                           );
    IF (rc > 0) | (rc = TCP_NOT_READY) THEN
      // no error
    
      IF rc > 0 THEN
        bsock^.lastSendTime_ms := ops.tAbsolute;
        DEBUG_VERBOSE2("%010d-OS_TCP_USER_SEND(sock %d): %d bytes sent", bsock^.sock$UDINT, TO_UDINT(rc));
        bsock^.lastSendTime := ops.tAbsolute;
      END_IF;
    
      IF TO_UDINT(rc) <> len THEN
        // not all data sent, append it to the buffer
        IF rc = TCP_NOT_READY THEN
          bytesSent := 0;
        ELSE
          bytesSent := TO_UDINT(rc);
        END_IF;
        
        _memcpy(#bsock^.txBuf[0], buf + bytesSent, len - bytesSent);
        bsock^.bytesInTxBuf := len - bytesSent;
        bsock^.bytesToSend  := len - bytesSent;
      END_IF;
      
    ELSE
      // error
      retval := rc;
      DEBUGLOG2("%010d-SendNBuffered: OS_TCP_USER_SEND(sock %d) failed, rc=%d", TO_UDINT(bsock^.sock), TO_UDINT(retval));
      CloseBufferedSock(bsock);
      RETURN;
    END_IF;                             
      
  END_IF;
  
  #ifdef DEBUG
  ChkBuffer(bsock);
  #endif

END_FUNCTION

//------------------------------------------------------------------------------
// SendUnsentData
//
//  This function tries to send data in a buffer when there is some data in the 
//  buffer marked as unsent.
//
// Return value
//  >= 0  .. Nbr of bytes send
//  <0    .. tcp-user error code
//------------------------------------------------------------------------------
FUNCTION SendUnsentData
VAR_INPUT
    bsock         : ^bufferedSocket;
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    rc            : DINT;
END_VAR

  #ifdef DEBUG
  ChkBuffer(bsock);
  #endif

  retval := 0;
  
  IF bsock^.bytesToSend > 0 THEN
    
    rc := OS_TCP_USER_SEND(bsock^.sock, 
                           #bsock^.txBuf[bsock^.txBufStrtInd], 
                           bsock^.bytesInTxBuf, 
                           0,
                           0      // timeout_ms: 0 means no blocking
                           );
    IF rc > 0 THEN
      bsock^.lastSendTime_ms := ops.tAbsolute;
      DEBUG_VERBOSE2("%010d-OS_TCP_USER_SEND(%d): %d bytes sent", bsock^.sock$UDINT, TO_UDINT(rc));
      // data sent, adjust buffer variables
      bsock^.lastSendTime := ops.tAbsolute;
      bsock^.bytesInTxBuf -= TO_UDINT(rc);
      IF TO_UDINT(rc) > bsock^.bytesToSend THEN
        bsock^.bytesToSend := 0;
      ELSE
        bsock^.bytesToSend  -= TO_UDINT(rc);
      END_IF;
      IF bsock^.bytesInTxBuf = 0 THEN
        bsock^.txBufStrtInd := 0;
      ELSE
        bsock^.txBufStrtInd += TO_UDINT(rc);
      END_IF;
    ELSE
      // no data sent, check for an error
      IF rc <> TCP_NOT_READY THEN
        retval := rc;
        DEBUGLOG2("%010d-SendUnsentData: OS_TCP_USER_SEND(sock %d) failed, rc=%d", TO_UDINT(bsock^.sock), TO_UDINT(retval));
        CloseBufferedSock(bsock);
        RETURN;
      END_IF;
    END_IF;                             
      
  END_IF;

  #ifdef DEBUG
  ChkBuffer(bsock);
  #endif
  
END_FUNCTION

//------------------------------------------------------------------------------
// SendCmdMsg
//
//  This function sends a message. The caller has to provide a buffer that 
//  includes space for the command message header before the actual message 
//  data. The header in this buffer can be uninitialized. 
//
// Return value
//  If the function succeeds, the return value is the number of bytes sent or 
//  queued.
//
// Remarks
//  When timeout_ms is set to a value other than zero, this function will block 
//  until all data is sent or a failure occurs. Otherwise the function does not 
//  block and data that could not be sent is placed in a buffer. This buffer 
//  has to be processed by any other cyclic function.
//------------------------------------------------------------------------------
FUNCTION SendCmdMsg
VAR_INPUT
    bsock         : ^bufferedSocket;
    cmd           : UDINT;      // command of the message
    type_ack      : UDINT;      // Type and Ack field of command
    msg           : ^cMessage;  // the message
    len           : UDINT;      // length of the message incl. header
    timeout_ms    : UDINT;      // timeout, in milliseconds
    useBuffer     : UDINT;      // flag, that indicates if send is bufferd
    fFlush        : UDINT;
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR

#ifdef DEBUG
  IF len < SIZEOF(cmdMsgHdr) THEN
    DEBUGLOG2("%010d-SendCmdMsg(sock %d): invalid len(%d)", TO_UDINT(bsock^.sock), TO_UDINT(len));
  END_IF;
#endif

  msg^.cHdr.vHdr.dataLen := len - SIZEOF(msg^.cHdr.vHdr);
  msg^.cHdr.vHdr.res     := 0x0000;
  msg^.cHdr.vHdr.id      := 0xFACE;
  msg^.cHdr.cmd          := (cmd AND MASK_CMD_CODE) OR type_ack;

  IF useBuffer THEN
    retval := SendNBuffered(bsock, msg$^CHAR, len, timeout_ms, fFlush);
  ELSE
    retval := SendNUnbuffered(bsock, msg$^CHAR, len, timeout_ms);
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// RecvBuf
//
//  This function reads data from a connection. The data is buffered. A tcp-recv 
//  operation receives as much data as possible into the internal rx buffer. 
//  Data that is not needed by the caller remains in the internal rx buffer. 
//  This techniqe is used to avoid multiple small reads from the tcp/ip 
//  connection. 
//
// Return value
//  If the function succeeds, the return value is the number of bytes read, 
//  otherwise a tcp-user error code is returned.
//------------------------------------------------------------------------------
FUNCTION RecvBuf
VAR_INPUT
    bsock         : ^bufferedSocket;
    buf           : ^CHAR;
    len           : UDINT;
    timeout_ms    : UDINT;      // timeout, in milliseconds
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    reqMsg        : msgNopReq;
END_VAR    

  #ifdef DEBUG
  ChkBuffer(bsock);
  #endif

  IF bsock^.bytesInRxBuf = 0 THEN
    // The receive buffer is empty
    
    // Receive as much as possible into the rx buffer
    retval := OS_TCP_USER_RECV(bsock^.sock,
                               #bsock^.rxBuf[0],
                               bsock^.sizeOfRxBuf,
                               0,
                               timeout_ms
                               );
    IF retval <= 0 THEN
#ifdef DEBUG                                 
      IF (retval <> TCP_NOT_READY) | (timeout_ms > 0) THEN
        DEBUGLOG2("%010d-OS_TCP_USER_RECV(sock %d) failed, rc=%d", TO_UDINT(bsock^.sock), TO_UDINT(retval));
      END_IF;
#endif      
      IF retval <> TCP_NOT_READY THEN
        CloseBufferedSock(bsock);
      END_IF;
      RETURN;
    END_IF;
    bsock^.bytesInRxBuf += TO_UDINT(retval);
    DEBUG_VERBOSE3("%010d-OS_TCP_USER_RECV(sock %d): %d bytes received, new bytesInRxBuf=%d", TO_UDINT(bsock^.sock), TO_UDINT(retval), TO_UDINT(bsock^.bytesInRxBuf));
    
    IF bsock^.fAckRcvdData THEN
      // send something after data received to avoid delayed ack
      IF bsock^.bytesInTxBuf > 0 THEN
        FLUSH_TXBUF(bsock);
      ELSE
        SendCmdMsg(bsock, 
                   CMD_NOP,
                   0,                 // Type and Ack field of command (type: request, ack: no response is required)
                   #reqMsg$cMessage, 
                   SIZEOF(reqMsg), 
                   0,                 // timeout: 5 sec.   
                   1,                 // use buffered sends
                   1                  // fFlush
                  );
      END_IF;
    END_IF;
    
  END_IF; // empty rx buffer
  
  // copy data from the buffer to the caller
  IF len < bsock^.bytesInRxBuf THEN
    retval := len$DINT;
  ELSE
    retval := bsock^.bytesInRxBuf$DINT;
  END_IF;
  IF buf <> NIL THEN
    _memcpy(buf, #bsock^.rxBuf[bsock^.rxBufStrtInd], TO_UDINT(retval));
  END_IF;
  
  // adjust buffer variables
  bsock^.bytesInRxBuf -= TO_UDINT(retval);
  bsock^.rxBufStrtInd += TO_UDINT(retval);
  IF bsock^.bytesInRxBuf = 0 THEN
    bsock^.rxBufStrtInd := 0;
  END_IF;
  
  #ifdef DEBUG
  ChkBuffer(bsock);
  #endif
  
END_FUNCTION

//------------------------------------------------------------------------------
// UnRecvBuf
//
//  This function places len bytes back into the internal rx buffer.
//
// Return value
//  The number of bytes placed into the buffer.
//------------------------------------------------------------------------------
FUNCTION UnRecvBuf
VAR_INPUT
    bsock          : ^bufferedSocket;
    buf            : ^CHAR;
    len            : UDINT;
END_VAR
VAR_OUTPUT
    retval         : UDINT;
END_VAR
VAR
    bytesFree      : UDINT;    // total number of free bytes in the rx buffer
END_VAR

  #ifdef DEBUG
  ChkBuffer(bsock);
  #endif

  bytesFree      := bsock^.sizeOfRxBuf - bsock^.bytesInRxBuf;

  IF len < bytesFree THEN
    retval := len;
  ELSE
    retval := bytesFree;
  END_IF;

  IF retval > bsock^.rxBufStrtInd THEN
    // move the data in rxBuf to the end of the buffer
    _memmove(#bsock^.rxBuf[retval], #bsock^.rxBuf[bsock^.rxBufStrtInd], bsock^.bytesInRxBuf);
    bsock^.rxBufStrtInd := retval;
  END_IF;
  
  bsock^.rxBufStrtInd -= retval;
  bsock^.bytesInRxBuf += retval;
  _memcpy(#bsock^.rxBuf[bsock^.rxBufStrtInd], buf, retval);
  
  #ifdef DEBUG
  ChkBuffer(bsock);
  #endif
  
END_FUNCTION

//------------------------------------------------------------------------------
// ReadN
//
//  This function reads n bytes from a connection.
//
// Return value
//  If the function succeeds, the return value is the number of bytes requested, 
//  otherwise a tcp-user error code is returned.
//------------------------------------------------------------------------------
FUNCTION ReadN
VAR_INPUT
    bsock         : ^bufferedSocket;
    buf           : ^CHAR;
    len           : UDINT;
    timeout_ms    : UDINT;      // timeout, in milliseconds
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    ptr           : ^CHAR;
    bytesToRead   : UDINT;      // number of bytes to read
    bytesRead     : UDINT;      // number of bytes already read
END_VAR

    bytesToRead := len;
    bytesRead   := 0;
    ptr         := buf;
    WHILE bytesToRead > 0 DO
      retval := RecvBuf(bsock, ptr, bytesToRead, timeout_ms);
      IF retval <= 0 THEN
        // In the case of an error place already read data back to the buffer
        IF UnRecvBuf(bsock, buf, bytesRead) <> bytesRead THEN
          // This code should never be reached !
          DEBUGLOG1("%010d-ReadN(sock %d): UnRecvBuf failed", TO_UDINT(bsock^.sock));
          CloseBufferedSock(bsock);
          retval := ERR_CONN_BROKEN;
        END_IF;
        RETURN;
      END_IF;
      bytesToRead -= retval$UDINT;
      bytesRead   += retval$UDINT;
      ptr         += retval;
    END_WHILE;
    
    retval := bytesRead$DINT;

END_FUNCTION

//------------------------------------------------------------------------------
// ReadVRec
//
//  This function reads a record with a variable length.
//
// Return value
//  If the function succeeds, the return value is the number of bytes received 
//  in the data field (without vrec-header). 
//
// Remarks
//  When the size of the buffer is too small for the message, the connection is 
//  closed.
//------------------------------------------------------------------------------
FUNCTION ReadVRec
VAR_INPUT
    bsock         : ^bufferedSocket;
    vrec          : ^vRecord;    // the variable record buffer
    len           : UDINT;      // length of the buffer incl. header
    timeout_ms    : UDINT;      // timeout, in milliseconds
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR

  // first read the vrec-header
  retval := ReadN(bsock, (#vrec^.vHdr)$^CHAR, SIZEOF(vrec^.vHdr), timeout_ms);
  IF retval <= 0 THEN
    RETURN;
  END_IF;
  
  // check if the header is valid
  IF vrec^.vHdr.id <> 0xFACE THEN
    DEBUGLOG1("%010d-ReadVRec(sock %d): invalid vrec-hdr received", TO_UDINT(bsock^.sock));
    CloseBufferedSock(bsock);
    retval := ERR_INVALID_VRECHDR;
    RETURN;
  END_IF;
  
  // check how many data fits into the buffer
  IF (vrec^.vHdr.dataLen + SIZEOF(vrec^.vHdr)) > len THEN
    DEBUGLOG1("%010d-ReadVRec(sock %d): buffer too small", TO_UDINT(bsock^.sock));
    CloseBufferedSock(bsock);
    retval := ERR_BUF_TOO_SMALL;
    RETURN;
  END_IF;

  // read the data of the variale record
  retval := ReadN(bsock, (#vrec^.data)$^CHAR, vrec^.vHdr.dataLen, timeout_ms);
  IF retval <= 0 THEN
    // error: write the vrec-header back to the buffer
    IF UnRecvBuf(bsock, (#vrec^.vHdr)$^CHAR, SIZEOF(vrec^.vHdr)) <> SIZEOF(vrec^.vHdr) THEN
      // This code should never be reached !
      DEBUGLOG1("%010d-ReadVRec(sock %d): UnRecvBuf failed", TO_UDINT(bsock^.sock));
      CloseBufferedSock(bsock);
      retval := ERR_CONN_BROKEN;
    END_IF;
    RETURN;
  END_IF;
  
END_FUNCTION

//------------------------------------------------------------------------------
// ReadCmdMsg
//
//  This function reads a command message.
//
// Return value
//  If the function succeeds, the return value is the number of bytes received 
//  in the data field (without vrec-header). 
//
// Remarks
//  When the size of the buffer is too small for the message, the rest of the 
//  message received and discarded.
//------------------------------------------------------------------------------
FUNCTION ReadCmdMsg
VAR_INPUT
    bsock         : ^bufferedSocket;
    msg           :^cMessage;   // the message
    len           : UDINT;      // length of the message incl. header
    timeout_ms    : UDINT;      // timeout, in milliseconds
    ack           :^UDINT;      // acknowledge field received (optional)
    cmd           : UDINT;      // matching command of the message (n/a for requests)
    typeMask      : UDINT;      // specifies which type (req.,resp.or both) is expected
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR

  retval := ReadVRec(bsock, msg$^vRecord, len, timeout_ms);
  IF retval <= 0 THEN
    RETURN;
  END_IF;
  
  IF retval < SIZEOF(msg^.cHdr.cmd) THEN
    // at least the header must be available
    DEBUGLOG1("%010d-ReadCmdMsg(sock %d): missing cmd-hdr", TO_UDINT(bsock^.sock));
    CloseBufferedSock(bsock);
    retval := ERR_INVALID_CMSGHDR;
    RETURN;
  END_IF;

  // check if the type of message is expected by the caller
  IF msg^.cHdr.cmd AND MASK_CMD_ISRESP THEN
     // a response is received
     
    IF (typeMask AND CMD_TYPE_RESP) = 0 THEN
      DEBUGLOG1("%010d-ReadCmdMsg(sock %d): unexpected resonse received", TO_UDINT(bsock^.sock));
      CloseBufferedSock(bsock);
      retval := ERR_INVALID_CMSGTYPE;
    END_IF;
    IF (msg^.cHdr.cmd AND MASK_CMD_CODE) <> cmd THEN
      DEBUGLOG1("%010d-ReadCmdMsg(sock %d): cmd-code in response does not match", TO_UDINT(bsock^.sock));
      CloseBufferedSock(bsock);
      retval := ERR_INVALID_CMSGCODE;
    END_IF;
    IF (msg^.cHdr.cmd AND MASK_CMD_ACK) = 0 THEN
      DEBUGLOG1("%010d-ReadCmdMsg(sock %d): nack received", TO_UDINT(bsock^.sock));
      CloseBufferedSock(bsock);
      retval := ERR_NACK_RECEIVED;
    END_IF;
    
  ELSE
    // a request is received
    
    IF (typeMask AND CMD_TYPE_REQ) = 0 THEN
      DEBUGLOG1("%010d-ReadCmdMsg(sock %d): unexpected request received", TO_UDINT(bsock^.sock));
      CloseBufferedSock(bsock);
      retval := ERR_INVALID_CMSGTYPE;
    END_IF;
    
  END_IF;
  
  // extract the ack field
  IF ack <> NIL THEN
    ack^ := msg^.cHdr.cmd AND MASK_CMD_ACK;
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// UpdateCell
//
// Return value:
//  Error                                                     : <=0
//  When udLastUpdate did not expire                          : 1
//  When udLastUpdate expired, but dData did not change       : 2
//  When udLastUpdate expired, and dData changed (transfer)   : 3
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION UpdateCell
VAR_INPUT
    pRefrConn     : ^refrConnBlock;
    pUpdCell      : ^comStCell;  // pointer to cell in updatelist
    iOff          : UINT;
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    reqMsg        : msgUpdCellReq; // request structure for command CMD_UPDCELL
    dData         : DINT;
    rc            : DINT;
END_VAR

  retval := 1;
  
  if( pUpdCell = NIL )then
    return;
  end_if;
  
  IF pRefrConn^.bsock.fTxBufferShortage <> 0 THEN
    // in the case of a buffer shortage do not send any updatecells
    RETURN;
  END_IF;
  
  IF QUERY_FREE_TXBUF(pRefrConn^.bsock) < (2 * SIZEOF(reqMsg)) THEN
    // when there is no more space for 2 messages, then enter buffer shortage
    DEBUGLOG0("%010d-UpdateCell: entering tx buffer shortage");
    pRefrConn^.bsock.fTxBufferShortage := 1;
    // try to send all unsent data
    FLUSH_TXBUF(#pRefrConn^.bsock);
  END_IF;

  IF (ops.tAbsolute - pUpdCell^.udLastUpdate) < (pUpdCell^.udUpdateRate AND 16#FFFF) THEN
    // udLastUpdate did not expire
    RETURN;
  END_IF;
  
  IF pUpdCell^.pCh = NIL THEN
    RETURN;
  END_IF;
  
  retval := 2;
  
  IF (pUpdCell^.udUpdateRate and UPDRATE_FLAG_IS_VBI) THEN
    dData := (pUpdCell^.pCh$^virtualBaseInit^.getCrc())$DINT;
  ELSIF (pUpdCell^.udUpdateRate AND UPDRATE_FLAG_IS_MERKEREX) THEN
     dData := (pUpdCell^.pCh$^merkerex^.getcrc())$DINT;
  ELSIF (pUpdCell^.udUpdateRate AND UPDRATE_FLAG_IS_GLOBALVAR) THEN
//    dData := (pUpdCell^.pCh$^DINT)^;
    dData := UpDateGlobalVar(pUpdCell^.pCh, pUpdCell^.udUpdateRate);
  ELSE
    dData := rd(pUpdCell^.pCh);
  END_IF;
  
  IF dData = pUpdCell^.dData & (pUpdCell^.udUpdateRate and UPDRATE_FLAG_FORCE_CHANGE) = 0 THEN
    // dData did not change
    pUpdCell^.udLastUpdate := ops.tAbsolute;
    RETURN;
  END_IF;
  pUpdCell^.udUpdateRate := pUpdCell^.udUpdateRate AND NOT UPDRATE_FLAG_FORCE_CHANGE;
    
  retval := 3;    
    
  // prepare the request message structure
  reqMsg.iOff  := iOff;
  reqMsg.pCh   := pUpdCell^.pCh;
  reqMsg.dData := dData;

  // Send the request
  rc := SendCmdMsg(#pRefrConn^.bsock, 
                   CMD_UPDCELL,
                   0,                // Type and Ack field of command (type: request, ack: no response required)
                   #reqMsg$cMessage, 
                   SIZEOF(reqMsg), 
                   0,                // timeout: non-blocking 
                   1,                // use buffered sends
                   pRefrConn^.bsock.fTxBufferShortage // fFlush
                  );
  IF rc <= 0 THEN
    DEBUGLOG2("%010d-*** UpdateCell(VarPos %d): SendCmdMsg failed, rc=%d", TO_UDINT(iOff), TO_UDINT(rc));
    retval := rc;
  ELSE
    //DEBUGLOG3("%010d-UpdateCell: UPDCELL request sent, conn=0x%08X, VarPos=%d, dData=0x%08X", pRefrConn$UDINT, TO_UDINT(iOff), TO_UDINT(dData));
    pUpdCell^.dData := dData;
    pUpdCell^.udLastUpdate := ops.tAbsolute;
  END_IF;
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// UpdateListProcessor
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION UpdateListProcessor
VAR_INPUT
    pRefrConn     : ^refrConnBlock;
END_VAR
VAR
    list          : ^updateList;
    pUpdCell      : ^comStCell;  // pointer to cell in updatelist
    pStrtCell     : ^comStCell;
END_VAR

  if( pRefrConn = NIL )then
    return;
  end_if;
  if( pRefrConn^.ptr = NIL )then
    return;
  end_if;
  list := #pRefrConn^.ptr^.statList;
  if( list = NIL )then 
    return;
  end_if;  
  
  IF list^.nbrRefrCells > 0 THEN
    pStrtCell := list^.pActCell;
    WHILE 1 DO
      pUpdCell := list^.pActCell;
  
      UpdateCell(pRefrConn, pUpdCell, list^.actInd$UINT);
      IF pRefrConn^.bsock.sock < 0 THEN
        RETURN;
      END_IF;
  
      IF (list^.actInd + 1) >= list^.nbrRefrCells THEN
        list^.actInd   := 0;
        list^.pActCell := list^.pFirstCell;
      ELSE
        list^.actInd   += 1;
        list^.pActCell += SIZEOF(list^.pActCell^);
      END_IF;
  
      IF pStrtCell = list^.pActCell THEN
        EXIT;
      END_IF;
    END_WHILE;
  END_IF;    
 
  list := #pRefrConn^.ptr^.dynList;
  if( list = NIL )then
    return;
  end_if;
  IF list^.nbrRefrCells > 0 THEN
    pStrtCell := list^.pActCell;
    WHILE 1 DO
      pUpdCell := list^.pActCell;
      
      if( pUpdCell = NIL )then
        return;
      end_if;
      UpdateCell(pRefrConn, pUpdCell, (list^.actInd + 1000)$UINT);
      IF pRefrConn^.bsock.sock < 0 THEN
        RETURN;
      END_IF;
      
      IF (list^.actInd + 1) >= list^.nbrRefrCells THEN
        list^.actInd   := 0;
        list^.pActCell := list^.pFirstCell;
      ELSE
        list^.actInd   += 1;
        list^.pActCell += SIZEOF(list^.pActCell^);
      END_IF;
      
      IF pStrtCell = list^.pActCell THEN
        EXIT;
      END_IF;
    END_WHILE;
  END_IF;  
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// SendAliveMessage_TCP
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION SendAliveMessage_TCP
VAR_INPUT
    pRefrConn     : ^refrConnBlock;
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    reqMsg        : msgIamAliveReq; // request structure for command CMD_UPDCELL
    rc            : DINT;
END_VAR

  retval := 1;
  
  IF pRefrConn^.bsock.fTxBufferShortage <> 0 THEN
    // in the case of a buffer shortage do not send any requests
    RETURN;
  END_IF;
  
  IF QUERY_FREE_TXBUF(pRefrConn^.bsock) < (2 * SIZEOF(reqMsg)) THEN
    // when there is no more space for 2 messages, then enter buffer shortage
    DEBUGLOG0("%010d-SendAliveMessage_TCP: entering tx buffer shortage");
    pRefrConn^.bsock.fTxBufferShortage := 1;
    // try to send all unsent data
    FLUSH_TXBUF(#pRefrConn^.bsock);
  END_IF;

  // Send the request
  rc := SendCmdMsg(#pRefrConn^.bsock, 
                   CMD_IAMALIVE,
                   0,                // Type and Ack field of command (type: request, ack: no response required)
                   #reqMsg$cMessage, 
                   SIZEOF(reqMsg), 
                   0,                // timeout: non-blocking 
                   1,                // use buffered sends
                   1                 // fFlush
                  );
  IF rc <= 0 THEN
    DEBUGLOG1("%010d-*** SendAliveMessage_TCP: SendCmdMsg failed, rc=%d", TO_UDINT(rc));
    retval := rc;
  ELSE
    DEBUG_VERBOSE1("%010d-SendAliveMessage_TCP: IAMALIVEL request sent, conn=0x%08X", pRefrConn$UDINT);
  END_IF;
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// SyncRefListsTcp  - siehe Beschreibung von SyncRefLists
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION SyncRefListsTcp
VAR_INPUT
    pCmdConn    : ^cmdConnBlock;
END_VAR
VAR
    obj         : pVoid;
    pHead       : ^IprHead; 
    instr       : ^instruct;
    pRefrConn   : ^refrConnBlock;
END_VAR
    
    pRefrConn := pRefrConnRoot;
    WHILE pRefrConn <> NIL DO
        IF pRefrConn^.ipAddrPeer = pCmdConn^.ipAddrPeer THEN

#ifndef DUMMY_LOADER
            pHead := pCmdConn^.ptr^.com.GetHead();
#endif            
            instr := pHead^.pCode;

            IF instr^.opCode = I_WRITE THEN

                obj := instr^.aPara[0]$^pVoid;

                // statische Refresh-Liste durchsuchen
                InvalidateRefListEntry(pRefrConn^.ptr^.statList.pFirstCell, pRefrConn^.ptr^.statList.nbrRefrCells$UINT, obj);

                // dynamische Refresh-Liste durchsuchen
                InvalidateRefListEntry(pRefrConn^.ptr^.dynList.pFirstCell, pRefrConn^.ptr^.dynList.nbrRefrCells$UINT, obj);

            END_IF;

        END_IF;
        pRefrConn := pRefrConn^.next;    
    END_WHILE;

END_FUNCTION

FUNCTION GLOBAL SyncRefListsTcpAll
  VAR_INPUT
    lasalid : udint;
  END_VAR
  var
    pRefrConn   : ^refrConnBlock;
  end_var
    
  pRefrConn := pRefrConnRoot;
  
  while(pRefrConn <> NIL) do
    // statische Refresh-Liste durchsuchen
    InvalidateRefListEntry(pRefrConn^.ptr^.statList.pFirstCell, pRefrConn^.ptr^.statList.nbrRefrCells$UINT, lasalid$^void);

    // dynamische Refresh-Liste durchsuchen
    InvalidateRefListEntry(pRefrConn^.ptr^.dynList.pFirstCell, pRefrConn^.ptr^.dynList.nbrRefrCells$UINT, lasalid$^void);
    pRefrConn := pRefrConn^.next; 
  end_while;

END_FUNCTION
#endif


//------------------------------------------------------------------------------
// Entfernt unbenutzte Verbindungen.
// Es werden alle Verbindungen mit einer bestimmten Remote-IP-Adresse untersucht. 
// Wenn die Inst-ID nicht mit der angegebenen übereinstimmt, dann kann es sich 
// nur um einen Neustart der Station mit dieser IP-Adresse handeln, da die 
// Inst-ID beim Comlink-Init immer neu vergeben wird.
//
// Verbindungen, bei denen die Inst-ID für eine längere Zeit 0 ist, werden 
// ebenfalls beendet. Hier handelt es sich um Verbindungen, bei denen abgesteckt 
// wurde, bevor CMD_SESSINFO empfangen worden ist.
//
// ApplicationID:
// Wird nur von Comlink Verbindungen von PC-Anwendungen verwendet. 
// Wenn auf einem PC mehrere Anwendungen eine Comlink Verbindung verwenden, 
// dann kann jede Anwendung eine eigene ApplicationID mitschicken. Damit wird 
// verhindert, dass die zweite Anwendung die Verbindung der ersten löscht.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION DeleteUnusedConnections
VAR_INPUT
    ipAddrPeer   : UDINT;
    instIdOfPeer : UDINT;
    ApplicationID : UDINT;  // ApplicationID
END_VAR
VAR
    pCmdConn   : ^cmdConnBlock;
    pRefrConn   : ^refrConnBlock;
    pCmdConn_next   : ^cmdConnBlock;
    pRefrConn_next  : ^refrConnBlock;
END_VAR
    
    pCmdConn := pCmdConnRoot;
    WHILE pCmdConn <> NIL DO
        pCmdConn_next := pCmdConn^.next; 
        IF ((pCmdConn^.ipAddrPeer = ipAddrPeer) & 
            (pCmdConn^.ptr^.instIdOfPeer <> 0) & 
            (pCmdConn^.ptr^.instIdOfPeer <> instIdOfPeer) &
            (pCmdConn^.ApplicationID = ApplicationID))
            |
           ((pCmdConn^.ptr^.instIdOfPeer = 0) & 
            ((ops.tAbsolute - pCmdConn^.connectTime) > 5000)) THEN
            DEBUGLOG1("%010d-  =====  unused Cmd conn.data block found (ip 08%X) -> releasing block  =====", ipAddrPeer);
            ReleaseConnBlock(TYPE_CMD_CONN, pCmdConn$^connBlock, 0);
            pCmdConn := NIL;
        END_IF;
        pCmdConn := pCmdConn_next;
    END_WHILE;

    pRefrConn := pRefrConnRoot;
    WHILE pRefrConn <> NIL DO
        pRefrConn_next := pRefrConn^.next;
        IF ((pRefrConn^.ipAddrPeer = ipAddrPeer) & 
            (pRefrConn^.ptr^.instIdOfPeer <> 0) &
            (pRefrConn^.ptr^.instIdOfPeer <> instIdOfPeer) &
            (pRefrConn^.ApplicationID = ApplicationID))
            |
           ((pRefrConn^.ptr^.instIdOfPeer = 0) &
            ((ops.tAbsolute - pRefrConn^.connectTime) > 5000)) THEN 
            DEBUGLOG1("%010d-  =====  unused Refr conn.data block found (ip 08%X) -> releasing block  =====", ipAddrPeer);
            ReleaseConnBlock(TYPE_REFR_CONN, pRefrConn$^connBlock, 0);
            pRefrConn := NIL;
        END_IF;
        pRefrConn := pRefrConn_next;
    END_WHILE;

END_FUNCTION
#endif


//------------------------------------------------------------------------------
// DeleteUnusedRecycleData
//
//  Es werden unbenutzte refresh-connection-data-block's im recycle pool 
//  gesucht und wenn gefunden, dann freigegeben.
//
//  Kriterium, ob ein data-block unbenutzt ist:
//  - IP Adresse muss übereinstimmen
//  - InstanceId muss vorhanden, aber unterschiedlich sein
//  - ApplicationID muss übereinstimmen
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION DeleteUnusedRecycleData
VAR_INPUT
    ipAddrPeer   : UDINT;
    instIdOfPeer : UDINT;
    ApplicationID : UDINT;  // ApplicationID
END_VAR
VAR
    pDataWork : ^refrConnData;
    pDataPrev : ^refrConnData;
END_VAR

  pDataPrev := NIL;
  pDataWork := pRefrConnDataRoot;
  WHILE pDataWork <> NIL DO
    IF ((pDataWork^.ipAddrPeer = ipAddrPeer) & 
        (pDataWork^.instIdOfPeer <> 0) &
        (pDataWork^.instIdOfPeer <> instIdOfPeer) &
        (pDataWork^.ApplicationID = ApplicationID)) THEN 
      // remove it from the recycle pool
      IF pDataPrev = NIL THEN
        pRefrConnDataRoot  := pDataWork^.next;
      ELSE
        pDataPrev^.next := pDataWork^.next;
      END_IF;
      DEBUGLOG1("%010d-  =====  unused recycle refr conn.data block found (ip 08%X) -> releasing block  =====", ipAddrPeer);
      FreeRefrConnData(pDataWork);
      nbrRecycled -= 1;
      EXIT;
    END_IF;
    pDataPrev := pDataWork;
    if( pDataWork = pDataWork^.next )then
      exit;
    end_if;
    pDataWork := pDataWork^.next;
  END_WHILE;
  
END_FUNCTION  
#endif


//------------------------------------------------------------------------------
// CmdMsgListener
//
//  This function serves incoming requests from the client on the command 
//  channel.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION CmdMsgListener
VAR_INPUT
    pCmdConn         : ^cmdConnBlock;
END_VAR
VAR_OUTPUT
    retval    : DINT;
END_VAR
VAR
    cmsg      : cMessage512;
    ackRcvd   : UDINT;        // received ack-field
    ackSend   : UDINT;        // ack-field to send
    dataLen   : UDINT;
    bNext     : UDINT;
    msgLen    : UDINT;
    pResu     : ^results;
    cmd       : UDINT;
    pCmdConnTemp : ^cmdConnBlock;
    rc : DINT;
    releaseFlag : UDINT;
END_VAR

  retval := 0;
  releaseFlag := 0;

  #ifdef LOADER_TCP_MODERATE
    if (pCmdConn^.bsock.isDataAvailable = false) then
      return;
    end_if;
  #endif

  rc := ReadCmdMsg(#pCmdConn^.bsock, 
                       #cmsg$cMessage, 
                       sizeof(cmsg),
                       0,                   // timeout: 0 means non blocking   
                       #ackRcvd,            // received ack field
                       0,                   // matching cmd-code (n/a for requests)
                       CMD_TYPE_REQ         // accept only request types
                      );
  IF rc <= 0 THEN
#ifdef DEBUG  
    IF rc <> TCP_NOT_READY THEN
      DEBUGLOG2("%010d-*** CmdMsgListener(sock %d): ReadCmdMsg failed, rc=%d", TO_UDINT(pCmdConn^.bsock.sock), TO_UDINT(rc));
    END_IF;
#endif    
    RETURN;
  END_IF;

  retval := 1;

  dataLen := cmsg.cHdr.vHdr.dataLen - SIZEOF(cmsg.cHdr.cmd);
  ackSend := 0; // unsupported command
  msgLen  := SIZEOF(cmsg.cHdr);
  
  cmd := cmsg.cHdr.cmd AND MASK_CMD_CODE;
  CASE cmd OF
  
    CMD_BEFIPR:
      DEBUG_VERBOSE2("%010d-CmdMsgListener: sock %d - BEFIPR received, dataLen=%d", TO_UDINT(pCmdConn^.bsock.sock), dataLen$UDINT);
      // req-data:
      //    workState   : iprStates;
      //    instr       : instruct;
      // resp-data:
      //    workState   : iprStates;
      //    resu        : results;
      
      IF (dataLen < SIZEOF(iprStates)) | ((dataLen - SIZEOF(iprStates)) > SIZEOF(instruct)) THEN
        cmsg$msgBefiprResp.workstate := ERROR;
      ELSE
        // execute befipr
#ifndef DUMMY_LOADER
#ifdef LOADER_4_LASAL2          
        _memcpy(pCmdConn^.ptr^.com._m.head.pCode, #cmsg$msgBefiprReq.instr, dataLen - SIZEOF(iprStates));
        pCmdConn^.ptr^.com._m.head.workstate := cmsg$msgBefiprReq.workstate;
        pCmdConn^.ptr^.com._m.head.funkSt    := READY;
        pCmdConn^.ptr^.Com._m.resu.uilng :=0;
#else
        _memcpy(pCmdConn^.ptr^.com.head.pCode, #cmsg$msgBefiprReq.instr, dataLen - SIZEOF(iprStates));
        pCmdConn^.ptr^.com.head.workstate := cmsg$msgBefiprReq.workstate;
        pCmdConn^.ptr^.com.head.funkSt    := READY;
        pCmdConn^.ptr^.Com.resu.uilng :=0;
#endif
        cmsg$msgBefiprResp.workstate := pCmdConn^.ptr^.com.BEFIPR(#bNext);
#endif        
        SyncRefListsTcp(pCmdConn);
      END_IF;
  
      // prepare the response
      ackSend := MASK_CMD_ACK;  // command processed
      msgLen := SIZEOF(cmsg.cHdr) + SIZEOF(iprStates);
      IF cmsg$msgBefiprResp.workstate <> ERROR THEN
#ifndef DUMMY_LOADER
        pResu := pCmdConn^.ptr^.com.GetResult();
#endif        
        IF pResu^.uilng <> 16#FFFF THEN
          IF pResu^.uilng <= SIZEOF(cmsg$msgBefiprResp.resu) THEN
            msgLen += pResu^.uilng;
            _memcpy(#cmsg$msgBefiprResp.resu, pResu, pResu^.uilng);
          ELSE
            cmsg$msgBefiprResp.workstate := ERROR;
          END_IF;
        END_IF;
      END_IF;

    CMD_SESSINFO:
      DEBUGLOG2("%010d-CmdMsgListener: sock %d - SESSINFO received, dataLen=%d", TO_UDINT(pCmdConn^.bsock.sock), dataLen$UDINT);
      // req-data:
      //    version     : UDINT;
      // resp-data:
      //    version     : UDINT;

      IF dataLen >= (msgSessInfo.sizeofDynList - msgSessInfo.version) THEN

        pCmdConn^.ApplicationID := 0; // ApplicationID
        IF(datalen >= (msgSessInfo2.ApplicationID - msgSessInfo2.version + 4)) THEN  // ApplicationID
          pCmdConn^.ApplicationID := cmsg$msgSessInfo2.ApplicationID; // ApplicationID
        END_IF; // ApplicationID

        DEBUGLOG3("%010d- instId=%d, appId=%d, sessId=%d", TO_UDINT(cmsg$msgSessInfo.instId), TO_UDINT(pCmdConn^.ApplicationID), TO_UDINT(cmsg$msgSessInfo.sessId));

        // Inaktive Cmd-Connections suchen und freigeben
        pCmdConnTemp := pCmdConnRoot;
        WHILE pCmdConnTemp <> NIL DO
      
          IF pCmdConnTemp <> pCmdConn THEN
            IF (pCmdConnTemp^.ptr^.instIdOfPeer = cmsg$msgSessInfo.instId) & 
               (pCmdConnTemp^.ApplicationID = pCmdConn^.ApplicationID) & // ApplicationID
               (pCmdConnTemp^.ptr^.sessIdOfPeer = cmsg$msgSessInfo.sessId) THEN
               DEBUGLOG0("%010d-  =====  a cmd conn.data block was found in the active list -> releasing block  =====");
               ReleaseConnBlock(TYPE_CMD_CONN, pCmdConnTemp$^connBlock, 0);
               pCmdConnTemp := NIL;
              EXIT;
            END_IF;
          END_IF;
          
          pCmdConnTemp := pCmdConnTemp^.next;    
          
        END_WHILE;
        
        // store the information in my context
        pCmdConn^.ptr^.instIdOfPeer := cmsg$msgSessInfo.instId;
        pCmdConn^.ptr^.sessIdOfPeer := cmsg$msgSessInfo.sessId;

        DeleteUnusedConnections(pCmdConn^.ipAddrPeer, pCmdConn^.ptr^.instIdOfPeer, pCmdConn^.ApplicationID);

        // prepare the response
        ackSend := MASK_CMD_ACK;  // command processed
        msgLen := SIZEOF(msgSessInfo);
        cmsg$msgSessInfo.version := COMLINKT_VERSION;
        cmsg$msgSessInfo.instId  := myInstanceID;
        cmsg$msgSessInfo.sessId  := 0;
        cmsg$msgSessInfo.sizeofDynList  := MAX_DYNLIST_ENTRIES;
        cmsg$msgSessInfo.sizeofStatList := MAX_STATLIST_ENTRIES;

#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-CmdMsgListener: dataLen too small");
#endif      
      END_IF;
      
    CMD_LOGOUT:
      DEBUGLOG0("%010d-  =====  LOGOUT CmdConn =====");
      releaseFlag := 1;
      
    ELSE
      // unknown command
      DEBUGLOG3("%010d-CmdMsgListener: sock %d - unknown command received, cmd=0x%X, dataLen=%d", TO_UDINT(pCmdConn^.bsock.sock), cmd$UDINT, dataLen$UDINT);
      
  END_CASE;

  IF ackRcvd THEN
    // the received ack field indicates that a response must be sent
    rc := SendCmdMsg(#pCmdConn^.bsock, 
                         cmd, 
                         MASK_CMD_ISRESP OR MASK_CMD_ACK,  // Type and Ack field of command (type: response, ack: command processed)
                         #cmsg$cMessage, 
                         msgLen,          // length of the message incl. header
                         0,               // timeout_ms: no-blocking
                         1,               // useBuffer
                         1                // fFlush
                        );
    IF rc <= 0 THEN
      DEBUGLOG2("%010d-*** CmdMsgListener(sock %d): SendCmdMsg failed, rc=%d", TO_UDINT(pCmdConn^.bsock.sock), TO_UDINT(rc));
      CloseBufferedSock(#pCmdConn^.bsock);
      IF releaseFlag THEN
        ReleaseConnBlock( TYPE_CMD_CONN, pCmdConn$^connBlock, 0 );
        pCmdConn := NIL;
        retval := -1; // zeigt an, dass der Block freigegeben wurde und nicht mehr benutzt werden darf
      END_IF;
      RETURN;
#ifdef DEBUG      
    ELSE
      DEBUG_VERBOSE2("%010d-CmdMsgListener: sock %d - cmd response sent, msgLen=%d", TO_UDINT(pCmdConn^.bsock.sock), msgLen$UDINT);
#endif    
    END_IF;
  END_IF;

  IF releaseFlag & pCmdConn THEN
    ReleaseConnBlock( TYPE_CMD_CONN, pCmdConn$^connBlock, 0 );
    pCmdConn := NIL;
    retval := -1; // zeigt an, dass der Block freigegeben wurde und nicht mehr benutzt werden darf
  END_IF;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// RefrMsgListener
//
//  This function serves incoming requests from the client on the refresh 
//  channel.
//
// Return value
//  This function returns 0 when no message was received, otherwise a value 
//  greater than 0 is returned.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION RefrMsgListener
VAR_INPUT
    pRefrConn         : ^refrConnBlock;
END_VAR
VAR_OUTPUT
    retval    : DINT;
END_VAR
VAR
    cmsg      : cMessage512;
    ackRcvd   : UDINT;        // received ack-field
    ackSend   : UDINT;        // ack-field to send
    dataLen   : UDINT;
    msgLen    : UDINT;
    cmd       : UDINT;
    updList   : ^updateList;
    listType  : UDINT;
    pComCell  : ^comStCell;
    udLastUpdate : UDINT;
    pData     : ^refrConnData;
    pRefrConnTemp : ^refrConnBlock;
    rc        : DINT;
    releaseFlag : UDINT;
END_VAR

  retval := 0;
  releaseFlag := 0;
  
  #ifdef LOADER_TCP_MODERATE
    if (pRefrConn^.bsock.isDataAvailable = false) then
      return;
    end_if;
  #endif
  
  rc := ReadCmdMsg(#pRefrConn^.bsock, 
                   #cmsg$cMessage, 
                   SIZEOF(cmsg),
                   0,                   // timeout: 0 means non blocking   
                   #ackRcvd,            // received ack field
                   0,                   // matching cmd-code (n/a for requests)
                   CMD_TYPE_REQ         // accept only request types
                  );
  IF rc <= 0 THEN
#ifdef DEBUG  
    IF rc <> TCP_NOT_READY THEN
      DEBUGLOG2("%010d-*** RefrMsgListener(sock %d): ReadCmdMsg failed, rc=%d", pRefrConn^.bsock.sock$UDINT, TO_UDINT(rc));
    END_IF;
#endif    
    #ifdef LOADER_TCP_MODERATE
      pRefrConn^.bsock.isDataAvailable := false;
    #endif
    RETURN;
  END_IF;

  retval := 1;

  dataLen := cmsg.cHdr.vHdr.dataLen - SIZEOF(cmsg.cHdr.cmd);
  ackSend := 0; // unsupported command
  msgLen  := SIZEOF(cmsg.cHdr);
  
  cmd := cmsg.cHdr.cmd AND MASK_CMD_CODE;
  CASE cmd OF
  
    CMD_TXUPD:
      DEBUG_VERBOSE3("%010d-RefrMsgListener: sock %d - TXUPD received, dataLen=%d, VarPos=%d", pRefrConn^.bsock.sock$UDINT, dataLen$UDINT, TO_UDINT(cmsg$msgTxUpdReq.iOff));
      // req-data:
      //    iOff        : UINT;
      //    comcell     : comStCell;
      // resp-data:
      //    this request requires no response

      IF dataLen >= (SIZEOF(msgTxUpdReq) - SIZEOF(msgTxUpdReq.cHdr)) THEN
        IF cmsg$msgTxUpdReq.iOff < 1000 THEN
          updList   := #pRefrConn^.ptr^.statList;
          listType  := 0;
        ELSE
          updList   := #pRefrConn^.ptr^.dynList;
          listType  := 1;
          cmsg$msgTxUpdReq.iOff -=1000;
        END_IF;
        
        if( updList^.pFirstCell = NIL )then
          return;
        end_if;
        
        IF TO_UDINT(cmsg$msgTxUpdReq.iOff) >= updList^.nbrCells THEN
          IF IncreaseUpdateList(updList, TO_UDINT(cmsg$msgTxUpdReq.iOff) + 1, listType) = NIL THEN
            DEBUGLOG1("%010d-*** RefrMsgListener: sock %d - MemoryUpdateList failed", pRefrConn^.bsock.sock$UDINT);
            CloseBufferedSock(#pRefrConn^.bsock);
            RETURN;
          END_IF;
          DEBUGLOG3("%010d-*** RefrMsgListener: sock %d - updList %d increased to %d entries", pRefrConn^.bsock.sock$UDINT, listType, updList^.nbrCells);
        END_IF;
        
        pComCell := updList^.pFirstCell + (SIZEOF(comStCell) * cmsg$msgTxUpdReq.iOff);
        
        IF (cmsg$msgTxUpdReq.comcell.pCh <> pComCell^.pCh) | 
           ((cmsg$msgTxUpdReq.comcell.udUpdateRate AND 0xF0000000) <> (pComCell^.udUpdateRate AND 0xF0000000)) |
           (cmsg$msgTxUpdReq.iOff >= updList^.nbrRefrCells) THEN
          // Es handelt sich um eine geänderte oder neue Zelle 
          // -> Werte so setzen, damit gleich einmal ein Refresh passiert
          DEBUGLOG1("%010d-CMD_TXUPD: new cell %d", TO_UDINT(cmsg$msgTxUpdReq.iOff));
          pComCell^ := cmsg$msgTxUpdReq.comcell;
          pComCell^.dData        := 16#80000001$DINT; // invalid
          pComCell^.udUpdateRate := pComCell^.udUpdateRate or UPDRATE_FLAG_FORCE_CHANGE;
          pComCell^.udLastUpdate := ops.tAbsolute - 20*60*1000;  // current time - 20min.
        ELSE
          // Die Zelle hat es schon gegeben. udLastUpdate wird nicht geändert, da kein 
          // Refresh notwendig ist
          DEBUGLOG1("%010d-CMD_TXUPD: re-used cell %d", TO_UDINT(cmsg$msgTxUpdReq.iOff));
          udLastUpdate := pComCell^.udLastUpdate;
          pComCell^ := cmsg$msgTxUpdReq.comcell;
          pComCell^.udLastUpdate := udLastUpdate;
        END_IF;
        
        IF (pComCell^.udUpdateRate AND 0xFFFF) < SMALLEST_UPD_INTERVALL THEN
          pComCell^.udUpdateRate := (pComCell^.udUpdateRate AND 0xFFFF0000) OR SMALLEST_UPD_INTERVALL;
        END_IF;
        IF !!(pComCell^.udUpdateRate AND UPDRATE_FLAG_IS_GLOBALVAR) & 
           GetdataBufferFlag(pComCell^.pCh$^VirtualBase) THEN
          // The object has the flag ATTR_DATA_BUFFER set. For these object more than 
          // the 4 bytes of dData can be transmitted. The comlink server updates the cell 
          // when the crc changes instead of dData.
          pComCell^.udUpdateRate := pComCell^.udUpdateRate or UPDRATE_FLAG_IS_VBI;
        END_IF;
#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-RefrMsgListener: dataLen too small");
#endif      
      END_IF;
      
    CMD_SSREFR:
      DEBUG_VERBOSE3("%010d-RefrMsgListener: sock %d - SSREFR received, typ=%d, count=%d", pRefrConn^.bsock.sock$UDINT, TO_UDINT(cmsg$msgSsRefrReq.typ), TO_UDINT(cmsg$msgSsRefrReq.count));
      // req-data:
      //    count       : UINT;
      //    typ         : UINT;
      // resp-data:
      //    this request requires no response

      IF dataLen >= (SIZEOF(msgSsRefrReq) - SIZEOF(msgSsRefrReq.cHdr)) THEN
        IF cmsg$msgSsRefrReq.typ = 0 THEN
          // static list
          LoadUpdateList(#pRefrConn^.ptr^.statList, cmsg$msgSsRefrReq.count);
        ELSE
          // dynamic list
          LoadUpdateList(#pRefrConn^.ptr^.dynList, cmsg$msgSsRefrReq.count);
        END_IF;       
        // Wenn ein StartStopRefresh empfangen wurde, wird ein Flag gesetzt, das bewirkt,  
        // dass die Sends der Abarbeitung der Refreshliste nicht gepuffert sondern gleich 
        // gesendet werden.
        pRefrConn^.flushNextSends := TRUE;
#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-RefrMsgListener: dataLen too small");
#endif      
      END_IF;
      
    CMD_NOP:
      DEBUG_VERBOSE1("%010d-RefrMsgListener: sock %d - NOP received", pRefrConn^.bsock.sock$UDINT);
      // req-data:
      //    this request contains not data
      // resp-data:
      //    this request requires no response
      
    CMD_SESSINFO:
      DEBUGLOG2("%010d-RefrMsgListener: sock %d - SESSINFO received, dataLen=%d", TO_UDINT(pRefrConn^.bsock.sock), dataLen$UDINT);
      // req-data:
      //    version     : UDINT;
      // resp-data:
      //    version     : UDINT;

      IF dataLen >= (msgSessInfo.sizeofDynList - msgSessInfo.version) THEN

        pRefrConn^.ApplicationID := 0; // ApplicationID
        IF(datalen >= (msgSessInfo2.ApplicationID - msgSessInfo2.version + 4)) THEN  // ApplicationID
          pRefrConn^.ApplicationID := cmsg$msgSessInfo2.ApplicationID; // ApplicationID
        END_IF; // ApplicationID

        // Search in all other connections for the same inst-ID and sess-ID.
        // When found, we assume that this is an inactive connection and we close 
        // this connection.
        
        DEBUGLOG3("%010d- instId=%d, appId=%d, sessId=%d", TO_UDINT(cmsg$msgSessInfo.instId), TO_UDINT(pRefrConn^.ApplicationID), TO_UDINT(cmsg$msgSessInfo.sessId));
        
        // Inaktive Refr-Connections suchen und freigeben
        pRefrConnTemp := pRefrConnRoot;
        WHILE pRefrConnTemp <> NIL DO
      
          IF pRefrConnTemp <> pRefrConn THEN
            IF (pRefrConnTemp^.ptr^.instIdOfPeer = cmsg$msgSessInfo.instId) &
               (pRefrConnTemp^.ApplicationID = pRefrConn^.ApplicationID) & // ApplicationID
               (pRefrConnTemp^.ptr^.sessIdOfPeer = cmsg$msgSessInfo.sessId) THEN
               DEBUGLOG0("%010d-  =====  a refr.conn.data block was found in the active list -> releasing block  =====");
               ReleaseConnBlock(TYPE_REFR_CONN, pRefrConnTemp$^connBlock, 0);
               pRefrConnTemp := NIL;
              EXIT;
            END_IF;
          END_IF;
          
          pRefrConnTemp := pRefrConnTemp^.next;    
          
        END_WHILE;
        
        // Check if we can replace an untouched refresh connection data block 
        // with a data block from the recycle pool for reconnect of a client.
        pData := NIL;
        IF pRefrConn^.ptr^.instIdOfPeer = 0 THEN
          // the current refresh connection data block is unused
          pData := ReuseRefrConnData(cmsg$msgSessInfo.instId, cmsg$msgSessInfo.sessId);
          IF pData <> NIL THEN
            // a data block was found in the recycle pool
            // -> free the current data block and use the block from the pool
            DEBUGLOG0("%010d-  =====  re-using connnection data block from a previous connection  =====");
            FreeRefrConnData(pRefrConn^.ptr);
            pRefrConn^.ptr := pData;
            DEBUGLOG2("%010d- statList, nbrCells=%d, nbrRefrCells=%d", pRefrConn^.ptr^.statList.nbrCells, pRefrConn^.ptr^.statList.nbrRefrCells);
            DEBUGLOG2("%010d- dynList,  nbrCells=%d, nbrRefrCells=%d", pRefrConn^.ptr^.dynList.nbrCells,  pRefrConn^.ptr^.dynList.nbrRefrCells);
#ifdef DEBUG  
          ELSE
            DEBUGLOG0("%010d-  =====  could not re-use connnection data block from a previous connection  =====");
#endif      
          END_IF;
#ifdef DEBUG  
        ELSE
          DEBUGLOG0("%010d-  =====  did not try to re-use connnection data block from a previous connection  =====");
#endif      
        END_IF;

        // store the information in my context
        pRefrConn^.ptr^.instIdOfPeer := cmsg$msgSessInfo.instId;
        pRefrConn^.ptr^.sessIdOfPeer := cmsg$msgSessInfo.sessId;
        pRefrConn^.ptr^.ipAddrPeer := pRefrConn^.ipAddrPeer;
        pRefrConn^.ptr^.ApplicationID := pRefrConn^.ApplicationID;
        
        DeleteUnusedConnections(pRefrConn^.ipAddrPeer, pRefrConn^.ptr^.instIdOfPeer, pRefrConn^.ApplicationID);
        DeleteUnusedRecycleData(pRefrConn^.ipAddrPeer, pRefrConn^.ptr^.instIdOfPeer, pRefrConn^.ApplicationID);

        // prepare the response
        ackSend := MASK_CMD_ACK;  // command processed
        msgLen := SIZEOF(msgSessInfo);
        cmsg$msgSessInfo.version := COMLINKT_VERSION;
        cmsg$msgSessInfo.instId  := myInstanceID;
        cmsg$msgSessInfo.sessId  := 0;
        cmsg$msgSessInfo.sizeofDynList  := MAX_DYNLIST_ENTRIES;
        cmsg$msgSessInfo.sizeofStatList := MAX_STATLIST_ENTRIES;
        
#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-RefrMsgListener: dataLen too small");
#endif      
      END_IF;
      
    CMD_MAXIDLETIME:
      DEBUG_VERBOSE2("%010d-RefrMsgListener: sock %d - CMD_MAXIDLETIME received, maxIdleTime=%d", pRefrConn^.bsock.sock$UDINT, cmsg$msgIdlTimReq.maxIdleTime_ms);
      // req-data:
      //    maxIdleTime_ms : UDINT;
      // resp-data:
      //    this request requires no response

      IF cmsg$msgIdlTimReq.maxIdleTime_ms <> pRefrConn^.ptr^.maxIdleTime_ms THEN
        pRefrConn^.ptr^.maxIdleTime_ms := cmsg$msgIdlTimReq.maxIdleTime_ms;
        pRefrConn^.ptr^.lastSendTime_ms := ops.tAbsolute - pRefrConn^.ptr^.maxIdleTime_ms; // damit gleich was geschickt wird
      END_IF;

    CMD_LOGOUT:
      DEBUG_VERBOSE3("%010d-RefrMsgListener: sock %d - LOGOUT received, typ=%d, count=%d", pRefrConn^.bsock.sock$UDINT, TO_UDINT(cmsg$msgSsRefrReq.typ), TO_UDINT(cmsg$msgSsRefrReq.count));
      // req-data:
      //    this request requires no data
      // resp-data:
      //    this request requires no response
      LoadUpdateList( #pRefrConn^.ptr^.statList, 0 );
      LoadUpdateList( #pRefrConn^.ptr^.dynList, 0 );
      
      DEBUGLOG0("%010d-  =====  LOGOUT RefrConn  =====");
      releaseFlag := 1;

    ELSE
      // unknown command
      DEBUGLOG2("%010d-RefrMsgListener: sock %d - unknown command received, dataLen=%d", pRefrConn^.bsock.sock$UDINT, dataLen$UDINT);
      
  END_CASE;

  IF ackRcvd THEN
    // the received ack field indicates that a response must be sent
    rc := SendCmdMsg(#pRefrConn^.bsock, 
                     cmd, 
                     MASK_CMD_ISRESP OR MASK_CMD_ACK,  // Type and Ack field of command (type: response, ack: command processed)
                     #cmsg$cMessage, 
                     msgLen,          // length of the message incl. header
                     0,               // timeout_ms: no-blocking
                     1,               // useBuffer
                     1                // fFlush
                    );
    IF rc <= 0 THEN
      DEBUGLOG2("%010d-*** RefrMsgListener(sock %d): SendCmdMsg failed, rc=%d", TO_UDINT(pRefrConn^.bsock.sock), TO_UDINT(rc));
      IF rc = ERR_BUFFER_SHORTAGE THEN
        LDR_DebugLog0("%010d comlink: closing refr-sock due to buffer shortage");
        _onlineDiagFlags := _onlineDiagFlags OR 2;
      END_IF;
      CloseBufferedSock(#pRefrConn^.bsock);
      IF releaseFlag THEN
        ReleaseConnBlock( TYPE_REFR_CONN, s_pRefrConn$^connBlock, 0 );
        s_pRefrConn := NIL;
        retval := -1; // zeigt an, dass der Block freigegeben wurde und nicht mehr benutzt werden darf
      END_IF;
      RETURN;
#ifdef DEBUG  
    ELSE
      DEBUG_VERBOSE2("%010d-RefrMsgListener: sock %d - cmd response sent, msgLen=%d", TO_UDINT(pRefrConn^.bsock.sock), rc$UDINT);
#endif      
    END_IF;
  END_IF;

  IF releaseFlag & s_pRefrConn THEN
    ReleaseConnBlock( TYPE_REFR_CONN, s_pRefrConn$^connBlock, 0 );
    s_pRefrConn := NIL;
    retval := -1; // zeigt an, dass der Block freigegeben wurde und nicht mehr benutzt werden darf
  END_IF;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// AliveCheckMsgRcvd
//------------------------------------------------------------------------------
FUNCTION AliveCheckMsgRcvd
VAR_INPUT
  pLogin         : ^loginContext; // connection context
END_VAR
VAR
	pNode 						  : ^MonitorNode;
END_VAR

    OS_MT_WAIT(monitorNodeMutex);
    pNode := pLogin^.pNode;
    IF pNode THEN
        pNode^.lastRcvdTime_ms := ops.tAbsolute;
        pNode^.isRemotePlcAlive := TRUE;
    END_IF;
    OS_MT_SIGNAL(monitorNodeMutex);

END_FUNCTION

//------------------------------------------------------------------------------
// UpdCellListener
//
//  This function serves incoming requests from the server on the refresh 
//  channel.
//
// Return value
//  This function returns 0 when no message was received, otherwise a value 
//  greater than 0 is returned.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION UpdCellListener
VAR_INPUT
    pLogin         : ^loginContext; // connection context
END_VAR
VAR_OUTPUT
    retval         : UDINT;
END_VAR
VAR
    cmsg      : cMessage512;
    ackRcvd   : UDINT;        // received ack-field
    ackSend   : UDINT;        // ack-field to send
    dataLen   : UDINT;
    msgLen    : UDINT;
    cmd       : UDINT;
    pComCell  : ^comStCell;
    data2LSE  : d2LSE;        
    uiOffs    : UINT;
    rc        : DINT;
    original_this : UDINT;
    pCallbackListEntry :^sDynCallbackList;
END_VAR

  retval := 0;

  #ifdef LOADER_TCP_MODERATE
    if (pLogin^.refrSockClnt.isDataAvailable = FALSE) then
      return;
    end_if;
  #endif

  pLogin^.refrSockClnt.fAckRcvdData := 1;
  
  rc := ReadCmdMsg(#pLogin^.refrSockClnt, 
                   #cmsg$cMessage, 
                   SIZEOF(cmsg),
                   0,                   // timeout: 0 means non blocking   
                   #ackRcvd,            // received ack field
                   0,                   // matching cmd-code (n/a for requests)
                   CMD_TYPE_REQ         // accept only request types
                  );
  IF rc <= 0 THEN
    IF rc <> TCP_NOT_READY THEN
      DEBUGLOG2("%010d-*** UpdCellListener: ReadCmdMsg failed, rc=%d, closing refr sock %d", TO_UDINT(rc), pLogin^.refrSockClnt.sock$UDINT);
      CloseBufferedSock(#pLogin^.refrSockClnt);
    END_IF;
    #ifdef LOADER_TCP_MODERATE
      pLogin^.refrSockClnt.isDataAvailable := false;
    #endif
    RETURN;
  END_IF;
  pLogin^.refrSockClnt.fAckRcvdData := 0;
  AliveCheckMsgRcvd(pLogin);

  retval := 1;

  dataLen := cmsg.cHdr.vHdr.dataLen - SIZEOF(cmsg.cHdr.cmd);
  ackSend := 0; // value for an unsupported command (=nack)
  msgLen  := SIZEOF(cmsg.cHdr);
  
  cmd := cmsg.cHdr.cmd AND MASK_CMD_CODE;
  CASE cmd OF
  
    CMD_UPDCELL:
      DEBUG_VERBOSE3("%010d-UpdCellListener: sock %d - UPDCELL received, dataLen=%d, VarPos=%d", pLogin^.refrSockClnt.sock$UDINT, dataLen$UDINT, TO_UDINT(cmsg$msgUpdCellReq.iOff));
      // req-data:
      //    iOff        : INT;
      //    comcell     : comStCell;
      // resp-data:
      //    this request requires no response
      
      IF dataLen >= (SIZEOF(msgUpdCellReq) - SIZEOF(msgUpdCellReq.cHdr)) THEN
        uiOffs := cmsg$msgUpdCellReq.iOff$UINT;
        IF uiOffs < 1000 THEN
          pComCell := pLogin^.statList.pFirstCell;
          IF TO_UDINT(cmsg$msgUpdCellReq.iOff) >= pLogin^.statList.nbrCells THEN
            DEBUGLOG2("%010d-*** UpdCellListener: sock %d - VarPos(%d) is too big for StatList", pLogin^.refrSockClnt.sock$UDINT, TO_UDINT(cmsg$msgUpdCellReq.iOff));
            RETURN;
          END_IF;
        ELSE 
          uiOffs -=1000;
          pComCell := pLogin^.dynlist.pFirstCell;
          IF TO_UDINT(uiOffs) >= pLogin^.dynList.nbrCells THEN
            DEBUGLOG2("%010d-*** UpdCellListener: sock %d - VarPos(%d) is too big for DynList", pLogin^.refrSockClnt.sock$UDINT, TO_UDINT(cmsg$msgUpdCellReq.iOff));
            RETURN;
          END_IF;
        END_IF;
        
        pComCell        := pComcell + (SIZEOF(comStCell) * uiOffs);
        
        IF pComCell^.pCh <> cmsg$msgUpdCellReq.pCh THEN
          DEBUGLOG1("%010d-*** UpdCellListener: sock %d - invalid upd-cell received", pLogin^.refrSockClnt.sock$UDINT);
          RETURN;
        END_IF;
        pComCell^.dData := cmsg$msgUpdCellReq.dData; 
  
        // call callback function      
        IF ((pLogin^.pCallback <> NIL) | (pLogin^.pDynCallbacks <> NIL)) THEN
          data2LSE.data      := pComCell^.dData;
          data2LSE.uiOffs    := cmsg$msgUpdCellReq.iOff$UINT;
          data2LSE.varlistID := pComCell^.VarlistID;
          original_this := ESI;
          
          IF (pLogin^.pCallback <> NIL) THEN
             ESI := pLogin^.pThis;
            pLogin^.pCallback$PrototypeCallbackT(pLogin^.pComdefT, #data2LSE);
          END_IF;
          IF (pLogin^.pDynCallbacks <> NIL) THEN
             pCallbackListEntry := pLogin^.pDynCallbacks;
             WHILE ((pCallbackListEntry <> NIL) & (pCallbackListEntry^.pCallback <> NIL)) DO
                ESI := pCallbackListEntry^.pCallback^.pThis;
                pCallbackListEntry^.pCallback^.pCallback$PrototypeCallbackT(pLogin^.pComdefT, #data2LSE); 
                pCallbackListEntry := pCallbackListEntry^.pNext;
             END_WHILE;
          END_IF;
          
          ESI := original_this;

          DEBUG_VERBOSE3("%010d-UpdCellListener: Callback(dData=0x%08X,uiOffs=%d,id=0x%08X", TO_UDINT(data2LSE.data), TO_UDINT(data2LSE.uiOffs), TO_UDINT(data2LSE.varlistID));
        END_IF;
        
        
        
#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-UpdCellListener: dataLen too small");
#endif      
      END_IF;
      
    CMD_IAMALIVE:
      DEBUG_VERBOSE1("%010d-UpdCellListener: sock %d - IAMALIVE received", pLogin^.refrSockClnt.sock$UDINT);
      // req-data:
      // resp-data:
      //    this request requires no response
      
    ELSE
      // unknown command
      DEBUGLOG2("%010d-UpdCellListener: sock %d - unknown command received, dataLen=%d", pLogin^.refrSockClnt.sock$UDINT, dataLen$UDINT);
      
  END_CASE;

  IF ackRcvd THEN
    // the received ack field indicates that a response must be sent
    rc := SendCmdMsg(#pLogin^.refrSockClnt, 
                     cmd, 
                     MASK_CMD_ISRESP OR MASK_CMD_ACK,  // Type and Ack field of command (type: response, ack: command processed)
                     #cmsg$cMessage, 
                     msgLen,          // length of the message incl. header
                     0,               // timeout_ms: no-blocking
                     1,               // useBuffer
                     1                // fFlush
                    );
    IF rc <= 0 THEN
      DEBUGLOG2("%010d-*** UpdCellListener: SendCmdMsg failed, rc=%d, closing refr sock %d", TO_UDINT(rc), pLogin^.refrSockClnt.sock$UDINT);
      CloseBufferedSock(#pLogin^.refrSockClnt);
      RETURN;
#ifdef DEBUG      
    ELSE
      DEBUG_VERBOSE2("%010d-UpdCellListener: sock %d - cmd response sent, msgLen=%d", TO_UDINT(pLogin^.refrSockClnt.sock), rc$UDINT);
#endif
    END_IF;
  END_IF;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// PndConnListener
//
//  This function listens on the pending socket for a response of a SESSINFO 
//  command.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION PndConnListener
VAR_INPUT
    pLogin         : ^loginContext; // connection context
END_VAR
VAR_OUTPUT
    retval         : UDINT;
END_VAR
VAR
    cmsg      : cMessage512;
    ackRcvd   : UDINT;        // received ack-field
    ackSend   : UDINT;        // ack-field to send
    dataLen   : UDINT;
    msgLen    : UDINT;
    cmd       : UDINT;
    rc        : DINT;
END_VAR

  retval := 0;
  IF pLogin^.refrSockClnt.sock < 0 THEN
    RETURN;
  END_IF;

  rc := ReadCmdMsg(#pLogin^.refrSockClnt, 
                   #cmsg$cMessage, 
                   SIZEOF(cmsg),
                   0,                   // timeout: 0 means non blocking   
                   #ackRcvd,            // received ack field
                   0,                   // matching cmd-code (n/a for requests)
                   CMD_TYPE_RESP        // accept only request types
                  );
  IF rc <= 0 THEN
    IF rc <> TCP_NOT_READY THEN
      DEBUGLOG2("%010d-*** PndConnListener: ReadCmdMsg failed, rc=%d, closing refr sock %d", TO_UDINT(rc), pLogin^.refrSockClnt.sock$UDINT);
      CloseBufferedSock(#pLogin^.refrSockClnt);
    END_IF;
    RETURN;
  END_IF;
  AliveCheckMsgRcvd(pLogin);

  retval := 1;

  dataLen := cmsg.cHdr.vHdr.dataLen - SIZEOF(cmsg.cHdr.cmd);
  ackSend := 0; // value for an unsupported command (=nack)
  msgLen  := SIZEOF(cmsg.cHdr);
  
  cmd := cmsg.cHdr.cmd AND MASK_CMD_CODE;
  CASE cmd OF
  
    CMD_SESSINFO:
      IF cmsg.cHdr.cmd AND MASK_CMD_ISRESP THEN
        DEBUGLOG2("%010d-PndConnListener: sock %d - SESSINFO response received, dataLen=%d", TO_UDINT(pLogin^.refrSockClnt.sock), dataLen$UDINT);
        // req-data:
        //    version     : UDINT;
        // resp-data:
        //    version     : UDINT;
  
        IF dataLen >= (msgSessInfo.sizeofDynList - msgSessInfo.version) THEN
  
          IF pLogin^.refrSockClnt.isInstIdVerified = INSTID_CMD_PENDING THEN
            IF pLogin^.instIdOfPeer = cmsg$msgSessInfo.instId THEN
              DEBUGLOG0("%010d-*** PndConnListener: instance ID matches");
              pLogin^.refrSockClnt.isInstIdVerified := INSTID_MATCH;
            ELSE
              DEBUGLOG0("%010d-*** PndConnListener: instance ID does not match, closeing connection");
              pLogin^.refrSockClnt.isInstIdVerified := INSTID_MISMATCH;
              CloseBufferedSock(#pLogin^.refrSockClnt);
            END_IF;
          END_IF;
          
#ifdef DEBUG  
        ELSE
          DEBUG_VERBOSE0("%010d-PndConnListener: dataLen too small");
#endif      
        END_IF;

        IF dataLen >= (msgSessInfo.sizeofStatList + sizeof(msgSessInfo.sizeofStatList) - msgSessInfo.version) THEN
            pLogin^.sizeofRemoteDynList  := cmsg$msgSessInfo.sizeofDynList;
            pLogin^.sizeofRemoteStatList := cmsg$msgSessInfo.sizeofStatList;
        END_IF;

      END_IF;
      
    ELSE
      // unknown command
      DEBUGLOG2("%010d-PndConnListener: sock %d - unknown command received, dataLen=%d", pLogin^.refrSockClnt.sock$UDINT, dataLen$UDINT);
      
  END_CASE;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// SendSessionInfo
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION SendSessionInfo
VAR_INPUT
    bsock         : ^bufferedSocket;
    mySessId      : UDINT;
    timeout_ms    : UDINT;      // timeout, in milliseconds
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    cmsg          : cMessage256;
    useBuffer     : UDINT;
    fFlush        : UDINT;
END_VAR

  // Send the request
  cmsg$msgSessInfo2.version := COMLINKT_VERSION;
  cmsg$msgSessInfo2.instId  := myInstanceId;
  cmsg$msgSessInfo2.sessId  := mySessId;
  cmsg$msgSessInfo2.sizeofDynList  := MAX_DYNLIST_ENTRIES;
  cmsg$msgSessInfo2.sizeofStatList := MAX_STATLIST_ENTRIES;
  cmsg$msgSessInfo2.applicationId := myApplId;
  
  IF timeout_ms = 0 THEN
    useBuffer := 1;
    fFlush    := 1;
  ELSE
    useBuffer := 0;
    fFlush    := 0;
  END_IF;
  
  retval := SendCmdMsg(bsock, 
                       CMD_SESSINFO,
                       MASK_CMD_ACK,      // Type and Ack field of command (type: request, ack: a response is required)
                       #cmsg$cMessage, 
                       SIZEOF(msgSessInfo2), 
                       timeout_ms, 
                       useBuffer,
                       fFlush   
                      );
  IF retval <= 0 THEN
    DEBUGLOG1("%010d-*** SendSessionInfo: SendCmdMsg(CMD_SESSINFO) failed, retval=%d", TO_UDINT(retval));
    RETURN;
  END_IF;
  DEBUGLOG0("%010d-SendSessionInfo: SESSINFO request sent");
  
END_FUNCTION  
#endif

//------------------------------------------------------------------------------
// RecvSessionInfo
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION RecvSessionInfo
VAR_INPUT
    bsock         : ^bufferedSocket;
    timeout_ms    : UDINT;      // timeout, in milliseconds
    versionOfPeer : ^UDINT;
    instIdOfPeer  : ^UDINT;
    sizeofRemoteDynList  : ^UINT;
    sizeofRemoteStatList : ^UINT;
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    cmsg          : cMessage256;
END_VAR

  // Read the response
  retval := ReadCmdMsg(bsock, 
                       #cmsg$cMessage, 
                       SIZEOF(cmsg),
                       timeout_ms,
                       NIL,                 // received ack field
                       CMD_SESSINFO,
                       CMD_TYPE_RESP        // accept only response types
                      );
  IF retval <= 0 THEN
#ifdef DEBUG    
    IF (retval <> TCP_NOT_READY) | (timeout_ms > 0) THEN
      DEBUGLOG1("%010d-*** RecvSessionInfo: ReadCmdMsg(CMD_SESSINFO) failed, retval=%d", TO_UDINT(retval));
    END_IF;
#endif      
    RETURN;
  END_IF;
  
  IF retval < (SIZEOF(msgSessInfo.cHdr.cmd) + SIZEOF(msgSessInfo.version) + SIZEOF(msgSessInfo.instId)) THEN
    // not enough data received !
    DEBUGLOG0("%010d-*** RecvSessionInfo: not enough data received");
    retval := ERR_INVALID_CMSGDATA;
    RETURN;
  END_IF;
  DEBUGLOG2("%010d-RecvSessionInfo: SESSINFO response received: version of peer=0x%08X, instId=0x%08X", cmsg$msgSessInfo.version, cmsg$msgSessInfo.instId);
  
  IF versionOfPeer <> NIL THEN
    versionOfPeer^ := cmsg$msgSessInfo.version;
    instIdOfPeer^  := cmsg$msgSessInfo.instId;
    
    IF retval >= (msgSessInfo.sizeofStatList + sizeof(msgSessInfo.sizeofStatList) - msgSessInfo.version) THEN
      sizeofRemoteDynList^  := cmsg$msgSessInfo.sizeofDynList;
      sizeofRemoteStatList^ := cmsg$msgSessInfo.sizeofStatList;
    END_IF;
  END_IF;
  
END_FUNCTION  
#endif

//------------------------------------------------------------------------------
// ExchgSessionInfo
//
//  Exchange session info with the server
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ExchgSessionInfo
VAR_INPUT
    bsock         : ^bufferedSocket;
    mySessId      : UDINT;
    timeout_ms    : UDINT;      // timeout, in milliseconds
    versionOfPeer : ^UDINT;
    instIdOfPeer  : ^UDINT;
    sizeofRemoteDynList  : ^UINT;
    sizeofRemoteStatList : ^UINT;
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR

  // Send the request
  retval := SendSessionInfo(bsock, mySessId, timeout_ms);
  IF retval <= 0 THEN
    RETURN;
  END_IF;

  // Read the response
  retval := RecvSessionInfo(bsock, 
                            timeout_ms, 
                            versionOfPeer, 
                            instIdOfPeer,
                            sizeofRemoteDynList,
                            sizeofRemoteStatList
                            );
  IF retval <= 0 THEN
    RETURN;
  END_IF;
  
END_FUNCTION  
#endif

#ifdef COMLINK_TCP_CLIENT
FUNCTION SetSocketOptions
VAR_INPUT
  sock : DINT;
END_VAR
VAR
  onOff : DINT;
END_VAR

  IF OS_TCP_USER_VERSION >= 5 THEN
   onOff := 0; // 0 means disable, 1 means enable
   OS_TCP_USER_SETSOCKOPT(sock, SOL_SOCKET, SO_DELAYED_ACK, #onOff$CHAR, sizeof(onOff));
   OS_TCP_USER_SETSOCKOPT(sock, SOL_SOCKET, SO_NAGLE,       #onOff$CHAR, sizeof(onOff));
  END_IF;

END_FUNCTION  

FUNCTION CreateSocket
VAR_OUTPUT
  sock : DINT;
END_VAR

  sock := OS_TCP_USER_SOCKET();
  IF sock >= 0 THEN
    IF sock < arraysize(usedsocks) THEN
      usedsocks[sock] := 'L';
    END_IF;
    SetSocketOptions(sock);
  END_IF;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// CheckClientConnection
//
//  Check the connection from the client to the server and try to re-connect 
//  when the connection does not exist.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION CheckClientConnection
VAR_INPUT
    pLogin        : ^loginContext;  // connection context
    bsock         : ^bufferedSocket;
    timeout_ms    : UDINT;          // timeout, in milliseconds
END_VAR
VAR_OUTPUT
    retval        : comlinkReason;
END_VAR
VAR
    versionOfPeer : UDINT;
    instIdOfPeer  : UDINT;
    rc            : DINT;
    sizeofRemoteDynList  : UINT;
    sizeofRemoteStatList : UINT;
END_VAR    

  IF bsock^.isInstIdVerified = INSTID_MISMATCH THEN
    // the server has been restarted, no retry possible
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;
  
  IF (bsock^.sock >= 0) & (bsock^.isInstIdVerified = INSTID_MATCH) THEN
    // everything o.k., the connection is up and the server has not been restarted
    retval := COMLINK_OK;
    RETURN;
  END_IF;

  IF bsock^.sock < 0 THEN
    // the connection is down, start a reconnect process
    retval := COMLINK_ERR_NOCONNECTION;
    
    bsock^.isInstIdVerified := INSTID_NOT_VERIFIED;
    IF bsock^.pndSock >= 0 THEN
      // the socket is already in a 'connect pending' state
      RETURN;
    END_IF;
    
    bsock^.pndSock := CreateSocket();
    IF bsock^.pndSock < 0 THEN
      DEBUGLOG1("%010d-*** CheckClientConnection: CreateSocket failed, rc=%d", bsock^.pndSock$UDINT);
      RETURN;
    END_IF;
    
    // The connection is now in a 'connect pending' state. The CyWork task does the rest
    DEBUGLOG2("%010d-*** CheckClientConnection: connection is now in pending state (sock %d, pndSock %d)", bsock^.sock$UDINT, bsock^.pndSock$UDINT);
    RETURN;
  END_IF;
  
  // the connection is up, but the instance id of the server is not verified
    
  IF bsock^.isInstIdVerified = INSTID_NOT_VERIFIED THEN
    // send session information
    rc := SendSessionInfo(bsock, pLogin^.pComdefT$UDINT, timeout_ms);
    IF rc <= 0 THEN
      DEBUGLOG1("%010d-*** CheckClientConnection: SendSessionInfo failed, retval=%d", TO_UDINT(retval));
      CloseBufferedSock(bsock);
      RETURN;
    END_IF;
    bsock^.isInstIdVerified := INSTID_CMD_PENDING;
  END_IF;
  
  IF bsock^.isInstIdVerified = INSTID_CMD_PENDING THEN
    // receive session information
    sizeofRemoteDynList  := pLogin^.sizeofRemoteDynList;
    sizeofRemoteStatList := pLogin^.sizeofRemoteStatList;
    rc := RecvSessionInfo(bsock, timeout_ms, #versionOfPeer, #instIdOfPeer, #sizeofRemoteDynList, #sizeofRemoteStatList);
    IF rc <= 0 THEN
      IF (rc <> TCP_NOT_READY) | (timeout_ms > 0) THEN
        DEBUGLOG1("%010d-*** CheckClientConnection: RecvSessionInfo failed, retval=%d", TO_UDINT(retval));
        CloseBufferedSock(bsock);
      END_IF;
      RETURN;
    END_IF;
    IF pLogin^.instIdOfPeer = instIdOfPeer THEN
      DEBUGLOG0("%010d-CheckClientConnection: instance ID matches");
      pLogin^.sizeofRemoteDynList  := sizeofRemoteDynList;
      pLogin^.sizeofRemoteStatList := sizeofRemoteStatList;
      bsock^.isInstIdVerified := INSTID_MATCH;
    ELSE
      DEBUGLOG0("%010d-*** CheckClientConnection: instance ID does not match, closeing connection");
      bsock^.isInstIdVerified := INSTID_MISMATCH;
      CloseBufferedSock(bsock);
    END_IF;
  END_IF;
  
  IF bsock^.isInstIdVerified = INSTID_MATCH THEN
    retval := COMLINK_OK;
  ELSE
    retval := COMLINK_ERR_LOGIN_REQUIRED;
  END_IF;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// TraceMsg4Client
//------------------------------------------------------------------------------
FUNCTION TraceMsg4Client

  IF ComlinkT_wasTraceMsgDisplayed = 0 THEN
    
    IF isClientReady < 0 THEN
      CASE isClientReady OF
        COMLINKT_ERR_NOTCP:
          TRACE_ERR("Comlink/TCP client error: TCP_USER interface not available");
          LSLCLI_LoaderTracePrint("Comlink/TCP client error: TCP_USER interface not available", 12);
        COMLINKT_ERR_NOMTAPI:
          TRACE_ERR("Comlink/TCP client error: MT interface not available");
          LSLCLI_LoaderTracePrint("Comlink/TCP client error: MT interface not available", 12);
        COMLINKT_ERR_WRONG_OSVER:
          TRACE_ERR("Comlink/TCP client error: wrong OS-version (must be > 5.42)");
          LSLCLI_LoaderTracePrint("Comlink/TCP client error: wrong OS-version (must be > 5.42)", 12);
      ELSE
          TRACE_ERR("Comlink/TCP client error: unknown error code");
          LSLCLI_LoaderTracePrint("Comlink/TCP client error: unknown error code", 12);
      END_CASE;
      ComlinkT_wasTraceMsgDisplayed := 1;
      RETURN;
    END_IF;
    
  END_IF;
  
END_FUNCTION

FUNCTION TCPIP_Task
VAR_INPUT
  pvoid : ^void;
END_VAR;
//------------------------------------------------------------------------------
// ComlinkT_Init
//
//  Creates sockets for the command- and refresh- server and places them 
//  into the listening state. The server queries its command port number from 
//  the operating system. Usually it is port 1955, but when multiple servers 
//  run on the same machine (e.g.LARS) then it can be a different number. 
//  The refresh port number is the command port number plus 1.
//------------------------------------------------------------------------------
FUNCTION ComlinkT_Init
VAR
#ifdef COMLINK_TCP_SERVER
    cmdPortSrvr  : UDINT;
    refrPortSrvr : UDINT;
#endif
#ifdef COMLINK_TCP_CLIENT
    i            : UDINT;
#endif
END_VAR
  ComlinkT_wasTraceMsgDisplayed :=0;
#ifdef COMLINK_TCP_SERVER
  isServerReady     := COMLINKT_ERR_NOTCP;
  pCmdConnRoot      := NIL;
  pRefrConnRoot     := NIL;
  nbrCmdConn        := 0;
  nbrRefrConn       := 0;
  nbrRecycled       := 0;
  pRefrConnDataRoot := NIL;
  tcpServerState    := 0;
  tcpAcceptState    := 0;
  s_pCmdConn        := NIL;
  s_pRefrConn       := NIL;

#endif
#ifdef COMLINK_TCP_CLIENT
  isClientReady     := COMLINKT_ERR_NOTCP;
  FOR i := 0 TO (g_max_logins - 1) DO
    loginArray[i] := NIL;
  END_FOR;
  loginAnz          := 0;
  g_pCallback       := NIL;
#ifdef LOADER_TCP_MODERATE  
  //SCHMAT_001
  TCPCltCntr        := 0;
#endif  
#endif
  myInstanceId      := GetMyInstanceId();
  myApplId          := GetMyApplId();
  
  IF OS_CILGET("TCP_USER", #lsl_tcp_user) THEN
    // the OS does not support TCP/IP
    DEBUGLOG0("%010d-TCP_USER interface not supported");
    lsl_tcp_user  := NIL;
    #ifdef COMLINK_TCP_SERVER
    cmdSockSrvr   := INVALID_SOCKET_NUM;
    refrSockSrvr  := INVALID_SOCKET_NUM;
    #endif
    RETURN;
  END_IF;

#ifdef COMLINK_TCP_CLIENT
  isClientReady := 0;
  IF mt_api = NIL THEN
    DEBUGLOG0("%010d-LSL_MULTITASK interface not supported");
    isClientReady := COMLINKT_ERR_NOMTAPI;
  END_IF;
  
  IF _RtOSversion < 16#052B THEN
      // there was a bug with LSL_MULTITASK in OS <= 5.42
    isClientReady     := COMLINKT_ERR_WRONG_OSVER;
  END_IF;
  
  loginArrayMutex := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                           1,    // initvalue
                                           0,    // flags: 0 means create unconitionally
                                           "COMLINKT0"
                                           );
  IF loginArrayMutex = NIL THEN
    isClientReady := COMLINKT_ERR_API_FAILED;
  END_IF;
  
#endif
  

#ifdef COMLINK_TCP_SERVER
  isServerReady := 0;
  cmdPortSrvr := 0;
  IF lsl_tcp_user^.udVersion >= 3 THEN
    cmdPortSrvr := OS_TCP_USER_GETSERVBYNAME("comlink", "tcp");
  END_IF;
  IF cmdPortSrvr = 0 THEN
    cmdPortSrvr  := 1955;               // default cmd port is hardcoded to 1955
  END_IF;
  refrPortSrvr := cmdPortSrvr + 1;

  cmdSockSrvr  := CreateSocket(); // create a new command socket
  refrSockSrvr := CreateSocket(); // create a new refresh socket

  IF (cmdSockSrvr >= 0) & (refrSockSrvr >= 0) THEN
    // place the sockets into the listening state
    IF OS_TCP_USER_LISTEN(cmdSockSrvr, cmdPortSrvr, 0) <> TCP_NO_ERROR THEN
      DEBUGLOG0("%010d-TCP Server for command data not started");
      TRACE_ERR("TCP Server for command data not started");
      LSLCLI_LoaderTracePrint("TCP Server for command data not started", 12);
      isServerReady := COMLINKT_ERR_API_FAILED;
    ELSE
      IF OS_TCP_USER_LISTEN(refrSockSrvr, refrPortSrvr, 0) <> TCP_NO_ERROR THEN
        DEBUGLOG0("%010d-TCP Server for refresh data not started");
        TRACE_ERR("TCP Server for refresh data not started");
        LSLCLI_LoaderTracePrint("TCP Server for refresh data not started", 12);
        isServerReady := COMLINKT_ERR_API_FAILED;
      ELSE
        // everything o.k.
        DEBUGLOG2("%010d-ComlinkT_Init:server started, cmdPort=%d, refrPort=%d", cmdPortSrvr$UDINT, refrPortSrvr$UDINT);
      END_IF;
    END_IF;
  ELSE
    isServerReady := COMLINKT_ERR_API_FAILED;
  END_IF;

  IF isServerReady < 0 THEN    
    // something was wrong, cleanup now
    DEBUGLOG0("%010d-*** ComlinkT_Init:server did not start");
    IF cmdSockSrvr >= 0 THEN
      CloseSock(#cmdSockSrvr);
    END_IF;
    IF refrSockSrvr >= 0 THEN
      CloseSock(#refrSockSrvr);
    END_IF;
  END_IF;
#endif

#ifdef LOADER_TCP_MODERATE
  taskhandle := OS_MT_CREATETHREAD(#TCPIP_Task(),
                                   14,
                                   16#4000,
                                   MT_Task_SAVETHIS,
                                   NIL,
                                   "LDR_TCPIP_Task");

  //udTimeTCP := 0;
#endif
END_FUNCTION
//------------------------------------------------------------------------------
// ComlinkT_Login
//
//  Establish a connection to a comlink server.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_Login
VAR_INPUT
    pComdef       : ^comdef; 
END_VAR
VAR_OUTPUT
    retval        : UINT;
END_VAR
VAR
    pComdefT      : ^comdefT; 
    pLogin        : ^loginContext; // connection context
    cmdPortSrvr   : UDINT;        // port number of the servers command port
    refrPortSrvr  : UDINT;        // port number of the servers refresh port
    versionOfPeer : UDINT;
    instIdOfPeer  : UDINT;
    rc            : DINT;        
    i             : UDINT;
    sizeofRemoteDynList  : UINT;
    sizeofRemoteStatList : UINT;
END_VAR

  pComdefT := pComdef$^comdefT;
  retval   := COMLINK_ERR_GENERAL;
  
  DEBUGLOG1("%010d-ComlinkT_Login: ip-address=0x%08X", pComdefT^.ipAddress$UDINT);

  IF isClientReady < 0 THEN
    DEBUGLOG0("%010d-*** ComlinkT_Login: client is not ready");
    TraceMsg4Client();
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;

  IF (pComdefT^.interface <> IF_TYPE_TCPIP1) & 
     (pComdefT^.interface <> IF_TYPE_TCPIP1_NEW) THEN
    // no other interface type than TCPIP1 is supported in the moment.
    DEBUGLOG0("%010d-*** ComlinkT_Login: invalid interface type");
    retval := COMLINK_ERR_UNSUPPORTED_INTERFACE;
    RETURN;
  END_IF;

  IF (pComdefT^.interface = IF_TYPE_TCPIP1_NEW) & (pComdefT^.port <> 0) THEN
    cmdPortSrvr  := pComdefT^.port;
  ELSE
    cmdPortSrvr  := 1955;
  END_IF;
  refrPortSrvr := cmdPortSrvr + 1;

  // try to get a connection context
  pLogin := AllocateLoginContext(pComdefT^.ipAddress, cmdPortSrvr);
  IF pLogin = NIL THEN
    DEBUGLOG0("%010d-*** ComlinkT_Login: AllocateLoginContext failed");
    RETURN;
  END_IF;

  // establish a connection to the command server
  pLogin^.cmdSockClnt.sock := CreateSocket();
  IF pLogin^.cmdSockClnt.sock < 0 THEN
    DEBUGLOG1("%010d-*** ComlinkT_Login: CreateSocket failed, rc=%d", pLogin^.cmdSockClnt.sock$UDINT);
    GOTO EndLabel;
  END_IF;
  rc := Connect(pLogin^.cmdSockClnt.sock, 
                pLogin^.ipAddrSrvr, 
                cmdPortSrvr, 
                CLIENT_TIMEOUT_CONNECT
                );
  IF rc < 0 THEN                                 
    DEBUGLOG1("%010d-*** ComlinkT_Login: Connect to command server failed, rc=%d", TO_UDINT(rc));
    retval := COMLINK_ERR_NOCONNECTION;
    GOTO EndLabel;
  END_IF;
  DEBUGLOG2("%010d-ComlinkT_Login: connected to command server (port:%d, sock%d)", cmdPortSrvr$UDINT, pLogin^.cmdSockClnt.sock$UDINT);

  // establish a connection to the refresh server
  pLogin^.refrSockClnt.sock := CreateSocket();
  IF pLogin^.refrSockClnt.sock < 0 THEN
    DEBUGLOG1("%010d-*** ComlinkT_Login: CreateSocket failed, rc=%d", pLogin^.refrSockClnt.sock$UDINT);
    GOTO EndLabel;
  END_IF;
  rc := Connect(pLogin^.refrSockClnt.sock, 
                pLogin^.ipAddrSrvr, 
                refrPortSrvr, 
                CLIENT_TIMEOUT_CONNECT
                );
  IF rc < 0 THEN                                 
    DEBUGLOG1("%010d-*** ComlinkT_Login: Connect to refresh server failed, rc=%d", TO_UDINT(rc));
    retval := COMLINK_ERR_NOCONNECTION;
    GOTO EndLabel;
  END_IF;
  DEBUGLOG2("%010d-ComlinkT_Login: connected to refresh server (port:%d, sock%d)", refrPortSrvr$UDINT, pLogin^.refrSockClnt.sock$UDINT);

  // exchange session info with the server
  sizeofRemoteDynList  := pLogin^.sizeofRemoteDynList;
  sizeofRemoteStatList := pLogin^.sizeofRemoteStatList;
  rc := ExchgSessionInfo(#pLogin^.refrSockClnt, 
                         pComdef$UDINT, 
                         CLIENT_TIMEOUT_REQ, 
                         #versionOfPeer, 
                         #instIdOfPeer,
                         #sizeofRemoteDynList,
                         #sizeofRemoteStatList);
  IF rc <= 0 THEN
    DEBUGLOG2("%010d-*** ComlinkT_Login(sock %d): ExchgSessionInfo failed, rc=%d", pLogin^.refrSockClnt$UDINT, TO_UDINT(rc));
    retval := COMLINK_ERR_NOCONNECTION;
    GOTO EndLabel;
  END_IF;

  retval := 0;
  pLogin^.versionOfPeer := versionOfPeer;
  pLogin^.instIdOfPeer  := instIdOfPeer;
  pLogin^.sizeofRemoteDynList  := sizeofRemoteDynList;
  pLogin^.sizeofRemoteStatList := sizeofRemoteStatList;
  pLogin^.pComdefT      := pComdefT; // this pointer is needed when the callback function is called
  pLogin^.cmdSockClnt.isInstIdVerified  := INSTID_MATCH;
  pLogin^.refrSockClnt.isInstIdVerified := INSTID_MATCH;

  // Die SessionInfo wird jetzt auch über die CmdConn geschickt, damit der Server die Chance 
  // hat inaktive CmdConns zu erkennen und freizugeben. Der Returncode wird nicht ausgewertet, 
  // das es alte Loaderversionen geben kann, die auf der CmdConn den SessionInfo Befehl nicht 
  // verstehen.
  ExchgSessionInfo(#pLogin^.cmdSockClnt, pComdef$UDINT, CLIENT_TIMEOUT_REQ, NIL, NIL, NIL, NIL);

EndLabel:
  IF retval <> 0 THEN
    // something was wrong, cleanup now
    ReleaseLoginContext(pLogin);
  ELSE
    // success: try to put pLogin into loginArray
    OS_MT_Wait(loginArrayMutex);
    
    // search for a connection context already in use
    FOR i := 0 TO (g_max_logins - 1) DO
      IF loginArray[i] <> NIL THEN
        IF (loginArray[i]^.ipAddrSrvr = pLogin^.ipAddrSrvr) & 
           (loginArray[i]^.cmdPortSrvr = pLogin^.cmdPortSrvr) THEN
          // address is already in use
          IF loginArray[i]^.pComdefT = pComdefT THEN
            // gleiches pcomdef bedeutet reconnect -> das bisgherige pcomdef wird daher freigegeben
            (* 
               In ReleaseLoginContext (aus UnlockLoginContext aufgerufen, falls refCnt 0 ist), 
               wird pLogin^.pComdefT^.pLogin auf NIL gesetzt wird, falls pLogin^.pComdefT <> NIL ist.
               Es soll damit verhindert werden, dass in der pComdefT Struktur, die ja vom 
               Aufrufer bereitgestellt wird, ein Verweis auf die freigegebene Login-Struktur 
               existiert. Weiter unten wird dann pComdefT^.pLogin auf die neue Login-Struktur gesetzt.
               
               Hier könnte es zu einem Fehler kommen, wenn ReleaseLoginContext in diesem 
               UnlockLoginContext noch nicht aufgerufen wird, weil der RefCnt > 0 ist. 
               Das kann passieren, wenn der ComlinkT_Login Aufruf aus dem Background erfolgt, 
               während im Cyclic gerade der Login-Context gelockt ist (ComlinkT_CyWork_Client).
               Wenn dann ReleaseLoginContext aufgerufen wird, nachdem weiter unten pComdefT^.pLogin 
               auf den neuen Wert gesetzt wurde, dann wird pComdefT^.pLogin in ReleaseLoginContext 
               wieder auf 0 gesetzt.
               
               Um das zu verhindern, wird hier die Referenz auf die Comdef Struktur des Aufrufers 
               auf NIL gesetzt.
            *)
            loginArray[i]^.pComdefT := NIL;
            UnlockLoginContext(loginArray[i]);
            EXIT;
          END_IF;
          // bei ungleichem pcomdef wird ein neuer channel aufgemacht (vor 2.6 wurde ADDR_IN_USE zurückgegeben)
        END_IF;
      END_IF;
    END_FOR;
    
    // search a free entry in loginArray
    FOR i := 0 TO (g_max_logins - 1) DO
      IF loginArray[i] = NIL THEN 
        EXIT;
      END_IF;
    END_FOR;
    IF i = g_max_logins THEN
      retval := COMLINK_ERR_MAXCONN;
      OS_MT_Signal(loginArrayMutex);
      ReleaseLoginContext(pLogin);
      RETURN;
    END_IF;
  
    pComdefT^.pLogin := pLogin;
    // make an entry in the loginArray
    loginArray[i] := pLogin;
    loginAnz += 1;
    
    OS_MT_Signal(loginArrayMutex);
  END_IF ;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_Logout
//
//  Close a connection to a comlink server.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_Logout
VAR_INPUT
    pComdef       : ^comdef; 
END_VAR
VAR
    pComdefT      : ^comdefT; 
    pLogin        : ^loginContext; // connection context
    startTime     : udint;
END_VAR

  pComdefT := pComdef$^comdefT;
  
  IF isClientReady < 0 THEN
    DEBUGLOG0("%010d-*** ComlinkT_Logout: client is not ready");
    TraceMsg4Client();
    RETURN;
  END_IF;

  pLogin := pComdefT^.pLogin;
  IF pLogin = NIL THEN
    RETURN;
  END_IF;
  IF pLogin^.instIdOfPeer = 0x1AFEE THEN
    // es hat bereits ein Logout stattgefunden
    RETURN;
  END_IF;
  // Da instIdOfPeer ab jetzt nicht mehr benötigt wird, wird der Magic Value 0x1AFEE 
  // in instIdOfPeer dafür verwendet, einen doppelten Logout Aufruf zu erkennen.
  pLogin^.instIdOfPeer := 0x1AFEE; 
  
  LockLoginContext2(pLogin);
  
  SendLogout( pComdefT );
  
  // 20ms nach dem Senden des Logout warten
  startTime := ops.tAbsolute;
  WHILE (ops.tAbsolute - startTime) < 20 DO
    OS_MT_Delay(1);
  END_WHILE;
  
  UnlockLoginContext(pLogin); // wg. dem LockLoginContext2 Aufruf in dieser Funktion
  UnlockLoginContext(pLogin); // wg. dem LockLoginContext im ComlinkT_Login

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_TxCommandEx
//
//  This function send interpreter code to the comlink server. The server 
//  executes the code and responds with the the result and the workstate.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_TxCommandEx
VAR_INPUT
    opCode    : UDINT;
    length    : UDINT;
    aPara     : ^USINT;
    pComdef   : ^comdef; 
    pResu     : ^UDINT;
    pStatus   : ^IPRSTATES;
    res       : UDINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR
VAR
    pComdefT  : ^comdefT; 
    pLogin    : ^loginContext; // connection context
    reqMsg    : msgBefiprReq; // request structure for command CMD_BEFIPR
    reqMsgLen : UDINT;
    fOk       : UDINT;
    ack       : UDINT;
    rc        : DINT;
    rc2       : comlinkReason;
END_VAR

  pComdefT := pComdef$^comdefT;
  pStatus^ := ERROR;
  retval   := COMLINK_ERR_GENERAL;
  fOk   := 0;
  
  IF isClientReady < 0 THEN
    DEBUGLOG0("%010d-*** ComlinkT_TxCommandEx: client is not ready");
    TraceMsg4Client();
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;

  pLogin := pComdefT^.pLogin;
  IF pLogin = NIL THEN
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;
  LockLoginContext2(pLogin);
  
  rc2 := CheckClientConnection(pLogin, #pLogin^.cmdSockClnt, CLIENT_TIMEOUT_REQ);
  IF rc2 <> COMLINK_OK THEN
    retval := rc2;
    UnlockLoginContext(pLogin); // wg. dem LockLoginContext2 Aufruf in dieser Funktion
    RETURN;
  END_IF;

  pResu^ := (#pLogin^.befiprResp.resu)$UDINT;
  
  IF pLogin^.cmdSockClnt.sock < 0 THEN
    // no connection exists
    DEBUGLOG0("%010d-*** ComlinkT_TxCommandEx: no connection exists");
    retval := COMLINK_ERR_NOCONNECTION;
    UnlockLoginContext(pLogin); // wg. dem LockLoginContext2 Aufruf in dieser Funktion
    RETURN;
  END_IF;

  IF length > SIZEOF(reqMsg.instr.aPara) THEN
    // parameter length is too big
    DEBUGLOG0("%010d-*** ComlinkT_TxCommandEx: parameter length is too big");
    retval := COMLINK_ERR_INVALID_PARAM;
    UnlockLoginContext(pLogin); // wg. dem LockLoginContext2 Aufruf in dieser Funktion
    RETURN;
  END_IF;

  // prepare the request message structure
  reqMsg.workState    := BUSY;
  reqMsg.instr.opCode := opCode$INSTRSET;
  reqMsg.instr.uiLng  := (length + SIZEOF(instruct.uiLng))$UINT;
  _memcpy(#reqMsg.instr.aPara, aPara, length);

  reqMsgLen := SIZEOF(cmdMsgHdr)           +
               SIZEOF(reqMsg.workState)    +
               SIZEOF(reqMsg.instr.opCode) +
               SIZEOF(reqMsg.instr.uiLng)  +
               length;

  // Send the request
  rc := SendCmdMsg(#pLogin^.cmdSockClnt, 
                   CMD_BEFIPR,
                   MASK_CMD_ACK,      // Type and Ack field of command (type: request, ack: a response is required)
                   #reqMsg$cMessage, 
                   reqMsgLen, 
                   CLIENT_TIMEOUT_REQ,
                   0,                 // do not use buffered sends
                   0                  // fFlush
                  );
  IF rc <= 0 THEN
    DEBUGLOG1("%010d-*** ComlinkT_TxCommandEx: SendCmdMsg failed, rc=%d", TO_UDINT(rc));
    retval := COMLINK_ERR_NOCONNECTION;
    GOTO EndLabel;
  END_IF;
  DEBUG_VERBOSE2("%010d-ComlinkT_TxCommandEx: BEFIPR request sent, opCode=%d, msgLen=%d", opCode, TO_UDINT(rc));

  // Read the response
  rc := ReadCmdMsg(#pLogin^.cmdSockClnt, 
                   #pLogin^.befiprResp$cMessage, 
                   SIZEOF(pLogin^.befiprResp),
                   CLIENT_TIMEOUT_REQ,
                   #ack,                // received ack field
                   CMD_BEFIPR,
                   CMD_TYPE_RESP        // accept only response types
                  );
  IF rc <= 0 THEN
    DEBUGLOG1("%010d-*** ComlinkT_TxCommandEx: ReadCmdMsg failed, rc=%d", TO_UDINT(rc));
    retval := COMLINK_ERR_NOCONNECTION;
    GOTO EndLabel;
  END_IF;
  AliveCheckMsgRcvd(pLogin);
  
  IF rc < (SIZEOF(pLogin^.befiprResp.cHdr.cmd) + SIZEOF(pLogin^.befiprResp.workstate)) THEN
    // not enough data received !
    DEBUGLOG0("%010d-*** ComlinkT_TxCommandEx: not enough data received");
    retval := COMLINK_ERR_INVALID_RESPONSE;
    GOTO EndLabel;
  END_IF;

  fOk := 1; 
               
EndLabel:
  IF fOk = 0 THEN
    // The connection has to be closed because it's possible that partial data 
    // is transferred.
    DEBUGLOG0("%010d-ComlinkT_TxCommandEx: closing socket");
    CloseBufferedSock(#pLogin^.cmdSockClnt);
    pStatus^ := ERROR;
  ELSE
    IF ack THEN
      pStatus^ := pLogin^.befiprResp.workstate;
      DEBUG_VERBOSE2("%010d-ComlinkT_TxCommandEx: BEFIPR response received, dataLen=%d, Status=%d", TO_UDINT(rc-4), pStatus^$UDINT);
      //DEBUG_VERBOSE3("%010d-                    uiLng=%d, aData[0]=0x%08X, aData[4]=0x%08X", TO_UDINT(pLogin^.befiprResp.resu.uiLng), pLogin^.befiprResp.resu.aData[0]$UDINT, pLogin^.befiprResp.resu.aData[4]$UDINT);
      retval := COMLINK_IPR;
    ELSE
      pStatus^ := ERROR;
      DEBUGLOG0("%010d-ComlinkT_TxCommandEx: NACK received");
      retval := COMLINK_ERR_INVALID_RESPONSE;
    END_IF;
  END_IF;
  UnlockLoginContext(pLogin); // wg. dem LockLoginContext2 Aufruf in dieser Funktion

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_TxUpd_nocs
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_TxUpd_nocs
VAR_INPUT
    pLslcommregdata : ^lslCommregData;
    pLogin          : ^loginContext; // connection context
    uiflags         : UINT;
END_VAR
VAR_OUTPUT
    retval          : comlinkReason;
END_VAR
VAR
    reqMsg        : msgTxUpdReq;   // request structure for command CMD_TXUPD
    pt_comStCell  : ^comStCell;
    timeout_ms    : UDINT;         // timeout value for send request
    fFlush        : UDINT;
    updList       : ^updateList;
    listIndex     : UDINT;
    listType      : UDINT;
    rc            : DINT;
    rc2           : comlinkReason;
    timx          : udint;
END_VAR

  retval := COMLINK_ERR_GENERAL;
  
  rc2 := CheckClientConnection(pLogin, #pLogin^.refrSockClnt, 0);
  IF rc2 <> COMLINK_OK THEN
    retval := rc2;
    RETURN;
  END_IF;
  
  IF pLslcommregdata^.varpos$INT < 1000 THEN
    updList   := #pLogin^.statList;
    listIndex := pLslcommregdata^.varpos;
    listType  := 0;
  ELSE
    updList   := #pLogin^.dynList;
    listIndex := pLslcommregdata^.varpos - 1000;
    listType  := 1;
  END_IF;
  
  IF listIndex >= updList^.nbrCells THEN
    IF IncreaseUpdateList(updList, listIndex + 1, listType) = NIL THEN
      DEBUGLOG0("%010d-*** ComlinkT_TxUpd: MemoryUpdateList failed");
      CloseBufferedSock(#pLogin^.refrSockClnt);
      RETURN;
    END_IF;
    DEBUGLOG2("%010d-*** ComlinkT_TxUpd: list %d increased to %d entries", listType, updList^.nbrCells);
  END_IF;    

  pt_comStCell := updList^.pFirstCell + (listIndex * SIZEOF (comStCell));
  
  timx                       := pLslcommregdata^.uiTime; // u32 := u16 
  pt_comStCell^.dData        := 16#80000001$DINT;     // INVALID    
  pt_comStCell^.pCh          := pLslcommregdata^.LASALID$^SVRCH;
  pt_comStCell^.udUpdateRate := (timx and 16#3FFF) or ((timx and 16#C000) shl 16) or (uiflags shl 14);
  pt_comStCell^.udUpdateRate := pt_comStCell^.udUpdateRate or UPDRATE_FLAG_FORCE_CHANGE;
  pt_comStCell^.VarlistID    := pLslcommregdata^.VarlistID;
  pt_comStCell^.udLastUpdate := 0;

  // prepare the request message structure
  reqMsg.iOff    := pLslcommregdata^.varpos$UINT;
  reqMsg.comcell := pt_comStCell^;  // copy struct on struct;

  IF QUERY_FREE_TXBUF(pLogin^.refrSockClnt) < (2 * SIZEOF(reqMsg)) THEN
    DEBUGLOG0("%010d-*** ComlinkT_TxUpd: send buffer shortage, next send will block");
    timeout_ms := CLIENT_TIMEOUT_REQ;
    fFlush := 1;
  ELSE
    timeout_ms := 0;    // timeout: non-blocking
    fFlush := 0;
  END_IF;
  
  // Send the request
  rc := SendCmdMsg(#pLogin^.refrSockClnt, 
                   CMD_TXUPD,
                   0,                // Type and Ack field of command (type: request, ack: no response required)
                   #reqMsg$cMessage, 
                   SIZEOF(reqMsg), 
                   timeout_ms,        
                   1,                // use bufferd send
                   fFlush            // fFlush
                  );
  IF rc <= 0 THEN
    DEBUGLOG2("%010d-*** ComlinkT_TxUpd(VarPos %d): SendCmdMsg failed, rc=%d", pLslcommregdata^.varpos, TO_UDINT(rc));
    CloseBufferedSock(#pLogin^.refrSockClnt);
    retval := COMLINK_ERR_NOCONNECTION;
  ELSE
    DEBUG_VERBOSE3("%010d-ComlinkT_TxUpd: TXUPD request sent, msgLen=%d, VarPos=%d, time=0x%08X", TO_UDINT(rc), TO_UDINT(pLslcommregdata^.varpos), TO_UDINT(pLslcommregdata^.uiTime));
    retval := COMLINK_OK;
  END_IF;

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_TxUpd
//
// Return value:
//  If the function succeeds, the return value is 0, othzerwise it is a 
//  negative error code.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_TxUpd
VAR_INPUT
    pLslcommregdata : ^lslCommregData;
    pComdef         : ^comdef; 
    res             : UDINT;
    uiflags         : UINT;
END_VAR
VAR_OUTPUT
    retval          : comlinkReason;
END_VAR
VAR
    pLogin          : ^loginContext; // connection context
END_VAR

  IF isClientReady < 0 THEN
    DEBUGLOG0("%010d-*** ComlinkT_TxUpd: client is not ready");
    TraceMsg4Client();
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;
  
  pLogin := pComdef^$comdefT.pLogin;
  IF pLogin = NIL THEN
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;

  OS_MT_Wait(pLogin^.hMutex);
  retval := ComlinkT_TxUpd_nocs(pLslcommregdata, pLogin, uiflags);
  OS_MT_Signal(pLogin^.hMutex);
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// SendMaxIdleTime_TCP_nocs
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION SendMaxIdleTime_TCP_nocs
VAR_INPUT
  pComdefT : ^comdefT;
  remoteMaxIdleTime_ms : UDINT;
  pLogin          : ^loginContext; // connection context
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
    reqMsg        : msgIdlTimReq;  // request structure for command CMD_MAXIDLETIME
    timeout_ms    : UDINT;         // timeout value for send request
    fFlush        : UDINT;
    rc            : DINT;
    rc2           : comlinkReason;
END_VAR

  retval := COMLINK_ERR_GENERAL;
  
  rc2 := CheckClientConnection(pLogin, #pLogin^.refrSockClnt, 0);
  IF rc2 <> COMLINK_OK THEN
    retval := rc2;
    RETURN;
  END_IF;
  
  // prepare the request message structure
  reqMsg.maxIdleTime_ms := remoteMaxIdleTime_ms;

  IF QUERY_FREE_TXBUF(pLogin^.refrSockClnt) < (2 * SIZEOF(reqMsg)) THEN
    DEBUGLOG0("%010d-*** SendMaxIdleTime_TCP_nots: send buffer shortage, next send will block");
    timeout_ms := CLIENT_TIMEOUT_REQ;
    fFlush := 1;
  ELSE
    timeout_ms := 0;    // timeout: non-blocking
    fFlush := 0;
  END_IF;
  
  // Send the request
  rc := SendCmdMsg(#pLogin^.refrSockClnt, 
                   CMD_MAXIDLETIME,
                   0,                // Type and Ack field of command (type: request, ack: no response required)
                   #reqMsg$cMessage, 
                   SIZEOF(reqMsg), 
                   timeout_ms,        
                   1,                // use bufferd send
                   fFlush            // fFlush
                  );
  IF rc <= 0 THEN
    DEBUGLOG1("%010d-*** SendMaxIdleTime_TCP_nots: SendCmdMsg failed, rc=%d", TO_UDINT(rc));
    CloseBufferedSock(#pLogin^.refrSockClnt);
    retval := COMLINK_ERR_NOCONNECTION;
  ELSE
    DEBUG_VERBOSE0("%010d-SendMaxIdleTime_TCP_nots: MAXIDLETIME request sent");
    retval := COMLINK_OK;
  END_IF;

END_FUNCTION


FUNCTION SendLOGOUT_TCP_nocs
VAR_INPUT
  pComdefT : ^comdefT;
  pLogin          : ^loginContext; // connection context
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
    reqLOGOUT     : msgLOGOUTReq;
    rc            : DINT;
    rc2           : comlinkReason;
END_VAR

  retval := COMLINK_ERR_GENERAL;
  
  rc2 := CheckClientConnection(pLogin, #pLogin^.refrSockClnt, 0);
  IF rc2 <> COMLINK_OK THEN
    retval := rc2;
    RETURN;
  END_IF;
  
  rc := SendCmdMsg(#pLogin^.refrSockClnt, 
                   CMD_LOGOUT,
                   0,      // Type and Ack field of command (type: request, ack: a response is required)
                   #reqLOGOUT$cMessage, 
                   SIZEOF(reqLOGOUT), 
                   10000,
                   1,                 // use buffered sends
                   1                  // fFlush
                  );
                  
  IF rc <= 0 THEN
    DEBUGLOG1("%010d-*** SendLOGOUT_TCP_nots: SendCmdMsg failed, rc=%d", TO_UDINT(rc));
    CloseBufferedSock(#pLogin^.refrSockClnt);
    retval := COMLINK_ERR_NOCONNECTION;
  ELSE
    DEBUG_VERBOSE0("%010d-SendLOGOUT_TCP_nots: LOGOUT request sent");
    retval := COMLINK_OK;
    FLUSH_TXBUF( #pLogin^.refrSockClnt ); //safety
  END_IF;


  rc := SendCmdMsg(#pLogin^.cmdSockClnt, 
                   CMD_LOGOUT,
                   0,      // Type and Ack field of command (type: request, ack: a response is required)
                   #reqLOGOUT$cMessage, 
                   SIZEOF(reqLOGOUT), 
                   10000,
                   1,                 // use buffered sends
                   1                  // fFlush
                  );
                  
  IF rc <= 0 THEN
    DEBUGLOG1("%010d-*** SendLOGOUT_TCP_nots: SendCmdMsg failed, rc=%d", TO_UDINT(rc));
    CloseBufferedSock(#pLogin^.cmdSockClnt);
    retval := COMLINK_ERR_NOCONNECTION;
  ELSE
    DEBUG_VERBOSE0("%010d-SendLOGOUT_TCP_nots: LOGOUT request sent");
    retval := COMLINK_OK;
    FLUSH_TXBUF( #pLogin^.cmdSockClnt ); //safety
  END_IF;

END_FUNCTION

#endif

//------------------------------------------------------------------------------
// SendMaxIdleTime_TCP
//
// Return value:
//  If the function succeeds, the return value is 0, othzerwise it is a 
//  negative error code.
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION SendMaxIdleTime_TCP
VAR_INPUT
  pComdefT : ^comdefT;
  remoteMaxIdleTime_ms : UDINT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
    pLogin          : ^loginContext; // connection context
END_VAR

  IF isClientReady < 0 THEN
    DEBUGLOG0("%010d-*** ComlinkT_TxUpd: client is not ready");
    TraceMsg4Client();
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;
  
  pLogin := pComdefT^.pLogin;
  IF pLogin = NIL THEN
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;
  LockLoginContext2(pLogin);

  OS_MT_Wait(pLogin^.hMutex);
  retval := SendMaxIdleTime_TCP_nocs(pComdefT, remoteMaxIdleTime_ms, pLogin);
  OS_MT_Signal(pLogin^.hMutex);

  UnlockLoginContext(pLogin); // wg. dem LockLoginContext2 Aufruf in dieser Funktion

END_FUNCTION
#endif

#ifdef COMLINK_TCP_CLIENT
FUNCTION SendLogout_TCP
VAR_INPUT
  pComdefT : ^comdefT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
    pLogin          : ^loginContext; // connection context
END_VAR

  IF isClientReady < 0 THEN
    DEBUGLOG0("%010d-*** ComlinkT_TxUpd: client is not ready");
    TraceMsg4Client();
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;
  
  pLogin := pComdefT^.pLogin;
  IF pLogin = NIL THEN
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;
  LockLoginContext2(pLogin);

  OS_MT_Wait(pLogin^.hMutex);
  retval := SendLogout_TCP_nocs(pComdefT, pLogin);
  OS_MT_Signal(pLogin^.hMutex);
  
  UnlockLoginContext(pLogin); // wg. dem LockLoginContext2 Aufruf in dieser Funktion
  
END_FUNCTION
#endif


//------------------------------------------------------------------------------
// ComlinkT_InstallCallback
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_InstallCallback
VAR_INPUT
    pCallback : ^void;
END_VAR
VAR
    i         : UDINT;
    pLogin    : ^loginContext;
END_VAR

  IF isClientReady < 0 THEN
    DEBUGLOG0("%010d-*** ComlinkT_InstallCallback: client is not ready");
    RETURN; // zeile geloescht laut pranz mail vom 30.01.2004
  END_IF;

  DEBUGLOG1("%010d-ComlinkT_InstallCallback: pCallback=0x%08X", pCallback$UDINT);

  FOR i := 0 TO (g_max_logins - 1) DO
    pLogin := LockLoginContext(i);
    IF pLogin <> NIL THEN
      pLogin^.pCallback := pCallback;
      UnlockLoginContext(pLogin);
    END_IF;
  END_FOR;

  g_pCallback := pCallback;
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_StartStopRefresh_nocs
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_StartStopRefresh_nocs
VAR_INPUT
    pLogin   : ^loginContext; // connection context
    count    : UINT;
    typ      : UINT;
    res      : UDINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR
VAR
    reqMsg        : msgSsRefrReq;  // request structure for command CMD_SSREFR
    timeout_ms    : UDINT;         // timeout value for send request
    fFlush        : UDINT;
    rc            : DINT;
    rc2           : comlinkReason;
END_VAR

  retval := COMLINK_ERR_GENERAL;

  rc2 := CheckClientConnection(pLogin, #pLogin^.refrSockClnt, 0);
  IF rc2 <> COMLINK_OK THEN
    retval := rc2;
    RETURN;
  END_IF;

  // prepare the request message structure
  reqMsg.count   := count;
  reqMsg.typ     := typ;

  fFlush := 1;
  IF QUERY_FREE_TXBUF(pLogin^.refrSockClnt) < (2 * SIZEOF(reqMsg)) THEN
    DEBUGLOG0("%010d-*** ComlinkT_StartStopRefresh: send buffer shortage, next send will block");
    timeout_ms := CLIENT_TIMEOUT_REQ;
  ELSE
    timeout_ms := 0;    // timeout: non-blocking
  END_IF;
  
  pLogin^.accelerateCellUpdates := TRUE;
  
  // Send the request
  rc := SendCmdMsg(#pLogin^.refrSockClnt, 
                   CMD_SSREFR,
                   0,                // Type and Ack field of command (type: request, ack: no response required)
                   #reqMsg$cMessage, 
                   SIZEOF(reqMsg), 
                   timeout_ms,        
                   1,                // use buffered send
                   fFlush            // fFlush
                  );
  IF rc <= 0 THEN
    DEBUGLOG3("%010d-*** ComlinkT_StartStopRefresh(typ %d, count %d): SendCmdMsg failed, rc=%d", TO_UDINT(typ), TO_UDINT(count), TO_UDINT(rc));
    CloseBufferedSock(#pLogin^.refrSockClnt);
    retval := COMLINK_ERR_NOCONNECTION;
  ELSE
    DEBUG_VERBOSE3("%010d-ComlinkT_StartStopRefresh: SSREFR request sent, msgLen=%d, typ=%d, count=%d", TO_UDINT(rc), TO_UDINT(typ), TO_UDINT(count));
    retval := COMLINK_OK;
  END_IF;
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_StartStopRefresh
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_StartStopRefresh
VAR_INPUT
    pComdef  : ^comdef; 
    count    : UINT;
    typ      : UINT;
    res      : UDINT;
END_VAR
VAR_OUTPUT
    retval   : comlinkReason;
END_VAR
VAR
    pLogin        : ^loginContext; // connection context
END_VAR

  IF isClientReady < 0 THEN
    DEBUGLOG0("%010d-*** ComlinkT_StartStopRefresh: client is not ready");
    TraceMsg4Client();
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;
  pLogin := pComdef^$comdefT.pLogin;
  IF pLogin = NIL THEN
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;

  OS_MT_Wait(pLogin^.hMutex);
  retval := ComlinkT_StartStopRefresh_nocs(pLogin, count, typ, res);
  OS_MT_Signal(pLogin^.hMutex);
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_CyWork_Client - cyclic work for client
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_CLIENT
FUNCTION ComlinkT_CyWork_Client
VAR
    pLogin      : ^loginContext;
    reqMsgNop   : msgNopReq;
    rc          : DINT;
#ifndef LOADER_TCP_MODERATE
    i           : UDINT;
#endif
END_VAR

  IF (isClientReady < 0) | (loginAnz = 0) THEN
    RETURN;
  END_IF;

  TRACE_ENTRY(200);
    
#ifndef LOADER_TCP_MODERATE 
  FOR i := 0 TO (g_max_logins - 1) DO
  
    pLogin := LockLoginContext(i);
#else
    pLogin := LockLoginContext(TCPCltCntr);
#endif

    IF pLogin <> NIL THEN
    
      TRACE_ENTRY(210);
    
      IF (pLogin^.cmdSockClnt.sock < 0) & (pLogin^.cmdSockClnt.pndSock >= 0) THEN
        // a pending command connection exists
        rc := Connect(pLogin^.cmdSockClnt.pndSock, pLogin^.ipAddrSrvr, pLogin^.cmdPortSrvr, 0);
        IF rc >= 0 THEN
          pLogin^.cmdSockClnt.sock    := pLogin^.cmdSockClnt.pndSock;
          pLogin^.cmdSockClnt.pndSock := INVALID_SOCKET_NUM;
          DEBUGLOG1("%010d-ComlinkT_CyWork: client connected to command server (port:%d)", pLogin^.cmdPortSrvr$UDINT);
        ELSIF rc <> TCP_NOT_READY THEN
          CloseSock(#pLogin^.cmdSockClnt.pndSock);
          DEBUGLOG2("%010d-ComlinkT_CyWork: connect to command server (port:%d) failed, rc=%d", pLogin^.cmdPortSrvr$UDINT, TO_UDINT(rc));
        END_IF;
      END_IF;          
      
      IF (pLogin^.refrSockClnt.sock < 0) & (pLogin^.refrSockClnt.pndSock >= 0) THEN
        // a pending refresh connection exists
        rc := Connect(pLogin^.refrSockClnt.pndSock, pLogin^.ipAddrSrvr, pLogin^.cmdPortSrvr + 1, 0);
        IF rc >= 0 THEN
          pLogin^.refrSockClnt.sock    := pLogin^.refrSockClnt.pndSock;
          pLogin^.refrSockClnt.pndSock := INVALID_SOCKET_NUM;
          DEBUGLOG1("%010d-ComlinkT_CyWork: client connected to refresh server (port:%d)", (pLogin^.cmdPortSrvr + 1)$UDINT);
        ELSIF rc <> TCP_NOT_READY THEN
          CloseSock(#pLogin^.refrSockClnt.pndSock);
          DEBUGLOG2("%010d-ComlinkT_CyWork: connect to refresh server (port:%d) failed, rc=%d", (pLogin^.cmdPortSrvr + 1)$UDINT, TO_UDINT(rc));
        END_IF;
      END_IF;

      IF OS_MT_WaitCond(pLogin^.hMutex) THEN
        IF CheckClientConnection(pLogin, #pLogin^.refrSockClnt, 0) <> COMLINK_OK THEN
        
          TRACE_ENTRY(211);
          // poll for a response of the sessioninfo request
          PndConnListener(pLogin);
          
        ELSE    

          TRACE_ENTRY(212);
          // flush the buffer when no transfer was in the last SMALLEST_UPD_INTERVALL ms
          IF (pLogin^.refrSockClnt.bytesInTxBuf > 0) &
             ((ops.tAbsolute - pLogin^.refrSockClnt.lastSendTime) > SMALLEST_UPD_INTERVALL) THEN
            DEBUG_VERBOSE1("%010d-ComlinkT_CyWork: initiate transfer of bufferd data (%d bytes)", pLogin^.refrSockClnt.bytesInTxBuf);
            FLUSH_TXBUF(#pLogin^.refrSockClnt);
          END_IF;
          
          // Send data at a regular interval to get informed when the server is not alive.
          // Without sending data at a regular interval we would recognize a connection failure 
          // only when the LSE kernel initiates a transfer.
          // When we know that the server does not respond, we can initiate the reconnect procedure.
          IF (ops.tAbsolute - pLogin^.refrSockClnt.lastSendTime) > HEARTBEAT_INTERVALL THEN
            DEBUG_VERBOSE0("%010d-ComlinkT_CyWork: sending heartbeat");
            SendCmdMsg(#pLogin^.refrSockClnt, CMD_NOP, 0, #reqMsgNop$cMessage, SIZEOF(reqMsgNop), 0, 1, 1);
          END_IF;
          
          // send data in the buffer that is marked as unsent
          SendUnsentData(#pLogin^.refrSockClnt);

          // process incoming update cell requests
          IF ((ops.tAbsolute - pLogin^.refrSockClnt.lastUpdTime) > SMALLEST_UPD_INTERVALL) | pLogin^.accelerateCellUpdates THEN
            WHILE 1 DO
              IF UpdCellListener(pLogin) = 0 THEN
                // UpdCellListener returns 0 when no message is received
                EXIT;
              ELSE
                pLogin^.accelerateCellUpdates := FALSE;
              END_IF;
            END_WHILE;
            pLogin^.refrSockClnt.lastUpdTime := ops.tAbsolute;
          END_IF;

        END_IF;
        OS_MT_Signal(pLogin^.hMutex);
      END_IF; // OS_MT_WaitCond(pLogin^.hMutex)
      
      TRACE_ENTRY(219);
      
      UnlockLoginContext(pLogin);
      
    END_IF;

#ifdef LOADER_TCP_MODERATE
    TCPCltCntr += 1;
    if (TCPCltCntr = g_max_logins) then
      TCPCltCntr := 0;
    end_if;
#else    
  END_FOR;
#endif

  TRACE_ENTRY(299);

END_FUNCTION
#endif


//------------------------------------------------------------------------------
// Ruft das Accept für den Command Socket auf
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION ComlinkT_AcceptCmdSock
VAR
    sock        : DINT;
END_VAR

  IF cmdSockSrvr >= 0 THEN
    sock := OS_TCP_USER_ACCEPT(cmdSockSrvr, 0);
    IF sock >= 0 THEN
      DEBUGLOG1("%010d-  =====  command client connected  =====  , sock=%d", sock$UDINT);
      IF sock < arraysize(usedsocks) THEN
        usedsocks[sock] := 'C';
      END_IF;
      SetSocketOptions(sock);
      IF AllocConnBlock(TYPE_CMD_CONN, sock) = NIL THEN
        CloseSock(#sock);
      END_IF;
    END_IF;
  END_IF;
  
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// Ruft das Accept für den Refresh Socket auf
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION ComlinkT_AcceptRefrSock
VAR
    sock        : DINT;
END_VAR

  IF refrSockSrvr >= 0 THEN
    sock := OS_TCP_USER_ACCEPT(refrSockSrvr, 0);
    IF sock >= 0 THEN
      DEBUGLOG1("%010d-  =====  refresh client connected  =====  , sock=%d", sock$UDINT);
      IF sock < arraysize(usedsocks) THEN
        usedsocks[sock] := 'R';
      END_IF;
      SetSocketOptions(sock);
      IF AllocConnBlock(TYPE_REFR_CONN, sock) = NIL THEN
        CloseSock(#sock);
      END_IF;
    END_IF;
  END_IF;
    
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// Serviciert einen offenen Command Socket
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION ComlinkT_ServeCmdSock
VAR_INPUT
    pCmdConn    : ^cmdConnBlock;
END_VAR
#ifdef LOADER_TCP_MODERATE
VAR
	rc : DINT;
END_VAR
#endif

    // send data in the buffer that is marked as unsent
    SendUnsentData(#pCmdConn^.bsock);

    // process incoming command requests
    #ifdef LOADER_TCP_MODERATE
    rc := 
    #endif
    CmdMsgListener(pCmdConn);

    #ifdef LOADER_TCP_MODERATE
    IF rc >= 0 THEN
      pCmdConn^.bSock.isDataAvailable := false;
    END_IF;
    #endif

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// Serviciert einen offenen Refresh Socket
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION ComlinkT_ServeRefrSock
VAR_INPUT
    pRefrConn   : ^refrConnBlock;
END_VAR
VAR
    i           : UDINT;
    rc : DINT;
END_VAR

    // flush the buffer when no transfer was in the last SMALLEST_UPD_INTERVALL ms
    IF (pRefrConn^.bsock.bytesInTxBuf > 0) &
       ((ops.tAbsolute - pRefrConn^.bsock.lastSendTime) > SMALLEST_UPD_INTERVALL) THEN
      DEBUG_VERBOSE1("%010d-ComlinkT_CyWork: initiate transfer of bufferd data (%d bytes)", pRefrConn^.bsock.bytesInTxBuf);
      FLUSH_TXBUF(#pRefrConn^.bsock);
    END_IF;
    // send data in the buffer that is marked as unsent
    SendUnsentData(#pRefrConn^.bsock);
    
    // process incoming requests (TXUPD, StartStopRefresh)
    IF pRefrConn^.bsock.lastSendTime_ms <> ops.tAbsolute | 
       pRefrConn^.bsock.refrMsgListenInhibitCnt > 5 // damit der RefrMsgListener auch eine Chance hat
       THEN
      // Es wird nur ein Receive durchgeführt, wenn in diesem Durchlauf nichts 
      // gesendet wurde. Das begrenzt die CPU Last.
      i := 0;
      WHILE 1 DO
        rc := RefrMsgListener(pRefrConn);
        IF rc = 0 THEN
          // RefrMsgListener returns 0 when no message is received
          EXIT;
        END_IF;
        IF rc < 0 THEN
          // pRefrConn was released
          RETURN;
        END_IF;
        i += 1;
        IF i > 500 THEN // for safety
          EXIT;
        END_IF;
      END_WHILE;
      pRefrConn^.bsock.refrMsgListenInhibitCnt := 0;
    ELSE
      pRefrConn^.bsock.refrMsgListenInhibitCnt += 1;
    END_IF;
    // scan the update list and send changes to the client
    IF pRefrConn^.bsock.fTxBufferShortage THEN
      // no update in the case ot a buffer shortage
      IF QUERY_FREE_TXBUF(pRefrConn^.bsock) > (pRefrConn^.bsock.sizeOfTxBuf / 2) THEN
        DEBUGLOG0("%010d-ComlinkT_CyWork: leaving tx buffer shortage");
        pRefrConn^.bsock.fTxBufferShortage := 0;
      END_IF;
    ELSE
      IF ((ops.tAbsolute - pRefrConn^.bsock.lastUpdTime) > SMALLEST_UPD_INTERVALL) | pRefrConn^.flushNextSends THEN
        UpdateListProcessor(pRefrConn);
        pRefrConn^.bsock.lastUpdTime := ops.tAbsolute;
        IF pRefrConn^.flushNextSends THEN
          FLUSH_TXBUF(#pRefrConn^.bsock);
          pRefrConn^.flushNextSends := FALSE;
        END_IF;
      ELSE
        prefrconn := prefrconn;
      END_IF;
    END_IF;
    
    if( pRefrConn^.ptr = NIL )then
      return;
    end_if;
    IF pRefrConn^.ptr^.maxIdleTime_ms > 0 THEN
      IF ops.tAbsolute - pRefrConn^.bsock.lastSendTime_ms > pRefrConn^.ptr^.maxIdleTime_ms THEN
        SendAliveMessage_TCP(pRefrConn); // der Gegenstelle mitteilen, daß man noch lebt
      END_IF;
    END_IF;
    
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_CyWork_Server - cyclic work for server
//------------------------------------------------------------------------------
#ifdef COMLINK_TCP_SERVER
FUNCTION ComlinkT_CyWork_Server
VAR
    pCmdConnOld : ^cmdConnBlock;
    pRefrConnOld: ^refrConnBlock;
END_VAR

  IF isServerReady < 0 THEN
    RETURN;
  END_IF;
  
  // Die Aufrufe für das Accept des Kommando- und Refreshkanals und die 
  // Bedienung dieser Kanäle wird so aufgeteilt, daß bei jedem Durchlauf 
  // nur ein Aufruf gemacht wird, wobei die Accepts nur jedes 50te mal 
  // aufgerufen werden. Ein Verbindungsaufbau braucht nämlich nicht so oft 
  // überwacht werden.
  // Der Grund für diese Aufteilung ist der hohe CPU Verbrauch eines 
  // TCP/IP Calls. 
  
  
  CASE tcpServerState OF
    0:
      TRACE_ENTRY(100);
      // accept incoming command connection
      ComlinkT_AcceptCmdSock();
      tcpServerState += 1;
      
    1:
      TRACE_ENTRY(101);
      // accept incoming refresn connection
      ComlinkT_AcceptRefrSock();
      tcpServerState += 1;
  
    2:
      TRACE_ENTRY(102);
      // cyclic work for refresh server
      IF s_pRefrConn = NIL THEN
        s_pRefrConn := pRefrConnRoot;
      END_IF;
      IF s_pRefrConn <> NIL THEN
    
        ComlinkT_ServeRefrSock(s_pRefrConn);
    
        // check if an error occured
        IF s_pRefrConn <> NIL THEN // s_pRefrConn kann in ComlinkT_ServeRefrSock freigegeben werden
	        IF s_pRefrConn^.bsock.sock < 0 THEN
	          pRefrConnOld := s_pRefrConn;
	          s_pRefrConn    := s_pRefrConn^.next;    
            DEBUGLOG0("%010d-  =====  Error in RefrConn -> releasing connBlock  =====");
	          ReleaseConnBlock(TYPE_REFR_CONN, pRefrConnOld$^connBlock, 1);
            pRefrConnOld := NIL;
	        ELSE
	          s_pRefrConn := s_pRefrConn^.next;    
	        END_IF;
        END_IF;
        
      END_IF;
      IF s_pRefrConn = NIL THEN
        tcpServerState += 1;
      END_IF;
      
    3:
      TRACE_ENTRY(103);
      // cyclic work for command server
      IF s_pCmdConn = NIL THEN
        s_pCmdConn := pCmdConnRoot;
      END_IF;
      IF s_pCmdConn <> NIL THEN
      
        ComlinkT_ServeCmdSock(s_pCmdConn);
                
        // check if an error occured
        IF s_pCmdConn <> NIL THEN // s_pCmdConn kann in ComlinkT_ServeCmdSock freigegeben werden
	        IF s_pCmdConn^.bsock.sock < 0 THEN
	          pCmdConnOld := s_pCmdConn;
	          s_pCmdConn    := s_pCmdConn^.next;    
            DEBUGLOG0("%010d-  =====  Error in CmdConn -> releasing connBlock =====");
	          ReleaseConnBlock(TYPE_CMD_CONN, pCmdConnOld$^connBlock, 0);
            pCmdConnOld := NIL;
	        ELSE
	          s_pCmdConn := s_pCmdConn^.next;    
	        END_IF;
        END_IF;
    
      END_IF;
      IF s_pCmdConn = NIL THEN
        tcpServerState += 1;
      END_IF;

  END_CASE;
  
  tcpAcceptState += 1;
  
  IF tcpServerState > 3 THEN
    // Es wird nur alle 50 Durchläufe ein Accept eingeschoben
    IF tcpAcceptState > 50 THEN
      tcpAcceptState := 0;
      tcpServerState := 0;
    ELSE
      tcpServerState := 2;
    END_IF;
  END_IF;

  TRACE_ENTRY(199);

END_FUNCTION
#endif

//------------------------------------------------------------------------------
// ComlinkT_CyWork
//------------------------------------------------------------------------------
FUNCTION ComlinkT_CyWork

  TRACE_ENTRY(10);

#ifdef COMLINK_TCP_SERVER
  ComlinkT_CyWork_Server();
#endif  

  TRACE_ENTRY(11);

#ifdef COMLINK_TCP_CLIENT
  ComlinkT_CyWork_Client();
#endif

  TRACE_ENTRY(12);

END_FUNCTION

#endif //COMLINK_TCP

#ifndef COMLINK_TCP_CLIENT

//------------------------------------------------------------------------------
// ComlinkT_MissingClientCode
//------------------------------------------------------------------------------
FUNCTION ComlinkT_MissingClientCode

  IF ComlinkT_wasTraceMsgDisplayed = 0 THEN
    TRACE_ERR("Comlink/TCP client error: missing client code (COMLINK_TCP_CLIENT not defined)");
    LSLCLI_LoaderTracePrint("Comlink/TCP client error: missing client code (COMLINK_TCP_CLIENT not defined)", 12);
    ComlinkT_wasTraceMsgDisplayed := 1;
  END_IF;
  
END_FUNCTION

#endif // !COMLINK_TCP_CLIENT

//++comlinkt:end



////////////////////////////////////////////////////////////////////////////////
//
//  I-am-Alive Mechanismus
//
////////////////////////////////////////////////////////////////////////////////


FUNCTION SendMaxIdleTime_CAN
VAR_INPUT
  pComdefT : ^comdefT;
  remoteMaxIdleTime_ms : UDINT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
  txinfo4006    : COMOBJSTR;
END_VAR

    txinfo4006.iLng  := 0;
    txinfo4006.iOff  := 4006;
    txinfo4006.dData := TO_DINT(remoteMaxIdleTime_ms);

    IF CanTxObj(pComdefT$^COMDEF^.pt_COM, #txinfo4006$void) <> 0 THEN
        retVal := -1;
    ELSE
        retVal := 0;    
    END_IF;

END_FUNCTION
FUNCTION SendAliveMessage_CAN
VAR_INPUT
  pCom : ^COMDATA;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
  txinfo4007    : COMOBJSTR;
END_VAR

    txinfo4007.iLng  := 0;
    txinfo4007.iOff  := 4007;
    txinfo4007.dData := 0;

    IF CanTxObj(pCom, #txinfo4007$void) <> 0 THEN
        retVal := -1;
    ELSE
        retVal := 0;    
    END_IF;

END_FUNCTION
//------------------------------------------------------------------------------
// SendMaxIdleTime
//
//  Schickt die maxIdleTime zur Gegenstation
//------------------------------------------------------------------------------
FUNCTION SendMaxIdleTime
VAR_INPUT
  pComdefT : ^comdefT;
  remoteMaxIdleTime_ms : UDINT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR

  CASE pComdefT^.interface  OF

  COMLINK_IFNUM_CAN1,
  COMLINK_IFNUM_CAN2,
  COMLINK_IFNUM_CAN20,
  COMLINK_IFNUM_CAN21,
  COMLINK_IFNUM_CAN22,
  COMLINK_IFNUM_CAN23,
  COMLINK_IFNUM_CAN24,
  COMLINK_IFNUM_CAN25,
  COMLINK_IFNUM_CAN26,
  COMLINK_IFNUM_CAN27,
  COMLINK_IFNUM_CAN28,
  COMLINK_IFNUM_CAN29:
    retVal := SendMaxIdleTime_CAN(pComdefT, remoteMaxIdleTime_ms);
#ifdef COMLINK_TCP_SERVER
  COMLINK_IFNUM_TCP1_OLD,
  COMLINK_IFNUM_TCP1,
  COMLINK_IFNUM_TCP2:
    retVal := SendMaxIdleTime_TCP(pComdefT, remoteMaxIdleTime_ms);
#endif    
  COMLINK_IFNUM_COM1,
  COMLINK_IFNUM_COM2,
  COMLINK_IFNUM_COM3,
  COMLINK_IFNUM_COM4,
  COMLINK_IFNUM_COM5,
  COMLINK_IFNUM_COM6,
  COMLINK_IFNUM_COM7,
  COMLINK_IFNUM_COM8,
  COMLINK_IFNUM_COM9,
  COMLINK_IFNUM_COM10:
    retVal := SendMaxIdleTime_RS232(pComdefT, remoteMaxIdleTime_ms);
  ELSE
    retVal := -1;   
  END_CASE;

END_FUNCTION

//------------------------------------------------------------------------------
// SendMaxIdleTime
//
//  Schickt die maxIdleTime zur Gegenstation
//------------------------------------------------------------------------------
FUNCTION SendLogout
VAR_INPUT
  pComdefT : ^comdefT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR

  CASE pComdefT^.interface  OF

  COMLINK_IFNUM_CAN1,
  COMLINK_IFNUM_CAN2,
  COMLINK_IFNUM_CAN20,
  COMLINK_IFNUM_CAN21,
  COMLINK_IFNUM_CAN22,
  COMLINK_IFNUM_CAN23,
  COMLINK_IFNUM_CAN24,
  COMLINK_IFNUM_CAN25,
  COMLINK_IFNUM_CAN26,
  COMLINK_IFNUM_CAN27,
  COMLINK_IFNUM_CAN28,
  COMLINK_IFNUM_CAN29:
    //retVal := SendMaxIdleTime_CAN(pComdefT, remoteMaxIdleTime_ms);
#ifdef COMLINK_TCP_SERVER
  COMLINK_IFNUM_TCP1_OLD,
  COMLINK_IFNUM_TCP1,
  COMLINK_IFNUM_TCP2:
    retVal := SendLogout_TCP(pComdefT);
#endif    
  COMLINK_IFNUM_COM1,
  COMLINK_IFNUM_COM2,
  COMLINK_IFNUM_COM3,
  COMLINK_IFNUM_COM4,
  COMLINK_IFNUM_COM5,
  COMLINK_IFNUM_COM6,
  COMLINK_IFNUM_COM7,
  COMLINK_IFNUM_COM8,
  COMLINK_IFNUM_COM9,
  COMLINK_IFNUM_COM10:
    //retVal := SendMaxIdleTime_RS232(pComdefT, remoteMaxIdleTime_ms);
  ELSE
    retVal := -1;   
  END_CASE;

END_FUNCTION

//------------------------------------------------------------------------------
// AliveCheck
//
//  Überprüft bei einem Node:
//  a) ob die Zeit maxSilentTime_ms abgelaufen ist (dann wird angenommen, daß 
//     die Gegenstelle tot ist oder die Verbindung unterbrochen wurde)
//  b) ob der Gegenstelle eine Alive-Message geschickt worden ist (falls seit 
//     der letzten gesendeten Message schon zu viel Zeit vergangen ist)
//------------------------------------------------------------------------------
FUNCTION AliveCheck
VAR_INPUT
    pNode : ^MonitorNode;
END_VAR

    IF pNode^.maxSilentTime_ms > 0 THEN
        IF ops.tAbsolute - pNode^.lastRcvdTime_ms > pNode^.maxSilentTime_ms THEN
            pNode^.isRemotePlcAlive := FALSE;   // Flag, das anzeigt, daß die Gegenstelle nicht mehr antwortet
        END_IF;
    END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// AliveChecker
//
//  Geht die Liste aller zu überwachenden Nodes durch und ruft für jeden Node 
//  die Funktion AliveCheck auf.
//------------------------------------------------------------------------------
FUNCTION AliveChecker
VAR
  pWork : ^MonitorNodeList;
END_VAR

  // die folgende Abfrage vermeidet die Verwendung des monitorNodeMutex wenn nichts 
  // zu überwachen ist
  IF monitorNodes = NIL THEN
    RETURN;
  END_IF;

  OS_MT_WAIT(monitorNodeMutex);
  
  pWork := monitorNodes;
  WHILE pWork DO
    AliveCheck(pWork$^MonitorNode);
    pWork := pWork^.pNext;
  END_WHILE;
  
  OS_MT_SIGNAL(monitorNodeMutex);

END_FUNCTION 
//------------------------------------------------------------------------------
// SearchMonitorNode
//
//  Sucht einen Node in der Überwachungsliste anhand der in pComdefT 
//  gespeicherten Netzwerkadresse .
//------------------------------------------------------------------------------
(*

    Diese Funktion kann man verwenden, wenn man nicht pro Kanal sondern pro 
    Netzwerkadresse überwacht.

FUNCTION SearchMonitorNode
VAR_INPUT
  pComdefT : ^comdefT;
END_VAR
VAR_OUTPUT
  pNode : ^MonitorNode;
END_VAR
VAR
  pWork : ^MonitorNodeList;
END_VAR

  pNode := NIL;
  pWork := monitorNodes;

  WHILE pWork DO

    CASE pComdefT^.interface  OF

    COMLINK_IFNUM_CAN1:
      IF pComdefT^.canAddress = pWork$^MonitorNode^.canAddress THEN
        pNode := pWork$^MonitorNode;
        EXIT;
      END_IF;

    COMLINK_IFNUM_TCP1_OLD,
    COMLINK_IFNUM_TCP1,
    COMLINK_IFNUM_TCP2:
      IF pComdefT^.ipAddress = pWork$^MonitorNode^.ipAddress & 
         pComdefT^.port      = pWork$^MonitorNode^.port THEN
        pNode := pWork$^MonitorNode;
        EXIT;
      END_IF;
    END_CASE;

    pWork := pWork^.pNext;
  END_WHILE;

END_FUNCTION
*)

//------------------------------------------------------------------------------
// RemoveMonitorNode
//
//  Hängt einen Node aus der Überwachungsliste aus
//------------------------------------------------------------------------------
FUNCTION RemoveMonitorNode
VAR_INPUT
  pNode : ^MonitorNode;
END_VAR
VAR
  pPrev : ^MonitorNodeList;
  pWork : ^MonitorNodeList;
END_VAR

  pPrev := NIL;
  pWork := monitorNodes;

  WHILE pWork DO

    IF pWork$^MonitorNode = pNode THEN
      IF pPrev = NIL THEN
        monitorNodes := pWork^.pNext;
      ELSE
        pPrev^.pNext := pWork^.pNext;
      END_IF;
      _free(pWork);
      pWork := NIL;
      EXIT;
    END_IF;

    pPrev := pWork;
    pWork := pWork^.pNext;
  END_WHILE;

END_FUNCTION

//------------------------------------------------------------------------------
// AddMonitorNode
//
//  Hängt einen Node in die Überwachungsliste ein
//------------------------------------------------------------------------------
FUNCTION AddMonitorNode
VAR_INPUT
    pNode : ^MonitorNode;
END_VAR

    pNode$^MonitorNodeList^.pNext := monitorNodes;
    monitorNodes := pNode$^MonitorNodeList;

END_FUNCTION
//------------------------------------------------------------------------------
// DeactivateAliveCheck
//
//  Deaktiviert die Überwachung einer Comlink Gegenstelle.
//------------------------------------------------------------------------------
FUNCTION DeactivateAliveCheck
VAR_INPUT
  pComdefT : ^comdefT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
  pt_COM   : ^COMDATA;
  pNode : ^MonitorNode;
END_VAR

  retVal := SendMaxIdleTime(pComdefT, 0);

  pt_COM := pComdefT^.pLogin$^COMDATA;
  IF pt_COM <> NIL THEN
    OS_MT_WAIT(monitorNodeMutex);
    IF pt_COM^.pNode THEN
      pNode := pt_COM^.pNode;
      pt_COM^.pNode := NIL;
      RemoveMonitorNode(pNode);
    END_IF;
    OS_MT_SIGNAL(monitorNodeMutex);
  END_IF;

END_FUNCTION
//------------------------------------------------------------------------------
// ActivateAliveCheck
//
//  Aktiviert die Überwachung einer Comlink Gegenstelle. Der Timeout für die 
//  Überwachung wird mit dem Parameter maxSilentTime_ms angegeben.
//  remoteMaxIdleTime_ms wird der Gegenstelle mitgeteilt.
//------------------------------------------------------------------------------
FUNCTION ActivateAliveCheck
VAR_INPUT
  pComdefT : ^comdefT;
  maxSilentTime_ms : UDINT;
  remoteMaxIdleTime_ms : UDINT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
    pt_COM   : ^COMDATA;
    pNode : ^MonitorNode;
    pOldNode : ^MonitorNode;
END_VAR

    retVal := 0;

    pt_COM := pComdefT^.pLogin$^COMDATA;
    IF pt_COM = NIL THEN
      RETURN;
    END_IF;
    
    OS_MT_WAIT(monitorNodeMutex);
    pOldNode := pt_COM^.pNode;
    IF pOldNode = NIL THEN
        // für diesen Kanal war noch kein Eintrag vorhanden -> neuen Listeneintrag erstellen
        pNode := calloc(SIZEOF(MonitorNodeList))$^MonitorNode;
        IF pNode <> NIL THEN
            pNode^.pt_COM           := pt_COM;
            pNode^.isRemotePlcAlive := TRUE;
        END_IF;
    ELSE
        // bestehenden Listeneintrag wiederverwenden
        pNode := pOldNode;
    END_IF; 
    OS_MT_SIGNAL(monitorNodeMutex);

    IF pNode = NIL THEN
        retVal := -1;
        RETURN;
    END_IF;

    (*
    // prüfen, ob die bisherigen Einstellungen des Nodes geändert werden müssen
    -> nur notwendig, wenn pro Netzwerkadresse und nicht pro Kanal überwacht wird
    IF pOldNode = NIL | maxSilentTime_ms < pNode^.maxSilentTime_ms THEN
        IF pOldNode = NIL | remoteMaxIdleTime_ms <> pNode^.remoteMaxIdleTime_ms THEN
            // Es wurde entweder ein neuer Eintrag in der Überwachungsliste erstellt 
            // oder der neue Wert für die maxIdleTime ist anders als der bisherige.
            pNode^.remoteMaxIdleTime_ms := remoteMaxIdleTime_ms;
        END_IF;
        pNode^.maxSilentTime_ms := maxSilentTime_ms;
    END_IF;
    *)

    // Der neue Wert für remoteMaxIdleTime_ms wird in jedem Fall gesendet, auch 
    // wenn man annehmen müßte daß die Gegenstelle den aktuellen Wert schon hat.
    // Wenn nämlich die Gegenstelle resettiert worden ist, dann hat sie die Einstellungen 
    // vergessen.
    retVal := SendMaxIdleTime(pComdefT, remoteMaxIdleTime_ms);
    IF retVal < 0 THEN
        IF pOldNode = NIL THEN
            _free(pNode);
            pNode := NIL;
            RETURN;
        END_IF;
    END_IF;

    OS_MT_WAIT(monitorNodeMutex);
    pNode^.lastRcvdTime_ms := ops.tAbsolute;
    pNode^.maxSilentTime_ms := maxSilentTime_ms;
    pNode^.remoteMaxIdleTime_ms := remoteMaxIdleTime_ms;

    IF pOldNode = NIL THEN
        // neuen Node in die Überwachungs-Liste einhängen
        AddMonitorNode(pNode);
    END_IF;

    // Die Zeit-Werte merken, damit beim entfernen eines Nodes aus der Überwachungs-Liste 
    // ein passender neuer Eintrag gesucht werden kann.
    pt_COM^.pNode := pNode;
    OS_MT_SIGNAL(monitorNodeMutex);

END_FUNCTION
//------------------------------------------------------------------------------
// LDR_SetIamAliveTime
//
//  API Funktion zum aktivieren oder deaktivieren der Überwachung einer Comlink 
//  Gegenstelle.
//
//  pComdefT
//    Comdef Struktur der Comlink Verbindung
//  remoteMaxIdleTime_ms
//    'maxIdleTime' die der Gegenstelle mitgeteilt wird. Die Gegenstelle schickt 
//    eine Message, wenn sie feststellt daß seit der letzten gesendeten Message 
//    die Zeit 'maxIdleTime' vergangen ist.
//      >0  .. Wert in ms
//      -1  .. Default-Wert (100ms), 
//       0  .. deaktivieren
//  maxNetworkDelay_ms
//    Die maximale Netzwerkverzögerung für diese Verbindung. 
//     >=0  .. Wert in ms
//      -1  .. Default-Wert (20ms), 
//  remoteMaxCyclicTime_ms
//    Die max.Durchlaufzeit des Cyclics der Gegenstation (nur für den Fall daß 
//    die Gegenstelle diesen Wert nicht liefert).
//     >=0  .. Wert in ms
//      -1  .. Default-Wert (300ms), 
//
//  Return:
//    0  .. o.k.
//   <0  .. Fehler
//------------------------------------------------------------------------------
FUNCTION GLOBAL __CDECL LDR_SetIamAliveTime
VAR_INPUT
  pComdef : ^COMDEF;
  remoteMaxIdleTime_ms : UDINT;
  maxNetworkDelay_ms : UDINT;
  remoteMaxCyclicTime_ms : UDINT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR

    IF (pComdef = NIL) | (pComdef^.pt_Com = NIL) THEN
        retVal := -1;
        RETURN;
    END_IF;

    IF pComdef^.pt_COM^.remoteMaxCyclicTime_ms = 16#FFFFffff THEN
        // die Gegenstelle hat beim Login keine maxCyclicTime geschickt, 
        // d.h. daß die Loaderversion der Gegenstelle den Alive Mechanismus 
        // noch nicht implementiert hat. -> Fehlercode zurückgeben
        retVal := -1;
        RETURN;
    END_IF;

    IF remoteMaxIdleTime_ms = 0 THEN
        // Überwachung deaktivieren
        retVal := DeactivateAliveCheck(pComdef$^comdefT);
    ELSE
        // Defaultwerte übernehmen
        IF remoteMaxIdleTime_ms = 16#FFFFffff THEN
            remoteMaxIdleTime_ms := 100;
        END_IF;
        IF maxNetworkDelay_ms = 16#FFFFffff THEN
            maxNetworkDelay_ms := 20;
        END_IF;
        IF pComdef^.pt_COM^.remoteMaxCyclicTime_ms <> 0 THEN
            // die Gegenstelle hat für ihre eigene maxCyclicTime einen Wert <> 0 mitgeteilt
            remoteMaxCyclicTime_ms := pComdef^.pt_COM^.remoteMaxCyclicTime_ms;
        ELSIF remoteMaxCyclicTime_ms = 16#FFFFffff THEN
            // maxCyclicTime ist weder vom Aufrufer noch von der Gegenstelle mitgeteilt worden
            // -> Defaultwert verwenden.
            remoteMaxCyclicTime_ms := 300;
        END_IF;

        // Überwachung aktivieren
        retVal := ActivateAliveCheck(pComdef$^comdefT, 
                                     remoteMaxIdleTime_ms + maxNetworkDelay_ms + remoteMaxCyclicTime_ms, 
                                     remoteMaxIdleTime_ms);
    END_IF;

END_FUNCTION
//------------------------------------------------------------------------------
// LDR_isRemotePlcAlive
//
//  Mit dieser API Funktion kann festgestellt werden, ob von der Remote PLC 
//  regelmäßig etwas geschickt wird.
//
//  Return:
//    0  .. Die Remote PLC hat sich in letzter Zeit nicht mehr gemeldet
//   >0  .. Von der Remote PLC werden regelmäßig Daten empfangen
//------------------------------------------------------------------------------
FUNCTION GLOBAL __CDECL LDR_isRemotePlcAlive
VAR_INPUT
  pComdef : ^COMDEF;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR

    retVal := 0;
    IF pComdef & pComdef^.pt_COM & pComdef^.pt_COM^.pNode  THEN
        retVal := pComdef^.pt_COM^.pNode^.isRemotePlcAlive;
    END_IF;

END_FUNCTION
//------------------------------------------------------------------------------
// LDR_SetCanWait
//------------------------------------------------------------------------------
FUNCTION GLOBAL __CDECL LDR_SetCanWait //ws001
VAR_INPUT
  us_wait : usint;
END_VAR
VAR_OUTPUT
  old : usint;
END_VAR
  
  old := LDR_canwait;
  LDR_canwait := us_wait;
  
END_FUNCTION // /ws001
//------------------------------------------------------------------------------
// LDR_SetWait4Visu
//  Setzt das Flag bWait4Visu. Wenn dieses Flag gesetzt ist, dann wird nach der 
//  Loader-Initialisierung nicht sofort der zyklische Betrieb aufgenommen.
//  Dadurch bleibt mehr CPU Zeit für die GetId-Phase der Visu übrig und die 
//  GetId-Phase wird schneller fertig. 
//  Der zyklische Betrieb wird dann gestartet, wenn das Flag bWait4Visu (siehe 
//  Befehl I_VISU_RDY) wieder zurückgesetzt wird. Mit dem Parameter timeout_ms 
//  kann ein Timeout-Wert festgelegt werden, nach dem der zyklische Betrieb 
//  aufgenommen wird, auch wenn das Flag bWait4Visu nicht zurückgesetzt wird. 
//  Der Timeout-Wert 0 bedeutet dass der Timeout nicht wirksam ist.
//------------------------------------------------------------------------------
FUNCTION GLOBAL __CDECL LDR_SetWait4Visu  //ZP000
VAR_INPUT
  bWait : BOOL;
  timeout_ms : UDINT;
END_VAR
  
  bWait4Visu := bWait;
  wait4Visu_timeout_ms := timeout_ms;
  
END_FUNCTION  //ZP000
//------------------------------------------------------------------------------
// LDR_SetCanWaitRemote
//
//  Legt den Wert der Canbremse serverseitig fest (nur für diesen einen Kanal). 
//  -1 bedeutet Defaultwert wiederherstellen.
//------------------------------------------------------------------------------
FUNCTION GLOBAL __CDECL LDR_SetCanWaitRemote
VAR_INPUT
  pComdef : ^COMDEF;
  wait : INT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
  txinfo4008    : COMOBJSTR;
END_VAR

    txinfo4008.iLng  := 0;
    txinfo4008.iOff  := 4008;
    txinfo4008.dData := 0;
    txinfo4008.dData$INT := wait;

    IF CanTxObj(pComdef^.pt_COM, #txinfo4008$void) <> 0 THEN
        retVal := -1;
    ELSE
        retVal := 0;    
    END_IF;

END_FUNCTION


(*===============================================================================

  ComlinkRS232 - Comlink über RS232
  
===============================================================================*)

(*
  Konzept:
  --------
  
  Auf unterer Ebene (L0) werden L0-Pakete (L0-Requests und L0-Responses) hin- und 
  hergeschickt. Der Aufbau ist so gewählt, daß sich die L0-Pakete mit dem Lasal32 
  Protokoll vertragen, d.h. wenn das Lasal32 Protokoll Comlink Daten empfängt 
  (oder umgekehrt), dann wird das Paktetformat verstanden, das darin enthaltene 
  Kommandobyte wird aber als nicht unterstützter Befehl erkannt und es wird ein 
  NAK zurückgeschickt.
  
  In einem L0-Paket sind die Comlink Requests und Responses eingepackt (z.B. 
  TxCommand Req.+Resp., UpdCell Req. usw.).
  
  Ein L0-Paket ist mit einer Checksumme versehen, um Übertragungsfehler zu 
  erkennen.
  
  Wenn ein L0-Request geschickt wird, dann muss auf die L0-Response gewartet werden, 
  bis wieder ein L0-Request geschickt werden darf. Wenn ein L0-Request empfangen 
  wird, dann muss dieser mit einer L0-Response beantwortet werden, sonst schickt 
  die Gegenstation nichts mehr. Mit diesem Mechanismus erreicht man eine Fluss-
  kontrolle, d.h. man kann den Empfangsbuffer der Gegenstation nicht überfluten. 
  Der Empfangsbuffer der Gegenstation muss so gross gewählt werden, daß er mindestens 
  den größtmöglichen L0-Request und die größtmöglichen L0-Response aufnehmen kann. 
  Die Werte wurden so gewählt, daß ein L0-Paket (Req. oder Resp.) max. 1000 Bytes 
  gross sein kann, daher muss der Empfangsbuffer 2000 Bytes gross sein.

  Die Comlink Requests und Responses sind L1 Pakete. Es gibt L0-Sendebuffer, 
  in welche die zu sendenden L1 Pakete beim Sendeaufruf hineingestellt werden.
  In einen L0-Sendebuffer können nur dann L1 Pakete hineinkopiert werden, wenn 
  er nicht gerade vom Sendeteil in Bearbeitung ist.
  Es werden 2 L0-Sendebuffer verwendet, damit auch Daten entgegengenommen werden 
  können, wenn einer in Bearbeitung ist (solange bis dieser freie L0-Sendebuffer 
  voll ist).
  
  Thread-Sicherheit:
  - Funktionsaufrufe für verschiedene Kanäle können aus verschiedenenen Tasks 
    aufgerufen werden.
  - Bei Funktionsaufrufen für den gleichen Kanal können nur Befehle aus unterschiedlichen 
    Befehlsgruppen aus verschiedenen Tasks aufgerufen werden. 
    Es gibt 2 Befehlsgruppen: Command- und Refreshlistenbefehle. 
    Commandbefehl: TxCommand
    Refreshbefehl: TxUpd, StartStopRefresh
*)

#define NBR_OF_RS232_ITFS 10   // Anzahl der unterstützten RS232 Schnittstellen

#define DFLT_BAUD         9
#define DFLT_PARITY       0
#define DFLT_STOP         1
#define DFLT_WORDL        8

TYPE

#pragma pack (push,1)

#define L0_STX   16#02
#define L0_ETX   16#03

#define RS232_L0_TIMEOUT_BAUD115KB          500  // bezogen auf 115200 baud (in dieser Zeit können ca. 5 1k Pakete übertragen werden)
#define RS232_L0_TIMEOUT_GSMDELAY          1000  // Verzögerung wenn ein GSM Modem verwendet wird

#define RS232_L0_RESP_TIMEVAL               100  // nach dieser Zeit wird spätestens eine L0 Response geschickt

#define RS232_L0FLAGS_IS_REQUEST          16#01
#define RS232_L0CMD                       16#A0

#define L0_EMPTY_DATALEN    5   // Das ist die DataLen im L0 Header, wenn 0 Bytes Schicht1 Daten vorhanden sind (nur HdrChk ist vorhanden)

  // Schicht 0 Datenformat
  // !STX  !                       !0xA0 !Hdr  !<---  Schicht1 Daten  --->!  Data     !ETX  !
  // !0x02 !        DataLen        !Flags!Chk  !            ...           !  Chksum   !0x03 !
  // +-----+-----+-----+-----+-----+-----+-----+-----+-----         +-----+-----+-----+-----+
  //        \-------- Hdr Chksum -------/ \--------  DataLen-4  ---------/
  //                                \----------- Data Chksum ------------/
  //  \------------------------------- DataLen+5 ------------------------------------------/
  SER_L0_BUF : STRUCT
    stx : BYTE;
    dataLen : UDINT;
    flagsCmd : BYTE;
    hdrChksum : BYTE;
    data : ARRAY[0..992] OF BYTE; // Platz für 990 Datenbytes, 2 Bytes Chksum, 1 Byte ETX
  END_STRUCT;

#define INVALID_CHIDX   254
#define UNKNOWN_CHIDX   255

#define RS232_L1CMD_MASK    16#3F

#define RS232_L1FLAGS_IS_REQUEST          16#80
#define RS232_L1FLAGS_RESPONSE_REQUIRED   16#40 // nur bei einem Request relevant
#define RS232_L1FLAGS_ACK                 16#40 // nur bei einer Response relevent (0=unbekannter Befehl)
  // Schicht 1 Datenformat
  // !Src  !Dst  !Flags!           !           !                             !
  // !ChIdx!ChIdx!Cmd  !  DataLen  !  Seq.Nbr  !  Data       ...             !
  // +-----+-----+-----+-----+-----+-----+-----+-----+-----+     +-----+-----+
  //                                \--------------- DataLen ---------------/
  SER_L1_TXBUF : STRUCT
    dataValid : BYTE; // wird nicht gesendet !
    srcChIdx : BYTE;
    dstChIdx : BYTE;
    flagsCmd : BYTE;
    dataLen : UINT;
    data : ARRAY[0..299] OF BYTE;
  END_STRUCT;
  
  SER_L1_BUF : STRUCT
    srcChIdx : BYTE;
    dstChIdx : BYTE;
    flagsCmd : BYTE;
    dataLen : UINT;
    data : ARRAY[0..299] OF BYTE;
  END_STRUCT;
#pragma pack (pop)
  
  RS232_ITF : STRUCT
    hComm : pVoid;              // Handle für die RS232 Schnittstelle
    hCommAppl : pVoid;          // das gleiche Handle - wird während der Initialisierungsphase von der Applikation verwendet (z.B.Modem Wählvorgang)
    baudrate : UINT;            // zu verwendende Baudrate
    timeoutVal : UDINT;         // Timeout in ms
    startServer : DINT;         // Flag, ob der Comlink Server die Schnittstelle benutzen soll
    fnInitInterface : pVoid;    // Callbackfunktion in der z.B. einen Modemverbindung aufgebaut wird
    pThis : pVoid;              // This-Pointer für die Callbackfunktion

    itfIdx : DINT;              // Index im rs232Itfs Array

    l0SendBufA : SER_L0_BUF;
    l0SendBufB : SER_L0_BUF;
    
    pL0SendBufferCollect : ^SER_L0_BUF;
    pL0SendBufferTransfer : ^SER_L0_BUF;
    bytesToSend : UDINT;        // Anzahl der zu sendenden Bytes im pL0SendBufferTransfer
    bytesSent : UDINT;          // Anzahl der gesendeten Bytes im pL0SendBufferTransfer
    sendInProgress : BOOL;      // TRUE, wenn das Senden läuft
    
    l0RespPending : BOOL;       // es wird eine L0 Response erwartet
    l0RespNecessary : BOOL;     // es muss eine L0 Response geschickt werden
    l0RespPendingTimeout_ms : UDINT;    // Wenn nach dieser Zeit keine Resp. eintrifft, dann wird das Flag zurückgesetzt
    l0RespNecessaryTimeval_ms : UDINT;  // Wenn nach dieser Zeit noch keine Resp. geschickt wurde, dann wird eine Response auch ohne Daten geschickt
    
    bytesInRecvBuf : UDINT;
    l0RecvBuf : ARRAY[0..999] OF BYTE;  // L0-Empfangsbuffer
  END_STRUCT;
  
#pragma pack (push,1)
  loginContextR : STRUCT
    // ---- muß immer da vorne sein (wegen CAN comData Structur) ----
    pCallback                     : ^void;
    pThis                         : UDINT;
    hComchMutex                   : MT_SEMAHANDLE; //
    typeOfStruct				          : UINT;		   // STRUCT_TYPE_...
    sizeofDynList                 : UINT;          //
    sizeofStatList                : UINT;          //
    sizeofRemoteDynList           : UINT;          //
    sizeofRemoteStatList          : UINT;          //
    remoteMaxCyclicTime_ms        : UDINT;         //
    pNode                         : ^MonitorNode;  //	
    pDynCallbacks       		  : ^sDynCallbackList; // Liste der Callbacks (ua. für Multimaster)
    // ---------------------------------------------------------
    versionOfPeer: UDINT;           // version number of the peer
    instIdOfPeer : UDINT;           // instance id of the peer
    interface    : UDINT;           // interface nummer (kein Index, nicht 0-basierend !)
    pItf         : ^RS232_ITF;

    txL1BufCmd   : SER_L1_TXBUF;    // der Sendebuffer für TxCommand, bzw. die Response darauf
    txL1BufRefr  : SER_L1_TXBUF;    // der Sendebuffer für die Verwaltung der Updatelisten
    rxL1BufResp  : SER_L1_BUF;
    rxL1BufReq   : SER_L1_BUF;
    statList     : updateList;      // static list
    dynList      : updateList;      // dynamic list
    pComdefT     : ^comdefT;        // a pointer to a structure that was given by the caller of login
    hMutex       : MT_SEMAHANDLE;   // Mutex for ???
    myChIdx      : BYTE;            // Channel Index lokal
    hisChIdx     : BYTE;            // Channel Index remote
    isConnected  : BOOL;            // nach einem Login:connected, nach einem Fehler:disconnected
    isClient		 : BOOL;
    seqNbr       : UINT;
    maxIdleTime_ms  : UDINT;		   
    lastSendTime_ms : UDINT;

    Com          : DebugIp;
    befiprResp   : msgBefiprRespData;  // the response message from command CMD_BEFIPR

  END_STRUCT;
#pragma pack (pop)
  
END_TYPE

#define MAX_LOGINS_R             16

VAR_PRIVATE
  rs232Itfs : ARRAY[0..NBR_OF_RS232_ITFS-1] OF RS232_ITF;
  loginArrayR    : ARRAY[0..(MAX_LOGINS_R - 1)] of ^loginContextR;
  loginAnzR      : UDINT;
  comlinkMutexR : MT_SEMAHANDLE;
  comlinkEventR : MT_SEMAHANDLE;
  taskHandleCyclic : UDINT;
  isComlinkRInitialized : BOOL;
  g_lockCnt : UDINT;
  g_inCS : UDINT;
END_VAR

#define LOCK_R()    OS_MT_Wait(comlinkMutexR)
#define UNLOCK_R()  OS_MT_Signal(comlinkMutexR)

FUNCTION ScanL1Block
VAR_INPUT
  pL0Buffer : ^SER_L0_BUF;
  nextOfs : ^UDINT;
  nRead : ^UDINT;
END_VAR
VAR_OUTPUT
  pL1Buffer : ^SER_L1_BUF;
END_VAR;

(*
        SERUSERBAUD_300 (0)
        SERUSERBAUD_600 (1)
        SERUSERBAUD_1200 (2)
        SERUSERBAUD_2400 (3)
        SERUSERBAUD_4800 (4)
        SERUSERBAUD_9600 (5)
        SERUSERBAUD_14400 (6)
        SERUSERBAUD_19200 (7)
        SERUSERBAUD_38400 (8)
        SERUSERBAUD_56000 (9)
        SERUSERBAUD_115200 (10)
*)
FUNCTION GetL0TimeoutValR
VAR_INPUT
  baudrate : UINT;
END_VAR
VAR_OUTPUT
  timeoutVal : UDINT;
END_VAR
VAR
  fact : UDINT;
END_VAR

  CASE baudrate OF
    0:  fact := 384;
    1:  fact := 192;
    2:  fact :=  96;
    3:  fact :=  48;
    4:  fact :=  24;
    5:  fact :=  12;
    6:  fact :=   8;
    7:  fact :=   6;
    8:  fact :=   3;
    9:  fact :=   2;
  ELSE  fact :=   1;
  END_CASE;
  
  timeoutVal := RS232_L0_TIMEOUT_BAUD115KB * fact + RS232_L0_TIMEOUT_GSMDELAY;

END_FUNCTION

(*
  Errechnet die Byte-Checksumme eines Datenblocks
*)
FUNCTION SerMakeChksum8
VAR_INPUT
  chksumIn : BYTE;
  pData : ^BYTE;
  len : UDINT;
END_VAR
VAR_OUTPUT
  chksumOut : BYTE;
END_VAR
//VAR
//  i : UDINT;
//END_VAR

  chksumOut := chksumIn;
  while(len) do
    chksumOut += pData^;
    pData += 1;
    len -= 1;
  end_while;
  
//  chksumOut := chksumIn;
//  FOR i := 0 TO len-1 DO
//    chksumOut += pData^;
//    pData += 1;
//  END_FOR;

END_FUNCTION

(*
  Errechnet die Word-Checksumme eines Datenblocks
*)
FUNCTION SerMakeChksum16
VAR_INPUT
  chksumIn : UINT;
  pData : ^BYTE;
  len : UDINT;
END_VAR
VAR_OUTPUT
  chksumOut : UINT;
END_VAR
//VAR
//  i : UDINT;
//END_VAR

  chksumOut := chksumIn;
  while(len) do
    chksumOut += pData^;
    pData += 1;
    len -= 1;
  end_while;

//  chksumOut := chksumIn;
//  FOR i := 0 TO len-1 DO  
//    chksumOut += pData^;
//    pData += 1;
//  END_FOR;

END_FUNCTION

(*
  Gibt die Größe eines L0 Pakets in Bytes zurück
*)
FUNCTION GetL0PktLen
VAR_INPUT
  pL0Block : ^SER_L0_BUF;
END_VAR
VAR_OUTPUT
  len : UDINT;
END_VAR

  len := pL0Block^.dataLen + L0_EMPTY_DATALEN;

END_FUNCTION

(*
  Gibt die Größe eines L1 Pakets in Bytes zurück
*)
FUNCTION GetL1PktLen
VAR_INPUT
  pL1Block : ^SER_L1_BUF;
END_VAR
VAR_OUTPUT
  len : UINT;
END_VAR

  len := pL1Block^.dataLen + 5;

END_FUNCTION

(*
  Errechnet die Checksumme eines L0 Blocks
*) 
FUNCTION CalcL0DataChksum
VAR_INPUT
  pL0Block : ^SER_L0_BUF;
END_VAR
VAR_OUTPUT
  chksum : UINT;
END_VAR

  chksum := SerMakeChksum16(16#FFFF, #pL0Block^.flagsCmd, GetL0PktLen(pL0Block) - 8);

END_FUNCTION

(*
  Errechnet die Checksumme eines L0 Headers
*) 
FUNCTION CalcL0HdrChksum
VAR_INPUT
  pL0Block : ^SER_L0_BUF;
END_VAR
VAR_OUTPUT
  chksum : BYTE;
END_VAR

  chksum := SerMakeChksum8(16#FF, #pL0Block^.dataLen$BYTE, 5);

END_FUNCTION

(*
  Liefert das Cmd-Byte eines L1 Buffers
*) 
FUNCTION GetL1CmdR
VAR_INPUT
  pL1Buffer : ^SER_L1_BUF;
END_VAR
VAR_OUTPUT
  cmd : BYTE;
END_VAR

  cmd := pL1Buffer^.flagsCmd AND RS232_L1CMD_MASK;

END_FUNCTION

(*
  Liefert die SeqNbr eines L1 Buffers
*) 
FUNCTION GetL1SeqNbrR
VAR_INPUT
  pL1Buffer : ^SER_L1_BUF;
END_VAR
VAR_OUTPUT
  seqNbr : UINT;
END_VAR

  seqNbr := pL1Buffer^.data$UINT;

END_FUNCTION

(*
  Stellt einen L1Block in den L0-Sendebuffer.
  Wenn kein Platz mehr im L0-Sendebuffer ist, dann wird -1 zurückgegeben, 
  ansonsten 0.
  Die Checksumme und das ETX Bytes wird hier nicht aktualisiert, das muss am 
  Ende gemacht werden, wenn alle L1 Blöcke in den L0 Block gelegt worden sind.
*)
FUNCTION PackL1Block
VAR_INPUT
  pLogin    : ^loginContextR;
  pL1Buffer : ^SER_L1_TXBUF;
END_VAR
VAR_OUTPUT
  retVal : comlinkReason;
END_VAR
VAR
  pL0Buffer : ^SER_L0_BUF;
  l0PktLen : UDINT;
  l1PktLen : UINT;
  pL1BufferTest : ^SER_L1_BUF;
  pReqMsg1 : ^msgUpdCellReqData;
  pReqMsg2 : ^msgUpdCellReqData;
  nextOfs : UDINT;
  nRead : UDINT;
END_VAR

  IF pL1Buffer^.dataValid = 0 THEN
    retVal := COMLINK_ERR_GENERAL;
    RETURN;
  END_IF;

  pL0Buffer := pLogin^.pItf^.pL0SendBufferCollect;

  IF pL1Buffer^.flagsCmd = (CMD_UPDCELL OR RS232_L1FLAGS_IS_REQUEST) THEN
    // Wenn es sich um einen CMD_UPDCELL Request handelt, dann wird der 
    // Sendebuffer daraufhin untersucht, ob bereits ein CMD_UPDCELL mit 
    // der gleichen LasalID vorhanden ist. Wenn ja, dann werden nur die Daten 
    // im Request aktualisiert. Damit wird verhindert, dass der Sendebuffer 
    // mit CMD_UPDCELL's von sich rasch ändernden LasalID's überflutet wird.
    nextOfs := SER_L0_BUF.data;
    WHILE nextOfs < (pL0Buffer^.dataLen + 2) DO
      pL1BufferTest := ScanL1Block(pL0Buffer, #nextOfs, #nRead);
      IF pL1BufferTest^.flagsCmd = pL1Buffer^.flagsCmd THEN
        pReqMsg1 := #pL1Buffer^.data[0]$msgUpdCellReqData;
        pReqMsg2 := #pL1BufferTest^.data[0]$msgUpdCellReqData;
        IF (pReqMsg1^.iOff = pReqMsg2^.iOff) & (pReqMsg1^.pCh = pReqMsg2^.pCh) THEN
          // Der Befehl ist gefunden worden -> dData aktualisieren und return
          DEBUGLOG0("%010d-    PackL1Block: data updated in send buffer");
          pReqMsg2^.dData := pReqMsg1^.dData;
          retVal := 0;
          RETURN;
        END_IF;
      END_IF;
    END_WHILE;        
  END_IF;

  l0PktLen := GetL0PktLen(pL0Buffer);
  l1PktLen := GetL1PktLen(#pL1Buffer^.srcChIdx$SER_L1_BUF);
  IF (l0PktLen + l1PktLen) > SIZEOF(SER_L0_BUF) THEN
    // kein Platz mehr vorhanden
    DEBUGLOG0("%010d- ***PackL1Block: no more space in send buffer");
    retVal := COMLINK_ERR_GENERAL;
    RETURN;
  END_IF;

  _memcpy(pL0Buffer + l0PktLen - 3, #pL1Buffer^.srcChIdx, l1PktLen);
  pL0Buffer^.dataLen += l1PktLen$UINT;
  
  // Die Message wurde aus dem Source Buffer in den Transferbuffer kopiert, 
  // daher wird dataValid im Source Buffer auf 0 gesetzt.
  pL1Buffer^.dataValid := 0;
  
  pLogin^.lastSendTime_ms := ops.tAbsolute;

  retVal := 0;

END_FUNCTION

(*
  Errechnet die Checksummen eines L0 Blocks und schreibt die Checksummen  
  und das ETX Byte in den L0 Block
*)
FUNCTION FinishL0Block
VAR_INPUT
  pL0Buffer : ^SER_L0_BUF;
END_VAR

  pL0Buffer^.hdrChksum := CalcL0HdrChksum(pL0Buffer);
  (pL0Buffer + GetL0PktLen(pL0Buffer) - 3)$^UINT^ := CalcL0DataChksum(pL0Buffer);
  (pL0Buffer + GetL0PktLen(pL0Buffer) - 1)$^BYTE^ := L0_ETX;

END_FUNCTION


FUNCTION ResetL0
VAR_INPUT
  pItf : ^RS232_ITF;
END_VAR

  _memset(#pItf^.l0SendBufA, 0, SIZEOF(pItf^.l0SendBufA));
  pItf^.l0SendBufA.stx := L0_STX;
  pItf^.l0SendBufA.dataLen := L0_EMPTY_DATALEN;
  
  _memset(#pItf^.l0SendBufB, 0, SIZEOF(pItf^.l0SendBufB));
  pItf^.l0SendBufB.stx := L0_STX;
  pItf^.l0SendBufB.dataLen := L0_EMPTY_DATALEN;

  pItf^.pL0SendBufferCollect  := #pItf^.l0SendBufA;
  pItf^.pL0SendBufferTransfer := #pItf^.l0SendBufA;
  pItf^.bytesToSend := 0;
  pItf^.bytesSent := 0;
  pItf^.sendInProgress := FALSE;
    
  pItf^.l0RespPending := FALSE;
  pItf^.l0RespNecessary := FALSE;
  pItf^.l0RespPendingTimeout_ms := pItf^.timeoutVal * 2;
  pItf^.l0RespNecessaryTimeval_ms := RS232_L0_RESP_TIMEVAL;
    
  pItf^.bytesInRecvBuf := 0;

END_FUNCTION

FUNCTION ResetL1
VAR_INPUT
  pLogin : ^loginContextR;
END_VAR

  pLogin^.isConnected := FALSE;
  pLogin^.hisChIdx := UNKNOWN_CHIDX;
  pLogin^.txL1BufCmd.dataValid := 0;
  pLogin^.txL1BufRefr.dataValid := 0;

END_FUNCTION

//------------------------------------------------------------------------------
// DisconnectAfterErrorR
//  Beendet die Verbindung nachdem ein Fehler festgestellt wurde.
//------------------------------------------------------------------------------
FUNCTION DisconnectAfterErrorR
VAR_INPUT
  pLogin : ^loginContextR;
END_VAR

  DEBUGLOG0("%010d-*** DisconnectAfterErrorR");

  ResetL1(pLogin);

END_FUNCTION


(*
  Beendet alle Verbindungen eines Interfaces
*)
FUNCTION DisconnectAllForItf
VAR_INPUT
  pItf : ^RS232_ITF;
END_VAR
VAR
  i : DINT;
  pLogin : ^loginContextR;
END_VAR

  FOR i := 0 TO (arraysize(loginArrayR) - 1) DO
    pLogin := loginArrayR[i];
    IF pLogin <> NIL & pLogin^.pItf = pItf THEN
      DisconnectAfterErrorR(pLogin);
    END_IF;
  END_FOR;
  
END_FUNCTION

(*
  Prüft, ob das Senden möglich ist, wenn ja dann wird der L0SendBufferTransfer 
  fürs Senden vorbereitet (Flags, Checksumme) und dann wird pItf^.sendInProgress 
  auf TRUE gesetzt.
  
  Es gibt 4 Zustände auf L0 Ebene:
    Resp.Pending  Resp.Necessary  Nächster Send  Kann gesendet werden ?
    ------------  --------------  -------------  ----------------------
    0             0               L0-Request     ja
    1             0               -              nein
    0             1               L0-Response    ja
    1             1               L0-Response    ja
*)
FUNCTION L0CheckSend
VAR_INPUT
  pItf : ^RS232_ITF;
END_VAR

(*
  // Timeouts abfragen und Flags bei abgelaufenen Timeouts berichtigen.
  IF pItf^.l0RespPending & ((ops.tAbsolute - pItf^.l0RespPendingTimeout_ms)$DINT > 0) THEN
    // Es darf nicht ewig auf eine Response gewartet werden.
    DEBUGLOG0("%010d-*** L0CheckSend: timeout for l0RespPending exceeded, resetting connections");
    DisconnectAllForItf(pItf);
    ResetL0(pItf);
    RETURN;
  END_IF;
  *)

  // L0-Sendebuffer ermitteln
  IF pItf^.l0RespPending & !!pItf^.l0RespNecessary THEN
    //DEBUGLOG0("%010d-*** L0CheckSend: send not possible");
    RETURN;
  END_IF;
  
  IF pItf^.l0RespNecessary THEN
    pItf^.pL0SendBufferTransfer^.flagsCmd  := RS232_L0CMD;
  ELSE
    pItf^.pL0SendBufferTransfer^.flagsCmd  := RS232_L0CMD OR RS232_L0FLAGS_IS_REQUEST;
  END_IF;
  
  FinishL0Block(pItf^.pL0SendBufferTransfer);

  DEBUGLOG2("%010d-    L0CheckSend: send started, flags: respPending=%d, respNecessary=%d", pItf^.l0RespPending, pItf^.l0RespNecessary);

  pItf^.sendInProgress := TRUE;

END_FUNCTION

(*
  Sendet die Daten im L0SendBufferTransfer.
  
  Bedingungen:
  Es müssen Daten vorhanden sein und das sendInProgress Flag muss auf TRUE stehen.
*)
FUNCTION L0Send
VAR_INPUT
  pItf : ^RS232_ITF;
END_VAR
VAR
  nWritten : UDINT;
END_VAR

  SERUSER_Send(pItf^.hComm, 
               pItf^.pL0SendBufferTransfer + pItf^.bytesSent, 
               pItf^.bytesToSend, 
               #nWritten);
  pItf^.bytesSent   += nWritten;               
  pItf^.bytesToSend -= nWritten;                

  DEBUGLOG1("%010d-    L0Send: %d bytes sent", nWritten);

END_FUNCTION

(*
  Liest einen L0-Buffer von der Schnittstelle
*)
FUNCTION L0Recv
VAR_INPUT
  pItf : ^RS232_ITF;
END_VAR
VAR_OUTPUT
  pL0Buffer : ^SER_L0_BUF;
END_VAR
VAR
  bytes2read : UDINT;
  bytesRead : UDINT;
  l0PktLen : UDINT;
  pL0Temp : ^SER_L0_BUF;
  rc : DINT;
END_VAR

  pL0Buffer := NIL;
  pL0Temp := #pItf^.l0RecvBuf$SER_L0_BUF;

  WHILE pItf^.bytesInRecvBuf < SER_L0_BUF.data DO
    bytes2read := (SER_L0_BUF.data)$UDINT - pItf^.bytesInRecvBuf;
    rc := SERUSER_RecvBlock(pItf^.hComm, 
                         #pItf^.l0RecvBuf[pItf^.bytesInRecvBuf], 
                         bytes2read, 
                         #bytesRead);
    IF rc <> 0 THEN
      RETURN;
    END_IF;
    pItf^.bytesInRecvBuf += bytesRead;
    DEBUG_VERBOSE2("%010d-*** L0Recv: %d bytes read, new bytes in recvBuf: %d", bytesRead, pItf^.bytesInRecvBuf);
    IF pItf^.bytesInRecvBuf = SER_L0_BUF.data THEN
      IF (pItf^.l0RecvBuf[0] <> L0_STX) | (CalcL0HdrChksum(pL0Temp) <> pL0Temp^.hdrChksum) THEN 
        _memmove(#pItf^.l0RecvBuf[0], #pItf^.l0RecvBuf[1], 6);
        pItf^.bytesInRecvBuf -= bytesRead;
      END_IF;
    END_IF;
  END_WHILE;

  l0PktLen := GetL0PktLen(pL0Temp);
  WHILE pItf^.bytesInRecvBuf < l0PktLen DO
    bytes2read := l0PktLen - pItf^.bytesInRecvBuf;
    rc := SERUSER_RecvBlock(pItf^.hComm, 
                            #pItf^.l0RecvBuf[pItf^.bytesInRecvBuf], 
                            bytes2read, 
                            #bytesRead);
    IF rc <> 0 THEN
      RETURN;
    END_IF;
    pItf^.bytesInRecvBuf += bytesRead;
    DEBUG_VERBOSE2("%010d-*** L0Recv: %d bytes read, new bytes in recvBuf: %d", bytesRead, pItf^.bytesInRecvBuf);
    IF pItf^.bytesInRecvBuf = l0PktLen THEN
      IF (pItf^.l0RecvBuf[l0PktLen-1] = L0_ETX) &
         (CalcL0DataChksum(pL0Temp) = pItf^.l0RecvBuf[l0PktLen-3]$UINT) THEN
        pL0Buffer := pL0Temp;
        IF pL0Temp^.flagsCmd AND RS232_L0FLAGS_IS_REQUEST THEN
          pItf^.l0RespNecessary := TRUE;
        ELSE
          pItf^.l0RespPending := FALSE;
        END_IF;
      END_IF;
      pItf^.bytesInRecvBuf := 0;
      DEBUGLOG2("%010d-    L0Recv: L0 block complete, new flags: respPending=%d, respNecessary=%d", pItf^.l0RespPending, pItf^.l0RespNecessary);
      RETURN;
    END_IF;
  END_WHILE;
  
END_FUNCTION

(*
  Entnimmt den nächsten L1 Block aus einem L0-Buffer
*)
FUNCTION ScanL1Block
VAR_INPUT
  pL0Buffer : ^SER_L0_BUF;
  nextOfs : ^UDINT;
  nRead : ^UDINT;
END_VAR
VAR_OUTPUT
  pL1Buffer : ^SER_L1_BUF;
END_VAR

  IF nextOfs^ < (pL0Buffer^.dataLen + 3) THEN
    pL1Buffer := pL0Buffer + nextOfs^;
    nRead^ := pL1Buffer^.dataLen + 5;
    nextOfs^ += nRead^;
  ELSE
    nRead^ := 0;
    pL1Buffer := NIL;
  END_IF;

END_FUNCTION


(*
  Liefert einen Zeiger auf den jeweils anderen der beiden Sendebuffer.
*)
FUNCTION SwitchL0Buffer
VAR_INPUT
  pItf : ^RS232_ITF;
  pL0SendBufferIn : ^SER_L0_BUF;
END_VAR
VAR_OUTPUT
  pL0SendBufferOut : ^SER_L0_BUF;
END_VAR

  IF pL0SendBufferIn = #pItf^.l0SendBufA THEN
    pL0SendBufferOut := #pItf^.l0SendBufB;
  ELSE
    pL0SendBufferOut := #pItf^.l0SendBufA;
  END_IF;
      
END_FUNCTION

//------------------------------------------------------------------------------
// AllocateLoginContextR
//------------------------------------------------------------------------------
FUNCTION AllocateLoginContextR
VAR_INPUT
    interface : UDINT;         // COM Interface number
END_VAR
VAR_OUTPUT
    pLogin       : ^loginContextR;
END_VAR
VAR
    pStatCells  : ^comStCell;
    pDynCells   : ^comStCell;
    mutexName   : ARRAY[0..30] OF CHAR;
END_VAR

  pStatCells := NIL;
  pDynCells  := NIL;

  pLogin := calloc(SIZEOF(loginContextR))$^loginContextR;
  IF pLogin = NIL THEN
    RETURN;
  END_IF;
  
  // initialize the new connection context
  pStatCells := MemoryUpdateList(#pLogin^.statList, INIT_STATLIST_ENTRIES, 0, 0);
  pDynCells  := MemoryUpdateList(#pLogin^.dynList, INIT_DYNLIST_ENTRIES, 1, 0);
  _strcpy(#mutexName[0], "LOGIN_SEMA_R");
  MakeResourceName(pLogin$UDINT, #mutexName[0]);
  pLogin^.hMutex := OS_MT_CreateSemaphore(MTSEMATYPE_COUNTING, 
                                          1,    // initvalue
                                          0,    // flags: 0 means create unconitionally
                                          #mutexName[0]
                                          );
  IF (pStatCells <> NIL) & (pDynCells <> NIL) & (pLogin^.hMutex <> NIL) THEN
    pLogin^.interface  := interface;
    pLogin^.pCallback := g_pCallback;
    pLogin^.hComchMutex := NewComchMutex(pLogin$UDINT, 'r');
    pLogin^.typeOfStruct := STRUCT_TYPE_LOGINCONTEXTR;

    pLogin^.sizeofStatList := MAX_STATLIST_ENTRIES;
    pLogin^.sizeofDynList  := MAX_DYNLIST_ENTRIES;

     // Die Refreshlisten-Größen der Remote Station werden vorerst mit der Größe der eigenen 
     // initialisiert. Wenn beim Login ein anderer Wert gemeldet wird, dann wird diese 
     // Variable noch einmal überschrieben
    pLogin^.sizeofRemoteDynList  := pLogin^.sizeofDynList;
    pLogin^.sizeofRemoteStatList := pLogin^.sizeofStatList;

    pLogin^.hisChIdx := INVALID_CHIDX;

    DEBUGLOG0("%010d-  =====  Login context allocated  =====");
  ELSE
    // cleanup
    IF pLogin^.hMutex <> NIL THEN
      OS_MT_DeleteSemaphore(pLogin^.hMutex);
    END_IF;
    IF pStatCells <> NIL THEN
      _free(pStatCells);
      pStatCells := NIL;
      g_nbrStatList -= 1;
    END_IF;
    IF pDynCells <> NIL THEN
      _free(pDynCells);
      pDynCells := NIL;
      g_nbrDynList -= 1;
    END_IF;
    _free(pLogin);
    pLogin := NIL;
  END_IF;

  
END_FUNCTION

//------------------------------------------------------------------------------
// ReleaseLoginContextR
//
//  Release all resources of a used login context.
//------------------------------------------------------------------------------
FUNCTION ReleaseLoginContextR
VAR_INPUT
    pLogin       : ^loginContextR;
END_VAR

  IF pLogin = NIL THEN
    RETURN;
  END_IF;

  IF pLogin^.hMutex <> NIL THEN
    OS_MT_DeleteSemaphore(pLogin^.hMutex);
  END_IF;
  DeleteComchMutex(#pLogin^.hComchMutex);

  IF pLogin^.pNode <> NIL THEN
    RemoveMonitorNode(pLogin^.pNode);
  END_IF;

  IF pLogin^.pComdefT THEN
    pLogin^.pComdefT^.pLogin := NIL;
  END_IF;

  // free allocated memory
  _free(pLogin^.statList.pFirstCell);
  g_nbrStatList -= 1;
  _free(pLogin^.dynList.pFirstCell);
  g_nbrDynList -= 1;
  pLogin^.statList.pFirstCell := NIL;
  pLogin^.dynList.pFirstCell := NIL;
  
  _free(pLogin);
  pLogin := NIL;
  
  DEBUGLOG0("%010d-  =====  Login context released  =====");
  
END_FUNCTION

(*
  Wandelt eine Interface Nummer (in ComTypes.h definiert, z.B.COMLINK_IFNUM_COM1) 
  in einen Interface Index um. Dieser Index kann für das Array rs232Itfs verwendet 
  werden.
  Bei einem Fehler wird -1 zurückgegeben.
*)
FUNCTION ComlinkR_Itfnbr2Idx
VAR_INPUT
  interface : UDINT;
END_VAR
VAR_OUTPUT
  idx : DINT;
END_VAR

  CASE interface OF
    COMLINK_IFNUM_COM1,
    COMLINK_IFNUM_COM2,
    COMLINK_IFNUM_COM3,
    COMLINK_IFNUM_COM4:
      idx$UDINT := interface - COMLINK_IFNUM_COM1;
    COMLINK_IFNUM_COM5,
    COMLINK_IFNUM_COM6,
    COMLINK_IFNUM_COM7,
    COMLINK_IFNUM_COM8,
    COMLINK_IFNUM_COM9,
    COMLINK_IFNUM_COM10:
      idx$UDINT := interface - COMLINK_IFNUM_COM5 + 4;
    ELSE
      idx := -1;
  END_CASE;
  
  IF idx >= arraysize(rs232Itfs) THEN
    idx := -1;
  END_IF;

END_FUNCTION

(*
  Wandelt einen Interface Index in eine Interface Nummer (in ComTypes.h definiert, 
  z.B.COMLINK_IFNUM_COM1) um.
*)
FUNCTION ComlinkR_Idx2Itfnbr
VAR_INPUT
  idx : DINT;
END_VAR
VAR_OUTPUT
  interface : UDINT;
END_VAR

  IF idx < 4 THEN
    interface := idx$UDINT + COMLINK_IFNUM_COM1;
  ELSE
    interface := idx$UDINT + COMLINK_IFNUM_COM5;
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkR_GetHandle
//  Gibt das zu einer Interface Nummmer gehörende Handle zurück.
//  Wenn es nocht nicht initialisiert worden ist, dann wird das hier gemacht.
//------------------------------------------------------------------------------
FUNCTION ComlinkR_GetHandle
VAR_INPUT
  interface : UDINT;
END_VAR
VAR_OUTPUT
  hComm : pVoid;
END_VAR
VAR
  idx : DINT;
  pItf : ^RS232_ITF;
  initStatus : DINT;
END_VAR

  idx := ComlinkR_Itfnbr2Idx(interface);

  IF idx >= 0 THEN
    pItf := #rs232Itfs[idx];
    hComm := pItf^.hComm;
    IF hComm = NIL THEN
      IF pItf^.hCommAppl = NIL THEN
        pItf^.hCommAppl := SERUSER_Init(idx$UINT + 1, pItf^.baudrate, DFLT_PARITY, DFLT_STOP, DFLT_WORDL);
      END_IF;
      IF pItf^.hCommAppl <> NIL THEN
        g_RS232activity := TRUE;
      
        IF pItf^.fnInitInterface <> NIL THEN
          initStatus := pItf^.fnInitInterface$LDR_InitRs232Interface(pItf^.pThis, interface, pItf^.hCommAppl);
        ELSE
          initStatus := 1;
        END_IF;
        IF initStatus > 0 THEN
          hComm := pItf^.hCommAppl;
          pItf^.hComm := hComm;
        END_IF;
      END_IF;
    END_IF;
  ELSE
    hComm := NIL;
  END_IF;
  
END_FUNCTION


//------------------------------------------------------------------------------
// ComlinkR_Init
//------------------------------------------------------------------------------
FUNCTION ComlinkR_Init
VAR
  i : DINT;
END_VAR

  IF mt_api = NIL | _LSL_POS^.piSSR^.SSR_CurrentTaskHandle = NIL THEN
    isComlinkRInitialized := FALSE;
    RETURN;
  END_IF;

  _memset(#rs232Itfs[0], 0, SIZEOF(rs232Itfs));
  FOR i := 0 TO arraysize(rs232Itfs)-1 DO
    rs232Itfs[i].itfIdx := i;
    rs232Itfs[i].baudrate := DFLT_BAUD;
    rs232Itfs[i].timeoutVal := GetL0TimeoutValR(rs232Itfs[i].baudrate);
    ResetL0(#rs232Itfs[i]);
    DEBUGLOG2("%010d-    ComlinkR_Init: itf %d - timeoutVal=%d", i$UDINT, rs232Itfs[i].timeoutVal);
  END_FOR;
  
  _memset(#loginArrayR[0], 0, SIZEOF(loginArrayR));
  loginAnzR         := 0;

  comlinkMutexR := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                         1,    // initvalue
                                         0,    // flags: 0 means create unconitionally
                                         "COMLINKR_MUTEX0"
                                         );
  comlinkEventR := OS_MT_CreateSemaphore(MTSEMATYPE_EVENT, 
                                         0,    // initvalue
                                         0,    // flags: 0 means create unconitionally
                                         "COMLINKR_EVENT0"
                                         );

  taskHandleCyclic := OS_SSR_CurrentTaskHandle();

  isComlinkRInitialized := TRUE;
    
END_FUNCTION

(*
  Blockiert so lange, bis eine RS232 Übertragung stattgefunden hat.
  
  Wenn der Aufruf ausserhalb des Cyclic erfolgt, dann blockiert diese Funktion 
  auf einem Event Semaphore, das vom Cyclic in der ComlinkR_SerTransfer Funktion 
  gepulst wird.
  
  Wenn der Aufruf im Context des Cyclic Tasks erfolgt, dann muss die ComlinkR_SerTransfer 
  Funktion selbst aufgerufen werden, weil ja der Loader-Cyclic nicht aufgerufen 
  wird, solange man sich in der RS232WaitForTransfer Funktion befindet.
*)
FUNCTION RS232WaitForTransfer
VAR_INPUT
  pItf : ^RS232_ITF;
END_VAR
VAR
  startTime : UDINT;
END_VAR

  IF OS_SSR_CurrentTaskHandle() = taskHandleCyclic THEN
    startTime := ops.tAbsolute;
    WHILE (ops.tAbsolute - startTime) < 20 DO
      IF ComlinkR_SerTransfer(pItf) THEN
        RETURN;
      END_IF;
      OS_MT_Delay(1);
    END_WHILE;
  ELSE
    OS_MT_WaitTimed(comlinkEventR, 20);
  END_IF;
  
END_FUNCTION

//------------------------------------------------------------------------------
// SendRequestR
//  Sendet einen Request
//------------------------------------------------------------------------------
FUNCTION SendRequestR
VAR_INPUT
  pLogin : ^loginContextR;
  cmd : BYTE;
  respRequired : BOOL;
  pData : pVoid;
  dataLen : UDINT;
  blockingMode : BOOL;
END_VAR
VAR_OUTPUT
  retval        : comlinkReason;
END_VAR
VAR
  startTime : UDINT;
  pTxL1Buf : ^SER_L1_TXBUF;
  seqNbrSize : UINT;
END_VAR

  retval := 0;

  IF pLogin^.isConnected = FALSE & pLogin^.hisChIdx <> UNKNOWN_CHIDX THEN
    DEBUGLOG0("%010d-*** SendRequestR: COMLINK_ERR_NOCONNECTION");
    retval := COMLINK_ERR_NOCONNECTION;
    RETURN;
  END_IF;

  IF respRequired THEN
    seqNbrSize := SIZEOF(UINT);
  ELSE      
    seqNbrSize := 0;
  END_IF;
  IF dataLen > SIZEOF(pTxL1Buf^.data) - seqNbrSize THEN
    DEBUGLOG0("%010d-*** SendRequestR: COMLINK_ERR_BUF_TOO_SMALL");
    retval := COMLINK_ERR_BUF_TOO_SMALL;
    RETURN;
  END_IF;

  IF respRequired THEN
    pTxL1Buf := #pLogin^.txL1BufCmd;
  ELSE
    pTxL1Buf := #pLogin^.txL1BufRefr;
  END_IF;

  pTxL1Buf^.srcChIdx := pLogin^.myChIdx;
  pTxL1Buf^.dstChIdx := pLogin^.hisChIdx;
  pTxL1Buf^.flagsCmd := cmd OR RS232_L1FLAGS_IS_REQUEST;
  
  IF respRequired THEN
    pLogin^.seqNbr += 1;
    pTxL1Buf^.flagsCmd := pTxL1Buf^.flagsCmd OR RS232_L1FLAGS_RESPONSE_REQUIRED;
    pTxL1Buf^.dataLen  := SIZEOF(UINT);
    pTxL1Buf^.data$UINT := pLogin^.seqNbr;
    // Wenn srcChIdx im L1 RxBuffer auf INVALID_CHIDX gesetzt wird, dann heisst das, 
    // dass der Buffer leer ist und vom Übertragungsteil beschrieben werden darf.
    pLogin^.rxL1BufResp.srcChIdx := INVALID_CHIDX;
  ELSE      
    pTxL1Buf^.dataLen := 0;
  END_IF;
  
  _memcpy(#pTxL1Buf^.data[0] + pTxL1Buf^.dataLen, pData, dataLen);
  pTxL1Buf^.dataLen += dataLen$UINT;
  
  pTxL1Buf^.dataValid := 1;

  startTime := OPS.tAbsolute;
  WHILE 1 DO
    LOCK_R();
    retval := PackL1Block(pLogin, pTxL1Buf);
    UNLOCK_R();
    IF retval = 0 THEN
      IF respRequired THEN
        DEBUGLOG3("%010d- 1> ch%d SendRequestR: put L1 message into collect buffer: dataLen(incl.seqNbr)=%d, seqNbr=%d", pLogin^.myChIdx, pTxL1Buf^.dataLen, TO_UDINT(pTxL1Buf^.data$UINT));
      ELSE
        DEBUGLOG2("%010d- 1> ch%d SendRequestR: put L1 message into collect buffer: dataLen=%d", pLogin^.myChIdx, pTxL1Buf^.dataLen);
      END_IF;
      EXIT;
    END_IF;
    
    IF !!blockingMode THEN
      EXIT;
    END_IF;
    
    IF (OPS.tAbsolute - startTime) > pLogin^.pItf^.timeoutVal THEN
      DEBUGLOG0("%010d-*** SendRequestR: COMLINK_ERR_TIMEOUT");
      retval := COMLINK_ERR_TIMEOUT;
      EXIT;
    END_IF;
    
    RS232WaitForTransfer(pLogin^.pItf);
    
  END_WHILE;

  pTxL1Buf^.dataValid := 0;

END_FUNCTION

//------------------------------------------------------------------------------
// WaitForResponseR
//  Wartet auf die Response
//------------------------------------------------------------------------------
FUNCTION WaitForResponseR
VAR_INPUT
  pLogin : ^loginContextR;
  pRespData : pVoid;
  respDataLen : UINT;
END_VAR
VAR_OUTPUT
  retval        : INT;
END_VAR
VAR
  startTime : UDINT;
END_VAR

  IF pLogin^.isConnected = FALSE & pLogin^.hisChIdx <> UNKNOWN_CHIDX THEN
    DEBUGLOG0("%010d-*** WaitForResponseR: COMLINK_ERR_NOCONNECTION");
    retval := COMLINK_ERR_NOCONNECTION$INT;
    RETURN;
  END_IF;

  startTime := OPS.tAbsolute;
  WHILE 1 DO
    IF (pLogin^.rxL1BufResp.srcChIdx <> INVALID_CHIDX) THEN
      IF ((pLogin^.rxL1BufResp.flagsCmd$USINT AND RS232_L1CMD_MASK) = (pLogin^.txL1BufCmd.flagsCmd$USINT AND RS232_L1CMD_MASK)) THEN
        IF (pLogin^.rxL1BufResp.dataLen >= SIZEOF(UINT)) THEN
          IF (pLogin^.rxL1BufResp.data$UINT = pLogin^.seqNbr) THEN
       
            DEBUGLOG3("%010d-<1  ch%d WaitForResponseR: received expected L1 message: dataLen(incl.seqNbr)=%d, seqNbr=%d", pLogin^.myChIdx, pLogin^.rxL1BufResp.dataLen, TO_UDINT(pLogin^.seqNbr));
            IF pLogin^.rxL1BufResp.flagsCmd AND RS232_L1FLAGS_ACK THEN
              retval := 0;
              IF respDataLen > pLogin^.rxL1BufResp.dataLen - SIZEOF(UINT) THEN
                respDataLen := pLogin^.rxL1BufResp.dataLen - SIZEOF(UINT);
              END_IF;
              IF pRespData <> NIL THEN
                _memcpy(pRespData, #pLogin^.rxL1BufResp.data[2], respDataLen);
              END_IF;
              retval := respDataLen$INT;
            ELSE
              DEBUGLOG0("%010d-*** WaitForResponseR: COMLINK_ERR_INVALID_RESPONSE");
              retval := COMLINK_ERR_INVALID_RESPONSE$INT;
            END_IF;
            EXIT;
            
          ELSE
            DEBUGLOG2("%010d-*** WaitForResponseR: received unexpected L1 message, rcvdSeqNbr=%d, expctdSeqNbr=%d", TO_UDINT(pLogin^.rxL1BufResp.data$UINT), TO_UDINT(pLogin^.seqNbr));
          END_IF;
        ELSE
          DEBUGLOG1("%010d-*** WaitForResponseR: received unexpected L1 message, invalid dataLen=%d", TO_UDINT(pLogin^.rxL1BufResp.dataLen));
        END_IF;
      ELSE
        DEBUGLOG2("%010d-*** WaitForResponseR: received unexpected L1 message, rcvdCmd=%d, expctdCmd=%d", TO_UDINT(pLogin^.rxL1BufResp.flagsCmd$USINT AND RS232_L1CMD_MASK), TO_UDINT(pLogin^.txL1BufCmd.flagsCmd$USINT AND RS232_L1CMD_MASK));
      END_IF;
    END_IF;
    
    IF (OPS.tAbsolute - startTime) > pLogin^.pItf^.timeoutVal THEN
      DEBUGLOG1("%010d-*** ch%d WaitForResponseR: COMLINK_ERR_TIMEOUT", pLogin^.myChIdx);
      retval := COMLINK_ERR_TIMEOUT$INT;
      EXIT;
    END_IF;
    
    RS232WaitForTransfer(pLogin^.pItf);
    
  END_WHILE;

END_FUNCTION

//------------------------------------------------------------------------------
// SendResponseR
//  Sendet eine Response
//------------------------------------------------------------------------------
FUNCTION SendResponseR
VAR_INPUT
  pLogin : ^loginContextR;
  seqNbr : UINT;
  ack : BOOL;
  cmd : BYTE;
  pData : pVoid;
  dataLen : UDINT;
END_VAR
VAR_OUTPUT
  retval        : comlinkReason;
END_VAR
VAR
  pTxL1Buf : ^SER_L1_TXBUF;
END_VAR

  retval := 0;

  IF sizeof(UINT) + dataLen > SIZEOF(pTxL1Buf^.data) THEN
    DEBUGLOG0("%010d-*** SendResponseR: COMLINK_ERR_BUF_TOO_SMALL");
    retval := COMLINK_ERR_BUF_TOO_SMALL;
    RETURN;
  END_IF;

  pTxL1Buf := #pLogin^.txL1BufCmd;
  
  IF pTxL1Buf^.dataValid THEN
    DEBUGLOG0("%010d-*** SendResponseR: txBuf is not free");
    retval := COMLINK_ERR_BUF_TOO_SMALL;
    RETURN;
  END_IF;

  pTxL1Buf^.srcChIdx := pLogin^.myChIdx;
  pTxL1Buf^.dstChIdx := pLogin^.hisChIdx;
  pTxL1Buf^.flagsCmd := cmd AND RS232_L1CMD_MASK;
  IF ack THEN
    pTxL1Buf^.flagsCmd := pTxL1Buf^.flagsCmd OR RS232_L1FLAGS_ACK;
  END_IF;
  
  pTxL1Buf^.dataLen  := sizeof(UINT) + dataLen$UINT;
  pTxL1Buf^.data$UINT := seqNbr;
  _memcpy(#pTxL1Buf^.data[2], pData, dataLen);
  
  pTxL1Buf^.dataValid := 1;
  
  LOCK_R();
  retval := PackL1Block(pLogin, #pTxL1Buf^);
  UNLOCK_R();

  IF retval = 0 THEN
    DEBUGLOG2("%010d- 1> SendResponseR: put L1 message into collect buffer: dataLen(incl.seqNbr)=%d, seqNbr=%d", pTxL1Buf^.dataLen, TO_UDINT(pTxL1Buf^.data$UINT));
  ELSE
    DEBUGLOG2("%010d- 1> *** SendResponseR: PackL1Block failed: dataLen(incl.seqNbr)=%d, seqNbr=%d", pTxL1Buf^.dataLen, TO_UDINT(pTxL1Buf^.data$UINT));
  END_IF;    

END_FUNCTION

//------------------------------------------------------------------------------
// ConnectR
//  Schickt den Login Befehl
//------------------------------------------------------------------------------
FUNCTION ConnectR
VAR_INPUT
  pLogin : ^loginContextR;
END_VAR
VAR_OUTPUT
  retval        : DINT;
END_VAR

  ResetL1(pLogin);

  // Request senden
  retval := SendRequestR(pLogin, CMD_CONNECT, respRequired:=TRUE, pData:=NIL, dataLen:=0, blockingMode:=TRUE);
  IF retval <> 0 THEN
    RETURN;
  END_IF;

  // warten auf die Antwort
  retval := WaitForResponseR(pLogin, NIL, 0);
  IF retval >= 0 THEN
    pLogin^.hisChIdx := pLogin^.rxL1BufResp.srcChIdx;
    pLogin^.isConnected := TRUE;
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// ExchgSessionInfoR
//  Exchange session info with the server
//------------------------------------------------------------------------------
FUNCTION ExchgSessionInfoR
VAR_INPUT
  pLogin : ^loginContextR;
  mySessId      : UDINT;
  versionOfPeer : ^UDINT;
  instIdOfPeer  : ^UDINT;
  sizeofRemoteDynList  : ^UINT;
  sizeofRemoteStatList : ^UINT;
END_VAR
VAR_OUTPUT
  retval        : DINT;
END_VAR
VAR
  msgData          : msgSessInfoData;
END_VAR

  msgData.version := COMLINKR_VERSION;
  msgData.instId  := myInstanceId;
  msgData.sessId  := mySessId;
  msgData.sizeofDynList  := MAX_DYNLIST_ENTRIES;
  msgData.sizeofStatList := MAX_STATLIST_ENTRIES;

  // Request senden
  retval := SendRequestR(pLogin, 
                         CMD_SESSINFO, 
                         respRequired:=TRUE, 
                         #msgData, 
                         SIZEOF(msgData), 
                         blockingMode:=TRUE);
  IF retval <> 0 THEN
    RETURN;
  END_IF;

  // warten auf die Antwort
  retval := WaitForResponseR(pLogin, #msgData, SIZEOF(msgData));
  IF retval >= 0 THEN
    versionOfPeer^:= msgData.version;
    instIdOfPeer^ := msgData.instId;
    sizeofRemoteDynList^  := msgData.sizeofDynList;
    sizeofRemoteStatList^ := msgData.sizeofStatList;
  END_IF;

END_FUNCTION  

//------------------------------------------------------------------------------
// IsConnectedR
//  Prüft, ob eine Verbindung besteht. Eine Verbindung ist aufrecht vom Login 
//  bis zu einem Fehler. Nach einem Fehler muss wieder ein Login gemacht werden.
//  Beim Login wird der Verbindungsstatus resettiert (z.B. alle Zellen in der 
//  Refreshliste auf 0 gesetzt)
//------------------------------------------------------------------------------
FUNCTION IsConnectedR
VAR_INPUT
  pLogin : ^loginContextR;
END_VAR
VAR_OUTPUT
  retval : BOOL;
END_VAR

  retval := pLogin^.isConnected;

END_FUNCTION

//------------------------------------------------------------------------------
// SearchUsedConnBlockCltR
//  Sucht einen vom Client bereits benutzten Connection Block
//------------------------------------------------------------------------------
FUNCTION SearchUsedConnBlockCltR
VAR_INPUT
  pComdef : ^comdef; 
END_VAR
VAR_OUTPUT
  pLogin : ^loginContextR; // connection context
END_VAR
VAR
  i : UDINT;
END_VAR

  pLogin := NIL;
  FOR i := 0 TO (arraysize(loginArrayR) - 1) DO
    IF (loginArrayR[i] <> NIL) &
       (loginArrayR[i]^.interface = pComdef^.interface) &
       (loginArrayR[i]^.pComdefT = pComdef$^comdefT) THEN
      pLogin := loginArrayR[i];
      IF pLogin^.pItf^.hComm = 0 THEN
        // ComlinkR_GetHandle erneut aufrufen, damit die Callbackfunktion aufgerufen 
        // werden kann.
        pLogin^.pItf^.hComm := ComlinkR_GetHandle(ComlinkR_Idx2Itfnbr(pLogin^.pItf^.itfIdx));
      END_IF;
      IF pLogin^.pItf^.hComm = 0 THEN
        pLogin := NIL;
      END_IF;
      EXIT;
    END_IF;
  END_FOR;

END_FUNCTION

//------------------------------------------------------------------------------
// SearchUsedConnBlockSvrR
//  Sucht einen vom Server bereits benutzten Connection Block
//------------------------------------------------------------------------------
FUNCTION SearchUsedConnBlockSvrR
VAR_INPUT
  itfIdx : DINT;
  remoteChIdx : BYTE; 
END_VAR
VAR_OUTPUT
  pLogin : ^loginContextR; // connection context
END_VAR
VAR
  i : UDINT;
END_VAR

  pLogin := NIL;
  FOR i := 0 TO (arraysize(loginArrayR) - 1) DO
    IF (loginArrayR[i] <> NIL) &
       (ComlinkR_Itfnbr2Idx(loginArrayR[i]^.interface) = itfIdx) &
       (loginArrayR[i]^.hisChIdx = remoteChIdx) THEN
      pLogin := loginArrayR[i];
      EXIT;
    END_IF;
  END_FOR;

END_FUNCTION

//------------------------------------------------------------------------------
// AllocNewConnBlockCltR
//  Alloziert für den Client einen neuen Connection Block
//------------------------------------------------------------------------------
FUNCTION AllocNewConnBlockCltR
VAR_INPUT
  pComdefT : ^comdefT; 
  pRetval : ^UINT;
END_VAR
VAR_OUTPUT
  pLogin : ^loginContextR; // connection context
END_VAR
VAR
  i : UDINT;
  hComm : pVoid;
END_VAR

  pLogin := NIL;
  IF loginAnzR = arraysize(loginArrayR) THEN
    DEBUGLOG0("%010d-*** AllocNewConnBlockCltR: COMLINK_ERR_MAXCONN");
    pRetval^ := COMLINK_ERR_MAXCONN;
    RETURN;
  END_IF;

  hComm := ComlinkR_GetHandle(pComdefT^.interface);
  IF hComm = 0 THEN
    DEBUGLOG0("%010d-*** AllocNewConnBlockCltR: ComlinkR_GetHandle failed");
    RETURN;
  END_IF;

  // try to get a connection context
  pLogin := AllocateLoginContextR(pComdefT^.interface);
  IF pLogin = NIL THEN
    DEBUGLOG0("%010d-*** AllocNewConnBlockCltR: AllocateLoginContextR failed");
    RETURN;
  END_IF;
  
  pLogin^.pItf := #rs232Itfs[ComlinkR_Itfnbr2Idx(pComdefT^.interface)];
  pLogin^.pItf^.hComm := hComm;
  
  // success: try to put pLogin into loginArrayR
  LOCK_R();
  
  // search a free entry in loginArrayR
  FOR i := 0 TO (arraysize(loginArrayR) - 1) DO
    IF loginArrayR[i] = NIL THEN 
      EXIT;
    END_IF;
  END_FOR;
  IF i = arraysize(loginArrayR) THEN
    pRetval^ := COMLINK_ERR_MAXCONN;
    UNLOCK_R();
    ReleaseLoginContextR(pLogin);
    RETURN;
  END_IF;

  pComdefT^.pLogin := pLogin$^loginContext;
  // make an entry in the loginArrayR
  loginArrayR[i] := pLogin;
  pLogin^.myChIdx := i;
  loginAnzR += 1;
  
  UNLOCK_R();

END_FUNCTION

//------------------------------------------------------------------------------
// AllocNewConnBlockSvrR
//  Alloziert für den Server einen neuen Connection Block
//------------------------------------------------------------------------------
FUNCTION AllocNewConnBlockSvrR
VAR_INPUT
  itfIdx : DINT;
  remoteChIdx : BYTE; 
END_VAR
VAR_OUTPUT
  pLogin : ^loginContextR; // connection context
END_VAR
VAR
  i : UDINT;
  interface : UDINT;
END_VAR

  interface := ComlinkR_Idx2Itfnbr(itfIdx);

  pLogin := NIL;
  IF loginAnzR = arraysize(loginArrayR) THEN
    DEBUGLOG0("%010d-*** AllocNewConnBlockSvrR: COMLINK_ERR_MAXCONN");
    RETURN;
  END_IF;

  // try to get a connection context
  pLogin := AllocateLoginContextR(interface);
  IF pLogin = NIL THEN
    DEBUGLOG0("%010d-*** AllocNewConnBlockSvrR: AllocateLoginContextR failed");
    RETURN;
  END_IF;
  pLogin^.pItf := #rs232Itfs[itfIdx];

  pLogin^.pItf^.hComm := ComlinkR_GetHandle(interface);
  IF pLogin^.pItf^.hComm = 0 THEN
    DEBUGLOG0("%010d-*** AllocNewConnBlockSvrR: ComlinkR_GetHandle failed");
    ReleaseLoginContextR(pLogin);
    RETURN;
  END_IF;
  
  // success: try to put pLogin into loginArrayR
  LOCK_R();
  
  // search a free entry in loginArrayR
  FOR i := 0 TO (arraysize(loginArrayR) - 1) DO
    IF loginArrayR[i] = NIL THEN 
      EXIT;
    END_IF;
  END_FOR;
  IF i = arraysize(loginArrayR) THEN
    UNLOCK_R();
    ReleaseLoginContextR(pLogin);
    RETURN;
  END_IF;

  // make an entry in the loginArrayR
  loginArrayR[i] := pLogin;
  pLogin^.myChIdx := i;
  loginAnzR += 1;
  
  UNLOCK_R();

END_FUNCTION


FUNCTION ComlinkR_Logout
VAR_INPUT
  pComdef : ^comdef;
END_VAR
VAR_OUTPUT
  retval : UINT;
END_VAR
VAR
  pComDefT : ^comdefT;
END_VAR

  pComDefT := pComdef$^comdefT;
  ReleaseLoginContextR( pComDefT^.pLogin$^loginContextR );
  Seruser_Close( ComlinkR_GetHandle( pComDefT^.Interface ) );
  retval := 0;

END_FUNCTION


//------------------------------------------------------------------------------
// ComlinkR_Login
//  Login über RS232
//------------------------------------------------------------------------------
FUNCTION ComlinkR_Login
VAR_INPUT
    pComdef       : ^comdef; 
END_VAR
VAR_OUTPUT
    retval        : UINT;
END_VAR
VAR
    pComdefT      : ^comdefT; 
    pLogin        : ^loginContextR; // connection context
    versionOfPeer : UDINT;
    instIdOfPeer  : UDINT;
    rc            : DINT;        
    sizeofRemoteDynList  : UINT;
    sizeofRemoteStatList : UINT;
END_VAR

  DEBUGLOG0("%010d-ComlinkR_Login");
  
  retval   := COMLINK_ERR_GENERAL;
  
  IF !!isComlinkRInitialized THEN
    RETURN;
  END_IF;
  
  pComdefT := pComdef$^comdefT;
  
  pLogin := SearchUsedConnBlockCltR(pComdef);
  IF pLogin = NIL THEN
    pLogin := AllocNewConnBlockCltR(pComdefT, #retval);
  END_IF;  
  
  IF pLogin = NIL THEN
    RETURN;
  END_IF;
  
  pLogin^.isClient := TRUE;

  //ResetL0(pLogin^.pItf);

  rc := ConnectR(pLogin);
  IF rc <> 0 THEN
    DEBUGLOG1("%010d-*** ComlinkR_Login: ConnectR failed, rc=%d", TO_UDINT(rc));
    retval := rc$UINT;
    RETURN;
  END_IF;

  // exchange session info with the server
  sizeofRemoteDynList  := pLogin^.sizeofRemoteDynList;
  sizeofRemoteStatList := pLogin^.sizeofRemoteStatList;
  rc := ExchgSessionInfoR(pLogin, 
                          pComdef$UDINT, 
                          #versionOfPeer, 
                          #instIdOfPeer,
                          #sizeofRemoteDynList,
                          #sizeofRemoteStatList);
  IF rc < 0 THEN
    DEBUGLOG1("%010d-*** ComlinkR_Login: ExchgSessionInfo failed, rc=%d", TO_UDINT(rc));
    retval := COMLINK_ERR_NOCONNECTION;
    RETURN;
  END_IF;

  retval := 0;
  pLogin^.versionOfPeer := versionOfPeer;
  pLogin^.instIdOfPeer  := instIdOfPeer;
  pLogin^.sizeofRemoteDynList  := sizeofRemoteDynList;
  pLogin^.sizeofRemoteStatList := sizeofRemoteStatList;
  pLogin^.pComdefT      := pComdefT; // this pointer is needed when the callback function is called

END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkR_TxCommandEx
//
//  This function send interpreter code to the comlink server. The server 
//  executes the code and responds with the the result and the workstate.
//------------------------------------------------------------------------------
FUNCTION ComlinkR_TxCommandEx
VAR_INPUT
    opCode    : UDINT;
    length    : UDINT;
    aPara     : ^USINT;
    pComdef   : ^comdef; 
    pResu     : ^UDINT;
    pStatus   : ^IPRSTATES;
    res       : UDINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR
VAR
    pComdefT  : ^comdefT; 
    pLogin    : ^loginContextR; // connection context
    reqMsg    : msgBefiprReqData; // request structure for command CMD_BEFIPR
    reqMsgLen : UDINT;
    fOk       : UDINT;
    rc        : DINT;
END_VAR

  pComdefT := pComdef$^comdefT;
  pStatus^ := ERROR;
  retval   := COMLINK_ERR_GENERAL;
  fOk   := 0;

  IF !!isComlinkRInitialized THEN
    DEBUGLOG0("%010d-*** ComlinkT_TxCommandEx: !!isComlinkRInitialized");
    RETURN;
  END_IF;

  pLogin := pComdefT^.pLogin$^loginContextR;
  IF pLogin = NIL THEN
    DEBUGLOG0("%010d-*** ComlinkT_TxCommandEx: COMLINK_ERR_LOGIN_REQUIRED");
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;

  pResu^ := (#pLogin^.befiprResp.resu)$UDINT;
  
  IF length > SIZEOF(reqMsg.instr.aPara) THEN
    // parameter length is too big
    DEBUGLOG0("%010d-*** ComlinkT_TxCommandEx: parameter length is too big");
    retval := COMLINK_ERR_INVALID_PARAM;
    RETURN;
  END_IF;

  // prepare the request message structure
  reqMsg.workState    := BUSY;
  reqMsg.instr.opCode := opCode$INSTRSET;
  reqMsg.instr.uiLng  := (length + SIZEOF(instruct.uiLng))$UINT;
  _memcpy(#reqMsg.instr.aPara, aPara, length);

  reqMsgLen := SIZEOF(reqMsg.workState)    +
               SIZEOF(reqMsg.instr.opCode) +
               SIZEOF(reqMsg.instr.uiLng)  +
               length;

  // Send the request
  rc := SendRequestR(pLogin, 
                     CMD_BEFIPR, 
                     respRequired:=TRUE, 
                     #reqMsg, 
                     reqMsgLen, 
                     blockingMode:=TRUE);
  IF rc <> 0 THEN
    DEBUGLOG1("%010d-*** ComlinkR_TxCommandEx: SendRequestR failed, rc=%d", TO_UDINT(rc));
    retval := COMLINK_ERR_NOCONNECTION;
    GOTO EndLabel;
  END_IF;
  DEBUGLOG2("%010d-ComlinkR_TxCommandEx: BEFIPR request sent, opCode=%d, msgLen=%d", opCode, TO_UDINT(rc));

  // Read the response
  rc := WaitForResponseR(pLogin, #pLogin^.befiprResp, SIZEOF(pLogin^.befiprResp));
  IF rc < 0 THEN
    DEBUGLOG1("%010d-*** ComlinkR_TxCommandEx: ReadCmdMsg failed, rc=%d", TO_UDINT(rc));
    retval := COMLINK_ERR_NOCONNECTION;
    GOTO EndLabel;
  END_IF;
  AliveCheckMsgRcvd(pLogin$^loginContext);
  
  IF rc < SIZEOF(pLogin^.befiprResp.workstate) THEN
    // not enough data received !
    DEBUGLOG0("%010d-*** ComlinkR_TxCommandEx: not enough data received");
    retval := COMLINK_ERR_INVALID_RESPONSE;
    GOTO EndLabel;
  END_IF;

  fOk := 1; 
               
EndLabel:
  IF fOk = 0 THEN
    // The connection has to be closed because it's possible that partial data 
    // is transferred.
    DEBUGLOG0("%010d-ComlinkR_TxCommandEx: closing connection");
    DisconnectAfterErrorR(pLogin);
    pStatus^ := ERROR;
  ELSE
    pStatus^ := pLogin^.befiprResp.workstate;
    DEBUGLOG2("%010d-ComlinkR_TxCommandEx: BEFIPR response received, dataLen=%d, Status=%d", TO_UDINT(rc-4), pStatus^$UDINT);
    //DEBUG_VERBOSE3("%010d-                    uiLng=%d, aData[0]=0x%08X, aData[4]=0x%08X", TO_UDINT(pLogin^.befiprResp.resu.uiLng), pLogin^.befiprResp.resu.aData[0]$UDINT, pLogin^.befiprResp.resu.aData[4]$UDINT);
    retval := COMLINK_IPR;
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkR_TxUpd_nocs
//------------------------------------------------------------------------------
FUNCTION ComlinkR_TxUpd_nocs
VAR_INPUT
    pLslcommregdata : ^lslCommregData;
    pLogin          : ^loginContextR; // connection context
    uiflags         : UINT;
END_VAR
VAR_OUTPUT
    retval          : comlinkReason;
END_VAR
VAR
    pt_comStCell  : ^comStCell;
    updList       : ^updateList;
    listIndex     : UDINT;
    listType      : UDINT;
    reqMsg        : msgTxUpdReqData;   // request structure for command CMD_TXUPD
    timx           : udint;
END_VAR

  retval := COMLINK_ERR_GENERAL;
  
  IF pLslcommregdata^.varpos$INT < 1000 THEN
    updList   := #pLogin^.statList;
    listIndex := pLslcommregdata^.varpos;
    listType  := 0;
  ELSE
    updList   := #pLogin^.dynList;
    listIndex := pLslcommregdata^.varpos - 1000;
    listType  := 1;
  END_IF;
  
  IF listIndex >= updList^.nbrCells THEN
    IF IncreaseUpdateList(updList, listIndex + 1, listType) = NIL THEN
      DEBUGLOG0("%010d-*** ComlinkR_TxUpd: MemoryUpdateList failed");
      DisconnectAfterErrorR(pLogin);
      RETURN;
    END_IF;
    DEBUGLOG2("%010d-*** ComlinkR_TxUpd: list %d increased to %d entries", listType, updList^.nbrCells);
  END_IF;    

  pt_comStCell := updList^.pFirstCell + (listIndex * SIZEOF (comStCell));
  
  timx                       := pLslcommregdata^.uiTime; // u32 := u16 
  pt_comStCell^.dData        := 16#80000001$DINT;     // INVALID    
  pt_comStCell^.pCh          := pLslcommregdata^.LASALID$^SVRCH;
  pt_comStCell^.udUpdateRate := (timx and 16#3FFF) or ((timx and 16#C000) shl 16) or (uiflags shl 14);
  pt_comStCell^.VarlistID    := pLslcommregdata^.VarlistID;
  pt_comStCell^.udLastUpdate := 0;

  reqMsg.iOff    := pLslcommregdata^.varpos$UINT;
  reqMsg.comcell := pt_comStCell^;  // copy struct on struct;
  // Send the request
  retval := SendRequestR(pLogin, 
                         CMD_TXUPD, 
                         respRequired:=FALSE, 
                         #reqMsg, 
                         SIZEOF(reqMsg), 
                         blockingMode:=TRUE);
  IF retval = 0 THEN
    DEBUGLOG1("%010d-    ComlinkR_TxUpd_nocs: sent cell ofs %d", TO_UDINT(reqMsg.iOff));
  ELSE
    DEBUGLOG1("%010d-*** ComlinkR_TxUpd_nocs: SendRequestR failed, retval=%d", TO_UDINT(retval));
  END_IF;


END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkR_TxUpd
//
// Return value:
//  If the function succeeds, the return value is 0, othzerwise it is a 
//  negative error code.
//------------------------------------------------------------------------------
FUNCTION ComlinkR_TxUpd
VAR_INPUT
    pLslcommregdata : ^lslCommregData;
    pComdef         : ^comdef; 
    res             : UDINT;
    uiflags         : UINT;
END_VAR
VAR_OUTPUT
    retval          : comlinkReason;
END_VAR
VAR
    pLogin          : ^loginContextR; // connection context
END_VAR

  IF !!isComlinkRInitialized THEN
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;
  
  pLogin := pComdef^$comdefT.pLogin$^loginContextR;
  IF pLogin = NIL THEN
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;

  retval := ComlinkR_TxUpd_nocs(pLslcommregdata, pLogin, uiflags);
  
END_FUNCTION

//------------------------------------------------------------------------------
// SendMaxIdleTime_RS232_nocs
//------------------------------------------------------------------------------
FUNCTION SendMaxIdleTime_RS232_nocs
VAR_INPUT
  pComdefT : ^comdefT;
  remoteMaxIdleTime_ms : UDINT;
  pLogin          : ^loginContextR; // connection context
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
    reqMsg        : msgIdlTimReqData;  // request structure for command CMD_MAXIDLETIME
END_VAR

  //retval := COMLINK_ERR_GENERAL;
  
  // prepare the request message structure
  reqMsg.maxIdleTime_ms := remoteMaxIdleTime_ms;

  // Send the request
  retval := SendRequestR(pLogin, 
                         CMD_MAXIDLETIME, 
                         respRequired:=FALSE, 
                         #reqMsg, 
                         SIZEOF(reqMsg), 
                         blockingMode:=TRUE);
  IF retval = 0 THEN
    DEBUGLOG0("%010d-SendMaxIdleTime_RS232_nocs: MAXIDLETIME request sent");
  ELSE
    DEBUGLOG1("%010d-*** SendMaxIdleTime_RS232_nocs: SendCmdMsg failed, rc=%d", TO_UDINT(retval));
  END_IF;
  
  

END_FUNCTION

//------------------------------------------------------------------------------
// SendMaxIdleTime_RS232
//
// Return value:
//  If the function succeeds, the return value is 0, othzerwise it is a 
//  negative error code.
//------------------------------------------------------------------------------
FUNCTION SendMaxIdleTime_RS232
VAR_INPUT
  pComdefT : ^comdefT;
  remoteMaxIdleTime_ms : UDINT;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
    pLogin          : ^loginContextR; // connection context
END_VAR

  IF !!isComlinkRInitialized THEN
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;
  
  pLogin := pComdefT^.pLogin$^loginContextR;
  IF pLogin = NIL THEN
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;

  retval := SendMaxIdleTime_RS232_nocs(pComdefT, remoteMaxIdleTime_ms, pLogin);
  
END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkR_InstallCallback
//------------------------------------------------------------------------------
FUNCTION ComlinkR_InstallCallback
VAR_INPUT
    pCallback : ^void;
END_VAR
VAR
    i         : UDINT;
    pLogin    : ^loginContextR;
END_VAR

  IF !!isComlinkRInitialized THEN
    RETURN;
  END_IF;

  DEBUGLOG1("%010d-ComlinkR_InstallCallback: pCallback=0x%08X", pCallback$UDINT);

  FOR i := 0 TO (arraysize(loginArrayR) - 1) DO
    pLogin := loginArrayR[i];
    IF pLogin <> NIL & pLogin^.isClient THEN
      pLogin^.pCallback := pCallback;
    END_IF;
  END_FOR;

  g_pCallback := pCallback;
  
END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkR_StartStopRefresh_nocs
//------------------------------------------------------------------------------
FUNCTION ComlinkR_StartStopRefresh_nocs
VAR_INPUT
    pLogin   : ^loginContextR; // connection context
    count    : UINT;
    typ      : UINT;
    res      : UDINT;
END_VAR
VAR_OUTPUT
    retval    : comlinkReason;
END_VAR
VAR
    reqMsg        : msgSsRefrReqData;  // request structure for command CMD_SSREFR
END_VAR

//  retval := COMLINK_ERR_GENERAL;

  // prepare the request message structure
  reqMsg.count   := count;
  reqMsg.typ     := typ;

  // Send the request
  retval := SendRequestR(pLogin, 
                         CMD_SSREFR, 
                         respRequired:=FALSE, 
                         #reqMsg, 
                         SIZEOF(reqMsg), 
                         blockingMode:=TRUE);
  IF retval = 0 THEN
    DEBUGLOG3("%010d-ComlinkR_StartStopRefresh: SSREFR request sent, msgLen=%d, typ=%d, count=%d", TO_UDINT(retval), TO_UDINT(typ), TO_UDINT(count));
  ELSE
    DEBUGLOG3("%010d-*** ComlinkR_StartStopRefresh(typ %d, count %d): SendRequestR failed, rc=%d", TO_UDINT(typ), TO_UDINT(count), TO_UDINT(retval));
  END_IF;
  
END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkR_StartStopRefresh
//------------------------------------------------------------------------------
FUNCTION ComlinkR_StartStopRefresh
VAR_INPUT
    pComdef  : ^comdef; 
    count    : UINT;
    typ      : UINT;
    res      : UDINT;
END_VAR
VAR_OUTPUT
    retval   : comlinkReason;
END_VAR
VAR
    pLogin        : ^loginContextR; // connection context
END_VAR

  IF !!isComlinkRInitialized THEN
    retval := COMLINK_ERR_CLIENT_NOTREADY;
    RETURN;
  END_IF;
  
  pLogin := pComdef^$comdefT.pLogin$^loginContextR;
  IF pLogin = NIL THEN
    retval := COMLINK_ERR_LOGIN_REQUIRED;
    RETURN;
  END_IF;

  retval := ComlinkR_StartStopRefresh_nocs(pLogin, count, typ, res);

END_FUNCTION

//------------------------------------------------------------------------------
// UpdateCell
//
// Return value:
//  Error                                                     : <=0
//  When udLastUpdate did not expire                          : 1
//  When udLastUpdate expired, but dData did not change       : 2
//  When udLastUpdate expired, and dData changed (transfer)   : 3
//------------------------------------------------------------------------------
FUNCTION UpdateCellR
VAR_INPUT
    pLogin : ^loginContextR;
    pUpdCell      : ^comStCell;  // pointer to cell in updatelist
    iOff          : UINT;
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    reqMsg        : msgUpdCellReqData; // request structure for command CMD_UPDCELL
    dData         : DINT;
    rc            : UINT;
END_VAR

  retval := 1;
  
  IF (ops.tAbsolute - pUpdCell^.udLastUpdate) < (pUpdCell^.udUpdateRate AND 16#FFFF) THEN
    // udLastUpdate did not expire
    RETURN;
  END_IF;
  
  IF pUpdCell^.pCh = NIL THEN
    RETURN;
  END_IF;
  
  retval := 2;
  
  IF (pUpdCell^.udUpdateRate and UPDRATE_FLAG_IS_VBI) THEN
    dData := (pUpdCell^.pCh$^virtualBaseInit^.getCrc())$DINT;
  ELSIF (pUpdCell^.udUpdateRate AND UPDRATE_FLAG_IS_GLOBALVAR) THEN
//    dData := (pUpdCell^.pCh$^DINT)^;
    dData := UpDateGlobalVar(pUpdCell^.pCh, pUpdCell^.udUpdateRate);
  ELSE
    dData := rd(pUpdCell^.pCh);
  END_IF;
  
  IF dData = pUpdCell^.dData THEN
    // dData did not change
    pUpdCell^.udLastUpdate := ops.tAbsolute;
    RETURN;
  END_IF;
    
  retval := 3;    
    
  // prepare the request message structure
  reqMsg.iOff  := iOff;
  reqMsg.pCh   := pUpdCell^.pCh;
  reqMsg.dData := dData;

  // Send the request
  rc := SendRequestR(pLogin, 
                     CMD_UPDCELL, 
                     respRequired:=FALSE, 
                     pData:=#reqMsg, 
                     dataLen:=SIZEOF(reqMsg), 
                     blockingMode:=FALSE);
  IF rc <> 0 THEN
    DEBUG_VERBOSE2("%010d-*** UpdateCell(VarPos %d): SendCmdMsg failed, rc=%d", TO_UDINT(iOff), TO_UDINT(rc));
    // Dieser Fall tritt z.B. ein, wenn der Sendebuffer voll ist. Die Verbindung wird 
    // nicht beendet, da die Zelle später auch noch übertragen werden kann, wenn wieder 
    // Platz im Sendebuffer ist.
    retval := -rc;
  ELSE
    //DEBUGLOG2("%010d-UpdateCell: UPDCELL request sent, VarPos=%d, dData=0x%08X", TO_UDINT(iOff), TO_UDINT(dData));
    pUpdCell^.dData := dData;
    pUpdCell^.udLastUpdate := ops.tAbsolute;
  END_IF;
  
END_FUNCTION

//------------------------------------------------------------------------------
// UpdateListProcessorR
//------------------------------------------------------------------------------
FUNCTION UpdateListProcessorR
VAR_INPUT
  pLogin : ^loginContextR;
END_VAR
VAR
  list          : ^updateList;
  pUpdCell      : ^comStCell;  // pointer to cell in updatelist
  pStrtCell     : ^comStCell;
  rc : DINT;
END_VAR

  list := #pLogin^.statList;
  IF list^.nbrRefrCells > 0 THEN
    pStrtCell := list^.pActCell;
    WHILE 1 DO
      pUpdCell := list^.pActCell;
  
      rc := UpdateCellR(pLogin, pUpdCell, list^.actInd$UINT);
      IF rc <= 0 THEN
        RETURN;
      END_IF;
  
      IF (list^.actInd + 1) >= list^.nbrRefrCells THEN
        list^.actInd   := 0;
        list^.pActCell := list^.pFirstCell;
      ELSE
        list^.actInd   += 1;
        list^.pActCell += SIZEOF(list^.pActCell^);
      END_IF;
  
      IF pStrtCell = list^.pActCell THEN
        EXIT;
      END_IF;
      
      //Es wird nur 1 Zelle bei jedem Durchlauf gesendet, mehr macht bei 
      //der seriellen Verbindung keinen Sinn.
      IF rc = 3 THEN
        EXIT;
      END_IF;
    END_WHILE;
  END_IF;    
  
  list := #pLogin^.dynList;
  IF list^.nbrRefrCells > 0 THEN
    pStrtCell := list^.pActCell;
    WHILE 1 DO
      pUpdCell := list^.pActCell;
      
      rc := UpdateCellR(pLogin, pUpdCell, (list^.actInd + 1000)$UINT);
      IF rc <= 0 THEN
        RETURN;
      END_IF;
      
      IF (list^.actInd + 1) >= list^.nbrRefrCells THEN
        list^.actInd   := 0;
        list^.pActCell := list^.pFirstCell;
      ELSE
        list^.actInd   += 1;
        list^.pActCell += SIZEOF(list^.pActCell^);
      END_IF;
      
      IF pStrtCell = list^.pActCell THEN
        EXIT;
      END_IF;
      
      //Es wird nur 1 Zelle bei jedem Durchlauf gesendet, mehr macht bei 
      //der seriellen Verbindung keinen Sinn.
      IF rc = 3 THEN
        EXIT;
      END_IF;
    END_WHILE;
  END_IF;  
  
END_FUNCTION

//------------------------------------------------------------------------------
// SendAliveMessage_RS232
//------------------------------------------------------------------------------
FUNCTION SendAliveMessage_RS232
VAR_INPUT
  pLogin : ^loginContextR;
END_VAR
VAR_OUTPUT
    retval        : DINT;
END_VAR
VAR
    rc            : DINT;
END_VAR

  retval := 1;
  
  rc := SendRequestR(pLogin, 
                     CMD_IAMALIVE, 
                     respRequired:=FALSE, 
                     pData:=NIL, 
                     dataLen:=0, 
                     blockingMode:=FALSE);
  IF rc <> 0 THEN
    DEBUGLOG1("%010d-*** SendAliveMessage_RS232: SendCmdMsg failed, rc=%d", TO_UDINT(rc));
    retval := rc;
  ELSE
    DEBUGLOG0("%010d-SendAliveMessage_RS232: IAMALIVE request sent");
  END_IF;
  
  
END_FUNCTION

//------------------------------------------------------------------------------
// SyncRefListsRS232
//  siehe Beschreibung von SyncRefLists
//------------------------------------------------------------------------------
FUNCTION SyncRefListsRS232
VAR_INPUT
  pLogin : ^loginContextR;
END_VAR
VAR
    obj     : pVoid;
    pHead   : ^IprHead; 
    instr   : ^instruct;
END_VAR
    
#ifndef DUMMY_LOADER
    pHead := pLogin^.Com.GetHead();
#endif    
    instr := pHead^.pCode;

    IF instr^.opCode = I_WRITE THEN

        obj := instr^.aPara[0]$^pVoid;

        // statische Refresh-Liste durchsuchen
        InvalidateRefListEntry(pLogin^.statList.pFirstCell, pLogin^.statList.nbrRefrCells$UINT, obj);

        // dynamische Refresh-Liste durchsuchen
        InvalidateRefListEntry(pLogin^.dynList.pFirstCell, pLogin^.dynList.nbrRefrCells$UINT, obj);

    END_IF;

END_FUNCTION

(*
  Stellt die MaxIdleTime ein
*)
FUNCTION SetMaxIdleTimeR
VAR_INPUT
  pLogin : ^loginContextR;
  maxIdleTime_ms : UDINT;
END_VAR

  IF maxIdleTime_ms <> pLogin^.maxIdleTime_ms THEN
    pLogin^.maxIdleTime_ms := maxIdleTime_ms;
    pLogin^.lastSendTime_ms := ops.tAbsolute - pLogin^.maxIdleTime_ms; // damit gleich was geschickt wird
  END_IF;
  
END_FUNCTION  

(*
  ProcessCmdR
    Führt einen empfangenen Befehl aus.
*)
FUNCTION ProcessCmdR
VAR_INPUT
  pLogin : ^loginContextR;
  cmd : UINT;
  pData : pVoid;
  dataLen : UINT;
  pRespData : pVoid;
  pRespDataLen : ^UINT;
END_VAR
VAR_OUTPUT
  cmdProcessed : BOOL;
END_VAR
VAR
  bNext : UDINT;
  pResu : ^results;
  updList   : ^updateList;
  listType  : UDINT;
  pComCell  : ^comStCell;
  uiOffs    : UINT;
  data2LSE  : d2LSE;        
  original_this : UDINT;
END_VAR

  cmdProcessed := FALSE;

  IF pLogin^.isClient THEN
    AliveCheckMsgRcvd(pLogin$^loginContext);
  END_IF;

  CASE cmd OF
  
    CMD_BEFIPR:
      DEBUGLOG1("%010d-~~~ ProcessCmd: BEFIPR, dataLen=%d", dataLen$UDINT);
      // req-data:
      //    workState   : iprStates;
      //    instr       : instruct;
      // resp-data:
      //    workState   : iprStates;
      //    resu        : results;
      cmdProcessed := TRUE;
      
      IF (dataLen < SIZEOF(iprStates)) | ((dataLen - SIZEOF(iprStates)) > SIZEOF(instruct)) THEN
        pRespData^$msgBefiprRespData.workstate := ERROR;
      ELSE
        // execute befipr
#ifndef DUMMY_LOADER
#ifdef LOADER_4_LASAL2          
        _memcpy(pLogin^.com._m.head.pCode, #pData^$msgBefiprReqData.instr, dataLen - SIZEOF(iprStates));
        pLogin^.com._m.head.workstate := pData^$msgBefiprReqData.workstate;
        pLogin^.com._m.head.funkSt    := READY;
        pLogin^.com._m.resu.uilng :=0;
#else
        _memcpy(pLogin^.com.head.pCode, #pData^$msgBefiprReqData.instr, dataLen - SIZEOF(iprStates));
        pLogin^.com.head.workstate := pData^$msgBefiprReqData.workstate;
        pLogin^.com.head.funkSt    := READY;
        pLogin^.com.resu.uilng :=0;
#endif
        pRespData^$msgBefiprRespData.workstate := pLogin^.com.BEFIPR(#bNext);
#endif        
        SyncRefListsRs232(pLogin);
      END_IF;
  
      // prepare the response
      pRespDataLen^ := SIZEOF(iprStates);
      IF pRespData^$msgBefiprRespData.workstate <> ERROR THEN
#ifndef DUMMY_LOADER
        pResu := pLogin^.com.GetResult();
#endif        
        IF pResu^.uilng <> 16#FFFF THEN
          IF pResu^.uilng <= SIZEOF(pRespData^$msgBefiprRespData) THEN
            pRespDataLen^ += pResu^.uilng;
            _memcpy(#pRespData^$msgBefiprRespData.resu, pResu, pResu^.uilng);
          ELSE
#ifndef DUMMY_LOADER
#ifdef LOADER_4_LASAL2          
            pLogin^.com._m.head.workstate := ERROR;
#else
            pLogin^.com.head.workstate := ERROR;
#endif
#endif        
          END_IF;
        END_IF;
      END_IF;

    CMD_SESSINFO:
      DEBUGLOG1("%010d-~~~ ProcessCmd: SESSINFO, dataLen=%d", dataLen$UDINT);
      // req-data:
      //    version     : UDINT;
      // resp-data:
      //    version     : UDINT;

      IF dataLen >= (msgSessInfoData.sizeofDynList - msgSessInfoData.version) THEN

        cmdProcessed := TRUE;

        // prepare the response
        pRespDataLen^ := SIZEOF(msgSessInfoData);
        pRespData^$msgSessInfoData.version := COMLINKR_VERSION;
        pRespData^$msgSessInfoData.instId  := myInstanceID;
        pRespData^$msgSessInfoData.sessId  := 0;
        pRespData^$msgSessInfoData.sizeofDynList  := MAX_DYNLIST_ENTRIES;
        pRespData^$msgSessInfoData.sizeofStatList := MAX_STATLIST_ENTRIES;

#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-~~~ ProcessCmd: dataLen too small");
#endif      
      END_IF;
      
    CMD_TXUPD:
      DEBUGLOG2("%010d-~~~ ProcessCmd: CMD_TXUPD, dataLen=%d ofs=%d", dataLen$UDINT, TO_UDINT(pData^$msgTxUpdReqData.iOff));
      // req-data:
      //    iOff        : UINT;
      //    comcell     : comStCell;
      // resp-data:
      //    this request requires no response
      IF dataLen >= SIZEOF(msgTxUpdReqData) THEN
      
        cmdProcessed := TRUE;
      
        IF pData^$msgTxUpdReqData.iOff < 1000 THEN
          updList   := #pLogin^.statList;
          listType  := 0;
        ELSE
          updList   := #pLogin^.dynList;
          listType  := 1;
          pData^$msgTxUpdReqData.iOff -=1000;
        END_IF;
        
        IF TO_UDINT(pData^$msgTxUpdReqData.iOff) >= updList^.nbrCells THEN
          IF IncreaseUpdateList(updList, TO_UDINT(pData^$msgTxUpdReqData.iOff) + 1, listType) = NIL THEN
            DEBUGLOG0("%010d-*** ProcessCmd: MemoryUpdateList failed");
            DisconnectAfterErrorR(pLogin);
            RETURN;
          END_IF;
          DEBUGLOG2("%010d-*** ProcessCmd: updList %d increased to %d entries", listType, updList^.nbrCells);
        END_IF;
        
        pComCell := updList^.pFirstCell + (SIZEOF(comStCell) * pData^$msgTxUpdReqData.iOff);
        
        pComCell^ := pData^$msgTxUpdReqData.comcell;
        IF (pComCell^.udUpdateRate AND 0xFFFF) < SMALLEST_UPD_INTERVALL THEN
          pComCell^.udUpdateRate := (pComCell^.udUpdateRate AND 0xFFFF0000) OR SMALLEST_UPD_INTERVALL;
        END_IF;
        IF !!(pComCell^.udUpdateRate AND UPDRATE_FLAG_IS_GLOBALVAR) & 
           GetdataBufferFlag(pComCell^.pCh$^VirtualBase) THEN
          // The object has the flag ATTR_DATA_BUFFER set. For these object more than 
          // the 4 bytes of dData can be transmitted. The comlink server updates the cell 
          // when the crc changes instead of dData.
          pComCell^.udUpdateRate := pComCell^.udUpdateRate or UPDRATE_FLAG_IS_VBI;
        END_IF;
#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-~~~ ProcessCmd: dataLen too small");
#endif      
      END_IF;
      
    CMD_SSREFR:
      DEBUGLOG2("%010d-~~~ ProcessCmd: SSREFR received, typ=%d, count=%d", TO_UDINT(pData^$msgSsRefrReqData.typ), TO_UDINT(pData^$msgSsRefrReqData.count));
      // req-data:
      //    count       : UINT;
      //    typ         : UINT;
      // resp-data:
      //    this request requires no response

      IF dataLen >= SIZEOF(msgSsRefrReqData) THEN
      
        cmdProcessed := TRUE;
      
        IF pData^$msgSsRefrReqData.typ = 0 THEN
          // static list
          LoadUpdateList(#pLogin^.statList, pData^$msgSsRefrReqData.count);
        ELSE
          // dynamic list
          LoadUpdateList(#pLogin^.dynList, pData^$msgSsRefrReqData.count);
        END_IF;       
#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-~~~ ProcessCmd: dataLen too small");
#endif      
      END_IF;
      
    CMD_UPDCELL:
      DEBUGLOG2("%010d-~~~ ProcessCmd: UPDCELL received, dataLen=%d, VarPos=%d", dataLen$UDINT, TO_UDINT(pData^$msgUpdCellReqData.iOff));
      // req-data:
      //    iOff        : INT;
      //    comcell     : comStCell;
      // resp-data:
      //    this request requires no response
      
      IF dataLen >= SIZEOF(msgUpdCellReqData) THEN
      
        cmdProcessed := TRUE;
      
        uiOffs := pData^$msgUpdCellReqData.iOff$UINT;
        IF uiOffs < 1000 THEN
          pComCell := pLogin^.statList.pFirstCell;
          IF TO_UDINT(pData^$msgUpdCellReqData.iOff) >= pLogin^.statList.nbrCells THEN
            DEBUGLOG1("%010d-*** ProcessCmd: VarPos(%d) is too big for StatList", TO_UDINT(pData^$msgUpdCellReqData.iOff));
            RETURN;
          END_IF;
        ELSE 
          uiOffs -=1000;
          pComCell := pLogin^.dynlist.pFirstCell;
          IF TO_UDINT(uiOffs) >= pLogin^.dynList.nbrCells THEN
            DEBUGLOG1("%010d-*** ProcessCmd: VarPos(%d) is too big for DynList", TO_UDINT(pData^$msgUpdCellReqData.iOff));
            RETURN;
          END_IF;
        END_IF;
        
        pComCell        := pComcell + (SIZEOF(comStCell) * uiOffs);
        
        IF pComCell^.pCh <> pData^$msgUpdCellReqData.pCh THEN
          DEBUGLOG0("%010d-*** ProcessCmd: invalid upd-cell received");
          RETURN;
        END_IF;
        pComCell^.dData := pData^$msgUpdCellReqData.dData; 
  
        // call callback function      
        IF pLogin^.pCallback <> NIL THEN
          data2LSE.data      := pComCell^.dData;
          data2LSE.uiOffs    := pData^$msgUpdCellReqData.iOff$UINT;
          data2LSE.varlistID := pComCell^.VarlistID;
          original_this := ESI;
          ESI := pLogin^.pThis;
          pLogin^.pCallback$PrototypeCallbackT(pLogin^.pComdefT, #data2LSE);
          ESI := original_this;
          DEBUG_VERBOSE3("%010d-~~~ ProcessCmd: Callback(dData=0x%08X,uiOffs=%d,id=0x%08X", TO_UDINT(data2LSE.data), TO_UDINT(data2LSE.uiOffs), TO_UDINT(data2LSE.varlistID));
        END_IF;
#ifdef DEBUG  
      ELSE
        DEBUG_VERBOSE0("%010d-~~~ ProcessCmd: dataLen too small");
#endif      
      END_IF;
      
    CMD_MAXIDLETIME:
      DEBUGLOG1("%010d-~~~ ProcessCmd: CMD_MAXIDLETIME received, maxIdleTime=%d", pData^$msgIdlTimReqData.maxIdleTime_ms);
      // req-data:
      //    maxIdleTime_ms : UDINT;
      // resp-data:
      //    this request requires no response

      cmdProcessed := TRUE;
      
      SetMaxIdleTimeR(pLogin, pData^$msgIdlTimReqData.maxIdleTime_ms);

    CMD_IAMALIVE:
      DEBUGLOG0("%010d-~~~ ProcessCmd: IAMALIVE received");
      // req-data:
      // resp-data:
      //    this request requires no response
      
      cmdProcessed := TRUE;

    CMD_LOGOUT:
      DEBUGLOG0("%010d-~~~ ProcessCmd: LOGOUT received");
      // req-data:
      //    this request requires no data
      // resp-data:
      //    this request requires no response
      LoadUpdateList( #pLogin^.statList, 0 );
      LoadUpdateList( #pLogin^.dynList, 0 );

    ELSE
      // unknown command
      DEBUGLOG2("%010d-~~~ ProcessCmd: unknown command received, cmd=0x%X, dataLen=%d", cmd$UDINT, dataLen$UDINT);
      
  END_CASE;

END_FUNCTION

(*
  ProcessCmdAndRespR 
*)
FUNCTION ProcessCmdAndRespR
VAR_INPUT
  pLogin : ^loginContextR;
END_VAR
VAR
  pL1RecvBuffer : ^SER_L1_BUF;
  cmd : UINT;
  pData : pVoid;
  dataLen : UINT;
  respData : ARRAY[0..299] OF BYTE;
  respDataLen : UINT;
  seqNbr : UINT;
  ack : BOOL;
END_VAR

  pL1RecvBuffer := #pLogin^.rxL1BufReq;

  cmd := pL1RecvBuffer^.flagsCmd AND RS232_L1CMD_MASK;
  IF pL1RecvBuffer^.flagsCmd AND RS232_L1FLAGS_RESPONSE_REQUIRED THEN
    seqNbr := pL1RecvBuffer^.data$UINT;
    pData := #pL1RecvBuffer^.data[2];
    dataLen := pL1RecvBuffer^.dataLen - 2;
  ELSE
    pData := #pL1RecvBuffer^.data[0];
    dataLen := pL1RecvBuffer^.dataLen;
  END_IF;
  
  IF ProcessCmdR(pLogin, cmd, pData, dataLen, #respData[0], #respDataLen) THEN
    ack := TRUE;
  ELSE
    ack := FALSE;
  END_IF;

  IF pL1RecvBuffer^.flagsCmd AND RS232_L1FLAGS_RESPONSE_REQUIRED THEN
    SendResponseR(pLogin, seqNbr, ack, cmd, #respData[0], respDataLen);
  END_IF;

END_FUNCTION

(*
  Versucht, zu sendende Nodes aus der Queue zu entnehmen, in den L0-Buffer 
  zu stellen, und den L0-Buffer dann zu schicken.
  Empfangene Daten werden in die Receive-Queue gestellt.
  
  Die Funktion blockiert nicht. Sie ist nicht thread-safe und wird daher nur 
  aus dem Cyclic aufgerufen.
*)
FUNCTION ComlinkR_SerTransfer
VAR_INPUT
  pItf : ^RS232_ITF;
END_VAR
VAR_OUTPUT
  event : BOOL;
END_VAR
VAR
  pL0RecvBuffer : ^SER_L0_BUF;
  nextOfs : UDINT;
  nRead : UDINT;
  pL1Buffer : ^SER_L1_BUF;
  dstChIdx : UDINT;
  pLogin : ^loginContextR; // connection context
  flags : BYTE;
END_VAR

  event := FALSE;

  LOCK_R();
  IF pItf^.bytesToSend = 0 THEN
    // Es gibt dzt. nichts zu senden, daher im L0SendBufferCollect nachschauen, 
    // ob Daten vorhanden sind oder ob der Timeout für die anstehende Response 
    // abgelaufen ist.
    IF (pItf^.pL0SendBufferCollect^.dataLen > L0_EMPTY_DATALEN) | 
       (pItf^.l0RespNecessary & (ops.tAbsolute - pItf^.l0RespNecessaryTimeval_ms)$DINT > 0)
    THEN
      // Es soll was gesendet werden. 
      // -> L0SendBufferCollect auf den anderen Buffer umschalten und das 
      // Senden anstossen, indem bytesToSend auf >0 gesetzt wird.
      pItf^.pL0SendBufferCollect := SwitchL0Buffer(pItf, pItf^.pL0SendBufferCollect);
      pItf^.bytesToSend := pItf^.pL0SendBufferTransfer^.dataLen + L0_EMPTY_DATALEN;
      pItf^.bytesSent := 0;
      DEBUGLOG1("%010d-    SerTransfer: L0 send data available, bytesToSend: %d", TO_UDINT(pItf^.bytesToSend));
      OS_MT_Pulse(comlinkEventR);
      event := TRUE;
    END_IF;
  END_IF;

  IF pItf^.bytesToSend > 0 THEN
    // Es gibt was zu senden.
    IF !!pItf^.sendInProgress THEN
      L0CheckSend(pItf);
    END_IF;
    IF pItf^.sendInProgress THEN
      L0Send(pItf);
    END_IF;
    IF pItf^.bytesToSend = 0 THEN
      // Es konnte alles gesendet werden
      // -> Den L0SendBufferTransfer so vorbereiten, dass er wieder Daten aufsammeln 
      // kann und dann L0SendBufferTransfer auf den anderen Buffer umschalten.
      IF pItf^.pL0SendBufferTransfer^.flagsCmd AND RS232_L0FLAGS_IS_REQUEST THEN
        // es ist ein Request gesendet worden, daher ist ab jetzt eine Response anstehend
        pItf^.l0RespPending := TRUE;
        pItf^.l0RespPendingTimeout_ms := ops.tAbsolute + pItf^.timeoutVal;
      ELSE
        // es ist eine Response gesendet worden, daher ist ab jetzt keine mehr anstehend,
        pItf^.l0RespNecessary := FALSE;
      END_IF;
      pItf^.pL0SendBufferTransfer^.dataLen := L0_EMPTY_DATALEN;
      pItf^.pL0SendBufferTransfer := SwitchL0Buffer(pItf, pItf^.pL0SendBufferTransfer);
      pItf^.sendInProgress := FALSE;
      DEBUGLOG2("%010d->>> SerTransfer: L0 finished send, new flags: respPending=%d, respNecessary=%d", pItf^.l0RespPending, pItf^.l0RespNecessary);
    END_IF;
  END_IF;

  pL0RecvBuffer := L0Recv(pItf);
  UNLOCK_R();
  
  IF pL0RecvBuffer <> NIL THEN
    DEBUGLOG1("%010d-<<< SerTransfer: L0 received data, datlen=%d", pL0RecvBuffer^.dataLen);
    nextOfs := SER_L0_BUF.data;
    WHILE nextOfs < (pL0RecvBuffer^.dataLen + 2) DO
      pL1Buffer := ScanL1Block(pL0RecvBuffer, #nextOfs, #nRead);
      flags := pL1Buffer^.flagsCmd AND (RS232_L1FLAGS_IS_REQUEST OR 
                                        RS232_L1FLAGS_RESPONSE_REQUIRED);
      IF flags = (RS232_L1FLAGS_IS_REQUEST OR RS232_L1FLAGS_RESPONSE_REQUIRED) THEN
        // Es handelt sich um einen Request, der beantwortet werden möchte.
        DEBUGLOG2("%010d-<1  SerTransfer: got L1 Request that needs a response, dst=%d, src=%d", TO_UDINT(pL1Buffer^.dstChIdx), TO_UDINT(pL1Buffer^.srcChIdx));
        dstChIdx := pL1Buffer^.dstChIdx;
        IF (dstChIdx < 255) & (loginArrayR[dstChIdx] <> NIL) THEN
          DEBUG_VERBOSE0("%010d-*** SerTransfer: processing command");
          pLogin := loginArrayR[dstChIdx];
          _memcpy(#pLogin^.rxL1BufReq, pL1Buffer, nRead);
          ProcessCmdAndRespR(pLogin);
        ELSIF dstChIdx = 255 & 
              GetL1CmdR(pL1Buffer) = CMD_CONNECT & 
              pL1Buffer^.dataLen >= 2 THEN
          DEBUGLOG0("%010d-*** SerTransfer: processing login");
          // es will sich jemand einloggen
          pLogin := SearchUsedConnBlockSvrR(pItf^.itfIdx, pL1Buffer^.srcChIdx);
          IF pLogin = NIL THEN
            pLogin := AllocNewConnBlockSvrR(pItf^.itfIdx, pL1Buffer^.srcChIdx);
            DEBUGLOG2("%010d-    SerTransfer: allocated new channel, myChIdx=%d, hisChIdx=%d", pLogin^.myChIdx, pLogin^.hisChIdx);
          ELSE
            DEBUGLOG2("%010d-    SerTransfer: re-using old channel, myChIdx=%d, hisChIdx=%d", pLogin^.myChIdx, pLogin^.hisChIdx);
          END_IF;
          IF pLogin <> NIL THEN
            ResetL1(pLogin);
            // Die MaxIdleTime wird auf einen Wert <> 0 gesetzt, damit der Server 
            // die Verbindung abbricht, wenn keine Kommunikation mehr stattfindet. 
            // Das ist bei RS232 wichtig, weil sich ja um eine temporäre Modemverbindung 
            // handeln könnte.
            SetMaxIdleTimeR(pLogin, 10000(*10Sek.*));
            
            pLogin^.hisChIdx := pL1Buffer^.srcChIdx;
            pLogin^.isConnected := TRUE;
#ifndef DUMMY_LOADER
            pLogin^.Com.InitCode(FALSE);
#endif        
            SendResponseR(pLogin, 
                          GetL1SeqNbrR(pL1Buffer),
                          ack := TRUE,
                          pL1Buffer^.flagsCmd AND RS232_L1CMD_MASK,
                          pData := NIL,
                          dataLen := 0);
          END_IF;
          
        END_IF;
      ELSIF flags = RS232_L1FLAGS_IS_REQUEST THEN
        DEBUGLOG2("%010d-<1  SerTransfer: got L1 Request, dst=%d, src=%d", TO_UDINT(pL1Buffer^.dstChIdx), TO_UDINT(pL1Buffer^.srcChIdx));
        dstChIdx := pL1Buffer^.dstChIdx;
        IF (dstChIdx < 255) & (loginArrayR[dstChIdx] <> NIL) THEN
          DEBUG_VERBOSE0("%010d-*** SerTransfer: processing command");
          pLogin := loginArrayR[dstChIdx];
          _memcpy(#pLogin^.rxL1BufReq, pL1Buffer, nRead);
          ProcessCmdAndRespR(pLogin);
        END_IF;
      ELSE
        // Es handelt sich um eine Response
        DEBUGLOG2("%010d-    SerTransfer: got L1 Response, dst=%d, src=%d", TO_UDINT(pL1Buffer^.dstChIdx), TO_UDINT(pL1Buffer^.srcChIdx));
        dstChIdx := pL1Buffer^.dstChIdx;
        IF (dstChIdx < 255) & (loginArrayR[dstChIdx] <> NIL) THEN
          pLogin := loginArrayR[dstChIdx];
          IF GetL1CmdR(pL1Buffer) = CMD_CONNECT THEN
            pLogin^.hisChIdx := pL1Buffer^.srcChIdx;
          END_IF;
          IF pL1Buffer^.srcChIdx = pLogin^.hisChIdx THEN
            DEBUGLOG0("%010d-    SerTransfer: finish pending response");
            _memcpy(#pLogin^.rxL1BufResp.dstChIdx, pL1Buffer + 1, nRead - 1);
            // Wenn srcChIdx im L1 RxBuffer auf <>INVALID_CHIDX gesetzt wird, dann heisst das, 
            // dass der Buffer voll ist und vom Aufrufer ausgewertet werden darf.
            pLogin^.rxL1BufResp.srcChIdx := pL1Buffer^$BYTE;
            OS_MT_Pulse(comlinkEventR);
            event := TRUE;
          ELSE
            DEBUGLOG0("%010d-*** SerTransfer: no pending response in this channel");
            DisconnectAfterErrorR(pLogin);
          END_IF;
        END_IF;
      END_IF;
    END_WHILE;
  END_IF;
  
END_FUNCTION

//------------------------------------------------------------------------------
// ComlinkR_CyWork
//------------------------------------------------------------------------------
FUNCTION ComlinkR_CyWork
VAR
  i : UDINT;
  pLogin : ^loginContextR; // connection context
  pItf : ^RS232_ITF;
END_VAR

  IF !!g_RS232activity THEN
    // Aus Performancegründen wird hier gleich zurückgesprungen, wenn keiner 
    // eine RS232 Kommunikation anfgefordert hat.
    RETURN;
  END_IF;


  FOR i := 0 TO arraysize(rs232Itfs)-1 DO
    pItf := #rs232Itfs[i];
    IF pItf^.hComm <> 0 THEN
      IF pItf^.fnInitInterface <> NIL THEN
        // Callback Funktion aufrufen, damit die Anwendung die Möglichkeit hat, eine 
        // Modemverbindung zu beenden
        IF pItf^.fnInitInterface$LDR_InitRs232Interface(
              pItf^.pThis, ComlinkR_Idx2Itfnbr(i$DINT), pItf^.hCommAppl) <= 0 THEN
          LOCK_R();
          pItf^.hComm := 0;
          UNLOCK_R();
          EXIT;
        END_IF;
      END_IF;
      ComlinkR_SerTransfer(pItf);
      
      // Wenn das RespPending Flag länger als l0RespPendingTimeout_ms ansteht, 
      // dann ist wahrscheinlich die Gegenstation nicht mehr aktiv.
      IF pItf^.l0RespPending & ((ops.tAbsolute - pItf^.l0RespPendingTimeout_ms)$DINT > 0) THEN
        DEBUGLOG0("%010d-*** ComlinkR_CyWork: timeout for l0RespPending exceeded, resetting itf");
        DisconnectAllForItf(pItf);
        ResetL0(pItf);
      END_IF;
      
    ELSIF pItf^.startServer THEN
      pItf^.hComm := ComlinkR_GetHandle(ComlinkR_Idx2Itfnbr(i$DINT));
    END_IF;
  END_FOR;

  FOR i := 0 TO (arraysize(loginArrayR) - 1) DO
    pLogin := loginArrayR[i];
    IF pLogin <> NIL & !!pLogin^.isClient THEN
      IF IsConnectedR(pLogin) THEN
        IF pLogin^.txL1BufCmd.dataValid THEN
          // Es gibt eine Response, die wegen zu wenig Platz im L0 Buffer 
          // noch nicht gesendet werden konnte
          LOCK_R();
          PackL1Block(pLogin, #pLogin^.txL1BufCmd);
          UNLOCK_R();
        END_IF;
      
        UpdateListProcessorR(pLogin);
        
        IF pLogin^.maxIdleTime_ms > 0 THEN
          IF ops.tAbsolute - pLogin^.lastSendTime_ms > pLogin^.maxIdleTime_ms THEN
            SendAliveMessage_RS232(pLogin); // der Gegenstelle mitteilen, daß man noch lebt
          END_IF;
        END_IF;
      END_IF;
    END_IF;
  END_FOR;

END_FUNCTION

(*
  Diese Funktion wird von der Anwendung aufgerufen, wenn entweder die 
  Standardeinstellungen der RS232 Schnittstelle verändert werden sollen, 
  oder wenn der Loader einen Server auf einer RS232 Schnittstelle aktivieren 
  soll.
  Der Server ist auf einer RS232 Schnittstelle standardmäßig nicht aktiv, 
  weil sonst wären alle verfügbaren RS232 Schnittstellen vom Loader belegt.
*)
FUNCTION GLOBAL __CDECL LDR_SetRs232ComlinkParams
VAR_INPUT
  interface : UDINT;
  baudrate : UINT;
  startServer : DINT;
  fnInitInterface : pVoid;
  pThis : pVoid;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR
VAR
  idx : DINT;
END_VAR

  retVal := 0;

  idx := ComlinkR_Itfnbr2Idx(interface);

  IF idx >= 0 THEN
    IF baudrate <> 16#FFFF THEN
      rs232Itfs[idx].baudrate := baudrate;
      rs232Itfs[idx].timeoutVal := GetL0TimeoutValR(rs232Itfs[idx].baudrate);
      DEBUGLOG2("%010d-    LDR_SetRs232ComlinkParams: itf %d - timeoutVal=%d", idx$UDINT, rs232Itfs[idx].timeoutVal);
    END_IF;
    IF startServer <> -1 THEN
      rs232Itfs[idx].startServer := startServer;
      g_RS232activity := TRUE;
    END_IF;
    IF fnInitInterface <> NIL THEN
      rs232Itfs[idx].fnInitInterface := fnInitInterface;
      rs232Itfs[idx].pThis := pThis;
    END_IF;
  ELSE
    retVal := -1;
  END_IF;

END_FUNCTION

VAR_GLOBAL
  aSoc          : ARRAY [0..63] OF DINT;
END_VAR

#ifdef LOADER_TCP_MODERATE
FUNCTION TCPIP_Task
VAR_INPUT
  pvoid       : ^void;
END_VAR
VAR
  i           : DINT;
  SocNum      : DINT; 
  aSocPtr     : ARRAY [0..63] OF ^bufferedsocket;
  pLogin      : ^loginContext;
  pCmdConn    : ^cmdConnBlock;
  pRefrConn   : ^refrConnBlock;
END_VAR

  while 1 do
    OS_MT_DELAY(5);

    //--------------------------------------------------------------------
    // reset the number of sockets
    //--------------------------------------------------------------------
    SocNum := 0;

    //--------------------------------------------------------------------
    // T C P   C L I E N T   S O C K E T S
    //--------------------------------------------------------------------
    if (loginanz) then
      for i := 0 to (g_max_logins - 1) do
        pLogin := loginarray[i];
        if (pLogin <> NIL) then
          aSoc[SocNum] := pLogin^.refrSockClnt.sock;
          aSocPtr[SocNum] := #pLogin^.refrSockClnt;
          SocNum += 1;
        end_if;
      end_for;
    end_if;

    //--------------------------------------------------------------------
    // T C P   C O M M A N D   S E R V E R   S O C K E T S  
    //--------------------------------------------------------------------
    if (nbrCmdConn) then
      pCmdConn := pCmdConnRoot;
      while (pCmdConn <> NIL) do
        aSoc[SocNum] := pCmdConn^.bsock.sock;
        aSocPtr[SocNum] := #pCmdConn^.bsock;
        SocNum += 1;
        pCmdConn := pCmdConn^.next;
      end_while;
    end_if;

    //--------------------------------------------------------------------
    // T C P   R E F R E S H   S E R V E R   S O C K E T S
    //--------------------------------------------------------------------
    if (nbrRefrConn) then
      pRefrConn := pRefrConnRoot;
      while (pRefrConn <> NIL) do
        aSoc[SocNum] := pRefrConn^.bsock.sock;
        aSocPtr[SocNum] := #pRefrConn^.bsock;
        SocNum += 1;
        pRefrConn := pRefrConn^.next;
      end_while;
    end_if;

    //--------------------------------------------------------------------
    // // look for available data
    //--------------------------------------------------------------------
    if (SocNum) then
      OS_TCP_USER_SELECT(SocNum,                  // Anzahl der Sockets
                         #aSoc[0],                // Pointer auf Socket-Array
                         SELECT_OPT_READ,         // Option
                         0,                       // Flag -> wird 
                         0);

      for i := 0 to (SocNum - 1) do
        if (aSoc[i] > 0) then
          aSocPtr[i]$^bufferedsocket^.isDataAvailable := true;
        end_if;
      end_for;
    end_if;

  end_while;

END_FUNCTION
#endif

#endif // COMLINK_LASAL

