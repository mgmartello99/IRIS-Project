#include ".\Loader\RTOS_PrivHeader.h"
//#include <RTOS_Variables.h>
#include <LSL_ST_IFLINKER.H>
#include ".\Loader\RTOS_MemIntern.h"
#ifdef COMLINK_LASAL
#include ".\Loader\ComTypes.h"
#endif // COMLINK_LASAL

#include ".\Loader\RTOS_IprIntern.h"
//#include <RTOS_Private.h>
#include ".\include\HashTab.h"
#ifndef DUMMY_LOADER
#include ".\LoaderItf.h"
#endif


#ifdef LSL_TEST
//#define CHECK_HEAP	// define this to check heap
#endif // LSL_TEST

FUNCTION GLOBAL Ini_Com_IF;
FUNCTION GLOBAL ComLink;
FUNCTION GLOBAL MakeVirtClient
VAR_OUTPUT
	pMtbl		: ^CmdMeth;
END_VAR;
FUNCTION GLOBAL _CalcDescCRC;
FUNCTION GLOBAL LDR_CommitDescrBlocks
VAR_INPUT
  pPrjRd			: pVoid;
  pFirstCls		: pVoid;
  pLastCls		: pVoid;
END_VAR;
FUNCTION GLOBAL LDR_LoadDescrBlocks
VAR_INPUT
  ppPrjRd			: ^pVoid;
  ppFirstCls	: ^pVoid;
  ppLastCls	: ^pVoid;
END_VAR
VAR_OUTPUT
  result : BOOL;
END_VAR;
FUNCTION GLOBAL LDR_free_descrSave
VAR_INPUT
	pMem            : pVOID;
END_VAR;
FUNCTION GLOBAL LDR_GetDescrBlocksFlag
VAR_OUTPUT
	retVal : BOOL;
END_VAR;
FUNCTION GLOBAL LDR_ClsHashTable_Initialize
VAR_OUTPUT
  clsHashTab : HashTable;
END_VAR;
FUNCTION GLOBAL LDR_SortCRCMapIfNececcary;
FUNCTION GLOBAL LDR_InitCRCMap;
FUNCTION GLOBAL RamExCleanUp;
FUNCTION GLOBAL   GET_USED_S_RAM;  // Whenever new S-RAM is alocatetd,S_Ram saver must be informed
FUNCTION GLOBAL LDR_InitLookup;

#ifndef DUMMY_LOADER
#pragma using ProjectReader
#pragma using IprMgr
#pragma using TaskReader
#pragma using MMConnectionHandler
#endif

TYPE
  // Spezielle Version von CmdMeth mit insgesamt 256 Methoden (6 Std. + 250 User)
  // Wird für die Methodentabelle zur Erkennung des Aufruf unverbundener Objektkanäle 
  // benötigt.
	CmdMeth_256			: STRUCT		// minimum structure of a command meth table
		Read		: pFct;
		Write		: pFct;
		pNxt		: ^CMDMETH;
		nCmds		: CMDMETHSIZE;	// #succeeding function pointers
		Init		: pFct;
		CyWork		: pFct;
		RtWork		: pFct;
		NewInstr	: pFct;
		Kill		: pFct;
		GetState	: pFct;
		// pointers to user defined methods may follow
    UserMethods : ARRAY[0..249] OF pFct;
		END_STRUCT;
  PostInitCBList : STRUCT
    pThis : ^VirtualBase;
    newInstCmd : UINT;
    pNext : ^PostInitCBList;
  END_STRUCT;
END_TYPE

VAR_GLOBAL
	sbCallInit		: TCallInit;	// standard method lists
	
	g_LoadTimeout	: UDINT;//Zeit um die das Laden verzögert wird

    Loader_time1 :UDINT;
    Loader_time2 :UDINT;
    Loader_time3 :UDINT;
    Loader_time4 :UDINT;
    Loader_time5 :UDINT;
    Loader_time6 :UDINT;
    Loader_time7 :UDINT;
    Loader_time8 :UDINT;
    Loader_time9 :UDINT;
    Loader_time10 : UDINT; //ZP000
    bWait4Visu  : BOOL; //ZP000
    wait4Visu_timeout_ms : UDINT;

#ifdef LDR_RETSVR
    usHasRetentive : usint;
    usCheckRetentive : usint;
    usReportRetentive : usint;
    udSRAMVersion : udint;
#endif  
  g_postInitCB_pThis : ^VirtualBase;
  g_postInitCB_newInstCmd : UINT;
END_VAR

VAR_PRIVATE
  methTabBlks : ContainerBlockPtr;
	pFirstCmd_ldr	: ^CMDMETH;		 // ptr to first command table (Loader objects)
#if 0  
  g_postInitCB_pThis : ^VirtualBase;
  g_postInitCB_newInstCmd : UINT;
#endif  
  postInitCBs : ^PostInitCBList;
  LDR_ConfigFlags : UDINT;
END_VAR

VAR_EXTERNAL

	pFirstCls			: ^ClsHdr;       // ptr to first class descriptor
	pLastCls			: ^ClsHdr;
  clsHashTab    : HashTable;
	pFirstCmd			: ^CMDMETH;		 // ptr to first command table
	pMapper				: ^CRCMap;		 // to find faster the CRC and the objects

    LDR_outOfSramMem : UDINT; // TRUE, wenn kein Platz mehr für eine RAM- oder RAMEx- Zelle vorhanden ist

END_VAR


//============================================================================================
// Header of global functions
//============================================================================================

FUNCTION GLOBAL S_RAM_INIT
  VAR_OUTPUT
	ret_code 		: CONFSTATES;
  END_VAR;
FUNCTION GLOBAL SramSaveFile;
FUNCTION GLOBAL LDR_FreeSramKopie;
FUNCTION GLOBAL MEM_Init;
#ifndef DUMMY_LOADER
FUNCTION GLOBAL DebugIp_Init;
#endif
FUNCTION GLOBAL LDR_HasSram
VAR_OUTPUT
  ret   : BOOL;
END_VAR;


FUNCTION GLOBAL _LookupCls
VAR_INPUT
	pName           : ^CHAR;
END_VAR
VAR_OUTPUT
	pCls            : ^ClsHdr;
END_VAR;


//============================================================================================
// Header of global functions
//============================================================================================
FUNCTION GLOBAL __CDECL _GetObjectName
VAR_INPUT
	pO			: ^Obj;
	pCh			: ^void;
	pName		: ^CHAR;
END_VAR;

FUNCTION GLOBAL	CheckIprProgs
VAR_OUTPUT
	ret_code	: ConfStates;
END_VAR;

FUNCTION GLOBAL OpCyclic;

(* Mit dieser Funktion kann die Applikation ein bestimmtes Verhalten des Loaders 
   festlegen.
   z.B. LDR_DO_NOT_USE_RAMEX_FOR_IPRPROG bewirkt, dass Interpreterprogramme nicht 
   in Ramex- sondern in MerkerEx Objekten gespeichert werden.
   Zurückgegeben wird die aktuelle Einstellung.
   Unbekannte Flags werden nicht übernommen.
 *)
FUNCTION GLOBAL LDR_SetConfigFlags
VAR_INPUT
  flags : UDINT;
END_VAR
VAR_OUTPUT
  actFlags : UDINT;
END_VAR
#ifndef DUMMY_LOADER
  IF flags AND LDR_DO_NOT_USE_RAMEX_FOR_IPRPROG THEN
    LDR_ConfigFlags := LDR_ConfigFlags OR LDR_DO_NOT_USE_RAMEX_FOR_IPRPROG;
  END_IF;
#endif  
  actFlags := LDR_ConfigFlags;
END_FUNCTION

// categories of error messages, displayed by LASAL
//
//#define IDS_RUNTIME_LOADER_ERROR	11124
//#define IDS_RUNTIME_RTDB_ERROR		11125
#define IDS_RUNTIME_TRACE			11126

(* Installiert eine Callback-Methode, die nach den Initmethoden als NewInst aufgerufen wird.
   Mit dieser Funktion kann wird nur eine einzige Callback-Methode gespeichert, 
   d.h. bei einem erneuten Aufruf wird der alte Wert überschrieben.
*)
FUNCTION GLOBAL LDR_InstallPostInitCB
VAR_INPUT
  pThis : ^VirtualBase;
  pCmdSvr : ^SVRCHCMD;
  newInstCmd : UINT;
END_VAR
  if(g_postInitCB_pThis = nil) then
    if(pThis$^void = pCmdSvr$^void) then
      g_postInitCB_pThis      := pThis;
      g_postInitCB_newInstCmd := newInstCmd;
    else
      TRACE_WARN("PostInit Callback will just work with NewInst of ClassSvr");  
    end_if;
  else
    TRACE_WARN("PostInit Callback already in use");  
  end_if;
  
END_FUNCTION

(* Fügt eine eine Callback-Methode in eine Liste ein, die nach den Initmethoden als NewInst 
   aufgerufen wird. Der Aufruf erfolgt noch vor der mit LDR_InstallPostInitCB installierten 
   Callback-Methode.
*)
FUNCTION GLOBAL LDR_InstallPostInitCB_Add
VAR_INPUT
  pThis : ^VirtualBase;
  pCmdSvr : ^SVRCHCMD;
  newInstCmd : UINT;
END_VAR
VAR
	pNode : ^PostInitCBList;
END_VAR
  
  if(pThis$^void = pCmdSvr$^void) then
    pNode := _alloc(sizeof(PostInitCBList))$^PostInitCBList;
    pNode^.pThis      := pThis;
    pNode^.newInstCmd := newInstCmd;
    pNode^.pNext := postInitCBs;
    postInitCBs := pNode;
  else
    TRACE_WARN("PostInit Callback will just work with NewInst of ClassSvr");  
  end_if;
  
END_FUNCTION

(* Resettiert die für LDR_InstallPostInitCB und LDR_InstallPostInitCB_Add benötigten Variablen *)
FUNCTION ResetPostInitCB
  g_postInitCB_pThis := NIL;
  g_postInitCB_newInstCmd := 0;
  postInitCBs := NIL;
END_FUNCTION

(* Ruft die mit LDR_InstallPostInitCB_Add und LDR_InstallPostInitCB installierten Callbackfunktionen auf *)
FUNCTION CheckPostInitCB
VAR
  tempCmd : CmdStruct;
  _result : results;
	pNode : ^PostInitCBList;
END_VAR

  pNode := postInitCBs;
  WHILE pNode DO
    _memset(#tempCmd, 0, sizeof(tempCmd));
    tempCmd.uiCmd := pNode^.newInstCmd;
    pNode^.pThis^.NewInst(#tempCmd, #_result);

    pNode := pNode^.pNext;
  END_WHILE;

  IF g_postInitCB_pThis THEN
    _memset(#tempCmd, 0, sizeof(tempCmd));
    tempCmd.uiCmd := g_postInitCB_newInstCmd;
    g_postInitCB_pThis^.NewInst(#tempCmd, #_result);
  END_IF;
  
END_FUNCTION

// A message is inserted to a OS-message queue. The LASAL debugger
// will periodically try to fetch these messages and display it in 
// the compiler output window
// -> udLine	: Line in the source file, use __LINE__ in macros
// -> pFile		: file name, use __FILE__ in macros
// -> pText		: message text
// Remark: use the TRACE-macro to call this function: TRACE("message");
//
FUNCTION GLOBAL _OutMessage
VAR_INPUT
	udLine		: DWORD;
	pFile		: ^CHAR;
	pText		: ^CHAR;
	nMsgType  	: CHAR;
END_VAR
VAR
	szMessage		: ARRAY[0..3*NLNG] OF CHAR;
  uiLen       : UDINT;
  uiCopyLen   : UDINT;
END_VAR

	IF _RtOSversion >= 16#032F THEN
		szMessage[0] := nMsgType;
		_itoa(udLine$UINT, #szMessage[1]);
		_strcat(#szMessage[0], "$01");
		_strcat(#szMessage[0],  pFile);
		_strcat(#szMessage[0], "$01");
    
    // Copy array bytewise
    uiLen := _strlen(#szMessage[0]);
    uiCopyLen := 0;
    while uiCopyLen < (sizeof(szMessage) - 4 - uiLen) do
      szMessage[uiLen + uiCopyLen] := (pText + uiCopyLen)^;
      uiCopyLen := uiCopyLen + 1;
    end_while;
  
    szMessage[sizeof(szMessage)-3] := 0;
    _strcat(#szMessage[0], "$01");


		OS_LNK_Add2ErrorList(IDS_RUNTIME_TRACE, #szMessage[0]);
	END_IF;

END_FUNCTION
// Set the pointers to the new server and send a message to
// the connected object.
FUNCTION GLOBAL SetConnection
VAR_INPUT
	pObj_ChClt		: ^CltCh;
	pObjClt			: ^VirtualBase;
	pObj_ChSvr		: ^SvrCh;
	pObjSvr			: ^VirtualBase;
END_VAR
VAR_OUTPUT
	ret_code		: ConfStates;
END_VAR
VAR
	CmdStruct		: CmdStruct;
	Result			: Results;
END_VAR

	ret_code	:= C_OK;

	          // connect client channel and server channel or object
	pObj_ChClt^.pCh  := pObj_ChSvr;
	pObj_ChClt^.pCmd := pObjSvr;

	// send message to the connected client object
	CmdStruct.uiCmd		:= LSL_MSG_CONNECTED;
	CmdStruct.aPara[0]	:= pObj_ChClt$DINT;	// this is the connected channel

	CASE pObjClt$^VirtualBase^.NewInst(#CmdStruct, #Result) OF
	BUSY:	ret_code	:= C_WRONG_CONNECT;
	READY:	ret_code	:= C_OK;
	//ELSE	default method returns ERROR
	END_CASE;

END_FUNCTION
// Send a message to the object with the client to prepare for a
// disconnection and disconnect the client afterwards.
//[>pObj_ChClt] points to the client that is to disconnect
//[>pObjClt] points to the object which the client belongs to
//[<ret_code] C_WRONG_CONNECT or C_OK
FUNCTION GLOBAL SetDisConnect
VAR_INPUT
	pObj_ChClt		: ^CltCh;
	pObjClt			: ^VirtualBase;
END_VAR
VAR_OUTPUT
	ret_code		: ConfStates;
END_VAR
VAR
	CmdStruct		: CmdStruct;
	Result			: Results;
END_VAR

	ret_code	:= C_OK;

	// send message to the connected client object
	CmdStruct.uiCmd		:= LSL_MSG_DISCONNECT;
	CmdStruct.aPara[0]	:= pObj_ChClt$DINT;	// this is the connected channel

	CASE pObjClt$^VirtualBase^.NewInst(#CmdStruct, #Result) OF
	BUSY:	ret_code	:= C_WRONG_CONNECT;
//	READY,
	ELSE	//default method returns ERROR
		          // connect client channel and server channel or object
		pObj_ChClt^.pCh  := (#pObj_ChClt^.pCmd)$^SvrCh;
		pObj_ChClt^.pCmd := sbCallInit.pCltMeth$^VirtualBase;

	END_CASE;

END_FUNCTION
//    
//    C_CONNECT - command
//    
//    This function reads the configurated connections of one
//    object and initializes the pointer to the channels.
//    All objects, that are used for a connection, must have
//    been defined previously.
//    
// [>pScan]    : pointer to the text buffer
// [>bOnePair] : the string only contains the names of one pair of Svr/Clts, def. FALSE
// [<ret_code] : C_OK             if all connections could be established
//               C_UNKNOWN_OBJECT if an object name couldn't be found
//               C_UNKNOWN_CHNL   if a channel name couldn't be found
//
FUNCTION GLOBAL _ReadConnect
VAR_INPUT
	pScan           : ^char;
	bOnePair		: BOOL := FALSE;
END_VAR
VAR_OUTPUT
	ret_code        : CONFSTATES;
END_VAR
VAR
	eChMode			: ChMode;
	pObj_ChClt		: ^CltCh;
	pObj_ChSvr		: ^SvrCh;
	pObjSvr			: ^VirtualBase;
	pObjClt			: ^VirtualBase;
	pScanSave   : ^char;
END_VAR

	ret_code		:= C_WRONG_CONNECT;

	WHILE TRUE DO
	                           // read client side of the connection
    pScanSave := pScan;
		eChMode := _LookupEmbedded(#pScan, #pObj_ChClt, #pObjClt);

		CASE eChMode OF
		_CH_NONE:
			IF pScanSave^ = 0 & !!bOnePair THEN
          ret_code := C_OK;	// end of connection pairs
          EXIT;
			END_IF;
			ret_code := C_UNKNOWN_CHNL;
			EXIT;					// unknown channel or object name
		_CH_OBJ,
		_CH_SVR,
		_CH_CMD,
		_CH_SVR_OBJ:
			ret_code := C_UNKNOWN_CHNL;
			EXIT;					// a server channel can't be connected
		END_CASE;

		IF pScan^ = '^' THEN
			SetDisConnect(pObj_ChClt, pObjClt);	// remove connection
			IF ret_code <> C_OK THEN
				RETURN;
			END_IF;
			GOTO NEXT_CON;
		END_IF;

		eChMode := _LookupEmbedded(#pScan, #pObj_ChSvr, #pObjSvr);

		CASE eChMode OF
		_CH_NONE,
		_CH_CLT_CMD,
		_CH_CLT_DATA,
		_CH_CLT_EMB,
		_CH_CLT_OBJ:
			ret_code := C_UNKNOWN_CHNL;
			EXIT;                     // a server channel can't be connected
		END_CASE;

		ret_code	:= SetConnection(pObj_ChClt, pObjClt, pObj_ChSvr, pObjSvr);
		IF ret_code <> C_OK THEN
			RETURN;
		END_IF;
		IF bOnePair THEN
      EXIT;
    END_IF;

	NEXT_CON:
	END_WHILE;

END_FUNCTION
//    
//    
//    Ermittelt den Zeiger auf den Sprungleisten-Eintrag einer
//    globalen Funktion anhand ihres Namens.
//    Funktioniert wie LFA, aber zur Laufzeit.
//    
//    --> szFctName: Ascii-String mit dem Funktionsnamen
//    <-- pFct     : Zeiger auf die Funktion

FUNCTION GLOBAL _FindFctName
VAR_INPUT
	szFctName		: ^char;
END_VAR
VAR_OUTPUT
	pFct			: pFct;
END_VAR
VAR
	isExe			: Char;
END_VAR

//~menk
	pFct := OS_LNK_GetAddress_Function(szFctName, #isExe);

END_FUNCTION
// Initializes a command table with pointers to standard
// methods. Ensures, every standard function can be called
// for every channel.
//
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL InitCmdTable
VAR_INPUT
	nCmd			: UINT;
	pCmd			: ^CMDMETH;
END_VAR
VAR
	ptc				: ^PVOID;
	anz				: UINT;
END_VAR

	ptc := #pCmd^.Init;
  anz := nCmd;

	while anz do
		ptc^ := #M_NO_F();
		ptc  += SIZEOF(pVoid);
    anz  -= 1;
	end_while;

	pCmd^.Read		:= #M_RD_DIRECT();

	pCmd^.Write		:= #M_NO_FUNCT();

	pCmd^.NewInstr	:= #M_NO_CMD();

	pCmd^.GetState	:= #M_NO_GETSTAT();

	pCmd^.nCmds		:= nCmd;

END_FUNCTION
#else
FUNCTION AWL GLOBAL InitCmdTable
VAR_INPUT
	nCmd			: UINT;
	pCmd			: ^CMDMETH;
END_VAR
VAR
	ptc				: ^PVOID;
	i				: UINT;
END_VAR

//	ptc := #pCmd^.Init;
	l.edi		pCmd
	add.edi		CMDMETH.Init

//	FOR i:= 0 TO nCmd-1 DO
//		ptc^ := #M_NO_F();
//		ptc  += SIZEOF(pVoid);
//	END_FOR;

	l.eax		#M_NO_F()		
	lzxw.ecx	ncmd
	jecxz		InitCmdTableNo
InitCmdTable	
	s.eax		(edi)
	add.edi		SIZEOF(pVoid)
	loop		InitCmdTable
InitCmdTableNo

	l.edi		pCmd
	
//	pCmd^.Read		:= #M_RD_DIRECT();
	l.eax		#M_RD_DIRECT()	
	s.eax		(edi+CMDMETH.Read)

//	pCmd^.Write		:= #M_NO_FUNCT();
	l.eax		#M_NO_FUNCT()	
	s.eax		(edi+CMDMETH.Write)

//	pCmd^.NewInstr	:= #M_NO_CMD();
	l.eax		#M_NO_CMD()	
	s.eax		(edi+CMDMETH.NewInstr)

//	pCmd^.GetState	:= #M_NO_GETSTAT();
	l.eax		#M_NO_GETSTAT()	
	s.eax		(edi+CMDMETH.GetState)

//	pCmd^.nCmds		:= nCmd;
	lzxw.eax	nCmd
	s.eax		(edi+CMDMETH.nCmds)


END_FUNCTION
#endif

//    
//    Stores a method table for a command server
//    
//    mode: SHARED		try to use existing method tables
//          EXCLUSIVE	create method table for each object
//

//    
//    Stores a method table for a command server
//    
//    mode: SHARED		try to use existing method tables
//          EXCLUSIVE	create method table for each object
//

FUNCTION GLOBAL StoreCmdEx
VAR_INPUT
	pCmd            : ^CMDMETH;
	mode            : CMDMETHMODE;
	useOsResources	: BOOL;
  allocPermanent : BOOL; // TRUE=Speicher kann in einen Bereich gelegt werden, der nicht freigegeben werden muss
END_VAR
VAR_OUTPUT
	pMeth           : ^CMDMETH;
END_VAR
VAR
	nUserMeths		: CMDMETHSIZE;
	nCmdSize		: UINT;
	Thisp           : UDINT;
	pObj			: ^OBJ;
	pOrigChnl       : ^SvrChCmd;   
	pTemp           : ^CMDMETH;
END_VAR

  Thisp:=ESI;

  IF (Mode <> SHARED) | (thisp = NIL) | (useOsResources) THEN
    Goto NeueTabelle;
  END_IF;
   
   // wir sind im shared Mode !!
    
  pObj:=(Thisp-Sizeof(obj))$^obj;
  If pObj^.pClsHdr = NIL then
    Goto NeueTabelle;
  end_if;

   // Wenn Schon ein Objekt vorhanden, übernehmen wir die Methodenliste

  IF pObj^.pClsHdr^.nObjs >1 then
    pOrigChnl:=pObj^.pClsHdr^.pFirstObj+Sizeof(obj);
    pMeth:=pOrigChnl^.pMeth;
    
    nUserMeths := pCmd^.nCmds;
    nCmdSize   := nUserMeths$UINT*SIZEOF(pVoid);
    
    // read = read
    IF pMeth^.Read  = pCmd^.Read then 
      // write = write 
      IF pMeth^.Write = pCmd^.Write  then
        // nCmd = nCmd 
        IF pMeth^.nCmds = pCmd^.nCmds then
          IF _MEMCMP(#pCmd^.Init, #pMeth^.Init, nCmdSize) = 0 THEN
            RETURN;
          END_IF;
        END_IF;
      END_IF;
    END_IF;
  END_IF;

NeueTabelle:
	nUserMeths := pCmd^.nCmds;
	nCmdSize   := nUserMeths$UINT*SIZEOF(pVoid);

	nCmdSize += CmdMeth.Init;

	IF useOsResources THEN
		pMeth	:= OS_SysMalloc(nCmdSize)$^CmdMeth;	// read/write access

		IF pMeth = NIL THEN
			RETURN;								// alloc failed
		END_IF;
    pMeth^.pNxt := NIL;
    
		// Methodentabellen, die aus dem OS-Heap allokiert wurden, werden 
		// nicht in die Methodentabellen-Liste eingetragen.
	ELSE
  
    IF allocPermanent THEN
      pMeth	:= LDR_alloc_in_container(#methTabBlks, nCmdSize)$^CmdMeth;	// read/write access
      IF pMeth = NIL THEN
        RETURN;								// alloc failed
      END_IF;
      pMeth^.pNxt := pFirstCmd;
      pFirstCmd := pMeth;
    ELSE
      pMeth	:= _alloc_align(nCmdSize)$^CmdMeth;	// read/write access
      IF pMeth = NIL THEN
        RETURN;								// alloc failed
      END_IF;
      pMeth^.pNxt := pFirstCmd_ldr;
      pFirstCmd_ldr := pMeth;
    END_IF;
  END_IF;
  

	// if there are optional commands, then copy them
  pTemp := pMeth^.pNxt;
  _MEMCPY(pMeth, pCmd, nCmdSize);
  pMeth^.pNxt := pTemp;

END_FUNCTION

FUNCTION GLOBAL StoreCmd
VAR_INPUT
	pCmd            : ^CMDMETH;
	mode            : CMDMETHMODE;
END_VAR
VAR_OUTPUT
	pMeth           : ^CMDMETH;
END_VAR

  pMeth := StoreCmdEx(pCmd, mode, useOsResources := FALSE, allocPermanent:=TRUE);

END_FUNCTION

//    
//    Free's method table
//    

FUNCTION GLOBAL LOADER_FreeCmd
VAR_INPUT
	pCmd            : ^CMDMETH;
	useOsResources	: BOOL;
END_VAR
VAR
	pCmdWork		: ^CMDMETH;
	pCmdLast		: ^CMDMETH;
END_VAR

	IF useOsResources THEN
		// Methodentabellen, die aus dem OS-Heap allokiert wurden, sind 
		// nicht in der Methodentabellen-Liste eingetragen.
		OS_SysFree(pCmd);
		RETURN;
	END_IF;

	pCmdWork := pFirstCmd_ldr;
	pCmdLast := NIL;

	WHILE pCmdWork <> NIL DO
	 	
		IF pCmdWork = pCmd THEN
			IF pCmdLast = NIL THEN
				pFirstCmd_ldr := pCmdWork^.pNxt;
			ELSE
				pCmdLast^.pNxt := pCmdWork^.pNxt;
			END_IF;
			_free(pCmd);
			EXIT;
		END_IF;
		
		pCmdLast := pCmdWork;
		pCmdWork := pCmdWork^.pNxt;  // searchptr

	END_WHILE;

END_FUNCTION

(*============================================================================*)

VAR_GLOBAL
  methHashTab : HashTable;
END_VAR

(* Funktion, mit der ein Hashwert eines ClsHdr's gebildet wird *)
FUNCTION MethHash
VAR_INPUT
	Key : pVoid;  // CHMETH
END_VAR  
VAR_OUTPUT
  idx : UDINT;
END_VAR

  idx := 5381 + (Key$^CHMETH^.pRd)$UDINT;  
  idx := ((idx ROL 5) + idx) + (Key$^CHMETH^.pWr)$UDINT;  
  
  idx := idx AND 0x7FFFFFFF;

END_FUNCTION

(* Vergleichsfunktion für die ClsHdr Elemente in der Hashtabelle *)
#ifdef OTTOSPEED
FUNCTION MethCmp
VAR_INPUT
  pMeth : ^CHMETH;
  pMethKey : ^CHMETH;
  cookie : pVoid;
END_VAR  
VAR_OUTPUT
  result : DINT;
END_VAR
  
  IF (pMeth^.pRd = pMethKey^.pRd) & (pMeth^.pWr = pMethKey^.pWr) THEN
    result := 1;
  ELSE
    result := 0;
  END_IF;
  
END_FUNCTION
#else
FUNCTION MethCmp
VAR_INPUT
	Element : pVoid;
	Key : pVoid;
  cookie : pVoid;
END_VAR  
VAR_OUTPUT
  result : DINT;
END_VAR
VAR
  pMethKey : ^CHMETH;
  pMeth : ^CHMETH;
END_VAR

  pMeth	:= Element$^CHMETH;
  pMethKey := Key$^CHMETH;
  
  IF (pMeth^.pRd = pMethKey^.pRd) & (pMeth^.pWr = pMethKey^.pWr) THEN
    result := 1;
  ELSE
    result := 0;
  END_IF;

END_FUNCTION
#endif
FUNCTION GLOBAL MethTabsInit
  methHashTab := HashTable_Initialize(300, #MethHash(), #MethCmp());
  methTabBlks := NIL;
END_FUNCTION

FUNCTION GLOBAL MethTabsExit
  HashTable_Destroy(#methHashTab);
END_FUNCTION

(*============================================================================*)
//
//	Stores a method table for an ordinary server channel
//	Only read and write methods are available.
//
FUNCTION GLOBAL StoreMethod
VAR_INPUT
	mRd			: pFct;
	mWr			: pFct;
END_VAR
VAR_OUTPUT
	pMethod		: ^CHMETH;
END_VAR
VAR
  searchRes : pVoid;
	method: CHMETH;
END_VAR

  IF methHashTab THEN
    method.pRd := mRd;
    method.pWr := mWr;
    searchRes := HashTable_Find(#method, methHashTab, NIL);
    IF searchRes THEN
      pMethod := HashTable_RetrieveEl(searchRes)$^CHMETH;
      RETURN;
    END_IF;
  END_IF;

	// new method
	pMethod := LDR_alloc_in_container(#methTabBlks, SIZEOF(ChMeth))$^ChMeth;
	IF pMethod = NIL THEN
		RETURN;
	END_IF;
  
	pMethod^.pRd  := mRd;
	pMethod^.pWr  := mWr;
  pMethod^.pNxt := NIL; // Verkettung in einer linearen Liste wird nicht benötigt
  
  IF methHashTab THEN
    IF HashTable_Insert(pMethod(*Key*), NIL(*Value*), methHashTab, NIL) < 0 THEN
      // Das Einfügen ist nicht gut gegangen, die Hashtabelle wird daher zerstört, 
      // damit später keine falschen Suchergebnisse entstehen
      HashTable_Destroy(#methHashTab);    
      RETURN;
    END_IF;
  END_IF;

END_FUNCTION
//
//	DeriveCmdTable: Derive a Command Table Object from a Base Command Server
//
//	Input:
//		pCmdTable: 	A pointer to the command table to initialize
//		pBaseCmd:  	A pointer to the base class command table
//		nCmds:		The total number of derived class commad methods
//
//		This function is called to initialize the command table of a
//		derived class object from it's base class object.  The
//		function will copy ALL standard and user-custom methods from
//		the base class server into the derived class command table.
//
//	Notes:
//		1)	The derived class command table and user function array MUST
//			be the same length (or larger) as the base class.  As normal
//			with constructors, the user function array should be defined
//			as a local (stack) variable followed by the standard command
//			table array.
//		2)	The base class object MUST be constructed before this
//			function is called.
//
//
FUNCTION GLOBAL DeriveCmdTable
VAR_INPUT
	pCmdTable	: ^CMDMETH;
	pBaseCmd	: ^CMDMETH;
	nCmds		: UINT;
END_VAR
VAR
	ptc			: ^PVOID;
	ptcb		: ^PVOID;
	i			: UINT;
END_VAR

	// First, initialize the derived command table
	InitCmdTable (nCmd:= nCmds, pCmd:= pCmdTable);

	// Copy in Read/Write methods from base
	pCmdTable^.Read := pBaseCmd^.Read;
	pCmdTable^.Write := pBaseCmd^.Write;

	// Copy in rest of std and custom methods from base
	ptc := #pCmdTable^.Init;
	ptcb := #pBaseCmd^.Init;
	FOR i:= 0 TO pBaseCmd^.nCmds-1 DO
		ptc^ := ptcb^;
		ptc  += SIZEOF(pVoid);
		ptcb += SIZEOF(pVoid);
	END_FOR;

END_FUNCTION

(* 
   Diese Funktion wird aufgerufen, wenn jemand versucht über einen nicht verbundenen 
   Objektkanal eine Usermethode aufzurufen.
   Beim Aufruf dieser Funkton wird eine Trace-Meldung ausgegeben und eine Exception 
   ausgelöst, damit das Projekt kontrolliert angehalten wird.
*)
FUNCTION M_UNCONNECTED_CHANNEL_FUNCT
VAR_INPUT
#pragma warning(disable:73)
	in : UDINT;
#pragma warning(default:73)
END_VAR
VAR_OUTPUT
	retcode (EAX) : UDINT;
END_VAR

  TRACE("*** Error: unconnected object channel method called.");
  
  //Anm. praerw:
  // Habe keine Möglichkeit gefunden, wie man den Namen des Objekts ausgeben 
  // kann, von dem aus die Methode aufgerufen wird. Der Thispointer zeigt 
  // nämlich nicht auf das Objekt sondern auf den Client und von dort aus 
  // findet man nicht auf das umgebende Objekt.
  // So funktionierts nicht:
  //  VAR
  //  	pThis		: ^VirtualBase;
  //  END_VAR
	//  pThis$UDINT	:= ESI;
  //  TRACE0_ERR("Unconnected object channel method called '{0}'.", LSL_GetObjName(pThis));
  
  (0$^UDINT)^ := 0; // Exception auslösen
  retcode := ACCESS_DENIED;

END_FUNCTION

// Get pointer to base class VirtualBaseInit and check if object
// channels are to be connected to VBI-objects
//[<ret_code]	: C_OUTOF_NEAR when memory allocation fails
//
FUNCTION InitCallInit
VAR_OUTPUT
	ret_code		: ConfStates;
END_VAR
VAR
	CmdTable	: CmdMeth_256;
  nCmd_save	: DINT;
	ptc				: ^PVOID;
	anz				: DINT;
END_VAR

	ret_code			:= C_OK;

	// create standard method tables for data channels

	ESI:=0;
	sbCallInit.pDataMeth	:= StoreMethod(	mRd := #M_RD_DIRECT(),
						              		mWr := #M_NO_FUNCT());
	// for command servers
	InitCmdTable(nSTDCMD, #CmdTable$CmdMeth);    // RD,WR,Init,CyWork...

	ESI:=0;
	sbCallInit.pCmdMeth		:= StoreCmd(#CmdTable$CmdMeth, SHARED);

	// for unconnected client channels
	CmdTable.Read			:= #M_RD_CONST();	// predefined methods
	CmdTable.Write			:= #M_WR_CONST();

  (* praerw, DEVOS-967 [SA 52460] Aufruf unverbundener Objektkanäle unterbinden.
    Die Anzahl der Methoden in CmdTable wird während des Aufrufs von StoreCmd 
    vorübergehend vergrößert, damit Platz für Methoden geschaffen wird, die 
    im Falle eines Aufruf zu einem kontrollierten Absturz führen.
    Anschließend wird CmdTable.nCmds wieder auf den alten Wert gesetzt, 
    damit sich nach aussen hin nichts ändert (wg. Kompatibilität, man weiß ja nie 
    welche Codeteile sich darauf verlassen, dass die Anzahl der Methoden in 
    bCallInit.pCltMeth nSTDCMD ist).
    Die Größe der Methodentabelle wird auf 256 Einträge ausgelegt. Das benötigt 
    1k an zusätzlichem Speicher. Man könnte mit einer kompilzierteren Logik den 
    tatsächlichen Bedarf des Projekts ermitteln (in der Funktion InitCmdTable, 
    die vom Std-Konstruktor aufgerufen wird, den Maximalwert im Projekt ermitteln), 
    und dann sbCallInit.pCltMeth nochmals mit der richtigen Größe allokieren. 
    Das Problem dabei ist, dass es zu dem Zeitpunkt wo die richtige Größe bekannt ist, 
    bereits Referenzen auf die alte sbCallInit.pCltMeth gibt (z.B. InitCltChs speichert  
    sbCallInit.pCltMeth auf pCh^.pCmd für nicht verbundene Kanäle). 
    Diese Referenzen nachträglich zu korrigieren rechtfertigt den Aufwand nicht.
    Es wird der einfache Weg gegangen mit einer fixen Größe von 256 Einträgen.
  *)
  nCmd_save := CmdTable.nCmds;
  CmdTable.nCmds := 256; 

	ESI:=0;
	sbCallInit.pCltMeth		:= StoreCmd(#CmdTable$CmdMeth, SHARED);

	ptc := #sbCallInit.pCltMeth^.Init + nCmd_save * SIZEOF(pVoid);
  anz := CmdTable.nCmds - nCmd_save;
	WHILE anz DO
		ptc^ := #M_UNCONNECTED_CHANNEL_FUNCT();
		ptc  += SIZEOF(pVoid);
    anz  -= 1;
	END_WHILE;;
  
  CmdTable.nCmds := nCmd_save;
  
	IF 	sbCallInit.pDataMeth	= NIL |
		sbCallInit.pCmdMeth		  = NIL |
		sbCallInit.pCltMeth		  = NIL THEN
		ret_code	:= C_OUTOF_NEAR;
	END_IF;

END_FUNCTION
// The function copies the method table of an object to a local
// storage. There is no check if the provided memory buffer is
// big enough to store the method table.
// [>pCmd]	pointer to local buffer for method table
// [>pThis]	#object which provides the table, default is'this'
//
FUNCTION GLOBAL GetCmdTable
VAR_INPUT
	pCmd		: ^CmdMeth;
	pThis(ESI)	: ^SvrChCmd (*:=ESI*);
END_VAR
VAR
	nActCmds	: UINT;
END_VAR

	nActCmds	:= TO_UINT(pThis^.pMeth^.nCmds);

	_memcpy(pCmd$pVoid, pThis^.pMeth$pVoid, 
			sizeof(CmdMeth)+(nActCmds-nSTDCMD)*sizeof(pFct));

END_FUNCTION
//------------------------------------------------------------------------------
// OPS_SystemInit
//------------------------------------------------------------------------------
FUNCTION OPS_SystemInit

  // Die OPS Struktur mit 0 initialisieren mit Ausnahme der Elemente, die 
  // vom OS initialisiert werden
	_memset(#OPS$void, 0, OPSys.RtInterv_mSec);
  //OPS.RtInterv_mSec
  //OPS.RtInterv_uSec
	IF _RtOSversion >= 16#1000 THEN
  	_memset(#OPS.udProgLength$void, 0, OPSys.loaderFlags-OPSys.udProgLength);
  ELSE
  	_memset(#OPS.udProgLength$void, 0, OPSys.uiLoaderVersion-OPSys.udProgLength);
  END_IF;

	// from version 3.60 on, the OS is writing the correct times
	IF _RtOSversion <= 16#33c THEN
		OPS.RtInterv_mSec		:= IRQTime;
		OPS.RtInterv_uSec		:= IRQTime*1000;
	END_IF;

	OPS.uiLoaderVersion.usLoRev	:= LOADER_REV_LO;
	OPS.uiLoaderVersion.usHiRev	:= LOADER_REV_HI;

END_FUNCTION
//------------------------------------------------------------------------------
// OPS_ApplInit
//------------------------------------------------------------------------------
FUNCTION OPS_ApplInit

	OPS.tAbsolute	:= 0;			// absolute time since start in ms
	OPS.SysState	:= C_OK;		// LASAL RunTime states
	_memset(#OPS.CH16buf$void, 		// Channel16 to make CAN connect
	        0, 
			SIZEOF(OPS.CH16buf));
	OPS.CH16buf.CANID_REQCH := -1;
	OPS.udDescCRC	:= 0;			// CRC of the active project, calculated from
									// all the descriptor lists
	OPS.pSequDir	:= NIL;			// interpreter objects
	OPS.pProg		:= NIL;			// interpreter programs
	_memset(#OPS.ComChs$void, 		// ptrs to communication channels
	        0, 
			SIZEOF(OPS.ComChs));
	OPS.ConfigTime	:= 0;			// time for configuration
	_memset(#OPS.CanConnect$void, 	// used by Appli with MAE60-visu
	        0, 
			SIZEOF(OPS.CanConnect));
	OPS.OverRun		:= 0;			// counts overrun conditions
  
	//OPS.RtInterv_mSec :=
	//OPS.RtInterv_uSec :=


	// variables for exchanging IP-programs
	OPS.udProgLength	:= 0;		// total size of program
	OPS.udProgSize		:= 0;		// remaining size of program
	OPS.uiProgNoLoad	:= 0;		// index of currently loaded program
									// programs to load from temp buffer to regulary buffer
	OPS.uiProgsToLoad	:= 0;		// bitfield (nr-prog = bit)
  uiProgsToLoad[ 0 ] := 0;
  uiProgsToLoad[ 1 ] := 0;
	OPS.pDestProg		:= NIL;		// destination of currently loaded program

	//OPS.uiLoaderVersion.usLoRev :=
	//OPS.uiLoaderVersion.usHiRev :=

END_FUNCTION

FUNCTION GLOBAL MethTabsInit;

//------------------------------------------------------------------------------
// SystemInit
//
//	Parameter
//	  useOsResources
//		Dieses Flag ist gesetzt, wenn Betriebssystem-Resourcen anstatt von 
//		Applikations-Resourcen verwendet werden können. Applikations-Resourcen 
//		werden beim Reset automatisch freigegeben, Betriebssystem-Resourcen 
//		aber nicht.
//		Wenn dieses Flag gesetzt ist, dann kann man darauf vertrauen, daß 
//		SystemExit aufgerufen wird. In dieser Funktion müssen dann die 
//		Betriebssystem-Resourcen freigegeben werden.
//------------------------------------------------------------------------------
FUNCTION GLOBAL SystemInit
VAR_INPUT
	useOsResources	: BOOL;
END_VAR
VAR_OUTPUT
	ret_code 		: CONFSTATES;
END_VAR
   
	IF _RtOSversion >= 16#1000 THEN
    OPS.runstats.usSysInitCnt += 1;
  END_IF;

	IF (_RtOSversion >= 16#1000) & (OPS.loaderFlags AND LOADER_ISINITIALIZED) THEN
    TRACE("SystemInit: already initialized");//++pr
		ret_code := C_OK;
		RETURN;
	END_IF;

	pFirstCls       := NIL;
	pLastCls		    := NIL;
  clsHashTab      := NIL;
	pFirstCmd       := NIL;
	pFirstCmd_ldr   := NIL;
  //DS1747:Bisher wurde hier MethTabsInit aufgerufen, das ist aber 
  //nicht erlaubt, da MethTabsInit Speicher am Heap allokiert
  //(SystemInit initialisiert den Loaderteil, der auch nach dem Reset 
  //weiter läuft).
  methHashTab := NIL;
  methTabBlks := NIL;

	OPS_SystemInit();

#ifndef DUMMY_LOADER
	// das erste DebugIp Objekt anlegen
	OPS.ptPgBuff := IprMgr::GetIpObj(useOsResources);
	IF OPS.ptPgBuff = NIL THEN
    TRACE_ERR("SystemInit: GetIpObj failed");
    LSLCLI_LoaderTracePrint("SystemInit: GetIpObj failed", 12);
		ret_code := C_OUTOF_NEAR;
		RETURN;
	END_IF;
#endif  

	IF _RtOSversion >= 16#1000 THEN
	  OPS.loaderFlags := OPS.loaderFlags OR LOADER_ISINITIALIZED;
  END_IF;      

  TRACE("SystemInit: OK");
	ret_code := C_OK;

END_FUNCTION
//------------------------------------------------------------------------------
// SystemExit
//------------------------------------------------------------------------------
FUNCTION GLOBAL SystemExit
VAR
#ifndef DUMMY_LOADER
	pIp			: ^DebugIp;
#endif  
	dummy		: UINT;
END_VAR

	IF _RtOSversion >= 16#1000 THEN
    OPS.runstats.usSysExitCnt += 1;
    
    IF !!(OPS.loaderFlags AND LOADER_ISINITIALIZED) THEN
      RETURN;
    END_IF;
    
    dummy := LOADER_ISINITIALIZED;	// weil der Compiler NOT LOADER_ISINITIALIZED nicht versteht
    OPS.loaderFlags := OPS.loaderFlags AND NOT dummy;
  END_IF;      

	IF OPS.ptPgBuff THEN
#ifndef DUMMY_LOADER
		pIp	:= OPS.ptPgBuff;
		pIp^.ReleaseAllCommPorts();
#endif    
		OPS.ptPgBuff := NIL;
	END_IF;

END_FUNCTION


//    
//    Initialize LASAL RealTime-DataBase
//    ==================================
//    
//    time:    starting intervall in msec  1=1 msec
//    
//
FUNCTION GLOBAL ApplInit
VAR_OUTPUT
	ret_code 		: CONFSTATES;
END_VAR
VAR
  udTime      : UDINT;
	pCls        : ^ClsHdr;
#ifndef DUMMY_LOADER
	pPrjRd			: ^ProjectReader;
#endif  
  dummy : UINT;
  skipLoad : BOOL;
  startMMThread : BOOL;
END_VAR

	IF _RtOSversion >= 16#1000 THEN
    // Während der Initphase wird das Flag LOADER_ISAPPLVALID zurückgesetzt, 
    // weil die RTDB inkonsistent ist. Erst wenn alles initialisiert worden ist 
    // wird das Flag wieder gesetzt.
		IF _RtOSversion >= 16#1160 THEN		// 1.1.96
      dummy := LOADER_ISAPPLVALID;	// weil der Compiler NOT LOADER_ISAPPLVALID nicht versteht
		ELSE
			// Das LOADER_ISINITIALIZED Flag wird erst ab OS 1.1.96 vom OS zurückgesetzt 
			// (wenn z.B. ein neuer Loader geladen wird). Bei früheren OS Versionen 
			// muß dieses Flag daher hier vom Loader zurückgesetzt werden.
      dummy := LOADER_ISAPPLVALID OR LOADER_ISINITIALIZED;
		END_IF;
    OPS.loaderFlags := OPS.loaderFlags AND NOT dummy;
  	OPS.runstats.usApplInitCnt += 1;
  END_IF;      
	pFirstCls       := NIL;
	pLastCls		:= NIL;
  IF LDR_GetDescrBlocksFlag() THEN
    // Wenn die Deskriptoren in ein File ausgelagert werden, dann darf die Hashtabelle nicht 
    // verwendet werden, weil in diesem Fall Code in der Initphase übersprungen wird und 
    // die Hashtabelle dann nicht richtig erstellt wird (die LDR_ClsHashTable_Insert fehlen).
    clsHashTab := NIL;
  ELSE
    clsHashTab := LDR_ClsHashTable_Initialize();
  END_IF;
	pFirstCmd       := NIL;
  MethTabsInit();
  ResetPostInitCB();
  LDR_InitLookup();
  
#ifdef LDR_RETSVR
  usHasRetentive := 0;
  usCheckRetentive := 0;
  usReportRetentive := 0;
#endif
  bWait4Visu := FALSE; //ZP000

#ifndef DUMMY_LOADER
  TaskReader::InitModule();  // Initialisierung des Moduls TaskReader
#endif  
	MEM_Init();		// Initialierung des Moduls Mem.st
	IF (_RtOSversion < 16#1000) | !!(OPS.loaderFlags AND LOADER_ISINITIALIZED) THEN
  
    if (!!(OPS.loaderFlags AND LOADER_ISINITIALIZED)) then
      ret_code := SystemInit(TRUE);
    else
      ret_code := SystemInit(FALSE);
    end_if;
    
		IF ret_code <> C_OK THEN
			RETURN;
		END_IF;
    
	END_IF;
	OPS_ApplInit();
  

	Loader_time1 := ops.tabsolute;

	udTime					:= Ops.RtInterv_mSec;

	//~menk
	OS_LNK_LookupEmbedded	:= #_LookUpEmbed_C();
	OS_LNK_PrepareLink		:= #_PrepareLink();
	OS_LNK_ApplyLink		:= #_ApplyLink();
	IF _RtOSversion >= 16#32F THEN
		OS_LNK_GetNextChannel	:= #_GetNextRAMch();
		OS_LNK_GetObjectName	:= #_GetObjectName();
	END_IF;
  TRACE("Loading project...");

#ifndef DUMMY_LOADER
  IprMgr::InitModule();// Initialierung des Moduls IprMgr
  ProgIp::InitModule();// Initialierung des Moduls ProgIp
  DebugIp_Init();
#endif  

  (* Initialisieren der CRC Map (pMapper) anhand der aus einer CRC eines Objektnamens 
     eine Objektadresse ermittelt werden kann. *)
  LDR_InitCRCMap();
  
	// das S-RAM herrichten
	ret_code := S_RAM_INIT();
	if ret_code <> C_OK then
		ret_code := C_OUTOF_NEAR;
		RETURN;
    end_if;
	Loader_time2 := ops.tabsolute;
	#ifdef COMLINK_LASAL
    ComLinkInit();				// Init data for communication
	#else
	#pragma message ("old style of CAN-communication active")
	#endif // COMLINK_LASAL

	//====================================================================
	// CONFIGURATE
	//====================================================================
	Loader_time3 := ops.tabsolute;
#ifndef DUMMY_LOADER

  //DescrBlocks:
  //Alles was zwischen LDR_LoadDescrBlocks und LDR_CommitDescrBlocks allokiert wird, 
  //wird aufgesammelt und als Binärinformation gespeichert. Wenn sich bei einem späteren 
  //Start das Projekt nicht geändert hat und die Binärinformation vorliegt, dann kann 
  //der dazwischenliegende Code übersprungen werden.

  skipLoad := FALSE;
  IF LDR_LoadDescrBlocks(#pPrjRd, #pFirstCls, #pLastCls) THEN
    skipLoad := TRUE;
    Goto DescrBlocks_CheckPoint;
  END_IF;

  pPrjRd := LDR_alloc_descr(sizeof(ProjectReader)) $ ^ProjectReader;
	IF pPrjRd = 0 THEN
		ret_code := C_OUTOF_NEAR;
		RETURN;
  END_IF;
  _memset(pPrjRd, 0, sizeof(ProjectReader));

	IF pPrjRd^.CreateClasses() = FALSE THEN
    ret_code  := C_SYNTAX_ERROR;
		RETURN;
	END_IF;
  
 	Loader_time4 := ops.tabsolute;

  LDR_CommitDescrBlocks(pPrjRd, pFirstCls, pLastCls);
DescrBlocks_CheckPoint:  

 // hierher verschoben, da MMConnectionHandler::OnMultiMasterInit() schon die CmdTables braucht
	// mark VBI-channels, alloc standard method lists
	IF InitCallInit() <> C_OK THEN		// reset struct just once
		ret_code	:= C_OUTOF_NEAR;
		RETURN;
	END_IF;
  
  Loader_time5 := ops.tabsolute;
  
  startMMThread := MMConnectionHandler::OnMultiMasterInit();
  
  IF ((startMMThread = FALSE) & (pFirstCls = NIL)) THEN
    // Projekt ohne Klassen und Objekte geladen
    LDR_FreeSramKopie();
    RETURN;     
  END_IF;

	IF pPrjRd^.InitOsiClasses() = FALSE THEN
    ret_code  := C_SYNTAX_ERROR;
		RETURN;
	END_IF;
#endif  
 	// ===============================================================================
 	//   test ob C-IPC wenn ja muß die Klasse RAM eine gewisse Version aufweisen
	// ===============================================================================
	
	if (_whoami = 32) & LDR_HasSram() then
	 if (_S_RAM_Hptr^.DataValid SHR 16) > 1 then
      pCls := _LookupCls("RAM");
	  if pCls <> NIL then
	   if pCls^.pDsc^.udRev$UDINT < 16#00010001 then
	    TRACE_WARN("CLASS RAM must have a version of minimum 1.1 --> Please import new CLASS RAM");
	    LSLCLI_LoaderTracePrint("CLASS RAM must have a version of minimum 1.1 --> Please import new CLASS RAM", 12);
      ret_code:=C_INCOMPATIBLE;
	    return;
       end_if;
	  end_if; 	
      pCls := _LookupCls("RAMEX"); // Silber 220304
	  if pCls <> NIL then
	   if pCls^.pDsc^.udRev$UDINT < 16#00010002 then
	    TRACE_WARN("CLASS RAMex must have a version of minimum 1.2 --> Please import new CLASS RAM");
	    LSLCLI_LoaderTracePrint("CLASS RAMex must have a version of minimum 1.2 --> Please import new CLASS RAM", 12);
      ret_code:=C_INCOMPATIBLE;
	    return;
       end_if;
	  end_if; 	
     end_if;
	end_if;
  
	// ONs anlegen
#ifndef DUMMY_LOADER

	IF pPrjRd^.CreateONs() = FALSE THEN
    ret_code  := C_SYNTAX_ERROR;
		RETURN;
	END_IF;
  
  MethTabsExit();
  HashTable_Destroy(#clsHashTab);
   
  // load IPR - progams
  IF pPrjRd^.LoadIprProgs() = FALSE THEN
    ret_code  := C_NO_FILE_OPEN;
    RETURN;
  END_IF;
  
	Loader_time6 := ops.tabsolute;
  
  RamExCleanUp();
  
	IF _RtOSversion >= 16#1000 THEN
  	OPS.loaderFlags := OPS.loaderFlags OR LOADER_ISAPPLVALID;
  END_IF;
	_firstscan := 0;
    
  IF LDR_outOfSramMem THEN
	    TRACE_ERR("Out of SRAM memory");
	    LSLCLI_LoaderTracePrint("Out of SRAM memory", 12);
		ret_code	:= C_OUTOF_NEAR;
		RETURN;
  END_IF;

  // DB1822:
  // Nach dem Aufruf der Kontruktoren und vor den Init-Methoden werden die Daten 
  // fürs Sram auf die Disk geschrieben (nur Sramformat 2), damit neu angelegte 
  // Sram-Objekte im Falle einer Exception oder eines Resets in der Init-Methode 
  // nicht verloren gehen.
  SramSaveFile();
  LDR_FreeSramKopie();


  // Berechnung der Descriptor CRC:
  // Muss vor pPrjRd^.Initialize() aufgerufen werden, weil in pPrjRd^.Initialize() 
  // gewartet wird, bis alle required MM Server initialisiert sind. 
  // Während dem Warten können aber von anderen MM Stationen Anfragen daherkommen, 
  // dabei wird auch die Descriptor CRC abgefragt.
  _CalcDescCRC();

  // Start MM THreads:
  // Muss vor pPrjRd^.Initialize() aufgerufen werden, weil in pPrjRd^.Initialize() 
  // gewartet wird, bis alle required MM Server initialisiert sind. 
  // Muss nach dem Anlegen der Projekt-Struktur (Klassen und Objektdeskriptoren) 
  // aufgerufen werden, weil beim LOGIN Speicher allokiert wird und sich dadurch 
  // Objektadressen ändern können, was die Descriptor-CRC beeinflusst.
  IF  startMMThread = TRUE THEN
    MMConnectionHandler::OnMultiMasterStartThreads();
  END_IF;

  Loader_time7 := ops.tabsolute;
	pPrjRd^.Initialize();				// call Init - meths of all objects	

  // call post init callback if available
  CheckPostInitCB();

	IF pPrjRd^.CheckIprProgs() = FALSE THEN
		RETURN;
	END_IF;
#endif  
	Loader_time8 := ops.tabsolute;

	// nochmal falls im init ram oder ramex angelegt wurden (mit AddObj)
	// 02.08.2004 Âschl
	SramSaveFile();					// daten für die Sram auf die Disk
	GET_USED_S_RAM(); // UsedData im _s_ram_hptr aktualisieren

 
	//====================================================================
	// LASAL RTDB is now completly configured, return RunRAM
	//====================================================================

	OPS.ConfigTime	:= OPS.tAbsolute;

	ret_code		:= (C_RUNRAM $ INT + (_RunStatus AND 1)) $ ConfStates;
  
	//Wenn Interpreter vorhanden sind, dann wird eine Zeit gewartet um die 
	//Möglichkeit zu schaffen von Lasal aus Breakpoints für die Interpreter zu setzten
	IF OPS.pSequDir^.uiIprCnt <> 0 &
		_OnlineMap$UDINT <> 0 THEN

		g_LoadTimeout := ops.tAbsolute + 500;
		OPS.SysState := C_RUNRAM;
		WHILE (g_LoadTimeout - ops.tAbsolute)$DINT > 0 DO
			OpCyclic();
			OS_SSR_Delay(10);
		END_WHILE;
	END_IF;
	Loader_time9 := ops.tabsolute;
  
  //ZP000
  if (bWait4Visu = TRUE) then

    udTime := ops.tabsolute;
		OPS.SysState := C_RUNRAM;
		while (bWait4Visu = TRUE) do
			OpCyclic();
			OS_SSR_Delay(1);
      if ((wait4Visu_timeout_ms > 0) & ((ops.tabsolute - udTime) > wait4Visu_timeout_ms)) then
        exit;
      end_if;
		end_while;

  end_if;
  
#ifndef DUMMY_LOADER
  LDR_free_descrSave(pPrjRd);
#endif  

  // Sicherstellen, dass die CRCMap sortiert ist, damit im zyklischen Betrieb kein Sortiervorgang erforderlich ist
  LDR_SortCRCMapIfNececcary();
  
	Loader_time10 := ops.tabsolute;
  //ZP000
  
  TRACE("Project successfully loaded");

END_FUNCTION
