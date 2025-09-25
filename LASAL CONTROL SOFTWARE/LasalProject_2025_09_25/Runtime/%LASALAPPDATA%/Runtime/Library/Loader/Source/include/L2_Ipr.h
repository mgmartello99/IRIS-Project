#pragma once

#include "RTOS_MemIntern.h"
#include "RTOS_IprIntern.h"

#define SEND_STATE_ONLY (-1)$UINT
#define MIN_PROG_SIZE 4			// size of the udLng element of the SubPrgHeader

#define FROM_TABLE 16#FFFF_FFFF			// load source program from a table (default program)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions defined in the AWL-module BefIpr
////////////////////////////////////////////////////////////////////////////////////////////////////
FUNCTION GLOBAL  GetResu
VAR_INPUT
	ppRead			: ^VOID;
	pResu			: ^RESULTS;
END_VAR
VAR_OUTPUT
	new				: DINT;
END_VAR;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions defined in other modules
////////////////////////////////////////////////////////////////////////////////////////////////////
FUNCTION GLOBAL _LookupCls
VAR_INPUT
	pName		: ^char;
END_VAR
VAR_OUTPUT
	pCls		: ^ClsHdr;
END_VAR;


#ifndef DUMMY_LOADER
#pragma using MerkerEx
#pragma using RamEx
#ifdef LDR_RETSVR
#pragma using RetSvrRam
#pragma using RetSvrFile
#endif
#endif

VAR_EXTERNAL
	g_LoadTimeout 	: UDINT;
END_VAR
