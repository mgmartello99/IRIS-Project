#pragma once
#define __RTOS__PRIV__HEADER__

#include "RTOS_PrivDefs.h"

// =========================================================================================
// Functions for accessing the RTDB 
// =========================================================================================
FUNCTION GLOBAL _GetClsList
VAR_OUTPUT
	pCls		: ^CLSHDR;
END_VAR;

FUNCTION GLOBAL _GetChName
VAR_INPUT
	pCls            : ^ClsHdr;
	uiCh            : UINT;
END_VAR
VAR_OUTPUT
	pName           : pChar;
END_VAR;

//FUNCTION GLOBAL _GetMethName
//VAR_INPUT
//	pCls            : ^ClsHdr;
//	uiMeth          : UINT;
//END_VAR
//VAR_OUTPUT
//	pName           : pChar;
//END_VAR;

FUNCTION GLOBAL _GetClsNameN
VAR_INPUT
   #ifdef _LSL_TARGETARCH_ARM
	pLng        	: ^UINT;
   #endif 
	pCls		    : ^ClsHdr;
	pClsEntry_      : pVoid;
END_VAR
VAR_OUTPUT
	ret_code        : ConfStates;
END_VAR;

FUNCTION GLOBAL _GetObjNameN
VAR_INPUT
   #ifdef _LSL_TARGETARCH_ARM
	pLng        	: ^UINT;
   #endif 
	pCls            : ^ClsHdr;
	pObj            : ^OBJ;
	pObjDsc_        : pVoid;
END_VAR
VAR_OUTPUT
	ret_code        : ConfStates;
END_VAR;

FUNCTION GLOBAL _GetClsChan
VAR_INPUT
   #ifdef _LSL_TARGETARCH_ARM
	pLng        	: ^UINT;
   #endif 
	pCls            : ^ClsHdr;
	pCh             : ^ChDsc;
	pChDsc_         : ^void;
END_VAR
VAR_OUTPUT
	ret_code        : ConfStates;
END_VAR;

//FUNCTION GLOBAL _GetMethNameN
//VAR_INPUT
//	pCls            : ^ClsHdr;
//	uiMeth          : UINT;
//	pResu           : ^MethDscEntry;
//END_VAR;

//FUNCTION GLOBAL _GetMethDsc
//VAR_INPUT
//	pCls            : ^ClsHdr;
//	uiMeth          : UINT;
//END_VAR
//VAR_OUTPUT
//	pMeth           : ^MethDscEntry;
//END_VAR;



FUNCTION GLOBAL _LookupEmbedded
VAR_INPUT
	ppScan		: ^pchar;
	pObj_ch		: ^pVoid;
	ppObj		: ^pVirtualBase;
END_VAR
VAR_OUTPUT
	chMode		: CHMODE;
END_VAR;


// Functions for Online - download
FUNCTION GLOBAL __CDECL _PrepareLink
VAR_INPUT
	udStartModule	: UDINT;
	udEndModule		: UDINT;
END_VAR
VAR_OUTPUT
	bDone			: BOOL;
END_VAR;

FUNCTION GLOBAL __CDECL _ApplyLink
VAR_OUTPUT
	bDone			: BOOL;
END_VAR;

#define LOADER_ISINITIALIZED 16#0001
#define LOADER_ISAPPLVALID   16#0002
#define LOADER_FORCE_SRAM_REORG_ON_NEXT_RUN     16#0004


type
  LSL_CLI4LASAL : STRUCT
    SetIPEx : pVoid;
    SRamLoad : pVoid;
    SRamSave : pVoid;
    Reset : pVoid;
    LoaderTraceInit : pVoid;
    LoaderTracePrint : pVoid;
  END_STRUCT;
end_type

var_external
	plslcli4lasal : ^LSL_CLI4LASAL;
	uiProgsToLoad : array[ 0..1 ] of udint;
end_var

FUNCTION GLOBAL __cdecl P_LslCLI_LoaderTraceInit;
#define LSLCLI_LoaderTraceInit()											plslcli4lasal^.LoaderTraceInit $ P_LslCLI_LoaderTraceInit()

FUNCTION GLOBAL __cdecl P_LslCLI_LoaderTracePrint
VAR_INPUT
	txt : ^char;
	col : char;
END_VAR
VAR_OUTPUT
	EAX : dint;
END_VAR;
#define LSLCLI_LoaderTracePrint(p1, p2)								plslcli4lasal^.LoaderTracePrint $ P_LslCLI_LoaderTracePrint(p1,p2)

