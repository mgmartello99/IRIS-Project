#pragma once
#define __RTOS__IPR__INTERN__

#define MAXNESTING      10
#define ANZ_FPTRS		10
#define MAX_BREAKPOINT	10
#define INVALID_LABEL	16#FFFF_FFFF

#include <RTOS_HEADER.h>

#define MAX_IP_PROGS	64		// max. # of IPR Programs

// *************************************************************************
//			data types for internal use
// *************************************************************************
TYPE
	CLTCHCMD_IprInternRamEx	:STRUCT
		pCH		:^SVRCHCMD_UDINT;
		dData	:UDINT;
		pCmd	:^RAMex;
	END_STRUCT;

		// instruction set for interpreter
	InstrSet                        : (
		I_GET_OBJ,             // name -> handle
		I_GET_OBJ_CLS,         // name -> handle and class name
		I_READ_CLASS,          // index -> class info
		I_WRITE,               // write to server
		I_WRITE_TO_CLNT,       // write to the server connected to a client
		I_READ_OBJECT,         // class header, index -> object info
		I_READ_CHANNEL,		   // read the channel
		I_READ_METHOD,         // class header, index -> method info
		I_READ,                // handle -> read server channel
		I_STARTPROG,           // program no, label no -> start parallel
		I_RUNPROG,             // program no, label no -> start sequential
		I_CMD,                 // handle, cmd, paras -> execute method
		I_DELAY,               // time -> wait till time elapsed
		I_NOP,                 // ==== not used   
		I_LBL,                 // ==== not used
		I_GOTO,                // goto label
		I_ENDPROG,             // terminates the current ipr program
		I_CALL,                // subroutine call
		I_RET,                 // return from subroutine
		I_FUNCTION,            // ======= not used
		I_ENDFUNCTION,         // ======= not used
		I_INC,                 // handle -> increments data image
		I_DEC,                 // handle -> decrements data image
		I_START_LOAD_PR,       // prepare OPsystem for new program to load
		I_LOAD_PROG,           // load new program code
		I_COMMENT,             // line is not executed
		I_JMPIF,               // jump if condition is true
		I_WAITFOR,             // wait until condition is true
		I_SETFORTIME,          // ======= not used
		I_GETPROG,             // load program code from the PLC
		I_GETPROGSTATE,        // program no -> state
		I_CHECK_FOR_LOAD,      // check if new progs are to load
		I_GET_DESC_CRC,        // CRC of all LASAL-descriptor lists in the PLC
		I_READ_CONNECT,		   // get the client connection
		I_GET_CLS,			   // get the class name from an object's address
		I_GET_OBJ_NAME,		   // get object name from class name and object addr
		I_READ_CLT,			   // call read method of connected server
		I_STOPPROG,				// stops a interpreter program
		I_GET_PROGNR,			// get programnumber of programname
		I_GET_ACT_OFFSET,		// get the actual offset of the program
		I_GET_TRIGGER_COUNT,	// get the count of the trigger
		I_GET_VERSION,			// returns the loader version

		I_NEW_OBJ,				// creates an object of a class
		I_CONNECT,				// connect 2 channels
		I_SET_CYCLE,			// set real- or cyclic time
		I_DEL_OBJ,				// delete an object, remove connections
		I_STOP_IPR_CHECK_FOR_LOAD, //stop all interpreter and checkforload
		I_REGISTER,				// for login and to inform the PLC that the channel is still used
		I_RELEASE,				// release the communication channel at end of debug session
		I_TRY_SOFTLOAD_IPR,		// Try to load an program from the temporary memory in a soft mode
		I_END_SOFTLOAD_IPR,		// End the softload
		I_GET_CFL_CYCLE,		// Get the Cycle time of the CheckForLoad
		I_GET_CALLED_IPR,		// Liefert die Nummer des Interpreters der von diesem Ipr aufgerufen wurde
		I_GET_CALLED_FROM,		// Bringt die Nummer des Iprs von dem dieser Ipr aufgerufen wurde

		I_INIT,					// calls the init-method of a server or an object
		I_GET_CLS_BY_NAME,		// gets a class header pointer for a given class name
		I_GET_NXT_DERIVED,		// get next derived class of a base class
		I_LOCK,					// lock/unlock communication buffer
		I_UNLOCK,
		I_LSLCMD,
		I_CMD_DEBUGIP,      	// gleich wie I_CMD, nur wird für die Serveradresse der this-Pointer (DebugIp Objekt) verwendet
		I_CREATE_PROG,  (*** ++pr:test ***)
		I_LOAD_PROG2,			// wie I_LOAD_PROG jedoch ohne den Fehler mit der falschen Länge bei der Checksummenberechnung
    I_START_IPR,    (*** ++pr:test ***)
    I_GET_GLOBAL_ADDR,
    I_GET_DATA,
		I_GETPROGSTATE_ALL,        // program state
    I_SET_DATA,
    I_GET_STACKINFO,
		I_GET_OBJ_LIST,
		I_VISU_RDY,      	// wenn in einer Init die Glob. Variable "bWait4Visu" gesetzt wurde, muss diese Kommando von der Visu geschickt werden, damit die CPU die Init abschließt.
    I_RD_SVRLIST,     // damit auf mehrere server mit einem aufruf gelesen werden kann
    I_WR_SVRLIST,     // damit auf mehrere server mit einem aufruf geschrieben werden kann
    
    I_INIT_MERKEREX,  // Nur für MerkerEx Klassen mit Rev > 1.80 , startet einen lese oder schreib zugriff auf die kompletten merkerEx daten
    I_MERKEREX,       // Nur für MerkerEx Klassen mit Rev > 1.80 , liest aus bzw schreibt in dem in I_INIT_MERKEREX angelegten Buffer 
   
		I_ILLEGAL)$USINT;

	Prefix                          : (
		P_IMMED,				// data source is a constant
		P_VARIA,				// data source is a channel
		P_SYS,					// data source are system variables

		P_POPEN,				// open parenthesis
		P_PCLOSE,				// close parenthesis
		P_ADD,					// addition
		P_SUB,					// subtraction
		P_COMMA,				// 
		P_EOL,					// end of line
		P_EQ,					// comparisons
		P_NEQ,
		P_GT,
		P_GEQ,
		P_LT,
		P_LEQ,
		P_NOT,					// binary negation
		P_AND,
		P_OR,
		P_XOR,

		P_USER_STREAM,			// no special format for parameter list

		P_MUL,
		P_DIV,

		P_ILLEGAL)$USINT;

	sysFunct                        : (
	  RD_ABSTIME,
	  SYS_RESULT,
	  SYS_ILLEGAL)$UINT;

#pragma pack(push,1)	// because LASAL is transmitting packed data

	instruct                        : STRUCT  // structure of an instruction
		// length of interpreter code line, include 2 bytes for uiLng
	  uiLng                         : UINT;
		// interpreter command
	  opCode                        : INSTRSET;
	  	// variable length of parameters, usually terminated by P_EOL
	  aPara                         : ARRAY [0..NLNG-1] OF USINT;
	 END_STRUCT;

	iprHead                         : STRUCT
		// state of the interpreter
	  funkSt                        : IprStates;
	  	// used for writing commands to an ipr, i.e. from LASAL
	  workState                     : IprStates;
	  	// pointer to actual instruction
	  pCode                         : ^INSTRUCT;
	  	// pointer to result
	  pResu                         : ^RESULTS;
	  	// start of interpreting actual line 
	  tStart                        : UDINT;
	  fPtrs                         : ARRAY [0..ANZ_FPTRS-1] OF ^VirtualBase;
	  workStateIntern								: IprStates;
	 END_STRUCT;

	SubPrgHead                      : STRUCT         	// header of a sub prog
	  udLng                         : UDINT;         	// length of program
	  udCRC                         : UDINT;       		// crc32 for the program
	  szName                        : ARRAY [0..31] OF CHAR;  // name of pro
	  udOffLbl                      : UDINT;        		// offset of labeltabelle
	  udOffLinks                    : UDINT;          	// offset of linktabele
	  udOffCode                     : UDINT;          	// offset of code
	  udOffProgLinks				: UDINT;			// offset of the programmlinktable
	  udCRCorig						: UDINT;			// CRC of the unlinked program
	  udCRCHead						: UDINT;			// CRC of the Head
	 END_STRUCT;

	pSubPrgHead						: ^SUBPRGHEAD;

	NewSubPrgHead					: STRUCT
		udFileCheck					: UDINT;	
		subProgHead					: SubPrgHead;
	  END_STRUCT;

	pNewSubPrgHead					: ^NewSubPrgHead;

#pragma pack (pop)

	pProgIp							:^ProgIp;

	IprDir                          : STRUCT   			// directory of interpreters
		uiIprCnt					: UINT;
		pFirstIpr					: ^ProgIp;
	  END_STRUCT;

	                                       						// header of the sequence program
	PrgHead				: STRUCT
		uiProgs			: UINT;		// number of programs
		pTable			: pVoid;	// pointer to config table
		udOffInit		: UDINT;	// offset to init data

		pProg1			: ^RamEx;	// 1st prog
		// n-1 pointers to IPR-programs following
	END_STRUCT;
#pragma pack(push,1)	// used in IP-code

		// internal structure for label references								        // directory with pointers to SequPrgs
	Label                           : STRUCT         // structure of a label
	  uiLineNo                        : UINT;
	  udLineOffset                    : UDINT;
//	  name                          : ARRAY [0..16] OF CHAR;
	 END_STRUCT;

	 ptrLabel	: ^Label;

#pragma pack (pop)
	LOAD_STATE	:(
	LS_NONE,
	LS_WAIT,
	LS_LOADED
	)$UINT;

	//Struktur für einen Interpreterbreakpoint
	IprBreakpoint: STRUCT
		dOffset : UDINT;//Offset des Breakpoints
		dIprCRC : UDINT;//Ungelinkte Checksumme des Interpreters
	END_STRUCT;
	
END_TYPE
