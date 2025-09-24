#pragma once
#define __RTOS__MEM__

#include <lsl_st_ifssr.h>
#include <lsl_st_kernel.h>
#include <rtos_interfaces.h> // we need this for the SysCIL functions


// =========================================================================================
// Functions for memory allocation                     
// =========================================================================================
FUNCTION GLOBAL _alloc_resize
VAR_INPUT
	udSize          : UDINT;
END_VAR
VAR_OUTPUT
	pMem            : ^VOID;
END_VAR;

FUNCTION GLOBAL _alloc_resize_LDR
VAR_INPUT
	udSize          : UDINT;
END_VAR
VAR_OUTPUT
	pMem            : ^VOID;
END_VAR;

FUNCTION GLOBAL LDR_alloc_descr
VAR_INPUT
	udSize		: UDINT;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
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

FUNCTION GLOBAL _free
VAR_INPUT
	pMem            : ^VOID;
END_VAR;

// falls jemand _alloc aufruft -> wird daraus _alloc_align darufhin
// wird daraus _alloc_resize 

// -- daher kann man gleich mit _alloc_resize(xx) aufrufen um das verfahren abzukürzen

#define _alloc(a)	            _alloc_align(a)
#define _alloc_align(udSize)	_alloc_resize_LDR(udSize)

FUNCTION GLOBAL __cdecl P_SSR_OSMalloc
VAR_INPUT
  heapId  : UDINT;	// 0=C-Heap, 1=UserData-Sysheap
  size0   : UDINT;
END_VAR
VAR_OUTPUT 
  ret0    : PVOID;
END_VAR;

FUNCTION GLOBAL __cdecl P_SSR_OSFree
VAR_INPUT
  heapId  : UDINT;	// 0=C-Heap, 1=UserData-Sysheap
  ptr0     : PVOID;
END_VAR;

FUNCTION GLOBAL __cdecl P_SSR_OSRealloc
VAR_INPUT
  heapId  : UDINT;	// 0=C-Heap, 1=UserData-Sysheap
  ptr0     : PVOID;
  size0    : UDINT;
END_VAR
VAR_OUTPUT 
  ret0    : PVOID;
END_VAR;

#define OS_SysMalloc(p1) _LSL_POS^.piSSR^.Malloc $ P_SSR_OSMalloc(1,p1)
#define OS_SysFree(p1) _LSL_POS^.piSSR^.Free $ P_SSR_OSFree(1,p1)
#define OS_SysRealloc(p1,p2) _LSL_POS^.piSSR^.Realloc $ P_SSR_OSRealloc(1,p1,p2)

FUNCTION GLOBAL LDR_ForceSramReorgOnNextRun;
FUNCTION GLOBAL LDR_IsAsnycFileOperationInProgress
VAR_OUTPUT
  ret   : BOOL;
END_VAR;
