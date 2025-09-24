////////////////////////////////////////////////////////////////////////////////
//
// LSL.H
//
//  Include-File mit den Lasal-Definition.
//  
//  Anm.pr:
//    Früher wurde RTOS_CGlobals inkludiert. Nachdem aber Lasal2 manche Daten-
//    strukturen geändert hat ohne den Namen der Struktur zu ändern, habe 
//    ich dieses Headerfile als Ersatz für RTOS_CGlobals erstellt. 
//    Dieses Headerfile enthält jetzt nur mehr die Datenstrukturen, die im 
//    Betriebssystem benötigt werden und diese dürfen auch vom Lasal nicht 
//    mehr geändert werden !
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LSL_H_INCLUDED_
#define _LSL_H_INCLUDED_

typedef unsigned short	_UINT;
typedef unsigned char	USINT;
typedef unsigned long	UDINT;
typedef unsigned short	BINT;
typedef long			DINT;
typedef void* PVOID;
typedef DINT CMDMETHSIZE;
typedef UDINT CHMODE;

//
//	Loader Configuration States
//
typedef enum {
	C_INIT,										 // start of initialization	
	C_RUNRAM,
	C_RUNROM,
	C_RUNTIME,
	C_READY,
	C_OK,
	C_UNKNOWN_CID,
	C_UNKNOWN_CONSTR,
	C_UNKNOWN_OBJECT,
	C_UNKNOWN_CHNL,
	C_WRONG_CONNECT,
	C_WRONG_ATTR,
	C_SYNTAX_ERROR,
	C_NO_FILE_OPEN,
	C_OUTOF_NEAR,
	C_OUTOF_FAR,
	C_INCOMPATIBLE,		// object with the same name already exists, but has a different class
	C_COMPATIBLE, 	 	// object with the same name and class already exists, has to be updated
	C_DESTRUCTORS,		// when passed to main, the destructors of static C-objects are called
	C_INVALID_CHNL,		// client channels must not be used in IPRs
	C_SYSTEM_ERROR,		// system error
	C_EXIT,			      // exit the application
	C_SYSINIT,			  // loader initialization
  C_SYSOK,          // run system cyclic work (no application available)
	C_SYSEXIT,			  // exit the loader
} CONFSTATES;

#pragma pack(push, 1)

typedef struct
{
	UDINT dw0;
	UDINT dw1;
	UDINT dw2;
	UDINT dw3;
} lslGUID;

typedef struct
{
	_UINT uiIdx;
	lslGUID guid;
	USINT usRevHi;
	USINT usRevLo;
} cGUID;

#pragma pack(pop)

#pragma pack(push, 4)

//typedef struct SVRCH SVRCH;
//typedef struct OBJ OBJ;
//typedef struct CLSHDR CLSHDR;
//typedef struct OBJDSC OBJDSC;
typedef struct SVRDSC SVRDSC;
//typedef struct CHMETH CHMETH;
//typedef struct CHDSCENTRY CHDSCENTRY;

typedef struct
{
	void* pRd;		// Pointer to Read function
	void* pWr;		// Pointer to Write functions
	void* pNxt;		// Pointer to next command table in OS list (^CHMETH)
} CHMETH;

typedef struct
{
	void* Read;
	void* Write;
	void* pNxt;			// pointer to next tbl in OS list (^CMDMETH)
	CMDMETHSIZE nCmds;  // nr of following cmds
	void* Init;         // initialise
	void* CyWork;       // entry for cyclic work
	void* RtWork;       // entry for Reatime WOrk
	void* NewInstr;     // enter new comands
	void* Kill;         // kill actual action
	void* GetState;     // get state of function
	void* BackCheck;	// (not currently used ???)
	void* optMeth1;		// start of any optional entries)
} CMDMETH;

typedef struct SVRCH
{
	CHMETH* pMeth;		// pointer to method table
	DINT dData;			// Server "data store"
	SVRDSC* pSvrDsc;	// Pointer to server channel descriptor
} SVRCH;

#pragma pack(push, 1)
typedef struct
{
	void *pNxtDsc;		// point to next Objdscr (^OBJDSC)
	void* pObj;			// point back to the object (^OBJ)
	UDINT CRC;			// CRC for fast name compare
	BINT Flags;			// Flag word
		#define Embedded 0x01	// Set if object is embedded
	// SymName                       : ARRAY [0..0] OF CHAR;
} OBJDSC_LSL1;

// Object Descriptor im Lasal2
typedef struct
{
	BINT Flags;			// Flag word
	UDINT CRC;
  // SymName                       : ARRAY [0..0] OF CHAR;
} OBJDSC_LSL2;
#pragma pack(pop)

typedef struct
{
		cGUID ID;

	// flags for class-type field, constant names are TY_<class-name>
	// see CT_-tables
	#define  UPD_AUTOMATIC   0x8000  // objects of this class are automatically updated
	#define  UPD_PRESCAN     0x4000  // 1..during prescan, 0.. during postscan
	#define  UPD_HIPRIOR     0x2000  // 1..with high priority (1 ms), 0..10 ms

	#define  CTY_HW_CLASS    0x0004	// the class is a hardware class
	#define  CTY_USERCLASS   0x0002  // the class was not developed by Sigmatek
	#define  CTY_NEWEST_CLS  0x0001  // this is the latest version of the class

	// definition of what high and low priority mean
	#define  UPD_T_HIPRIOR   1  // ms   // short intervall for automatic update
	#define  UPD_T_LOPRIOR   10 // ms   // long  intervall for automatic update

	_UINT ClsType;      // identifier of class
	_UINT nObjs;        // nr of objects
	_UINT lObj;  		// length of an object in bytes
	void* pNxtCls;		// pointer to next CLSHDR in list
	void* pLastObj;		// faster access
	void* pFirstObj;	// pointer to 1st object of class (^OBJ)
	_UINT nSvrChs;     	// number of server channels
	_UINT nCltChs;     	// number of client channels
	_UINT nMeths;
	_UINT nInitValues;
	PVOID pBaseCls;
	PVOID pConstructor;
	// Following entries are simply "place markers"
	// (variable size structurer from here)
	// (will define more when atrributes are set)
	//aDscChs                       : ARRAY [0..0] OF CHDSCENTRY;
	//aDscMeth                      : ARRAY [0..0] OF METHDSCENTRY;
	//aChNames                      : ARRAY [0..0] OF CHAR;
	//aClsName                      : ARRAY [0..0] OF CHAR;  // -"-
	//aMethNames                    : ARRAY [0..0] OF CHAR;  // -"-
	//aInitValues                   : ARRAY [0..0] OF INITDSCENTRY;
} CLSHDR;

// OBJDSC wird als void-Zeiger deklariert und und nicht als Struktur, weil der 
// Object-Descriptor im Lasal1 und im Lasal2 unterschiedlich ist.
// Wenn auf Strukturelemente des Object-Descriptor zugegriffen werden muß, dann 
// muß man explizit angeben ob man die Struktur OBJDSC_LSL1 oder OBJDSC_LSL2 meint !
typedef void* OBJDSC_; 

typedef struct
{
	CLSHDR* pClsHdr;	// to header for this class
	void* pNxtObj;		// Next obj for this class
	OBJDSC_* pObjDsc;	// points to descriptor
} OBJ;

typedef struct
{
	CMDMETH* pMeth;		// pointer to method table
	DINT dData;			// Server "data store"
	SVRDSC* pSvrDsc;	// Pointer to server channel descriptor
} SVRCHCMD;

typedef struct
{
	_UINT uiOff;	// offset and CRC of the chann
	UDINT udCRC;	// (02/02/01 THR: CHanged from uint to udint)
	CHMODE uiMode;	// Clt, Svr, Typed
	CLSHDR* pCls;	// Ptr to hdr for this class
} CHDSCENTRY;

typedef struct
{
	USINT nParas;			// no. parameters
	USINT nRets;
} METHDSCENTRY;

#pragma pack(pop)

#endif // _LSL_H_INCLUDED_