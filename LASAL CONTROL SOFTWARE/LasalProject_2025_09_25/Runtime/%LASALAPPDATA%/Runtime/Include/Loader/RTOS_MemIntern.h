#pragma once
#define __RTOS__MEM__INTERN__

#include "RTOS_Mem.h"

FUNCTION GLOBAL _GetRAM
VAR_OUTPUT
	ret_code   : ConfStates;
END_VAR;

// defined in RAM.st, added to every project by LASAL automatically
FUNCTION _alloc_init;

// defined in Mem.ST, used for descriptor table- allocation
FUNCTION GLOBAL _alloc_init_dsc;
FUNCTION GLOBAL _alloc_lock_dsc;
FUNCTION GLOBAL _alloc_dsc
VAR_INPUT
	udSize          : UDINT;
END_VAR
VAR_OUTPUT
	pMem            : PVOID;
END_VAR;


// defined in Mem.ST, used for retentive memory on x386
FUNCTION GLOBAL _alloc_retentive
VAR_INPUT
	udSize          : UDINT;
	udCRC			: UDINT;
	bUsed			: BOOL := TRUE;
END_VAR
VAR_OUTPUT
	pMem            : PVOID;
END_VAR;

// defined in Mem.ST, used for retentive memory on x386
FUNCTION GLOBAL _realloc_retentive
VAR_INPUT
	pOld			: pVoid;
	udSize          : UDINT;
END_VAR
VAR_OUTPUT
	pMem            : PVOID;
END_VAR;

FUNCTION GLOBAL _free_retentive
VAR_INPUT
	pMem		: ^void;
END_VAR;

FUNCTION GLOBAL _xchange_retentive
VAR_INPUT
	pMem1		: ^void;
	pMem2		: ^void;
END_VAR;

FUNCTION GLOBAL _set_retentive_used;

FUNCTION GLOBAL _take_retentive
VAR_INPUT
	pObj		: ^Obj;
	pSvrCh		: ^SvrCh;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
END_VAR;

// assign a block of memory to a new object (Obj-CRC, header-CRC)
FUNCTION GLOBAL _assign_retentive
VAR_INPUT
	pMem		: ^void;
	pObj		: ^VirtualBase;
END_VAR;

// used by the OS to iterate through the channels with retentive data
FUNCTION __CDECL GLOBAL _GetNextRAMch
VAR_INPUT
	pO			: ^Obj;
	pCh			: ^SvrCh;
END_VAR
VAR_OUTPUT
	pNext		: ^SvrCh;			// pointer to next channel
END_VAR;

#ifdef MEM_MNGR
FUNCTION MemMngrAlloc
VAR_INPUT
	udSize		: UDINT;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
END_VAR;
FUNCTION MemMngrRealloc
VAR_INPUT
	pOld		: ^void;
	udSize		: UDINT;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
END_VAR;
FUNCTION	MemMngrInit;
FUNCTION MemMngrFree
VAR_INPUT
	pMem		: ^void;
END_VAR;
#endif // MEM_MNGR

FUNCTION GLOBAL __CDECL _IsAddrValid
VAR_INPUT
	udAddr		: ^void;
END_VAR
VAR_OUTPUT
	bValid		: BOOL;
END_VAR;
