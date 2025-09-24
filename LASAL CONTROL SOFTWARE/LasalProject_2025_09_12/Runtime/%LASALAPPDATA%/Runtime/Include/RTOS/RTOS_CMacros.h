/* $$!FNOTE 10 */
/****************************************************************************/
/*                                                                          */
/*  File: RTOS_CMacros.H                                                    */
/*  Date: June 14, 2000                                                     */
/*  By: THR                                                                 */
/*  Description:                                                            */
/*		This file contains macro definitions for use by C programs to		*/
/*		declare LASAL Object Methods in C.									*/
/*  Revision History:                                                       */
/*      Rev     By      Date        Description                             */
/*      ---     --      ----        -----------                             */
/*      1.00    THR     6/14/00     Initial Version                         */
/*                                                                          */
/****************************************************************************/
/* $$!ENDCMD */
#pragma once
#define _RTOS_CMACROS_H_


// Pack structures for "C" for 4 byte alignment
// (5/8/01 THR)
#pragma pack(push, 4)

//
//	General "C" Notes:
//
//		1)	Macros are provided for basic LASAL Methods implemented
//				in C.  These macros can be used / expanded for custom
//				methods in C.
//		3)	The LASAL object "this" pointer is passed to ALL methods
//				in ESI.
//		3)	By default, Structured Text (ST) passes parameters on
//				the stack PACKED (a WORD is passed as a WORD) where C
//				extends to a LONG (a WORD is extended to a LONG then
//				pushed).  If all parameters are LONG (DINT) then C
//				can be used as is for parameters.  Otherwise, macros
//				must pull things off the stack according to size.
//		4)	Many LASAL Class Methods pass a signle parameter in EAX.
//				Macros are used to obtain this input.
//		5)	By default, ST puts return variables on the stack AFTER the
//				input paramters.  To C, they look like additional input
//				parameters on the stack (as if they were defined to the
//				right of the input paramters).  Like input paramters, ST
//				pushes the outputs packed (not extended).
//		6)	Many LASAL Class methods return a single parameter in
//				EAX.  Macros have been created to handle this.
//		7)	The __CDECL specifier can be used in ST function declarations
//				and references to specify C parameter passing and expansion,
//				and C standard single return value in EAX.

//
//	Define C Macros
//

#define CMETH_GETTHIS(pt) _asm	{mov pt,esi}
#define CMETH_GETEAX(var) _asm {mov var,eax}
#define CMETH_PUTEAX(var) _asm {mov eax,var}
#define CMETH_PTHIS(cls,pt) cls* pt
#define CMETH_SVROFS(cls,svr) _asm {sub esi,OFFSET cls.svr}

#define CMETH_PROLOG(cls) \
	CMETH_PTHIS(cls,pThis);\
	CMETH_GETTHIS(pThis)

#define CMETH_SVRPROLOG(cls,svr) \
	CMETH_PTHIS(cls,pThis);\
	CMETH_SVROFS(cls,svr); \
    CMETH_GETTHIS(pThis)

#define CMETH_EPILOG } 

//
//	Define declaration specifiers for LASAL methods in C
//

// Main Command Server Generic Method Macros
#define CMETH_METHOD_NOPARM(cls,mth) \
	DINT cls##_##mth(void){\
	CMETH_PROLOG(cls);

#define CMETH_METHOD_EAXPARM(cls,mth,var) \
	DINT cls##_##mth(void){DINT var;\
	CMETH_PROLOG(cls);\
	CMETH_GETEAX(var);

#define CMETH_METHOD_SVRNOPARM(cls,svr,mth) \
	DINT cls##_##svr##_##mth(void){\
	CMETH_SVRPROLOG(cls,svr);

#define CMETH_METHOD_SVREAXPARM(cls,svr,mth,var) \
	DINT cls##_##svr##_##mth(void){DINT var;\
	CMETH_SVRPROLOG(cls,svr);\
	CMETH_GETEAX(var);

#define CMETH_METHOD_NORETURN CMETH_EPILOG

#define CMETH_METHOD_EAXRETURN(val) \
	return(val); \
	CMETH_EPILOG

// Use CMETH_RETURN macro for all method returns.
// Use CMETH_RETURN(0) for functions that do not
// specify a return value or use 1131-ST stack returns.
// USE THIS MACRO ONLY AT THE LAST LINE OF A METHOD FUNCTION.
#define CMETH_RETURN(val) CMETH_METHOD_EAXRETURN(val)

// Main Command Server Method Declaration Macros
#define CMETH_READ(cls) CMETH_METHOD_NOPARM(cls,Read)

#define CMETH_WRITE(cls,var) CMETH_METHOD_EAXPARM(cls,Write,var)

#define CMETH_INIT(cls) CMETH_METHOD_NOPARM(cls,Init)

#define CMETH_RTWORK(cls) CMETH_METHOD_NOPARM(cls,RtWork)

#define CMETH_CYWORK(cls) CMETH_METHOD_NOPARM(cls,CyWork)

#define CMETH_NEWINSTR(cls,pPara,pResult) \
	DINT cls##_NewInstr(void* pPara,void* pResult)\
	{CMETH_PROLOG(cls);\
	_UINT __output;

#define CMETH_NEWINSTR_RETURN(val) \
	_asm mov ax,val _asm mov __output,ax CMETH_EPILOG

#define CMETH_KILL(cls) CMETH_METHOD_NOPARM(cls,Kill)

#define CMETH_GETSTATE(cls,pPara,pResult) \
	DINT cls##_GetState(void* pPara,void* pResult)\
	{CMETH_PROLOG(cls);\
	_UINT __output;

#define CMETH_GETSTATE_RETURN(val) \
	_asm mov ax,val _asm mov __output,ax CMETH_EPILOG

// Data Channel Server Method Declaration Macros
#define CMETH_SVRREAD(cls,svr) CMETH_METHOD_SVRNOPARM(cls,svr,Read)

#define CMETH_SVRWRITE(cls,svr,var) CMETH_METHOD_SVREAXPARM(cls,svr,Write,var)


//
//	Define Client Channel Macros
//

// Call a client with EAX parm and EAX return
#define CCLIENT_RCALLEAX(ret,cls,cli,mth,prm) \
	_asm {mov eax,pThis};\
	_asm {mov esi,[eax] + cls.cli.pCH};\
	_asm {mov eax,esi};\
	_asm {mov edx,[eax]};\
	_asm {mov eax,prm};\
	_asm {call [edx]+mth};\
	_asm {mov ret,eax}

// Call a client with EAX return
#define CCLIENT_RCALL(ret,cls,cli,mth) \
	_asm {mov eax,pThis};\
	_asm {mov esi,[eax] + cls.cli.pCH};\
	_asm {mov eax,esi};\
	_asm {mov edx,[eax]};\
	_asm {call [edx]+mth};\
	_asm {mov ret,eax}

// THR: 
#define CCLIENT_CALL1(cli,typ,mth,prm1) \
	((typ)pThis->cli.pCH->pMeth->mth)(prm1)

#define CCLIENT_CALL2(cli,typ,mth,prm1,prm2) \
	((typ)pThis->cli.pCH->pMeth->mth)(prm1,prm2)



//
//	Define Offsets for Method Tables
//	(Read/Write common to COMMAND and DATA channels)
//
#define CCLIENT_OFS_READ		0
#define CCLIENT_OFS_WRITE		4
#define CCLIENT_OFS_INIT		16
#define CCLIENT_OFS_CYWORK		20
#define CCLIENT_OFS_RTWORK		24
#define CCLIENT_OFS_NEWINSTR	28
#define CCLIENT_OFS_KILL		32
#define CCLIENT_OFS_GETSTATE	36

//
//	Define Client Call Macros
//
#define CCLIENT_READ(ret,cls,cli) CCLIENT_RCALL(ret,cls,cli,CCLIENT_OFS_READ)
#define CCLIENT_WRITE(ret,cls,cli,val) CCLIENT_RCALLEAX(ret,cls,cli,CCLIENT_OFS_WRITE,val)
//#define CCLIENT_INIT(ret,cli) CCLIENT_RCALL(ret,cli,pCMETH_READ,pRd)

// determine the size (number of elements) of an array 
#define LSL_entriesof(a)   (sizeof(a) / sizeof((a)[0]))

//
//	End of File
//
#pragma pack(pop)
