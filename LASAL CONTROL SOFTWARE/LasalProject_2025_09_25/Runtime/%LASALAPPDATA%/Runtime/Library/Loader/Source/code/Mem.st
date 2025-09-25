#include <LSL_St_IfSSR.h>
#include <LSL_ST_LSLFILE.H> // RTKernel V3.18
//#include <RTOS_Header.h>
//#include <RTOS_Variables.h>
//#include "RTOS_MemIntern.h"
#include ".\include\HashTab.h"

//#include <.\c_Memory.h>

#include "RTOS_LdHeap.h"
#include <LSL_ST_OSFILE.H>
#include <LSL_ST_SYSSERNUM.H>
#include <LSL_ST_IFLINKER.H>
#include <LSL_ST_SYSENV.H>
#include <LSL_ST_MT.H>
#include <LSL_ST_SRAMDISK.H>
#include <LSL_StiTask.h>
#include ".\RTOS_PrivHeader.h"

#pragma using RetSvrFile

#define arraysize(p)                (sizeof(p)/sizeof(p[0]))
#define MEMORY_KENNUNG_LDR	15
#define MEMORY_KENNUNG_PRG	13

// Define the following constants to write debug messages to event19.log
#define DEBUG
//#undef DEBUG
//#define DEBUG_VERBOSE
#undef DEBUG_VERBOSE

#ifdef DEBUG
#include <LSL_ST_SYSMSG.h>
#define DEBUGLOG0(txt)    	        IF OS_pLslSysMsg & hLog0 THEN OS_SYSMSG_LPRINTFLN1(hLog0,1,txt,ops.tAbsolute);OS_SYSMSG_LFLUSH(hLog0); END_IF;
#define DEBUGLOG1(txt,p1)           IF OS_pLslSysMsg & hLog0 THEN OS_SYSMSG_LPRINTFLN2(hLog0,1,txt,ops.tAbsolute,p1);OS_SYSMSG_LFLUSH(hLog0); END_IF;
#define DEBUGLOG2(txt,p1,p2)        IF OS_pLslSysMsg & hLog0 THEN OS_SYSMSG_LPRINTFLN3(hLog0,1,txt,ops.tAbsolute,p1,p2);OS_SYSMSG_LFLUSH(hLog0); END_IF;
#define DEBUGLOG3(txt,p1,p2,p3)     IF OS_pLslSysMsg & hLog0 THEN OS_SYSMSG_LPRINTFLN4(hLog0,1,txt,ops.tAbsolute,p1,p2,p3);OS_SYSMSG_LFLUSH(hLog0); END_IF;
#else
#define DEBUGLOG0(txt)    	        
#define DEBUGLOG1(txt,p1)           
#define DEBUGLOG2(txt,p1,p2)        
#define DEBUGLOG3(txt,p1,p2,p3)     
#endif

#define SRAM_KOPIE_PFAD				"C:\LSLDATA"
#define SRAM_KOPIE_SRAM_FNAME		"C:\LSLDATA\SRAM.CPY"
#define SRAM_KOPIE_RAMFILE_FNAME	"C:\LSLDATA\RAMFILE.CPY"
#define SRAM_INIT_SRAM_FNAME    "C:\LSLDATA\SRAM.INI"
#define SRAM_INIT_RAMFILE_FNAME	"C:\LSLDATA\RAMFILE.INI"
#define SRAM_INIT_SRAM_FNAME_DONE    "C:\LSLDATA\SRAM.DON"
#define SRAM_INIT_RAMFILE_FNAME_DONE	"C:\LSLDATA\RAMFILE.DON"

#define X_FILE_CELLS_2	131072	  // belegt genau die 512 kByte S-RAM	

// Die Grösse des Speichers für die SRAM-Kopie ist immer ein Vielfaches des folgenden Wertes
#define SRAM_KOPIE_BLOCKSIZE		16#4000

#define DESCR_BLOCKS_PFAD		  "C:\LSLDATA"
#define DESCR_BLOCKS_FNAME		"C:\LSLDATA\DSCBLKS.BIN"

#define DESCR_BLOCKS_FILE_VERSION 1

#ifdef LDR_RETSVR
#define LDR_RETENTIVE_SVRFILENAME   "c:\retsvr.dat"
#define LDR_RETENTIVE_SVRFILENAME_ERR "c:\retsvr.err"
#endif

#define RAMEXFILES      "C:\lsldata\ramex\"
#define RAMEXINDEXFILE  "C:\lsldata\ramex.idx"

// Error Codes
#define EC_LOADFILE_ERR           1
#define EC_NOT_VALID              2
#define EC_INV_BLK0_CHKSUM        3
#define EC_OUT_OF_SRAM            5
#define EC_INV_BLK0_CHKSUM2       11
#define EC_INV_BLK_ENTRIES        100
#define EC_INV_XRAMCELL_CHKSUM    100
#define EC_INV_BLK_CHKSUM         101
#define EC_INV_CELL_CHKSUM        102
#define EC_INV_BLK_CHAIN          103
#define EC_INV_V1_CELL            200
#define EC_UNSUPPORTED_FORMAT_V1  201
#define EC_SRAMDISK_NOT_WRITTEN_AT_POWER_DOWN   202

VAR_GLOBAL
	loaderRamexError 	: UDINT;		// wenn ungleich 1, dann heisst das, daß ein Fehler im SRAM festgestellt wurde
	pSramAlarmSvrCh		: ^SVRCH;
	pRamFile			: ^XFILEHDR;
	isRamfileDatUsed	: BOOL;
	sramErrorCode	: UDINT;
	fileCellChkIdx	: UDINT;
  descrBlocksActive : BOOL;
  _osinstanceId       AT % M 0104     : UDINT;
  LDR_outOfSramMem : UDINT; // TRUE, wenn kein Platz mehr für eine RAM- oder RAMEx- Zelle vorhanden ist
  LDR_allocated_XFileCells : UDINT;
  LDR_allocated_XFileSize : UDINT;
END_VAR

#define CST_SRAM_BLOCKSIZE     16		 // alte version 16 Byte
#define CST_SRAM_BLOCKSIZE_NEW  4		 // neue Version 4 Byte

TYPE
	 MAT_INFO_TYPE   : STRUCT // Memory Allocation Table Info Header
		status         : UDINT;  
		memlength      : UDINT;	
		matsize        : UDINT;
		blocksize	   : UDINT;	
		mat            : ARRAY[0..0] of USINT;	// nur Platzhalter, MAT wird allokiert
	 END_STRUCT;

#pragma pack (push,1)
	XFILEHDR_HDR     : STRUCT
		udEntries  : UDINT;	// anzahl
		uiChange	: UINT;		// change flag	
		CRC32		: UDINT;    // CRC über gesamtes FILE
	END_STRUCT;
#pragma pack (pop)

  DescrBlock : STRUCT
    blockAddr     : pVoid;
    allocatedSize : UDINT;  // aktuell allokierte Grösse 
    originalSize  : UDINT;  // Grösse, mit der der Block das erste mal angelegt wurde
    usedSize      : UDINT;  // benutzte Grösse
    pNext         : ^DescrBlock;
    dummyData     : USINT;  // Dummy Variable wg. variabler Länge der Daten
  END_STRUCT;  

  DescrBlocksFileHdr : STRUCT
    dataValid   : UDINT;
    version     : UDINT;
    prjCrc      : UDINT;
    osVer       : UDINT;
    userHeapStartAddr  : UDINT;	// start address of UserHeapMemory
    userHeapTotalSize  : UDINT;	// total size of UserHeapMemory
    checkptFreeMem     : UDINT;	// freier Speicher zu einem best. Zeitpunkt
    nbrOfBlocks : UDINT;
    pPrjRd			: pVoid;
    pFirstCls		: pVoid;
    pLastCls		: pVoid;
  END_STRUCT;  

END_TYPE


VAR_PRIVATE
    pLdrmem 	: ^MAT_INFO_TYPE;
	maxSramSize	: UDINT;
#ifdef DEBUG
	OS_pLslSysMsg : ^OS_LSLSYSMSG;
	LogBuf0       : ARRAY[0..2047] OF CHAR;
	hLog0         : UDINT;
#endif
	fUseMallocV1 : BOOL;
	fUseDescrBlocks : BOOL;
  // Wenn das fReorgSram Flag gesetzt ist, dann heisst das folgendes:
  // 1. Find_sRamFile und Find_sRam suchen ihre Zelle vorerst nicht im SRAM sondern 
  //    in der Kopie. Wenn sie gefunden wurde, dann wird sie von der Kopie ins SRAM 
  //    kopiert.
  // 2. GET_USED_S_RAM setzt das dataValid Flag nicht auf TRUE (GET_USED_S_RAM wird 
  //    immer dann aufgerufen, wenn Speicher im SRAM allokiert worden ist, damit der 
  //    usedData Wert aktualisiert wird). Das dataValid Flag darf nämlich in der 
  //    Reorg Phase nicht auf TRUE gesetzt werden, das wird erst am Ende gemacht.
  // 3. Wenn alles vorbei ist (LDR_FreeSramKopie), dann wird fReorgSram zurückgesetzt 
  //    und GET_USED_S_RAM aufgerufen, damit das usedData und dataValid aktualisiert wird.
	fReorgSram : UDINT;
	fSramInitCompleted : UDINT;
	_S_RAM_Hptr_Kopie : ^MRAM_DESCR;
	pRamFile_Kopie			: ^XFILEHDR;
	nbrOfCells_notFound : UDINT;
	nbrOfCells_foundInSram : UDINT;
	nbrOfCells_foundInCopy : UDINT;
  removeableMediaChanged : DINT;  // -1 = info nicht vorhanden, 0 = kein Wechsel, 1 = Wechsel
	pISysSernum : ^LSL_ISYSSERNUM;
  pDescrBlockList : ^DescrBlock;  // verkettete Liste von Descriptor-Blöcken
//  checkptFreeMem : UDINT;

#ifdef LDR_RETSVR
  _S_RAM_Hptr_File : ^MRAM_DESCR;
  _S_RAM_Hptr_File_New : ^MRAM_DESCR;
  hLDR_remSvr : dint;
#endif
#ifndef DUMMY_LOADER
  mt_api          : ^LSL_MT_TYPE; // multitask interface (needed for semaphores)
#endif  
  hMemMutex : MT_SEMAHANDLE;
  pFirstAsyncState : ^RetSvrFile::ASYNC_FILE_STATE; // erstes Element in der verketteten Liste
  pLastAsyncState : ^RetSvrFile::ASYNC_FILE_STATE; // letztes Element in der verketteten Liste
  bSramContinueOnError : BOOL;
  isSramNewOrCleared : BOOL;
  AsnycFileOperationInProgress : BOOL;
  hasSram : BOOL;
END_VAR

var_global
  checkptFreeMem : UDINT;
  processAsyncFileError : DINT; // Fehlercode der asynchronen RetSrvFile Objekte
  nAsyncStateObjs : UDINT; // Diagnose: Anzahl der Elemente in der AsyncState Liste
  nAsyncStateReqs : UDINT; // Diagnose: Anzahl der AsyncState Requests die sich in der OS Queue befinden
#ifdef TEST_RETSVRFILE  // Code kann für Testzwecke aktiviert werden
  simuFSeekA_error : DINT; // Test: Simulation eines Fehlers beim asynchronen seek
  simuFWriteA_error : DINT; // Test: Simulation eines Fehlers beim asynchronen schreiben
  simuFReq_error : DINT; // Test: Simulation eines Fehlers des asynchronen Requests
#endif
end_var

FUNCTION RoundUpPow2
VAR_INPUT
	value	: UDINT;
END_VAR
VAR_OUTPUT
	result : UDINT;
END_VAR;
FUNCTION SramCheckerInit;
FUNCTION SramError;
FUNCTION GetRamfileDatUsedFlag
VAR_INPUT
  pFormatChanged : ^BOOL;
END_VAR
VAR_OUTPUT
	retVal : BOOL;
END_VAR;
FUNCTION GetSramContinueOnErrorFlag
VAR_OUTPUT
  retVal : BOOL;
END_VAR;
FUNCTION GetSramNewOrClearedFlag
VAR_OUTPUT
  retVal : BOOL;
END_VAR;
FUNCTION GLOBAL LDR_GetDescrBlocksFlag
VAR_OUTPUT
	retVal : BOOL;
END_VAR;


FUNCTION GLOBAL LDR_DebugLog0
VAR_INPUT
  fmt : ^CHAR;
END_VAR
  DEBUGLOG0(fmt);
END_FUNCTION

#ifdef LDR_RETSVR
FUNCTION GLOBAL InitRetainSvrFile
	VAR_INPUT
		usProbe 	: usint;
	END_VAR
VAR_OUTPUT
		ok 	: DINT;
END_VAR;
#endif

(*
  Korrigiert den Wert der Variablen _s_ram_hptr.Datalength so, dass damit 
  genau die Datenmenge angegeben wird, die mit der MAT adressiert werden 
  kann. Diese Korrektur ist notwendig, weil Lasal-Class den Wert ausliest 
  und daraus die beim Project-Info angezeigten Werte 'Max Size' und 'Free Size' 
  berechnet.
  Grund, warum mit der MAT nicht immer der ganze Speicher adressiert werden kann: 
  In der Mat wird mit einem Bit ein belegter Block gekennzeichnet (0=belegt). 
  Pro MAT-Byte kann man einen Speicher der Größe 8*Blocksize angeben. 
  Wenn der mit _s_ram_hptr.Datalength angegebene Speicher aber nicht durch 
  8*Blocksize teilbar ist, dann ist der Rest von der MAT nicht benutzbar.
*)
FUNCTION CorrectSramDatalength
VAR_INPUT
  blockSize : UDINT;
END_VAR
VAR
	memPerMatByte : UDINT;
END_VAR

  memPerMatByte := blockSize * 8;
  _s_ram_hptr^.Datalength := (_s_ram_hptr^.Datalength / memPerMatByte) * memPerMatByte;

END_FUNCTION

//------------------------------------------------------------------------------
// MEM_Init
//
// 	Initialisierungsfunktion für dieses Modul (Mem.st)
//	Darf nur einmal beim Loaderstart aufgerufen werden.
//
// pr 29.7.2004 - neu
//------------------------------------------------------------------------------
FUNCTION GLOBAL MEM_Init
VAR
  formatChanged : BOOL;
END_VAR


	pLdrmem := NIL;
  pRamFile := NIL;
  AsnycFileOperationInProgress := FALSE;
  
	IF (_whoami = DESTPLC_IPC_TEXT) |
	   (_whoami = DESTPLC_IPC_800) 	|
	   (_whoami = DESTPLC_IPC_1024) |
	   (_whoami = DESTPLC_IPC_320) 	THEN
	   	// Der IPC hat zwar ein (virtuelles)SRAM von 1MB, es wurde aber bei 
		// früheren Loaderversionen (vor 29.7.2004) wegen einer fixen MAT-Size 
		// auf 256k begrenzt.
		// Diese Begrenzung könnte jetzt wegen einer variablen MAT-Size fallen, 
		// wird aber vorläufig noch beibehalten, da ein größeres SRAM auch neg. 
		// Auswirkungen haben kann (z.B. auf die Laufzeit).
	   	maxSramSize := 256 * 1024;
	ELSE
		// bei allen anderen Plattformen wird die Größe des SRAMs vom 
		// Betriebssystem unverändert übernommen.
		maxSramSize := 16#7FFFFFFF;	// kein Limit
	END_IF;

	loaderRamexError  := 0;
	pSramAlarmSvrCh := NIL;

	IF (_RtOSversion > 16#1119) & // 01.01.025
	   (_LSL_POS^.piSSR^.SSR_MallocV1  <> NIL) &
	   (_LSL_POS^.piSSR^.SSR_ReallocV1 <> NIL) THEN
		fUseMallocV1 := TRUE;
	ELSE
		fUseMallocV1 := FALSE;
	END_IF;

  IF(_s_ram_hptr = NIL) | 
    (_s_ram_hptr^.DataStart = 0) |
    (_s_ram_hptr^.DataLength = 0) THEN
    hasSram := FALSE;
  ELSE
    hasSram := TRUE;
  END_IF;

	fReorgSram := 0;
	_S_RAM_Hptr_Kopie := NIL;
	pRamFile_Kopie := NIL;
	fSramInitCompleted := 0;
	nbrOfCells_notFound := 0;
	nbrOfCells_foundInSram := 0;
	nbrOfCells_foundInCopy := 0;
  removeableMediaChanged := -9999;
  
#ifdef LDR_RETSVR
  _S_RAM_Hptr_File := NIL;
  _S_RAM_Hptr_File_New := NIL;
  hLDR_remSvr := 0;
#endif

  fUseDescrBlocks := LDR_GetDescrBlocksFlag();
  pDescrBlockList := NIL;
  descrBlocksActive := FALSE;

#ifdef DEBUG
    OS_CILGet("SYSMSG", #OS_pLslSysMsg$void);
	IF OS_pLslSysMsg THEN
    	// write to C:\SYSMSG\EVENT19.LOG
    	hLog0 := OS_SYSMSG_LCREATE(9, #LogBuf0[0], arraysize(LogBuf0), 100000, 0);
	END_IF;
#endif
	DEBUGLOG3("%010d-Loader V%02d.%02d.%03d", LOADER_REV_HI SHR 4, LOADER_REV_HI AND 16#0F, LOADER_REV_LO);

  OS_CILGet("ISYSSERNUM", #pISysSernum$void);

	isRamfileDatUsed := GetRamfileDatUsedFlag(#formatChanged);
  IF isRamfileDatUsed THEN
    // Wert von _s_ram_hptr.Datalength so korrigieren, dass damit die allokierbare Sramgröße angezeigt wird.
    // Anm.: Beim Format 1 wird nichts korrigiert, weil hier der Wert von Free-Mem sowieso nur 
    // ein Richtwert dafür ist, wie viele RAM oder RAMEX Objekte noch angelegt werden können, 
    // weil es hier zusätzlich zur MAT noch eine Verwaltungsstruktur gibt (xRamHdr = Block mit 500 RAM-Zellen),
    // in der auch RAM-Zellen allokiert werden.
    CorrectSramDatalength(CST_SRAM_BLOCKSIZE_NEW);
  END_IF;
  bSramContinueOnError := GetSramContinueOnErrorFlag();
  isSramNewOrCleared := GetSramNewOrClearedFlag();

	SramCheckerInit();
    LDR_outOfSramMem := FALSE;
  
  IF OS_CILGet("LSL_MULTITASK", #mt_api$void) THEN
    TRACE_ERR("ERROR: missing LSL_MULTITASK interface");
    mt_api := NIL;
  END_IF;
  hMemMutex := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                     1,    // initvalue
                                     0,    // flags: 0 means create unconitionally
                                     "MEM_MUTEX"
                                     );
  pFirstAsyncState := NIL;                                     
  pLastAsyncState := NIL;                                     
  processAsyncFileError := 0;
  
  nAsyncStateObjs := 0;
  nAsyncStateReqs := 0;

#ifdef TEST_RETSVRFILE  // Code kann für Testzwecke aktiviert werden
  simuFSeekA_error := 0;
  simuFWriteA_error := 0;
  simuFReq_error := 0;
#endif

  LDR_allocated_XFileCells := 0;
  LDR_allocated_XFileSize := 0;

END_FUNCTION


(* Der Wert für MAX_FILE_WRITE_BLK_SIZE richtet sich nach der Größe des Srams, das 
   im OS für SecuStorage verwendet wird (beim CIPC sinds 64k). Da der Wert nicht vom 
   OS abgefragt werden kann, wird er hier hardcodiert.
*)
#define MAX_FILE_WRITE_BLK_SIZE   (32*1024)
(* 
   Schreibt Daten ins File, wobei beim FileWrite Aufruf eine Obergrenze des Datenblocks 
   nicht überschritten wird, um einen -150-Fehler (Sram zu klein) zu vermeiden. 
   
   Anm.: 
   Im OS wird das Schreiben bereits so geblockt werden, dass kein -150 Fehler auftreten 
   dürfte. Ich habe aber trotzdem solche Fehler beobachtet. Daher wird im Loader nur 
   blockweise geschrieben, um von der OS Implementierung unabhängig zu sein.
*)
FUNCTION FileWrite
VAR_INPUT
  handle : DINT;
  buffer : pvoid;
  buflen : UDINT;
END_VAR
VAR_OUTPUT
  retval : DINT;
END_VAR
VAR
  rc : DINT;
  nWritten : UDINT;
  blksize : UDINT;
END_VAR;

  nWritten := 0;
  
  WHILE buflen > 0 DO
  
    IF buflen > MAX_FILE_WRITE_BLK_SIZE THEN
      blksize := MAX_FILE_WRITE_BLK_SIZE;
    ELSE
      blksize := buflen;
    END_IF;
  
  	rc := OS_FILE_WRITEV1(handle, buffer + nWritten, blksize);
  	IF rc <> blksize$DINT THEN
      IF rc < 0 THEN
        retval := rc;
      ELSE
        retval := nWritten$DINT + rc;
      END_IF;
      RETURN;
    END_IF;
    
    nWritten += blksize;
    buflen -= blksize;
  
  END_WHILE;
  
  retval := nWritten$DINT;

END_FUNCTION


(* Macht das Ramfile.bak ungültig *)
FUNCTION InvalidateRamfileBak
VAR
	handle : DINT;
  filehdr : XFILEHDR_HDR;
END_VAR

  handle := OS_FILE_OPEN("c:\RamFile.bak",0,0);	
  if handle < 0 then
    // file existiert nicht, daher muss auch nichts ungültig gemacht werden
    return;
  end_if;
  OS_FILE_CLOSE(handle);

  handle := OS_FILE_OPEN("c:\RamFile.bak",2,ATT_CREATE);	
  if handle >= 0 then
    filehdr.udEntries := 0;
    filehdr.uiChange := 0;
    filehdr.CRC32 := 0;
    FileWrite(handle, #filehdr, sizeof(filehdr));
    OS_FILE_CLOSE(handle);
  else
    DEBUGLOG1("%010d-SramSaveFile: could not open ramfile.bak, rc=%d", handle$UDINT);
  end_if;

END_FUNCTION


FUNCTION WasRemoveableMediaChanged
VAR_OUTPUT
  changed : BOOL;
END_VAR
VAR
  serNumLen : UDINT;
  serNumPLC : ARRAY[0..49] OF USINT;
  serNumFILE : ARRAY[0..49] OF USINT;
	handle : DINT;
	rc     : UDINT;
END_VAR

  IF removeableMediaChanged < -1 THEN
    // Funktion wurde das 1.Mal aufgerufen
    removeableMediaChanged := -1;

    IF pISysSernum <> NIL & _rtosversion > 16#537 & LSLFILE_IsInstalled THEN
      serNumLen$DINT := ISYSSERNUM_SERNUMGETPLC(#serNumPLC[0], sizeof(serNumPLC));
      IF serNumLen$dint > 0 THEN
        handle := OS_FILE_OPEN("C:\LSLDATA\SERNUM.DAT", 0, 0);		// 0 = read only
        IF handle >= 0 THEN
          rc := OS_FILE_READ(handle, #serNumFILE[0], serNumLen);
          IF rc > 0 THEN
            removeableMediaChanged := 1;
            IF rc = serNumLen & _memcmp(#serNumPLC[0], #serNumFILE[0], serNumLen) = 0 THEN
              removeableMediaChanged := 0;
            END_IF;
          END_IF;
          OS_FILE_CLOSE(handle);
        END_IF;
        IF removeableMediaChanged <> 0 THEN
          handle := OS_FILE_OPEN("C:\LSLDATA\SERNUM.DAT", 1, 0); 	// 1 = create always
          IF handle >= 0 THEN
            FileWrite(handle, #serNumPLC[0], serNumLen);
            OS_FILE_CLOSE(handle);
          END_IF;
        END_IF;
      END_IF;
    END_IF;
    
  END_IF;
  
  IF removeableMediaChanged > 0 THEN
    changed := TRUE;
  ELSE
    changed := FALSE;
  END_IF;

END_FUNCTION

//------------------
//  Den reservieren speicher freigeben (pMem)
//------------------
FUNCTION GLOBAL _Free
VAR_INPUT
	pMem            : pVOID;
END_VAR

		OS_SSR_Free(pMem);

END_FUNCTION

// dient zum auffinden von allocs, die eigentlich in den deskriptor-bereich gehören
(*
function alloctest
  if descrBlocksActive then
    descrBlocksActive := descrBlocksActive; //<- breakpoint hier setzen
  end_if;
end_function
*)

//    Allocates memory on the heap (data segment)
//    
//    -> uiSize : requested number of byte
//    <- pMem   : - pointer to the allocated memory area
//                - NIL if function failed
//       if uiSize = 0
//              <--no allocation , pMem= next free


// Das ist ein alloc das auf gante 4-bytes schaun sollte wegen speed (obs ds tut wiss ma net so richtig)
// hat es bis 5.28 nicht getan weil der linker (onlinelinker) fuer die variablen
// eine ungerade adresse geliefert hat.
FUNCTION GLOBAL _alloc_resize
VAR_INPUT
	udSize		: UDINT;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
END_VAR
//alloctest();

	IF fUseMallocV1 THEN
		pMem	:= OS_SSR_MallocV1(udSize, MEMORY_KENNUNG_PRG);
	ELSE
		pMem	:= OS_SSR_Malloc(udSize);
	END_IF;
END_FUNCTION


FUNCTION GLOBAL _alloc_resize_LDR
VAR_INPUT
	udSize		: UDINT;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
END_VAR
//alloctest();

	IF fUseMallocV1 THEN
		pMem	:= OS_SSR_MallocV1(udSize, MEMORY_KENNUNG_LDR);
	ELSE
	pMem	:= OS_SSR_Malloc(udSize);
	END_IF;
  
END_FUNCTION


// CONVERT DataSize into alocated size (nrof blocks * CST_SRAM_BLOCKSIZE)
// 
// Es wird die Anzahl Bytes zurückgegeben, die allokiert werden, wenn Platz 
// für 'DataSize' Bytes benötigt wird (aufrunden auf den nächsten Wert der 
// durch pLdrmem^.blocksize teilbar ist).
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL SRAM_GET_ALLOCATED
VAR_INPUT
 DataSize   :UDINT;
END_VAR
VAR_OUTPUT
 AlocSize   :UDINT; 
END_VAR
VAR
	blockSize : UDINT;
END_VAR

  blocksize := pLdrmem^.blocksize;
  AlocSize := DataSize + blockSize - 1;
  AlocSize := AlocSize - AlocSize MOD blockSize;

END_FUNCTION
#else
FUNCTION AWL GLOBAL SRAM_GET_ALLOCATED
VAR_INPUT
 DataSize   :UDINT;
END_VAR
VAR_OUTPUT
 AlocSize   :UDINT; 
END_VAR

    L.ECX		pLdrmem
	L.ECX		(ECX + MAT_INFO_TYPE.blocksize)
	CLR			EDX
	L.EAX		DataSize
	DIV			ECX
	O.EDX		EDX
	JZ			Nxb
	INC			EAX
Nxb
	MUL 		ECX
	S.EAX		AlocSize

END_FUNCTION
#endif


//    ReAllocates memory on the heap (data segment)
//    
//	  -> pOld	: pointer to previously allocated block of mem
//    -> uiSize : requested number of byte
//    <- pMem   : - pointer to the allocated memory area
//                - NIL if function failed
//    if uiSize = 0
//              <--no allocation , pMem= next free
//
FUNCTION GLOBAL _realloc
VAR_INPUT
	pOld			: ^void;
	udSize          : UDINT;
END_VAR
VAR_OUTPUT
	pMem            : ^void;
END_VAR
//alloctest();

	IF fUseMallocV1 THEN
		pMem	:= OS_SSR_REALLOCV1(pOld, udSize, MEMORY_KENNUNG_PRG);
	ELSE
		pMem	:= OS_SSR_REALLOC(pOld, udSize);
	END_IF;

END_FUNCTION


FUNCTION GLOBAL _realloc_LDR
VAR_INPUT
	pOld			: ^void;
	udSize          : UDINT;
END_VAR
VAR_OUTPUT
	pMem            : ^void;
END_VAR

	IF fUseMallocV1 THEN
		pMem	:= OS_SSR_REALLOCV1(pOld, udSize, MEMORY_KENNUNG_LDR);
	ELSE
		pMem	:= OS_SSR_REALLOC(pOld, udSize);
  END_IF;

END_FUNCTION

(*
  Allokiert einen neuen Container-Block und hängt ihn in die verkettete Liste ein.
  
  Innerhalb eines Container-Block können weitere kleinere Speicherblöcke allokiert 
  werden. Diese inneren Blöcke werden einfach der Reihe nach hintereinander platziert 
  und können somit nur als Ganzes wieder freigegeben werden.
 *)
FUNCTION newContainerBlock
VAR_INPUT
  ppContainerBlockList : ^ContainerBlockPtr;  // verkettete Liste von Container-Blöcken
  minSize : UDINT;
END_VAR
VAR_OUTPUT
  pContainerBlock : ^ContainerBlock;
END_VAR
VAR
  allocSize : UDINT;
END_VAR

#define CONTAINER_BLOCK_ALLOC_SIZE    16#4000
  minSize += TO_UDINT(ContainerBlock.dummyData);
  IF minSize > CONTAINER_BLOCK_ALLOC_SIZE THEN
    allocSize := minSize;
  ELSE
    allocSize := CONTAINER_BLOCK_ALLOC_SIZE;
  END_IF;

  pContainerBlock	:= _alloc_resize_LDR(allocSize)$^ContainerBlock;
  IF pContainerBlock = NIL THEN
    RETURN;
  END_IF;
  _memset(pContainerBlock, 0, SIZEOF(ContainerBlock));
  pContainerBlock^.allocatedSize := allocSize;
  pContainerBlock^.usedSize := TO_UDINT(ContainerBlock.dummyData);
  pContainerBlock^.pNext := ppContainerBlockList^;
  ppContainerBlockList^ := pContainerBlock;
    
END_FUNCTION

(*
  Allokiert Container-Speicher im sog. Container-Bereich.
 
  Wenn Container-Speicher allokiert werden soll, dann wird vorerst ein grösserer 
  Container-Block am Heap allokiert. Aus diesem Container-Block wird dann so lange 
  Speicher vergeben, bis der Block voll ist. Ist er voll, dann wird ein neuer leerer 
  Container-Block allokiert. Die Container-Blöcke sind mit Zeigern zu einer Liste 
  verkettet.
  
  Alles was im Container-Bereich allokiert wird, kann später schnell freigegeben 
  werden (durch Freigeben der Container-Blöcke anstatt der einzelenen Blöcke 
  in den Containern). 
 *)
FUNCTION GLOBAL LDR_alloc_in_container
VAR_INPUT
  ppContainerBlockList : ^ContainerBlockPtr;  // verkettete Liste von Container-Blöcken
	udSize		: UDINT;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
END_VAR
VAR
  pContainerBlockList : ^ContainerBlock;
END_VAR

  pMem := NIL;

  // aufrunden auf einen durch 4 teilbaren Wert
  WHILE udSize AND 3 DO
    udSize += 1;
  END_WHILE;
  
  IF ppContainerBlockList^ = NIL THEN
    IF newContainerBlock(ppContainerBlockList, minSize := udSize) = NIL THEN
      RETURN;
    END_IF;
  END_IF;
  pContainerBlockList := ppContainerBlockList^;

  IF (pContainerBlockList^.allocatedSize - pContainerBlockList^.usedSize) < udSize THEN
    IF newContainerBlock(ppContainerBlockList, minSize := udSize) = NIL THEN
      RETURN;
    END_IF;
    pContainerBlockList := ppContainerBlockList^;
  END_IF;
  
  pMem := pContainerBlockList + pContainerBlockList^.usedSize;
  pContainerBlockList^.usedSize += udSize;

END_FUNCTION

//#define TRACE_MEM_USAGE
(* 
  Gibt den Speicher der Container-Blöcke frei
*)
FUNCTION GLOBAL LDR_free_all_container
VAR_INPUT
  ppContainerBlockList : ^ContainerBlockPtr;  // verkettete Liste von Container-Blöcken
END_VAR
VAR
  pContainerBlock : ^ContainerBlock;
  pMem : pVoid;
#ifdef TRACE_MEM_USAGE  
  cnt : UDINT;
  nbrName : ARRAY[0..20] OF CHAR;
#endif  
END_VAR

#ifdef TRACE_MEM_USAGE  
  cnt := 0;
#endif  
  pContainerBlock := ppContainerBlockList^;
  WHILE pContainerBlock DO
    pMem := pContainerBlock;
    pContainerBlock := pContainerBlock^.pNext;
    _free(pMem);
#ifdef TRACE_MEM_USAGE  
    cnt += 1;
#endif    
  END_WHILE;
  ppContainerBlockList^ := NIL;
#ifdef TRACE_MEM_USAGE  
	_itoa(cnt * CONTAINER_BLOCK_ALLOC_SIZE, #nbrName[0]);
	TRACE0("LDR_free_all_container: freed {0} bytes", #nbrName[0]);
#endif  

END_FUNCTION

(*
  Allokiert einen neuen Descriptor-Block und hängt ihn in die verkettete Liste ein.
 *)
FUNCTION newDescrBlock
VAR_INPUT
  minSize : UDINT;
END_VAR
VAR_OUTPUT
  pDescrBlock : ^DescrBlock;
END_VAR
VAR
  allocSize : UDINT;
END_VAR

#define DESCR_BLOCK_ALLOC_SIZE    16#10000
  IF minSize > DESCR_BLOCK_ALLOC_SIZE THEN
    allocSize := minSize;
  ELSE
    allocSize := DESCR_BLOCK_ALLOC_SIZE;
  END_IF;

	IF fUseMallocV1 THEN
		pDescrBlock	:= OS_SSR_MallocV1(allocSize, MEMORY_KENNUNG_LDR)$^DescrBlock;
	ELSE
		pDescrBlock	:= OS_SSR_Malloc(allocSize)$^DescrBlock;
	END_IF;
  IF pDescrBlock = NIL THEN
    RETURN;
  END_IF;
  _memset(pDescrBlock, 0, SIZEOF(DescrBlock));
  pDescrBlock^.blockAddr := pDescrBlock;
  pDescrBlock^.allocatedSize := allocSize;
  pDescrBlock^.originalSize := allocSize;
  pDescrBlock^.usedSize := TO_UDINT(DescrBlock.dummyData);
  pDescrBlock^.pNext := pDescrBlockList;
  pDescrBlockList := pDescrBlock;
    
END_FUNCTION

(*
  Verkleinert die allokierte Grösse eines Descriptor-Blocks auf die benutzte Grösse
 *)
FUNCTION ShrinkDescrBlock
VAR_INPUT
  pDescrBlock : ^DescrBlock;
END_VAR

  OS_SSR_ShrinkAlloc(pDescrBlock, pDescrBlock^.usedSize);
  pDescrBlock^.allocatedSize := pDescrBlock^.usedSize;

END_FUNCTION

(*
  Allokiert Descriptor-Speicher im sog. Descriptor-Bereich.
  
  Alles was im Descriptor-Bereich allokiert wird, wird später in ein File geschrieben, 
  damit beim nächsten Hochlauf der Descriptor-Bereich aus dem File wiederhergestellt 
  werden kann.
  In den Descriptor-Bereich kann man z.B. die Klassendeskriptoren legen. Beim Hochlauf 
  wird dann anhand einer CRC geprüft, ob man die Klassendeskriptoren aus dem File 
  wiederherstellen kann. Wenn ja, dann erspart man sich das langwierige Erstellen der 
  Klassendrscriptoren.
 
  Wenn Descriptor-Speicher allokiert werden soll, dann wird vorerst ein grösserer 
  Descriptor-Block am Heap allokiert. Aus diesem Descriptor-Block wird dann so lange 
  Speicher vergeben, bis der Block voll ist. Ist er voll, dann wird ein neuer leerer 
  Descriptor-Block allokiert. Die Descriptor-Blöcke sind mit Zeigern zu einer Liste 
  verkettet.
 *)
FUNCTION GLOBAL LDR_alloc_descr
VAR_INPUT
	udSize		: UDINT;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
END_VAR

  IF !!fUseDescrBlocks | !!descrBlocksActive THEN
    pMem := _alloc_resize_LDR(udSize);
    RETURN;
  END_IF;

  pMem := NIL;

  IF pDescrBlockList = NIL THEN
    IF newDescrBlock(minSize := udSize) = NIL THEN
      RETURN;
    END_IF;
  END_IF;

  // aufrunden auf einen durch 4 teilbaren Wert
  WHILE udSize AND 3 DO
    udSize += 1;
  END_WHILE;
  
  IF (pDescrBlockList^.allocatedSize - pDescrBlockList^.usedSize) < udSize THEN
    // Den aktuellen Descriptor-Block auf usedSize reduzieren
    IF newDescrBlock(minSize := udSize) = NIL THEN
      RETURN;
    END_IF;
  END_IF;
  
  pMem := pDescrBlockList + pDescrBlockList^.usedSize;
  pDescrBlockList^.usedSize += udSize;

END_FUNCTION

(* 
  Funktion zum Freigeben von Speicher, wobei geprüft wird, ob sich der 
  freizugebende Block in einem Desckriptor-Block befindet. Wenn ja, dann 
  wird kein _free aufgerufen, weil der Block kein echter Heap-Block ist 
  (nur der Desckriptor-Block ist ein Heap-Block).
  
  Anm: Würde man _free für einen Block aufrufen, der kein Heap-Block ist
  dann passiert normalerweise nichts, weil das vom OS geprüft wird. Aber 
  wenn die DBGHEAP Option CHECK_FREE_PTR aktiviert ist, dann wird ein Fehler 
  gemeldet.
*)
FUNCTION GLOBAL LDR_free_descrSave
VAR_INPUT
	pMem            : pVOID;
END_VAR
VAR
  pDescrBlock : ^DescrBlock;
END_VAR

  IF fUseDescrBlocks THEN
    pDescrBlock := pDescrBlockList;
    WHILE pDescrBlock DO
      IF (pMem >= pDescrBlock) & (pMem < (pDescrBlock + pDescrBlock^.usedSize)) THEN
        RETURN;
      END_IF;
      pDescrBlock := pDescrBlock^.pNext;
    END_WHILE;
  END_IF;
  
	_free(pMem);

END_FUNCTION

  
(*
  Sagt aus, ob es sich um eine CPU mit Readonly-Filesystem handelt oder nicht.
*)
FUNCTION IsReadonlyFilesystem  
VAR_OUTPUT
	retVal : BOOL;
END_VAR

  CASE _whoami OF
      DESTPLC_CCL081CET,
      DESTPLC_CCL722,
      DESTPLC_DCL642,
      DESTPLC_DTC081,
      DESTPLC_DTC081_IP,
      DESTPLC_DTC101,
      DESTPLC_HZS511,
      DESTPLC_HZS515:
      retVal := TRUE;
    ELSE
      retVal := FALSE;
  END_CASE;

END_FUNCTION  


(*
  Sagt aus, ob der Deskriptor-Bereich in ein File geschrieben wird, damit dieser beim 
  nächsten Hochlauf aus dem File wiederhergestellt werden kann.
 *)
FUNCTION GLOBAL LDR_GetDescrBlocksFlag
VAR_OUTPUT
	retVal : BOOL;
END_VAR

	retVal := FALSE;	
  IF _rtosversion >= (16#1200 + 124) THEN
    (* Ab OS Version 1.2.124 werden die Deskriptoren nicht mehr in einem File zwischengespeichert, 
       weil aufgrund einer Optimierung die Hochlaufphase verkürzt wurde. Das Zwischenspeichern 
       zahlt sich in diesem Fall nicht mehr aus.
    *)
    RETURN;
  END_IF;  
  
  IF IsReadonlyFilesystem() THEN
    RETURN;
  END_IF;
  
  IF _rtosversion >= (16#1100 + 217) THEN
    IF LSLFILE_IsInstalled THEN
      retVal := TRUE;
    END_IF; 
  END_IF;

END_FUNCTION  

(*
  Liefert die Projektchecksumme die zum Vergleich des aktuellen Projekts mit den 
  im File gespeicherten Descriptor-Informationen verwendet wird.
  
  Anm:
  In dieser Checksumme ist auch die Reihenfolge der Module im Speicher mit 
  eingerechnet (bei OS_LNK_GetPrjChk ist das nicht der Fall).
 *)
FUNCTION GetDescrBlocksPrjChk
VAR_OUTPUT
	chk : UDINT;
END_VAR

  chk := OS_LNK_GetPrjChk_WithModOrder();
  
END_FUNCTION  

(*
  Schreibt die vorhandenen Descriptor-Blöcke in ein File.
 *)
FUNCTION GLOBAL LDR_CommitDescrBlocks
VAR_INPUT
  pPrjRd			: pVoid;
  pFirstCls		: pVoid;
  pLastCls		: pVoid;
END_VAR
VAR 
	handle : DINT;
  pDescrBlock : ^DescrBlock;
  fileHdr : DescrBlocksFileHdr;
  rc : DINT;
END_VAR

  descrBlocksActive := FALSE;
  
  IF !!fUseDescrBlocks | pDescrBlockList = NIL THEN
    RETURN;
  END_IF;

  // Alle Descriptor-Blocks auf usedSize reduzieren
  pDescrBlock := pDescrBlockList;
  WHILE pDescrBlock <> NIL DO
    ShrinkDescrBlock(pDescrBlock);
    pDescrBlock := pDescrBlock^.pNext;
  END_WHILE;

	OS_FILE_CREATEDIR(DESCR_BLOCKS_PFAD);
  
  handle:= OS_FILE_OPEN(DESCR_BLOCKS_FNAME, 2, ATT_CREATE);
  IF handle < 0 THEN
    RETURN;
  END_IF;

  _memset(#fileHdr, 0, SIZEOF(fileHdr));

  pDescrBlock := pDescrBlockList;
  WHILE pDescrBlock <> NIL DO
    fileHdr.nbrOfBlocks += 1;
    pDescrBlock := pDescrBlock^.pNext;
  END_WHILE;
  fileHdr.prjCrc    := GetDescrBlocksPrjChk();
  fileHdr.osVer     := _RtOSversion;
  fileHdr.userHeapStartAddr := _UserHeapStartAddr;
  fileHdr.userHeapTotalSize := _UserHeapTotalSize;
  fileHdr.checkptFreeMem    := checkptFreeMem;
  fileHdr.version   := DESCR_BLOCKS_FILE_VERSION;
  fileHdr.pPrjRd    := pPrjRd;
  fileHdr.pFirstCls := pFirstCls;
  fileHdr.pLastCls  := pLastCls;

	rc := FileWrite(handle, #fileHdr, sizeof(fileHdr));
	IF rc <> sizeof(fileHdr) THEN
		OS_FILE_CLOSE(handle);
		RETURN;
	END_IF;

  pDescrBlock := pDescrBlockList;
  WHILE pDescrBlock <> NIL DO

    rc := FileWrite(handle, pDescrBlock, pDescrBlock^.usedSize);
    IF rc$UDINT <> pDescrBlock^.usedSize THEN
      OS_FILE_CLOSE(handle);
      RETURN;
    END_IF;
  
    pDescrBlock := pDescrBlock^.pNext;
  
  END_WHILE;

  OS_FILE_CLOSE(handle);
  
  //====
	// Jetzt wird das File gültig gemacht, indem im Header DataValid auf 1 gesetzt wird
  //====
  
	fileHdr.dataValid := 1;
	handle := OS_FILE_OPEN(DESCR_BLOCKS_FNAME, 2, ATT_READ_WRITE); 	// 2 = use flags in parameter 3
	IF handle <= 0 THEN
		RETURN;
	END_IF;
	
	rc := FileWrite(handle, #fileHdr, SIZEOF(fileHdr.dataValid));
	IF rc <> SIZEOF(fileHdr.dataValid) THEN
		OS_FILE_CLOSE(handle);
		RETURN;
	END_IF;

  (* Test
  pDescrBlock := pDescrBlockList;
  WHILE pDescrBlock <> NIL DO
    DEBUGLOG3("%010d-pDescrBlock=0x%08X, alloc=0x%08X, used=0x%08X", pDescrBlock$UDINT, pDescrBlock^.allocatedSize, pDescrBlock^.usedSize);
    pDescrBlock := pDescrBlock^.pNext;
  END_WHILE;
  *)

	OS_FILE_CLOSE(handle);

END_FUNCTION

(*
  Versucht die Descriptor-Blöcke aus dem File zu laden.
  Wenn sie geladen werden konnten, dann wird TRUE zurückgegeben.
 *)
FUNCTION GLOBAL LDR_LoadDescrBlocks
VAR_INPUT
  ppPrjRd			: ^pVoid;
  ppFirstCls	: ^pVoid;
  ppLastCls	: ^pVoid;
END_VAR
VAR_OUTPUT
  result : BOOL;
END_VAR
VAR 
	handle : DINT;
  pDescrBlock : ^DescrBlock;
  pDescrBlockNext : ^DescrBlock;
  fileHdr : DescrBlocksFileHdr;
  descrBlk : DescrBlock;
  rc : DINT;
  i : UDINT;
END_VAR

  result := FALSE;    

  IF !!fUseDescrBlocks THEN
    RETURN;
  END_IF;
  
  descrBlocksActive := TRUE;

  handle := OS_FILE_OPEN(DESCR_BLOCKS_FNAME, 0, 0);		// 0 = read only
  IF handle < 0 THEN
    RETURN;
  END_IF;

  // An dieser Stelle wird aktuelle freie Speicher im Userheap gemerkt.
  // Nur wenn im Deskriptorblock-File auch der gleiche Wert steht, darf die  
  // Info im File ausgewertet werden. Wenn ein anderer Wert vorgefunden wird, 
  // dann deutet das darauf hin, dass die Speicherverwaltung nicht die gleiche 
  // ist (z.B.wenn sich die DBGHEAP-Flags geändert haben).
  checkptFreeMem := _UserHeapFreeMem;
  
  rc := OS_FILE_READ(handle, #fileHdr, SIZEOF(fileHdr))$DINT;
  IF rc <> SIZEOF(fileHdr)                            | 
     fileHdr.dataValid  =  0                          |
     fileHdr.prjCrc     <> GetDescrBlocksPrjChk()     | 
     fileHdr.osVer      <> _RtOSversion               | 
     fileHdr.userHeapStartAddr <> _UserHeapStartAddr  |
     fileHdr.userHeapTotalSize <> _UserHeapTotalSize  |
     fileHdr.checkptFreeMem    <> checkptFreeMem      |
     fileHdr.version    <> DESCR_BLOCKS_FILE_VERSION  THEN
    OS_FILE_CLOSE(handle);
    RETURN;
  END_IF;
  
  ppPrjRd^    := fileHdr.pPrjRd;
  ppFirstCls^ := fileHdr.pFirstCls;
  ppLastCls^  := fileHdr.pLastCls;

  pDescrBlockList := NIL;
  FOR i := 0 TO fileHdr.nbrOfBlocks - 1 DO
    rc := OS_FILE_READ(handle, #descrBlk, SIZEOF(descrBlk))$DINT;
    IF rc <> SIZEOF(descrBlk) THEN
      GOTO NotOK;
    END_IF;
    
    // Der Block wird nicht sofort mit usedSize allokiert sondern vorerst mit originalSize 
    // und dann wird er auf usedSize verkürzt.
    // Damit wird erreicht, dass nach dem Used-HeapBlock ein Free-HeapBlock ensteht, der 
    // mit dem darauffolgenden Free-HeapBlock noch nicht verschmolzen ist. Das ist notwendig, 
    // damit sich die Speicherverwaltung in Folge gleich verhält wie beim vorherigen 
    // Projektstart, d.h. damit die später allokierten Objekte die gleichen Adressen zugewiesen 
    // bekommen.
    // Wenn sich die Adressen der Objekte ändern würden, dann wäre die Deskriptor-CRC eine 
    // andere (ops.udDescCRC). Das hat zur Folge, dass der LSE die LasalIDs neu anfordert.
    IF OS_SSR_MarkAllocV1(descrBlk.blockAddr, descrBlk.originalSize, MEMORY_KENNUNG_LDR) = 0 THEN
      GOTO NotOK;
    END_IF;
    
		rc := OS_FILE_SEEK(handle$DINT, (SIZEOF(descrBlk) * -1)$UDINT, 1(*FILE_CURRENT*))$DINT;
		IF rc < 0 THEN
      OS_SSR_Free(descrBlk.blockAddr);
      GOTO NotOK;
    END_IF;
    
    rc := OS_FILE_READ(handle, descrBlk.blockAddr, descrBlk.usedSize)$DINT;
    IF rc$UDINT <> descrBlk.usedSize THEN
      OS_SSR_Free(descrBlk.blockAddr);
      GOTO NotOK;
    END_IF;
    
    pDescrBlock := descrBlk.blockAddr$^DescrBlock;
    pDescrBlock^.pNext := pDescrBlockList;
    pDescrBlockList := pDescrBlock;
    
  END_FOR;
  
  // Alle Descriptor-Blocks auf usedSize reduzieren
  pDescrBlock := pDescrBlockList;
  WHILE pDescrBlock <> NIL DO
    ShrinkDescrBlock(pDescrBlock);
    pDescrBlock := pDescrBlock^.pNext;
  END_WHILE;
  
  descrBlocksActive := FALSE;
  result := TRUE;

  (* Test
  pDescrBlock := pDescrBlockList;
  WHILE pDescrBlock <> NIL DO
    DEBUGLOG3("%010d-pDescrBlock=0x%08X, alloc=0x%08X, used=0x%08X", pDescrBlock$UDINT, pDescrBlock^.allocatedSize, pDescrBlock^.usedSize);
    pDescrBlock := pDescrBlock^.pNext;
  END_WHILE;
  *)

NotOK:
  IF !!result THEN
    // bereits allokierten Speicher wieder freigeben
    pDescrBlock := pDescrBlockList;
    WHILE pDescrBlock <> NIL DO
      pDescrBlockNext := pDescrBlock^.pNext;
      OS_SSR_Free(pDescrBlock);
      pDescrBlock := pDescrBlockNext;
    END_WHILE;
    pDescrBlockList := NIL;
    // evtl. bereits vergebene Adressen wieder auf NIL zurücksetzen
    ppPrjRd^    := NIL;
    ppFirstCls^ := NIL;
    ppLastCls^  := NIL;
  END_IF;

  OS_FILE_CLOSE(handle);
  
  IF result THEN
		DEBUGLOG0("%010d-info: re-using binary descr.block-info from prev.run");
  END_IF;

END_FUNCTION

//###############################################################
//  HIER BEGINNT DIE MELKUS VERBESSERUNG
//###############################################################

//============================================================================================
// Header of global functions
//============================================================================================

FUNCTION GLOBAL S_RAM_INIT
 VAR_OUTPUT
	ret_code 		: CONFSTATES;
 END_VAR;




FUNCTION SramLoadFile
VAR_OUTPUT
	result : UDINT;
END_VAR	;
FUNCTION LogSramError;
FUNCTION CHK_S_RAM
VAR_INPUT
 fDoNotReportErrors : UDINT;
END_VAR
VAR_OUTPUT
 wasValid : UDINT;
END_VAR;
FUNCTION CLEAR_LASTENTRY; 
FUNCTION GLOBAL SramSaveFile;


FUNCTION InitXramBlockEx
VAR_INPUT
  pGotMemory : ^BOOL;  // Da beim Returnwert 0 nicht unterschieden werden kann, ob 
                       // der Offset 0 gemeint ist oder Out-Of-Memory, wird mit diesem 
                       // Flag angezeigt, ob Speicher allokiert werden konnte.
END_VAR
VAR_OUTPUT
  xBlockOffset :UDINT ; // Offset of header 
END_VAR;
FUNCTION GLOBAL InitXramBlock
VAR_OUTPUT
  xBlockOffset :UDINT ; // Offset of header 
END_VAR;


FUNCTION GLOBAL IniNewRamBlock
VAR_INPUT
  PtLastRamcell :^xRamCell;
END_VAR
VAR_OUTPUT
  Reladress :UDINT;
END_VAR;

FUNCTION GLOBAL   GET_USED_S_RAM;  // Whenever new S-RAM is alocatetd,S_Ram saver must be informed


//-------------------------
// Calc CKSUM of RAMEX Hdr End MakeIt Valid
//-------------------------

FUNCTION GLOBAL AWL MakeRamExChk
VAR_INPUT
  pXramCell : ^XramCell;
END_VAR;



//####################################
// menks gerRAMle ab 2.2.2002 mit a.m zur vergangenheitsbewältigung
//####################################

// --- menks Sram alocation 

FUNCTION GLOBAL SRAM_Startup  // Belegungslieste Initialisieren
VAR_INPUT
   length0  : UDINT;
   clear0   : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR;

FUNCTION GLOBAL SRAM_ClearMAT;   // -- ALLE Blockmarker mit 1 auf "leer" setzen


FUNCTION GLOBAL SRAM_GetLastAddr   // Liefert Die Belegte Speichergrösse (bezogen auf speicherstart)

VAR_OUTPUT
   ret0 : UDINT;
END_VAR;



FUNCTION GLOBAL SRAM_MarkMemory   // belegten speicher an alocation melden (beim hochfahren)
VAR_INPUT
   reladdr0 :^usint;
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR;

FUNCTION AWL GLOBAL SRAM_UnmarkMemory
VAR_INPUT
   reladdr0 :^usint;
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR;

FUNCTION SRAM_GetMemoryEx
VAR_INPUT
   length0  : UDINT;
   pGotMemory : ^BOOL;  // Da beim Returnwert 0 nicht unterschieden werden kann, ob 
                        // der Offset 0 gemeint ist oder Out-Of-Memory, wird mit diesem 
                        // Flag angezeigt, ob Speicher allokiert werden konnte.
END_VAR
VAR_OUTPUT
   ret0 : ^USINT;
END_VAR;
FUNCTION GLOBAL SRAM_GetMemory   // belegung in der alocation anfordern 
VAR_INPUT
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : ^USINT;
END_VAR;

//#SR 16.12.2005
FUNCTION SRAM_FreeMarkMemory   //belegung in der alocation freigeben oder markieren
VAR_INPUT
   reladdr0 : UDINT;
   length0  : UDINT;
   mode0    : UDINT;
   check0   : BOOL;         //FALSE:ändern, TRUE:nur prüfen
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR;

//----------------------------





//----------------------
// S_RAM INITIALISIEREN
//-----------------------

//--- Alte datensructur der Active.dat -----

TYPE
  DatKopf : STRUCT
  Signi  :ARRAY [0..15] of CHAR;
  CRC	 :UDINT;
  REV    :UDINT;
  NrRec	 :UDINT;
  Frei	 :UDINT;
  END_STRUCT;
END_TYPE

// -- dieser typ dient zum laden eines alten datensatzes. die Feldlängen sind im file jedoch 
// variabel

TYPE 
  Eintrag : STRUCT
  myguid  : array [0..19] of usint;
  CRC     : UDINT;
  name    : array [0..99] of char;
  Data    : DINT;
  Len     : DINT;
  daten   : array [0..63] of char;	

  END_STRUCT; 
END_TYPE


#define  RamValid 16#8000_0000
#define  RamUsed  16#4000_0000
#define  IsXram   16#3000_0000
  
(*
  Sagt aus, ob der Sram-Bereich (_s_ram_hpter) nullspannungssicher (d.h. batteriegepuffert 
  oder Backup auf SD-Karte) ist oder nicht.
*)
FUNCTION HasRetentiveSram  
VAR_OUTPUT
	retVal : BOOL;
END_VAR

  CASE _whoami OF
    DESTPLC_HZS511,
    DESTPLC_CCL081,
    DESTPLC_CCL081CET,
    DESTPLC_BDF2000_USB,
    DESTPLC_AUW11X:
      retVal := FALSE;
    ELSE
      retVal := TRUE;
  END_CASE;
  

END_FUNCTION  

  
(*
  Sagt aus, ob das SRAM im neuen Format mit dem Ramfile.dat-File benutzt 
  wird oder nicht.

  Es gibt 2 SRAM Formate:
    1 = ohne RAMFILE.DAT
    2 = mit RAMFILE.DAT (in der RAMFILE.DAT sind die statischen Daten wie z.B. CRC abgelegt)
  
  Das zu verwendende SRAM Format wird folgendermassen ermittelt:
  - Wenn die Umgebungsvariable SRAMFORMAT gesetzt ist:
    - Der Wert dieser Variablen bestimmt das SRAM Format (mögliche Werte sind '1' oder '2')
  - Wenn die Umgebungsvariable SRAMFORMAT nicht gesetzt ist:
    - Wenn es sich um best.hardcodierte Plattformen handelt (dzt. C_IPC und TEACHBOX):
      - Es wird das SRAM Format 2 verwendet
    - Bei nicht hardcodierten Plattformen: 
      - Wenn im _s_ram_hptr schon die Version 1 oder 2 eingetragen ist, dann wird diese 
        Version verwendet (das ist das zuletzt verwendete Format)
      - Andernfalls (bei einer jungfräulichen Hardware) oder leerem bzw. ungültigem SRAM:
        - Es wird das SRAM Format 1 verwendet

  Durch die Verwendung der Umgebungsvariable SRAMFORMAT müssen die Plattformen, bei 
  denen das SRAM Format 2 verwendet werden soll, nicht mehr im Loader hardcodiert werden, 
  sondern man kann das Format in der Autoexec.lsl einstellen.
  Wenn einmal mit einer Autoexec.lsl gebootet wird, in der die Umgebungsvariable nicht 
  gesetzt ist (z.B. wenn ein USB Stick angesteckt ist), dann wird das Format verwendet, 
  mit dem das SRAM zuletzt initialisiert worden ist. Das funktioniert dann aber nur bei 
  einer nicht jungräulichen CPU.
  Wenn mit der Umgebungsvariablen gearbeitet wird, dann sollte darauf geachtet werden, 
  dass sich der Befehl zum Setzen des Wertes (SETENV SRAMFORAMT <n>) in der Autoexec.lsl 
  auf c:\ befindet.
*)  
FUNCTION GetRamfileDatUsedFlag
VAR_INPUT
  pFormatChanged : ^BOOL;
END_VAR
VAR_OUTPUT
	retVal : BOOL;
END_VAR
VAR
  pSysenv : ^OS_SYSENV;
  value : ARRAY[0..9] OF CHAR;
  envVar : BOOL;
  oldFormat : UDINT;
END_VAR

  pFormatChanged^ := FALSE;
  retVal := FALSE;
  IF _S_RAM_Hptr = NIL THEN
    RETURN;
  END_IF;
  
  oldFormat := (_S_RAM_Hptr^.DataValid SHR 16) AND 16#00FF;
  
  envVar := FALSE;
  OS_CILGet("SYSENV", #pSysenv$void);
  IF pSysenv & OS_GETENVVAR(pSysenv, "SRAMFORMAT", #value[0], SIZEOF(value)) THEN
    IF !!_strcmp(#value[0], "1") THEN
      envVar := TRUE;
      retVal := FALSE;
    ELSIF !!_strcmp(#value[0], "2") THEN
      envVar := TRUE;
      retVal := TRUE;
    END_IF;
  END_IF;

  IF _s_ram_hptr^.DataLength + 64 < 8*1024 THEN
    // Bei SRAM Format 1 muss auf jeden Fall der xRamHdr Platz haben, daher ist 
    // bei DataLength < 8k nur Format 2 möglich.
    retVal := TRUE;	
  ELSIF envVar THEN
		DEBUGLOG1("%010d-EnvVar SRAMFORMAT defines SRAM format %s", (#value[0])$UDINT);
  ELSE
    // keine Umgebungsvariable gesetzt
#ifdef _LSL_TARGETARCH_ARM
    retVal := TRUE;	
#else
    CASE _whoami OF
      // hardcodierte Plattformen
      DESTPLC_C_IPC,
      DESTPLC_TEACHBOX:
        DEBUGLOG1("%010d-Platform %d defines SRAM format 2", _whoami$UDINT);
        retVal := TRUE;
      ELSE
        IF !!HasRetentiveSram() THEN
          DEBUGLOG1("%010d-Platform %d (no retentive sram) defines SRAM format 1", _whoami$UDINT);
          retVal := FALSE;
        // nicht hardcodierte Plattformen: Auswertung der Version im _S_RAM_Hptr
        ELSIF (_S_RAM_Hptr <> NIL) & (oldFormat = 2) THEN
          DEBUGLOG1("%010d-_S_RAM_Hptr version %d defines SRAM format 2", _S_RAM_Hptr^.DataValid SHR 16);
          retVal := TRUE;	
        ELSE
          DEBUGLOG0("%010d-Default value defines SRAM format 1");
          retVal := FALSE;	
        END_IF;
    END_CASE;
#endif    
  END_IF;
  
  IF (oldFormat >= 2 & !!retVal) | (oldFormat < 2 & retVal) THEN
    DEBUGLOG1("%010d-*** SRAM format switch *** (oldformat=%d)", oldFormat);
    pFormatChanged^ := TRUE;
  END_IF;
  
END_FUNCTION  

// Setzt ein Flag, damit beim nächsten Hochlauf das Sram reorganisiert wird, auch wenn 
// die übrigen Kriterien kein Reorg veranlassen.
FUNCTION GLOBAL LDR_ForceSramReorgOnNextRun

	IF _RtOSversion >= 16#1000 THEN
	  OPS.loaderFlags := OPS.loaderFlags OR LOADER_FORCE_SRAM_REORG_ON_NEXT_RUN;
  END_IF;      
  
END_FUNCTION


// Sagt aus, ob das SRAM über das Filesystem reorganisiert werden soll oder nicht
FUNCTION GetSramReorgFlag
VAR_OUTPUT
	retVal : BOOL;
END_VAR
VAR
  pSysenv : ^OS_SYSENV;
  value : ARRAY[0..9] OF CHAR;
END_VAR

	retVal := FALSE;	
  
	IF !!HasRetentiveSram() THEN
    // Wenn kein batterigepuffertes Sram, dann wird nicht reorganisiert
    RETURN;
  END_IF;

  IF _rtosversion <= 16#537 | !!(LSLFILE_IsInstalled) THEN
    // Es gibt kein Filesystem, oder das OS ist so alt, dass es nicht weiß ob eines vorhanden ist
    RETURN;
  END_IF;

  // SA20733/DS1777, DS2352: Wird die Umgebungsvariable SRAM_DISABLE_REORG <> 0 gesetzt, so wird die 
  // Reorganisation unterbunden, allerdings nur dann wenn das Sram gültig ist. Wenn SRAM_DISABLE_REORG_FORCE <> 0 
  // ist, dann wird die Reorganisation in jedem Fall unterbunden.
  OS_CILGet("SYSENV", #pSysenv$void);
  IF pSysenv & OS_GETENVVAR(pSysenv, "SRAM_DISABLE_REORG_FORCE", #value[0], SIZEOF(value)) THEN
    IF _strcmp(#value[0], "0") THEN
      RETURN;
    END_IF;
  END_IF;
  IF pSysenv & OS_GETENVVAR(pSysenv, "SRAM_DISABLE_REORG", #value[0], SIZEOF(value)) THEN
    IF _strcmp(#value[0], "0") THEN
      // Die Umgebungsvariable SRAM_DISABLE_REORG existiert und weist einen Wert 
      // ungleich 0 auf. Wir dürfen somit das SRAM NICHT! reorganisieren.
      // Ausnahme: 
      // Wenn das Bit31 in DataValid 1 ist, dann wurde ein Reorg unterbrochen. In diesem 
      // Fall wird der Wert in SRAM_DISABLE_REORG und das Reorg jetzt fertig ausgeführt.
      IF (_s_ram_hptr^.DataValid AND 16#80000000) = 0 THEN
        RETURN;
      ELSE
  			DEBUGLOG0("%010d ignoring env.var SRAM_DISABLE_REORG because sram reorg was interrupted");
      END_IF;
    END_IF;
  END_IF;
  
  (*
  DEVOS-316:
  Bei den Loaderversionen 2.2.103 bis 2.2.194 wurde nicht reorganiaisert, wenn die 
  Applikation nach dem Einschalten schon einmal gelaufen ist und sich der Projektname 
  nicht geändert hat (damit konnte man sich beim Entwickeln Zeit einsparen). 
  Das hat aber den ungewünschten Effekt, dass die Descriptor-CRC im Reorg und im nicht-Reorg 
  Fall unterschiedlich ist (LoadSramKopie wird während der Erstellung der Objekte vom Konstruktor 
  von RAM-Objekten aufgerufen und allokiert Speicher, wodurch sich die Adressen von nachfolgend 
  erstellten Objekten ändert).
  Es wird deshalb jetzt wieder immer reorganisiert, damit die Descriptor-CRC in diesen beiden 
  Fällen gleich ist.
  *)
  retVal := TRUE;

END_FUNCTION  

(* Liefert die Info, ob die Loader Initphase angehalten wird, wenn ein Fehler im Sram erkannt wird. *)
FUNCTION GetSramContinueOnErrorFlag
VAR_OUTPUT
  retVal : BOOL;
END_VAR
VAR
  pSysenv : ^OS_SYSENV;
  value : ARRAY[0..9] OF CHAR;
END_VAR

  retVal := FALSE;
  
  OS_CILGet("SYSENV", #pSysenv$void);
  IF pSysenv & OS_GETENVVAR(pSysenv, "SRAM_CONTINUE_ON_ERROR", #value[0], SIZEOF(value)) THEN
    IF _strcmp(#value[0], "0") THEN
      // Die Umgebungsvariable SRAM_CONTINUE_ON_ERROR existiert und weist einen Wert 
      // ungleich 0 auf. 
      retVal := TRUE;
    END_IF;
  END_IF;
  
END_FUNCTION

#ifndef OTTOSPEED
(* Prüft, ob ein Buffer vollständig mit einem bestimmten Wert gefüllt ist *)
FUNCTION IsBufFilledWith
VAR_INPUT
  buf : ^USINT;
  len : UDINT;
  val : USINT;
END_VAR
VAR_OUTPUT
  retVal : BOOL;
END_VAR
VAR
	i : UDINT;
END_VAR

  retVal := TRUE;

  FOR i := 0 TO len - 1 DO 
    IF buf^ <> val THEN
      retVal := FALSE;
      RETURN;
    END_IF;
    buf += 1;
  END_FOR;

END_FUNCTION
#endif

(* Liefert die Info, ob das Sram neu (noch nie initialisiert nach der Produktion) 
   oder durch einen SRAMCLEAR Befehl gelöscht wurde. In so einem Fall wird TRUE 
   zurückgegeben, andernfalls FALSE.
*)
FUNCTION GetSramNewOrClearedFlag
VAR_OUTPUT
  retVal : BOOL;
END_VAR
VAR
	handle : DINT;
  rc : UDINT;
  buf : ARRAY[0..255] OF USINT;
END_VAR

  retVal := FALSE;
  IF _S_RAM_Hptr = NIL THEN
    RETURN;
  END_IF;

  IF LSLFILE_IsInstalled THEN
    (* Wenn ein Filesystem vorhanden ist und die Datei isnotnew.inf existiert 
       noch nicht, kann aber erstellt werden, dann handelt es sich um ein 
       jungfräuliches System.
     *)
    handle := OS_FILE_OPEN("c:\lsldata\isnotnew.inf", 0, 0);	// read-only
    IF handle >= 0 THEN
      // Datei existiert -> kein jungfäuliches System
      OS_FILE_CLOSE(handle);
    ELSE
      OS_FILE_CREATEDIR("c:\lsldata");
			handle := OS_FILE_OPEN("c:\lsldata\isnotnew.inf", 2, ATT_CREATE);
      IF handle >= 0 THEN
        // Datei existiert nicht, konnte aber erstellt werden -> jungfäuliches System
        OS_FILE_CLOSE(handle);
        retVal := TRUE;
        RETURN;
      END_IF;
    END_IF;
  END_IF;


	IF isRamfileDatUsed THEN
    (* Format 2 (mit ramfile.dat):
    
       Wenn die Datei ramfile.dat fehlt, dann handelt es sich um ein jungfräuliches 
       Sram oder um einen Versuch, durch Löschen der ramfile.dat das Sram zu löschen. 
       
       Wenn die Daten in ramfile.dat 256 Bytes groß ist und mit 0x00 gefüllt ist, 
       dann wurde das Sram mit dem CLI-Befehl SRAMCLEAR gelöscht.
     *)
    handle := OS_FILE_OPEN("c:\ramfile.dat", 0, 0);	// read-only
    IF handle >= 0 THEN
      rc := OS_FILE_READ(handle, #buf[0], SIZEOF(buf));
      OS_FILE_CLOSE(handle);
      IF rc = SIZEOF(buf) THEN
       #ifdef OTTOSPEED
        if _ismemset(#buf[0], 0, sizeof(buf)) then
       #else
        IF IsBufFilledWith(#buf[0], SIZEOF(buf), 0) THEN
       #endif
          // ramfile.dat ist 256 bytes groß und enthält nur 0er
          retVal := TRUE;
          RETURN;
        END_IF;
      END_IF;
    ELSIF handle = -9 THEN // -9 = RTF_FILE_NOT_FOUND
      // ramfile.dat fehlt
      retVal := TRUE;
      RETURN;
    END_IF;
     
  ELSE
    (* Format 1 (ohne ramfile.dat)
    
       Wenn das Sram mit dem CLI Befehl SRAMCLEAR gelöscht wurde, dann enthalten die 
       ersten 256 Bytes ab _S_RAM_Hptr^.DataStart nur 0er.
       In so einem Fall kommt man gar nicht hierher weil diese Daten einem gültigen 
       Sram ohne Ram-Zellen entsprechen. 
       Für den Fall, dass einmal eine erweiterte Prüfung eingebaut wird und dann 
       doch ein Sram-Fehler nach dem SRAMCLEAR festgestellt wird, wird hier weiter 
       geprüft.
       Um zwischen SRAMCLEAR und einer leeren Batterie unterscheiden zu können 
       (für den Fall dass bei einer leeren Batterie der Sram Inhalt 0 ist), wird 
       zusätzlich _S_RAM_Hptr^.DataValid abgefragt. Der Wert dieser Variablen müßte 
       bei einem SRAMCLEAR auf <>0 stehen, denn SRAMCLEAR ändert in diesem Bereich nichts.
    *)
    
   #ifdef OTTOSPEED 
    if _ismemset(_S_RAM_Hptr^.DataStart$^USINT, 0, 256) & _S_RAM_Hptr^.DataValid <> 0 then
   #else
    IF IsBufFilledWith(_S_RAM_Hptr^.DataStart$^USINT, 256, 0) & _S_RAM_Hptr^.DataValid <> 0 THEN
   #endif
      retVal := TRUE;
      RETURN;
    END_IF;
  
  END_IF;

END_FUNCTION

(*

SRAM Header
-----------

//-------------------------------------------------------------------
  MRAM_DESCR     : STRUCT
    version     : UDINT;    // wird vom OS ausgefüllt
    DataStart   : UDINT;    // wird vom OS ausgefüllt
    DataLength  : UDINT;    // wird vom OS ausgefüllt
    UsedData    : UDINT;    // wird vom Loader ausgefüllt
    DataValid   : UDINT;    // wird vom Loader ausgefüllt
  END_STRUCT;
//-------------------------------------------------------------------

SRAM Format 1
-------------

//-------------------------------------------------------------------
   XRAMHDR     : STRUCT
     udEntries  : UDINT;
     udChk      : UDINT;     // = NOT udEntries
     aData      : ARRAY [0..X_RAM_CELLS] OF XRamCell;
   END_STRUCT;

   XRamCell     : STRUCT    // for all entries from Nr 1000 on
     udChk      : UDINT;    // = udCRC32+udData
     udCRC32    : UDINT;
     udData     : UDINT;    // RAMEX = Bit 0 - 15 length of RAMEX overhead - Bit 16-31 length of RAMEX (add both length)
     udType     : UDINT;    // see below
   END_STRUCT;

//  udType:
//
// Bit 31.. 1= VALID  (CHksum is OK)
//     30.. 1= USED   (After Startup, it is conected to an Objekt)
//     29.. 1= RAMEX
//     28.. 1= RAMEX
//     27.. 0
//     26.. 0
//     25..   Frei
//     24..   Frei
//
//     0-23   RAMEX = Relative Startadress (of DataStart)
//-------------------------------------------------------------------

SRAM Format 2
-------------

Um Platz im SRAM zu sparen, gibt es beim CIPC das SRAM Format 2. Die
unveränderlichen Daten werden dabei in ein File ausgelagert (RAMFILE.DAT)

Das RAMFILE.DAT besteht aus einem Header und einer fixen Anzahl von XFileCell's.
Im Header steht die Anzahl der benutzten XFileCell's.

Eine XFileCell beinhaltet die CRC32 des Objektnamens, den Typ und den rel.Offset
im SRAM. Im SRAM steht dann bei einer RAM Zelle nur dData (4 Bytes), bei einer
RAMEX Zelle die gesamte XRamCell (16 Bytes) + die allokierten Daten der RAMEX.

  XFileCell     : STRUCT
    udCRC32     : UDINT;    // CRC of Name
    udType      : UDINT;    // bit 31 = RAM 4Btye  1=Extended ram (more than 4 bytes long) 0-31= offset of data from RAMHDR
    udpRelSram  : UDINT;    // relativ offset in S-Ram
  END_STRUCT;

  XFILEHDR      : STRUCT
    udEntries   : UDINT;    // anzahl
    uiChange    : UINT;     // change flag
    CRC32       : UDINT;    // CRC über gesamtes FILE
    pLastEntry  : UDINT;    // Zaehler auf letzten gefundenen eintrag
    aData       : ARRAY [0..(X_FILE_CELLS-1)] OF XFileCell;
  END_STRUCT;

*)

FUNCTION CONVERT_1_2
VAR_INPUT
  pNewSRam      : ^UDINT;
  newSRamLength : UDINT;
  pFileMem      : ^XFILEHDR;
END_VAR
VAR_OUTPUT
  convertOK   : BOOL;
END_VAR
VAR
  noEntries   : UDINT;
  iEntries    : UDINT;
  iConverted  : UDINT;
  iErrors     : UDINT;
  iConnection : UDINT;
  offsSRam    : UDINT;
  pRamCell    : ^XRamCell;
  pFileCell   : ^XFileCell;
  ramExLen    : UDINT;
  ramExAddr   : ^VOID;
  pConnection : ^UDINT;
  msg         : ARRAY [0..99] OF CHAR;
END_VAR

  if((16#FFFFFFFF - _s_ram_hptr^.DataStart^.udEntries) <> _s_ram_hptr^.DataStart^.udChk) then
    TRACE_ERR("SRAM Header not valid");
    LSLCLI_LoaderTracePrint("SRAM Header not valid", 12);
    DEBUGLOG0("SRAM Header not valid");
    convertOK := 0;
    return;
  end_if;
  
  noEntries := _s_ram_hptr^.DataStart^.udEntries;
  offsSRam := 8;  // + 8 - das sind 8 Byte für udEntries und udChk - damit nicht immer um 8 korrigiert werden muss
  iEntries := 0;
  iConverted := 0;
  iErrors := 0;
  iConnection := 0;
  pRamCell := #_s_ram_hptr^.DataStart^.aData[0];
  pFileCell := #pFileMem^.aData[0];


  convertOK := 1;

CONVERT_1_2_NEXT_BLOCK:
  while(iEntries < noEntries) do
    // valid ?
    if(pRamCell^.udType and 16#80000000) then

      // RamEx **********************
      if((pRamCell^.udType and 16#30000000) = 16#30000000) then
        if(((pRamCell^.udType and 16#00FFFFFF) + pRamCell^.udData + pRamCell^.udCRC32) <> pRamCell^.udChk) then
          goto CONVERT_1_2_ERROR;
        end_if;

        pFileCell^.udCRC32 := pRamCell^.udCRC32;
        pFileCell^.udType := 16#B0000000;  // valid and RAMEX
        pFileCell^.udpRelSram := offsSRam;

        ramExLen := pRamCell^.udData shr 16;
        ramExAddr := _s_ram_hptr^.DataStart + ((pRamCell^.udType) and 16#00FFFFFF);
        
        pRamCell^.udType := pFileCell^.udpRelSram + sizeof(pRamCell^);    // set the new data offset in SRAM
        pRamCell^.udType := pRamCell^.udType or 16#F0000000;  // id for valid, used and RAMEX
        pRamCell^.udChk := (pRamCell^.udType and 16#00FFFFFF) + pRamCell^.udData + pRamCell^.udCRC32;

        if((offsSRam + sizeof(pRamCell^)) >= newSRamLength) then
          convertOK := 0;
          exit;
        end_if;
        _memcpy((pNewSRam + offsSRam), pRamCell, sizeof(pRamCell^));
        offsSRam += sizeof(pRamCell^);

        if((offsSRam + ramExLen) >= newSRamLength) then
          convertOK := 0;
          exit;
        end_if;
        _memcpy((pNewSRam + offsSRam), ramExAddr, ramExLen);
        offsSRam += ramExLen;

        pFileCell += sizeof(pFileCell^);
        
      // Normale Ramzelle **********************
      elsif((pRamCell^.udType and 16#30000000) = 0) then
        if((pRamCell^.udData + pRamCell^.udCRC32) <> pRamCell^.udChk) then
          goto CONVERT_1_2_ERROR;
        end_if;
          
        pFileCell^.udCRC32 := pRamCell^.udCRC32;
        pFileCell^.udType := 16#80000000;
        pFileCell^.udpRelSram := offsSRam;

        if((offsSRam + 4) >= newSRamLength) then
          convertOK := 0;
          exit;
        end_if;
        (pNewSRam + offsSRam)^ := pRamCell^.udData;
        offsSRam += 4;
        pFileCell += sizeof(pFileCell^);
      end_if;
      
      iConverted += 1;
    else
CONVERT_1_2_ERROR:
      iErrors += 1;
    end_if;
    
    iEntries += 1;
    pRamCell += sizeof(pRamCell^);
  end_while;

  // Verbindungszelle **********************
  if((iEntries = 500) & (pRamCell^.udCRC32 = 1954)) then

    if((pRamCell^.udData + pRamCell^.udCRC32) <> pRamCell^.udChk) then
      TRACE_ERR("ConnectionCell Header not valid");
      LSLCLI_LoaderTracePrint("ConnectionCell Header not valid", 12);
      DEBUGLOG0("ConnectionCell Header not valid");
      convertOK := 0;
      return;
    end_if;

    pConnection := (_s_ram_hptr^.DataStart) + pRamCell^.udData;

    // entries - check
    if((16#FFFFFFFF - pConnection^) <> ((pConnection + 4)$^UDINT)^) then
      TRACE_ERR("Next Block Header not valid");
      LSLCLI_LoaderTracePrint("ConnectionCell Header not valid", 12);
      DEBUGLOG0("Next Block Header not valid");
      convertOK := 0;
      return;
    end_if;
    
    noEntries := pConnection^;
    // Nach entries und check gehts mit der ersten RamCell weiter
    pRamCell := (pConnection + 8)$^XRamCell;
    iEntries := 0;

    iConnection += 1;
    
    // Weiter konvertieren
    goto CONVERT_1_2_NEXT_BLOCK;
  end_if;

  if(convertOK = 1 & iConverted <= X_FILE_CELLS_2) then
  // Einträge ins RAMFILE.DAT
    pFileMem^.udEntries := iConverted;
    pFileMem^.uiChange := 16#1234;

    // Einträge ins SRAM
    _s_ram_hptr^.version := 0; // ?????
    _s_ram_hptr^.UsedData := offsSRam;
    _s_ram_hptr^.DataValid := 16#00020001;
    // Nach memcpy, da diese 8 Byte sonst überschrieben werden
    _memcpy(_s_ram_hptr^.DataStart, pNewSRam, offsSRam);
    _s_ram_hptr^.DataStart^.udEntries := iConverted;
    _s_ram_hptr^.DataStart^.udChk := 16#FFFFFFFF - _s_ram_hptr^.DataStart^.udEntries;
  else
    TRACE_ERR("SRAM CONVERT - CONVERSION not OK");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - CONVERSION not OK", 12);
    DEBUGLOG0("SRAM CONVERT - CONVERSION not OK");
    return;
  end_if;

  _strcpy(#msg[0], "SRAM CONVERT - STATISTIC - converted= ");
  _itoa(iConverted, #msg[_strlen(#msg[0])]);
  _strcat(#msg[_strlen(#msg[0])], " errors= ");
  _itoa(iErrors, #msg[_strlen(#msg[0])]);
//  _strcat(#msg[_strlen(#msg[0])], " connection= ");
//  _itoa(iConnection, #msg[_strlen(#msg[0])]);
  TRACE(#msg[0]);
  LSLCLI_LoaderTracePrint(#msg[0], 15);
  DEBUGLOG1("%010d-%s", (#msg[0])$UDINT);
  
END_FUNCTION

FUNCTION CREATE_RAMFILE_DAT_MEMORY
VAR_OUTPUT
	pXfileHdr   : ^XFILEHDR;
END_VAR
VAR
	allocSize : UDINT;
END_VAR

  allocSize := XFILEHDR.adata + X_FILE_CELLS_2 * SIZEOF(XFileCell);

  pXfileHdr := OS_SSR_Malloc(allocSize)$^XFILEHDR;
  if(pXfileHdr = NIL) then
    TRACE_ERR("SRAM CONVERT - Can not allocate memory for RamFile.dat");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Can not allocate memory for RamFile.dat", 12);
    DEBUGLOG0("SRAM CONVERT - Can not allocate memory for RamFile.dat");
    return;
  end_if;

  _memset(pXfileHdr$^void, 0, allocSize);
  
END_FUNCTION

FUNCTION CALC_RAMFILE_DAT_CRC
VAR_INPUT
  pXfileHdr   : ^XFILEHDR;
END_VAR
VAR_OUTPUT
  crc : UDINT;
END_VAR
VAR
  crcDataSize : UDINT;
END_VAR

  IF pXfileHdr^.udEntries > X_FILE_CELLS THEN
    crcDataSize := sizeof(XFileCell) * X_FILE_CELLS_2;
  ELSE
    crcDataSize := sizeof(XFileCell) * X_FILE_CELLS;
  END_IF;
  crc := CRC32_Buffer(#pXfileHdr^.aData[0], crcDataSize);
  
END_FUNCTION

FUNCTION WRITE_RAMFILE_DAT_CRC
VAR_INPUT
  pXfileHdr   : ^XFILEHDR;
END_VAR
VAR
	fHandle     : DINT;
  bytes2write : UDINT;
END_VAR

  pXfileHdr^.CRC32 := CALC_RAMFILE_DAT_CRC(pXfileHdr);

  fHandle := OS_FILE_OPEN("c:\RamFile.dat", 2, ATT_CREATE);
  if(fHandle <= 0) then
    TRACE_ERR("SRAM CONVERT - Can not create C:\RamFile.dat");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Can not create C:\RamFile.dat", 12);
    DEBUGLOG0("SRAM CONVERT - Can not create C:\RamFile.dat");
    return;
  end_if;

  if pXfileHdr^.udEntries > X_FILE_CELLS then
    bytes2Write := XFILEHDR.adata + X_FILE_CELLS_2 * SIZEOF(XFileCell);
  else
    bytes2Write := XFILEHDR.adata + X_FILE_CELLS * SIZEOF(XFileCell);
  end_if;

  if(OS_FILE_WRITE(fHandle, pXfileHdr$^void, bytes2Write) <> bytes2Write) then
    TRACE_ERR("SRAM CONVERT - Can not write empty C:\RamFile.dat");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Can not write empty C:\RamFile.dat", 12);
    DEBUGLOG0("SRAM CONVERT - Can not write empty C:\RamFile.dat");
  end_if;

  OS_FILE_CLOSE(fHandle);

END_FUNCTION

FUNCTION ROUNDUP16
VAR_INPUT
  value   : UDINT;
END_VAR
VAR_OUTPUT
  ret     : UDINT;
END_VAR

  // Auf 16er Schritt aufrunden + 16 dazu. (16#1F60)
  ret := (((value + 15) / 16) * 16) + 16;
  
END_FUNCTION

FUNCTION CONVERT_2_1
VAR_INPUT
  pNewSRam      : ^UDINT;
  newSRamLength : UDINT;
  pFileMem      : ^XFILEHDR;
END_VAR
VAR
  convertOK   : BOOL;
  noEntries   : UDINT;
  iEntries    : UDINT;
  iConverted  : UDINT;
  iErrors     : UDINT;
  offsNewSRam : UDINT;
  offsRamEx   : UDINT;
  pSram       : ^UDINT;
  pRamCell    : ^XRamCell;
  pFileCell   : ^XFileCell;
  pOldRamCell : ^XRamCell;
  ramExLen    : UDINT;
  msg         : ARRAY [0..99] OF CHAR;
END_VAR

  noEntries := pFileMem^.udEntries;
  offsNewSRam := 8;   // + 8 - das udEntries und udChk - damit nicht immer um 8 korrigiert werden muss
//  offsRamEx := (((((X_RAM_CELLS + 1) * sizeof(XRamCell)) + 15) / 16) * 16) + 16;
  offsRamEx := ROUNDUP16((X_RAM_CELLS + 1) * sizeof(XRamCell));
  iEntries := 0;
  iConverted := 0;
  iErrors := 0;
  pRamCell := (pNewSRam + 8)$^XRamCell;   // + 8 - das udEntries und udChk - damit nicht immer um 8 korrigiert werden muss
  pFileCell := #pFileMem^.aData[0];
  pSRam := (_s_ram_hptr^.DataStart)$^UDINT;

  convertOK := 1;
  
  while(iEntries < noEntries) do
    // valid ?
//    if(pFileCell^.udType and 16#80000000) then
    // Hier sollte eigentlich die überprüft werden, ob die Filezelle gültig ist !!!!!
    // Warum das bei den RamEx nicht so ist, ..... ?????

      // RamEx **********************
      if((pFileCell^.udType and 16#30000000) = 16#30000000) then

        if((offsNewSRam + sizeof(pRamCell^)) >= newSRamLength) then
          convertOK := 0;
          exit;
        end_if;
        pOldRamCell := pSRam + pFileCell^.udpRelSram;
        
        pRamCell^.udCRC32 := pFileCell^.udCRC32;
        pRamCell^.udData := pOldRamCell^.udData;
        pRamCell^.udData := pRamCell^.udData and 16#FFFF0000;
        pRamCell^.udData := pRamCell^.udData or sizeof(pRamCell^);
        pRamCell^.udType := offsRamEx;
        pRamCell^.udType := pRamCell^.udType or 16#F0000000;    // id for valid, used and RAMEX
        pRamCell^.udChk := (pRamCell^.udType and 16#00FFFFFF) + pRamCell^.udData + pRamCell^.udCRC32;
        offsNewSRam += sizeof(pRamCell^);
        pRamCell += sizeof(pRamCell^);

        ramExLen := pOldRamCell^.udData shr 16;
        
        if((offsNewSRam + ramExLen) >= newSRamLength) then
          convertOK := 0;
          exit;
        end_if;
        _memcpy((pNewSRam + offsRamEx), (pSram + (pOldRamCell^.udType and 16#00FFFFFF)), ramExLen);
        offsRamEx += ramExLen;

      // Normale Ramzelle **********************
      else
        if((iEntries <> 0) & ((iEntries mod 500) = 0)) then
          // Es muss eine Verbindungszelle erstellt werden
          if((offsNewSRam + sizeof(pRamCell^)) >= newSRamLength) then
            convertOK := 0;
            exit;
          end_if;
          pRamCell^.udCRC32 := 1954;
          pRamCell^.udType := 16#80000005;
          pRamCell^.udData := ROUNDUP16(offsRamEx);
          pRamCell^.udChk := pRamCell^.udCRC32 + pRamCell^.udData;

          offsNewSRam := pRamCell^.udData;

          (pNewSRam + offsNewSRam)^ := noEntries - iEntries;
          if((pNewSRam + offsNewSRam)^ > 500) then
            (pNewSRam + offsNewSRam)^ := 500;
          end_if;
          offsNewSRam += 4;
          (pNewSRam + offsNewSRam)^ := 16#FFFFFFFF - (pNewSRam + offsNewSRam - 4)^;

          offsNewSRam += 4;
          pRamCell := (pNewSRam + offsNewSRam)$^XRamCell;
          offsRamEx := ROUNDUP16(offsNewSRam + ((X_RAM_CELLS + 1) * sizeof(XRamCell)));
        end_if;
        
        if((offsNewSRam + sizeof(pRamCell^)) >= newSRamLength) then
          convertOK := 0;
          exit;
        end_if;
        pRamCell^.udCRC32 := pFileCell^.udCRC32;
        pRamCell^.udType := pFileCell^.udType;
        pRamCell^.udData := (pSRam + pFileCell^.udpRelSram)^;
        pRamCell^.udChk := pRamCell^.udCRC32 + pRamCell^.udData;

        offsNewSRam += sizeof(pRamCell^);
        pRamCell += sizeof(pRamCell^);
        
      end_if;
      
      iConverted += 1;

    // valid kann bei RamEx nicht überprüft werden
//    else
//      iErrors += 1;
//    end_if;
    
    iEntries += 1;
    pFileCell += sizeof(pFileCell^);
  end_while;

  if(convertOK = 1) then
    // Einträge ins SRAM
    _s_ram_hptr^.version := 0; // ?????
    _s_ram_hptr^.UsedData := offsRamEx;
    _s_ram_hptr^.DataValid := 16#00010001;
    _memcpy(_s_ram_hptr^.DataStart, pNewSRam, offsRamEx);
    // Nach memcpy, da diese 8 Byte sonst überschrieben werden
    if(iConverted <= 500) then
      _s_ram_hptr^.DataStart^.udEntries := iConverted;
    else
      _s_ram_hptr^.DataStart^.udEntries := 500;
    end_if;
    _s_ram_hptr^.DataStart^.udChk := 16#FFFFFFFF - _s_ram_hptr^.DataStart^.udEntries;

  else
    TRACE_ERR("SRAM CONVERT - CONVERSION not OK");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - CONVERSION not OK", 12);
    DEBUGLOG0("SRAM CONVERT - CONVERSION not OK");
  end_if;

  _strcpy(#msg[0], "SRAM CONVERT - STATISTIC - converted= ");
  _itoa(iConverted, #msg[_strlen(#msg[0])]);
  _strcat(#msg[_strlen(#msg[0])], " errors= ");
  _itoa(iErrors, #msg[_strlen(#msg[0])]);
  TRACE(#msg[0]);
  LSLCLI_LoaderTracePrint(#msg[0], 15);
  DEBUGLOG1("%010d-%s", (#msg[0])$UDINT);

END_FUNCTION

FUNCTION READ_RAMFILE_DAT
VAR_OUTPUT
  pXfileHdr   : ^XFILEHDR;
END_VAR
VAR
	fHandle     : DINT;
  ramError    : BOOL;
	allocSize : UDINT;
END_VAR

  allocSize := XFILEHDR.adata + X_FILE_CELLS_2 * SIZEOF(XFileCell);

  ramError := FALSE;
  
  pXfileHdr := OS_SSR_Malloc(allocSize)$^XFILEHDR;
  if(pXfileHdr = NIL) then
    TRACE_ERR("SRAM CONVERT - Can not allocate memory for RamFile.dat");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Can not allocate memory for RamFile.dat", 12);
    DEBUGLOG0("SRAM CONVERT - Can not allocate memory for RamFile.dat");
    return;
  end_if;

  fHandle := OS_FILE_OPEN("c:\RamFile.dat", 2, ATT_READ_ONLY);
  if(fHandle <= 0) then
    TRACE_ERR("SRAM CONVERT - Can not open C:\RamFile.dat");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Can not open C:\RamFile.dat", 12);
    DEBUGLOG0("SRAM CONVERT - Can not open C:\RamFile.dat");
    ramError := TRUE;
  end_if;

  if(OS_FILE_READ(fHandle, pXfileHdr, allocSize) < 0) then
    TRACE_ERR("SRAM CONVERT - Can not read C:\RamFile.dat");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Can not read C:\RamFile.dat", 12);
    DEBUGLOG0("SRAM CONVERT - Can not read C:\RamFile.dat");
    ramError := TRUE;
  end_if;

  if (CALC_RAMFILE_DAT_CRC(pXfileHdr) <> pXfileHdr^.CRC32) then
    TRACE_ERR("SRAM CONVERT - Invalid CRC in C:\RamFile.dat");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Invalid CRC in C:\RamFile.dat", 12);
    DEBUGLOG0("SRAM CONVERT - Invalid CRC in C:\RamFile.dat");
    ramError := TRUE;
  end_if;

  OS_FILE_CLOSE(fHandle);
  
  if((ramError = TRUE) & (pXfileHdr <> NIL)) then
    OS_SSR_FREE(pXfileHdr);
    pXfileHdr := NIL;
  end_if;

END_FUNCTION

FUNCTION CONVERT_SRAM
VAR
  pSysenv   : ^OS_SYSENV;
  value     : ARRAY[0..1] OF CHAR;
  oldFormat : UDINT;
  newFormat : UDINT;
  pNewSRam  : ^UDINT;
  pFileMem  : ^XFILEHDR;
END_VAR

  if(_s_ram_hptr = NIL) | 
    (_s_ram_hptr^.DataStart = 0) |
    (_s_ram_hptr^.DataLength = 0) |
    ((_s_ram_hptr^.DataValid and 16#0000FFFF) <> 1) | 
    ((_s_ram_hptr^.DataValid shr 16) > 2) then
    DEBUGLOG0("SRAM CONVERT - _s_ram_hptr or version not valid");
    return;
  end_if;

  oldFormat := (_s_ram_hptr^.DataValid shr 16) and 16#00FF;
  
  OS_CILGet("SYSENV", #pSysenv$void);
  if((pSysenv <> NIL) & (OS_GETENVVAR(pSysenv, "SRAMFORMAT", #value[0], sizeof(value)))) then
    newFormat := value[0] - '0';
  else
//    TRACE("SRAM CONVERT - can't get old or new format");
//    LSLCLI_LoaderTracePrint("SRAM CONVERT - can't get old or new format", 12);
//    DEBUGLOG0("SRAM CONVERT - can't get old or new format");
    return;
  end_if;
  if _s_ram_hptr^.DataLength + 64 < 8*1024 then
    // Bei SRAM Format 1 muss auf jeden Fall der xRamHdr Platz haben, daher ist 
    // bei DataLength < 8k nur Format 2 möglich.
    newFormat := 2;
  end_if;


  // convert 1 -> 2
  if(((oldFormat = 0) | (oldFormat = 1)) & (newFormat = 2)) then
    TRACE("SRAM CONVERT - Format 1 --> 2");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Format 1 --> 2", 15);
    DEBUGLOG0("SRAM CONVERT - Format 1 --> 2");
    
    // UsedData ist in diesem Fall genug, da der SRAM Bereich kleiner wird
    pNewSRam := OS_SSR_Malloc(_s_ram_hptr^.UsedData)$^UDINT;
    if(pNewSram = NIL) then
      TRACE_ERR("SRAM CONVERT - Can not allocate memory for convert");
      LSLCLI_LoaderTracePrint("SRAM CONVERT - Can not allocate memory for convert", 12);
      DEBUGLOG0("SRAM CONVERT - Can not allocate memory for convert");
      return;
    end_if;

    pFileMem := CREATE_RAMFILE_DAT_MEMORY();
      
    if(pFileMem <> NIL) then
      if(CONVERT_1_2(pNewSRam, _s_ram_hptr^.UsedData, pFileMem) = 1) then
        WRITE_RAMFILE_DAT_CRC(pFileMem);
      end_if;
    end_if;

    if(pNewSRam <> NIL) then
      OS_SSR_FREE(pNewSRam);
    end_if;
    if(pFileMem <> NIL) then
      OS_SSR_FREE(pFileMem);
    end_if;

  // convert 2 -> 1
  elsif((newFormat = 1) & (oldFormat = 2)) then
    TRACE("SRAM CONVERT - Format 2 --> 1");
    LSLCLI_LoaderTracePrint("SRAM CONVERT - Format 2 --> 1", 15);
    DEBUGLOG0("SRAM CONVERT - Format 2 --> 1");

    pNewSRam := OS_SSR_Malloc(_s_ram_hptr^.DataLength)$^UDINT;
    if(pNewSram = NIL) then
      TRACE_ERR("SRAM CONVERT - Can not allocate memory for convert");
      LSLCLI_LoaderTracePrint("SRAM CONVERT - Can not allocate memory for convert", 12);
      DEBUGLOG0("SRAM CONVERT - Can not allocate memory for convert");
      return;
    end_if;
      
    pFileMem := READ_RAMFILE_DAT();
      
    if(pFileMem <> NIL) then
      CONVERT_2_1(pNewSRam, _s_ram_hptr^.DataLength - 16#40, pFileMem);
    end_if;

    if(pNewSRam <> NIL) then
      OS_SSR_FREE(pNewSRam);
    end_if;
    if(pFileMem <> NIL) then
      OS_SSR_FREE(pFileMem);
    end_if;
    
  end_if;

END_FUNCTION

FUNCTION MySort
VAR_INPUT
  src 	: ^USINT;
  no 	: UDINT;
  itemsize 	: UDINT;
  sortfunct 	: ^void;
END_VAR
VAR_OUTPUT
  retcode 	: BOOL;
END_VAR;

FUNCTION GLOBAL RamExSortCompare
VAR_INPUT
  p1 : ^UDINT;
  p2 : ^UDINT;
END_VAR
VAR_OUTPUT
  retcode : dint;
END_VAR

  retcode := -1;

  if(p1^ > p2^) then
    retcode := 1;
  elsif(p1^ = p2^) then
    retcode := 0;
    
    // doppelt vorhandene crc's
    TRACE_WARN("RamExCleanUp - Same CRC at two objects");
    LSLCLI_LoaderTracePrint("RamExCleanUp - Same CRC at two objects", 12);
    DEBUGLOG0("RamExCleanUp - Same CRC at two objects");
  end_if;

END_FUNCTION

FUNCTION RamExFind
VAR_INPUT
  pBuffer : ^UDINT;
  start   : UDINT;
  anz     : UDINT;
  crc     : UDINT;
END_VAR
VAR_OUTPUT
  retcode : BOOL;
END_VAR
VAR
  index   : UDINT;
  tmpcrc  : UDINT;
END_VAR
  
  retcode := false;
  
  ECONOMIZE_RECURSION :
  
  if(anz > 0) then
  
    index  := start + (anz / 2);
    tmpcrc := (pBuffer + (index * sizeof(UDINT)))^;

    if(crc < tmpcrc) then
      anz := anz / 2;
      goto ECONOMIZE_RECURSION;
    elsif (crc > tmpcrc) then
      // retcode := FindSorted(index + 1, ((anz - 1) / 2), crc);
      start := index + 1;
      anz   := ((anz - 1) / 2);
      goto ECONOMIZE_RECURSION;
    else
      retcode := true;
    end_if;
  end_if;

END_FUNCTION

FUNCTION add_hex
VAR_INPUT
  val   : ^UDINT;
  code  : CHAR;
END_VAR
VAR_OUTPUT
  ret   : BOOL;
END_VAR

  ret := TRUE;

  if((code >= '0') & (code <= '9')) then
    val^ := val^ * 16 + (code - '0');
  elsif((code >= 'A') & (code <= 'F')) then
    val^ := val^ * 16 + (code - 'A' + 10);
  elsif((code >= 'a') & (code <= 'f')) then
    val^ := val^ * 16 + (code - 'a' + 10);
  else
    ret := FALSE;
  end_if;

END_FUNCTION

FUNCTION HtoD
VAR_INPUT
  pa          : ^CHAR;
  chkPrefix   : BOOL;
END_VAR
VAR_OUTPUT
  value   : UDINT;
END_VAR
VAR
  i     : UDINT;
  no    : UDINT;
END_VAR

  value := 0;

  if(chkPrefix = TRUE) then
    if(_memcmp(pa, "16#", 3) = 0) then
      pa += 3;
    elsif(_memcmp(pa, "0x", 2) = 0) then
      pa += 2;
    end_if;
  end_if;
  
  no := _strlen(pa);
  i := 0;
  while(i < no) do
    if(add_hex(#value, (pa + i)^) = FALSE) then
      exit;
    end_if;
    
    i += 1;
  end_while;
  
END_FUNCTION

FUNCTION GLOBAL RamExCleanUp
VAR
  fHandle   : DINT;
  pIndex    : ^UDINT;
  indexNo   : DINT;
  tmp       : ARRAY[0..49] of CHAR;
  fileName  : ARRAY[0..49] of CHAR;
  ddeInfo   : _DDE_INFO;
  hFirst    : DINT;
  hNext     : DINT;
END_VAR

  if !!hasSram then
    return;
  end_if;
  if (GetSramReorgFlag() = FALSE) then
    return;
  end_if;
  
  pIndex := NIL;
  
  fHandle := OS_FILE_OPEN(RAMEXINDEXFILE, 2, ATT_READ_ONLY);
  if(fHandle <= 0) then
//    TRACE("RamExCleanUp - Cannot open Indexfile");
//    LSLCLI_LoaderTracePrint("RamExCleanUp - Can not open Indexfile", 12);
//    DEBUGLOG0("RamExCleanUp - Can not open Indexfile");
    goto RamExCleanUp_ERROR;
  end_if;

  indexNo := OS_FILE_LENGTH(fHandle);
  if(indexNo <= 0) then
    TRACE_ERR("RamExCleanUp - Can not get file length");
    LSLCLI_LoaderTracePrint("RamExCleanUp - Can not get file length", 12);
    DEBUGLOG0("RamExCleanUp - Can not get file length");
    goto RamExCleanUp_ERROR;
  end_if;
  
  pIndex := OS_SSR_Malloc(indexNo$UDINT)$^UDINT;
  if(pIndex = NIL) then
    TRACE_ERR("RamExCleanUp - Can not allocate memory");
    LSLCLI_LoaderTracePrint("RamExCleanUp - Can not allocate memeory", 12);
    DEBUGLOG0("RamExCleanUp - Can not allocate memory");
    goto RamExCleanUp_ERROR;
  end_if;

  if(OS_FILE_READ(fHandle, pIndex, indexNo$UDINT) <> indexNo$UDINT) then
    TRACE_ERR("RamExCleanUp - Can not read Indexfile");
    LSLCLI_LoaderTracePrint("RamExCleanUp - Can not read Indexfile", 12);
    DEBUGLOG0("RamExCleanUp - Can not read Indexfile");
    goto RamExCleanUp_ERROR;
  end_if;

  indexNo /= sizeof(UDINT);
  
  if(MySort(pIndex$^USINT, indexNo$UDINT, sizeof(UDINT), #RamExSortCompare()) = false) then
    TRACE_ERR("RamExCleanUp - Can not sort");
    LSLCLI_LoaderTracePrint("RamExCleanUp - Can not sort", 12);
    DEBUGLOG0("RamExCleanUp - Can not sort");
    goto RamExCleanUp_ERROR;
  end_if;

  _strcpy(#tmp[0], RAMEXFILES);
  _strcat(#tmp[0], "*.*");

  hFirst := OS_FILE_FINDFIRSTEX(#tmp[0], 0, 0, #ddeInfo, #fileName[0], sizeof(fileName));
  hNext := 1;
  
  while((hFirst >= 0) & (hNext >= 0)) do

    if(ddeInfo.Attributes <> ATTR_DIR) then
      if(RamExFind(pIndex, 0, indexNo$UDINT, HtoD(#fileName[0], FALSE)) = FALSE) then
        _strcpy(#tmp[0], RAMEXFILES);
        _strcat(#tmp[0], #fileName[0]);
        OS_FILE_DELETE(#tmp[0]);
      end_if;
    end_if;
    
    hNext := OS_FILE_FINDNEXTEX(hFirst, #ddeInfo, #fileName[0], sizeof(fileName));
  end_while;
  
  OS_FILE_FINDCLOSE(hFirst);

RamExCleanUp_ERROR:
  if(fHandle >= 0) then
    OS_FILE_CLOSE(fHandle);
  end_if;
  if(pIndex <> NIL) then
    OS_SSR_FREE(pIndex);
  end_if;

END_FUNCTION

//------------------------------------------------------------------------
// Den SRAM Inhalt (samt RAMFILE.DAT) in die Files C:\LSLDATA\SRAM.CPY 
// und C:\LSLDATA\RAMFILE.CPY kopieren (fürs Reorg des SRAMs).
//------------------------------------------------------------------------
FUNCTION MakeSramKopie
VAR_INPUT
	pErrCode : ^DINT;
END_VAR
VAR_OUTPUT
	retVal : DINT;
END_VAR
VAR
	handle : DINT;
	rc     : DINT;
	mramDescr : MRAM_DESCR;
	string : ^CHAR;
	dataLen : UDINT;
	fileSize : UDINT;
	aXFileHdr_Hdr : XFILEHDR_HDR;
	bRamfileWriteNeeded : BOOL;
END_VAR

	retVal := -1;

	OS_FILE_CREATEDIR(SRAM_KOPIE_PFAD);

	handle := OS_FILE_OPEN(SRAM_KOPIE_SRAM_FNAME, 2, ATT_CREATE);
	IF handle <= 0 THEN
		pErrCode^ := handle;
		retVal := -2;
		RETURN;
	END_IF;
	
	//
	// SRAM-Speicher kopieren
	//

	//	Erst einen Header mit valid=0 schreiben und am Ende Header mit valid=1
	_memcpy(#mramDescr, _s_ram_hptr, SIZEOF(mramDescr));
	mramDescr.DataValid$UINT := 0;
	// GET_USED_S_RAM zählt zu UsedData den Unterschied zw. DataStart und HeaderAdresse hinzu.
	// Normalerweise ist das die Headergröße aber nur wenn die Daten auf den Header folgen.
	// Davon gehe ich aber hier einmal nicht aus, daher folgende Korrektur:
	// 1.Headergröße vom SRAM abziehen
	// 2.Headergröße im File dazuzählen
	mramDescr.UsedData -= (_S_RAM_Hptr^.DataStart$UDINT - _S_RAM_Hptr$UDINT);
	mramDescr.UsedData += SIZEOF(mramDescr);
	mramDescr.DataStart$UDINT := SIZEOF(mramDescr);	// rel. Fileofset für die folgenden Daten
	rc := FileWrite(handle, #mramDescr, sizeof(mramDescr));
	IF rc <> sizeof(mramDescr) THEN
		OS_FILE_CLOSE(handle);
		pErrCode^ := rc$DINT;
		retVal := -3;
		RETURN;
	ELSE
		string := SRAM_KOPIE_SRAM_FNAME;
		DEBUGLOG2("%010d-MakeSramKopie: header written to %s (%d bytes)", string$UDINT, rc$UDINT);
	END_IF;

	dataLen := mramDescr.UsedData - SIZEOF(mramDescr);
	rc := FileWrite(handle, _S_RAM_Hptr^.DataStart, dataLen);
	IF rc <> dataLen$DINT THEN
		OS_FILE_CLOSE(handle);
		pErrCode^ := rc$DINT;
		retVal := -4;
		RETURN;
	ELSE
		string := SRAM_KOPIE_SRAM_FNAME;
		DEBUGLOG2("%010d-MakeSramKopie: data written to %s (%d bytes)", string$UDINT, rc$UDINT);
	END_IF;

	OS_FILE_CLOSE(handle);

	//
	// RAMFILE.DAT kopieren
	//
	IF isRamfileDatUsed THEN

		// Vorerst das File öffnen und die Checksumme und udEntries im File mit der 
		// Checksumme und udEntries im Speicher vergleichen. Wenn die Werte gleich 
		// sind, dann erspart man sich einen Schreibvorgang.
		bRamfileWriteNeeded := TRUE;
		handle := OS_FILE_OPEN(SRAM_KOPIE_RAMFILE_FNAME, 0, 0);		// 0 = read only
		IF handle > 0 THEN
			rc := OS_FILE_READ(handle, #aXFileHdr_Hdr, SIZEOF(aXFileHdr_Hdr))$DINT;
			IF rc = SIZEOF(aXFileHdr_Hdr) THEN
				IF (aXFileHdr_Hdr.CRC32 = pRamFile^.CRC32) & (aXFileHdr_Hdr.udEntries = pRamFile^.udEntries) THEN
					bRamfileWriteNeeded := FALSE;
				END_IF;
			END_IF;
			OS_FILE_CLOSE(handle);
		END_IF;

		IF bRamfileWriteNeeded THEN
			IF pRamFile^.udEntries > X_FILE_CELLS_2 THEN
				DEBUGLOG1("%010d-MakeSramKopie: invalid number of XFILEHDR.udEntries (%d)", pRamFile^.udEntries);
				retVal := -9;
				RETURN;
			END_IF;
			fileSize := XFILEHDR.adata + pRamFile^.udEntries * SIZEOF(XFileCell);

			handle := OS_FILE_OPEN(SRAM_KOPIE_RAMFILE_FNAME, 2, ATT_CREATE);
			IF handle <= 0 THEN
				pErrCode^ := handle;
				retVal := -5;
				RETURN;
			END_IF;
			rc := FileWrite(handle, pRamFile, fileSize);
			IF rc <> fileSize$DINT THEN
				OS_FILE_CLOSE(handle);
				pErrCode^ := rc$DINT;
				retVal := -6;
				RETURN;
			ELSE
				string := SRAM_KOPIE_RAMFILE_FNAME;
				DEBUGLOG3("%010d-MakeSramKopie: data written to %s (%d bytes, %d entries)", string$UDINT, rc$UDINT, pRamFile^.udEntries);
			END_IF;
			OS_FILE_CLOSE(handle);
		ELSE
			string := SRAM_KOPIE_RAMFILE_FNAME;
			DEBUGLOG3("%010d-MakeSramKopie: no need to write to %s (crc32=0x%08X and udEntries=%d are still the same)", string$UDINT, pRamFile^.CRC32, pRamfile^.udEntries);
		END_IF;
	END_IF;

	//
	// Jetzt werden die Files gültig gemacht, indem der Header mit DataValid=1 geschrieben wird
	//
	mramDescr.DataValid$UINT := 1;
	handle := OS_FILE_OPEN(SRAM_KOPIE_SRAM_FNAME, 2, ATT_READ_WRITE); 	// 2 = use flags in parameter 3
	IF handle <= 0 THEN
		pErrCode^ := handle;
		retVal := -7;
		RETURN;
	END_IF;
	
	rc := FileWrite(handle, #mramDescr, SIZEOF(mramDescr));
	IF rc <> SIZEOF(mramDescr) THEN
		OS_FILE_CLOSE(handle);
		pErrCode^ := rc$DINT;
		retVal := -8;
		RETURN;
	ELSE
		string := SRAM_KOPIE_SRAM_FNAME;
		DEBUGLOG2("%010d-MakeSramKopie: header written to %s (%d bytes)", string$UDINT, rc$UDINT);
	END_IF;

	OS_FILE_CLOSE(handle);

	retVal := 0;

END_FUNCTION

//------------------------------------------------------------------------
// Allokiert Speicher für einen MRAM_DESCR. Die Größe des allokierten Bereichs 
// wird in MRAM_DESCR.DataLength gemerkt.
//------------------------------------------------------------------------
FUNCTION ReAlloc_S_RAM_Hptr_File
VAR_INPUT
	pMramDescr : ^MRAM_DESCR;
	udSize : UDINT;
END_VAR
VAR_OUTPUT
	pMramDescr_new : ^MRAM_DESCR;
END_VAR
VAR
	nAlloc : UDINT;
END_VAR

  IF pMramDescr <> NIL & udSize <= pMramDescr^.DataLength THEN
    // Speicher ist bereits groß genug
    pMramDescr_new := pMramDescr;
    RETURN;
  END_IF;

  (* Wenn immer ein größerer Wert als zuvor allokiert wird, 
     kann der Heap sehr stark fragmentiert werden. Der ungünstige Fall tritt 
     dann auf, wenn nach dem realloc ein weiterer Block allokiert wird und 
     dieser nach dem realloc-Block liegt. Es entstehen dann lauter nicht 
     zusammenhängende freie Blöcke, wenn der Heap-Algorithmus nicht optimal 
     arbeitet (z.B. kein Best-Fit oder kein Compact nach einem free). 
     Um das zu verhindern, wird die Blockgröße aufgerundet (nur wenn genug frei ist). 
  *)
  nAlloc := RoundUpPow2(udSize);
  IF nAlloc > _UserHeapFreeMem / 8 THEN
    nAlloc := udSize;
  END_IF;

  IF pMramDescr = NIL THEN
    // Speicher wird das erste mal allokiert
    pMramDescr_new := _alloc_resize_LDR(nAlloc)$^MRAM_DESCR;
  ELSE
    // Speicher muss vergrößert werden
    pMramDescr_new$^void := _realloc_LDR(pMramDescr$^void, nAlloc);
  END_IF;
  
  IF pMramDescr_new <> NIL THEN
    pMramDescr_new^.DataLength := nAlloc;
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------
// Liest die für das SRAM Reorg erstellte Kopie des SRAM Inhalts ein
//------------------------------------------------------------------------
FUNCTION LoadSramKopie
VAR_INPUT
	pErrCode : ^DINT;
  fhandle   : dint;
  usProbe   : usint;
END_VAR
VAR_OUTPUT
	retVal : DINT;
END_VAR
VAR
	dataOfs : UDINT;
	dataLen : UDINT;
	handle : DINT;
	rc     : UDINT;
	string : ^CHAR;
	udEntries : UDINT;
	fileSize : UDINT;
	kopieSize : UDINT;
  sramImage  : ^CHAR;
  ramfileDat : ^CHAR;
  loadINIfiles : BOOL;
  DataLengthSave : UDINT;
END_VAR


#ifdef LDR_RETSVR
  // Sollen die SRAM Daten aus dem File für die remanenten Server geladen werden?
  if( fhandle = NIL )then
    retVal := -1;
#endif

  loadINIfiles := FALSE;
  
	IF _S_RAM_Hptr_Kopie = NIL THEN

    loadINIfiles := FALSE;
    
		_S_RAM_Hptr_Kopie := _alloc_resize_LDR(SIZEOF(_S_RAM_Hptr_Kopie^))$^MRAM_DESCR;  
		IF _S_RAM_Hptr_Kopie = NIL THEN
			retVal := -2;
			RETURN;
		END_IF;

    // Gibt es ein SRAM.INI File
		handle := OS_FILE_OPEN(SRAM_INIT_SRAM_FNAME, 0, 0); 	// 0 = read only
    if(handle > 0) then
      sramImage := SRAM_INIT_SRAM_FNAME;
      ramfileDat := SRAM_INIT_RAMFILE_FNAME;
      loadINIfiles := TRUE;
      TRACE("LoadSramKopie - Try to load INI files");
      LSLCLI_LoaderTracePrint("LoadSramKopie - Try to load INI files", 15);
    else
  		handle := OS_FILE_OPEN(SRAM_KOPIE_SRAM_FNAME, 0, 0); 	// 0 = read only
      sramImage := SRAM_KOPIE_SRAM_FNAME;
      ramfileDat := SRAM_KOPIE_RAMFILE_FNAME;
    end_if;
		IF handle <= 0 THEN
			retVal := -3;
			RETURN;
		END_IF;
		rc := OS_FILE_READ(handle, _S_RAM_Hptr_Kopie, SIZEOF(_S_RAM_Hptr_Kopie^));
		IF rc <> SIZEOF(_S_RAM_Hptr_Kopie^) THEN
			OS_FILE_CLOSE(handle);
			retVal := -4;
			RETURN;
		ELSE
			string := sramImage;
			DEBUGLOG2("%010d-LoadSramKopie: header read from %s (%d bytes)", string$UDINT, rc$UDINT);
		END_IF;

		IF !!_S_RAM_Hptr_Kopie^.DataValid$UINT THEN
			// Die Kopie ist nicht gültig
			OS_FILE_CLOSE(handle);
			retVal := -5;
			DEBUGLOG0("%010d-LoadSramKopie: header is not valid !");
			RETURN;
		END_IF;

		dataOfs := _S_RAM_Hptr_Kopie^.DataStart$UDINT;
		dataLen := _S_RAM_Hptr_Kopie^.UsedData - _S_RAM_Hptr_Kopie^.DataStart$UDINT;

		(*
			Anm:
			Die Grösse des für die Kopie reservierten Heap-Speichers hat Auswirkungen 
			auf die Adressen der Objekte, die danach allokiert werden. Je grösser der 
			Kopie-Speicher ist, umso weiter hinten werden die nachfolgenden Objekt angeordnet.

			Das hat den unangenehmen Nachteil, dass nach einer SRAM Grössenänderung beim 
			nächsten Hochlauf die Lasal-IDs (=Objektadressen) anders sind und die Visualisierung 
			die Lasal-IDs neu anfordern muss. Das ist meistens sehr zeitaufwendig.

			Vorgehensweise, damit die alloc-Size des Kopie-Speichers sich nicht dauernd ändert:
			Grosse CPUs: Es wird immer das Maximum allokiert
			Kleine CPUs: Die Grösse des Kopie-Speichers wird aufgerundet
			Auf den Gesamtspeicherverbrauch hat das keine Auswirkung, weil der Kopie-Speicher 
			am Ende der Loaderinitialisierung wieder freigeben wird.
		 *)
		IF _UserHeapFreeMem > 4 * 1024 * 1024 THEN
			kopieSize := _S_RAM_Hptr^.DataLength;
		ELSE
			kopieSize := sizeof(_S_RAM_Hptr_Kopie^) + dataLen;
			IF (kopieSize < _S_RAM_Hptr^.DataLength & (_S_RAM_Hptr^.DataLength - kopieSize) < SRAM_KOPIE_BLOCKSIZE) THEN
				kopieSize := _S_RAM_Hptr^.DataLength;
			ELSE
				kopieSize := kopieSize + SRAM_KOPIE_BLOCKSIZE - (kopieSize MOD SRAM_KOPIE_BLOCKSIZE);
			END_IF;
		END_IF;

    // DB1729
    IF kopieSize < sizeof(_S_RAM_Hptr_Kopie^) + dataLen THEN
      kopieSize := sizeof(_S_RAM_Hptr_Kopie^) + dataLen;
    END_IF;

		_S_RAM_Hptr_Kopie$pVoid := _realloc_LDR(_S_RAM_Hptr_Kopie$pVoid, kopieSize);
		IF _S_RAM_Hptr_Kopie = NIL THEN
			OS_FILE_CLOSE(handle);
			retVal := -6;
			RETURN;
		END_IF;

		rc := OS_FILE_SEEK(handle, dataOfs, 0);
		IF rc <> dataOfs THEN
			OS_FILE_CLOSE(handle);
			retVal := -7;
			RETURN;
		END_IF;

		_S_RAM_Hptr_Kopie^.DataStart$UDINT := _S_RAM_Hptr_Kopie$UDINT + sizeof(_S_RAM_Hptr_Kopie^);

		rc := OS_FILE_READ(handle, _S_RAM_Hptr_Kopie^.DataStart, dataLen);
		IF rc <> dataLen THEN
			OS_FILE_CLOSE(handle);
			retVal := -8;
			RETURN;
		ELSE
			string := sramImage;
			DEBUGLOG2("%010d-LoadSramKopie: data read from %s (%d bytes)", string$UDINT, rc$UDINT);
		END_IF;
		OS_FILE_CLOSE(handle);
	END_IF;

	IF isRamfileDatUsed THEN
		IF pRamFile_Kopie = NIL THEN

			handle := OS_FILE_OPEN(ramfileDat, 0, 0);		// 0 = read only
			IF handle <= 0 THEN
				retVal := -10;
				RETURN;
			END_IF;

			rc := OS_FILE_READ(handle, #udEntries, SIZEOF(udEntries));
			IF rc <> SIZEOF(udEntries) THEN
				OS_FILE_CLOSE(handle);
				retVal := -11;
				RETURN;
			END_IF;
			IF udEntries > X_FILE_CELLS_2 THEN
				OS_FILE_CLOSE(handle);
				retVal := -15;
				RETURN;
			END_IF;

			string := ramfileDat;

			fileSize := XFILEHDR.adata + udEntries * SIZEOF(XFileCell);

			pRamFile_Kopie := _alloc_resize_LDR(fileSize)$^XFILEHDR;  
			IF pRamFile_Kopie = NIL THEN
				retVal := -12;
				RETURN;
			END_IF;

			rc := OS_FILE_SEEK(handle, 0, 0);
			IF rc <> 0 THEN
				OS_FILE_CLOSE(handle);
				retVal := -13;
				RETURN;
			END_IF;

			rc := OS_FILE_READ(handle, pRamFile_Kopie, fileSize);
			IF rc <> fileSize THEN
				OS_FILE_CLOSE(handle);
				retVal := -14;
				RETURN;
			ELSE
				string := ramfileDat;
				DEBUGLOG3("%010d-LoadSramKopie: data read from %s (%d bytes, %d entries)", string$UDINT, rc$UDINT, udEntries);
			END_IF;
			OS_FILE_CLOSE(handle);

		END_IF;
	END_IF;

  if(loadINIfiles = TRUE) then
    OS_FILE_DELETE(SRAM_INIT_SRAM_FNAME_DONE);
    OS_FILE_DELETE(SRAM_INIT_RAMFILE_FNAME_DONE);
    OS_FILE_RENAME(SRAM_INIT_SRAM_FNAME, SRAM_INIT_SRAM_FNAME_DONE);
    OS_FILE_RENAME(SRAM_INIT_RAMFILE_FNAME, SRAM_INIT_RAMFILE_FNAME_DONE);
  end_if;
  
	retVal := 0;

#ifdef LDR_RETSVR
  else
  // es handelt sich um remanente Server
    retVal := -1;
  
    // Wenn die Daten noch nicht geladen wurden...
    IF _S_RAM_Hptr_File = NIL THEN
  
      // Anlegen des Fileimages
      _S_RAM_Hptr_File := ReAlloc_S_RAM_Hptr_File(NIL, SIZEOF(_S_RAM_Hptr_File^));
      IF _S_RAM_Hptr_File = NIL THEN
        retVal := -2;
        RETURN;
      END_IF;
      
      // Vorbereitung des Reorg. Images
      _S_RAM_Hptr_File_New := ReAlloc_S_RAM_Hptr_File(NIL, SIZEOF(_S_RAM_Hptr_File_New^));
      IF _S_RAM_Hptr_File_New = NIL THEN
        retVal := -2;
        RETURN;
      END_IF;
      
      // gültiger Header für das Reorg. Image
      _S_RAM_Hptr_File_New^.version     := 0;
      _S_RAM_Hptr_File_New^.DataStart   := 20;
      _S_RAM_Hptr_File_New^.UsedData    := 20;
      _S_RAM_Hptr_File_New^.DataValid   := 1;
      
      // existiert das Filehandle?
      IF fhandle <= 0 THEN
        retVal := -3;
        RETURN;
      END_IF;
      
      // Fileheader in das Image laden
      DataLengthSave := _S_RAM_Hptr_File^.DataLength; // In Datalength ist die allokierte Größe gespeichert, 
                                                      // daher sichern und nach dem File-lesen wiederherstellen
      rc := OS_FILE_READ(fhandle, _S_RAM_Hptr_File, SIZEOF(_S_RAM_Hptr_File^));
      IF rc <> SIZEOF(_S_RAM_Hptr_File^) THEN
        retVal := -4;
        RETURN;
      END_IF;
      _S_RAM_Hptr_File^.DataLength := DataLengthSave;
  
      // Prüfen ob gültig
      IF !!_S_RAM_Hptr_File^.DataValid$UINT THEN
        // Die Kopie ist nicht gültig
        retVal := -5;
        RETURN;
      END_IF;
  
      // Länge ermitteln
      dataOfs := _S_RAM_Hptr_File^.DataStart$UDINT;
      dataLen := _S_RAM_Hptr_File^.UsedData - _S_RAM_Hptr_File^.DataStart$UDINT;
  
      // den Rest des Files in das Image laden
      _S_RAM_Hptr_File$pVoid := ReAlloc_S_RAM_Hptr_File(_S_RAM_Hptr_File, sizeof(_S_RAM_Hptr_File^) + dataLen );
      IF _S_RAM_Hptr_File = NIL THEN
        retVal := -6;
        RETURN;
      END_IF;
  
      rc := OS_FILE_SEEK(fhandle, dataOfs, 0);
      IF rc <> dataOfs THEN
        retVal := -7;
        RETURN;
      END_IF;
  
      DataLengthSave := _S_RAM_Hptr_File^.DataLength; // In Datalength ist die allokierte Größe gespeichert, 
                                                      // daher sichern und nach dem File-lesen wiederherstellen
      rc := OS_FILE_READ(fhandle, ( _S_RAM_Hptr_File$udint + dataOfs )$^void, dataLen);
      IF rc <> dataLen THEN
        retVal := -8;
        RETURN;
      END_IF;
      _S_RAM_Hptr_File^.DataLength := DataLengthSave;
      
    END_IF;
    
    // wenn nur probiert wurde, dann aufräumen!
    if( usProbe = 1 )then
      if( _S_RAM_Hptr_File <> NIL )then
        _free( _S_RAM_Hptr_File );
        _S_RAM_Hptr_File := NIL;
      end_if;
      if( _S_RAM_Hptr_File_New <> NIL )then
        _free( _S_RAM_Hptr_File_New );
        _S_RAM_Hptr_File_New := NIL;
      end_if;
      OS_FILE_SEEK( fhandle, 0, 0 );
    end_if;
  
    retVal := 0;  
  end_if;
#endif //LDR_RETSVR

END_FUNCTION
//------------------------------------------------------------------------
// Gibt die belegten Resourcen der SRAM Kopie frei
//------------------------------------------------------------------------
FUNCTION GLOBAL LDR_FreeSramKopie

  IF !!hasSram THEN
    RETURN;
  END_IF;

	IF _S_RAM_Hptr_Kopie THEN
		_free(_S_RAM_Hptr_Kopie);
		_S_RAM_Hptr_Kopie := NIL;
	END_IF;

	IF pRamFile_Kopie THEN
		_free(pRamFile_Kopie);
		pRamFile_Kopie := NIL;
	END_IF;

	fSramInitCompleted := 1;
	IF fReorgSram THEN
		// damit wird das SRAM gültig, sobald GET_USED_S_RAM aufgerufen wird
		fReorgSram := 0;
	END_IF;
  // pr, 2.2.43:
  //  GET_USED_S_RAM wird hier in jedem Fall aufgerufen, da es in 
  //  Add_sRamFile_I nicht aufgerufen wird, auch wenn SRAM allokiert worden 
  //  ist. Aus Performancegründen ist es in die LDR_FreeSramKopie verlagert 
  //  worden. Es wird damit 1x am Ende aufgerufen.
  GET_USED_S_RAM();

	IF _s_ram_hptr^.DataValid$UINT <> 0 THEN
		DEBUGLOG0("%010d sram is now valid");
	ELSE
		DEBUGLOG0("%010d sram is still invalid");
	END_IF;

	DEBUGLOG3("%010d sram-cells - found in copy:%d, found in sram:%d, not found: %d", nbrOfCells_foundInCopy, nbrOfCells_foundInSram, nbrOfCells_notFound);

END_FUNCTION

//------------------------------------------------------------------------
// Löscht die Files C:\LSLDATA\SRAM.CPY und C:\LSLDATA\RAMFILE.CPY 
// (werden fürs Reorg des SRAMs verwendet).
//------------------------------------------------------------------------
FUNCTION DeleteSramKopie

  OS_FILE_DELETE(SRAM_KOPIE_SRAM_FNAME);
  OS_FILE_DELETE(SRAM_KOPIE_RAMFILE_FNAME);

END_FUNCTION

FUNCTION IsVortex
VAR_OUTPUT
  retcode   : BOOL;
END_VAR

  retcode := FALSE;
  
  if((_WhoAmI = DESTPLC_ETVEDGE) |
     (_WhoAmI = DESTPLC_CCP511)) then

    retcode := TRUE;
  end_if;
  
END_FUNCTION

// Gibt TRUE zurück, wenn die Sramdisk beim Powerdown nicht geschrieben wurde.
// Auf Plattformen ohne Sramdisk (!ETVEdge) oder bei Betriebssystemversionen mit einer zu 
// alten Version des SRAMDISK CILs wird immer FALSE zurückgegeben.
// Zusätzlich wird im Fehlerfall das OS-Flag DIAG_FLAG_NOT_WRITTEN_AT_POWER_DOWN 
// zurückgesetzt, damit nach einem Reset+Run der Fehler nicht noch einmal angezeigt wird.
FUNCTION SramdiskNotWrittenAtPowerDown
VAR_OUTPUT
  result : BOOL;
END_VAR
VAR 
  pSrd : ^OS_SRAMDISK;
  pSysenv : ^OS_SYSENV;
  value : ARRAY[0..9] OF CHAR;
  noSramPowerdownCheck : BOOL;
END_VAR

  result := FALSE;

  (* Wenn die Umgebungsvariable NO_SRAM_POWERDOWN_CHECK existiert und einen Wert 
     ungleich "0" aufweist, dann wird die Prüfung, ob das Sram beim Powerdown 
     geschrieben wurde, übersprungen.
  *)
  noSramPowerdownCheck := FALSE;
  OS_CILGet("SYSENV", #pSysenv$void);
  IF pSysenv & OS_GETENVVAR(pSysenv, "NO_SRAM_POWERDOWN_CHECK", #value[0], SIZEOF(value)) THEN
    IF _strcmp(#value[0], "0") THEN
      noSramPowerdownCheck := TRUE;
    END_IF;
  END_IF;

  IF !!noSramPowerdownCheck THEN
    OS_CILGet("SRAMDISK", #pSrd$void);
    IF (pSrd <> NIL) & (pSrd^.udVersion >= 3) THEN
      IF pSrd^.diagFlags AND DIAG_FLAG_NOT_WRITTEN_AT_POWER_DOWN THEN
        // Fehlerflag in der OS-Struktur zurücksetzen, damit der Fehler beim 
        // nächsten Reset+Run nicht noch einmal angezeigt wird.
        pSrd^.diagFlags := pSrd^.diagFlags AND (16#FFFFFFFF XOR DIAG_FLAG_NOT_WRITTEN_AT_POWER_DOWN);
        result := TRUE;
      END_IF;
    END_IF;
  END_IF;

END_FUNCTION

//------------------------------------------------------------------------
// Das ist die Funktion, die das SRAM initialisiert, nachdem der ganze 
// Konvertierungsmist durchlaufen wurde.
//------------------------------------------------------------------------
FUNCTION S_RAM_INIT_I
VAR
	wasValid : UDINT;
	errCode : DINT;
	rc : DINT;
  fDoNotReportErrors : UDINT;
END_VAR

	IF !!HasRetentiveSram() THEN
    _s_ram_hptr^.UsedData := 0;
    _s_ram_hptr^.DataStart^.udEntries := 0;
    _s_ram_hptr^.DataStart^.udChk:=NOT _s_ram_hptr^.DataStart^.udEntries;
    _s_ram_hptr^.DataValid := 1;
  END_IF;
  
  IF SramdiskNotWrittenAtPowerDown() THEN
		DEBUGLOG0("%010d error: sramdisk was not written at power-down");
    _s_ram_hptr^.DataValid$UINT := 0;
    sramErrorCode := EC_SRAMDISK_NOT_WRITTEN_AT_POWER_DOWN;
    LogSramError();
  END_IF;

	IF isRamfileDatUsed THEN
		// Das RAMFILE.DAT auf pRamFile einlesen
		IF SramLoadFile() <> 0 THEN
			_s_ram_hptr^.DataValid$UINT := 0;
			sramErrorCode := EC_LOADFILE_ERR;
			LogSramError();
		ELSIF IsVortex() = FALSE & WasRemoveableMediaChanged() THEN
			DEBUGLOG0("%010d error: detected media change, ramfile.dat not usable, marking sram as invalid");
			_s_ram_hptr^.DataValid$UINT := 0;
		END_IF;
	END_IF;

  fDoNotReportErrors := 0;
  IF GetSramReorgFlag() & ((_s_ram_hptr^.DataValid AND 16#FF000000) = 16#80000000) THEN
    // Wenn das Reorg-Flag im DataValid Feld gesetzt ist, dann heißt das, daß 
    // ein Reorg Vorgang unterbrochen wurde, nachdem eine File Kopie angelegt 
    // wurde. In so einem Fall wird keine Fehlermeldung generiert.
    fDoNotReportErrors := 1;
  END_IF;
  
	IF GetSramReorgFlag() THEN
		fReorgSram := 1;
	END_IF;
	wasValid := CHK_S_RAM(fDoNotReportErrors);
	IF GetSramReorgFlag() THEN
		IF wasValid THEN

			// gültigen SRAM Inhalt auf Files kopieren
			rc := MakeSramKopie(#errCode);
			IF rc = 0 THEN

				// Der SRAM Inhalt befindet jetzt als Kopie in den Files.
				// Es wird jetzt als ungültig gekennzeichnet und durch den Aufruf von CHK_S_RAM 
				// wird das SRAM noch einmal mit 0 initialisiert.
				DEBUGLOG0("%010d MakeSramKopie succeeded, marking sram as invalid");
        
				_s_ram_hptr^.DataValid$UINT := 0;
        // Das Reorg-Flag setzen. Wenn dieses Flag gesetz ist, dann heisst das, daß 
        // das SRAM zwar ungültig ist, aber eine gültige Kopie im File existiert.
        // In diesem Fall (ungültige SRAM, gültige Kopie im File) wird kein SRAM 
        // Fehler gemeldet.
        _s_ram_hptr^.DataValid := (_s_ram_hptr^.DataValid AND 16#00FFFFFF) OR 16#80000000; 
        
        // Der folgende Aufruf von CHK_S_RAM bewirkt, daß das SRAM neu initilisiert 
        // wird (weil DataValud 0 ist).
				fReorgSram := 1;
				CHK_S_RAM(fDoNotReportErrors := 1);
        // Reorg-Flag muss hier noch einmal gesetzt werden, da CHK_S_RAM den 
        // ganzen Header auf 0 setzt.
        _s_ram_hptr^.DataValid := (_s_ram_hptr^.DataValid AND 16#00FFFFFF) OR 16#80000000; 

			ELSE
				DEBUGLOG2("%010d MakeSramKopie failed, rc=%d, errCode = 0x%X" , rc$UDINT, errCode$UDINT);
			END_IF;

		//ELSIF !!WasRemoveableMediaChanged() THEN
    ELSE
			_s_ram_hptr^.DataValid$UINT := 0;
            // Der folgende Aufruf von CHK_S_RAM bewirkt, daß das SRAM neu initilisiert 
            // wird (weil DataValid 0 ist). Das ist notwendig, damit in der MAT die 
			// durch SRAM_MARKMemory belegten Bereiche wieder freigegeben werden.
			// (SRAM_MARKMemory wurde in CHK_S_RAM aufgerufen, um die vorhandenen 
			// SRAM Einträge als belegt zu markieren).
			fReorgSram := 1;
			CHK_S_RAM(fDoNotReportErrors := 1);

			// Das SRAM war ungültig, durch das Setzen des Flags fReorgSram wird 
			// versucht, auf Daten von einer früheren Kopie zurückzugreifen.
			fReorgSram := 1;
			DEBUGLOG0("%010d MakeSramKopie: no copy created, because sram is not valid");
		END_IF;
	END_IF;

	CLEAR_LASTENTRY(); 

END_FUNCTION

//------------------------------------------------------------------------
// S-Ram Initialisieren und ggf alte formate konvertieren (aus active.dat)
//-------------------------------------------------------------------------
// Bei aufruf hat das BS schon in  "_S_RAM_Hptr"  einen pointer auf den S-Ram-Descriptor 
// Descriptor eingetragen (oder auch nicht. ein altes system hat dort 0 stehen
//
// Logik : a) _S_RAM_Hptr hat einen gültigen pointer.
//				Wenn der eintrag genutzte länge > 0 ist gelten diese daten
//				Wenn der eintrag genutzte länge = 0 ist, suchen wir eine Active.dat
//			
// 06.03.04
// wenn C-IPC konvertieren auf neues format
// es stehen nur noch die Daten im Ram der Rest ist in einem File 
// so sparen wir pro eintrag 12 Byte !!!
//
//


FUNCTION S_RAM_INIT

VAR_OUTPUT
	ret_code 		: CONFSTATES;
END_VAR


VAR
	ActiveDatHandle  :DINT; 
	DateiKopf     : DatKopf;
	OneRecord     : Eintrag; 
	RecNr		  : UDINT;
	I			  : DINT;
	PtChar		  : ^CHAR;
	PtxNextRAM	  : ^XRamCell;
	ptxRamHead	  : ^XRAMHDR;
	RelAdress	  :  UDINT;
  formatChanged : BOOL;
  initCnt : UDINT;
	gotMemory : BOOL;
END_VAR

  if !!hasSram then
    ret_code:=C_OK;
    return;
  end_if;

  CONVERT_SRAM();

	// das erstellen wir automatisch 
  if _rtosversion > 16#537 then
    if LSLFILE_IsInstalled then
      // Das Direktory für die File Daten erstellen
      OS_FILE_CREATEDIR("C:\FData");
    end_if; 
  end_if;
	
  ret_code:=C_OK;
  initCnt := 0;
Brutal_start:
  initCnt += 1;

//---------------------------------------
// Wenn Neues BS ist der Ptr ungleich 0
//---------------------------------------

  IF _S_RAM_Hptr <> 0 then      // Diesen Pointer gibt es seit a.m mit pranz und gradl am 2.2.2002 vergangenheitsbewältigung gemacht hat

    if (_S_RAM_Hptr^.DataLength <= maxSramSize) then
      SRAM_STARTUP(_S_RAM_Hptr^.DataLength , 1);
    else
      SRAM_STARTUP(maxSramSize, 1);  // SRAM begrenzen
    end_if; 
		

    //------------------------------------------------------ 
    // wenn jungfräulich, test ob alte active.dat vorhanden
    //------------------------------------------------------	
    IF _S_RAM_Hptr^.UsedData = 0 then

      //-- Neues _SRAM Löschen ---
      
      // Der Aufruf dieser Funktion bewirkt, dass auch bei gelöschtem Sram 
      // das File sernum.dat, wo die Seriennummer gespeichert ist, erstellt wird.
      WasRemoveableMediaChanged();
		 
      RelAdress:=InitXramBlockEx(#gotMemory);
      IF gotMemory THEN
        ptxRamHead:= _S_RAM_Hptr^.DataStart+RelAdress;
        PtxNextRam:=#ptxRamHead^.aData[0];

        // active.dat nur möglich wenn filesystem vorhanden

        IF (LSLFILE_IsInstalled) THEN

          ActiveDatHandle:=OS_FILE_OPEN("c:\active.dat",0,16#000); 

          // das file ist gefunden -> konvertieren
          IF ActiveDatHandle > 0 THEN
            OS_FILE_READ(ActiveDatHandle, #DateiKopf.signi[0], sizeof(DateiKopf));
            IF DateiKopf.NrRec <> 0 then  // #FA wenn keine eintraege ende  (gibt es auch)
              FOR RecNr:= 0 to DateiKopf.NrRec -1 do
                // -- GUID LESEN 
                OS_FILE_READ(ActiveDatHandle,#OneRecord.myguid[0],sizeof(Eintrag.myguid));
                // -- NAMEN LESEN 

                PtCHar:=#OneRecord.Name[0];
                FOR I:= 0 to SIZEOF(Eintrag.Name) -1 do
                  OS_FILE_READ(ActiveDatHandle,PtCHar,1);
                  if PtChar^ =0 then
                    GOTO GOTNAME;
                  elsif PtChar^ = '.' then
                    PtChar^:=0;
                  end_if;
                  PtCHar+=1;
                END_FOR;

                // ERROR NAME TO LONG !!!!
          
                ret_code:=C_OUTOF_NEAR;
                OS_FILE_CLOSE(ActiveDatHandle);
                GOTO	Brutal_ende;

              //...........................
              // Der Name ist eingelesen !
              //...........................

              GOTNAME:


                // wenn block voll, neuen block erzeugen & Verkettung herstellen

                if ptxRamHead^.udEntries = X_RAM_CELLS	then
                  RelAdress:=IniNewRamBlock(PtxNextRam);
                  If Reladress <> 0 then
                    ptxRamHead:= _S_RAM_Hptr^.DataStart+RelAdress;
                    PtxNextRam:=#ptxRamHead^.aData[0];
                    // Kein platz mehr im RAM !!! 
                  else
                    ret_code:=C_OUTOF_NEAR;
                    OS_FILE_CLOSE(ActiveDatHandle);
                    GOTO	Brutal_ende;
                  end_if; 			
                end_if;

                // Daten und Sizeof daten lesen

                OneRecord.CRC:=CRC32(#OneRecord.Name[0]);

                OS_FILE_READ(ActiveDatHandle,#OneRecord.Data,2*4);

                // Wenn Len > 0 dann ist es eine RAMEX falls es eine RAMEX mit 0 ist
                // kommt das später beim Instanzieren dran

                IF OneRecord.Len <> 0 then
                  RelAdress$UDINT:= (SRAM_GetMemory(OneRecord.Len$UDINT))$UDINT;
                  If Reladress <> 0 then

                    PtxNextRam^.udType:=IsXram$UDINT+RelAdress$UDINT;
                    OS_FILE_READ(ActiveDatHandle,_S_RAM_Hptr^.DataStart+RelAdress,OneRecord.Len$UDINT);
                    // in the data HIGH Part is the real length 
                    // in the lo part the allocated length
                    OneRecord.Data$UDINT := OneRecord.Len$UDINT SHL 16;
                    OneRecord.Data$UDINT := OneRecord.Data$UDINT OR  SRAM_GET_ALLOCATED(OneRecord.Len$UDINT);
                  end_if;	

                ELSE
                  PtxNextRam^.udType:= 0; 
                END_IF;

                PtxNextRam^.udCRC32:=OneRecord.CRC;
                PtxNextRam^.udData$UDINT:=OneRecord.Data$UDINT;
                PtxNextRam^.udChk$UDINT:=OneRecord.Data$UDINT+OneRecord.CRC+(PtxNextRam^.udType$UDINT AND 16#00ffFFFF);
                PtxNextRam^.udType:=PtxNextRam^.udType or RamValid;  // VALID RAM LOCATION


                ptxNextRam+=SIZEOF(xRamCell);
                ptxRamHead^.udEntries+=1;
                ptxRamHead^.udChk:=NOT ptxRamHead^.udEntries;
          
              END_FOR;
            END_IF; 
            OS_FILE_CLOSE(ActiveDatHandle);
            S_RAM_INIT_I();
            
          // keine active.dat vorhanden  und neues system und dateisystemvorhanden .
          //--> Speicher Initialisieren 
          ELSE
            S_RAM_INIT_I();
          END_IF;

        // Kein filesystem vorhanden & keine daten: jungfräulich, initialisieren
        // oder das alte Format konvertieren !
        // muss eigentlich Eine 386 sein ??	

        ELSE
          // #FA
          // wir versuchen die alten daten auch zu konvertieren aber nur einmal
          if aRamHdr.udEntries <> 0 then 
            if aRamHdr.udEntries = NOT aRamHdr.udChk then
              if aRamHdr.udEntries < 1000 then // sicherheit
                // schleife für die konvertierung bilden !!!
                for RecNr:=0 to (aRamHdr.udEntries-1) do
          
                  // wenn block voll, neuen block erzeugen & Verkettung herstellen
                  if ptxRamHead^.udEntries = X_RAM_CELLS	then
                    RelAdress:=IniNewRamBlock(PtxNextRam);
                    If Reladress <> 0 then
                      ptxRamHead:= _S_RAM_Hptr^.DataStart+RelAdress;
                      PtxNextRam:=#ptxRamHead^.aData[0];
                    // Kein platz mehr im RAM !!! 
                    else
                      ret_code:=C_OUTOF_NEAR;
                      GOTO	Brutal_ende;
                    end_if; 			
                  end_if;
        
                  // check if chk is o.k. 
                  if aRamHdr.aData[RecNr].udChk = aRamHdr.aData[RecNr].udCRC32 + aRamHdr.aData[RecNr].udData then	
                    PtxNextRam^.udCRC32:=aRamHdr.aData[RecNr].udCRC32;
                    PtxNextRam^.udData$UDINT:=aRamHdr.aData[RecNr].udData;
                    PtxNextRam^.udChk$UDINT:=aRamHdr.aData[RecNr].udChk;

                    PtxNextRam^.udType:=PtxNextRam^.udType or RamValid;  // VALID RAM LOCATION

                    ptxNextRam+=SIZEOF(xRamCell);
                    ptxRamHead^.udEntries+=1;
                    ptxRamHead^.udChk:=NOT ptxRamHead^.udEntries;
                  end_if; 
                
                end_for;
              end_if;		  
            end_if;
            aRamHdr.udEntries :=0;
            aRamHdr.udChk :=0;	
          end_if;   
            
          S_RAM_INIT_I();		   		
        END_IF;
      ELSE // gotMemory
        IF !!isRamfileDatUsed THEN
          // Bei SRAM Format 1 muss auf jeden Fall der xRamHdr Platz haben
          LDR_outOfSramMem := TRUE;
        END_IF;
      
        S_RAM_INIT_I();		   		
      END_IF;

    //--- ES SIND DATEN VORHANDEN, TESTEN !!! ---	

    ELSE 
      S_RAM_INIT_I();
    END_IF;


  //------------------------------------
  // ALTES FORMAT (kein Ptr auf ram vorhanden)
  //------------------------------------
  else
    ret_code:=C_OUTOF_NEAR;
  END_IF;
Brutal_ende:

  IF loaderRamexError THEN
    IF isSramNewOrCleared THEN
      DEBUGLOG0("%010d-resetting sram content due to a detected new or cleared sram");
    ELSE
      DEBUGLOG0("%010d-resetting sram content due to a detected error");
    END_IF;
    LDR_outOfSramMem := FALSE;
    _s_ram_hptr^.UsedData := 0;
    _s_ram_hptr^.DataStart^.udEntries := 0;
    _s_ram_hptr^.DataStart^.udChk:=NOT _s_ram_hptr^.DataStart^.udEntries;
    IF GetRamfileDatUsedFlag(#formatChanged) THEN
      _s_ram_hptr^.DataValid := 0x20001;
    ELSE
      _s_ram_hptr^.DataValid := 0x00001;
    END_IF;
    IF pRamFile THEN
      pRamFile^.udEntries :=  0;
      pRamFile^.uiChange  :=  16#1234;	// änderung eintragen wir speicher hier gleich
    END_IF;
    SramSaveFile();	
    
    // DS2589, 02.02.182, 9.5.2016: 
    // Eine möglicherweise von früher noch vorhandene Sram-Kopie (vom Reorganisieren) 
    // wird gelöscht, damit im Fall einer gesetzten SRAM_CONTINUE_ON_ERROR Umgebungsvariable 
    // das Sram mit den Init-Werten und nicht mit den Werten aus der Kopie initialisiert wird.
    DeleteSramKopie();
    
    IF (!!bSramContinueOnError & !!isSramNewOrCleared) | (initCnt > 1) THEN
      ret_code:=C_OUTOF_NEAR;
    ELSIF formatChanged THEN
      // Wenn das Format geändert wurde, dann muss noch einmal initialisiert 
      // werden (es ist z.B. die Größe der MAT vom Format abhängig).
      loaderRamexError := 0;
      GOTO Brutal_start;
    END_IF;
  END_IF;

END_FUNCTION

// Löscht im alten Format in der letzten Zelle des ersten Blocks den im Element 
// udType (in den untersten 20 Bits) gespeicherten Merker für die letzte gefundene 
// Ramzelle. Dieser Merker wird von der Funktion Find_sRam_I zum Zwischenspeichern 
// des Block-Index und Zell-Offsets der zuletzt gefundenen Zelle verwendet. 
// Damit wird die Suche schneller, wenn Find_sRam_I nicht jedesmal von vorne 
// zu suchen beginnt, sondern dort wo sie den letzten Eintrag gefunden hat.
//
// Im neuen Format (mit ramfile.dat) wird dieser Merker nicht verwendet.
FUNCTION CLEAR_LASTENTRY
VAR
  pRamCell : ^XRamCell;
END_VAR;  

  IF (_S_RAM_Hptr^.DataValid SHR 16) < 2 &  _s_ram_hptr^.DataLength >= SIZEOF(XRAMHDR) THEN
    // altes Format
    pRamCell := #_S_RAM_Hptr^.DataStart^.aData[X_RAM_CELLS];
    pRamCell^.udType := pRamCell^.udType AND 16#FFF0_0000;
  END_IF;
  
END_FUNCTION


// loescht alle Einträge
FUNCTION ClearEntrys

	pRamFile^.udEntries   := 0; 
	pRamFile^.CRC32       := 0; 
	pRamFile^.pLastEntry  := 0; 

END_FUNCTION

#define READ_COMPLETE_RAMFILEDAT

// Wenn das Sram im RAM liegt mit einem Backup auf SD Karte (Edge-CPUs), 
// dann wird in dieser Funktion der Sram-Inhalt auf die SD Karte gesichert.
FUNCTION FlushSramToDisk
VAR 
  pSrd : ^OS_SRAMDISK;
END_VAR

  OS_CILGet("SRAMDISK", #pSrd$void);
  IF (pSrd <> NIL) & (pSrd^.udVersion >= 2) THEN
    SRAMDISK_COMMIT(pSrd);
#ifdef _LSL_TARGETARCH_X86
    // SA26191,DS2087: Workaround für einen OS-Fehler bei ETVEdge Plattformen: 
    // Interrupts sind nach Aufruf von SRAMDISK_COMMIT gesperrt, wenn fullCopyAtPowerdown gesetzt ist.
    // Die Interrupts werden hier wieder freigegeben.
    IF _RtOSVersion < 16#9000 THEN
      STI();
    END_IF;
#endif
  END_IF;

END_FUNCTION

// funktion schreibt daten auf Flash falls verson C-IPC +
// die Version > 2 ist und sich auch was geändert hat !! in diesem File
FUNCTION GLOBAL SramSaveFile
VAR 
	Handle : DINT;
	CRC32  : UDINT;
	aXFileHdr_Hdr : XFILEHDR_HDR;
	pCopy  : ^XFILEHDR;	
	rc     : UDINT;
	bytes2Write	 : UDINT;
	bytes2Read   : UDINT;
  allocSize : UDINT;
  att : UDINT;
END_VAR

  if !!hasSram then
    return;
  end_if;

	// test auf C-IPC
	if isRamfileDatUsed then
 	  // test auf pointer vorhanden und version groesser 1
	  if (_S_RAM_Hptr <> NIL) & ((_S_RAM_Hptr^.DataValid shr 16) > 1) then

      if pRamFile^.uichange <> 16#1234 then
        // Wenn keine Änderung war und noch kein RamFile.dat existiert (neuer CIPC), 
        // dann wird pRamFile^.uichange auf 16#1234 gesetzt. Dadurch wird das Anlegen 
        // eines (leeren) RamFile.dat erzwungen.
        Handle:=OS_FILE_OPEN("c:\RamFile.dat",0,16#000);	// read-only
        if handle >= 0 then
          OS_FILE_CLOSE(handle);
        else
          pRamFile^.uichange := 16#1234;
        end_if;
      end_if;

      // test ob auch sich was geändert hat	
      if pRamFile^.uichange = 16#1234 then
        // Ab 1.1.27 wird das SRAM bei jedem Start reorganisiert.
        // Die pRamFile Struktur wird immer neu aufgebaut, d.h. uichange hat 
        // dann immer den Wert 16#1234 weil immer Änderungen gemacht werden.
        // Jetzt kann es aber sein, daß die Struktur gleich geblieben ist (wenn sich  
        // die Checksumme und udEntries nicht geändert hat). Wenn das der Fall ist, dann 
        // wird uichange auf 0 gesetzt, damit das RAMFILE.DAT nicht unnötigerweise 
        // neu erstellt wird.

        // Checksumme rechnen 
  #ifdef READ_COMPLETE_RAMFILEDAT
        CRC32 := CALC_RAMFILE_DAT_CRC(pRamFile);
  #else
        CRC32:=CRC32_Buffer(#pRamFile^.aData,pRamFile^.udEntries * SIZEOF(XFileCell));
  #endif
        // Checksumme aus dem File einlesen und vergleichen
        handle := OS_FILE_OPEN("c:\RamFile.dat", 0, 0);		// 0 = read only
        IF handle > 0 THEN
          rc := OS_FILE_READ(handle, #aXFileHdr_Hdr, SIZEOF(aXFileHdr_Hdr));
          IF rc = SIZEOF(XFileHdr_Hdr) THEN
            IF (aXFileHdr_Hdr.CRC32 = CRC32) & (aXFileHdr_Hdr.udEntries = pRamFile^.udEntries) THEN
              pRamFile^.uichange := 0;
              DEBUGLOG2("%010d-SramSaveFile: no need to write to ramfile.dat (crc32=0x%08X and udEntries=%d are still the same)", CRC32, pRamFile^.udEntries);
            END_IF;
          END_IF;
          OS_FILE_CLOSE(handle);
        END_IF;
      end_if;

      if pRamFile^.uichange = 16#1234 then

        pRamFile^.CRC32 := CRC32;		 

				// RAMFILE.DAT auf RAMFILE.BAK kopieren
        Handle:=OS_FILE_OPEN("c:\RamFile.dat",0,16#000);
	      if handle >= 0 then
          allocSize := XFILEHDR.adata + X_FILE_CELLS_2 * SIZEOF(XFileCell);
     			pCopy := (_alloc_resize_LDR(allocSize))$^XFILEHDR;  
		      if pCopy <> NIL then
						bytes2Read := allocSize;
	     			rc := OS_FILE_READ(Handle, pCopy, bytes2Read); 
		        OS_FILE_CLOSE(handle);
						if rc < 0 then
							DEBUGLOG2("%010d-SramSaveFile: error reading ramfile.dat, bytes2read=%d, rc=%d", bytes2read, rc);
						else
		          handle:=OS_FILE_OPEN("c:\RamFile.bak",2,ATT_CREATE);	
							if handle >= 0 then
								bytes2Write := rc;
			 					rc := FileWrite(handle, pCopy, bytes2Write)$UDINT;
								if rc <> bytes2Write then
									DEBUGLOG2("%010d-SramSaveFile: error writing ramfile.bak, bytes2write=%d, rc=%d", bytes2Write, rc);
								else
									DEBUGLOG1("%010d-SramSaveFile: ramfile.dat copied into ramfile.bak (%d bytes)", rc);
								end_if;
				 				OS_FILE_CLOSE(handle);
							else
								DEBUGLOG1("%010d-SramSaveFile: could not open ramfile.bak, rc=%d", handle$UDINT);
							end_if;
	          end_if;
	     		  _free(pCopy);
					else
					  DEBUGLOG1("%010d-SramSaveFile: could not allocate enough memory (%d)", allocSize$UDINT);
		        OS_FILE_CLOSE(handle);
	        end_if;	
	   		end_if;	

        // den Inhalt von pRamFile ins File RAMFILE.DAT schreiben
        if pRamFile^.udEntries > X_FILE_CELLS then
          bytes2Write := XFILEHDR.adata + X_FILE_CELLS_2 * SIZEOF(XFileCell);
        else
          bytes2Write := XFILEHDR.adata + X_FILE_CELLS * SIZEOF(XFileCell);
        end_if;
        if bytes2Write <> LDR_allocated_XFileSize then
          att := ATT_CREATE_ALWAYS;
        else
          att := ATT_CREATE;
        end_if;

        handle:=OS_FILE_OPEN("c:\RamFile.dat", 2, att);	
        if Handle >= 0 then
          rc := FileWrite(handle, pRamFile, bytes2Write)$UDINT;
          if rc <> bytes2Write then
            DEBUGLOG2("%010d-SramSaveFile: error writing ramfile.dat, bytes2write=%d, rc=%d", bytes2Write, rc);
            SramError();
          else
            DEBUGLOG3("%010d-SramSaveFile: data written to ramfile.dat (%d bytes, crc32=0x%X), udEntries=%d", rc, pRamFile^.CRC32, pRamFile^.udEntries);
            // Wenn das Sram im RAM liegt mit einem Backup auf SD Karte (Edge-CPUs), 
            // dann wird nach dem Ändern der ramfile.dat der Sram-Inhalt auf die SD Karte 
            // geschrieben. Damit ist ramfile.dat und Sram konsistent, auch für den Fall dass 
            // beim Abschalten das Sram nicht mehr auf die SD Karte geschriben wird.
            FlushSramToDisk();            
            pRamFile^.uichange := 0;
          end_if;
          OS_FILE_CLOSE(handle);
        else
          DEBUGLOG1("%010d-SramSaveFile: could not open ramfile.dat, rc=%d", handle$UDINT);
          Trace_ERR("SAVE OF RamFile not possible (Disk full??)");
          LSLCLI_LoaderTracePrint("SAVE OF RamFile not possible (Disk full??)", 12);
          SramError();
        end_if;	
        
        // ramfile.bak ungültig machen, nachdem ramfile.dat erstellt worden ist
        InvalidateRamfileBak();

      end_if;			
    end_if;
	end_if;
END_FUNCTION

// ------------------------------------------
//   laden des File von C-IPC in Speicher 
// ------------------------------------------
// das file ist 192k /4 Byte (je Eintrag im S-RAM) * 12 byte (je Eintrag im File)
// groß = 576 k Byte
// die RamEx bleibt so wie alt 
// udCRC32    (4 Byte)
// udType     (4 Byte)
// udpRelSram (4 Byte)  

FUNCTION SramLoadFile
VAR_OUTPUT
	result : UDINT;
END_VAR	
VAR 
	Handle : DINT;
	CRC32  : UDINT;
	rc     : UDINT;
	bytes2Read   : UDINT;
	fileSize   : UDINT;
  udEntries_file  : UDINT;
  nCellsAlloc : UDINT;
  allocSize : UDINT;
END_VAR
  result := 16#FFFFFFFF;	

  // Die Anzahl der benötigten Zellen ist erst nach dem Lesen des Ramfile.dat 
  // bekannt, da aber nachfolgender Code davon ausgeht, dass pRamfile <> NIL ist, wird 
  // hier einmal mit X_FILE_CELLS allokiert. Ggf. wird das dann noch vergrößert.
  nCellsAlloc := X_FILE_CELLS;
  allocSize := XFILEHDR.adata + nCellsAlloc * SIZEOF(XFileCell);
	pRamFile := _alloc_resize_LDR(allocSize)$^XFILEHDR;  
  if pRamFile <> NIL then
    LDR_allocated_XFileCells := nCellsAlloc;
    LDR_allocated_XFileSize := allocSize;
    ClearEntrys();
  end_if;
  
  Handle := OS_FILE_OPEN("c:\RamFile.dat",0,0); 
  if Handle < 0 then
    DEBUGLOG1("%010d-SramLoadFile: could not open ramfile.dat, rc=%d", handle$UDINT);
    SramError();
    return;
  end_if;
  rc := OS_FILE_READ(Handle, #udEntries_file, SIZEOF(udEntries_file));
  if rc <> SIZEOF(udEntries_file) then
    DEBUGLOG1("%010d-SramLoadFile: could not read udEntries_file from ramfile.dat, rc=%d", rc);
    SramError();
    return;
  end_if;
  OS_FILE_CLOSE(handle);
  
  if udEntries_file > X_FILE_CELLS & pRamFile <> NIL then
    nCellsAlloc := X_FILE_CELLS_2;
    allocSize := XFILEHDR.adata + nCellsAlloc * SIZEOF(XFileCell);
   	pRamFile := _realloc_LDR(pRamFile, allocSize)$^XFILEHDR; 
  end_if;
  
  if pRamFile <> NIL then
    LDR_allocated_XFileCells := nCellsAlloc;
    LDR_allocated_XFileSize := allocSize;

    ClearEntrys();

    // try to open file 
    Handle:=OS_FILE_OPEN("c:\RamFile.dat",0,0); 
	  if Handle > 0 then
	 // file ist da laden
#ifdef READ_COMPLETE_RAMFILEDAT
			bytes2Read := allocSize;
			rc := OS_FILE_READ(Handle, pRamFile, bytes2Read);
#else
			rc := OS_FILE_READ(Handle, pRamFile, SIZEOF(XFILEHDR.udEntries));
      if rc = SIZEOF(XFILEHDR.udEntries) then
			  bytes2Read := XFILEHDR.adata + pRamFile^.udEntries * SIZEOF(XFileCell) - SIZEOF(XFILEHDR.udEntries);
			  rc := OS_FILE_READ(Handle, pRamFile + SIZEOF(XFILEHDR.udEntries), bytes2Read);
      end_if;
#endif
	    OS_FILE_CLOSE(handle);
      fileSize := XFILEHDR.adata + pRamFile^.udEntries * SIZEOF(XFileCell);
			if rc < 0 then
				DEBUGLOG1("%010d-SramLoadFile: error reading ramfile.dat, no header information, rc=%d", rc);
				SramError();
			elsif rc <> bytes2Read then
				DEBUGLOG3("%010d-SramLoadFile: error reading ramfile.dat, fileSize=%d, bytes2Read=%d, rc=%d", fileSize, bytes2Read, rc);
				SramError();
			else
	      // file im Speicher CRC rechnen
	      pRamFile^.uiChange :=0;				// keine Änderung
	      pRamFile^.pLastEntry :=0;				// und von anfang an
#ifdef READ_COMPLETE_RAMFILEDAT
	      CRC32 := CALC_RAMFILE_DAT_CRC(pRamFile);
#else
	      CRC32:=CRC32_Buffer(#pRamFile^.aData,pRamFile^.udEntries * SIZEOF(XFileCell));
#endif
	  
	      if CRC32 = pRamFile^.CRC32 then
	        // file im speicher CRC = o.k. wir belegen den Speicher
	        if ((pRamFile^.udEntries <> 0) & (pRamFile^.udEntries <= X_FILE_CELLS_2)) then
						DEBUGLOG2("%010d-SramLoadFile: data read from ramfile.dat (%d bytes), udEntries=%d", rc, pRamFile^.udEntries);
	          result:=0;
	        else
						if pRamFile^.udEntries > X_FILE_CELLS_2 then
					  	DEBUGLOG1("%010d-SramLoadFile: invalid udEntries (%08X)", pRamFile^.udEntries);
						  SramError();
						else
							result := 0;
						end_if;
						ClearEntrys(); // 0 oder zu viele einträge
  	      end_if;	
	      else
					DEBUGLOG2("%010d-SramLoadFile: invalid crc32 (calc:0x%08X, file:0x%08X)", CRC32, pRamFile^.CRC32);
					// die CRC ist falsch 

					Handle := OS_FILE_OPEN("c:\RamFile.er1",2,ATT_CREATE);
					FileWrite(handle,pRamFile, allocSize);
					OS_FILE_CLOSE(handle);
			
          // wir nehmen das BAK falls vorhanden
          Handle:=OS_FILE_OPEN("c:\RamFile.bak",2,ATT_READ_WRITE); 
          if Handle > 0 then
            OS_FILE_CLOSE(handle);
            OS_FILE_DELETE("c:\RamFile.dat");
            OS_FILE_RENAME("c:\RamFile.bak", "c:\RamFile.dat");
            _free(pRamFile);			// wird nochmal gemacht !!
            result:=SramLoadFile();	// rekursion !!
          end_if;
	      end_if;   
	    end_if;
	  else
			DEBUGLOG1("%010d-SramLoadFile: could not open ramfile.dat, rc=%d", handle$UDINT);
			SramError();
	  end_if;
  end_if;

	IF result <> 0 THEN
   		DEBUGLOG1("%010d-SramLoadFile failed, rc=%d", result);
	END_IF;

END_FUNCTION

//------------------------------------------------------------------------------
// AlarmSramError
//------------------------------------------------------------------------------
FUNCTION AlarmSramError
VAR_INPUT
  param : DINT;
END_VAR
	IF (pSramAlarmSvrCh = NIL) & ( (OPS.SysState = C_RUNRAM) | (OPS.SysState = C_RUNROM) ) THEN
		_LookUpEmbed_C(SRAMALARM_SVR_NAME, #pSramAlarmSvrCh, NIL);
	END_IF;
	IF pSramAlarmSvrCh <> NIL THEN
		ESI := pSramAlarmSvrCh;
		pSramAlarmSvrCh^.pMeth^.pWr$M_WR_DIRECT(param);
	END_IF;
END_FUNCTION
//------------------------------------------------------------------------------
// SramError
//------------------------------------------------------------------------------
FUNCTION SramError
	IF loaderRamexError = 0 THEN
    IF isSramNewOrCleared THEN
		  TRACE("New or cleared SRAM detected, see c:\sysmsg\event19.log. Resetted SRAM.");
      LSLCLI_LoaderTracePrint("New or cleared SRAM detected, see c:\sysmsg\event19.log. Resetted SRAM.", 12);
    ELSIF bSramContinueOnError THEN
		  TRACE_WARN("Error in SRAM, see c:\sysmsg\event19.log. Resetted SRAM.");
      LSLCLI_LoaderTracePrint("Error in SRAM, see c:\sysmsg\event19.log. Resetted SRAM.", 12);
    ELSE
		  TRACE_WARN("Error in SRAM, see c:\sysmsg\event19.log. Resetted SRAM, project must be restarted");
      LSLCLI_LoaderTracePrint("Error in SRAM, see c:\sysmsg\event19.log. Resetted SRAM, project must be restarted", 12);
    END_IF;
	END_IF;
	loaderRamexError := 1;
END_FUNCTION
//------------------------------------------------------------------------------
// LogSramError
//------------------------------------------------------------------------------
FUNCTION LogSramError
	IF sramErrorCode = 4 THEN
		// Der Fehlercode 4 kommt zustande, wenn ein RAMFILE.DAT mit udEntries = 0 
		// vorgefunden wird.
		// pr:
		//	Meiner Meinung nach ist das kein Fehler, daher wird hier keine Meldung 
		//  ausgegeben.
		//  Es wird aber trotzdem der Code für die Fehlerbehandlung durchlaufen, weil 
		//  ich nicht weiss wie der ganze Assemblercode auf udEntries = 0 reagieren würde.
		RETURN;
	END_IF;
	DEBUGLOG1("%010d-SramError, errorCode=%08X", sramErrorCode);
	IF _S_RAM_Hptr THEN
		DEBUGLOG1("%010d             Version    = %08X" , _S_RAM_Hptr^.Version);
		DEBUGLOG1("%010d             DataStart  = %08X" , _S_RAM_Hptr^.DataStart$UDINT);
		DEBUGLOG1("%010d             DataLength = %08X" , _S_RAM_Hptr^.DataLength);
		DEBUGLOG1("%010d             UsedData   = %08X" , _S_RAM_Hptr^.UsedData);
		DEBUGLOG1("%010d             DataValid  = %08X" , _S_RAM_Hptr^.DataValid);
		IF _S_RAM_Hptr^.DataStart THEN
			DEBUGLOG1("%010d             udEntries  = %08X" , _S_RAM_Hptr^.DataStart^.udEntries);
			DEBUGLOG1("%010d             udChk      = %08X" , _S_RAM_Hptr^.DataStart^.udChk);
		END_IF;
	END_IF;
	SramError();
END_FUNCTION
//------------------------------------------------------------------------------
// LogSramRamexError
//------------------------------------------------------------------------------
FUNCTION __CDECL LogSramRamexError
VAR_INPUT
	pXramCell : ^XRamCell;
END_VAR
	DEBUGLOG1("%010d-RamexError, cellAddr = %08X" , pXramCell$UDINT);
	DEBUGLOG1("%010d             udChk    = %08X" , pXramCell^.udChk);
	DEBUGLOG1("%010d             udCRC32  = %08X" , pXramCell^.udCRC32);
	DEBUGLOG1("%010d             udData   = %08X" , pXramCell^.udData);
	DEBUGLOG1("%010d             udType   = %08X" , pXramCell^.udType);
	SramError();
END_FUNCTION
//------------------------------------------------------------------------------
// LogSramChk
//------------------------------------------------------------------------------
FUNCTION LogSramChk
	DEBUGLOG0("%010d-checking Sram");
END_FUNCTION
//------------------------------------------------------------------------------
// SramCheckerInit
//------------------------------------------------------------------------------
FUNCTION SramCheckerInit
	fileCellChkIdx := 0;
END_FUNCTION

#ifdef CYCLIC_SRAM_CHECK
#define CELLMASK_VALID      0x80000000
#define CELLMASK_USED       0x40000000
#define CELLMASK_CELLTYPE   0x3C000000
#define CELLMASK_RAMEXADR   0x00FFFFFF

#define CELLTYPE_RAM        0x00000000
#define CELLTYPE_RAMEX      0x30000000

FUNCTION SramCheckV1
VAR
  pHdr : ^XRAMHDR;
  sramStart : pVoid;
  sramEnd : pVoid;
  pCellEnd : ^XRamCell;
  pCell : ^XRamCell;
  cellChksum : UDINT;
END_VAR

  sramStart := _S_RAM_Hptr^.dataStart;
  sramEnd := sramStart + _S_RAM_Hptr^.dataLength;
  pHdr := _S_RAM_Hptr^.dataStart;
  
  WHILE ((pHdr >= sramStart) & (pHdr < sramEnd)) DO 
  
    IF pHdr^.udEntries > X_RAM_CELLS + 1 THEN
  		sramErrorCode := EC_INV_BLK_ENTRIES;
      SramError();
      AlarmSramError(pHdr$DINT);
    END_IF;
    IF (pHdr^.udEntries XOR pHdr^.udChk) <> 16#FFFFffff THEN
  		sramErrorCode := EC_INV_BLK_CHKSUM;
      SramError();
      AlarmSramError(pHdr$DINT);
    END_IF;
    
    pCell := #pHdr^.aData[0];
    pCellEnd := #pHdr^.aData[pHdr^.udEntries];
    WHILE pCell < pCellEnd DO 
    
      IF pCell^.udType AND CELLMASK_VALID THEN
        IF pCell^.udType AND CELLMASK_CELLTYPE AND CELLTYPE_RAMEX THEN
          cellChksum := pCell^.udData + pCell^.udCRC32 + (pCell^.udType AND CELLMASK_RAMEXADR);
        ELSE
          cellChksum := pCell^.udData + pCell^.udCRC32;
        END_IF;
        IF cellChksum <> pCell^.udChk THEN
      		sramErrorCode := EC_INV_CELL_CHKSUM;
          SramError();
          AlarmSramError(pCell$DINT);
        END_IF;
      END_IF;
    
      pCell += sizeof(XRamCell);
    END_WHILE;

    IF pCell = #pHdr^.aData[X_RAM_CELLS] THEN
      // Block war zur Gänze gefüllt
      // prüfen, ob die 501. Zelle auf einen weiteren Block zeigt
      cellChksum := pCell^.udData + pCell^.udCRC32;
      IF ((pCell^.udCRC32 = 1954) & (pCell^.udType AND CELLMASK_VALID) & (cellChksum = pCell^.udChk)) THEN
        // in der 501.Zelle ist ein Zeiger auf den nächsten XRam-Block
        pHdr := sramStart + pCell^.udData;
        IF ((pHdr < sramStart) | (pHdr >= sramEnd)) THEN
          // Verkettung ungültig
      		sramErrorCode := EC_INV_BLK_CHAIN;
          SramError();
          AlarmSramError(pCell$DINT);
        END_IF;
      ELSE
        pHdr := NIL;
      END_IF;
    ELSE
      // Block war nur teilweise gefüllt, es handelt sich also um den letzten Block
      pHdr := NIL; // bewirkt Schleifenaabruch
    END_IF;
    
  END_WHILE;

  
END_FUNCTION

//------------------------------------------------------------------------------
// SramChecker
//------------------------------------------------------------------------------
FUNCTION GLOBAL SramCheckerCyclic
VAR
	pFileCell 	: ^XFileCell;
	pRamCell	: ^XRamCell;
	quit		: BOOL;
	chkSum		: UDINT;
	fileCellCnt	: UDINT;
END_VAR

	// wenn bereits ein Fehler festgestellt wurde, dann nichts mehr prüfen, 
	// damit das Logfile nicht voll wird
	IF loaderRamexError <> 0 THEN
		AlarmSramError(1);	// Alarm immer wieder auslösen
		RETURN;
	END_IF;

 	IF _S_RAM_Hptr = NIL THEN
		RETURN;
	END_IF;
  
 	IF _S_RAM_Hptr^.DataValid < 2 THEN
    SramCheckV1();
		RETURN;
	END_IF;
  
 	IF !!isRamfileDatUsed THEN
		RETURN;
	END_IF;

	IF pRamFile^.udEntries = 0 THEN
		RETURN;
	END_IF;

(*
	pr, 7.2.05: ab 1.1.3 wird bei udEntries = 0 kein Sramfehler gemeldet
	IF _S_RAM_Hptr^.dataStart^.udEntries = 0 THEN
		sramErrorCode := 10;
		LogSramError();
		AlarmSramError(1);
		RETURN;
	END_IF;
*)

	IF (_S_RAM_Hptr^.dataStart^.udEntries XOR _S_RAM_Hptr^.dataStart^.udChk) <> 16#FFFFffff THEN
		sramErrorCode := EC_INV_BLK0_CHKSUM2;
		LogSramError();
		AlarmSramError(1);
		RETURN;
	END_IF;

	fileCellCnt := 0;
	quit := FALSE;
	WHILE !!quit DO

		pFileCell := #pRamFile^.aData[fileCellChkIdx];
		IF (pFileCell^.udCRC32 <> 0) & ((pFileCell^.udType AND 16#3C00_0000) = isXram) THEN
			// Ramex Zelle gefunden -> Checksumme prüfen, dann Schleife abbrechen
			pRamCell := _S_RAM_Hptr^.dataStart + pFileCell^.udpRelSram;

			chkSum := (pRamCell^.udType AND 16#00ffFFFF) + pRamCell^.udData + pRamCell^.udCRC32;
			IF chkSum <> pRamCell^.udChk THEN
				LogSramRamexError(pRamCell);
				AlarmSramError(pRamCell$DINT);
				//(0$^UDINT)^ := 0;	// Exception auslösen
			END_IF;

			quit := TRUE;
		END_IF;

		fileCellChkIdx += 1;
		IF fileCellChkIdx >= pRamFile^.udEntries THEN
			// das Ende ist erreicht -> index auf 0, dann Schleife abbrechen
			fileCellChkIdx := 0;
			quit := TRUE;
		END_IF;

		fileCellCnt += 1;
		IF fileCellCnt > 15 THEN
			// max. 15 File-Cells pro Durchlauf prüfen (sonst kann ein RUNTIME Error auftreten !)
			quit := TRUE;
		END_IF;

	END_WHILE;

END_FUNCTION
#endif


#ifdef _LSL_TARGETARCH_ARM
(*
  Überprüft den Inhalt der Sram-V2 Datenstruktur ob diese gültig ist 
  und markiert den verwendeten Sram-Speicher in der MAT als belegt.
  
  Wenn der Sram Inhalt inhalt gültig ist, wird 0 zurückgegeben, sonst 
  ein entsprechender Fehlercode.
*)
FUNCTION CHK_S_RAMV2
VAR_OUTPUT
  errCode : UDINT;
END_VAR
VAR
  i : UDINT;
  pXRAMHDR : ^XRAMHDR;
  pXRamCell : ^XRamCell;
  pXFileCell : ^XFileCell;
  ramStart :^XRamHdr;
END_VAR

  errCode := 0;

  IF !!(_S_RAM_Hptr^.DataValid AND 1) THEN
    errCode := EC_NOT_VALID;
    RETURN;
  END_IF;
 
  // In UsedData ist der Header mit eingerechnet
  IF _S_RAM_Hptr^.UsedData > _S_RAM_Hptr^.DataLength + _S_RAM_Hptr^.DataStart$UDINT - _S_RAM_Hptr$UDINT THEN
    errCode := EC_OUT_OF_SRAM;
    RETURN;
  END_IF;

  pXRAMHDR := _S_RAM_Hptr^.DataStart;
  IF pXRAMHDR^.udChk <> NOT pXRAMHDR^.udEntries THEN
    errCode := EC_INV_BLK0_CHKSUM;
    RETURN;
  END_IF;

  ramStart := _S_RAM_Hptr^.DataStart;
  
  // alle einträge o.k. jetzt machen wir noch eine Belegung im RAM mit dem mapper

  // *****************************************
  // die ersten 8 byte sind besonders //
  // *****************************************
  SRAM_MARKMemory(NIL, 8); //length: XRAMHDR (udEntries + udChk)
  
  pXFileCell := #pRamFile^.aData[0];
  FOR i := 1 TO pRamFile^.udEntries DO 
    IF pXFileCell^.udCRC32 <> 0 THEN
      IF (pXFileCell^.udType AND 16#3C00_0000) <> ISXRAM THEN
        // normale RAMZelle	einfach speicher reservieren 
        
        SRAM_MARKMemory(pXFileCell^.udpRelSram$^USINT, 4);
        
      ELSE
        // RAMEx Zelle
        
        pXRamCell := ramStart + pXFileCell^.udpRelSram;
      
        // 'XRamCell.udCRC32 = XFileCell.udCRC32' darf nicht geprueft werden weil 
        // durch exchdataptr woanders liegen kann
        
        // CRC pruefen
        IF pXRamCell^.udChk = ((pXRamCell^.udType AND 16#00ffFFFF) +
                               pXRamCell^.udData +
                               pXRamCell^.udCRC32) THEN
          // super o.k. --> reservieren 16 byte block + reservieren Datenblock
          SRAM_MARKMemory(pXFileCell^.udpRelSram$^USINT, sizeof(XRamCell));
          SRAM_MARKMemory((pXRamCell^.udType AND 16#00FF_FFFF)$^USINT, pXRamCell^.udData AND 16#ffff);
        ELSE
          // die Ramex weist Fehler auf 
          errCode := EC_INV_XRAMCELL_CHKSUM;
          LogSramRamexError(pXRamCell);
          
          pXFileCell^.udpRelSram := 0; // ungültig machen neuer speicher anlegen
          pXFileCell^.udCRC32 := 0; // loeschen damit nicht mehr gefunden wird
          
          pRamFile^.uiChange := 16#1234; // merker das Änderung
          
          pXRamCell^.udType := 0; // daten auch loaschen
          pXRamCell^.udData := 0; // daten auch loaschen
          pXRamCell^.udCRC32 := 0;
          pXRamCell^.udChk := 1;	 // damit chk sicher falsch ist 
        END_IF;
      END_IF;
    END_IF;
    pXFileCell += sizeof(XFileCell);
  END_FOR;
    
END_FUNCTION

#ifdef _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1
(*
  Initialisiert das Sram im alten Format mit einem leeren Block.
*)
FUNCTION S_RAMV1_INITIALIZE_WITH_ONE_BLOCK
VAR
	sramSize : UDINT;
END_VAR

  _S_RAM_Hptr^.UsedData := 0;
  _S_RAM_Hptr^.DataValid := _S_RAM_Hptr^.DataValid AND 16#8000_0000; // alles außer Reorg-Flag rücksetzen

  IF _S_RAM_Hptr^.DataLength < maxSramSize THEN
    sramSize := _S_RAM_Hptr^.DataLength;
  ELSE
    sramSize := maxSramSize;
  END_IF;
  SRAM_STARTUP(sramSize, 1); // MAT für die Größe des SRAMs anlegen
  InitXramBlock();
  
END_FUNCTION
#endif // _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1


#ifdef _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1
(*
  Überprüft den Inhalt der Sram-V1 Datenstruktur ob diese gültig ist 
  und markiert den verwendeten Sram-Speicher in der MAT als belegt.
  
  Wenn der Sram Inhalt inhalt gültig ist, wird 0 zurückgegeben, sonst 
  ein entsprechender Fehlercode.
*)
FUNCTION CHK_S_RAMV1
VAR_OUTPUT
  errCode : UDINT;
END_VAR
VAR
  i : UDINT;
  pXRAMHDR : ^XRAMHDR;
  pXRamCell : ^XRamCell;
  cellValid : BOOL;
  typeFlags : UDINT;
  len : UDINT;
  ofs : UDINT;
END_VAR

  errCode := 0;

  IF !!(_S_RAM_Hptr^.DataValid AND 1) THEN
    errCode := EC_NOT_VALID;
    RETURN;
  END_IF;

  // In UsedData ist der Header mit eingerechnet
   IF _S_RAM_Hptr^.UsedData > _S_RAM_Hptr^.DataLength + _S_RAM_Hptr^.DataStart$UDINT - _S_RAM_Hptr$UDINT THEN
     errCode := EC_OUT_OF_SRAM;
     RETURN;
   END_IF;

  _S_RAM_Hptr^.DataStart^.aData[X_RAM_CELLS].udType := 
    _S_RAM_Hptr^.DataStart^.aData[X_RAM_CELLS].udType AND 16#80FFffff;  // 16*65535 Entries Total(used for quicksearch)
  
  //- Header prüfen --
  pXRAMHDR := _S_RAM_Hptr^.DataStart;
  WHILE 1 DO
  
    IF pXRAMHDR^.udEntries > X_RAM_CELLS THEN
      // more than 500 entries in one list !
      errCode := EC_INV_BLK_ENTRIES;
      RETURN;
    END_IF;
    IF pXRAMHDR^.udEntries <> NOT pXRAMHDR^.udChk THEN
      // CHKSUMME nicht OK
      errCode := EC_INV_BLK_CHKSUM;
      RETURN;
    END_IF;
    
    IF pXRAMHDR^.udEntries = 0 THEN
      S_RAMV1_INITIALIZE_WITH_ONE_BLOCK();	// #FA kommt vor weil active.dat nur header hat und ekien eintrage
      RETURN;
    END_IF;

    //-- Speicherblock belegen  mit länge eines blockes !
    SRAM_MARKMemory(pXRAMHDR$^USINT - _S_RAM_Hptr^.DataStart$UDINT, SIZEOF(XRAMHDR));
    
    pXRamCell := #pXRAMHDR^.aData[0];
    i := 0;
    WHILE i < pXRAMHDR^.udEntries DO 
      cellValid := TRUE;
      IF (pXRamCell^.udType AND RamValid) = 0 THEN // ValidFlg (Bit 31) Must Be High !
        cellValid := FALSE;
      ELSIF pXRamCell^.udChk <> ((pXRamCell^.udType AND 16#00ffFFFF) + 
                                 pXRamCell^.udData + pXRamCell^.udCRC32) THEN
        cellValid := FALSE;
      ELSE
        pXRamCell^.udType := pXRamCell^.udType AND NOT RAMUSED; // Clr Used Bit
        // CHK FOR Xtended Ram
        typeFlags := pXRamCell^.udType AND 16#3C00_0000;
        IF typeFlags <> 0 THEN
          IF typeFlags <> IsXram THEN
            cellValid := FALSE;
          END_IF;
        END_IF;
      END_IF;

      IF cellValid THEN
        len := pXRamCell^.udData AND 16#ffff; // mehr als 16#ffff geht nicht
        ofs := pXRamCell^.udType AND 16#00FF_ffff;
        SRAM_MARKMemory(ofs$^USINT, len);
      ELSE
        //--- contens of cell not valid , mark it as free (unused) --	
        pXRamCell^.udType := 0;
        pXRamCell^.udData := 0;
        pXRamCell^.udCRC32 := 0;
        pXRamCell^.udChk := 0;
#ifdef SRAM_REPORT_INVALID_CELL1
        // Wenn ein Fehler bei einer Format-1 Zelle gefunden wird, dann wird standardmäßig 
        // der Zelleninhalt auf 0 gesetzt und kein Sram Fehler gemeldet. 
        // Warum wird kein Fehler gemeldet ?
        // Es hat einmal eine Loaderversion gegeben, bei der eine Sram-Zelle die noch nie 
        // beschrieben wurde (also den Init-Wert aufweist), eine ungültige Chksum hatte. 
        // Fürs Projekt hatte das keine neg. Auswirkungen, da die Chksum bereichtigt wurde, 
        // sobald die Zelle beschrieben wurde (siehe Änderung in 02.02.102, SA17043).
        // Würde man jetzt bei einer falschen Chksum in einer Zelle einen Sram-Fehler melden 
        // und das ganze Sram resettieren, dann könnte bei funktionierenden Applikationen 
        // das Sram zurückgesetzt werden.
        errCode := EC_INV_V1_CELL;
        RETURN;
#endif    
      END_IF;
      
      pXRamCell += sizeof(XRamCell);
      i += 1;
    END_WHILE;
    
    // -- CHK FOR NEXT BLOCK --
    IF i < X_RAM_CELLS THEN
      RETURN;
    END_IF;
    
    IF (pXRamCell^.udCRC32 <> 1954) | !!(pXRamCell^.udType AND RamValid) | 
       (((pXRamCell^.udType AND 16#00ff_FFFF) + pXRamCell^.udData + pXRamCell^.udCRC32) <> pXRamCell^.udChk) THEN
      // Mark Cell 500 As EMPTY
      pXRamCell^.udType := 0;
      pXRamCell^.udData := 0;
      pXRamCell^.udCRC32 := 0;
      pXRamCell^.udChk := 0;
      RETURN;
    END_IF;
    
    pXRamCell := _S_RAM_Hptr^.DataStart + pXRamCell^.udData;
    
  END_WHILE;
  
END_FUNCTION
#endif // _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1
#endif

//-------------------------------------------
// Inhalt der daten im S-RAM -Prüfen und verwendeten Sram-Speicher in der MAT 
// als belegt markieren.
//-------------------------------------------

//  udType:
//
// Bit 31.. 1= VALID  (CHksum is OK)
//     30.. 1= USED   (After Startup, it is conected to an Objekt)

//     29..   0   1    			(3000 0000) 
//     28..   0   1  
//     27..   0   0 
//	   26..   0   0   
//			  !   +-- 	RAMEX ; UdData field contains LENGTH of data, 
//			  +--- RAM						
//     25..   Frei
//	   24..   Frei
//
//	   0-23   Relative Startadress  of datablock if RAMEX	 							   


// CHK := udCrc+udData+(udType AND 16#00ffffff)

#ifdef _LSL_TARGETARCH_ARM
FUNCTION CHK_S_RAM
VAR_INPUT
  fDoNotReportErrors : UDINT;
END_VAR
VAR_OUTPUT
  wasValid : UDINT;
END_VAR
VAR
	sramSize : UDINT;
  errCode : UDINT;
END_VAR

  LogSramChk();
  wasValid := 1;
  sramErrorCode := 0;
  
  IF _S_RAM_Hptr^.DataLength < maxSramSize THEN
    sramSize := _S_RAM_Hptr^.DataLength;
  ELSE
    sramSize := maxSramSize;
  END_IF;
  SRAM_STARTUP(sramSize, 1); // MAT für die Größe des SRAMs anlegen
  
  IF isRamfileDatUsed & ((_S_RAM_Hptr^.DataValid SHR 16) AND 16#7fff) >= 2 THEN // Bit31 von DataValid ist das Reorg-Flag
    // =========================================================================
    // neue Version (mit ramfile.dat)
    // =========================================================================
    errCode := CHK_S_RAMV2();
    IF errCode <> 0 THEN
      sramErrorCode := errCode;
      wasValid := 0;
      
      IF errCode < 100 THEN
        IF !!fDoNotReportErrors THEN
          LogSramError();
        END_IF;
        // im file alles auf 0
        pRamFile^.udEntries := 0;
        pRamFile^.CRC32 := 0;
        pRamFile^.pLastEntry := 0;
        pRamFile^.uiChange := 16#1234; // merker das Änderung

        // auch im ram alles auf 0 
        _S_RAM_Hptr^.UsedData := 0;
        _S_RAM_Hptr^.DataValid := (_S_RAM_Hptr^.DataValid AND 16#8000_0000) OR 16#0002_0000; // alles außer Reorg-Flag rücksetzen
      // und die anzahl der einträge auch loeschen
        _S_RAM_Hptr^.DataStart^.udEntries := 0;
        _S_RAM_Hptr^.DataStart^.udChk := 16#FFFF_ffff;

        IF _S_RAM_Hptr^.DataLength < maxSramSize THEN
          sramSize := _S_RAM_Hptr^.DataLength;
        ELSE
          sramSize := maxSramSize;
        END_IF;
        SRAM_STARTUP(sramSize, 1); // MAT für die Größe des SRAMs anlegen
        SRAM_MARKMemory(NIL, 8); //length: XRAMHDR (udEntries + udChk)
      END_IF;
    END_IF;
    
    // RAM allociert und als belegt gemeldet jetzt kommen die Konstruktoren der 
    // RAM und RAMEX und wollen sich die Pointer holen					
  ELSE
#ifdef _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1
    // =========================================================================
    // alte Version (ohne ramfile.dat)
    // =========================================================================
    errCode := CHK_S_RAMV1();
    IF errCode <> 0 THEN
      sramErrorCode := errCode;
      wasValid := 0;
      
      IF !!fDoNotReportErrors THEN
        LogSramError();
      END_IF;

      S_RAMV1_INITIALIZE_WITH_ONE_BLOCK();
      
    END_IF;
#else
    // altes Sram Format wird nicht unterstützt
    sramErrorCode := EC_UNSUPPORTED_FORMAT_V1;
    wasValid := 0;
    IF !!fDoNotReportErrors THEN
      LogSramError();
    END_IF;
#endif
    
  END_IF;
    
  GET_USED_S_RAM();    

END_FUNCTION
#else
FUNCTION AWL CHK_S_RAM
VAR_INPUT
 fDoNotReportErrors : UDINT;
END_VAR
VAR_OUTPUT
 wasValid : UDINT;
END_VAR
VAR
 MAX_MEM	:UDINT ;
 RamStart   :^XramHdr;
END_VAR


		//*************************
		CALL		LogSramChk
		//*************************

		L.D			wasValid, 1

//	  SRAM_STARTUP(	16#40000 , 1); 
		
		l.eax		_S_RAM_Hptr
		l.eax		(eax+MRAM_DESCR.DataLength)
		cmp.eax		maxSramSize
		jbe			CHK_S_RAMn  
		l.eax		maxSramSize
CHK_S_RAMn
		PUSH		EAX			 // result
		PUSH.D		1
		PUSH.D		EAX 		 // Länge von OS
		CALL		SRAM_STARTUP
		ADD.ESP		2*4
		POP			EAX			 // RESULT


	
		L.D			MAX_MEM,0

	
   		L.EDI 		_S_RAM_Hptr
// test ob Ramfile.dat verwendet wird
		cmp.b		isRamfileDatUsed, 0
		je			chk_s_ram1

		l.ecx		(EDI+MRAM_DESCR.DataValid)
		shr.ecx		16
		cmp.ecx		2
		jb			chk_s_ram1			 // alte version

// es ist schon eine neue version
		l.ecx		(EDI+MRAM_DESCR.DataValid)
		and.ecx		1
		//*****
		L.D			sramErrorCode,EC_NOT_VALID
		//*****
		jz			chk_s_ram_error		// not valid


    (*DB1875*)
		l.eax		(EDI+MRAM_DESCR.DataLength)
		l.ecx		(EDI+MRAM_DESCR.UsedData)
    add.eax (EDI+MRAM_DESCR.DataStart)  // Header ist in UsedData mit eingerechnet
    sub.eax EDI
    cmp.ecx eax
		//*****
		L.D			sramErrorCode,5
		//*****
		jg			chk_s_ram_error

   		L.EDI		(EDI+MRAM_DESCR.DataStart)
		L.EAX		(EDI+XRAMHDR.udEntries)
		x.EAX		16#ffffffff
		cmp.EAX		(EDI+XRAMHDR.udChk)		
		//*****
		L.D			sramErrorCode,EC_INV_BLK0_CHKSUM
		//*****
		jne			chk_s_ram_error

// alle einträge o.k. jetzt machen wir noch eine Belegung im RAM mit dem mapper
		L.EBX		pRamFile
		l.ECX		(EBX+XFILEHDR.udEntries)

		// pr, 21.1.06:
		//	Hier war eine Abfrage ob XFILEHDR.udEntries 0 ist und ein Jump 
		//	auf chk_s_ram_error falls es 0 ist. Im Kommentar steht, daß das 
		//  ein Fehler ist, der nicht sein kann.
		//	Wieso soll das ein Fehler sein ?
		//	Meiner Meinung nach ist das ganz normal daß XFILEHDR.udEntries 0 ist 
		//  wenn ich ein Projekt ohne RAM- und RAMEX- Zellen habe.
		//	Ich war daher so frei und habe diese Abfrage einfach auskommentiert.
		//*****
		//L.D			sramErrorCode,4
		//*****
		//jecxz		chk_s_ram_error		// --> fehler der nicht sein kann		

// *****************************************
// die ersten 8 byte sind besonders //
// *****************************************

		PUSH		EAX		// RESULT
		PUSH.D		8		// XRAMHDR: STRUCT =udEntries UDINT+udChk UDINT;
		CLR			EAX		// OFFSET 0 = 8 Byte	
		PUSH		EAX
		CALL		SRAM_MARKMemory
		ADD.ESP		2*4
		POP			EAX		// Result	


		ADD.EBX		XFILEHDR.aData		// EBX zeigt auf die ersten Dateneintrag

		O.ECX		ECX
		JZ			chk2lp_end
chk2lp		
		l.eax		(ebx+XFileCell.udCRC32)		// CRC
		o.eax		eax							// flags setzen vergessen 01.10.2004 (aschl) damit geht eine Lawine los alles hin
		jz			chk2nxt				        // --> CRC = 0 invalid !! (nur zur Sicherheit)

		l.eax		(ebx+XFileCell.udType)		// finden ob RAM oder RAMEX
		A.EAX		16#3C00_0000
		cmp.eax		isXram						// test ob RAMEX
		je			chk2ramex					// --> ramEX

		l.eax		(ebx+XFileCell.udpRelSram)

// normale RAMZelle	einfach speicher reservieren 
		PUSH		EAX							// RESULT
		PUSH.D		4							// XRAMHDR: STRUCT =udEntries UDINT+udChk UDINT;
		PUSH		EAX							// relativer RAMoffset
		CALL		SRAM_MARKMemory
		ADD.ESP		2*4
		POP			EAX							// Result	
		jmp			chk2nxt
						

chk2ramex

		l.edi		_S_RAM_Hptr
		l.edi		(edi+MRAM_DESCR.DataStart)
		add.edi		(ebx+XFileCell.udpRelSram)  // EDI zeigt jetzt auf die Zelle

// jetzt zeigen wir in S_RAM da steht der Inhalt der gesamten Zelle		

// darf nicht geprueft werden weil durch exchdataptr woanders liegen kann
//		l.eax		(edi+XRamCell.udCRC32)
//		cmp.eax		(ebx+XFileCell.udCRC32)
//		jne			chk2ramexFault				// CRC-RAM <> CRC-File

// CRC pruefen

		L.EAX		(EDI+XRamCell.udType)		
		A.EAX		16#00ffFFFF	
		ADD.EAX		(EDI+XRamCell.udData)
	   	ADD.EAX		(EDI+XRamCell.udCRC32)
		cmp.eax		(EDI+XRamCell.udChk)
		//*****
		L.D			sramErrorCode,EC_INV_XRAMCELL_CHKSUM
		//*****
		jne			chk2ramexFault				// checksumme falsch

// super o.k. --> reservieren 16 byte block + reservieren Datenblock

		PUSH		EAX		                 	// Result
		PUSH.D		sizeof (XRamCell)
		L.EAX		(EBX+XFileCell.udpRelSram)  // Startoffset Zelle
		PUSH		EAX
		CALL		SRAM_MARKMemory
		ADD.ESP		2*4
		POP			EAX							// Result	

		PUSH		EAX		                 	// Result
		l.eax		(EDI+XRamCell.udData) 		// = LÄNGE
		and.eax		16#ffff                   	// mehr geht nicht
		PUSH		EAX
		L.EAX		(EDI+XRamCell.udType) 		// Startoffset
		A.EAX		16#00FF_FFFF
		PUSH		EAX
		CALL		SRAM_MARKMemory
		ADD.ESP		2*4
		POP			EAX							// Result	
		jmp			chk2nxt

// die Ramex weist Fehler auf 
chk2ramexFault						
		//*************************
		PUSHAD
		PUSHFD
		PUSH		EDI
		CALL		LogSramRamexError
		POP			EDI
		POPFD
		POPAD
		//*************************
		L.D			wasValid, 0
    
		l.d			(ebx+XFileCell.udpRelSram),0 // ungültig machen neuer speicher anlegen
		l.d			(ebx+XFileCell.udCRC32),0	 // loeschen damit nicht mehr gefunden wird

		PUSH		EBX
		L.EBX		pRamFile
		L.W			(EBX+XFILEHDR.uiChange),16#1234		// merker das Änderung
		POP			EBX

		L.D			(EDI+XRamCell.udType) ,0	 // daten auch loaschen
		L.D			(EDI+XRamCell.udData) ,0
	   	L.D			(EDI+XRamCell.udCRC32),0
		L.D			(EDI+XRamCell.udChk)  ,1	 // damit chk sicher falsch ist 

chk2nxt
		add.ebx		sizeof (XFileCell)	
		loop		chk2lp		
chk2lp_end
// RAM allociert und als belegt gemeldet jetzt kommen die Konstruktoren der 
// RAM und RAMEX und wollen sich die Pointer holen					
		JMP			S_RAM_CHK_RET

chk_s_ram_error
		L.EAX		fDoNotReportErrors
		O.EAX		EAX
		JNZ			Keine_Fehlermeldung_ins_Logfile
		//*************************
		PUSHAD
		PUSHFD
		CALL		LogSramError
		POPFD
		POPAD
		//*************************
Keine_Fehlermeldung_ins_Logfile

		L.D			wasValid, 0

// im file alles auf 0
		L.EDI		pRamFile
		l.d			(EDI+XFILEHDR.udEntries),0
		l.d			(EDI+XFILEHDR.CRC32),0
		l.d			(EDI+XFILEHDR.pLastEntry),0
		l.w			(EDI+XFILEHDR.uiChange),16#1234		// merker das Änderung

// auch im ram alles auf 0 
   		L.EDI 		_S_RAM_Hptr
   		L.D			(EDI+MRAM_DESCR.UsedData),0
   		L.W			(EDI+MRAM_DESCR.DataValid+0),0
   		A.W			(EDI+MRAM_DESCR.DataValid+2),16#8000 // alles außer Reorg-Flag rücksetzen
   		O.W			(EDI+MRAM_DESCR.DataValid+2),2

// und die anzahl der einträge auch loeschen
		L.EDI		(EDI+MRAM_DESCR.DataStart)
		clr			eax
		s.eax		(EDI+XRAMHDR.udEntries) 
		not			eax
		s.eax		(EDI+XRAMHDR.udChk)

		l.edi		_S_RAM_Hptr
		l.EAX		(edi+MRAM_DESCR.DataLength)
		cmp.eax		maxSramSize
		jbe			CHK_S_RAM2
		l.eax		maxSramSize

CHK_S_RAM2
		PUSH		EAX			// result
		PUSH.D		1
		PUSH.D		EAX         // max. SRAM Größe
		CALL		SRAM_STARTUP
		ADD.ESP		2*4
		POP			EAX			// RESULT

		PUSH		EAX		// RESULT
		PUSH.D		8		// XRAMHDR: STRUCT =udEntries UDINT+udChk UDINT;
		CLR			EAX		// OFFSET 0 = 8 Byte	
		PUSH		EAX
		CALL		SRAM_MARKMemory
		ADD.ESP		2*4
		POP			EAX		// Result	

		JMP			S_RAM_CHK_RET

		

CHK_S_RAM1

		l.ecx		(EDI+MRAM_DESCR.DataValid)
		and.ecx		1
		//*****
		L.D			sramErrorCode,EC_NOT_VALID
		//*****
		jz			S_RAM_INVALID		// not valid

    (*DB1875*)
		l.eax		(EDI+MRAM_DESCR.DataLength)
		l.ecx		(EDI+MRAM_DESCR.UsedData)
    add.eax (EDI+MRAM_DESCR.DataStart)  // Header ist in UsedData mit eingerechnet
    sub.eax EDI
    cmp.ecx eax
		//*****
		L.D			sramErrorCode,5
		//*****
		jg			S_RAM_INVALID

   		L.EDI		(EDI+MRAM_DESCR.DataStart)
		S.EDI		RamStart
		A.D			(EDI+XRAMHDR.aData[X_RAM_CELLS].udType),16#80FFffff  // 16*65535 Entries Total(used for quicksearch)

		
//- Header prüfen --

HDR_CHK_LP		
		L.ECX		(EDI+XRAMHDR.udEntries) 
		CMP.ECX		X_RAM_CELLS		
		//*****
		L.D			sramErrorCode,EC_INV_BLK_ENTRIES
		//*****
		JA			S_RAM_INVALID		// more than 500 entries in one list !
		L.EAX		ECX
		NOT         EAX
		CMP.EAX		(EDI+XRAMHDR.udChk)	// CHKSUMME nicht OK
		//*****
		L.D			sramErrorCode,EC_INV_BLK_CHKSUM
		//*****
		JNE			S_RAM_INVALID
		jecxz		S_RAM_INITIALIZE_WITH_ONE_BLOCK	// #FA kommt vor weil active.dat nur header hat und ekien eintrage

//-- Speicherblock belegen  mit länge eines blockes !

		PUSH		EAX		// RESULT
		PUSH.D		SIZEOF(XRAMHDR)		// resu:=SRAM_MARKMEMORY(STartadr,LNG)
		L.EAX		EDI
		SUB.EAX		RamStart
		PUSH		EAX
		CALL		SRAM_MARKMemory
		ADD.ESP		2*4
		POP			EAX		// Result	

		L.EDX		ECX

	   	L.EBX		XRAMHDR.aData[0]

S_CELL_CHK_LP
		L.EAX		(EDI+EBx+XRamCell.udType)			// ValidFlg Must Be High !
		O.EAX		EAX
		JNS			S_CELL_NOT_VALID					// Call not VALD (bit 31=0)

		A.EAX		16#00ffFFFF	
		ADD.EAX		(EDI+EBX+XRamCell.udData)
	   	ADD.EAX		(EDI+EBX+XRamCell.udCRC32)
		CMP.EAX		(EDI+EBX+XRamCell.udChk)
		Jne			S_CELL_NOT_VALID
		
		A.D			(EDI+EBX+XRamCell.udType),NOT RamUsed  // Clr Used Bit

//    CHK FOR Xtended Ram // 		
		L.EAX		(EDI+EBX+XRamCell.udType)
		A.EAX		16#3C00_0000
		JZ			CHK_NEXT_CELL

		CMP.EAX		IsXram
		JNE			S_CELL_NOT_VALID
		
//        resu:=SRAM_MARKMEMORY(STartadr,LNG)

		PUSH		EAX		// RESULT

//     #FA vorsicht die length hat 2 teile byte0/1 ist die allocated length
//                                         byte2/3 ist die used length
//										   also ein mask ist wichtig			     

		l.eax		(EDI+EBX+XRamCell.udData) // = LÄNGE
		and.eax		16#ffff                   // mehr geht nicht
		PUSH		EAX
		L.EAX		(EDI+EBX+XRamCell.udType) // STartoffset
		A.EAX		16#00FF_ffff
		PUSH		EAX
		CALL		SRAM_MARKMemory
		ADD.ESP		2*4
		POP			EAX		// Result	
		JMP			CHK_NEXT_CELL



//--- contens of cell not valid , mark it as free (unused) --		
		
S_CELL_NOT_VALID
				
		L.D			(EDI+EBX+XRamCell.udType) ,0
		L.D			(EDI+EBX+XRamCell.udData) ,0
	   	L.D			(EDI+EBX+XRamCell.udCRC32),0
		L.D			(EDI+EBX+XRamCell.udChk)  ,0
#ifdef SRAM_REPORT_INVALID_CELL1
    // Wenn ein Fehler bei einer Format-1 Zelle gefunden wird, dann wird standardmäßig 
    // der Zelleninhalt auf 0 gesetzt und kein Sram Fehler gemeldet. 
    // Warum wird kein Fehler gemeldet ?
    // Es hat einmal eine Loaderversion gegeben, bei der eine Sram-Zelle die noch nie 
    // beschrieben wurde (also den Init-Wert aufweist), eine ungültige Chksum hatte. 
    // Fürs Projekt hatte das keine neg. Auswirkungen, da die Chksum bereichtigt wurde, 
    // sobald die Zelle beschrieben wurde (siehe Änderung in 02.02.102, SA17043).
    // Würde man jetzt bei einer falschen Chksum in einer Zelle einen Sram-Fehler melden 
    // und das ganze Sram resettieren, dann könnte bei funktionierenden Applikationen 
    // das Sram zurückgesetzt werden.
    JMP     S_RAM_INVALID
#endif    
		

// -- Next CELL

CHK_NEXT_CELL		
		ADD.EBX  	SIZEOF(XRamCell)
		LOOP		S_CELL_CHK_LP


// -- CHK FOR NEXT BLOCK --

		CMP.EDX		X_RAM_CELLS				// entry nr must be X_RAM_CELLS
		JB			S_RAM_CHK_RET

	   	CMP.D		(EDI+EBX+XRamCell.udCRC32),1954
		JNE			NO_MORE_BLOCKS
	    L.EAX      	(EDI+EBX+XRamCell.udType)   // MUST BE VALID
		O.EAX		EAX
		JNS			NO_MORE_BLOCKS
		A.EAX		16#00ff_FFFF							// chk summ Must Be ok
		L.EAX		(EDI+EBX+XRamCell.udData)
	   	ADD.EAX		(EDI+EBX+XRamCell.udCRC32)
		CMP.EAX		(EDI+EBX+XRamCell.udChk)
		JNE			NO_MORE_BLOCKS

		L.EAX		(EDI+EBX+XRamCell.udData)
		L.EDI		RamStart
		ADD.EDI		EAX
		JMP			HDR_CHK_LP

				

// Mark Cell 500 As EMPTY		
		
NO_MORE_BLOCKS
		L.D			(EDI+EBX+XRamCell.udType) ,0
		L.D			(EDI+EBX+XRamCell.udData) ,0
	   	L.D			(EDI+EBX+XRamCell.udCRC32),0
		L.D			(EDI+EBX+XRamCell.udChk)  ,0
		JMP			S_RAM_CHK_RET


//--------------------------------------------------
//-- S-RAM is Invalid ,initialise it with ONE BLOCK
//--------------------------------------------------


S_RAM_INVALID
		L.EAX		fDoNotReportErrors
		O.EAX		EAX
		JNZ			Keine_Fehlermeldung_ins_Logfile2
		//*************************
		PUSHAD
		PUSHFD
		CALL		LogSramError
		POPFD
		POPAD
		//*************************
Keine_Fehlermeldung_ins_Logfile2

		L.D			wasValid, 0

S_RAM_INITIALIZE_WITH_ONE_BLOCK
   		L.EDI 		_S_RAM_Hptr
   		L.D			(EDI+MRAM_DESCR.UsedData),0
   		L.W			(EDI+MRAM_DESCR.DataValid+0),0
   		A.W			(EDI+MRAM_DESCR.DataValid+2),16#8000 // alles außer Reorg-Flag rücksetzen


//	  SRAM_STARTUP(	16#40000 , 1); 
		
		l.EAX		(edi+MRAM_DESCR.DataLength)
		cmp.eax		maxSramSize
		jbe			S_RAM_INVALID1
		l.eax		maxSramSize
				
S_RAM_INVALID1
		PUSH		EAX			// result
		PUSH.D		1
		PUSH		EAX  		// max. SRAM Größe
		CALL		SRAM_STARTUP
		ADD.ESP		2*4
		POP			EAX			// RESULT

//		 RelAdress:=InitXramBlock();

		PUSH		EAX
		CALL		InitXramBlock
		POP			EAX			// RESULTAT



//--- CHK IS Finished, Find Length of data --

S_RAM_CHK_RET
	    CALL		GET_USED_S_RAM
   		L.EDI 		_S_RAM_Hptr

END_FUNCTION
#endif

//-------------------------------------------------------------
// Whenever new S-RAM is alocatetd,S_Ram saver must be informed
//-------------------------------------------------------------

FUNCTION GLOBAL GET_USED_S_RAM
VAR
     usedLength : UDINT;
END_VAR

  IF !!hasSram THEN
    RETURN;
  END_IF;

  // Belegte Speichergrösse (bezogen auf speicherstart) abfragen
  usedLength := SRAM_GetLastAddr();
  // Größe des Headers dazuzählen
  usedLength := usedLength + _S_RAM_Hptr^.DataStart$UDINT - _S_RAM_Hptr$UDINT;

  // UsedData im _S_RAM_Hptr aktualisieren
  _S_RAM_Hptr^.UsedData := usedLength;

  // Wenn das SRAM Reorg aktiviert ist, dann wird das DataValid Flag
  // erst gesetzt, wenn alle RAM- und RAMEX- Zellen angelegt worden sind.
  IF !!fReorgSram | fSramInitCompleted THEN
    // Die unteren 16 Bit von DataValid auf den Wert 1 setzen
    _S_RAM_Hptr^.DataValid$UINT := 1;
    // Reorg-Flag (oberstes Bit von DataValid) rücksetzen
    _S_RAM_Hptr^.DataValid := _S_RAM_Hptr^.DataValid AND 0x7FFF_FFFF;
  END_IF;

END_FUNCTION

//-------------------------
// Calc CKSUM of RAMEX Hdr End MakeIt Valid
//-------------------------

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL MakeRamExChk
VAR_INPUT
  pXramCell : ^XramCell;
END_VAR
VAR
	udChk : UDINT;
END_VAR

  udChk := pXramCell^.udType AND 16#00ffFFFF;
  udChk += pXramCell^.udData;
  udChk += pXramCell^.udCRC32;
  
  pXramCell^.udChk := udChk;
  pXramCell^.udType := pXramCell^.udType OR (IsXram+RamValid+RamUsed);

END_FUNCTION
#else
FUNCTION GLOBAL AWL MakeRamExChk
VAR_INPUT
  pXramCell : ^XramCell;
END_VAR

  L.EDI     pXramCell
  L.EAX     (EDI+XRamCell.udType)		
  L.EDX     EAX
  A.EAX     16#00ffFFFF	
  ADD.EAX   (EDI+XRamCell.udData)
  ADD.EAX   (EDI+XRamCell.udCRC32)
  S.EAX     (EDI+XRamCell.udChk)
  O.EDX     (IsXram+RamValid+RamUsed)
  S.EDX     (EDI+XRamCell.udType)
  
END_FUNCTION
#endif


//-------------------------------
// Diese funktion erzeugt einen neuen ramblock mit 501 einträgen 
// beim aufruf zeigt ein pointer auf die letzte zelle des akuellen blockes
// hier wird im feld "Data" die relative start_adresse des neuen blockes eingetregen
// als crc 32 steht  1954
// Die chk besteht wieder als summe von Data und CRC
// 
//------------------------------

FUNCTION GLOBAL IniNewRamBlock
VAR_INPUT
  PtLastRamcell :^xRamCell;
END_VAR
VAR_OUTPUT
  Reladress :UDINT;
END_VAR

  RelAdress:=InitXramBlock();
  If Reladress <> 0 then
    PtLastRamcell^.udCRC32:=1954;
    PtLastRamcell^.udData$UDINT:=RelAdress$UDINT;
    PtLastRamcell^.udChk$UDINT:=PtLastRamcell^.udData$UDINT+PtLastRamcell^.udCRC32;
    PtLastRamcell^.udType:=PtLastRamcell^.udType or 16#80000000;      // valid 
  END_IF;		

END_FUNCTION


//-------------------------------------
// Initialise one block of Xram entries
//-------------------------------------
//  1. Ramdatenblock (hdr+501 eintrage) anfordern
//  2. nr Einträge auf 0 setzen
//  3. Alle blöcke mit 0 füllen  
//  
//
FUNCTION InitXramBlockEx
VAR_INPUT
  pGotMemory : ^BOOL;  // Da beim Returnwert 0 nicht unterschieden werden kann, ob 
                       // der Offset 0 gemeint ist oder Out-Of-Memory, wird mit diesem 
                       // Flag angezeigt, ob Speicher allokiert werden konnte.
END_VAR
VAR_OUTPUT
  xBlockOffset :UDINT ; // Offset of header 
END_VAR

VAR
	I			  : DINT;
	PtxCell		  : ^XRamCell;
	ptxRamHead	  : ^XRAMHDR;
END_VAR

  xBlockOffset$^usint:=( SRAM_GetMemoryEx(sizeof(xRamHdr), pGotMemory));
  IF pGotMemory^ THEN
    ptxRamHead:= _S_RAM_Hptr^.DataStart+xBlockOffset; 		 

    ptxRamHead^.udEntries:=0;
    ptxRamHead^.udChk:= 0;                     	// #FA damit kein compiler Fehler
    ptxRamHead^.udChk:= NOT ptxRamHead^.udChk; 	// value := not value (siehe doku)
    PtxCell:=#ptxRamHead^.aData[0];
    
    For i:= 0 to X_RAM_CELLS do
      PtxCell^.udChk:=0;
      PtxCell^.udCRC32:=0; 
      PtxCell^.udData:=0;
      PtxCell^.udType:=16#0; 					// invalid 
      PtxCell+=SIZEOF(XRamCell); 		
    end_for;
  END_IF;

END_FUNCTION

FUNCTION GLOBAL InitXramBlock
VAR_OUTPUT
  xBlockOffset :UDINT ; // Offset of header 
END_VAR
VAR
	gotMemory : BOOL;
END_VAR

  xBlockOffset := InitXramBlockEx(#gotMemory);

END_FUNCTION

FUNCTION GLOBAL GetVersion_sRam
VAR_OUTPUT
	Version		: UDINT;
END_VAR

  IF !!hasSram THEN
    Version := 0;
  ELSE
    Version := (_S_RAM_Hptr^.DataValid SHR 16) AND 16#7FFF; // das oberste Bit (Reorg-Flag) ausblenden
  END_IF;

END_FUNCTION

#ifdef _LSL_TARGETARCH_ARM
// ---------------------------------------------------------------
// Funktion überprüft bei RAMex ob CRC im File = CRC im RAM
//
// Wenn die CRC im File nicht mit der CRC im RAM übereinstimmt, 
// dann werden alle Einträge im File durchgegangen und über deren 
// Verweis ins SRAM (udpRelSram) nachgeschaut, ob man eine Ramex-
// Zelle im SRAM mit der fraglichen CRC findet. Wenn eine gefunden 
// wird, dann werden bei dem zu prüfenden und dem gefundenen Eintrag 
// im File die Verweise ins SRAM (udpRelSram) ausgetauscht.
// 
// Anm.pr:
// Das Aktualisieren der Verweises ins SRAM wird wegen EXCHANGE_DATA_PTR 
// in der RamEx gemacht, wo die Daten von 2 Ramexobjekten vertauscht 
// werden (füer IPR genützt)
// ---------------------------------------------------------------
FUNCTION CRC_extended
VAR_INPUT
  pXFileCell : ^XFileCell;
	pRamFile0	: ^XFILEHDR;
	_S_RAM_Hptr0 : ^MRAM_DESCR;
END_VAR
VAR_OUTPUT
  result : BOOL;
END_VAR
VAR
  pXRamCell : ^XRamCell;
  pXFileCellTemp : ^XFileCell;
  i : UDINT;
  udCRC32 : UDINT;
  oldUdpRelSram : UDINT;
  ramStart :^XRamHdr;
END_VAR

  result := FALSE;

  ramStart := _S_RAM_Hptr0^.DataStart;

  udCRC32 := pXFileCell^.udCRC32;
  pXRamCell := ramStart + pXFileCell^.udpRelSram;
  IF udCRC32 = pXramCell^.udCRC32 THEN
    result := TRUE;
    RETURN;
  END_IF;
  
  // CRC stimmt nicht überein -> Zellen im RAM nach der CRC durchsuchen 
  // und wenn gefunden, die relative Ram-Adresse in der File-Zelle aktualisieren
  pXFileCellTemp := #pRamFile0^.aData[0];
  FOR i := 1 TO pRamFile0^.udEntries DO 
    pXRamCell := ramStart + pXFileCellTemp^.udpRelSram;
    IF udCRC32 = pXramCell^.udCRC32 THEN
      // udpRelSram wird ausgetauscht
      oldUdpRelSram := pXFileCell^.udpRelSram;
      pXFileCell^.udpRelSram := pXFileCellTemp^.udpRelSram;
      pXFileCellTemp^.udpRelSram := oldUdpRelSram;
      result := TRUE;
      RETURN;
    END_IF;
    pXFileCellTemp += sizeof(XFileCell);
  END_FOR;

END_FUNCTION
#else
// ---------------------------------------------------------------
// funktion überprüft bei RAMex ob CRC im File = CRC im RAM
// ---------------------------------------------------------------
// --> ECX zeigt auf die Ramadresse(SRAM)
//     EAX = CRC
FUNCTION AWL CRC_extended
VAR
  oldUdpRelSram : UDINT;
END_VAR

		cmp.eax		(ecx+XRamCell.udCRC32)
		je			CRC_extend_ret // die CRC stimmt -> return

    // die CRC stimmt nicht -> File-Zellen durchgehen und über udpRelSram im Sram die Sram-Zelle mit der CRC suchen
		push		ebx
		push		edi
    
    l.ebx		(ebx+edi+XFileCell.udpRelSram)
    s.ebx   oldUdpRelSram // udpRelSram merken, damit man einen Austausch durchführen kann

		l.ecx		(EDI+XFILEHDR.udEntries)
		l.ebx		XFILEHDR.aData
Find_crcextend_loop		
		l.edx		(edi+ebx+XFILECELL.udtype)
		and.edx		16#3C00_0000
		cmp.edx		isXram
		jne			find_crcextended_next

// now we found an extended RAM
// check if this ram has the right CRC	

		l.EDX		_S_RAM_Hptr                 //++pr:Ich glaube hier sollte man besser nicht _S_RAM_Hptr verwenden, 
                                        //     sondern den vom Aufrufer verwendeten Parameter _S_RAM_Hptr0 !
		L.EDX		(EDX+MRAM_DESCR.DataStart)
		ADD.EDX		(ebx+edi+XFileCell.udpRelSram)  // pointer to XramCell

		cmp.eax		(EDX+XRamCell.udCRC32)
		jne			find_crcextended_next			// --> crc is different
				
// wir haben das richtige gefunden 
		l.ecx		edx
    // werte in updRelSram austauschen
		l.eax		(ebx+edi+XFileCell.udpRelSram)  
    l.edx   oldUdpRelSram
    s.edx		(ebx+edi+XFileCell.udpRelSram)  
    
		pop			edi
		pop			ebx
		s.eax		(ebx+edi+XFileCell.udpRelSram)  // store the real pointer
		clc
		jmp			CRC_extend_end


find_crcextended_next
		add.ebx		sizeof (XFileCell)
		loop		Find_crcextend_loop				// weitersuchen
		pop			edi
		pop			ebx
		stc
		jmp			CRC_extend_end
		
CRC_extend_ret
		clc
CRC_extend_end
END_FUNCTION
#endif

//------------------------------------------------------------------------------
// Allokiert eine Ramzelle (für SRAM Version 2 = SRAM + FILE)
//	a) RAM
//		Im SRAM werden 4 Bytes belegt,
//		Im FILE werden 12 Bytes belegt
//	b) RAMEX
//		Im SRAM werden 16 Bytes belegt,
//		Im FILE werden 12 Bytes belegt
// Die Zelle im FILE (XRamCell) enthält die Offset-Adresse der Zelle im SRAM.
// Das Flag uiChange des File-Headers wird auf 16#1234 gesetzt, um anzuzeigen, 
// das der File-Cache (pRamFile) aufs File geschrieben werden muß.
//
// Der SRAM Speicher für die RAMEX Daten wird hier nicht allokiert, dafür ist 
// der Aufrufer zuständig.
//------------------------------------------------------------------------------
FUNCTION Add_sRamFile_I
VAR_INPUT
	CRC32		:  UDINT;
	ppRamAdr	: ^UDINT;
	ppFileAdr	: ^pXFileCell_t;
	uiType		: UDINT;				// 0=RamZelle 1=RamExZelle 	
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR
VAR
	nAlloc : UDINT;
  pRam : ^USINT;
  pXFileCell : ^XFileCell;
  pXRamCell : ^XRamCell;
  allocSize : UDINT;
END_VAR

  state := 0; // auf o.k.
  
  IF uiType = 0 THEN
    nAlloc := 4;
  ELSE
    nAlloc := 16;
  END_IF;
  
  IF pRamFile^.udEntries < X_FILE_CELLS_2 THEN
    IF pRamFile^.udEntries = X_FILE_CELLS & LDR_allocated_XFileCells = X_FILE_CELLS THEN
      allocSize := XFILEHDR.adata + X_FILE_CELLS_2 * SIZEOF(XFileCell);
    	pRamFile := _realloc_LDR(pRamFile, allocSize)$^XFILEHDR; 
      IF pRamFile <> NIL THEN
        LDR_allocated_XFileCells := X_FILE_CELLS_2;
        LDR_allocated_XFileSize := allocSize;
      END_IF;
    END_IF;
    pRam := SRAM_GetMemory(nAlloc);
  ELSE
    TRACE_ERR("Too many RAM objects");
    pRam := NIL;
  END_IF;
  IF pRam = 0 THEN
    ppRamAdr^ := NIL; // kein memory pointer
    ppFileAdr^ := NIL; // kein memory pointer
    state := 16#FFFFffff; // auf fehler stellen
    LDR_outOfSramMem := 1; // im Flag merken, dass keine Sram-Speicher mehr vorhanden ist
    RETURN;
  END_IF;

  pXFileCell := #pRamFile^.aData[pRamFile^.udEntries];
  
  //pr:
  //  Hier müsste GET_USED_S_RAM aufgerufen werden, weil SRAM_GetMemory 
  //  etwas allokiert hat. Aus Performancegründen wird dieser Aufruf aber 
  //  in die Funktion LDR_FreeSramKopie ausgelagert. Diese Funktion wird 
  //  am Ende der SRAM Initialisierung aufgerufen.

  // merken im File
  pXFileCell^.udpRelSram := pRam$UDINT;
  
  _S_RAM_Hptr^.DataStart^.udEntries += 1;
  _S_RAM_Hptr^.DataStart^.udChk := NOT _S_RAM_Hptr^.DataStart^.udEntries;

  // jetzt absolut pointer berechnen
  pXRamCell := _S_RAM_Hptr^.DataStart + pRam$UDINT;
  ppRamAdr^ := pXRamCell$UDINT; // rückgabewert
  
  // wenn wir 16 byte haben muessen wir jetzt auch die CRC eintragen und alles andere loeschen
  IF uiType <> 0 THEN
		// wir reservieren 16 byte und tun so 
		// als wäre das eine alte XramCell damit ist RamEx fast kompatibel
    pXRamCell^.udCRC32 := CRC32;
    pXRamCell^.udType := 0;
    pXRamCell^.udData := 0;
    pXRamCell^.udChk := 0;
    
    pXFileCell^.udType := ISXRAM;
  ELSE
    pXFileCell^.udType := 0;
  END_IF;
  pXFileCell^.udCRC32 := CRC32;
  
  ppFileAdr^ := pXFileCell; // rückgabewert

  pRamFile^.udEntries += 1; // eintrag erhöhen am ende
  pRamFile^.uiChange := 16#1234;	// merker das Änderung

END_FUNCTION

// ---------------------------------------------------------------------
//  suchen der eines Eintrages im File und daraus die Pointer ableiten
// ---------------------------------------------------------------------
//
//  Achtung bei uiType = 1 kann wenn die Ramex ein IPR Programm ist
//  durch exchange dataptr in der Ramex es passiren das die Ramex 
//  jetzt woanders ist um somit der offset nicht mehr stimmt !!
//  Dann wird muessen alle Ramex durchsucht werden um herauszufinden 
//  wo der neue steckt !!
//
#ifdef _LSL_TARGETARCH_ARM
FUNCTION Find_sRamFile_I
VAR_INPUT
	pRamFile0	: ^XFILEHDR;
	_S_RAM_Hptr0: ^MRAM_DESCR;
	CRC32		:  UDINT;
	ppRamAdr	: ^UDINT;
	ppFileAdr	: ^pXFileCell_t;
	uiType		: UDINT;				// 0=RamZelle 1=RamExZelle 	
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR
VAR
	nLoops : UDINT;
  pXFileCell : ^XFileCell;
  pLastEntry : UDINT;
END_VAR

  state := 0; // auf o.k.
  
  IF (pRamFile0^.pLastEntry > 0) & (pRamFile0^.pLastEntry < pRamFile0^.udEntries) THEN 
    // wir haben uns den letzten stand der dinge vom suchen gemerkt und um 
    // schneller zu sein suchen wir ab hier und dann erst vom beginn
    // ist bei vielen ramzellen 10.000 mal schneller
    nLoops := 2;
  ELSE
    // suchen von vorne
    pRamFile0^.pLastEntry := 0;
    nLoops := 1;
  END_IF;

  WHILE nLoops > 0 DO 
  
    pLastEntry := pRamFile0^.pLastEntry;
    pXFileCell := #pRamFile0^.aData[pLastEntry];
    WHILE pLastEntry < pRamFile0^.udEntries DO 
      pLastEntry += 1;
      
      IF (pXFileCell^.udCRC32 = CRC32) & ((uiType = 0) | (CRC_extended(pXFileCell, pRamFile0, _S_RAM_Hptr0))) THEN
        // gefunden jetzt geben wir die pointer zurück
        ppRamAdr^$UDINT := _S_RAM_Hptr0^.DataStart$UDINT + pXFileCell^.udpRelSram;
        ppFileAdr^ := pXFileCell;
        
        pRamFile0^.pLastEntry := pLastEntry;
        RETURN;
      END_IF;
      
      pXFileCell += sizeof(XFileCell);
    END_WHILE;
  
    // Zweiter Schleifendurchlauf von vorne
    pRamFile0^.pLastEntry := 0;
    nLoops -= 1;
    
  END_WHILE;

  // **************************************** //
  // nicht vorhanden --> neuen erfinden       //
  // **************************************** //

	state := 16#FFFFFFFE;			// auf nicht gefunden stellen

END_FUNCTION
#else
FUNCTION AWL Find_sRamFile_I
VAR_INPUT
	pRamFile0	: ^XFILEHDR;
	_S_RAM_Hptr0: ^MRAM_DESCR;
	CRC32		:  UDINT;
	ppRamAdr	: ^UDINT;
	ppFileAdr	: ^pXFileCell_t;
	uiType		: UDINT;				// 0=RamZelle 1=RamExZelle 	
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR

		l.d			state,0						// auf o.k.

		push		esi
		l.edi		pRamFile0
		l.ecx		(EDI+XFILEHDR.udEntries)
		jecxz		Find_sRamFile_add			// --> neues eintragen

// wir haben uns den letzten stand der dinge vom suchen gemerkt und um 
// schneller zu sein suchen wir ab hier und dann erst vom beginn
// ist bei vielen ramzellen 10.000 mal schneller

		cmp.d		(edi+XFILEHDR.pLastEntry),0
		je			find_sRamFile_start0		// --> wir fangen bei 0 an 
		sub.ecx		(edi+XFILEHDR.pLastEntry)
		js			find_sRamFile_start0		// --> von vorne suchen
		jecxz		find_sRamFile_start0		// --> von vorne suchen

		l.ebx		(edi+XFILEHDR.pLastEntry)
		l.eax		sizeof (XFileCell)
		mul			ebx
		l.ebx		eax
		add.ebx		XFILEHDR.aData
		l.eax		CRC32

Find_sRamFile_loop0
		cmp.eax		(ebx+edi+XFileCell.udCRC32)
		jne			Find_sRamFileNext0

		inc.d		(edi+XFILEHDR.pLastEntry)		// einer weiter vor 

// gefunden jetzt geben wir die pointer zurück

		l.ecx		_S_RAM_Hptr0
		L.ECX		(ECX+MRAM_DESCR.DataStart)
		ADD.ECX		(ebx+edi+XFileCell.udpRelSram)

// falls type = 1 check ob auch CRC übereinstimmt
		cmp.d		uiType,0
		je			find_sramStd
		call		CRC_extended		     // pruefen ob wirklich der richtige
		jc			find_sRamFile_start0
find_sramStd
		l.EAX		ppRamAdr
		s.ECX		(EAX)

		l.eax		ppFileAdr
		add.ebx		edi
		s.ebx		(eax)

		jmp			Find_sRamFile_ret

Find_sRamFileNext0
		add.ebx		sizeof (XFileCell)
		// merken wo wir waren
		inc.d		(edi+XFILEHDR.pLastEntry)
		loop		Find_sRamFile_loop0

// hier starten wir ganz von vorne und durchsuchen alles

find_sRamFile_start0
		l.ecx		(EDI+XFILEHDR.udEntries)
		clr			edx
		l.ebx		XFILEHDR.aData
		l.eax		CRC32
Find_sRamFile_loop
		cmp.eax		(ebx+edi+XFileCell.udCRC32)
		jne			Find_sRamFileNext
// merken wo wir waren
		s.edx		(edi+XFILEHDR.pLastEntry)


// gefunden jetzt geben wir die pointer zurück

		l.ecx		_S_RAM_Hptr0
		L.ECX		(ECX+MRAM_DESCR.DataStart)
		ADD.ECX		(ebx+edi+XFileCell.udpRelSram)

// falls type = 1 check ob auch CRC übereinstimmt
		cmp.d		uiType,0
		je			find_sramStdB
		call		CRC_extended		     // pruefen ob wirklich der richtige
		jc			Find_sRamFile_add
find_sramStdB

		l.EAX		ppRamAdr
		s.ECX		(EAX)

		l.eax		ppFileAdr
		add.ebx		edi
		s.ebx		(eax)

		jmp			Find_sRamFile_ret

Find_sRamFileNext
		add.ebx		sizeof (XFileCell)
		inc			edx
		loop		Find_sRamFile_loop

// **************************************** //
// nicht vorhanden --> neuen erfinden       //
// **************************************** //

Find_sRamFile_add
		l.d			state,16#FFFFFFFE			// auf nicht gefunden stellen
		jmp			Find_sRamFile_ret
		
Find_sRamFile_ret
		pop			esi
END_FUNCTION
#endif

#ifdef LDR_RETSVR
(* Rundet einen Wert auf den nächsten 2^x Wert auf (z.B. 100 auf 128, 200 auf 256) *)
FUNCTION RoundUpPow2
VAR_INPUT
	value	: UDINT;
END_VAR
VAR_OUTPUT
	result : UDINT;
END_VAR

  result := 1;
  WHILE value > 0 DO
    value := value SHR 1;
    result := result SHL 1;
  END_WHILE;

END_FUNCTION

FUNCTION Add_File_I
VAR_INPUT
	CRC32		:  UDINT;
	ppRamAdr	: ^void;
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR
var
  Cell : XRamCell;
end_var

  state := 2; // hat nicht funktioniert!
  
  // Zelle vorbereiten
  Cell.udChk := 0;
  Cell.udCRC32 := CRC32;
  Cell.udData := 0;
  Cell.udType := _S_RAM_Hptr_File^.UsedData;  // im udType wird die Fileposition gespeichert!
  
  // Fileimage updaten
  
  _S_RAM_Hptr_File^.UsedData += sizeof( XRamCell );
  _S_RAM_Hptr_File := ReAlloc_S_RAM_Hptr_File(_S_RAM_Hptr_File, _S_RAM_Hptr_File^.UsedData);
  if( _S_RAM_Hptr_File = NIL )then
    return;
  end_if;
  _memcpy( _S_RAM_Hptr_File + Cell.udType, #Cell, sizeof( XRamCell ) );
  
  // Reorg. Image updaten
  
  _S_RAM_Hptr_File_New := ReAlloc_S_RAM_Hptr_File(_S_RAM_Hptr_File_New, _S_RAM_Hptr_File_New^.UsedData + sizeof( XRamCell ));
  if( _S_RAM_Hptr_File_New = NIL )then
    return;
  end_if;
  Cell.udType := _S_RAM_Hptr_File_New^.UsedData;
  _memcpy( _S_RAM_Hptr_File_New + _S_RAM_Hptr_File_New^.UsedData, #Cell, sizeof( XRamCell ) );
  _S_RAM_Hptr_File_New^.UsedData += sizeof( XRamCell );
  
  // Rückgabe
  _memcpy( ppRamAdr, #Cell, sizeof( XRamCell ) );
  state := 1;
  
END_FUNCTION

VAR_PRIVATE
  lastFindFileIndex : UDINT;
END_VAR

FUNCTION Find_File_I
VAR_INPUT
  pMem    : ^MRAM_DESCR;
	CRC32		:  UDINT;
	ppRamAdr	: ^void;
  bDontAdd   : BOOL;
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR
var
  pCell   : ^XRamCell;
  nCells : udint;
  i :  udint;
end_var

  // Die Suche wird nicht bei Index 0 sondern bei Index lastFindFileIndex gestartet.
  // lastFindFileIndex steht auf dem Wert vom letzten Aufruf dieser Funktion. 
  // Dadurch wird die Suche beschleunigt, weil die Zellen normalerweise in der Reihenfolge 
  // abgefragt werden, wie sie im Speicher angeordnet sind.
  ppRamAdr^ := NIL;
  state := 1;
  nCells := (pMem^.UsedData - pMem^.DataStart$udint) / sizeof( XRamCell );
  pCell := pMem + pMem^.DataStart$udint + lastFindFileIndex * sizeof( XRamCell );
  
  for i := 1 to nCells do
    if lastFindFileIndex >= nCells then
      lastFindFileIndex := 0;
      pCell := pMem + pMem^.DataStart$udint;
    end_if;
    
    // Zelle gefunden -> ins Reorg. eintragen und den richtigen Pointer zurückgeben
    if( pCell^.udCRC32 = CRC32 )then


      if !!bDontAdd then
        _S_RAM_Hptr_File_New := ReAlloc_S_RAM_Hptr_File(_S_RAM_Hptr_File_New, _S_RAM_Hptr_File_New^.UsedData + sizeof( XRamCell ));
        if( _S_RAM_Hptr_File_New = NIL )then
          return;
        end_if;
        pCell^.udType := _S_RAM_Hptr_File_New^.UsedData;  //den udType brauch ich hier nicht, da kommt der Offset im File rein!
        
        //Reorg.
        _memcpy( _S_RAM_Hptr_File_New + _S_RAM_Hptr_File_New^.UsedData, pCell, sizeof( XRamCell ) );
        
        // Datenlänge im Reorg. korrigieren
        _S_RAM_Hptr_File_New^.UsedData += sizeof( XRamCell );
      end_if;

      // Rückgabe
      _memcpy( ppRamAdr, pCell, sizeof( XRamCell ) );
        
      state := 0;
      exit;
    
    end_if;

    // nächster Eintrag
    lastFindFileIndex += 1;
    pCell += sizeof( XRamCell );
    
  end_for;

END_FUNCTION

// sucht einen remanenten Server im File
FUNCTION GLOBAL Find_FileEx
VAR_INPUT
	CRC32		:  UDINT;
	ppRamAdr	: ^void;
  bDontAdd   : BOOL;
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR
var
 	errCode : DINT;
	rc : DINT;
end_var

  // gibts den Handle überhaupt schon?
  if( hLDR_remSvr = NIL )then
  
    // wenn nicht muss das File zuerst initialisiert werden
    if( InitRetainSvrFile( 0 ) <> 1 )then
      state := 3;
      return;
    end_if;
  end_if;
  
  // Einlesen der remanenten Server und vorbereiten des Reorg.
  rc := LoadSramKopie( #errCode, hLDR_remSvr, 0 );
  if( rc <> 0 )then
    DEBUGLOG2( "%010d Retentive server file I/O error, rc=%d, errCode = 0x%X", rc$UDINT, errCode$udint );
  end_if;
  
  // suchen eines bestimmten Servers im File
  state := Find_File_I( _S_RAM_Hptr_File, CRC32, ppRamAdr, bDontAdd );
  
  if(bDontAdd = 0) then
  	if( state <> 0 ) then
  		// nicht gefunden, neue Zelle anlegen
  		nbrOfCells_notFound += 1;
	  	state := Add_File_I( CRC32, ppRamAdr );
  	end_if;
  end_if;
  
END_FUNCTION

FUNCTION GLOBAL Find_File
VAR_INPUT
	CRC32		:  UDINT;
	ppRamAdr	: ^void;
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR

  state := Find_FileEx(CRC32, ppRamAdr, FALSE);

END_FUNCTION

#endif //LDR_RETSVR

FUNCTION GLOBAL Find_sRamFileEx
VAR_INPUT
	CRC32		:  UDINT;
	ppRamAdr	: ^UDINT;
	ppFileAdr	: ^pXFileCell_t;
	uiType		: UDINT;				// 0=RamZelle 1=RamExZelle 	
  bDontAdd  : BOOL;
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR
VAR
	pRamAdr_Kopie	: ^XRamCell;
	pFileAdr_Kopie	: ^XFileCell;
	// Die folgenden beiden Variablen werden benötigt, weil die beiden Übergabeparameter 
	// ppRamAdr und ppFileAdr nicht als Zeiger auf Zeiger deklariert wurden.
	// Wenn man die Werte auf die beide lokalen Variablen kopiert, dann kann man ohne 
	// umständliche casterei damit arbeiten.
	pRamAddr	: ^XRamCell;
	pFileAdr : ^XFileCell;

	udAllocated : UDINT;
	udLength : UDINT;
	RelAdress		: UDINT;
	pData : ^USINT;
	pData_Kopie : ^USINT;
	errCode : DINT;
	rc : DINT;
    ret_sRamHptr : UDINT;
END_VAR

  // Diese Funktion wird für die RamEx Klasse benötigt und um den Loader kompatibel zu halten
  // 1 bewirkt, dass die Funktion "Find_sRamFileEx" beim Rückgabewert nicht 0 für gefunden liefert,
  // sonder entweder _s_Ram_hptr oder _s_Ram_hptr_Kopie (REORG) um auf die SRAM Daten zugreifen zu können.
  ret_SramHptr := 0;
  if((ppRamAdr <> NIL) & (ppRamAdr^ = 1)) then
    ret_SramHptr := 1;
  end_if;

	IF fReorgSram THEN
		rc := LoadSramKopie(#errCode, 0, 0);
		IF rc <> 0 THEN
			// das Laden de SRAM Kopie hat nicht funktioniert
			fReorgSram := 0;
			DEBUGLOG2("%010d LoadSramKopie failed, rc=%d, errCode = 0x%X" , rc$UDINT, errCode$UDINT);
		END_IF;
	END_IF;

	IF fReorgSram THEN
		// in der Kopie suchen
		state := Find_sRamFile_I(pRamFile_Kopie, _s_Ram_hptr_Kopie, CRC32, ppRamAdr, ppFileAdr, uiType);
	ELSE
		// im SRAM suchen
		state := Find_sRamFile_I(pRamFile, _s_Ram_hptr, CRC32, ppRamAdr, ppFileAdr, uiType);
	END_IF;

	if((state = 0) & (bDontAdd = TRUE)) then
    if(ret_sRamHptr = 1) then
    	if(fReorgSram) then
        state := _s_Ram_hptr_Kopie$UDINT;
      else
        state := _s_Ram_hptr$UDINT;
      end_if;
    end_if;

		return;
	end_if;

	IF state <> 0 | ppRamAdr^ = NIL THEN
		// nicht gefunden (weder im SRAM noch in der Kopie), neue Zelle anlegen
		nbrOfCells_notFound += 1;

    if(bDontAdd = FALSE) then
  		state := Add_sRamFile_I(CRC32, ppRamAdr, ppFileAdr, uiType);
    end_if;
	ELSIF fReorgSram THEN
		// in der Kopie gefunden, Zelle aus der Kopie ins SRAM kopieren
		nbrOfCells_foundInCopy += 1;

		pRamAdr_Kopie := ppRamAdr^$^XRamCell;
		pFileAdr_Kopie := ppFileAdr^;
		ppRamAdr^$^UDINT := NIL;
		ppFileAdr^$^XFileCell := NIL;

    if(bDontAdd = FALSE) then
  		state := Add_sRamFile_I(CRC32, ppRamAdr, ppFileAdr, uiType);
    end_if;
		pRamAddr := ppRamAdr^$^XRamCell;
		pFileAdr := ppFileAdr^$^XFileCell;

		IF state = 0 & pRamAddr <> NIL THEN
			IF uiType = 0 THEN
				// RAM
			    pRamAddr^$dint  :=pRamAdr_Kopie^$dint;
			    pFileAdr^.udType:=16#C0000000;    // ganz am ende machen wir uns gültig !
			ELSE
				// RAMEX
 		     	udLength := pRamAdr_Kopie^.udData shr 16;
				// wenn die Länge der Daten > 0 ist, dann muss das RAMEX Objekt mit 
				// den Daten aus der Kopie vorgeladen und für gültig erklärt werden
				IF udLength > 0 THEN
				 	RelAdress$UDINT:= (SRAM_GetMemory(udLength))$UDINT;
				 	// --- Speicher ist zugewiesen worden --
				 	If RelAdress <> 0 THEN
				   		GET_USED_S_RAM();
				   		udAllocated		      := SRAM_GET_ALLOCATED(udLength);
			  	   		pRAMaddr^.udData$UDINT := (udLength shl 16) or (udAllocated and 16#ffff); 
				   		pRAMaddr^.udType 	  := RelAdress;
				   		pData				  := _S_RAM_Hptr^.DataStart+RelAdress;
				   		pData_Kopie			  := _S_RAM_Hptr_Kopie^.DataStart+(pRamAdr_Kopie^.udType and 16#00FF_ffff);
				   		_memcpy(pData, pData_Kopie, udLength);   // Speicher aus dem Inhalt aus der Kopie vorladen
		           		MakeRamExChk(pRAMaddr);	
			
				 	// -- Kein speicher verfügbar !!
				 	ELSE  

	               		state := 0xFFFFFFFF;			   
			       		ppRamAdr^$^UDINT := NIL;
			       		ppFileAdr^$^XFileCell := NIL;

				 	END_IF;
				ELSE
	           		MakeRamExChk(pRAMaddr);	
				END_IF;
			END_IF;
		END_IF;
	ELSE
		nbrOfCells_foundInSram += 1;
	END_IF;

END_FUNCTION

FUNCTION GLOBAL Find_sRamFile
VAR_INPUT
	CRC32		:  UDINT;
	ppRamAdr	: ^UDINT;
	ppFileAdr	: ^pXFileCell_t;
	uiType		: UDINT;				// 0=RamZelle 1=RamExZelle 	
END_VAR
VAR_OUTPUT
	state		: UDINT;
END_VAR

  state := Find_sRamFileEx(CRC32, ppRamAdr, ppFileAdr, uiType, FALSE);

END_FUNCTION

//------------------------------------------------------------------------------
// Allokiert eine Ramzelle (für SRAM Version 1 = SRAM ohne FILE)
//  Im SRAM werden 16 Bytes belegt
//
// Der SRAM Speicher für die RAMEX Daten wird hier nicht allokiert, dafür ist 
// der Aufrufer zuständig.
//------------------------------------------------------------------------------
#ifdef _LSL_TARGETARCH_ARM
FUNCTION Add_sRam_I
VAR_INPUT
#pragma warning(disable:73)
	CRC32		: UDINT;
#pragma warning(default:73)
END_VAR
VAR_OUTPUT
	pRamAdr		: ^XramCell;
END_VAR
#ifdef _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1
VAR
   RamStart     : ^XramHdr;
   pXramHdr     : ^XRAMHDR;
   RelAdress	  :  UDINT;
END_VAR

  pRamAdr := 0$^XRamCell;
  RamStart := _S_RAM_Hptr^.DataStart;

  // Die folgende Schleife geht die Verkettung der XRAM-Blöcke durch 
  // bis das Ende erreicht ist.
  pXramHdr := RamStart;
  WHILE 1 DO 
		// Abbruch, wenn ein XRAM-Block weniger als 500 XRAM-Zellen enthält
		// oder wenn kein Zeiger auf einen weiteren XRAM-Block existiert.
    IF (pXramHdr^.udEntries < X_RAM_CELLS) | (pXramHdr^.aData[X_RAM_CELLS].udCRC32 <> 1954) THEN
      EXIT;
    END_IF;
    pXramHdr := RamStart + pXramHdr^.aData[X_RAM_CELLS].udData; // offset zum nächsten block
  END_WHILE;

// There is Place in our list, Write It

  IF pXramHdr^.udEntries = X_RAM_CELLS THEN
    // Neuen Block anlegen  und eintragen
    RelAdress := IniNewRamBlock(#pXramHdr^.aData[X_RAM_CELLS]);
    IF RelAdress = 0 THEN
      // out of memory !!!
      LDR_outOfSramMem := 1;    // im Flag merken, dass keine Sram-Speicher mehr vorhanden ist
      RETURN;
    END_IF;
    GET_USED_S_RAM();	// Memory updaten
    pXramHdr := RamStart + RelAdress;
  END_IF;
  
  pRamAdr := #pXramHdr^.aData[pXramHdr^.udEntries];
					
//------------------------------------------------------
// Block belegen und auf ungültig setzen
//------------------------------------------------------

  pRamAdr^.udCRC32 := CRC32;
  pRamAdr^.udData := 0;
  pRamAdr^.udChk := CRC32;
  pRamAdr^.udType := 0; // Type = RAM,Daten nicht gültig
  
  pXramHdr^.udEntries += 1;
  pXramHdr^.udChk := NOT pXramHdr^.udEntries;
#else // _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1
  // Format 1 nicht unterstützt
  pRamAdr := 0$^XRamCell;
#endif  

END_FUNCTION // Add_sRam_I
#else
FUNCTION AWL Add_sRam_I
VAR_INPUT
	CRC32		: UDINT;
END_VAR
VAR_OUTPUT
	pRamAdr		: ^XramCell;
END_VAR
VAR
   RamStart     : ^XramHdr;
END_VAR
		l.d			pRamAdr,0
 		L.EDI 		_S_RAM_Hptr
 		L.EDI		(EDI+MRAM_DESCR.DataStart)
		S.EDI		RamStart
		
		// Die folgende Schleife geht die Verkettung der XRAM-Blöcke durch 
		// bis das Ende erreicht ist. Am Ende zeigt EBX auf 

SearchLoop
		L.EDX     	(EDI+XRAMHDR.udEntries)

		// Abbruch, wenn ein XRAM-Block weniger als 500 XRAM-Zellen enthält
		CMP.EDX		X_RAM_CELLS    					
		JNE			TheEnd

		// Abbruch, wenn kein Zeiger auf einen weiteren XRAM-Block existiert
		CMP.D		(EDI + XRAMHDR.aData[0] + SIZEOF(XRamCell)*X_RAM_CELLS + XRamCell.udCRC32),1954 
		JNE			TheEnd

		// offset zum nächsten block
   		L.EDI		(EDI + XRAMHDR.aData[0] + SIZEOF(XRamCell)*X_RAM_CELLS + XRamCell.udData) 	  	
   		ADD.EDI 	RamStart
   		JMP			SearchLoop			

TheEnd
		IMUL.EBX	EDX, SIZEOF(XRamCell)	
   		ADD.EBX		XRAMHDR.aData[0]
		
// There is Place in our list, Write It
					
		CMP.EBX		XRAMHDR.aData[X_RAM_CELLS]
		JNE			MakeNewEntry
		
// Neuen Block anlegen  und eintragen

		L.EAX		EDI				// pointer auf letzte zelle
		ADD.EAX		EBX

		PUSH		EAX				// place for result
		PUSH		EAX				// Parameter
		CALL		IniNewRamBlock
		POP			EAX				// Parameter
		POP			EAX				// Result

		O.EAX		EAX
		JNZ			GotMemory
		//////////////////////////
		// out of memory !!!    //
		//////////////////////////
		S.EAX		pRamAdr		
		L.D			LDR_outOfSramMem, 1     // im Flag merken, dass keine Sram-Speicher mehr vorhanden ist
		JMP			Add_sRam_I_END

GotMemory
		PUSHAD
		CALL		GET_USED_S_RAM	// Memory updaten
		POPAD

		L.EDI		RamStart
		ADD.EDI		EAX
					
		L.EAX		CRC32
		L.EBX		XRAMHDR.aData[0]


//------------------------------------------------------
// Block belegen und auf ungültig setzen
//------------------------------------------------------

		
MakeNewEntry

		S.EAX		(EDI+EBX+XRamCell.udCRC32)
		L.D			(EDI+EBX+XRamCell.udData),0    
		S.EAX		(EDI+EBX+XRamCell.udChk)

		L.D			(EDI+EBX+XRamCell.udType),16#00000000  // Type = RAM,Daten nicht gültig

		L.ECX		(EDI+XRAMHDR.udEntries)
		INC			ECX 
		S.ECX		(EDI+XRAMHDR.udEntries)
		NOT			ECX
		S.ECX		(EDI+XRAMHDR.udChk) 

		// --- Pointer auf Zelle Mitgeben ---	

		L.EAX		EDI
		ADD.EAX		EBX
		S.EAX		pRamAdr		
	
Add_sRam_I_END

END_FUNCTION // Add_sRam_I
#endif

//---------------------------------------------
// im neuen ram die verbindung zum s-ram suchen
//---------------------------------------------
// Wird von allen S-Ram klassen verwendet um das objekt mit den
// Daten zu verbinden

// auch mit Erweiterung wissen wir ob es ein RAMex ist anhand der Kennung oder nicht

#ifdef _LSL_TARGETARCH_ARM
FUNCTION Find_sRam_I
VAR_INPUT
#pragma warning(disable:73)
	_S_RAM_Hptr0: ^MRAM_DESCR;
	CRC32		: UDINT;
#pragma warning(default:73)
END_VAR
VAR_OUTPUT
	pRamAdr		: ^XramCell;
END_VAR
#ifdef _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1
VAR
   RamStart     : ^XramHdr;

   FirstChain   :^XramCell;  // pointer to first chain
   LastFoundNr	:UDINT;

   DirNr		:UDINT;
   DirIndex		:UDINT; 

   NewFoundNr	:UDINT;

   pXramHdr     : ^XRAMHDR;
   pXramCell	  :^XramCell;
   nRest        : DINT;
   nLoops       : DINT;
END_VAR

  pRamAdr := 0$^XramCell;
  RamStart := _S_RAM_Hptr0^.DataStart;
  
//-- Ptr to Cell 500 of first block, where we store the element nr auf the previous RAM object
  FirstChain := #RamStart^.aData[X_RAM_CELLS];
  
// convert Nr into list + index
  LastFoundNr := FirstChain^.udType AND 16#Fffff;
  
// spezial situation if the first Objekt and List ist empty
  IF (LastFoundNr = 0) & (RamStart^.udEntries = 0) THEN
    RETURN;
  END_IF;
  
// Select startingPoint by getting the right list + Index
  DirNr := LastFoundNr / X_RAM_CELLS;
  DirIndex := LastFoundNr MOD X_RAM_CELLS;
  
//-- Liste suchen (über verkettung)--
  pXramHdr := RamStart;
  WHILE DirNr > 0 DO 
    pXramHdr := RamStart + pXramHdr^.aData[X_RAM_CELLS].udData; // offset zum nächsten block
    DirNr -= 1;
  END_WHILE;
// -- index suchen  -
  pXramCell := #pXramHdr^.aData[DirIndex];

  nRest := pXramHdr^.udEntries$DINT - DirIndex$DINT;
  IF nRest < 0 THEN
    // wenn bei der Subtraktion eine neg.Zahl rauskommt, dann stimmen die Daten im Sram nicht
    RETURN;
  END_IF;

  NewFoundNr := LastFoundNr;
  IF LastFoundNr = 0 THEN 
    // nr 0 suchen wir von vorne
    nLoops := 1;
  ELSE
    // zuerst von LastFoundNr bis ende, dann von vorne suchen
    nLoops := 2;
  END_IF;
  
  WHILE nLoops > 0 DO 
    WHILE nRest > 0 DO 
    
      WHILE nRest > 0 DO 
        NewFoundNr += 1;
        pXramCell += SIZEOF(XRamCell);
        nRest -= 1;
        IF CRC32 = pXramCell^.udCRC32 THEN
          // gefunden
          FirstChain^.udType := RamValid OR NewFoundNr; // memorize where we are now to make search faster
          pRamAdr := pXramCell;
          RETURN;
        END_IF;
      END_WHILE;
      
      // See if there is another list
      IF pXramCell^.udCRC32 = 1954 THEN
        pXramHdr := RamStart + pXramHdr^.aData[X_RAM_CELLS].udData; // offset zum nächsten block
        nRest := pXramHdr^.udEntries$DINT;
      END_IF;
 		
    END_WHILE;
    
    // Zweiter Schleifendurchlauf von vorne
    pXramHdr := RamStart;
    pXramCell := #pXramHdr^.aData[0];
    nLoops -= 1;
    
  END_WHILE;
#else // _LSL_TARGETARCH_ARM_SUPPORTS_SRAMV1
  pRamAdr := 0$^XramCell;
#endif

END_FUNCTION // Find_sRam_I
#else
FUNCTION AWL Find_sRam_I
VAR_INPUT
	_S_RAM_Hptr0: ^MRAM_DESCR;
	CRC32		: UDINT;
END_VAR
VAR_OUTPUT
	pRamAdr		: ^XramCell;
END_VAR
VAR
   RamStart     : ^XramHdr;

   FirstChain   :^XramCell;  // pointer to first chain
   LastFoundNr	:UDINT;

   DirNr		:UDINT;
   Dirindex		:UDINT; 

   LastListAdr	:^XramHdr;  // Memory for end of list 
   LastEleOffs	:^XramCell;
   NewFoundNr	:UDINT;

END_VAR
		l.d			pRamAdr,0
   		L.EDI 		_S_RAM_Hptr0
		L.EAX		(EDI+MRAM_DESCR.DataValid)
		

   		L.EDI		(EDI+MRAM_DESCR.DataStart)
		S.EDI		RamStart


//-- Ptr to Cell 500 of first block, where we store the element nr auf the previous RAM object

		L.EBX		EDI
		ADD.EBX		XRAMHDR.aData[X_RAM_CELLS]
		S.EBX		FirstChain



// convert Nr into list + index

		L.EAX		(EBX+xRamCell.udType) 
		A.EAX		16#Fffff
		S.EAX		LastFoundNr

// spezial situation if the first Objekt and List ist empty

		JNZ			ItIsNotTheFirst
		CMP.D		(EDI+XRAMHDR.udEntries),0
		Jne			ItIsNotTheFirst
		JMP			Find_sRam_I_END


// Select startingPoint by getting the right list + Index

ItIsNotTheFirst	

		CLR			EDX
		L.ECX		X_RAM_CELLS
		DIV			ECX
		S.EAX		DirNr
		S.EDX		DirIndex

	
//-- Liste suchen (über verkettung)--

		O.EAX		EAX
		JZ			Go1
SFLP
   		L.EDI		(EDI+XRAMHDR.aData[X_RAM_CELLS].udData) 	  // offset zum nächsten block
   		ADD.EDI 	RamStart
		DEC			EAX
		JNZ			SFLP

// -- index suchen (ist rest der division) -

Go1		IMUL.EBX	EDX,SIZEOF(XRamCell)	
   		ADD.EBX		XRAMHDR.aData[0]
							
	
//-------------------------------
// -- Von Jetzt bis ende suchen--	
//-------------------------------

//  EDX = INDEX In LISTE


		L.ECX		(EDI+XRAMHDR.udEntries)//EDI:blockPtr
		SUB.ECX		EDX//EDX:DirIndex
    JC      ItsNotInTheList // wenn bei der Subtraktion eine neg.Zahl rauskommt, dann stimmen die Daten im Sram nicht
   		L.EAX		CRC32

		L.EDX        LastFoundNr
		O.EDX		EDX						// nr 0 suchen wir von vorne
		JZ			SearchToEndLoop	

		INC			EDX						// NewFoundNr ab dann immer den nächsten
		ADD.EBX  	SIZEOF(XRamCell)
		LOOP	 	SearchToEndLoop			//--> Suchen, ECX:rest.Anz.Entries im Block

		JMP			SearchToEnd_X


SearchToEndLoop

		CMP.EAX		(EDI+EBX+XRamCell.udCRC32)
		JE			RamFound
		INC			EDX						// NewFoundNr
		ADD.EBX  	SIZEOF(XRamCell)
		LOOP	 	SearchToEndLoop			//--> Suchen
SearchToEnd_X
		CMP.EBX		XRAMHDR.aData[X_RAM_CELLS]
		JNE			NoMatchAtEnd

// See if there is another list
 		
   		CMP.D		(EDI+EBX+XRamCell.udCRC32),1954
		JNE			NoMatchAtEnd

   		L.EDI		(EDI+EBX+XRamCell.udData) 	  // offset zum nächsten block
   		ADD.EDI 	RamStart
		L.ECX		(EDI+XRAMHDR.udEntries)
  		L.EBX		XRAMHDR.aData[0]
   		JMP			SearchToEndLoop			


// We are at the end and found no entry
					
NoMatchAtEnd

   		S.EDI		LastListAdr	 // Memory for end of list 
   		S.EBX		LastEleOffs	
		S.EDX		NewFoundNr

			
//----------------------------
// now we search from beginning
//-----------------------------

		CLR			EDX   // 		 	NewFoundNr
		L.EDI		RamStart
		
		L.ECX		(EDI+XRAMHDR.udEntries)
   		L.EBX		XRAMHDR.aData[0]

SearchFromBegLoop		

  		CMP.EAX		(EDI+EBX+XRamCell.udCRC32) 
   		Je			RamFound

		INC			EDX
		ADD.EBX  	SIZEOF(XRamCell)
		LOOP	 	SearchFromBegLoop		//--> Suchen
		
		CMP.EBX		XRAMHDR.aData[X_RAM_CELLS]
		JNE			ItsNotInTheList

// See if there is another list
 		
   		CMP.D		(EDI+EBX+XRamCell.udCRC32),1954
		JNE			ItsNotInTheList

   		L.EDI		(EDI+EBX+XRamCell.udData) 	  // offset zum nächsten block
   		ADD.EDI 	RamStart
		L.ECX		(EDI+XRAMHDR.udEntries)
   		L.EBX		XRAMHDR.aData[0]

   		JMP			SearchFromBegLoop			
				

//---------------------------------------------------
// Now we definately know that this is an new element		
//---------------------------------------------------
		
ItsNotInTheList				
		JMP			Find_sRam_I_END

// --- Gefunden, Pointer auf Zelle Mitgeben ---	

RamFound

// memorize where we are now to make search faster 

		L.EAX		FirstChain
		O.EDX		RamValid
		S.EDX		(EAX+xRamCell.udType) 


   		L.EAX		EDI
		ADD.EAX		EBX
   		S.EAX		pRamAdr		

Find_sRam_I_END
			
END_FUNCTION // Find_sRam_I
#endif

FUNCTION GLOBAL Find_sRamEx
VAR_INPUT
	CRC32		: UDINT;
  bDontAdd : BOOL;
END_VAR
VAR_OUTPUT
	pRamAdr			: ^XramCell;
END_VAR
VAR
	pRamAdr_Kopie	: ^XramCell;
	udAllocated : UDINT;
	udLength : UDINT;
	RelAdress		: UDINT;
	pData : ^USINT;
	pData_Kopie : ^USINT;
	errCode : DINT;
	rc : DINT;
END_VAR

	IF fReorgSram THEN
		rc := LoadSramKopie(#errCode, 0, 0);
		IF rc <> 0 THEN
			// das Laden de SRAM Kopie hat nicht funktioniert
			fReorgSram := 0;
			DEBUGLOG2("%010d LoadSramKopie failed, rc=%d, errCode = 0x%X" , rc$UDINT, errCode$UDINT);
		END_IF;
	END_IF;

	IF fReorgSram THEN
		// in der Kopie suchen
		pRamAdr := Find_sRam_I(_S_RAM_Hptr_Kopie, CRC32);
	ELSE
		// im SRAM suchen
		pRamAdr := Find_sRam_I(_S_RAM_Hptr, CRC32);
	END_IF;

	IF pRamAdr = NIL THEN
		// nicht gefunden (weder im SRAM noch in der Kopie), neue Zelle anlegen
    if(bDontAdd = FALSE) then
  		nbrOfCells_notFound += 1;
  		pRamAdr := Add_sRam_I(CRC32);
    end_if;
    
	ELSIF fReorgSram THEN
		// in der Kopie gefunden, Zelle aus der Kopie ins SRAM kopieren
		nbrOfCells_foundInCopy += 1;

		pRamAdr_Kopie := pRamAdr;
		pRamAdr := Add_sRam_I(CRC32);
		IF pRamAdr THEN
			IF (pRamAdr_Kopie^.udType AND 16#3C000000) = IsXram THEN
				// RAMEX Zelle
 		     	udLength := pRamAdr_Kopie^.udData shr 16;
				// wenn die Länge der Daten > 0 ist, dann muss das RAMEX Objekt mit 
				// den Daten aus der Kopie vorgeladen und für gültig erklärt werden
				IF udLength > 0 THEN
				 	RelAdress$UDINT:= (SRAM_GetMemory(udLength))$UDINT;
				 	// --- Speicher ist zugewiesen worden --
				 	If RelAdress <> 0 THEN
				   		GET_USED_S_RAM();
				   		udAllocated		      := SRAM_GET_ALLOCATED(udLength);
			  	   		pRAMadr^.udData$UDINT := (udLength shl 16) or (udAllocated and 16#ffff); 
				   		pRAMadr^.udType 	  := RelAdress;
				   		pData				  := _S_RAM_Hptr^.DataStart+RelAdress;
				   		pData_Kopie			  := _S_RAM_Hptr_Kopie^.DataStart+(pRamAdr_Kopie^.udType and 16#00FF_ffff);
				   		_memcpy(pData, pData_Kopie, udLength);   // Speicher aus dem Inhalt aus der Kopie vorladen
		           		MakeRamExChk(pRAMadr);	

				 	// -- Kein speicher verfügbar !!
				 	ELSE  

						pRAMadr := NIL;

				 	END_IF;
				ELSE
	           		MakeRamExChk(pRAMadr);	
				END_IF;
			ELSE
				// RAM Zelle
				pRamAdr^.udData := pRamAdr_Kopie^.udData;
				pRamAdr^.udChk := pRamAdr^.udData + CRC32;
				pRamAdr^.udType := 16#C0000000;    // ganz am ende machen wir uns gültig !
			END_IF;
		END_IF;
	ELSE
		nbrOfCells_foundInSram += 1;
	END_IF;

END_FUNCTION

FUNCTION GLOBAL Find_sRam
VAR_INPUT
	CRC32		: UDINT;
END_VAR
VAR_OUTPUT
	pRamAdr			: ^XramCell;
END_VAR

  pRamAdr := Find_sRamEx(CRC32, FALSE);

END_FUNCTION



////////////////////////////////////////////////////////////////////
//
//  Die IDEE:
//
//  LDRMEM => STRUCT
//
//  +----+-------------------------------+------------------------
//  |INFO| Allocation Table:             |
//  |MAT |   einzele Bits geben benutze  |   DATEN in 16-Byte Blöcken
//  |    |   Blöcke an                   |
//  +----+-------------------------------+------------------------
//
//  Allocation Table:
//  - jedes Bit kennzeichnet einen Block in den Daten
//  - ein 1er im Bit sagt aus, daß der entsprechende Block in den Daten frei ist
//  - das MSB zeigt auf den Block mit der höheren Adresse
//
//
////////////////////////////////////////////////////////////////////


FUNCTION SRAM_GetMemoryEx
VAR_INPUT
   length0  : UDINT;
   pGotMemory : ^BOOL;  // Da beim Returnwert 0 nicht unterschieden werden kann, ob 
                        // der Offset 0 gemeint ist oder Out-Of-Memory, wird mit diesem 
                        // Flag angezeigt, ob Speicher allokiert werden konnte.
END_VAR
VAR_OUTPUT
   ret0 : ^USINT;
END_VAR
VAR
   allocblocknum : UDINT;
   allocblocknumorg : UDINT;
   matsize  : UDINT;
   actblockcount : UDINT;
   actblocknum : UDINT;
   fbptr:^usint;
   startptr:^usint;
   fb, xfb : usint;
   bitcounter:usint;
   start:usint;
   startbit:usint;
   bytecounter:udint;
END_VAR
   
  pGotMemory^ := FALSE;
  ret0 := NIL; 	//#FA16122003 
  if length0 = 0 then 
    ret0 := NIL; 
    return; 
  end_if;

  allocblocknum := ((length0-1) / pLdrmem^.blocksize)+1; // Anzahl der erforderlichen Blöcke
  allocblocknumorg := allocblocknum;
  matsize := pLdrmem^.matsize; // Größe der MAT in Bytes
  actblockcount := 0;
  fbptr := #pLdrmem^.mat[0];
  fb := fbptr^;
  start := 1; // 1=suche ersten freien Block, 0=freier wurde Block gefunden
  bytecounter := 0;
  
  // Schleife, in der die MAT nach einem zusammengehörenden Bereich mit freien 
  // Blöcken (=gesetzten Bits) durchsucht wird.
  while matsize do
    fb := fbptr^;
    if fb then
      // das aktuelle Byte in der MAT ist <>0, d.h. manche Blöcke sind frei
      if start = 0 then
        bytecounter += 1;
      end_if;
      
      // alle Bits des Bytes fb durchgehen
      bitcounter := 0;
      while bitcounter < 8 do
        xfb := fb and 1;
        fb  := fb shr 1;
        if xfb then
          // wir sind innerhalb des Bytes auf einen freien Block gestoßen
          if start then
            // es handelt sich um den ersten freien Block
            start := 0;
            startptr := fbptr;
            startbit := bitcounter;
            allocblocknum := allocblocknumorg;
            actblocknum := actblockcount;
            bytecounter := 0;
          end_if;
          allocblocknum -= 1;
          if allocblocknum = 0 then
            // es wurde ein zusammenhängender Bereich freier Blöcke gefunden
            ret0 := NIL + ((actblocknum * 8) + startbit) * pLdrmem^.blocksize;
            pGotMemory^ := TRUE;

            // es müssen jetzt noch die Bits in der MAT auf den Wert 'belegt'(=0) geändert werden
            if bytecounter = 0 then   // startbit and endbit same byte
              // auf fb wird jetzt eine Maske geschrieben, mit der dann die Bits auf 0 ausmaskiert werden können
              if startbit = bitcounter then   // only a block of 16 byte
                fb := not (1 shl bitcounter);
              else
                bitcounter := 7 - bitcounter;
                fb := ((16#FF shr startbit) and 16#FF) shl startbit; // Bits rechts von startbit werden auf 0 gesetzt
                fb := ((fb shl bitcounter) and 16#FF) shr bitcounter; // Bits links von bitcounter werden auf 0 gesetzt
                fb := not fb; // Wert invertieren, damit er als UND-Maske verwendet werden kann
              end_if;
              startptr^ := startptr^ and fb; // Bits mit der Maske auf 0 ausmaskieren
            else // 1 byte between start and end
              fb := ((16#FF shr startbit) and 16#FF) shl startbit; // Bits rechts von startbit werden auf 0 gesetzt
              fb := not fb; // Wert invertieren, damit er als UND-Maske verwendet werden kann
              startptr^ := startptr^ and fb; // Bits mit der Maske auf 0 ausmaskieren
              startptr += 1;
              if bytecounter then
                bytecounter -= 1;
                // die Bytes zwischen dem ersten und letzten werden zur Gänze auf 0 gesetzt
                while bytecounter do
                  startptr^ := 0;
                  startptr += 1;
                  bytecounter -= 1; 
                end_while;
              end_if;
              bitcounter := 7 - bitcounter;
              fb := ((16#FF shl bitcounter) and 16#FF) shr bitcounter;// Bits links von bitcounter werden auf 0 gesetzt
              fb := not fb; // Wert invertieren, damit er als UND-Maske verwendet werden kann
              startptr^ := startptr^ and fb; // Bits mit der Maske auf 0 ausmaskieren
            end_if;
            
            // Fertig !
            return;
            
          end_if;
        else
          // wir sind innerhalb des Bytes auf einen belegten Block gestoßen
          start := 1;
        end_if;
        bitcounter += 1;
      end_while;
    else // if fb
      // das aktuelle Byte in der MAT ist 0, d.h. alle Blöcke sind belegt
      start := 1;
    end_if;
    actblockcount += 1;
    fbptr += 1;
    matsize -= 1;
  end_while;
   
END_FUNCTION

FUNCTION GLOBAL SRAM_GetMemory
VAR_INPUT
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : ^USINT;
END_VAR
VAR
	gotMemory : BOOL;
END_VAR

  ret0 := SRAM_GetMemoryEx(length0, #gotMemory);

END_FUNCTION

//---------------------------------------------
// Speicher Belegen
//---------------------------------------------
// es werden immer ganze blöcke vergeben
// -> Raladdr0 = Startadresse im s-Ram
// ->  length0 = Speicherlänge
//     #FA vorsicht die length hat 2 teile byte0/1 ist die allocated length
//                                         byte2/3 ist die used length
//										   also ein mask ist wichtig			     

// Wenn in length0 das oberste Bit gesetzt ist, dann wird vor dem Belegen geprüft, 
// ob der gesamte Bereich frei ist. Wenn er nicht frei ist, dann wird auch nichts 
// belegt.

//<-- 0 wenn ok

// diese Funktion wird nur beim Start verwendet um die bereits belegten RAMZellen zu markieren
// in Version alt 1 Bit sind 16 Byte
// in Version neu 1 Bit sind  4 Byte (der Rest der INFO steht im File beim C-IPC)
// abfrage über version die in den oberen 16 Bit von DataValid steht

FUNCTION SRAM_MarkMemory2
VAR_INPUT
   reladdr0 :^usint;
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR

    IF length0 AND 16#80000000 THEN
        // prüfen, ob der gesamte Bereich belegt ist
        length0 -= 16#80000000;
        ret0 := SRAM_FreeMarkMemory(reladdr0$UDINT, length0, 0, TRUE);
        IF ret0 <> 0 THEN
            RETURN;
        END_IF;
    END_IF;

    ret0 := SRAM_FreeMarkMemory(reladdr0$UDINT, length0, 1(*mark*), FALSE);

END_FUNCTION

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL SRAM_MarkMemory
VAR_INPUT
   reladdr0 :^usint;
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR

  ret0 := SRAM_MarkMemory2(reladdr0, length0);

END_FUNCTION
#else
FUNCTION AWL GLOBAL SRAM_MarkMemory
VAR_INPUT
   reladdr0 :^usint;
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR
    PUSHAD

    push      eax         //Result
    push.d    length0
    push.d    reladdr0

    call      SRAM_MarkMemory2

    add.esp   8
    pop.d     ret0        //Result

	POPAD

END_FUNCTION
#endif

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL SRAM_UnmarkMemory
VAR_INPUT
   reladdr0 :^usint;
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR

  ret0 := SRAM_FreeMarkMemory(reladdr0$UDINT, length0, 0(*0=Free Memory*), 0(*FALSE:ändern*));

END_FUNCTION
#else
FUNCTION AWL GLOBAL SRAM_UnmarkMemory
VAR_INPUT
   reladdr0 :^usint;
   length0  : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR
    PUSHAD

    //#SR 16.12.2005
    push      eax         //Result
    push.d    0           //check
    push.d    0           //for unmark memory
    push.d    length0
    push.d    reladdr0

    call      SRAM_FreeMarkMemory

    add.esp   16
    pop.d     ret0        //Result

	POPAD

END_FUNCTION
#endif

//-------------------------------------------
//--- S-Ram Speicherverwaltung Initialisieren
//-------------------------------------------

// Hat a.m geprüft 

FUNCTION GLOBAL SRAM_Startup
VAR_INPUT
   length0  : UDINT;
   clear0   : UDINT;
END_VAR
VAR_OUTPUT
   ret0 : DINT;
END_VAR
VAR
   matsize  : UDINT;
   memlength  : UDINT;
END_VAR

   ret0 := -1; // desciptior fehlerhaft, nichts gibts für die ramexerei

   // Größe der MAT ermitteln
   if !!isRamfileDatUsed | ((_S_RAM_Hptr^.DataValid SHR 16) < 2)  then
    matsize   := length0 / (CST_SRAM_BLOCKSIZE * 8); // anzahl der BitMarker
    memlength := matsize * (CST_SRAM_BLOCKSIZE * 8);
   else
    matsize   := 4 * length0 / (CST_SRAM_BLOCKSIZE * 8); // anzahl der BitMarker
    memlength := matsize * (CST_SRAM_BLOCKSIZE * 8) / 4;
   end_if; 

   IF pLdrmem = NIL THEN
    // MAT neu allokieren
	// pr:
	//	In der Funktion SRAM_ClearMAT wird um ein DWORD mehr als matsize initialisiert, 
	//  daher müssen 4 Bytes zusätlich allokiert werden. Den Grund für die zusätzlichen 
	//	4 Bytes kenne ich nicht.
   	pLdrmem := _alloc_resize_LDR(MAT_INFO_TYPE.mat + matsize + 4)$^MAT_INFO_TYPE;
   ELSIF pLdrmem^.matsize <> matsize THEN
    // Größe der MAT ändern
   	pLdrmem := _realloc_LDR(pLdrmem, MAT_INFO_TYPE.mat + matsize + 4)$^MAT_INFO_TYPE;
   END_IF;
	
   IF pLdrmem = NIL THEN
    ret0 := -1;
    RETURN;
   END_IF;

   pLdrmem^.memlength := memlength;
   pLdrmem^.matsize   := matsize;
   if !!isRamfileDatUsed | ((_S_RAM_Hptr^.DataValid SHR 16) < 2)  then
	pLdrmem^.blocksize := CST_SRAM_BLOCKSIZE;
   else
	pLdrmem^.blocksize := CST_SRAM_BLOCKSIZE_NEW;
   end_if;

   if clear0 then 
    SRAM_ClearMAT(); 
   end_if;

   ret0 := 0; // alles ist gut, daher probieren mal eine kleine ramexerei
END_FUNCTION

//---------------------------------------------
// -- ALLE Blockmarker mit 1 auf "leer" setzen
//---------------------------------------------
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL SRAM_ClearMAT

  _memset(#pLdrmem^.mat[0], 16#ff, pLdrmem^.matsize);

END_FUNCTION
#else
FUNCTION AWL GLOBAL SRAM_ClearMAT

		l.ecx		pLdrmem
   		l.ecx		(ecx + MAT_INFO_TYPE.matsize)
		shr.ecx		2				// /4
		jecxz		SRAM_ClearMATRet

		// pr: wozu das ???
		// ** Fehler IPC 300 **     // es wird mehr eigetragen wie vorhandener speicher
		inc			ecx				// wegen IPC 300 sonst werden die Daten nicht gespeichert				

		l.edi		pLdrmem
		add.edi		MAT_INFO_TYPE.mat
		l.eax		16#ffffffff
		rep
		stosd
SRAM_ClearMATRet
END_FUNCTION
#endif

//-----------------------------------
// Liefert Die Belegte Speichergrösse (bezogen auf speicherstart)
//-----------------------------------
FUNCTION GLOBAL SRAM_GetLastAddr
VAR_OUTPUT
   ret0 : UDINT;
END_VAR
VAR
   Lng: UDINT;
   size:udint;
   Muster : BSINT;
   fbptr : ^bsint;
END_VAR
   fbptr := #pLdrmem^.mat[pLdrmem^.matsize-1];  // in letzet eintrag im array

   Lng    := pLdrmem^.memlength;				// beginn bei voller belegung 
   size   := pLdrmem^.matsize;
   		
   while size do
      if fbptr^ <> 16#FF then     			// erster freier platz wenn nicht auf FF
		 Muster:=fbptr^ $bsint;
		 While Muster do
           Lng-=pLdrmem^.blocksize;
		   muster := muster SHL 1;
		 end_while;
		 Goto  LastAdrEnde;
		
      end_if;
      fbptr -= 1;
	  size -= 1;
	  Lng-= pLdrmem^.blocksize*8;
   end_while;

LastAdrEnde:
   ret0 := Lng;
END_FUNCTION

(*
    Markiert im Sram in der MAT einen Bereich als frei oder belegt oder 
    prüft ob der Bereich vollständig frei oder vollständig belegt ist.
*)
FUNCTION SRAM_FreeMarkMemory
VAR_INPUT
    reladdr0 : UDINT;
    length0  : UDINT;
    mode0    : UDINT;        //0=Free Memory, 1=Mark Memory
    check0   : BOOL;         //FALSE:ändern, TRUE:nur prüfen
END_VAR
VAR_OUTPUT
    ret0 : DINT;
END_VAR
VAR
    startBit : UDINT;
    endBit : UDINT;
    curBit : UDINT;
    ofs : UDINT;
    pMat : ^USINT;
    matVal : USINT;
END_VAR

    ret0 := 0;

    IF ((reladdr0 + length0) > pLdrmem^.memlength) | (length0 = 0) THEN
        // Der angegebene Bereich liegt ausserhalb der Grenzen oder die Länge ist 0 -> Fehler zurückgeben
        ret0 := -1;
        RETURN;
    END_IF;
    
    startBit := reladdr0 / pLdrmem^.blocksize;
    endBit := (reladdr0 + length0 - 1) / pLdrmem^.blocksize;
    IF mode0 = 0 THEN
        matVal := 0xFF;
    ELSE
        matVal := 0x00;
    END_IF;
    
    curBit := startBit;
    pMat := #pLdrmem^.mat[curBit / 8];
    WHILE curBit <= endBit DO 
        ofs := curBit MOD 8;
        IF (ofs = 0) & ((endBit - curBit) >= 8) THEN
            // Wir befinden uns auf einer Byte-Grenze daher können alle 8 Bits 
            // des MAT-Bytes auf einmal behandelt werden.
            IF check0 THEN
                // nur prüfen
                IF pMat^ <> matVal THEN
                    ret0 := -1;
                    RETURN;
                END_IF;
            ELSE
                // ändern
                pMat^ := matVal;
            END_IF;
            
            curBit += 8;
            pMat += 1;
        ELSE
            // Wir befinden uns mitten im Byte. Die Bits müssen einzeln behandelt werden.
            IF check0 THEN
                // nur prüfen
                IF mode0 = 0 THEN
                    IF (pMat^ AND (1 SHL ofs)$USINT) = 0 THEN
                        ret0 := -1;
                        RETURN;
                    END_IF;
                ELSE
                    IF (pMat^ AND (1 SHL ofs)$USINT) <> 0 THEN
                        ret0 := -1;
                        RETURN;
                    END_IF;
                END_IF;
            ELSE
                // ändern
                IF mode0 = 0 THEN
                    pMat^ := pMat^ OR (1 SHL ofs)$USINT;
                ELSE
                    pMat^ := pMat^ AND NOT (1 SHL ofs)$USINT;
                END_IF;
            END_IF;
            
            curBit += 1;
            IF (curBit MOD 8) = 0 THEN
                pMat += 1;
            END_IF;
        END_IF;
    END_WHILE;

END_FUNCTION

#ifdef LDR_RETSVR
FUNCTION GLOBAL GetNbrNotFound
VAR_OUTPUT
  ret0 : udint;
END_VAR

  ret0 := nbrOfCells_notFound;
  
END_FUNCTION

// ein neues File muss erstellt werden
FUNCTION NewRetainSvrFile
	VAR_INPUT
		usProbe 	: usint;
	END_VAR
  VAR_OUTPUT
    ok : dint;
  END_VAR
  var
    pRead : ^MRAM_DESCR;
    pData : ^MRAM_DESCR;
  end_var

  // Schreib-/Lesebuffer
  pData := _alloc_resize_LDR( sizeof( MRAM_DESCR ) * 2 )$^MRAM_DESCR;  
  pRead := pData + sizeof( MRAM_DESCR );
  
  if( pData <> NIL )then
  
    // einen gültigen Header erzeugen
    _memset( pData, 16#00, sizeof( MRAM_DESCR ) * 2 );
    pData^.version     := 0;
    pData^.DataStart   := 20;
    pData^.DataLength  := -1$udint; //wir sehen mal keine Längenbegrenzung vor
    pData^.UsedData    := 20;
    pData^.DataValid   := 1;
    
    if( FileWrite( hLDR_remSvr, pData, sizeof( MRAM_DESCR ) ) = sizeof( MRAM_DESCR ) )then
    
      // schauen, ob das Filesystem auch beschreibbar ist
      if( OS_FILE_SEEK( hLDR_remSvr, 0, FILE_BEGIN ) = 0 )then
        if( OS_FILE_READ( hLDR_remSvr, pRead, sizeof( MRAM_DESCR ) ) = sizeof( MRAM_DESCR ) )then
          if( _memcmp( pData, pRead, sizeof( MRAM_DESCR ) ) = 0 )then
      
            ok := 1;
            
            //nur Probe, dann wieder schließen
            if( usProbe )then
              OS_FILE_CLOSE( hLDR_remSvr );
              OS_FILE_DELETE( LDR_RETENTIVE_SVRFILENAME );
              hLDR_remSvr := 0;
            else
            // sonst den Zeiger wieder auf den Fileanfang
              OS_FILE_SEEK( hLDR_remSvr, 0, FILE_BEGIN );
            end_if;
          end_if;
        end_if;
      end_if;
    end_if;
    _free( pData );
  end_if;

END_FUNCTION

// Initialisiert das File zum Speichern der remanenten Server
// > usProbe: 1..nur schauen, obs prinzipiell geht
//            0..das File auch gleich öffnen (offen lassen)
FUNCTION GLOBAL InitRetainSvrFile
	VAR_INPUT
		usProbe 	: usint;
	END_VAR
	VAR_OUTPUT
		ok 	: DINT;
	END_VAR

  if( hLDR_remSvr <> 0 )then
    ok := 1;
    return;
  end_if;
  
  ok := 0;

  //existiert das File
  hLDR_remSvr := OS_FILE_OPEN( LDR_RETENTIVE_SVRFILENAME, 2, ATT_OPEN_SHARED or ATT_COMMITTED); 
  
  if( hLDR_remSvr >= 0 )then
  
    //Fileinhalt checken
    if( LoadSramKopie( NIL, hLDR_remSvr, usProbe ) = 0 )then
      ok := 1;
    else
      DEBUGLOG0("%010d-InitRetainSvrFile: file corrupted. Stored in retsvr.err and generated a new one!");
      TRACE_ERR("Error in RetSvrFile, see c:\sysmsg\event19.log");
      LSLCLI_LoaderTracePrint("Error in RetSvrFile, see c:\sysmsg\event19.log", 12);
      OS_FILE_CLOSE( hLDR_remSvr );
      OS_FILE_RENAME( LDR_RETENTIVE_SVRFILENAME, LDR_RETENTIVE_SVRFILENAME_ERR );
      hLDR_remSvr := OS_FILE_OPEN( LDR_RETENTIVE_SVRFILENAME, 2, ATT_OPEN_SHARED or ATT_CREATE_ALWAYS or ATT_COMMITTED); 
      if( hLDR_remSvr >= 0 )then
        //Schreib/Lesetest
        ok := NewRetainSvrFile( usProbe );
      end_if;
    end_if;
    
    //nur Probe, dann wieder schließen
    if( ( usProbe <> 0 ) | ( ok = 0 ) )then
      OS_FILE_CLOSE( hLDR_remSvr );
      hLDR_remSvr := 0;
    end_if;
  else
  //nein, dann anlegen und testen
    hLDR_remSvr := OS_FILE_OPEN( LDR_RETENTIVE_SVRFILENAME, 2, ATT_OPEN_SHARED or ATT_CREATE_ALWAYS or ATT_COMMITTED); 
    if( hLDR_remSvr >= 0 )then
      DEBUGLOG0("%010d-InitRetainSvrFile: created new retsvr.dat");
    
      //Schreib/Lesetest
      ok := NewRetainSvrFile( usProbe );
      if( ( usProbe <> 0 ) | ( ok = 0 ) )then
        OS_FILE_CLOSE( hLDR_remSvr );
        hLDR_remSvr := 0;
      end_if;
    else
      hLDR_remSvr := 0;
    end_if;
  end_if;
  
END_FUNCTION


// Gibt den temporären Speicher frei
FUNCTION GLOBAL FreeMemoryRetainSvrFile
var
  a : ^MRAM_DESCR;
end_var
  // den Buffer vor dem reorg brauchen wir gar nicht mehr
  if( _S_RAM_Hptr_File <> NIL )then
    _free( _S_RAM_Hptr_File );
    _S_RAM_Hptr_File := NIL;
    
    // der reorganisiserte Bufer landet im File
    a := _S_RAM_Hptr_File_New;
    if( _S_RAM_Hptr_File_New <> NIL )then
      if( OS_FILE_SEEK( hLDR_remSvr, 0, FILE_BEGIN ) = 0 )then
        if( FileWrite( hLDR_remSvr, _S_RAM_Hptr_File_New, _S_RAM_Hptr_File_New^.UsedData )$UDINT = _S_RAM_Hptr_File_New^.UsedData )then
          
          // und das File schneiden wir auf die benötigte Länge zu
          OS_FILE_TRUNCATE( hLDR_remSvr );
          OS_FILE_SEEK( hLDR_remSvr, 0, FILE_BEGIN );
        else
          DEBUGLOG0("%010d-FreeMemoryRetainSvrFile: Cannot reorg.!");
        end_if;
      else
        DEBUGLOG0("%010d-FreeMemoryRetainSvrFile: Cannot reorg.!");
      end_if;
      _free( _S_RAM_Hptr_File_New );
      _S_RAM_Hptr_File_New := NIL;
    end_if;    
  end_if;

END_FUNCTION

(*
  Hängt eine ASYNC_FILE_STATE Datenstruktur ans Ende der verketteten Liste an.
*)
FUNCTION AppendAsyncState
VAR_INPUT
  pAsyncState : ^RetSvrFile::ASYNC_FILE_STATE;
END_VAR

  IF pAsyncState^.pNext <> NIL THEN
    // Dieser Zustand darf nie eintreten. Wenn doch, dann Excepion auslösen
    (0$^UDINT)^ := 0;
  END_IF;

  IF pLastAsyncState = NIL THEN
    IF pFirstAsyncState <> NIL THEN
      // Dieser Zustand darf nie eintreten. Wenn doch, dann Excepion auslösen
      (0$^UDINT)^ := 0;
    END_IF;
    pFirstAsyncState := pAsyncState;
    pLastAsyncState := pAsyncState;
  ELSE
    pLastAsyncState^.pNext := pAsyncState;
    pLastAsyncState := pAsyncState;
  END_IF;
  
  nAsyncStateObjs += 1;

END_FUNCTION

(*
  Entfernt eine ASYNC_FILE_STATE Datenstruktur aus der verketteten Liste.
*)
FUNCTION RemoveAsyncState
VAR_INPUT
  pPrev : ^RetSvrFile::ASYNC_FILE_STATE;
  pAsyncState : ^RetSvrFile::ASYNC_FILE_STATE;
END_VAR

  IF pPrev = NIL THEN
    // es wird das Element am Anfang entfernt
    pFirstAsyncState := pAsyncState^.pNext;
  ELSE
    // es wird ein Element nach dem Anfang entfernt
    pPrev^.pNext := pAsyncState^.pNext;
  END_IF;
  IF pAsyncState = pLastAsyncState THEN
    // es wird das Element am Ende entfernt
    pLastAsyncState := pPrev;
  END_IF;
  
  pAsyncState^.pNext := NIL;

  nAsyncStateObjs -= 1;

END_FUNCTION

FUNCTION Get_ITASK_Version
VAR_OUTPUT
	version : UDINT;
END_VAR
  
  // Anm.: Beim Salamander darf das udVersion Element aus der LSL_ITASK Struktur 
  // (Makro OS_ITASK_VERSION) erst ab 09.03.080 interpretiert werden, weil sonst 
  // keine Versionsnummer dahintersteckt, sondern ein Funktionszeiger auf eine Dummy-Funktion.
  IF _RtOSVersion < 16#9000 | _RtOSVersion >= 16#9350 THEN
    version := OS_ITASK_VERSION;
  ELSE
    version := 0;
  END_IF;
  
END_FUNCTION

(*
  Liefert die Info, ob das OS versucht, die Applikation zu stoppen
*)
FUNCTION ITASK_IS_STOPPING
VAR_OUTPUT
  is_stopping : UDINT;
END_VAR

  IF Get_ITASK_Version() >= 0x11 THEN
    is_stopping := OS_ITASK_IS_STOPPING();
  ELSE
    is_stopping := 0;
  END_IF;

END_FUNCTION

(*
  Inkrementiert oder dekrementiert den StopWait Zähler. 
  Wenn dieser >0 ist dann wartet das OS beim Stoppen der Applikation, bis dieser 
  Zähler auf 0 geht. Die Applikation kann damit das Stoppen der Applikation 
  verzögern bis z,B. alle Retentive File Server geschrieben sind.
*)
FUNCTION ITASK_SET_STOPWAIT_CNTR
VAR_INPUT
  inc_dec : DINT;
END_VAR

  IF Get_ITASK_Version() >= 0x11 THEN
    OS_ITASK_SET_STOPWAIT_CNTR(inc_dec);
  END_IF;

END_FUNCTION

(*
  Asynchrone Version von RetSvrFileWrite.
  
  Diese Funktion wird von einem RetSvrFile Objekt aufgerufen, wenn die XRamCell Struktur des 
  Objekts ins remanente File geschrieben werden soll. Das Schreiben erfolgt asynchron. 
  Der Request wird in die Liste pFirstAsyncState eingetragen. Diese Liste wird in der 
  zyklischen Loaderfunktion ProcessAsyncFileObjects abgearbeitet.
*)
FUNCTION GLOBAL RetSvrFileWrite_A
VAR_INPUT
  pAsyncState : ^RetSvrFile::ASYNC_FILE_STATE;  // Zeiger auf eine bereits initialisierte ASYNC_FILE_STATE Struktur
END_VAR
VAR_OUTPUT
  ret0 : dint;
END_VAR
VAR
	pCell : ^XRamCell;
END_VAR

  ret0 := -1;
  
  IF processAsyncFileError < 0 THEN
    return;
  END_IF;
  
  IF ITASK_IS_STOPPING() THEN
    // Wenn das Betriebsystem gerade versucht, die Applikation zu stoppen, 
    // wird kein neuer Schreibrequest angenommen.
    RETURN;
  END_IF;
  
  pCell := #pAsyncState^.XRamAddr;

  // wenn wir das Projekt grad hochfahren, dann darf nichts in das File geschrieben werden!
  // das landet (vorerst) alles im Buffer und wird beim Reorg geschrieben
  if( _S_RAM_Hptr_File_New = NIL )then
    
    // existiert das Handle?
    if( hLDR_remSvr <> NIL )then
      OS_MT_Wait(hMemMutex);
      IF pAsyncState^.state = 0 THEN
        // pAsyncState an die Liste der AsyncState Objekte hinten anhängen
        AppendAsyncState(pAsyncState);
      END_IF;
      pAsyncState^.state := pAsyncState^.state OR 1; // State 1 bedeutet, dass pDataObj^ geschrieben werden muss
      IF !!AsnycFileOperationInProgress THEN
        // Wird haben in einen Zustand gewechselt, in dem Daten gespeichert sind, 
        // die erst geschrieben werden müssen. Dem Betriebssystem wird das mit dem 
        // Aufruf der ITASK_SET_STOPWAIT_CNTR(1) mitgeteilt.
        ITASK_SET_STOPWAIT_CNTR(1);
      END_IF;
      AsnycFileOperationInProgress := TRUE;
      OS_MT_Signal(hMemMutex);
    end_if;
  
  else
    pCell^.udData := pAsyncState^.pDataObj^;
    _memcpy( _S_RAM_Hptr_File_New + pCell^.udType, pCell, sizeof( XRamCell ) );
  end_if;

  ret0 := 0;

END_FUNCTION

(*
  Bearbeitet eine asynchronene File Datenstruktur eines RetSvrFile Objekts.
  
  Hier wird das asynchrone Fileschreiben angestossen und überwacht. Sobale eine asynchrone Fileoperation 
  fertig ist, wird das Element state der Struktur auf 0 gesetzt.
  
  Return:
    1 wenn kein schwerwiegender Fehler aufgetreten ist und die aüssere Schleife abgebrochen werden 
      soll
    0 wenn kein schwerwiegender Fehler aufgetreten ist
   <0 wenn ein schwerwiegender Fehler aufgetreten ist, bei dem man annehmen muss, dass das File korrupt ist 
        und deswegen auf ungültig gesetzt werden muss.
*)
FUNCTION ProcessOneAsyncFileObject
VAR_INPUT
	pAsyncState : ^RetSvrFile::ASYNC_FILE_STATE;
END_VAR
VAR_OUTPUT
  result : DINT;
END_VAR
VAR
	pCell : ^XRamCell;
  rc : DINT;
  requestResult : DINT;
  itoa_buf : ARRAY[0..20] OF CHAR;
END_VAR

  result := 0;

  IF pAsyncState^.state = 1 THEN
    IF (_rtosversion < (16#1200 + 93)) & (nAsyncStateReqs > 0) THEN
      // Bei einer OS Version < 1.2.93 kommte es zu einem Fehler, wenn die Async-Mailbox voll wird, 
      // daher wird in so einem Fall nur dann etwas neues in die Mailbox gestellt, wenn gerade 
      // kein Request unterwegs ist (Anzahl der anstehenden Requests steht in nAsyncStateReqs)
      result := 1;
      RETURN;
    END_IF;
    pCell := #pAsyncState^.XRamAddr;
    rc := OS_FILE_SEEK_AV1(Async0:=1, handle0:=hLDR_remSvr, offset0:=pCell^.udType$DINT, fromwhere0:=FILE_BEGIN);
#ifdef TEST_RETSVRFILE  // Code kann für Testzwecke aktiviert werden
    IF simuFSeekA_error <> 0 THEN
      rc := simuFSeekA_error;
    END_IF;
#endif
    IF rc >= 0 THEN
      // Request fürs Positionieren wurde angenommen
      
      // Daten in die XRamCell übernehmen
      pCell^.udData := pAsyncState^.pDataObj^;
      // asynchrones FileWrite anstossen
      rc := OS_FILE_WRITE_AV1(Async0:=1, fhandle0:=hLDR_remSvr, buffer0:=pCell, length0:=sizeof(XRamCell));
#ifdef TEST_RETSVRFILE  // Code kann für Testzwecke aktiviert werden
      IF simuFWriteA_error <> 0 THEN
        rc := simuFWriteA_error;
      END_IF;
#endif      
      IF rc >= 0 THEN
        // Request konnte abgesetzt werden -> Request-Handle merken und Status aktualisieren
        pAsyncState^.hRequest := rc;
        pAsyncState^.state := 2;
        nAsyncStateReqs += 1;
      ELSIF rc = -200 THEN // -200=Mailbox voll
        result := 1; // Bearbeitung der restlichen Listeneinträge kann entfallen
        RETURN;
      ELSE
      	_itoa(rc$UDINT, #itoa_buf[0]);
        TRACE0_ERR("ERROR in ProcessOneAsyncFileObject: OS_FILE_WRITE_AV1 failed, {0}", #itoa_buf[0]);
      END_IF;
    ELSIF rc = -200 THEN // -200=Mailbox voll
      result := 1; // Bearbeitung der restlichen Listeneinträge kann entfallen
      RETURN;
    ELSE
      // SA33172,DS2528:
      // In diesem Fall (OS_FILE_SEEK_AV1 hat einen Fehler <> -200 geliefert), wird die 
      // Bearbeitung der restlichen Listeneinträge abgebrochen, weil sonst bei einem 
      // permanenten Fehler auf der SD-Karte die CPU-Belastung sehr hoch werden würde.
      result := 1;
    	_itoa(rc$UDINT, #itoa_buf[0]);
      TRACE0_ERR("ERROR in ProcessOneAsyncFileObject: OS_FILE_SEEK_AV1 failed, {0}", #itoa_buf[0]);
    END_IF;
  END_IF;

  IF pAsyncState^.state > 1 THEN
    rc := OS_FILE_GETASYNCSTATE(ID:=pAsyncState^.hRequest$UDINT, Erg:=#requestResult);
    IF rc >= 0 THEN
      // Request ist abgeschlossen -> Ergebnis der Fileoperation prüfen und Status aktualisieren
#ifdef TEST_RETSVRFILE  // Code kann für Testzwecke aktiviert werden
      IF simuFReq_error <> 0 THEN
        requestResult := simuFReq_error;
      END_IF;
#endif      
      IF requestResult < 0 THEN
        result := requestResult;
      END_IF;
      pAsyncState^.state := pAsyncState^.state AND (NOT 2);
      pAsyncState^.wrCnt += 1;
      nAsyncStateReqs -= 1;
      IF pAsyncState^.state = 1 THEN
        (* Wir sind jetzt in der Situation dass während der asynchronen Fileoperation wieder das 
           dData des RetSvrFile Objekts beschrieben worden ist. Wenn man jetzt nichts unternimmt, 
           dann könnte für dieses Objekt beim nächsten Durchlauf wieder ein Request abgesetzt werden. 
           Wenn sich dieses Spielchen ständig wiederholt, dann könnten andere Objekte mit Status 1, 
           die sich in der Liste weiter hinten befinden, möglicherweise nie gestartet werden.
           Um das zu verhindern, sollte diese Objekt aus der Liste entfernt und hinten wieder 
           angehängt werden. Das muss vom Aufrufer erledigt werden und damit er weiss dass er das 
           machen soll, wird der state auf den temporären Wert 4 gesetzt.
         *)
         pAsyncState^.state := 4;
      END_IF;
    ELSIF rc = RT_NOT_STARTED | rc = RT_IN_PROGRESS THEN
      result := 1; // Bearbeitung der restlichen Listeneinträge kann entfallen
      RETURN;
    ELSE
    	_itoa(rc$UDINT, #itoa_buf[0]);
      TRACE0_ERR("ERROR in ProcessOneAsyncFileObject: OS_FILE_GETASYNCSTATE failed, {0}", #itoa_buf[0]);
    END_IF;
  END_IF;

END_FUNCTION

(*
  Bearbeitet alle Elemente in der Liste der asynchronen File Datenstrukturen von RetSvrFile Objekten. 
  
  Für jedes Element wird ProcessOneAsyncFileObject aufgerufen und wenn dann angezeigt wird, dass die 
  asynchrone Fileoperation fertig ist, wird das aktuelle Element aus der Liste entfernt.
*)
FUNCTION GLOBAL ProcessAsyncFileObjects
VAR
	pAsyncState : ^RetSvrFile::ASYNC_FILE_STATE;
	pPrev : ^RetSvrFile::ASYNC_FILE_STATE;
	pNext : ^RetSvrFile::ASYNC_FILE_STATE;
  rc : DINT;
  validFlag : UDINT;
END_VAR

  IF processAsyncFileError < 0 THEN
    return;
  END_IF;

  rc := 16#12345678;
  
  // Mit dieser Abfrage wird der OVerhead von OS_MT_Wait/OS_MT_Signal vermieden, 
  // wenn nichts abzuarbeiten ist.
  IF pFirstAsyncState <> NIL THEN
    OS_MT_Wait(hMemMutex);
    pPrev := NIL;
    pAsyncState := pFirstAsyncState;
    WHILE pAsyncState <> NIL DO 
      IF pAsyncState^.state = 0 THEN
        // Dieser Zustand darf nie eintreten. Wenn doch, dann Excepion auslösen
        (0$^UDINT)^ := 0;
      END_IF;
      
      rc := ProcessOneAsyncFileObject(pAsyncState);
      IF rc < 0 THEN
        // schwerweigender Fehler, File ist möglicherweise korrupt
        // -> File als ungültig markieren, Fehlerflag setzen
        validFlag := 0;
        OS_FILE_SEEK( hLDR_remSvr, MRAM_DESCR.DataValid, FILE_BEGIN );
        FileWrite( hLDR_remSvr, #validFlag$^void, sizeof( MRAM_DESCR.DataValid ) );
        processAsyncFileError := rc;
      END_IF;
      IF rc = 1 THEN
        EXIT;
      END_IF;
      
      pNext := pAsyncState^.pNext;
      IF pAsyncState^.state = 0 THEN
        // state 0: Request wurde beendet -> Objekt entfernen
        RemoveAsyncState(pPrev, pAsyncState);
      ELSIF pAsyncState^.state = 4 THEN
        // state 4: Request wurde beendet, dData wurde aber zwischenzeitlich wieder geändert
        //  -> Objekt entfernen und hinten wieder anhängen
        RemoveAsyncState(pPrev, pAsyncState);
        pAsyncState^.state := 1;
        AppendAsyncState(pAsyncState);
      ELSE
        pPrev := pAsyncState;
      END_IF;
      pAsyncState := pNext;
    END_WHILE;
    OS_MT_Signal(hMemMutex);
  END_IF;

  if((rc = 0) | (rc = 16#12345678)) then
    OS_MT_Wait(hMemMutex);
    IF AsnycFileOperationInProgress THEN
      // Wird haben den Zustand verlassen, in dem Daten gespeichert sind, 
      // die erst geschrieben werden müssen. Dem Betriebssystem wird das mit dem 
      // Aufruf der ITASK_SET_STOPWAIT_CNTR(-1) mitgeteilt.
      ITASK_SET_STOPWAIT_CNTR(-1);
    END_IF;
    AsnycFileOperationInProgress := FALSE;
    OS_MT_Signal(hMemMutex);
  end_if;
  
END_FUNCTION

FUNCTION GLOBAL LDR_IsAsnycFileOperationInProgress
VAR_OUTPUT
  ret   : BOOL;
END_VAR

  ret := AsnycFileOperationInProgress;
END_FUNCTION

FUNCTION GLOBAL LDR_HasSram
VAR_OUTPUT
  ret   : BOOL;
END_VAR

  ret := hasSram;
END_FUNCTION

#if 0
FUNCTION GLOBAL RetSvrFileWrite
VAR_INPUT
  pCell : ^XRamCell;
END_VAR
VAR_OUTPUT
  ret0 : dint;
END_VAR

  ret0 := -1;

  // wenn wir das Projekt grad hochfahren, dann darf nichts in das File geschrieben werden!
  // das landet (vorerst) alles im Buffer und wird beim Reorg geschrieben
  if( _S_RAM_Hptr_File_New = NIL )then
    
    // existiert das Handle?
    if( hLDR_remSvr <> NIL )then
    
      // auf die gewünschte Zelle positionieren
      if( OS_FILE_SEEK( hLDR_remSvr, pCell^.udType, FILE_BEGIN ) = pCell^.udType )then
        pCell^.udChk := pCell^.udData;
        
        // und reinschreiben
        if( OS_FILE_WRITE( hLDR_remSvr, pCell, sizeof( XRamCell ) ) = sizeof( XRamCell ) )then
          ret0 := 0;
        else
          ret0 := -2;
        end_if;
      else
        ret0 := -2;
      end_if;
    end_if;
  
    // Positionieren oder Schreiben hat nicht hingehaun -> File ist korrupt!
    if( ret0 = -2 )then
      OS_FILE_SEEK( hLDR_remSvr, MRAM_DESCR.DataValid, FILE_BEGIN );
      ret0 := 0;
      OS_FILE_WRITE( hLDR_remSvr, #ret0$^void, sizeof( MRAM_DESCR.DataValid ) );
      ret0 := -2;
    end_if;
  else
    _memcpy( _S_RAM_Hptr_File_New + pCell^.udType, pCell, sizeof( XRamCell ) );
  end_if;

END_FUNCTION
#endif

#endif // LDR_RETSVR
