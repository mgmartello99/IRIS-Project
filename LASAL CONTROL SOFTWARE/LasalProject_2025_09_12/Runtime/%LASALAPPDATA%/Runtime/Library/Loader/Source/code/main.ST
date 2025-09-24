
// **************************************************************************
// 22.12.2003
//
// _itoa Fehler beim wandel von Buchstaben auch String
// ARCTAN ohne CO-Prozessor --> vorzeichen wurde nicht beachtet bei Rückgabe
// ARCTAN mit  CO_Prozessor --> vergessen den Stack des COpro zu loeschen (kann muß aber nicht gehen)
// RAM mit shared command table
// TCP/IP in Standard
// mehrere lokale Verbindungen (LSE+Text)
// mehrere kreuzweise Verbindungen mit CAN
// **************************************************************************


//#include "RTOS_PrivHeader.h"
#include <RTOS_Variables.h>
#include ".\RTOS_PrivHeader.h"
#include ".\Loader\RTOS_MemIntern.h"
#include ".\Loader\RTOS_IprIntern.h"
#include "LSL_STITask.h"
#ifdef DATASERVICE_LASAL
#include "..\Class\DataService\DS_Main.h"
#endif

#ifndef DUMMY_LOADER
#pragma using DebugIp
#pragma using IprMgr
#endif


VAR_GLOBAL
	ControlRegister	AT % m 0E90 : USINT;
  LOADERINFO : USINT;
	plslcli4lasal : ^LSL_CLI4LASAL;
  lslcli4lasal : LSL_CLI4LASAL;
  uiProgsToLoad : array[ 0..1 ] of udint;
END_VAR
VAR_PRIVATE
  executingDebugIp : BOOL;
  opCyclicFlag : BOOL;      
  callProgsequencesInCyclic : UDINT;
  callProgsequencesInCyclic_udPhase : UDINT;
END_VAR



// Globale Funktionen aus anderen Loader - Modulen
FUNCTION GLOBAL SystemInit
VAR_INPUT
	useOsResources	: BOOL;
END_VAR
VAR_OUTPUT
	ret_code 		: CONFSTATES;
END_VAR;
FUNCTION GLOBAL SystemExit;
FUNCTION GLOBAL ApplInit
VAR_OUTPUT
	ret_code 		: CONFSTATES;
END_VAR;
FUNCTION GLOBAL ComlinkCyWork;
FUNCTION GLOBAL ProcessAsyncFileObjects;

// Lokale Funktionen, Vorausdeklarationen
FUNCTION LslCallDestFus;
FUNCTION GLOBAL OpCyclic;
FUNCTION OpCyclicDebugIp;
FUNCTION SysCyclic;

#ifdef _LSL_TARGETARCH_ARM
FUNCTION dummyLoaderTracePrint
VAR_INPUT
  a : ^char;
  b : char;
END_VAR
VAR_OUTPUT
  retc (EAX) : dint;
END_VAR;
#else
FUNCTION dummyLoaderTracePrint
VAR_INPUT
  a : ^char;
  b : char;
END_VAR
VAR_OUTPUT
  EAX : dint;
END_VAR;
#endif

FUNCTION dummyLoaderTraceInit;
//
//	Input:
//		inp_code: Set to the CONFSTATE (C_INIT to init, C_OK if OK to run!)
//	Output:
//		EAX: Set to the CONFSTATE (C_OK if OK to run!)
//

#ifdef FAR_OBJECT 
function global __cdecl InitFarObject;
function global __cdecl ChkFarObject var_output retcode : ^void; end_var;
#endif

function global CriticalLoaderSectionInit;
#ifdef _LSL_TARGETARCH_ARM
FUNCTION main
VAR_INPUT
	inp_code : CONFSTATES;
END_VAR
VAR_OUTPUT
  retc  : udint;
END_VAR
#else
FUNCTION main
VAR_INPUT
	inp_code : CONFSTATES;
END_VAR
#endif
VAR
	ret_code 	: CONFSTATES;
END_VAR

	// Process according to the input code request
	CASE inp_code OF
		C_INIT:	// (0) Initialize the application
      CriticalLoaderSectionInit();
      #ifdef DATASERVICE_LASAL
       LDR_DS_CTor(2402);
      #endif

     #ifdef FAR_OBJECT 
      InitFarObject();  // FAR_OBJECT
     #endif
     callProgsequencesInCyclic := 0;
    
#ifdef LDR_USETRACE
      if( _RtOSversion >= 16#1199 )then
        OS_CILGet( "LSLCLI_4LASAL", #plslcli4lasal$void );
        if( plslcli4lasal <> 0 )then
          if( plslcli4lasal^.LoaderTraceInit <> 0 )then
            LSLCLI_LoaderTraceInit();
          else
            plslcli4lasal := #lslcli4lasal;
            lslcli4lasal.LoaderTracePrint := #dummyLoaderTracePrint();
            lslcli4lasal.LoaderTraceInit := #dummyLoaderTraceInit();        
          end_if;
        else
          plslcli4lasal := #lslcli4lasal;
          lslcli4lasal.LoaderTracePrint := #dummyLoaderTracePrint();
          lslcli4lasal.LoaderTraceInit := #dummyLoaderTraceInit();        
        end_if;
      else
        plslcli4lasal := #lslcli4lasal;
        lslcli4lasal.LoaderTracePrint := #dummyLoaderTracePrint();
        lslcli4lasal.LoaderTraceInit := #dummyLoaderTraceInit();
      end_if;
#else
        plslcli4lasal := #lslcli4lasal;
        lslcli4lasal.LoaderTracePrint := #dummyLoaderTracePrint();
        lslcli4lasal.LoaderTraceInit := #dummyLoaderTraceInit();
#endif
#ifdef DUMMY_LOADER
      TRACE_WARN("Missing Loader: You are using a dummy loader instead of the real loader !!!");
      LSLCLI_LoaderTracePrint("Missing Loader: You are using a dummy loader instead of the real loader !!!", 12);
			ret_code := C_SYNTAX_ERROR;
#else
      ControlRegister	:= 0;		// wichtig für DDM163
      executingDebugIp := FALSE;
			IF _IOSegment = 0 THEN
				_IOSegment := 16#D0000;
			END_IF;
			ret_code := ApplInit();
			OPS.SysState := ret_code;
			IF ret_code <= C_RUNROM THEN// tasks are started when
				ret_code := C_OK;		// ret_code is OK
#ifdef LDR_TRACE_SUCCESS
        LSLCLI_LoaderTracePrint("Loader finished", 10); 
        OS_SSR_DELAY( 801 );
#endif
			else
        LSLCLI_LoaderTracePrint("Loader Syntax Error", 14); 
        OS_SSR_DELAY( 801 );
      END_IF;

#endif      

		C_OK:	    // (5) Run cyclic work
			OpCyclic();
			ret_code := C_OK;
     #ifdef FAR_OBJECT 
      ChkFarObject();
     #endif

		6:	      // (6) Run cyclic work (nur DebugIp), Aufruf aus DebugHandler (Breakpoint)
			OpCyclicDebugIp();
			ret_code := C_OK;

		C_DESTRUCTORS: // (18)
			LslCallDestFus();
			ret_code := C_SYNTAX_ERROR;

		ELSE // Unknown / invalid state
			ret_code := C_SYNTAX_ERROR;

	END_CASE;

	// Return code
 #ifdef _LSL_TARGETARCH_ARM 
  retc := ret_code$UINT; // Return code
 #else 
	EAX := ret_code$UINT;
 #endif

END_FUNCTION

// 
// sysmain
//
//	Input:
//		inp_code: Set to the CONFSTATE (C_SYSINIT to init, C_SYSOK if OK to run, C_SYSEXIT to exit)
//	Output:
//		EAX: Set to the CONFSTATE (C_OK if OK to run!)
//


#ifdef _LSL_TARGETARCH_ARM
FUNCTION sysmain
VAR_INPUT
	inp_code : CONFSTATES;
END_VAR
VAR_OUTPUT
  retc (EAX) : udint;
END_VAR
VAR
	ret_code : ConfStates;
END_VAR
#else
FUNCTION sysmain
VAR_INPUT
	inp_code : CONFSTATES;
END_VAR
VAR
	ret_code 	: CONFSTATES;
END_VAR
#endif

	// Process according to the input code request
	CASE inp_code OF
		C_SYSINIT:			// loader initialization
      executingDebugIp := FALSE;
			ret_code := SystemInit(TRUE);

		C_SYSOK:          	// run system cyclic work (no application available)
      executingDebugIp := FALSE;  // since 2.2.47
			SysCyclic();
			ret_code := C_OK;

		C_SYSEXIT:			// exit the loader
			SystemExit();
			ret_code := C_OK;

		ELSE // Unknown / invalid state
			ret_code := C_SYNTAX_ERROR;

	END_CASE;

	// Return code
 #ifdef _LSL_TARGETARCH_ARM 
	retc := ret_code; // Return code
 #else 
	EAX := ret_code$UINT;
 #endif

END_FUNCTION

TYPE
	DEST_STACK		: STRUCT
		udSize		: UDINT;	// Anzahl der aufzurufenden Funktionen
		bOk			: UDINT;	// ist das Allozieren gut gegangen?
		pData		: ^pVoid;	// Liste mit Funktionspointern
	END_STRUCT;
END_TYPE

VAR_PRIVATE
	pDestructors	: ^DEST_STACK;	// private Liste mit Destruktor-Pointern
END_VAR

//[#DEUTSCH]
// Aufruf der Funktionen, die die Konstruktoren aufrufen, und der 
// Funktionen, die die Adressen der Destruktoren in die statische
// Funktions-Pointer-Liste eintragen.
// Die Liste der Funktionspointer wird vom Linker erstellt. Sie ergibt
// sich aus allen Einträgen vom Typ IMAGE_REL_DIR32_CRT_A.
//[>pTab] Pointer auf Tabelle mit Init-Funktionen
//[#ENGLISH]
// This function calls wrapper functions that either call a constructor themselves,
// or they add a destructor function pointer to a static list.
// The list of wrapper function pointers is created by the linker by checking for
// fixup entries of type IMAGE_REL_DIR32_CRT_A.
//[>pTab] pointer to table with init - functions

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL dummyCall;
FUNCTION GLOBAL LslCallConFus
VAR_INPUT
	pTab		: ^pVoid;	// Pointer auf Tabelle mit Funktionsadressen
END_VAR						// 1. UDINT enthält die Länge der Tabelle
VAR
	anz : udint;
END_VAR

  anz := pTab$^udint^;
  while anz do
    pTab += 4;
    pTab$^pvoid^$dummyCall();
    anz -= 1;
  end_while;

END_FUNCTION


FUNCTION LslCallDestFus
VAR
  anz : udint;	
  pfkt : ^pvoid;
END_VAR

  anz  := pDestructors^.udSize;
  pfkt := pDestructors^.pData^ + anz * 4;
  
  while anz do
    pfkt -= 4;
    pfkt^$dummyCall();
    anz -= 1;
  end_while;

END_FUNCTION
#else
FUNCTION AWL GLOBAL LslCallConFus
VAR_INPUT
	pTab		: ^pVoid;	// Pointer auf Tabelle mit Funktionsadressen
END_VAR						// 1. UDINT enthält die Länge der Tabelle

	PUSH	EDI
	PUSH	ECX

	L.EDI	pTab
	L.ECX	(EDI)		// Anzahl der Funktionspointer

	JECXZ	ENDE		// Schleifenzähler prüfen
	ADD.EDI	sizeof(UDINT)

LOOP
	PUSH	EDI			// Funktionspointer-Pointer und Zähler sichern
	PUSH	ECX

	CALL	(EDI)

	POP		EDI			// Funktionspointer-Pointer und Zähler rücksichern
	POP		ECX

	ADD.EDI	sizeof(pVoid)
	DEC		ECX
	JNZ		LOOP
	
ENDE

	POP		ECX
	POP		EDI
END_FUNCTION
//[#DEUTSCH]
// Ruft aus einer LIFO-Liste die Destruktoren auf. Die Liste wird durch
// Aufrufe von kleinen Funktionen im Rahmen der Ausführung des Init-Codes
// aufgebaut. Für jeden Destruktor wird eine Funktion aufgerufen, die die
// Adresse des Destruktors in die statische Liste einträgt.
// Diese Liste wird vor RESET in umgekehrter Reihenfolge abgearbeitet.
//[#ENGLISH]
// Calls destructors which are given by their function pointers in a static LIFO
// list. The list is created by calls to atexit during the init code execution.
// The list is executed before RESET in reverse order.
FUNCTION AWL LslCallDestFus

	PUSH	EDI
	PUSH	ECX

	L.EDI	pDestructors	// Pointer auf Tabelle mit Funktionsadressen

	L.ECX	(EDI+DEST_STACK.udSize)

	JECXZ	ENDE			// Schleifenzähler prüfen
	L.EDI	(EDI+DEST_STACK.pData)
	SHL.ECX	2				// Länge der Tabelle
	ADD.EDI	ECX
	SHR.ECX	2				// Anzahl der Einträge

LOOP
	PUSH	EDI				// Funktionspointer-Pointer und Zähler sichern
	PUSH	ECX

	CALL	(EDI)

	POP		EDI				// Funktionspointer-Pointer und Zähler rücksichern
	POP		ECX

	SUB.EDI	sizeof(pVoid)
	DEC		ECX
	JNZ		LOOP
	
ENDE

	POP		ECX
	POP		EDI
END_FUNCTION
#endif
//[#DETUSCH]
//Diese Funktion muß vom Init-Code aus aufgerufen werden (zwischen dem Initialisieren
//statischer C-Variablen und den Aufrufen von statischen Konsturktor-Aufruf-Funktionen).
//Sie initialisiert die statische Liste zum speichern der Destruktor-Pointer.
//[<bOk] TRUE Liste konnte alloziert werden, FALSE Speicherfehler
//[#ENGLISH]
//This function must be called by the init code prior to a call to LslCallConFus(). It
//initializes the static list of destructor pointers
//[<bOk] TRUE if the list could be allocated, FALSE if not
FUNCTION GLOBAL __CDECL LslAtexitInit
VAR_OUTPUT
	bOk		: BOOL;
END_VAR

	// Liste allozieren
	pDestructors			:= _alloc(sizeof(DEST_STACK)) $ ^DEST_STACK;
	IF pDestructors = NIL THEN
		bOk	:= FALSE;
		RETURN;
	END_IF;

	// Pointer-Liste allozieren, damit danach mit realloc gearbeitet werden kann
	pDestructors^.pData		:= _alloc_resize_LDR(1)$^pVoid;
	IF pDestructors^.pData = NIL THEN
		bOk	:= FALSE;
		RETURN;
	END_IF;

	pDestructors^.udSize	:= 0;		// Anzahl der Einträge
	pDestructors^.bOk		:= TRUE;	// Listeninhalt ist konsistent

	bOk	:= TRUE;

END_FUNCTION
//[#DEUTSCH]
//Diese Funktion wird von C-Compileren in die Registrier-Funktionen von
//Destruktoren eingetragen. Im Fall des LASAL-OS wird dann diese Funktion
//aufgerufen, und der Desturktor-Pointer in die statische Pointer-Liste
//eingetragen. Zuvor muß unbedingt LslAtexitInit aufgerufen werden.
//[>pFu] Zeiger auf einen Destruktor
//[#ENGLISH]
//Calls to this function are generated by C-compilers within the registration
//functions of destructors. With the LASAL-OS the atexit builds up a static
//pointer list with the given destructor pointers. The LslAtexitInit function
//has to be called before.
//[>pFu] pointer to a destructor
FUNCTION GLOBAL __CDECL atexit
VAR_INPUT
	pFu		: ^void;
END_VAR
VAR
	udSize		: UDINT;
END_VAR

	// keine neuen Einträge in kaputte Liste
	IF pDestructors^.bOk = FALSE THEN
		RETURN;
	END_IF;

	udSize := pDestructors^.udSize;

	// blockweise allozieren
	IF (udSize AND 16#F) = 0 THEN
		
		pDestructors^.pData		:= _alloc_resize_LDR((udSize + 16#10) * sizeof(pVoid))$^pVoid;
		IF pDestructors^.pData = NIL THEN
			pDestructors^.bOk	:= FALSE;
			RETURN;
		END_IF;
		
	END_IF;

	(pDestructors^.pData+udSize*sizeof(pVoid))^$pVoid	:= pFu;
	pDestructors^.udSize	:= udSize + 1;

END_FUNCTION
//[#DEUTSCH]
//Mit dieser Funktion kann die Gültigkeit der statischen Destruktor-Liste
//überprüft werden.
//[<bOk] TRUE Liste ist in Ordnung, FALSE Liste konnte nicht vollständig alloziert werden
//[#ENGLISH]
//This function returns, if the static list of destructor-pointers is valid or not.
//[<bOk] TRUE if the list is valid, FALSE if not
FUNCTION GLOBAL __CDECL LslAtexitOk
VAR_OUTPUT
	bOk		: BOOL;
END_VAR

	bOk := TO_BOOL(pDestructors^.bOk);

END_FUNCTION

FUNCTION GLOBAL SramCheckerCyclic;

//------------------------------------------------------------------------
// CancelDebugIps
//
//  Bricht laufende Debug Interpreter ab. 
//------------------------------------------------------------------------
FUNCTION CancelDebugIps
VAR
#ifndef DUMMY_LOADER
	pIp     : ^DebugIp;
  pHead		: ^IprHead;
#else
	pIp     : pVoid;
  pHead		: pVoid;
#endif
END_VAR

#ifndef DUMMY_LOADER
  pIp	:= OPS.ptPgBuff;
  WHILE pIp DO			// call the BefIpr for all active interpreters

		pHead := pIp^.GetHead();
    IF pHead <> NIL THEN
      pHead^.funkst		:= ERROR;
      pHead^.WorkState	:= ERROR;
    END_IF;

    pIp   := (pIp^.GetNextIp()) $ ^DebugIp;

  END_WHILE;
#endif // DUMMY_LOADER

END_FUNCTION

//------------------------------------------------------------------------
// OpCyclicDebugIp
//
//  Diese Funktion kann auch asynchron aus dem DebugHandler aufgerufen 
//  werden. Es wird daher ein Flag executingDebugIp verwendet, um eine 
//  Rekursion zu vermeiden.
//------------------------------------------------------------------------
FUNCTION OpCyclicDebugIp
VAR
#ifndef DUMMY_LOADER
	pIp     : ^DebugIp;
	pNext		: ^BaseIp;
#else
	pIp     : pVoid;
	pNext		: pVoid;
#endif
END_VAR

#ifndef DUMMY_LOADER
  IF !!executingDebugIp THEN

    executingDebugIp := TRUE; // Rekursion vermeiden

    pIp	:= OPS.ptPgBuff;
    WHILE pIp DO			// call the BefIpr for all active interpreters
  
      pIp^.CyWork();
  
      IF pIp^.m_bMarkedForDeletion THEN	//++bugfix040628
        // The DebugIp object should be removed from the list
        pNext := pIp^.GetNextIp();
        pIp^.ReleaseCommPort(pIp^.m_udIdTool);
        pIp	  := pNext $ ^DebugIp;
      ELSE
        pIp   := (pIp^.GetNextIp()) $ ^DebugIp;
      END_IF;
  
    END_WHILE;
    
    executingDebugIp := FALSE;
    
  END_IF;
  
#endif // DUMMY_LOADER

END_FUNCTION

//------------------------------------------------------------------------------
// SysCyclic
//------------------------------------------------------------------------------
FUNCTION SysCyclic

	OPS.runstats.usSysCyclicCnt += 1;

  IF opCyclicFlag THEN
    // Nach OpCyclic wird das erste mal SysCyclic aufgerufen.
    IF (_RtOSversion >= 16#1000) & (OPS.loaderFlags AND LOADER_ISAPPLVALID) THEN
      // Die Interpreterprogramme werden im SysCyclic Task nicht mehr 
      // aufgerufen, darum werden sie hier auf den Status ERROR gesetzt, 
      // damit das Lasal nicht fälschlicherweise meint, daß sie noch laufen.
#ifndef DUMMY_LOADER
      IprMgr::HaltProgsImmediately();
#endif      
      // Ab x.x.51 werden die Debug Interpreter hier abgebrochen. 
      // Wenn das nicht gemacht wird, dann kann folgender Fall eintreten:
      // - übers Lasal wird ein Server-Wert geändert
      // - der DebugIp ruft die Write Methode des Servers auf
      // - in der Write Methode ist eine Endlosschleife
      // - die Runtime Überwachung schlägt zu, das OS terminiert den Cyclic Task 
      //   und startet einen neuen (LT-Task)
      // - die Loaderfunktion SysCyclic wird jetzt zyklisch aufgerufen und der 
      //   DebugIp findet eine Anforderung zum Aufruf der Write Methode vor, und 
      //   zwar die zuvor abgebrochene mit der Endlosschleife !!!
      CancelDebugIps();
    END_IF;        
  END_IF;
  opCyclicFlag := FALSE;

	OpCyclicDebugIp();

END_FUNCTION

//------------------------------------------------------------------------
// OpCyclic
//------------------------------------------------------------------------
FUNCTION GLOBAL OpCyclic

	OPS.runstats.usApplCyclicCnt += 1;

  opCyclicFlag := TRUE;

#ifdef CYCLIC_SRAM_CHECK
	SramCheckerCyclic();
#endif

#ifdef COMLINK_LASAL
	ComLinkCyWork();
#endif // COMLINK_LASAL


#ifdef DATASERVICE_LASAL
LDR_DS_RunCy();
LDR_DS_RunBk();
#endif

//--------------------------------------------------------------------
// Pg Interface
//--------------------------------------------------------------------

  OpCyclicDebugIp();
  
//--------------------------------------------------------------------
// interpreters
//--------------------------------------------------------------------

#ifndef DUMMY_LOADER
  IF callProgsequencesInCyclic <> 2 THEN
    IprMgr::ProgSequences();
    IF callProgsequencesInCyclic = 1 THEN
      callProgsequencesInCyclic := 2;
      OS_AddCyFunc(IprMgr::ProgSequences, NIL, TASK_ALWAYS, 0, callProgsequencesInCyclic_udPhase);
    END_IF;
  END_IF;
#endif    

  ProcessAsyncFileObjects();

END_FUNCTION


(*
  Mit dem Aufruf dieser Funktion ändert sich der Aufruf des Programminterpreters (IprMgr::ProgSequences). 
  Er wird dann nicht mehr aus der zyklischen Loaderfunktion (OpCyclic), sondern als Cyclic-Function 
  im Cyclic-Task ausgerufen. 
  
  \param udPhase Dieser Parameter wird an OS_AddCyFunc übergeben. Damit kann angebeben werden,
      in welcher Phase der Cyworks der Programminterpreter aufgerufen wird
      (TASK_PRE, TASK_SCAN, TASK_POST)
      Achtung: unter RTK funktioniert nur TASK_SCAN, bei TASK_PRE und TASK_POST gibts eine Exception !
*)
FUNCTION GLOBAL LDR_CallProgsequencesInCyclic
VAR_INPUT
  udPhase	: UDINT;
END_VAR
  callProgsequencesInCyclic := 1;
  callProgsequencesInCyclic_udPhase := udPhase;
END_FUNCTION


#pragma warning (disable:073)

#ifdef _LSL_TARGETARCH_ARM
FUNCTION dummyLoaderTracePrint
VAR_INPUT
  a : ^char;
  b : char;
END_VAR
VAR_OUTPUT
  retc (EAX) : dint;
END_VAR

  retc := 0;

END_FUNCTION
#else
FUNCTION dummyLoaderTracePrint
VAR_INPUT
  a : ^char;
  b : char;
END_VAR
VAR_OUTPUT
  EAX : dint;
END_VAR

  EAX := 0;

END_FUNCTION
#endif
#pragma warning (default:073)
FUNCTION dummyLoaderTraceInit
END_FUNCTION
