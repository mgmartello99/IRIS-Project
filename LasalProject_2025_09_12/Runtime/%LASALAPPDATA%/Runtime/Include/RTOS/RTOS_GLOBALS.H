#pragma once
#define __RTOS__GLOBALS__


#define NLNG	16#ff		// maximum length of an identifier

#define SPC		16#20		// space character

#define IRQTIME	1			// time base 1 msec irqscan

#define DONT_CHECK_OS_0500	// use new memory management, needing OS-version >= 5.00

#define ACCESS_DENIED	(16#80000010)$DINT	// inform LSE to refresh a channel after write,
					// use this value as a return value of the write-method

// to get size of standard command table exclusive rd,wr and no
#define nSTDCMD	((CmdMeth.GetState-CmdMeth.Init)/sizeof(pVoid)+1)$uint

TYPE

//	FctDummyTyp		:STRUCT e1 : USINT; END_STRUCT;
//	pFct			: ^FctDummyTyp;	// displays ASCII-name
	pFct			: ^void;		// displays ASCII-name
	void			: (void := 1) $ USINT;	// C-compatibility
	pVoid			: ^void;
	pChar			: ^CHAR;		// character string

	SvrCh			: STRUCT		// server channel basic structure
		pMeth		: ^CHMETH;		// pointer to method table
		dData		: DINT;
		pSvrDsc		: ^SVRDSC;
		END_STRUCT;

	SvrChCmd		: STRUCT		// at least the very first channel of a class
		pMeth		: ^CMDMETH;		// pointer to method table
		dData		: DINT;
		pSvrDsc		: ^SVRDSC;
		END_STRUCT;

	CltCh			: STRUCT		// counterpart to SvrCh
		pCh			: ^SVRCH;
		dData		: DINT;
		pCmd		: ^VirtualBase;
		END_STRUCT;

	CltChCmd		: STRUCT		// counterpart to SvrChCmd 
		pCh			: ^SVRCHCMD;
		dData		: DINT;
		pCmd		: ^VirtualBase;
		END_STRUCT;

	ChMeth			: STRUCT		// struct of an ordinary server channel
		pRd			: pFct;			// which supports only read and write
		pWr			: pFct;			// accesses
		pNxt		: ^CHMETH;
		END_STRUCT;


	CmdMeth			: STRUCT		// minimum structure of a command meth table
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
		END_STRUCT;

	CmdMethMode		: (			// command table per class or per object
		SHARED,
		EXCLUSIVE
			)$UINT;

	CmdMethSize		: DINT;

	SvrDsc			: STRUCT    // server channel descriptor
		Atts		: BINT		// has do be defined
		[
		];
		FileAtts	: BINT;
		AcsLvlRd	: UINT;		// minimum accesslevel for read
		AcsLvlWr	: UINT;		// minimum Accesslevel for write
		END_STRUCT;

	ChMode			: (
		_CH_OBJ,			// return value for pure object names
		_CH_CMD,
		_CH_SVR,
		_CH_CLT_CMD,
		_CH_CLT_DATA,
		_CH_CLT_OBJ,		// typed object channel for external objects
		_CH_OBJ_DIR,		// LASAL Debugger: some part evaluated by LASAL
		_CH_CNST,			// unconnected client
		_CH_NONE,
		_CH_CLT_EMB,		// typed object channel for embedded objects 
		_CH_SVR_OBJ,
    _CH_CLTSVR,   // magic lient/server

		_CH_TEMP_VBI_OBJ,	// for internal use, obj-ch to VBI-obj
		_CH_TEMP_VBI_EMB	// for internal use, cmb-ch to VBI-obj
			)$UINT;  //ws002: compatible with Lasal Class

		GUID			: STRUCT	//ws: compatibility with Lasal Class
 		dw0			: UDINT;
		dw1			: UDINT;
		dw2			: UDINT;
		dw3			: UDINT;
		END_STRUCT;					// /ws

END_TYPE

#include "RTOS_Interpreter.h"

CLexem : CLASS
  //Servers:
  //Clients:
  //Variables:
		udCRC 	: UDINT;
		Text : ARRAY [0..0] OF CHAR;

  //Functions:
	FUNCTION GLOBAL iCompare
		VAR_INPUT
			pText 	: ^CLexem;
		END_VAR
		VAR_OUTPUT
			equal 	: BOOL;
		END_VAR;
	
	FUNCTION GLOBAL Length
		VAR_OUTPUT
			udLen 	: UDINT;
		END_VAR;
	
	FUNCTION GLOBAL Size
		VAR_OUTPUT
			udSize 	: UDINT;
		END_VAR;
END_CLASS;

// zur Verwendung als Klasse für lokale CLexem - Variablen
CLocLex			: CLASS : CLexem
	dummy		: ARRAY[0..NLNG] OF CHAR;	// Feld zur Aufnahme des Texts

	FUNCTION GLOBAL InitText
	VAR_INPUT
		pText	: ^CHAR;
	END_VAR;
END_CLASS;

TYPE

	ConfStates                      : (
		C_INIT,				// start of initialization, configuration in process
		C_RUNRAM,			// LASAL project was successfully started from RAM
		C_RUNROM,			// LASAL project was successfully started from ROM

		C_RUNTIME,			// unused
		C_READY,			// unused
		C_OK,				// unused
		C_UNKNOWN_CID,		// unknown class of stand-alone or embedded object 
							// or unknown base class
		C_UNKNOWN_CONSTR,	// OS interface class couldn't be constructed, probably wrong OS
		C_UNKNOWN_OBJECT,	// reference to an unknown object in an interpreter program
							// constructing more than one DCC080-objects
							// tried to create a task for an unknown object
		C_UNKNOWN_CHNL,		// HW-module number greater than 60
							// tried to connect unknown channels
							// creating an embedded object for an unknown parent channel
		C_WRONG_CONNECT,	// unconnected required channels
							// tried to connect unknown channels
		C_WRONG_ATTR,		// wrong server attributes
		C_SYNTAX_ERROR,		// unspecific error, recompile all component projects, download all
		C_NO_FILE_OPEN,		// tried to open an unknown table
		C_OUTOF_NEAR,		// memory allocation failed
		C_OUTOF_FAR,		// unused
		C_INCOMPATIBLE,		// object with the same name already exists, but has a different class
		C_COMPATIBLE,		// object with the same name and class already exists, has to be updated
		C_DESTRUCTORS,		// when passed to main, the destructors of static C-objects are called
		C_INVALID_CHNL		// client channels must not be used in IPRs
		C_SYSTEM_ERROR,		// system error
		C_EXIT,             // exit the application
		C_SYSINIT,          // loader initialization
		C_SYSOK,            // run system cyclic work (no application available)
		C_SYSEXIT           // exit the loader
			)$UINT;

	Obj				: STRUCT	// required object information
		pClsHdr		: ^CLSHDR;
		pNxtObj		: ^OBJ;
		pObjDsc		: ^OBJDSC;	// points to descriptor
		END_STRUCT;

								// 1 word ClassID, 1 word VersionNo
	ClsID			: STRUCT
		ID			: UINT;
		Rev			: USINT;
		Fix			: USINT;
		END_STRUCT;

	Revision		: STRUCT
		Hi			: UINT;
		Lo			: UINT;
		END_STRUCT;

#pragma pack(push, 1)
	ClsHdrConst		: STRUCT
		// flags for class-type field, constant names are TY_<class-name>
		// see CT_-tables
		#define  UPD_AUTOMATIC   16#8000	// objects of this class are automatically updated
		#define  UPD_PRESCAN     16#4000	// 1..during prescan, 0.. during postscan
		#define  UPD_HIPRIOR     16#2000	// 1..with high priority (1 ms), 0..10 ms

		#define  CTY_WATCHDOG	 16#1000	// Cy/RtWork is called to adjust PLC load

		#define	 CTY_OSI_CLASS	 16#0008	// OS-interface class
		#define  CTY_HW_CLASS    16#0004	// the class is a hardware class
		#define  CTY_USERCLASS   16#0002	// the class was not developed by Sigmatek
		#define  CTY_NEWEST_CLS  16#0001	// this is the latest version of the class

		// definition of what high and low priority mean
		#define  UPD_T_HIPRIOR   1  ms		// short intervall for automatic update
		#define  UPD_T_LOPRIOR   10 ms		// long  intervall for automatic update

		ClsType		: UINT;		// identifier of class //ws002: to match with ch_mode - undone (udint to be compatible with Lasal class)
		udRev		: Revision;	// class revision
		lObj		: UINT;		// length of an object in bytes
		nSvrChs		: UINT;		// number of server channels
		nCltChs		: UINT;		// number of client channels

		uiReserve	: UINT;		// to align the CRC on a 4-byte address

		Name		: CLexem;	// symbolic entry, real length is longer
		END_STRUCT;
#pragma pack (pop)

(*#pragma pack(push, 4)
	cGuid			: STRUCT  //ws002: compatible with Lasal class
		uiIdx		: UINT;
		guid		: GUID;
		usRevHi		: USINT;
		usRevLo		: USINT;
		END_STRUCT;			 // /ws002 
#pragma pack(pop)
*)
	ChDsc			: STRUCT
		pDsc		: ^ChDscEntry;	// write protected descriptor
		pCls		: ^ClsHdr;		// for object channels only
		END_STRUCT;

	ClsHdr			: STRUCT		// standard class header
		//ID			: cGUID;  //ws002: compatible with Lasal class 
		pDsc		: ^ClsHdrConst;

		nObjs		: UINT;		// nr of objects
		pNxtCls		: ^CLSHDR;
		pBaseCls	: pClsHdr;
		pFirstObj	: ^OBJ;
		pLastObj	: ^OBJ;	
		pConstructor: pFct;
		ClsState	: UINT;		// runtime information, i.e. CTY_NEWEST
								// the next entries only show the order, not
								// the real length of ClsHdr-components
		aDscChs		: ARRAY [0..0] OF ChDsc;
(*
		aDscMeth	: ARRAY [0..0] OF METHDSCENTRY;
		aChNames	: ARRAY [0..0] OF CHAR;
		aClsName	: ARRAY [0..0] OF CHAR;  // -"-
		aMethNames	: ARRAY [0..0] OF CHAR;  // -"-
		aInitValues	: ARRAY [0..0] OF INITDSCENTRY;
		aInternConnect : ARRAY[0..0] OF InternConnect;
*)
		END_STRUCT;

	                          	// object descr for search by name (far memory)
#pragma pack(push, 1)
	ObjDsc			: STRUCT
		Flags		: BINT
		[
			Embedded,			// object has a parent: "parent\child"
		];
		SymName		: CLexem;	// not real length
		END_STRUCT;
#pragma pack(pop)

	pObjDsc			: ^ObjDsc;

	MethDscEntry	: STRUCT	// function header of the CMD-methods
		nParas		: USINT;	// #parameters
		nRets		: USINT;	// #return values
		pName		: ^char;	// ptr to method name
		END_STRUCT;

	InitDscEntry	: STRUCT	// values initialized by loader from config-tables
		uiOff		: UINT;		// offset of data element in the class
		uiSize		: UINT;		// size of the init value
		END_STRUCT;

	pSvrCh			: ^SVRCH;

	pClsHdr			: ^CLSHDR;

	#define _MAX_COMCHS_ 6
	comIF			: ARRAY [0.._MAX_COMCHS_-1] OF ^COMDATA;

#pragma pack( push, 1 )
	ChDscEntry		: STRUCT	// channel properties
		uiOff		: UINT;		// offset of channel in the class
		uiMode		: CHMODE;	// client, server, aso.
    uiFlags   : BINT[
      Retentive,        // die Daten des Kanals werden 0-Spannungssicher gespeichert
      Required,         // für diesen Client - Kanal muß eine Verbindung angegeben werden
      Reserved,
      Remanent,
      IntoFile,
      Existed,
      Redirect,
    ];
		Name		: CLexem;	// ptr to channel name
		END_STRUCT;

	OPSys			: STRUCT		// Realtime task data
		tAbsolute	: UDINT;		// absolute time since start in ms
		SysState	: CONFSTATES;	// LASAL RunTime states
		ptPgBuff	: ^DebugIp;		// Communication interface for debugger
		CH16buf		: STRUCT		// Channel16 to make CAN connect
			CANID_REQCH	: INT;		// CANID of requester
			res1		: USINT;
			res2		: USINT;
			END_STRUCT;
		udDescCRC		: UDINT;	// CRC of the active project, calculated from
									// all the descriptor lists
		pSequDir	: ^IPRDIR;		// interpreter objects
		pProg		: ^PRGHEAD;		// interpreter programs
		ComChs		: COMIF;		// ptrs to communication channels
		ConfigTime	: UDINT;		// time for configuration
		CanConnect	: ARRAY [0..7] OF SINT;	// used by Appli with MAE60-visu

		OverRun		: UINT;			// counts overrun conditions
	  
		RtInterv_mSec: UDINT;		// set by OS since v 3.60, before always set to 1
		RtInterv_uSec: UDINT;		// always set to 1000 * RtInterv_mSec


		// variables for exchanging IP-programs
		udProgLength	: UDINT;	// total size of program
		udProgSize		: UDINT;	// remaining size of program
		uiProgNoLoad	: UINT;		// index of currently loaded program
									// programs to load from temp buffer to regulary buffer
		uiProgsToLoad	: UINT;		// bitfield (nr-prog = bit)
		pDestProg		: ^void;	// destination of currently loaded program

		uiLoaderVersion : STRUCT		// loader revisions:
			usLoRev			: USINT;
			usHiRev			: USINT;
			END_STRUCT;
			
		sizeofStruct : UINT;    // size of the OPS-structure
		loaderFlags : UINT;     // 
		tAbsoluteTicks : UDINT;  // tAbsolute in Timer ticks
		tAbsoluteMuSec : UDINT;  // tAbsolute in µs
		
    runStats : STRUCT
        usSysInitCnt : USINT;
        usSysCyclicCnt : USINT;
        usSysExitCnt : USINT;
        usApplInitCnt : USINT;
        usApplCyclicCnt : USINT;
        usApplExitCnt : USINT;
        debug : UDINT;
		    END_STRUCT;
		
		END_STRUCT;

  		ONEOBJ : STRUCT				// one OBJ in a Map
    	  	CRC  :    DINT;
    		pOBJ :   ^Obj;
  		END_STRUCT;

		CRCMap : STRUCT				// there are more then 500 it is for development
    		nr :   UDINT;
			is_sorted : UDINT;
			has_to_be_sorted : UDINT;
			errorcnt : UDINT;
            maxnr : UDINT;
    		OBJs : ARRAY[0..500] OF ONEOBJ;
  		END_STRUCT;


#pragma pack (pop)

#pragma pack( push, 1 )
	RetSvr		: STRUCT
		pmeth		: ^void;
		flags		: uint;
    pSaveObj : ^void;
	END_STRUCT;
#pragma pack( pop )

END_TYPE
