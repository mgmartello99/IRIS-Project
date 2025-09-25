#pragma once
#define __RTOS__HEADER__

#define MM_BufferIn  0
#define MM_BufferOut 1


TYPE
	SvrChCmd_void	: STRUCT
		pMeth		: ^CmdMeth;
		dData		: UDINT;
		pSvrDsc		: ^void;
	END_STRUCT;
	pVirtualBase	: ^VirtualBase;
END_TYPE

// a class just to cast pointers to any other class
VirtualBase	:	CLASS

	pSvrChCmd				: SvrChCmd_void;

	FUNCTION VIRTUAL GLOBAL Read
		VAR_OUTPUT
			output(EAX)		: DINT; 	// data
		END_VAR;

	FUNCTION VIRTUAL GLOBAL Write
		VAR_INPUT
			input(EAX)		: DINT;		// data
		END_VAR
		VAR_OUTPUT
			result(EAX)		: DINT;		// result
		END_VAR;

	FUNCTION VIRTUAL GLOBAL Init;
	
	FUNCTION VIRTUAL GLOBAL CyWork
		VAR_INPUT
			EAX				: UDINT;	// data
		END_VAR
		VAR_OUTPUT
			state(EAX)		: UDINT;	// state
		END_VAR;

	FUNCTION VIRTUAL GLOBAL RtWork
		VAR_INPUT
			EAX				: UDINT;	// data
		END_VAR
		VAR_OUTPUT
			state(EAX)		: UDINT;	// state
		END_VAR;

	FUNCTION VIRTUAL GLOBAL NewInst
		VAR_INPUT
			pPara			: ^CmdStruct;	// parameter buffer
			pResult			: ^Results;		// result buffer
		END_VAR
		VAR_OUTPUT
			ret_code		: IprStates;	// state
		END_VAR;

	FUNCTION VIRTUAL GLOBAL Kill
		VAR_OUTPUT
			ret_code(EAX)	: IprStates;	// state
		END_VAR;

	FUNCTION VIRTUAL GLOBAL GetState
		VAR_INPUT
			pPara			: ^CmdStruct;	// parameter buffer
			pResult			: ^Results;		// result buffer
		END_VAR
		VAR_OUTPUT
			ret_code		: IprStates;	// state
		END_VAR;

END_CLASS;

MMServerBase : CLASS
  //Servers:
	Data 	: SvrChCmd_void;
  //Clients:
  //Variables:
    m_flStatus : BYTE;
  //Functions:

  TYPE
  	 eBuffer :  
	  (
	    BufferIn,
	    BufferOut
	  )$UDINT;
    
  END_TYPE
  
	FUNCTION VIRTUAL GLOBAL WriteDataOff
		VAR_INPUT
			udLen 	: UDINT;
			udOff 	: UDINT;
			pData 	: ^USINT;
		END_VAR;
	
	FUNCTION VIRTUAL GLOBAL GetCharSize
		VAR_INPUT
			nBufferId 	: UDINT;
		END_VAR
		VAR_OUTPUT
			usSize 	: USINT;
		END_VAR;
	
	FUNCTION VIRTUAL GLOBAL GetCRC
		VAR_OUTPUT
			udCRC 	: UDINT;
		END_VAR;
	
	FUNCTION VIRTUAL GLOBAL GetDataAt
		VAR_INPUT
			pData 	: ^USINT;
			udSize 	: UDINT;
			udAt 	: UDINT;
		END_VAR;
	
	FUNCTION VIRTUAL GLOBAL GetBufferLen
		VAR_INPUT
			nBufferId 	: UDINT;
		END_VAR
		VAR_OUTPUT
			udLen 	: UDINT;
		END_VAR;
	
	FUNCTION VIRTUAL GLOBAL ReadDataOff
		VAR_INPUT
			udOff 	: UDINT;
			pData 	: ^USINT;
			udMax 	: UDINT;
		END_VAR
		VAR_OUTPUT
			udLen 	: UDINT;
		END_VAR;
	
	FUNCTION VIRTUAL GLOBAL GetDataPtrIn
		VAR_OUTPUT
			pData 	: ^USINT;
		END_VAR;
  //Tables:
	FUNCTION @STD
		VAR_OUTPUT
			ret_code	: CONFSTATES;
		END_VAR;
	FUNCTION GLOBAL TAB @CT_;
END_CLASS;
//==================================================
//  GLOBAL Function headers
//==================================================

#define _GetClsName(pCls)	#pCls^.pDsc^.Name.Text[0]
FUNCTION GLOBAL LSL_GetObjName
VAR_INPUT
  pThis   : ^VirtualBase;
END_VAR
VAR_OUTPUT
  pName   : ^char;
END_VAR;

FUNCTION __cdecl GLOBAL _GetObjName
VAR_INPUT
	pThis			: ^VirtualBase;
	pName			: pChar;
END_VAR
VAR_OUTPUT
	udLen			: UDINT;	// length of string
END_VAR;

//==================================================
// CRC 16 calculation
//==================================================
FUNCTION GLOBAL CRC32
VAR_INPUT
	ptr             : ^char;
END_VAR
VAR_OUTPUT
	udCrc           : UDINT;
END_VAR;

FUNCTION GLOBAL CRC32_Buffer
VAR_INPUT
	ptr             : ^void;
	udCnt           : UDINT;
END_VAR
VAR_OUTPUT
	udCrc			: UDINT;
END_VAR;

FUNCTION GLOBAL LDR_CRC32_BufferEx
VAR_INPUT
  startValue      : UDINT;
	ptr             : ^void;
	udCnt           : UDINT;
END_VAR
VAR_OUTPUT
	udCrc			: UDINT;
END_VAR;

// =========================================================================================
// Functions for searching through the RTDB
// =========================================================================================
FUNCTION __CDECL GLOBAL _LookUpObj
VAR_INPUT
	pName           : pChar;
END_VAR
VAR_OUTPUT
	ret             : ^SvrCh;
END_VAR;

FUNCTION __CDECL GLOBAL _LookUpEmbed_C
VAR_INPUT
	pText           : pChar;
	pObj_ch         : ^pVoid;
	pObj            : ^pVirtualBase := NIL;
END_VAR
VAR_OUTPUT
	ChMode          : CHMODE;
END_VAR;

FUNCTION GLOBAL _LookUpChannel
VAR_INPUT
	pName			: ^CLexem;
	ppCls           : ^pClsHdr;
END_VAR
VAR_OUTPUT
	uiChNo          : UINT;
END_VAR;

FUNCTION GLOBAL _LookUpClsIDExact
VAR_INPUT
	pStart			: ^ClsHdr;
	pName			: ^CLexem;
	udRev			: Revision;
	nExact			: UDINT;
END_VAR
VAR_OUTPUT
	pCls            : ^ClsHdr;
END_VAR;

FUNCTION GLOBAL _LookUpClsIdx
VAR_INPUT
	nClsIdx	 		: UINT;
END_VAR
VAR_OUTPUT
	pCls            : ^ClsHdr;
END_VAR;

FUNCTION GLOBAL _LookUpClsIDnewest
VAR_INPUT
	pDsc			: ^ClsHdrConst;
END_VAR
VAR_OUTPUT
	pCls			: ^ClsHdr;
END_VAR;

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL _GetDesc_CRC
VAR_INPUT
	pCrc            : ^UDINT;
END_VAR
VAR_OUTPUT
  retc : UDINT;
END_VAR;
#else
FUNCTION GLOBAL _GetDesc_CRC
VAR_INPUT
	pCrc            : ^UDINT;
END_VAR;
#endif

FUNCTION GLOBAL _GetObjIdx
VAR_INPUT
	pCls            : ^ClsHdr;
	uiIdx           : UINT;
END_VAR
VAR_OUTPUT
	pLastObj        : ^OBJ;
END_VAR;

// RTKernel V3.18: Changed to __cdecl type function
FUNCTION GLOBAL _GetChFromOff
VAR_INPUT
	uioff           : UINT;
	ppCls           : ^pClsHdr;
END_VAR
VAR_OUTPUT
	uich            : UINT;
END_VAR;

FUNCTION GLOBAL _FindFctName
VAR_INPUT
	szFctName       : ^char;
END_VAR
VAR_OUTPUT
	pFct            : pFct;
END_VAR;



// =========================================================================================
// ShortCut functions to access RTDB elements
// =========================================================================================
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL LSL_GetNxtObj
VAR_INPUT
	pObjI			: ^VirtualBase;
END_VAR
VAR_OUTPUT
	retcode			: ^VirtualBase;
END_VAR;
#else
FUNCTION GLOBAL LSL_GetNxtObj		// to step through the RTDB-object-list
VAR_INPUT
	EDI			: ^VirtualBase;
END_VAR
VAR_OUTPUT
	EDI			: ^VirtualBase;
END_VAR;
#endif
// The function checks if a channel is realy connected.
// -> EBX:	in IL #Channel.pData, in ST simply #Channel
//				(which is the same)
// <- ECX:	(0)	not connected, (1) connected

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL IsClientConnected
VAR_INPUT
	input : ^void;	// pointer to the channels data element
END_VAR
VAR_OUTPUT
	output : DINT;		// 0/1 .. connected
END_VAR;
#else
FUNCTION GLOBAL IsClientConnected
VAR_INPUT
	EBX			: ^void;
END_VAR
VAR_OUTPUT
	ECX			: DINT;
END_VAR;
#endif

// check if a client is connected with a hardware-server (via advanced io element)
FUNCTION GLOBAL LDR_IsHWConnected 
VAR_INPUT
	input : ^void;	// pointer to the channels data element
END_VAR
VAR_OUTPUT
	connected : DINT;		// 0 .. not connected, > 0 .. connected
END_VAR;

// check if a client is connected with a simulation-server (via advanced io element)
FUNCTION GLOBAL LDR_IsSimConnected 
VAR_INPUT
	input : ^void;	// pointer to the channels data element
END_VAR
VAR_OUTPUT
	connected : DINT;		// 0 .. not connected, > 0 .. connected
END_VAR;

// register a advanced io element that can be checked with LDR_IsHWConnected/LDR_IsSimConnected
FUNCTION GLOBAL LDR_RegisterAdvIoElWithNewinst
VAR_INPUT
  pObj : ^VirtualBase;
  uiCmdHwConn : UINT; // newinst command to check with LDR_IsHWConnected
  uiCmdSimConn : UINT; // newinst command to check with LDR_IsSimConnected
END_VAR;

//
//	THR 090101
//	_IsKindOf: Check for Parent - Child Class Relationship
//		--> pObj: A pointer to the child object (any class casted)
//		--> pBase: A pointer to the base class header
//		<-- bResult: TRUE if pChild is derived from pBase, FALSE if not
//
//	Note: This function is C friendly!
//
FUNCTION GLOBAL __CDECL _IsKindOf
VAR_INPUT
	pChild	: ^VirtualBase;
	pBase	: ^ClsHdr;
END_VAR
VAR_OUTPUT
	bResult	: UDINT;
END_VAR;


// checks, if pBase is a base class of pCurCls
FUNCTION GLOBAL __cdecl _IsBaseCls
VAR_INPUT
	pCurCls	: pClsHdr;
	pBase	: pClsHdr;
END_VAR
VAR_OUTPUT
	bIsBase	: BOOL;
END_VAR;

// checks, if for a class there is no object created yet
FUNCTION GLOBAL __CDECL _IsFirstObj
VAR_INPUT
	pThis		: ^VirtualBase;
	pName		: ^char := NIL;
END_VAR
VAR_OUTPUT
	bFirst		: BOOL;
END_VAR;

// get next class that is derived from pBase-class
// set pFirst to NIL to start
FUNCTION GLOBAL _GetNextDerivedClass
VAR_INPUT
	pFirst	: pClsHdr;
	pBase	: pClsHdr;
END_VAR
VAR_OUTPUT
	pNext	: pClsHdr;
END_VAR;

FUNCTION GLOBAL _GetNextDerivedObj
VAR_INPUT
	pObj            : ^VirtualBase;
	pBase           : ^ClsHdr;
END_VAR
VAR_OUTPUT
	pNext			: ^VirtualBase;
END_VAR;

#define LSL_GetHdr(a) (a-sizeof(Obj))$^Obj	 // get object header from object pointer
#define LSL_GetPtr(a) (a+sizeof(Obj))$^VirtualBase // get object pointer from object header
#define LSL_GetObj(a) LSL_GetPtr(a^.pObj)    // get object ptr from object descriptor

FUNCTION GLOBAL __CDECL SetRetentiveFlag
VAR_INPUT
	pSvr		: ^SvrCh;
END_VAR
VAR_OUTPUT
	bSet		: BOOL;
END_VAR;

FUNCTION GLOBAL __CDECL SetDataBufferFlag
VAR_INPUT
	pObj		: ^VirtualBase;
END_VAR;

FUNCTION GLOBAL __CDECL NewObj
VAR_INPUT
	pClsName		: ^char;
	pObjName		: ^char :=NIL;
END_VAR
VAR_OUTPUT
	pNewObj			: ^VirtualBase;
END_VAR;

FUNCTION GLOBAL __CDECL DelThis
VAR_INPUT
	pThis			: ^VirtualBase;	
END_VAR
VAR_OUTPUT
	ret_code		: BOOL;
END_VAR;

FUNCTION GLOBAL __CDECL DelObj
VAR_INPUT
	pObjName		: ^char;
END_VAR
VAR_OUTPUT
	ret_code		: BOOL;
END_VAR;

FUNCTION GLOBAL __CDECL ConnectCltSvr
VAR_INPUT
	pCltName	: ^char;
	pSvrName	: ^char;
END_VAR
VAR_OUTPUT
	ret_code	: BOOL;
END_VAR;

#define OUTMESSAGE_MSGTYPE_INFO 1
// Space + 1, da nur User Error Ids von (' ' + 1) bis 127 möglich sind.
#define OUTMESSAGE_MSGTYPE_WARNING (' '+1)
// Space + 2, da nur User Error Ids von (' ' + 1) bis 127 möglich sind.
#define OUTMESSAGE_MSGTYPE_ERROR (' '+2)


FUNCTION GLOBAL _OutMessage
VAR_INPUT
	udLine		  : DWORD;
	pFile		    : ^CHAR;
	pText		    : ^CHAR;
  nMsgType    : CHAR := OUTMESSAGE_MSGTYPE_INFO;
END_VAR;



// =========================================================================================
// Functions to read/write to/from channels
// =========================================================================================
FUNCTION GLOBAL ASM_RD_CHNL;
FUNCTION GLOBAL ASM_WR_CHNL;
FUNCTION GLOBAL ASM_WR_VIACHNL;
FUNCTION GLOBAL M_RD;
FUNCTION GLOBAL M_WR;

FUNCTION GLOBAL RD
VAR_INPUT
	pt              : ^void;
END_VAR
VAR_OUTPUT
 	dData           : DINT;
END_VAR;

FUNCTION GLOBAL RD_CHNL
VAR_INPUT
	pt              : ^void;
	chOffs          : UINT;
END_VAR
VAR_OUTPUT
	dData           : DINT;
END_VAR;

FUNCTION GLOBAL RD_VIACHNL
VAR_INPUT
	pt              : ^void;
	chOffs          : UINT;
END_VAR
VAR_OUTPUT
	dData           : DINT;
END_VAR;

FUNCTION GLOBAL UPD_WR_CHNL
VAR_INPUT
	pt              : ^void;
	chOffs          : UINT;
END_VAR;

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL WR
VAR_INPUT
	pt              : ^void;
	dData           : DINT;
END_VAR
VAR_OUTPUT
	ret0            : UDINT;
END_VAR;
#else
FUNCTION GLOBAL WR
VAR_INPUT
	pt              : ^void;
	dData           : DINT;
END_VAR
VAR_OUTPUT
	EAX             : UDINT;
END_VAR;
#endif

FUNCTION GLOBAL RT_RD_CHNL
VAR_INPUT
	pt              : ^void;
	chOffs          : UINT;
END_VAR
VAR_OUTPUT
	dData           : DINT;
END_VAR;


#ifdef _LSL_TARGETARCH_ARM	
FUNCTION GLOBAL WR_VIACHNL
VAR_INPUT
	pt      : ^void;
	chOffs  : UINT;
	dData   : DINT;
END_VAR
VAR_OUTPUT
	ret0    : UDINT;
END_VAR;
#else
FUNCTION GLOBAL WR_VIACHNL
VAR_INPUT
	pt              : ^void;
	chOffs          : UINT;
	dData           : DINT;
END_VAR
VAR_OUTPUT
	EAX             : UDINT;
END_VAR;
#endif	


// =====================================================
// macros to call command server-methods
//
//	call Rt/Cy-Work without a parameter
#define RT_WORK(p)			p$^VirtualBase^.RtWork(EAX)
#define CY_WORK(p)  		p$^VirtualBase^.CyWork(EAX)

//	call Rt/Cy-Work with a parameter
#define RT_WORK_W(p, a)		p$^VirtualBase^.RtWork(a)
#define CY_WORK_W(p, a)  	p$^VirtualBase^.CyWork(a)

#define KILL_FUNCT(p)		p$^VirtualBase^.Kill()

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL WR_CHNL
VAR_INPUT
	pt     : ^void;
	chOffs : UINT;
	dData  : DINT;
END_VAR
VAR_OUTPUT
	ret0   : UDINT;
END_VAR;
#else
FUNCTION GLOBAL WR_CHNL
VAR_INPUT
	pt              : ^void;
	chOffs          : UINT;
	dData           : DINT;
END_VAR
VAR_OUTPUT
	EAX             : UDINT;
END_VAR;
#endif
FUNCTION GLOBAL RT_WR_CHNL
VAR_INPUT
	pt              : ^void;
	chOffs          : UINT;
	dData           : DINT;
END_VAR;


// =========================================================================================
// Functions for creating the RTDB  
// =========================================================================================
FUNCTION GLOBAL InitCmdTable
VAR_INPUT
	nCmd            : UINT;
	pCmd            : ^CMDMETH;
END_VAR;

FUNCTION GLOBAL StoreCmd
VAR_INPUT
	pCmd            : ^CMDMETH;
	mode            : CMDMETHMODE;
END_VAR
VAR_OUTPUT
	pMeth           : ^CMDMETH;
END_VAR;

FUNCTION GLOBAL StoreMethod
VAR_INPUT
	mRd             : pFct;
	mWr             : pFct;
END_VAR
VAR_OUTPUT
	pMethod         : ^CHMETH;
END_VAR;

FUNCTION GLOBAL DeriveCmdTable
VAR_INPUT
	pCmdTable	: ^CMDMETH;
	pBaseCmd	: ^CMDMETH;
	nCmds		: UINT;
END_VAR;

// Copy method table from object to buffer
FUNCTION GLOBAL GetCmdTable
VAR_INPUT
	pCmd		: ^CmdMeth;
	pThis(ESI)	: ^SvrChCmd :=ESI$^SvrChCmd;
END_VAR;

// =========================================================================================
// Default methods for the server channels
// =========================================================================================
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL M_NO_FUNCT
VAR_INPUT
	in0 (EAX): UDINT;
END_VAR
VAR_OUTPUT
	retcode (EAX): UDINT;
END_VAR;
#else
FUNCTION GLOBAL M_NO_FUNCT
VAR_INPUT
EAX : UDINT;
END_VAR
VAR_OUTPUT
EAX : UDINT;
END_VAR;
#endif
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL M_RD_CONST
VAR_OUTPUT
	retcode : DINT;
END_VAR;
#else
FUNCTION GLOBAL M_RD_CONST
VAR_OUTPUT
	EAX		: DINT;
END_VAR;
#endif
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL M_RD_DIRECT
VAR_OUTPUT
	retcode : DINT;
END_VAR;
#else		
FUNCTION GLOBAL M_RD_DIRECT
VAR_OUTPUT
	EAX		: DINT;
END_VAR;
#endif

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL M_WR_CONST
VAR_INPUT
	in0 : DINT;
END_VAR
VAR_OUTPUT
	retcode : DINT;
END_VAR;
#else
FUNCTION GLOBAL M_WR_CONST
VAR_INPUT
	EAX		: DINT;
END_VAR
VAR_OUTPUT
	EAX		: DINT;
END_VAR;
#endif

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL M_WR_DIRECT
VAR_INPUT
	in0 : DINT;
END_VAR
VAR_OUTPUT
  retcode : DINT;
END_VAR;
#else
FUNCTION GLOBAL M_WR_DIRECT
VAR_INPUT
	EAX		: DINT;
END_VAR
VAR_OUTPUT
	EAX		: DINT;
END_VAR;
#endif

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL M_WR_BDINT
VAR_INPUT
	in0 : DINT;
END_VAR
VAR_OUTPUT
	retcode : DINT;
END_VAR;
#else
FUNCTION AWL GLOBAL M_WR_BDINT
VAR_INPUT
	EAX		: DINT;
END_VAR
VAR_OUTPUT
	EAX		: DINT;
END_VAR;
#endif

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL M_NO_F
VAR_OUTPUT
	retcode : UDINT;
END_VAR;
#else
FUNCTION GLOBAL M_NO_F
VAR_OUTPUT
	EAX : UDINT;
END_VAR;
#endif
FUNCTION  GLOBAL M_NO_CMD
VAR_INPUT
	pPara           : ^VOID;
	pResu           : ^RESULTS;
END_VAR
VAR_OUTPUT
	result          : IPRSTATES;
END_VAR;

FUNCTION  GLOBAL M_NO_GETSTAT
VAR_INPUT
	pPara           : ^VOID;
	pResu           : ^RESULTS;
END_VAR
VAR_OUTPUT
	result          : IPRSTATES;
END_VAR;


FUNCTION GLOBAL      DIV_AB_BY_C
VAR_INPUT
	A               : DINT;
	B               : DINT;
	_C              : DINT;
END_VAR
VAR_OUTPUT
	RESU            : DINT;
END_VAR;

FUNCTION GLOBAL      V_TO_Q
VAR_INPUT
	V               : DINT;
	A               : DINT;
END_VAR
VAR_OUTPUT
	Q               : DINT;
END_VAR;

#ifdef _TARGETARCH_ARM
FUNCTION GLOBAL _INTPOL50
VAR_INPUT
	pTable		: ^VOID;
	input0		: UINT;
END_VAR
VAR_OUTPUT
	output0		: UINT;
END_VAR;
#else
FUNCTION GLOBAL  _INTPOL50
VAR_INPUT
	pTable		: ^VOID;	// pointer to table
	AX			: UINT;		// input value
END_VAR
VAR_OUTPUT
	AX			: UINT;		// result value
END_VAR;
#endif
#ifdef _TARGETARCH_ARM
FUNCTION GLOBAL SQRT64
VAR_INPUT
	lo32		: UDINT;
	hi32		: UDINT;
END_VAR
VAR_OUTPUT
	retcode : UDINT;
END_VAR;
#else
FUNCTION GLOBAL      SQRT64
VAR_INPUT
	EAX		: UDINT;
	EDX		: UDINT;
END_VAR
VAR_OUTPUT
	EAX		: UDINT;
END_VAR;
#endif

FUNCTION GLOBAL      OS_ISARRAYOUTOFBOUND
VAR_INPUT
	ARG1		: DINT;
	ARG2		: DINT;
	ARG3		: DINT;
	ARG4		: UDINT;
END_VAR;

// exception codes: see definitions in lsl_st_excpt.h
FUNCTION GLOBAL      OS_RAISEEXCEPTION
VAR_INPUT
	ARG1		: UDINT;
END_VAR;

FUNCTION GLOBAL 	FindCRCinMap
VAR_INPUT
	CRC		: DINT;
END_VAR
VAR_OUTPUT
	pObj		: ^Obj;
END_VAR;

// =========================================================================================
// trigger macros for clients
// =========================================================================================
#define LSL_CLT_TRG_POS(clt)  (clt = 0) & (clt.Read() <> 0)
#define LSL_CLT_TRG_NEG(clt)  (clt <> 0) & (clt.Read() = 0)
#define LSL_CLT_TRG_AND(clt)  ((clt = 0) & (clt.Read() <> 0)) | ((clt <> 0) & (clt.Read() = 0))

// =========================================================================================
// functions for floating point numbers
// =========================================================================================

// possible classes of floating point values

TYPE
  REAL_CLASS :
  (
    REAL_SignalingNaN,
    REAL_QuietNaN,
    REAL_NegativeInfinity,
    REAL_NegativeNormal,
    REAL_NegativeSubnormal,
    REAL_NegativeZero,
    REAL_PositiveZero,
    REAL_PositiveSubnormal,
    REAL_PositiveNormal,
    REAL_PositiveInfinity
  )$UDINT;
END_TYPE

// floating point functions (recommended by IEEE 754)

FUNCTION GLOBAL Real_IsSigned
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult : BOOL;
END_VAR;

FUNCTION GLOBAL Real_IsNormal
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : BOOL;
END_VAR;

FUNCTION GLOBAL Real_IsFinite
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : BOOL;
END_VAR;

FUNCTION GLOBAL Real_IsZero
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR;

FUNCTION GLOBAL Real_IsSubnormal
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR;

FUNCTION GLOBAL Real_IsInfinite
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR;

FUNCTION GLOBAL Real_IsNaN
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR;

FUNCTION GLOBAL Real_IsSignaling
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  bResult   : DINT;
END_VAR;

FUNCTION GLOBAL Real_GetClass
VAR_INPUT
  Value   : REAL;
END_VAR
VAR_OUTPUT
  Result   : REAL_CLASS;
END_VAR;

FUNCTION AWL GLOBAL __cdecl OF_Multi_S
  VAR_INPUT 
    product : ^dint; 
    mul_a : dint; 
    mul_b : dint;
  END_VAR 
  VAR_OUTPUT 
    OF_Flag : bool; 
  END_VAR;
FUNCTION AWL GLOBAL __cdecl OF_Multi_U
  VAR_INPUT 
    product : ^udint; 
    mul_a : udint; 
    mul_b : udint;
  END_VAR 
  VAR_OUTPUT 
    OF_Flag : bool; 
  END_VAR;

FUNCTION STR GLOBAL __cdecl DivMod64 
 VAR_INPUT 
    lo : udint; 
    hi : udint; 
    denominator : udint; 
  END_VAR 
  VAR_OUTPUT 
    retcode : dint; 
  END_VAR; 

FUNCTION STR GLOBAL __cdecl uDivMod64 
  VAR_INPUT 
    lo : udint; 
    hi : udint; 
    denominator : udint; 
  END_VAR 
  VAR_OUTPUT 
    retcode : udint; 
  END_VAR;

FUNCTION STR GLOBAL __cdecl __udivdi6432 
  VAR_INPUT 
    lo : udint; 
    hi : udint; 
    denominator : udint; 
  END_VAR 
  VAR_OUTPUT 
    retcode : udint; 
  END_VAR;
  
FUNCTION STR GLOBAL __cdecl __divdi6432 
  VAR_INPUT 
    lo : udint; 
    hi : udint; 
    denominator : udint; 
  END_VAR 
  VAR_OUTPUT 
    retcode : dint; 
  END_VAR;  
  
FUNCTION AWL GLOBAL __cdecl OF_Add
  VAR_INPUT 
    sum : ^dint; 
    add_a : dint; 
    add_b : dint;
  END_VAR 
  VAR_OUTPUT 
    OF_Flag : bool; 
  END_VAR;

FUNCTION AWL GLOBAL __cdecl CF_Add
  VAR_INPUT 
    sum : ^udint; 
    add_a : udint; 
    add_b : udint;
  END_VAR 
  VAR_OUTPUT 
    Flag : bool; 
  END_VAR;

FUNCTION AWL GLOBAL __cdecl OF_Sub
  VAR_INPUT 
    diff : ^dint; 
    sub_a : dint; 
    sub_b : dint;
  END_VAR 
  VAR_OUTPUT 
    OF_Flag : bool; 
  END_VAR;

FUNCTION AWL GLOBAL __cdecl CF_Sub
  VAR_INPUT 
    diff : ^udint; 
    sub_a : udint; 
    sub_b : udint;
  END_VAR 
  VAR_OUTPUT 
    Flag : bool; 
  END_VAR;