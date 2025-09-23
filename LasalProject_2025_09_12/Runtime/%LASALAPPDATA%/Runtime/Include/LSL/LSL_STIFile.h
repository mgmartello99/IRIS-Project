//* $$!FNOTE 10 */
//****************************************************************************/
//*                                                                          */
//*  File: LSL_STIFile.H                                                      */
//*  Date: August 23, 2000                                                  */
//*  By:                                                                     */
//*  Description:                                                            */
//*		This file contains the LASAL OS File Interface.                		*/
//*  Revision History:                                                       */
//*      Rev     By      Date        Description                             */
//*      ---     --      ----        -----------                             */
//*      1.00    THR     8/23/00     Initial Version                         */
//*                                                                          */
//****************************************************************************/
//* $$!ENDCMD */
#ifndef LSL_STIFILE_H
#pragma once
#define LSL_STIFILE_H

//
//	The FILE Interface provides access to the various LASAL OS File API Functions.
//
//	Interface Name: FILE
//	Required: YES
//	References: UNLIMITED
//

//
//	File Interface Structure Definitions
//
TYPE
	pCFile		:	pVoid;	// C FILE pointer
	SYS_IFILE	:	STRUCT
		// File System Interface
		clearerr	:	pVoid;
		fclose		:	pVoid;
		_fcloseall	:	pVoid;
		feof		:	pVoid;
		ferror		:	pVoid;
		fflush		:	pVoid;
		fgetc		:	pVoid;
		_fgetchar	:	pVoid;
		fgetpos		:	pVoid;
		fgets		:	pVoid;
		_fileno		:	pVoid;
		_flushall	:	pVoid;
		fopen		:	pVoid;
		fprintf		:	pVoid;
		fputc		:	pVoid;
		fputs		:	pVoid;
		fread		:	pVoid;
		freopen		:	pVoid;
		fscanf		:	pVoid;
		fsetpos		:	pVoid;
		fseek		:	pVoid;
		ftell		:	pVoid;
		fwrite		:	pVoid;
		remove		:	pVoid;
		rename		:	pVoid;
		rewind		:	pVoid;
		_rmtmp		:	pVoid;
		setbuf		:	pVoid;
		setvbuf		:	pVoid;
		_tempnam	:	pVoid;
		tmpfile		:	pVoid;
		tmpnam		:	pVoid;
		ungetc		:	pVoid;
	END_STRUCT;

END_TYPE

(*****************
// Function prototype for AddXObject()
FUNCTION GLOBAL __cdecl P_AddObject 
VAR_INPUT
	pObj		: pVoid;
	udMode	: UDINT;
	udTime	: UDINT;
END_VAR
VAR_OUTPUT 
	handle	: UDINT;
END_VAR;

// Function prototype for AddXFunction()
FUNCTION GLOBAL __cdecl P_AddFunction
VAR_INPUT
	pFunc	: pVoid;
	pParam  : pVoid;
	udMode	: UDINT;
	udTime	: UDINT;
END_VAR
VAR_OUTPUT 
	handle	: UDINT;
END_VAR;

// Function prototype for RemoveXTask()
FUNCTION GLOBAL __cdecl P_RemoveTask
VAR_INPUT
	handle	: pVoid;
END_VAR;

// Function prototype for SignalXTask()
FUNCTION GLOBAL __cdecl P_SignalTask
VAR_INPUT
	handle	: pVoid;
END_VAR;

// Function prototype for StartMeasXTask()
FUNCTION GLOBAL __cdecl P_StartMeasTask
VAR_INPUT
	handle	: pVoid;
END_VAR;

// Function prototype for StopMeasTask()
FUNCTION GLOBAL __cdecl P_StopMeasXTask
VAR_INPUT
	handle	: pVoid;
END_VAR;

// Function prototype for AddFrgFunction()
FUNCTION GLOBAL __cdecl P_AddFrgFunction
VAR_INPUT
	pFunc		: pVoid;
	pParam	: pVoid;
	stack		: UDINT;
END_VAR
VAR_OUTPUT 
	handle	: UDINT;
END_VAR;

// Function prototype for AddFrgObject()
FUNCTION GLOBAL __cdecl P_AddFrgObject
VAR_INPUT
	pObj		: pVoid;
	stack		: UDINT;
END_VAR
VAR_OUTPUT 
	handle	: UDINT;
END_VAR;

// Function prototype for SuspendFrgTask()
FUNCTION GLOBAL __cdecl P_SuspendFrgTask
VAR_INPUT
	handle	: pVoid;
END_VAR;

// Function prototype for ResumeFrgTask()
FUNCTION GLOBAL __cdecl P_ResumeFrgTask
VAR_INPUT
	handle	: pVoid;
END_VAR;





//
//	ITASK Interface Macros
//
#define OS_AddCyObject(p1,p2) _LSL_POS^.piTask^.AddCyclicObject $ P_AddObject(this,p1,p2)
#define OS_AddCyFunc(p1,p2,p3,p4) _LSL_POS^.piTask^.AddCyclicFunction $ P_AddFunction(#p1(),p2,p3,p4)
#define OS_RemCyTask(p1) _LSL_POS^.piTask^.RemoveCyclicTask $ P_RemoveTask(p1)
#define OS_SigCyTask(p1) _LSL_POS^.piTask^.SignalCyclicTask $ P_SignalTask(p1)
#define OS_StrMsCyTask(p1) _LSL_POS^.piTask^.StartMeasCyclicTask $ P_StartMeasTask(p1)
#define OS_StpMsCyTask(p1) _LSL_POS^.piTask^.StopMeasCyclicTask $ P_StopMeasTask(p1)

#define OS_AddRtObject(p1,p2) _LSL_POS^.piTask^.AddRealTimeObject $ P_AddObject(this,p1,p2)
#define OS_AddRtFunc(p1,p2,p3,p4) _LSL_POS^.piTask^.AddRealTimeFunction $ P_AddFunction(#p1(),p2,p3,p4)
#define OS_RemRtTask(p1) _LSL_POS^.piTask^.RemoveRealTimeTask $ P_RemoveTask(p1)
#define OS_SigRtTask(p1) _LSL_POS^.piTask^.SignalRealTimeTask $ P_SignalTask(p1)
#define OS_StrRtCyTask(p1) _LSL_POS^.piTask^.StartMeasRealTimeTask $ P_StartMeasTask(p1)
#define OS_StpRtCyTask(p1) _LSL_POS^.piTask^.StopMeasRealTimeTask $ P_StopMeasTask(p1)

#define OS_AddFrgFunc(p1,p2,p3) _LSL_POS^.piTask^.AddFrgFunction $ P_AddFrgFunction(#p1(),p2,p3);
#define OS_AddFrgObject(p1) _LSL_POS^.piTask^.AddFrgObject $ P_AddFrgObject(this,p1);
#define OS_SuspendFrgTask(p1) _LSL_POS^.piTask^.SuspendFrgTask $ P_SuspendFrgTask(p1);
#define OS_ResumeFrgTask(p1) _LSL_POS^.piTask^.ResumeFrgTask $ P_ResumeFrgTask(p1);


************************)
//
//
//	End of File
//


 

#endif  // of #ifndef LSL_ITASK_H
