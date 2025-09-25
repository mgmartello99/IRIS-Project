#ifndef  __ALTERNATEBUF_H
#pragma once
#define  __ALTERNATEBUF_H

// Interface
#define INTERFACE_ALTERNATEBUF                       "IALTERNATEBUF"

// defines for possible Error Codes
#define AB_ERROR_NO					0
#define AB_ERROR_WRONG_IDX			-1
#define AB_ERROR_WRONG_BUFIDX		-2
#define AB_ERROR_INVALID_ADDR		-3
#define AB_ERROR_OFF_LEN			-4		// offset + length out of range
#define AB_ERROR_CALLBACK_SET		-5		// callback is already set

// typedef void (*alternatebuf_callback_t)(void *pCookie, uint32_t entryIdx, uint32_t bufIdx);

TYPE
	LSLAPI_IALTERNATEBUF_INFO :    STRUCT
		type0	: UDINT;			// EABTypeSDIAS = 0, EABTypeVaranOld = 1, EABTypeVaran = 2
		bufTypes : UDINT;			// bit0 = ISO, bit1 = asy, bit2 = da
		dummy0 : UDINT;				// 0
		dummy1 : UDINT;				// 0
	END_STRUCT;
END_TYPE

TYPE
    LSLAPI_IALTERNATEBUF :    STRUCT
    	//
    	// Version
    	//
    	udVersion		: UDINT;
    	//
    	GetCount		: pVoid;
    	GetInfo			: pVoid;
    	SetCallback		: pVoid;
    	Write			: pVoid;
    	Read			: pVoid;
  END_STRUCT;
END_TYPE

////////////////////////////////////////////////////////////////////////////////
// LASAL Function Prototypes
////////////////////////////////////////////////////////////////////////////////
// Callback Prototype
FUNCTION __CDECL GLOBAL P_ALTERNATEBUF_GETCOUNT
  VAR_OUTPUT
    retval  : UDINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_ALTERNATEBUF_GETINFO
  VAR_INPUT
    entryIdx  : UDINT;
    info  : ^LSLAPI_IALTERNATEBUF_INFO;
  END_VAR
  VAR_OUTPUT
    retval   : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_ALTERNATEBUF_SETCALLBACK
  VAR_INPUT
  	  callback : pVoid;
  	  cookie : pVoid;
  END_VAR
  VAR_OUTPUT
    retval   : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_ALTERNATEBUF_WRITE
VAR_INPUT
	entryIdx : UDINT;
	bufIdx : UDINT;
	offset : UDINT;
	pData : pVoid;
	length : UDINT;
END_VAR
VAR_OUTPUT
	retval   : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_ALTERNATEBUF_READ
VAR_INPUT
	entryIdx : UDINT;
	bufIdx : UDINT;
	offset : UDINT;
	pData : pVoid;
	length : UDINT;
END_VAR
VAR_OUTPUT
	retval   : DINT;
END_VAR;


////////////////////////////////////////////////////////////////////////////////
// LASAL MACROS
////////////////////////////////////////////////////////////////////////////////
#define OS_ALTERNATEBUF_GETCOUNT(pALTERNATEBUF) 				pALTERNATEBUF^.GetCount $ P_ALTERNATEBUF_GETCOUNT()
#define OS_ALTERNATEBUF_GETINFO(pALTERNATEBUF,p1,p2)			pALTERNATEBUF^.GetInfo $ P_ALTERNATEBUF_GETINFO(p1, p2)
#define OS_ALTERNATEBUF_SETCALLBACK(pALTERNATEBUF,p1,p2)		pALTERNATEBUF^.SetCallback $ P_ALTERNATEBUF_SETCALLBACK(p1, p2)
#define OS_ALTERNATEBUF_WRITE(pALTERNATEBUF,p1,p2,p3,p4,p5)		pALTERNATEBUF^.Write $ P_ALTERNATEBUF_WRITE(p1, p2,p3,p4,p5)
#define OS_ALTERNATEBUF_READ(pALTERNATEBUF,p1,p2,p3,p4,p5)		pALTERNATEBUF^.Read $ P_ALTERNATEBUF_READ(p1,p2,p3,p4,p5)

#endif // __ALTERNATEBUFFER_H
