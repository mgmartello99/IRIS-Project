//* $$!FNOTE 10 */
//****************************************************************************/
//*                                                                          */
//*  File: LSL_ITask.H                                                       */
//*  Date: June 16, 2000                                                     */
//*  By:                                                                     */
//*  Description:                                                            */
//*		This file contains the LASAL OS Task Interface.                		*/
//*  Revision History:                                                       */
//*      Rev     By      Date        Description                             */
//*      ---     --      ----        -----------                             */
//*      1.00    THR     6/16/00     Initial Version                         */
//*                                                                          */
//****************************************************************************/
//* $$!ENDCMD */
#ifndef LSL_ITASK_H
#pragma once
#define LSL_ITASK_H

//
//	The TASK Interface provides access to the various LASAL OS Task API Functions.
//
//	Interface Name: TASK
//	Required: YES
//	References: UNLIMITED
//

//
//	LSLTSK_APP_MODE Codes duplicated in LslTask.H
//
#define TASK_DEMAND	0	// Run on demand
#define TASK_ALWAYS	1	// Run always
#define TASK_TIMED	2	// Run on time period

//
//	Task Scan Modes: Define to run task as
//	Pre-Scan, Normal Scan, and/or Post-Scan
//
#define TASK_PRE	16#008	// Run as PRE-SCAN
#define TASK_SCAN	16#010	// Run as SCAN
#define TASK_POST	16#020	// Run as POST-SCAN
#define TASK_BACKGRD	16#040	// Run task with minor priority
#define TASK_HIPRIOR	16#080	// Automatic task, 1 ms cycle time
#define TASK_WATCHDOG	16#100  // Calles Cy/RtWork to adjust PLC load

// defines for TaskTypes of Objects
#define TASK_OBJ_CT         1  // cyclic
#define TASK_OBJ_RT         2  // realtime
#define TASK_OBJ_TT         3  // foreground
#define TASK_OBJ_BG         4  // background
#define TASK_OBJ_HP         5  // high-priority user

#define TASK_OBJ_MT        10	// multitask function
#define TASK_OBJ_LDRINIT   11  // Loader init-phase (used in exception handler)
#define TASK_OBJ_LDRMAIN   12  // Loader main-function (used in exception handler)

//! Version der Struktur #LSL_ITASK.
//  In #LSL_ITASK ist die Version das 22.Element (Offset 84).
#define ITASK_VERSION       0x11

TYPE
  //!
  // Parameterliste für #TASK_AddCommonTask.
  ITASK_PARAMLIST : struct
    udVersion     : udint;
    // Ab udVersion 0x11
    udTask        : udint;
    pObj          : ^void;
    pParam        : ^void;
    udIsFnc       : udint;
    udMode        : udint;
    udTime        : udint;
    udPhase       : udint;
    udSlot        : udint;
    // Ab udVersion 0x12
    core          : dint;
  END_STRUCT;
END_TYPE

// Function prototype for AddXObject()
FUNCTION GLOBAL __cdecl P_AddObject 
VAR_INPUT
	pObj		: pVoid;
	udMode	: UDINT;
	udTime	: UDINT;
	udPhase	: UDINT;
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
	udPhase	: UDINT;
END_VAR
VAR_OUTPUT 
	handle	: UDINT;
END_VAR;

// Function prototype for AddXObjectEx()
FUNCTION GLOBAL __cdecl P_AddObjectEx 
VAR_INPUT
	pObj		: pVoid;
	udMode	: UDINT;
	udTime	: UDINT;
	udPhase	: UDINT;
  core : DINT;
END_VAR
VAR_OUTPUT 
	handle	: UDINT;
END_VAR;

// Function prototype for AddXFunctionEx()
FUNCTION GLOBAL __cdecl P_AddFunctionEx
VAR_INPUT
	pFunc	: pVoid;
	pParam  : pVoid;
	udMode	: UDINT;
	udTime	: UDINT;
	udPhase	: UDINT;
  core : DINT;
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
FUNCTION GLOBAL __cdecl P_StopMeasTask
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

//unsigned long (* GetKernelCyclic)(void);
FUNCTION GLOBAL __cdecl P_GetKernelCyclic
VAR_OUTPUT 
	handle	: UDINT;
END_VAR;

//unsigned long (* SuspendKernelCyclic)(unsigned long cthandle0);
FUNCTION GLOBAL __cdecl P_SuspendKernelCyclic
VAR_INPUT
	cyhandle    : UDINT;
END_VAR
VAR_OUTPUT 
	success 	: UDINT;
END_VAR;


// Function prototype for AddHPUserObject()
// THR RTOS V5.06: High Priority User task object
FUNCTION GLOBAL __cdecl P_AddHPUserObject
VAR_INPUT
	pMeth		: pVoid;
	pThis		: pVoid;
	nSlot		: UDINT;
END_VAR
VAR_OUTPUT 
	result		: UDINT;
END_VAR;

// pr V5.43:
FUNCTION GLOBAL __cdecl P_SetNbrOfLdrPhaseObjects
VAR_INPUT
	nbrOfObjs : UDINT;
END_VAR
VAR_OUTPUT 
	result		: DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_SetNbrOfLdrPhaseObjectsEx
VAR_INPUT
	nbrOfObjs : UDINT;
  core : DINT;
END_VAR
VAR_OUTPUT 
	result		: DINT;
END_VAR;

// pr V5.43:
FUNCTION GLOBAL __cdecl P_MoveObject
VAR_INPUT
	thisPtr   : pVoid;
  taskType  : UDINT;
  flags     : UDINT;
END_VAR
VAR_OUTPUT 
	result		: DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_MoveObjectEx
VAR_INPUT
	thisPtr   : pVoid;
  taskType  : UDINT;
  flags     : UDINT;
  core : DINT;
END_VAR
VAR_OUTPUT 
	result		: DINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_MoveFunctionEx
VAR_INPUT
	pFunc	: pVoid;
	pParam  : pVoid;
  taskType  : UDINT;
  flags     : UDINT;
  core : DINT;
END_VAR
VAR_OUTPUT 
	result		: DINT;
END_VAR;

//long (* LSLOBJ_BackUpObject)(void* thisobj0, unsigned char taskmask0);
FUNCTION GLOBAL __cdecl P_BackUpObject
VAR_INPUT
	thisPtr   : pVoid;
  taskMask  : UDINT;
END_VAR
VAR_OUTPUT 
	result		: UDINT;
END_VAR;

//long (* LSLOBJ_RestoreObject)(void* thisobj0, unsigned char taskmask0);
FUNCTION GLOBAL __cdecl P_RestoreObject
VAR_INPUT
	thisPtr   : pVoid;
	taskMask  : UDINT;
END_VAR
VAR_OUTPUT 
	result		: UDINT;
END_VAR;

// Function prototype for AddCallback()
FUNCTION GLOBAL __cdecl P_AddTask
VAR_INPUT
	pCallback : ^void;
END_VAR
VAR_OUTPUT 
	ptr       : udint;
END_VAR;

// Function prototype for AddCallback()
FUNCTION GLOBAL __cdecl P_TaskAddCallback
VAR_INPUT
	udType    : UDINT;
	pCallback : ^void;
	pThis     : ^void;
	udParam   : udint;
END_VAR
VAR_OUTPUT 
	dRet      : dint;
END_VAR;

// Function prototype for OS_ITASK_IS_STOPPING()
FUNCTION GLOBAL __cdecl P_IsStopping
VAR_OUTPUT
  is_stopping : UDINT;
END_VAR;

// Function prototype for OS_ITASK_SET_STOPWAIT_CNTR()
FUNCTION GLOBAL __cdecl P_SetStopwaitCntr
VAR_INPUT
  inc_dec : DINT;
END_VAR;

//
//	ITASK Interface Macros
//
#define OS_AddCyObject(p1,p2,p3) _LSL_POS^.piTask^.AddCyclicObject $ P_AddObject(this,p1,p2,p3)
#define OS_AddCyFunc(p1,p2,p3,p4,p5) _LSL_POS^.piTask^.AddCyclicFunction $ P_AddFunction(#p1(),p2,p3,p4,p5)
#define OS_RemCyTask(p1) _LSL_POS^.piTask^.RemoveCyclicTask $ P_RemoveTask(p1)
#define OS_SigCyTask(p1) _LSL_POS^.piTask^.SignalCyclicTask $ P_SignalTask(p1)
#define OS_StrMsCyTask(p1) _LSL_POS^.piTask^.StartMeasCyclicTask $ P_StartMeasTask(p1)
#define OS_StpMsCyTask(p1) _LSL_POS^.piTask^.StopMeasCyclicTask $ P_StopMeasTask(p1)

#define OS_AddRtObject(p1,p2,p3) _LSL_POS^.piTask^.AddRealTimeObject $ P_AddObject(this,p1,p2,p3)
#define OS_AddRtFunc(p1,p2,p3,p4,p5) _LSL_POS^.piTask^.AddRealTimeFunction $ P_AddFunction(#p1(),p2,p3,p4,p5)
#define OS_RemRtTask(p1) _LSL_POS^.piTask^.RemoveRealTimeTask $ P_RemoveTask(p1)
#define OS_SigRtTask(p1) _LSL_POS^.piTask^.SignalRealTimeTask $ P_SignalTask(p1)
#define OS_StrRtCyTask(p1) _LSL_POS^.piTask^.StartMeasRealTimeTask $ P_StartMeasTask(p1)
#define OS_StpRtCyTask(p1) _LSL_POS^.piTask^.StopMeasRealTimeTask $ P_StopMeasTask(p1)

#define OS_AddFrgFunc(p1,p2,p3) _LSL_POS^.piTask^.AddFrgFunction $ P_AddFrgFunction(#p1(),p2,p3);
#define OS_AddFrgObject(p1) _LSL_POS^.piTask^.AddFrgObject $ P_AddFrgObject(this,p1);
#define OS_SuspendFrgTask(p1) _LSL_POS^.piTask^.SuspendFrgTask $ P_SuspendFrgTask(p1);
#define OS_ResumeFrgTask(p1) _LSL_POS^.piTask^.ResumeFrgTask $ P_ResumeFrgTask(p1);

#define OS_GetKernelCyclic() _LSL_POS^.piTask^.GetKernelCyclic $ P_GetKernelCyclic()
#define OS_SuspendKernelCyclic(p1) _LSL_POS^.piTask^.SuspendKernelCyclic $ P_SuspendKernelCyclic(p1)

// THR RTOS V5.06: High Priority User task object
#define OS_AddHPUserObject(p1,p2) _LSL_POS^.piTask^.AddHPUserObject $ P_AddHPUserObject(p1,this,p2);

// pr: V5.43
#define OS_SetNbrOfLdrPhaseObjects(p1) _LSL_POS^.piTask^.SetNbrOfLdrPhaseObjects $ P_SetNbrOfLdrPhaseObjects(p1);
#define OS_MoveObject(p1,p2,p3) _LSL_POS^.piTask^.MoveObject $ P_MoveObject(p1,p2,p3);

#define OS_BackUpObject(p1,p2) _LSL_POS^.piTask^.BackUpObject $ P_BackUpObject(p1,p2);
#define OS_RestoreObject(p1,p2) _LSL_POS^.piTask^.RestoreObject $ P_RestoreObject(p1,p2);

#define OS_AddTask(p1) _LSL_POS^.piTask^.AddTask $ P_AddTask(p1);
#define OS_TaskAddCallback(p1,p2, p3) _LSL_POS^.piTask^.AddCallback $ P_TaskAddCallback(p1,p2,p3);

#define OS_ITASK_VERSION         _LSL_POS^.piTask^.udVersion $ UDINT

// since version 0x11
#define OS_ITASK_IS_STOPPING()          _LSL_POS^.piTask^.IsStopping $ P_IsStopping()
#define OS_ITASK_SET_STOPWAIT_CNTR(p1)  _LSL_POS^.piTask^.SetStopwaitCntr $ P_SetStopwaitCntr(p1)

// since version 0x12
#define OS_AddRtFuncEx(p1,p2,p3,p4,p5,p6) _LSL_POS^.piTask^.AddRealTimeFunctionEx $ P_AddFunctionEx(#p1(),p2,p3,p4,p5,p6)
#define OS_AddRtObjectEx(p1,p2,p3,p4) _LSL_POS^.piTask^.AddRealTimeObjectEx $ P_AddObjectEx(this,p1,p2,p3,p4)
#define OS_AddCyFuncEx(p1,p2,p3,p4,p5,p6) _LSL_POS^.piTask^.AddCyclicFunctionEx $ P_AddFunctionEx(#p1(),p2,p3,p4,p5,p6)
#define OS_AddCyObjectEx(p1,p2,p3,p4) _LSL_POS^.piTask^.AddCyclicObjectEx $ P_AddObjectEx(this,p1,p2,p3,p4)
#define OS_MoveFunctionEx(p1,p2,p3,p4,p5) _LSL_POS^.piTask^.MoveFunctionEx $ P_MoveFunctionEx(p1,p2,p3,p4,p5);
#define OS_MoveObjectEx(p1,p2,p3,p4) _LSL_POS^.piTask^.MoveObjectEx $ P_MoveObjectEx(p1,p2,p3,p4);
#define OS_SetNbrOfLdrPhaseObjectsEx(p1,p2) _LSL_POS^.piTask^.SetNbrOfLdrPhaseObjectsEx $ P_SetNbrOfLdrPhaseObjectsEx(p1,p2);

//
//
//	End of File
//
#endif  // of #ifndef LSL_ITASK_H
