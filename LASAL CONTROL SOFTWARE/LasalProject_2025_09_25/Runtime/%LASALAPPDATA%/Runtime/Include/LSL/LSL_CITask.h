/* $$!FNOTE 10 */
/****************************************************************************/
/*                                                                          */
/*  File: LSL_ITask.H                                                       */
/*  Date: June 16, 2000                                                     */
/*  By:                                                                     */
/*  Description:                                                            */
/*        This file contains the LASAL OS Task Interface.                        */
/*  Revision History:                                                       */
/*      Rev     By      Date        Description                             */
/*      ---     --      ----        -----------                             */
/*      1.00    THR     6/16/00     Initial Version                         */
/*                                                                          */
/****************************************************************************/
/* $$!ENDCMD */
#ifndef LSL_ITASK_H
#pragma once
#define LSL_ITASK_H

//
//    The TASK Interface provides access to the various LASAL OS Task API Functions.
//
//    Interface Name: TASK
//    Required: YES
//    References: UNLIMITED
//


//
//    LSLTSK_APP_MODE Codes duplicated in LslTask.H
//
#define TASK_DEMAND    0    // Run on demand
#define TASK_ALWAYS    1    // Run always
#define TASK_TIMED     2    // Run on time period

//
//    OR these codes together to specify the "phase"
//    that the task should run in.
//
#define TASK_PRESCAN  0x08
#define TASK_SCAN     0x10
#define TASK_POSTSCAN 0x20

#define TASK_BACKGRD	0x040	// Run task with minor priority
#define TASK_HIPRIOR	0x080	// Automatic task, 1 ms cycle time
#define TASK_WATCHDOG	0x100  // Calles Cy/RtWork to adjust PLC load


//! Version der Struktur #LSL_ITASK.
//  In #LSL_ITASK ist die Version das 22.Element (Offset 84).
#define ITASK_VERSION       0x11


//! Übergabeparameter für #TASK_AddUserCallback.
//  Dieser Wert wird dann im #TASK_USERCALLBACK als Typ übergeben
//@{
#define TASK_UCB_RT           0x1     //!< Mask for start RT-Object-Event
#define TASK_UCB_CT           0x2     //!< Mask for start CT-Object-Event
//!@}

//!
// Parameterliste für #TASK_AddCommonTask.
typedef struct
{
  unsigned long udVersion;
  // Ab udVersion 0x11
  unsigned long ulTask;
  void *pThis;
  void *pParam;
  unsigned long udIsFunction;
  unsigned long udMode;
  unsigned long udTime;
  unsigned long udPhase;
  unsigned long udSlot;
  // Ab udVersion 0x12
  long core;
} LSLTASK_PARAMLIST;


//! Definition Funktion für Callback
typedef void (*TASK_USERCALLBACK(int iType, unsigned long ulParam));

//!   ITASK Interface Structure.
//
//    The following structure is the ITASK Interface Structure.  The structure
//    provides the API for the ITASK module.
//
typedef struct
{
  // Real Time App Task Function
  void* (* AddCyclicFunction)(void* pTask, void* pParam, unsigned long udMode, unsigned long udTime, unsigned long udPhase);
  void* (* AddCyclicObject)(void* pObj, unsigned long udMode, unsigned long udTime, unsigned long udPhase);
  void  (* RemoveCyclicTask)(void* pApp);
  void  (* SignalCyclicTask)(void* pApp);
  void* (* AddRealTimeFunction)(void* pTask, void* pParam, unsigned long udMode, unsigned long udTime, unsigned long udPhase);
  void* (* AddRealTimeObject)(void* pObj, unsigned long udMode, unsigned long udTime, unsigned long udPhase);
  void  (* RemoveRealTimeTask)(void* pApp);
  void  (* SignalRealTimeTask)(void* pApp);
  void  (* StartMeasCyclicTask)(void* pApp);
  void  (* StopMeasCyclicTask)(void* pApp);
  void  (* StartMeasRealTimeTask)(void* pApp);
  void  (* StopMeasRealTimeTask)(void* pApp);
  void* (* AddFrgFunction)(void* pTask, void* pParam, unsigned long stack);
  void* (* AddFrgObject)(void* pObj, unsigned long stack);
  void  (* SuspendFrgTask)(void* pFrgTsk);
  void  (* ResumeFrgTask)(void* pFrgTsk);
////////////////////////////////////////////////////
  long  (* InitTaskModul)(void*);
  void* (* GetPointerToTaskList)(void*, unsigned long, unsigned long); 
              // (*Workspace, typeoflist=0: RT; 1: CT, what0=0:ptr 2 Task; 1: Timer)
  long  (* AddHPUserObject)(void* pMeth, void* pThis, int nSlot); // THR RTOS V5.06: High Priority User task object
  long  (* LSLOBJ_MoveObject)(void* thisPtr, unsigned long taskType, unsigned long flags); // pr: V5.43
  int   (* SetNbrOfLdrPhaseObjects)(unsigned long nbrOfObjs);
  //! Version //////
  unsigned long ulVersion; // Anm.: Beim Salamander ist das keine Versionsnummer sondern ein Zeiger auf eine Dummy-Funktion !
  unsigned long (* GetKernelCyclic)(void);
  unsigned long (* SuspendKernelCyclic)(unsigned long cthandle0);
/////////////////////////////////////////////////////
  long  (* LSLOBJ_SetObjectState)(void* thisobj0, unsigned char action0, unsigned char objtask0, unsigned char debugobj0, unsigned long objtime0);
  long  (* LSLOBJ_ResetTaskState)(unsigned char tasktype0);
  long  (* LSLOBJ_GetObjectInfo)(void* thisobj0, unsigned char tasktype0, LSLOBJ_INFO* objinfo0);
  long  (* LSLOBJ_BackUpObject)(void* thisobj0, unsigned char taskmask0);
  long  (* LSLOBJ_RestoreObject)(void* thisobj0, unsigned char taskmask0);
  void* (* TASK_AddCommonTask)(LSLTASK_PARAMLIST *pParam);
  int   (* TASK_AddUserCallback)(unsigned int iType, TASK_USERCALLBACK pCallback, void *pThis, unsigned long ulParam);

  // since version 0x11:
  unsigned long unused0;
  int (* LASALTASK_IsStopping)();
  void (* LASALTASK_SetStopwaitCntr)(int inc_dec);

  // since version 0x12:
  void* (* AddRealTimeFunctionEx)(void* pTask, void* pParam, unsigned long udMode, unsigned long udTime, unsigned long udPhase, int core);  
  void* (* AddRealTimeObjectEx)(void* pObj, unsigned long udMode, unsigned long udTime, unsigned long udPhase, int core);  
  void* (* AddCyclicFunctionEx)(void* pTask, void* pParam, unsigned long udMode, unsigned long udTime, unsigned long udPhase, int core);  
  void* (* AddCyclicObjectEx)(void* pObj, unsigned long udMode, unsigned long udTime, unsigned long udPhase, int core);  
  long (* LSLOBJ_MoveFunctionEx)(void* pTask, void* pParam, unsigned long taskType, unsigned long flags, int core);  
  long (* LSLOBJ_MoveObjectEx)(void* thisPtr, unsigned long taskType, unsigned long flags, int core);  
  int  (* SetNbrOfLdrPhaseObjectsEx)(unsigned long nbrOfObjs, int core);
  
} LSL_ITASK;


//
//
//    End of File
//
#endif    // of #ifndef LSL_ITASK_H
