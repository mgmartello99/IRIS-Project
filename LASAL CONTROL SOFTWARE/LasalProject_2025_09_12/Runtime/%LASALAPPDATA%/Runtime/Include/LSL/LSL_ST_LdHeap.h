#pragma once
#define __LSL__LOADER__HEAP__HEADER__


#include <lsl_st_ifssr.h>
#include <lsl_st_kernel.h>
#include <rtos_interfaces.h> // we need this for the SysCIL functions

FUNCTION GLOBAL __cdecl P_LoaderHeapAlloc
VAR_INPUT
  size0   : UDINT;
END_VAR
VAR_OUTPUT 
  ret0    : PVOID;
END_VAR;

FUNCTION GLOBAL __cdecl P_LoaderHeapReAlloc
VAR_INPUT
	space0	 : pvoid;
	newsize0 : UDINT;
END_VAR
VAR_OUTPUT 
	ret0  : pvoid;
END_VAR;

FUNCTION GLOBAL __cdecl P_LoaderHeapFree
VAR_INPUT
    ptr0     : PVOID;
END_VAR
VAR_OUTPUT 
	ret0  : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LoaderHeapMarkAlloc
VAR_INPUT
	space0	 : pvoid;
    size0   : UDINT;
END_VAR
VAR_OUTPUT 
    ret0    : PVOID;
END_VAR;

FUNCTION GLOBAL __cdecl P_LoaderHeap_Backup
VAR_INPUT
  ptr0     : PVOID;
END_VAR
VAR_OUTPUT 
	ret0  : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LoaderHeap_Info
VAR_INPUT
	pAddr	: ^UDINT;
	pSize	: ^UDINT;
END_VAR
VAR_OUTPUT
	bOk		: DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LoaderHeap_CheckList;

FUNCTION GLOBAL __cdecl P_CheckHeapList;

#define OS_LDHEAP_ALLOC(p1)      	pLdheap^.LoaderHeapAlloc $ P_LoaderHeapAlloc(p1)
#define OS_LDHEAP_REALLOC(p1,p2) 	pLdheap^.LoaderHeapReAlloc $ P_LoaderHeapReAlloc(p1,p2)
#define OS_LDHEAP_FREE(p1)        	pLdheap^.LoaderHeapFree $ P_LoaderHeapFree(p1)
#define OS_LDHEAP_MARKALLOC(p1,p2) 	pLdheap^.LoaderHeapMarkAlloc $ P_LoaderHeapMarkAlloc(p1,p2)
#define OS_LDHEAP_BACKUP(p1)        pLdheap^.LoaderHeap_Backup $ P_LoaderHeap_Backup(p1)
#define OS_LDHEAP_GETBACKUPFNAME(p1,p2) pLdheap^.LoaderHeap_GetBackupFname $ P_LoaderHeap_GetBackupFname(p1,p2)
#define OS_LDHEAP_CHECKLIST()       pLdheap^.LoaderHeap_CheckList $ P_LoaderHeap_CheckList()
#define OS_LDHEAP_INFO(a, s)		pLdheap^.LoaderHeap_GetInfo $ P_LoaderHeap_Info(a, s)

#define OS_APPHEAP_CHECKLIST()      pAppheap^.CheckHeapList $ P_CheckHeapList()

TYPE
	OS_LDHEAP : STRUCT
  		VERSION					: UDINT;
  		LoaderHeapAlloc 		: pvoid;
  		LoaderHeapReAlloc 		: pvoid;
  		LoaderHeapFree 			: pvoid;
  		LoaderHeapMarkAlloc 	: pvoid;
  		LoaderHeap_Backup 		: pvoid;
  		LoaderHeap_GetBackupFname : pvoid;
		LoaderHeap_GetInfo		: pvoid;
		LoaderHeap_CheckList	: pvoid;
	END_STRUCT;

END_TYPE
