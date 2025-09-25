#pragma once
#define __RTOS__ST__MACROS__


//
//	STMacros.h: Structured Text Macros
//


//
//	Create macros to reference external C methods
//
#define C_READ(mth)  FUNCTION GLOBAL mth VAR_OUTPUT EAX : DINT; END_VAR
#define C_WRITE(mth) FUNCTION GLOBAL mth VAR_INPUT EAX : DINT; END_VAR VAR_OUTPUT	EAX : DINT; END_VAR
#define C_INIT(mth) FUNCTION GLOBAL mth
#define C_CYWORK(mth)  FUNCTION GLOBAL mth VAR_OUTPUT EAX : DINT; END_VAR
#define C_RTWORK(mth)  FUNCTION GLOBAL mth VAR_OUTPUT EAX : DINT; END_VAR
#define C_NEWINSTR(mth)  FUNCTION GLOBAL mth VAR_INPUT	pPara	: ^CMDBUFFER; pResult	: ^RESULTS;	END_VAR	VAR_OUTPUT retcode	: IPRSTATES; END_VAR
#define C_KILL(mth)  FUNCTION GLOBAL mth VAR_OUTPUT EAX : DINT; END_VAR
#define C_GETSTATE(mth)  FUNCTION GLOBAL mth VAR_INPUT	pPara	: ^CMDBUFFER; pResult	: ^RESULTS;	END_VAR	VAR_OUTPUT retcode	: IPRSTATES; END_VAR

// determine the size (number of elements) of an array 
#define LSL_entriesof(a)   (sizeof(a) / sizeof(a[0]))
