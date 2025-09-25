#ifndef  __LSL_MOUNT_H
#pragma once
#define  __LSL_MOUNT_H

#define MOUNT_TYPE_LOCAL	"LOCAL"
#define MOUNT_TYPE_SAMBA	"SAMBA"
// from interface version 2 on, the version of SAMBA can be selected
#define MOUNT_TYPE_SAMBA_V1	"SAMBA_V1"
#define MOUNT_TYPE_SAMBA_V2	"SAMBA_V2"

// These options are only working for network drives
#define MOUNT_OPTION_READONLY		1
#define MOUNT_OPTION_SYNCHRONOUS	16	// no cache for write operations

//typedef int (*MOUNT_CALLBACK_FUNCTION_TYPE)		(char*);

// Errors
#define MOUNT_ERROR_INVALID_PARAMETERS			16#81700001
#define MOUNT_ERROR_MAX_MOUNT					16#81700002
#define MOUNT_ERROR_INVALID_MOUNTLINK			16#81700003
#define MOUNT_ERROR_INVALID_MOUNTPOINT			16#81700004
#define MOUNT_ERROR_ALREADY_MOUNTED				16#81700005
#define MOUNT_ERROR_CANNOT_CREATE_MOUNTPOINT	16#81700006
#define MOUNT_ERROR_UNKNOWN_MOUNTTYPE			16#81700007
#define MOUNT_ERROR_NOT_MOUNTED					16#81700008
#define MOUNT_ERROR_IS_LOCKED					16#81700009

////////////////////////////////////////////////////////////////////////////////
// LASAL Function Prototypes
////////////////////////////////////////////////////////////////////////////////
FUNCTION __CDECL GLOBAL P_Mount
	VAR_INPUT
		pMountType		: ^CHAR;
		pMountLink		: ^CHAR;
		pMountPoint		: ^CHAR;
		pUserName		: ^CHAR;
		pPassword		: ^CHAR;
		options			: UDINT;
		pFunction		: ^void;
		pThis			: ^void;
		msTimeout		: UDINT;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_Umount
	VAR_INPUT
		pMountPoint		: ^CHAR;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_AddCallback
	VAR_INPUT
		pMountPoint		: ^CHAR;
		pFunction		: ^void;
		pThis			: ^void;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_RemoveCallback
	VAR_INPUT
		pMountPoint		: ^CHAR;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

////////////////////////////////////////////////////////////////////////////////
// LASAL MACROS
////////////////////////////////////////////////////////////////////////////////
#define OS_MOUNT_Mount(p1, p2, p3, p4, p6, x1, p7, p8, p9) MOUNT^.Mount $ P_Mount(p1, p2, p3, p4, p6, x1, p7, p8, p9)
#define OS_MOUNT_Umount(p1) MOUNT^.Umount $ P_Umount(p1)
#define OS_MOUNT_AddCallback(p1, p2, p3) MOUNT^.AddCallback $ P_AddCallback(p1, p2, p3)
#define OS_MOUNT_RemoveCallback(p1) MOUNT^.RemoveCallback $ P_AddCallback(p1)

#endif
