//Loader.h

#pragma once

#include "RTOS_Globals.h"
#include "RTOS_IprIntern.h"
#include "Multimaster.h"

#if 0
		////////////////////////////////////////////////////////////////////////////
		// auskommentiert, da nur neue Schnittstellen hier definiert werden
		// (wegen Kompatibilität zu bestehenden Headerfiles (comlink.h, ...)
		////////////////////////////////////////////////////////////////////////////



		////////////////////////////////////////////////////////////////////////////
		// Main
		////////////////////////////////////////////////////////////////////////////
		FUNCTION main
		VAR_INPUT
			inp_code : CONFSTATES;
		END_VAR;
		
		////////////////////////////////////////////////////////////////////////////
		// Comlink
		////////////////////////////////////////////////////////////////////////////
		TYPE
		#pragma pack (push,1)
			comlinkReason : UINT;
		  
		  Comdef : STRUCT
			Interface: UDINT;
			Adress   : UINT;
			pt_COM   : pVoid;
		  END_STRUCT; 
		  
		  d2LSE : STRUCT
			VarlistID : UDINT;
			uiOffs    : UINT;
			Data      : DINT;
		  END_STRUCT; 
		
		  LslCommRegData :struct
			LASALID   :UDINT;
			channel	  :UINT;
			VarPos	  :UINT;   
			uiTIME    :UINT; 
			VarlistID :UDINT;	// from LSE a number give back at callback (not interpreted)
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
		  
		  
		#pragma pack (pop)
		END_TYPE
		
		FUNCTION GLOBAL __CDECL LOGIN
		VAR_INPUT
			pComdef  :^COMDEF; 
		END_VAR
		VAR_OUTPUT
			result   :UINT;
		END_VAR;

		FUNCTION GLOBAL __CDECL LOGOUT
		VAR_INPUT
			pComdef  :^COMDEF; 
		END_VAR
		VAR_OUTPUT
			result   :UINT;
		END_VAR;

		FUNCTION GLOBAL __CDECL TXCOMMAND
		VAR_INPUT
			Command  :  UDINT;
			length   :  UDINT;
			charptr  : ^USINT;
			pComdef  : ^COMDEF;
			Presu	 : ^UDINT;	
		END_VAR
		VAR_OUTPUT
			Status   : IPRSTATES;
		END_VAR;
		
		FUNCTION GLOBAL __CDECL TXCOMMANDEX
		VAR_INPUT
			Command  	:  UDINT;  	// command IPRStates
			length   	:  UDINT;  	// length of command 
			charptr  	: ^USINT;  	// pointer on the command line (must be copied) 
			pComdef  	: ^COMDEF; 
			pResuBuf	: ^RESULTS; // result buffer
			sizeResuBuf	: UDINT;	// size of result buffer
			pReason		: ^comlinkReason;	// reason-code in the case of an error
		END_VAR
		VAR_OUTPUT
			Status   : IPRSTATES;
		END_VAR;
		
		FUNCTION GLOBAL __CDECL TXUPD
		VAR_INPUT
			pLslcommregdata : ^Lslcommregdata;
			pComdef  : ^COMDEF;
		END_VAR
		VAR_OUTPUT
			retval    : comlinkReason;
		END_VAR;
		
		FUNCTION GLOBAL __CDECL PrototypeCallback
		VAR_INPUT
			pComdef  : ^COMDEF;
			pData    : ^d2LSE;
		END_VAR;
		
		FUNCTION GLOBAL __CDECL StartStopRefresh 
		VAR_INPUT
			pComdef : ^COMDEF;
			count   : UINT;
			typ	  	: UINT; 	
		END_VAR;
		
		FUNCTION GLOBAL __CDECL INSTALLCALLBACK
		VAR_INPUT
			PCallback : ^void;
		END_VAR;
		
		////////////////////////////////////////////////////////////////////////////
		// Set_up
		////////////////////////////////////////////////////////////////////////////
		FUNCTION AWL GLOBAL InitCmdTable
		VAR_INPUT
			nCmd			: UINT;
			pCmd			: ^CMDMETH;
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
			mRd			: pFct;
			mWr			: pFct;
		END_VAR
		VAR_OUTPUT
			pMethod		: ^CHMETH;
		END_VAR;
		
		FUNCTION GLOBAL _OutMessage
		VAR_INPUT
			udLine		: DWORD;
			pFile		: ^CHAR;
			pText		: ^CHAR;
		END_VAR;
		
		////////////////////////////////////////////////////////////////////////////
		// String
		////////////////////////////////////////////////////////////////////////////
		(*
		  ist schon in lsl_string.h definiert
		FUNCTION GLOBAL _strcpy
		VAR_INPUT
		 dest            : ^CHAR;
		 src             : ^CHAR;
		END_VAR;
		
		FUNCTION GLOBAL _strcat
		VAR_INPUT
		 dest            : ^CHAR;
		 src             : ^CHAR;
		END_VAR;
		
		FUNCTION GLOBAL _memmove
		VAR_INPUT
			dest            : ^void;
			src             : ^void;
			anz             : UDINT;
		END_VAR;
		
		FUNCTION GLOBAL _strupr
		VAR_INPUT
			str	: ^Char;
		END_VAR;
		
		FUNCTION GLOBAL _memicmp
		VAR_INPUT
			str1            : ^void;
			str2            : ^void;
			len             : UDINT;
		END_VAR
		VAR_OUTPUT
			result          : UDINT;
		END_VAR;
		
		FUNCTION GLOBAL _strcmp
		VAR_INPUT
			str1            : ^char;
			str2            : ^char;
		END_VAR
		VAR_OUTPUT
			retcode         : UINT;
		END_VAR;
		
		FUNCTION GLOBAL _stricmp
		VAR_INPUT
			str1            : ^char;
			str2            : ^char;
		END_VAR
		VAR_OUTPUT
			retcode         : UDINT;
		END_VAR;
		
		FUNCTION GLOBAL _strchr
		VAR_INPUT
			src             : ^char;
			chr             : CHAR;
		END_VAR
		VAR_OUTPUT
			dest            : ^CHAR;
		END_VAR;
		
		FUNCTION GLOBAL _strlen
		VAR_INPUT
			src             : ^CHAR;
		END_VAR
		VAR_OUTPUT
			retcode         : UDINT;
		END_VAR;
		
		FUNCTION GLOBAL _memcmp
		VAR_INPUT
			ptr1			: ^void;
			ptr2			: ^void;
			cntr			: UDINT;
		END_VAR
		VAR_OUTPUT
			notequal		: UDINT;
		END_VAR;
		
		FUNCTION GLOBAL _strncpy
		VAR_INPUT
			dest            : ^CHAR;
			src             : ^CHAR;
			max             : UDINT;
		END_VAR;
		
		FUNCTION GLOBAL _memcpy
		VAR_INPUT
			ptr1			: ^void;
			ptr2			: ^void;
			cntr			: UDINT;
		END_VAR;
		
		FUNCTION GLOBAL _memset
		VAR_INPUT
			dest			: ^void;
			usByte			: USINT;
			cntr			: UDINT;
		END_VAR;
		
		FUNCTION GLOBAL _itoa
		VAR_INPUT
			udNumber		: UDINT;
			pString			: ^CHAR;
		END_VAR;
		
		FUNCTION GLOBAL _Format
		VAR_INPUT
		  pFormat   : ^char;
		  pTxt0     : ^char;
		  pTxt1     : ^char;
		  pTxt2     : ^char;
		END_VAR
		VAR_OUTPUT
		  pText     : ^char;
		END_VAR;
		*)
		
		////////////////////////////////////////////////////////////////////////////
		// OsiBaseNew
		////////////////////////////////////////////////////////////////////////////
		(*
		FUNCTION GLOBAL InitOsiM_new
		VAR_INPUT
			pMeths		: ^void;
			nMeths		: UINT;
		END_VAR
		VAR_OUTPUT
			ret_code	: ConfStates;
		END_VAR;
		*)
		
		////////////////////////////////////////////////////////////////////////////
		// StdMeths
		////////////////////////////////////////////////////////////////////////////
		FUNCTION AWL  GLOBAL M_NO_F
		VAR_OUTPUT
			EAX		: UDINT;
		END_VAR;

#ifdef _LSL_TARGETARCH_ARM
		FUNCTION GLOBAL M_RD_DIRECT
		VAR_OUTPUT
			retcode : DINT;
		END_VAR;
#else		
		FUNCTION AWL GLOBAL      M_RD_DIRECT
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
		FUNCTION AWL GLOBAL      M_WR_DIRECT
		VAR_INPUT
			EAX		: DINT;
		END_VAR
		VAR_OUTPUT
			EAX		: DINT;
		END_VAR;
#endif		
		////////////////////////////////////////////////////////////////////////////
		// MethLib
		////////////////////////////////////////////////////////////////////////////
		FUNCTION RD
		VAR_INPUT
			pt              : ^void;
		END_VAR
		VAR_OUTPUT
			dData           : DINT;
		END_VAR;
		
		FUNCTION ASM_RD_CHNL;
		
		FUNCTION ASM_WR_CHNL;
		
		//FUNCTION ASM_WR_VIACHNL;
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
	   
#ifdef _LSL_TARGETARCH_ARM
		FUNCTION GLOBAL IsClientConnected
		VAR_INPUT
			input : ^void;	// pointer to the channels data element
		END_VAR
		VAR_OUTPUT
			output : DINT;		// 0/1 .. connected
		END_VAR;
#else
	   
		FUNCTION IsClientConnected
		VAR_INPUT
			EBX			: ^void;	// pointer to the channels data element
		END_VAR
		VAR_OUTPUT
			ECX			: DINT;		// 0/1 .. connected
		END_VAR;
#endif		
		////////////////////////////////////////////////////////////////////////////
		// Rtdb
		////////////////////////////////////////////////////////////////////////////
		FUNCTION GLOBAL __cdecl NewObj
		VAR_INPUT
		  pClsName    : ^char;
		  pObjName    : ^char (* := NIL *);
		END_VAR
		VAR_OUTPUT
		  pNewObj     : ^VirtualBase;
		END_VAR;
		
		FUNCTION GLOBAL __CDECL _IsFirstObj
		VAR_INPUT
			pThis		: ^VirtualBase;
			pName		: ^char;
		END_VAR
		VAR_OUTPUT
			bFirst		: BOOL;
		END_VAR;
		
		FUNCTION GLOBAL __CDECL SetDataBufferFlag
		VAR_INPUT
			pObj		: ^VirtualBase;
		END_VAR;
		
		////////////////////////////////////////////////////////////////////////////
		// Lib
		////////////////////////////////////////////////////////////////////////////
		FUNCTION STR GLOBAL CRC32_Buffer
		VAR_INPUT
			ptr             : ^void;
			udCnt           : UDINT;
		END_VAR
		VAR_OUTPUT
			udCrc			: UDINT;
		END_VAR;
		
		FUNCTION AWL GLOBAL      CRC16_BUFFER
		VAR_INPUT
		 ptr             : ^CHAR;
		 count           : UINT;
		END_VAR
		VAR_OUTPUT
		 crc_16          : UINT;
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
		FUNCTION GLOBAL  AWL _INTPOL50
		VAR_INPUT
			pTable		: ^VOID;
			AX			: UINT;
		END_VAR
		VAR_OUTPUT
			AX			: UINT;
		END_VAR;
       #endif		
		////////////////////////////////////////////////////////////////////////////
		// Lookup
		////////////////////////////////////////////////////////////////////////////
		FUNCTION GLOBAL __CDECL _LookUpEmbed_C
		VAR_INPUT
			pText           : PCHAR;
			pObj_ch			: ^PVOID;
			pObj			: ^pVirtualBase;
		END_VAR
		VAR_OUTPUT
			chMode          : CHMODE;
		END_VAR;
		
		FUNCTION GLOBAL _LookupCls
		VAR_INPUT
			pName           : ^CHAR;
		END_VAR
		VAR_OUTPUT
			pCls            : ^ClsHdr;
		END_VAR;
		
		////////////////////////////////////////////////////////////////////////////
		// BefiprAWL
		////////////////////////////////////////////////////////////////////////////
		(*
		FUNCTION STR GLOBAL GetResu
		VAR_INPUT
			ppRead	: ^VOID;
			pResu	: ^RESULTS;
		END_VAR
		VAR_OUTPUT
			new		: DINT;
		END_VAR;
		*)
		
		////////////////////////////////////////////////////////////////////////////
		// Mem
		////////////////////////////////////////////////////////////////////////////
		FUNCTION GLOBAL AWL GetVersion_sRam
		VAR_OUTPUT
			Version		: UDINT;
		END_VAR;
		
		FUNCTION GLOBAL AWL Find_sRamFile
		VAR_INPUT
			CRC32		:  UDINT;
			ppRamAdr	: ^UDINT;
			ppFileAdr	: ^XFileCell;
			uiType		: UDINT;				// 0=RamZelle 1=RamExZelle 	
		END_VAR
		VAR_OUTPUT
			state		: UDINT;
		END_VAR;
		
		FUNCTION GLOBAL AWL Find_sRam
		VAR_INPUT
			CRC32		: UDINT;
		END_VAR
		VAR_OUTPUT
			pRamAdr		: ^XramCell;
		END_VAR;
		
		FUNCTION AWL GLOBAL SRAM_GET_ALLOCATED
		VAR_INPUT
		 DataSize   :UDINT;
		END_VAR
		VAR_OUTPUT
		 AlocSize   :UDINT; 
		END_VAR;
		
		FUNCTION GLOBAL   GET_USED_S_RAM;
		
		FUNCTION GLOBAL SRAM_GetMemory
		VAR_INPUT
		   length0  : UDINT;
		END_VAR
		VAR_OUTPUT
		   ret0 : ^USINT;
		END_VAR;
		
		FUNCTION GLOBAL AWL MakeRamExChk
		VAR_INPUT
		  pXramCell : ^XramCell;
		END_VAR;
		
		FUNCTION GLOBAL _alloc_resize
		VAR_INPUT
			udSize		: UDINT;
		END_VAR
		VAR_OUTPUT
			pMem		: ^void;
		END_VAR;
		
		FUNCTION GLOBAL _alloc_resize_LDR
		VAR_INPUT
			udSize		: UDINT;
		END_VAR
		VAR_OUTPUT
			pMem		: ^void;
		END_VAR;
		
		FUNCTION GLOBAL _Free
		VAR_INPUT
			pMem            : pVOID;
		END_VAR;
		
		FUNCTION GLOBAL _realloc
		VAR_INPUT
			pOld			: ^void;
			udSize          : UDINT;
		END_VAR
		VAR_OUTPUT
			pMem            : ^void;
		END_VAR;
		
		FUNCTION GLOBAL _realloc_LDR
		VAR_INPUT
			pOld			: ^void;
			udSize          : UDINT;
		END_VAR
		VAR_OUTPUT
			pMem            : ^void;
		END_VAR;

#endif

FUNCTION GLOBAL ProgMgr_GetProgram
VAR_INPUT
	nProgNo         : UDINT;
END_VAR
VAR_OUTPUT
	pp              : ^SubPrgHead;
END_VAR;

FUNCTION GLOBAL ProgMgr_GetProgramPtr
VAR_INPUT
	nProgNo			: UDINT;
END_VAR
VAR_OUTPUT
	pp				: ^pNewSubPrgHead;
END_VAR;

FUNCTION GLOBAL IprMgr_GetInterpreter
VAR_INPUT
  nProgNo 	: UDINT;
END_VAR
VAR_OUTPUT
  pp 	: pVoid;
END_VAR;

FUNCTION GLOBAL IprMgr_KillProgs;

FUNCTION GLOBAL IprMgr_StartLabel
VAR_INPUT
  nProgNo 	: UDINT;
  pName 	: ^CHAR;
END_VAR
VAR_OUTPUT
  bFound 	: BOOL;
END_VAR;
  
FUNCTION GLOBAL IprMgr_SetMaxProgSeqDurationAll_ms
VAR_INPUT
  maxProgSeqDuration_ms : UDINT;
END_VAR;

FUNCTION GLOBAL IprMgr_SetProgSeqCycleTime_ms
VAR_INPUT
  progSeqCycleTime_ms : UDINT;
END_VAR;
  
FUNCTION GLOBAL IprMgr_SetMaxNbrIprCmds
VAR_INPUT
  maxNbrIprCmds : UDINT;
END_VAR;

FUNCTION GLOBAL ClassReader_NewCls
VAR_INPUT
  pName 	: ^CHAR;
END_VAR
VAR_OUTPUT
  pCls 	: ^ClsHdr;
END_VAR;

ProgIp : CLASS;
TYPE
#pragma pack(push, 1)
  StackInfo : STRUCT
    pCall : ^instruct;
    uiLineNr : UINT;
    uiCalledFrom : UINT;
    uiCalledIpr : UINT;
  END_STRUCT;
#pragma pack(pop)
END_TYPE

FUNCTION GLOBAL ProgIp_GetHead
VAR_INPUT
  pp 	: ^ProgIp;
END_VAR
VAR_OUTPUT
  pHead 	: ^iprHead;
END_VAR;

FUNCTION GLOBAL ProgIp_GetActLine
VAR_INPUT
  pp 	: ^ProgIp;
END_VAR
VAR_OUTPUT
  uiActLine	: UINT;
END_VAR;

FUNCTION GLOBAL ProgIp_DeleteBreakpoint
VAR_INPUT
  pp 	: ^ProgIp;
  dOffset 	: UDINT;
END_VAR
VAR_OUTPUT
  bBpValid 	: BOOL;
END_VAR;

FUNCTION GLOBAL ProgIp_StartProgram
VAR_INPUT
  pp 	: ^ProgIp;
  uiPrgNo 	: UINT;
  uiLabelNo 	: UINT;
END_VAR
VAR_OUTPUT
  state 	: iprStates;
END_VAR;
	
FUNCTION GLOBAL ProgIp_StartProgramOfs
VAR_INPUT
  pp 	: ^ProgIp;
  uiPrgNo 	: UINT;
  offset : UDINT;
END_VAR
VAR_OUTPUT
  state 	: iprStates;
END_VAR;
	
FUNCTION GLOBAL ProgIp_StopProgram
VAR_INPUT
  pp 	: ^ProgIp;
  uiProgNo 	: UINT;
END_VAR
VAR_OUTPUT
  state 	: iprStates;
END_VAR;
	
FUNCTION GLOBAL ProgIp_RunProgram
VAR_INPUT
  pp 	: ^ProgIp;
  uiProgNo 	: UINT;
  uiLabelNo 	: UINT;
END_VAR
VAR_OUTPUT
  state 	: iprStates;
END_VAR;

FUNCTION GLOBAL ProgIp_SetBreakpoint
VAR_INPUT
  pp 	: ^ProgIp;
  dOffset 	: UDINT;
END_VAR
VAR_OUTPUT
  bBpValid 	: BOOL;
END_VAR;

FUNCTION GLOBAL ProgIp_MakeSingleStep
VAR_INPUT
  pp 	: ^ProgIp;
END_VAR;
  
FUNCTION GLOBAL ProgIp_SetCodePointer
VAR_INPUT
  pp 	: ^ProgIp;
  udCodeOffset 	: UDINT;
END_VAR;

FUNCTION GLOBAL ProgIp_InitVariables
VAR_INPUT
  pp 	: ^ProgIp;
END_VAR;
  
FUNCTION GLOBAL ProgIp_SetPrgHead
VAR_INPUT
  pp 	: ^ProgIp;
  pPrgHead 	: ^NewSubPrgHead;
END_VAR;

FUNCTION GLOBAL ProgIp_SetNextInstructionLabelNo
VAR_INPUT
  pp 	: ^ProgIp;
  nLabel 	: UINT;
END_VAR
VAR_OUTPUT
  ret_code 	: UINT;
END_VAR;

FUNCTION GLOBAL ProgIp_SetNextInstruction
VAR_INPUT
  pp 	: ^ProgIp;
END_VAR;

FUNCTION GLOBAL ProgIp_SetInstructionToProgStart
VAR_INPUT
  pp 	: ^ProgIp;
	nLabel		: UINT;
END_VAR
VAR_OUTPUT
	ret_code	: IPRSTATES;
END_VAR;

FUNCTION GLOBAL ProgIp_SetMaxProgSeqDuration_ms
VAR_INPUT
  pp 	: ^ProgIp;
  maxProgSeqDuration_ms : UDINT;
END_VAR;

FUNCTION GLOBAL ProgIp_SetMaxNbrIprCmds
VAR_INPUT
  pp 	: ^ProgIp;
  maxNbrIprCmds : UDINT;
END_VAR;

#define PROGMGR_FLAGS_ALLOCFIX  2
FUNCTION GLOBAL ProgMgr_CreateIprProg
VAR_INPUT
	flags : UDINT;     
	preAllocSize : UDINT; 
END_VAR
VAR_OUTPUT
	prgNbr : UINT;        
END_VAR;

FUNCTION GLOBAL ProgMgr_LoadIprProg
VAR_INPUT
  prgNbr 	: UINT;
  pProg 	: ^SubPrgHead;
  prgSize 	: UDINT;
  bCheck		: BOOL := TRUE;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

FUNCTION GLOBAL ProgMgr_LoadIprProg2
VAR_INPUT
  prgNbr 	: UINT;
  pProg 	: ^SubPrgHead;
  prgSize 	: UDINT;
  bCheck		: BOOL := TRUE;
  bDoNotLink : BOOL := FALSE;
  bBTnoRun	: BOOL := FALSE;
END_VAR
VAR_OUTPUT
  retVal : DINT;
END_VAR;

// eventType of data in function IprCallback
#define IPRCB_IPCHANGED 0   
#define IPRCB_ERROR     1   

// ProgIp error codes
#define PROGIP_NO_ERROR             0
#define PROGIP_ERR_GENERAL          1
#define PROGIP_ERR_ENDOFCODE        2
#define PROGIP_ERR_STACKOVERFLOW    3
#define PROGIP_ERR_LABELINVALID     4
#define PROGIP_ERR_SYNTAX           5
#define PROGIP_ERR_ICMDFAILED       6
#define PROGIP_ERR_RUNPROGFAILED    7
#define PROGIP_ERR_PROGSTOPPED      8

TYPE
  LDR_IpChanged_Data : STRUCT
    sizeOfStruct : UDINT;
    ofs : UDINT;
    prgNbr : UINT;
    pp 	: ^ProgIp;
  END_STRUCT;
  LDR_Error_Data : STRUCT
    sizeOfStruct : UDINT;
    errorCode : DINT;
    prgNbr : UINT;
    pp 	: ^ProgIp;
  END_STRUCT;
END_TYPE

FUNCTION GLOBAL IprCallback
VAR_INPUT
  eventType : UDINT;
  data : pVoid;
END_VAR;
    
FUNCTION GLOBAL ProgIp_InstallIprCallback
VAR_INPUT
  pThis : pVoid;
  pFunc : pVoid;
END_VAR;

FUNCTION GLOBAL ProgIp_GetProgsToLoad
VAR_INPUT
	pBitmask 	: ^pVoid;
END_VAR
VAR_OUTPUT
	usLen 	: USINT;
END_VAR;

FUNCTION GLOBAL ProgIp_GetSubStack
VAR_INPUT
  pp 	: ^ProgIp;
  pNoOfSubLevels : ^UINT;
END_VAR
VAR_OUTPUT
  pSubStack 	: ^StackInfo;
END_VAR;

FUNCTION GLOBAL LDR_InstallPostInitCB
VAR_INPUT
  pThis : ^VirtualBase;
  pCmdSvr : ^SVRCHCMD;
  newInstCmd : UINT;
END_VAR;

FUNCTION GLOBAL LDR_InstallPostInitCB_Add
VAR_INPUT
  pThis : ^VirtualBase;
  pCmdSvr : ^SVRCHCMD;
  newInstCmd : UINT;
END_VAR;

FUNCTION GLOBAL LDR_GetPrjInfo
VAR_INPUT
  pRevHi : ^BYTE;
  pRevLo : ^BYTE;
  ppName : ^PCHAR;
END_VAR
VAR_OUTPUT
  result : BOOL;
END_VAR;

////////////////////////////////////////////////////////////////////////////
// Multimaster Loaderfunctions
////////////////////////////////////////////////////////////////////////////
FUNCTION GLOBAL LDR_MM_StatNameGetNr
VAR_INPUT
  pszName : ^CHAR;
END_VAR
VAR_OUTPUT
  udStationId : UDINT;
END_VAR;
FUNCTION GLOBAL LDR_MM_StatNrGetName
VAR_INPUT
  udStationId : UDINT;
  pszName : ^CHAR;
END_VAR
VAR_OUTPUT
  status : DINT;
END_VAR;
FUNCTION GLOBAL LDR_MM_StatNrConnStop
VAR_INPUT
  udStationId : UDINT;
END_VAR
VAR_OUTPUT
  status    : DINT;
END_VAR;
FUNCTION GLOBAL LDR_MM_StatNrConnStart
VAR_INPUT
  udStationId : UDINT;
END_VAR
VAR_OUTPUT
  status    : DINT;
END_VAR;
FUNCTION GLOBAL LDR_MM_StatNrConnSetStr
VAR_INPUT
  udStationId : UDINT;
  pszConn       : ^CHAR;
END_VAR
VAR_OUTPUT
  status    : DINT;
END_VAR;
FUNCTION GLOBAL LDR_MM_StatNrConnGetStr 
VAR_INPUT
  udStationId : UDINT;
  pszConn     : ^CHAR;
END_VAR
VAR_OUTPUT
  status    : DINT;
END_VAR;
FUNCTION GLOBAL LDR_MM_StatNrConnConfigStr
VAR_INPUT
  udStationId : UDINT;
  pszConn : ^CHAR;
END_VAR
VAR_OUTPUT
  status    : DINT; // 0=ok
END_VAR;
FUNCTION GLOBAL LDR_MM_StatNrIsConnected 
VAR_INPUT
  udStationId : UDINT;
END_VAR
VAR_OUTPUT
  status    : DINT;
END_VAR;
FUNCTION GLOBAL LDR_MM_AreAllReqInitialised 
VAR_INPUT
  udStationId : UDINT;
END_VAR
VAR_OUTPUT
  status    : BOOL;
END_VAR;
FUNCTION GLOBAL LDR_MM_SvrIsConnected 
VAR_INPUT
  udStationId : UDINT; 
  pszSvrName  : ^CHAR; 
END_VAR
VAR_OUTPUT
  status    : DINT; 
END_VAR;
FUNCTION GLOBAL LDR_MM_ReadSvr
VAR_INPUT
  udStationId : UDINT; 
  pszSvrName  : ^CHAR;
  pBuffer     : ^UDINT;
END_VAR
VAR_OUTPUT
  status    : DINT; 
END_VAR; 
FUNCTION GLOBAL LDR_MM_SvrGetHandle 
VAR_INPUT
  udStationId : UDINT; 
  pszSvrName  : ^CHAR; 
END_VAR
VAR_OUTPUT
  udSvrHandle : UDINT; 
END_VAR; 
FUNCTION GLOBAL LDR_MM_SvrGetHandleFromCmpName 
VAR_INPUT
  pszCmpName  : ^CHAR; 
END_VAR
VAR_OUTPUT
  udSvrHandle : UDINT; 
END_VAR;
FUNCTION GLOBAL LDR_MM_SvrGetNameFromHandle 
VAR_INPUT
  udSvrHandle : UDINT;
  udStationId : ^UDINT; 
  pszSvrName  : ^CHAR; 
END_VAR
VAR_OUTPUT 
  bFound      : BOOL;
END_VAR;
FUNCTION GLOBAL LDR_MM_ReadSvrByHandle 
VAR_INPUT
  udSvrHandle : UDINT; 
  pBuffer     : ^UDINT;
END_VAR
VAR_OUTPUT
  status    : DINT; 
END_VAR; 
FUNCTION GLOBAL LDR_MM_SvrNewInst 
VAR_INPUT
  udSvrHandle : UDINT;
  pPara 	    : ^CmdStruct;
  pResult 	  : ^Results;
END_VAR
VAR_OUTPUT
  ret_code 	: IprStates;
END_VAR;
FUNCTION GLOBAL LDR_MM_SvrGetState 
VAR_INPUT
  udSvrHandle : UDINT;
  pPara 	    : ^CmdStruct;
  pResult 	  : ^Results;
END_VAR
VAR_OUTPUT
  ret_code 	: IprStates;
END_VAR;
FUNCTION GLOBAL LDR_MM_UserCallbackPrototyp
VAR_INPUT
  pThis           : ^void;
  udStationId     : UDINT;
  szStationName   : ^CHAR;
  udServerHandle	: UDINT;
  szServerName  	: ^CHAR;
  udState 	      : _MMState;
END_VAR;
FUNCTION GLOBAL LDR_MM_InstallCallback
VAR_INPUT
  pCallback  : ^void;
  pThis      : ^void;
END_VAR;
FUNCTION GLOBAL LDR_MM_SetWaitForReqTimeout 
VAR_INPUT
  udStationId : UDINT;
  waitForReqTimeout_ms : UDINT;
END_VAR
VAR_OUTPUT
  status    : DINT;
END_VAR;
FUNCTION GLOBAL LDR_CallProgsequencesInCyclic
VAR_INPUT
  udPhase	: UDINT;
END_VAR;

#define LDR_DO_NOT_USE_RAMEX_FOR_IPRPROG    1
FUNCTION GLOBAL LDR_SetConfigFlags
VAR_INPUT
  flags : UDINT;
END_VAR
VAR_OUTPUT
  actFlags : UDINT;
END_VAR;