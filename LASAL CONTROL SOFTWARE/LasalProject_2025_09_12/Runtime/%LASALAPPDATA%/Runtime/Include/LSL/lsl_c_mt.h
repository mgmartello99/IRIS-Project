#ifndef  __LSL_C_MT_H
#pragma once
#define  __LSL_C_MT_H

typedef void *MT_TASKHANDLE;
typedef void *MT_SEMAHANDLE;
typedef void *MT_MAILBOX;

typedef void *(*MT_TaskFunction)(void *parameter0);

/* Task states */
typedef enum { MTASKSTATE_READY,
               MTASKSTATE_CURRENT,
               MTASKSTATE_SUSPENDED,
               MTASKSTATE_DELAYING,
               MTASKSTATE_BLOCKED_WAIT,
               MTASKSTATE_TIMED_WAIT,
               MTASKSTATE_BLOCKED_PUT,
               MTASKSTATE_BLOCKED_GET,
               MTASKSTATE_TIMED_PUT,
               MTASKSTATE_TIMED_GET,
               MTASKSTATE_BLOCKED_SEND,
               MTASKSTATE_BLOCKED_RECEIVE,
               MTASKSTATE_TIMED_SEND,
               MTASKSTATE_TIMED_RECEIVE,
               MTASKSTATE_DEADLOCKED,
               MTASKSTATE_ILLEGAL,
               MTASKSTATE_TERMINATED
} LSL_MT_TASKSTATE;

/* Semaphore types */
typedef enum { MTSEMATYPE_COUNTING,
               MTSEMATYPE_RESOURCE,
               MTSEMATYPE_BINARY,
               MTSEMATYPE_EVENT,
               MTSEMATYPE_MUTEX
} LSL_MT_SEMATYPE;
#define MTSEMACREATE_SEARCH          1
#define MTSEMACREATE_FAIL_NOT_FOUND  2

#define MT_TASK_SAVETHIS             0x00000008   // input parmaeter is LASAL - thispointer



#define MTERROR_NONE         0
#define MTERROR_NOMEM       -1
#define MTERROR_NOFCT       -2
#define MTERROR_FCTNOTINMEM -3
#define MTERROR_WRONGPRIOR  -4
#define MTERROR_STACK       -5
#define MTERROR_NAME        -6
#define MTERROR_NAMEUSED    -7
#define MTERROR_HANDLE      -8
#define MTERROR_NOTALLOWED  -9
#define MTERROR_MESSAGESIZE -10
#define MTERROR_DATA        -11
#define MTERROR_PTR         -12
#define MTERROR_TASKID      -13
#define MTERROR_NOTASK      -14
#define MTERROR_NOIFFNC     -15


#define MT_TASKID_RT        1
#define MT_TASKID_CT        2
#define MT_TASKID_BT        3



typedef struct
{
#define  LSL_MT_TYPE_VERSION 0x00010003

    unsigned long    version;
    long             (* MT_GetLastError)(void);
    // Aktuelle Scheduler Zeit in ms
    unsigned long    (* MT_GetTime)(void);
    // TASK Functions: Creation, Termination, Intertask Communication, Blocking,...
    MT_TASKHANDLE    (* MT_CreateThread)(MT_TaskFunction taskfunction0,
                                        unsigned long   priority0,
                                        unsigned long   stackSize0,
                                        unsigned long   flags0,
                                        void            * parameter0,
                                        char            * name0);
    long             (* MT_TerminateTask)(MT_TASKHANDLE handle0);
    long             (* MT_Suspend)(MT_TASKHANDLE handle0);
    long             (* MT_Resume)(MT_TASKHANDLE  handle0);
    MT_TASKHANDLE    (* MT_CurrentTaskHandle)(void);
    LSL_MT_TASKSTATE (* MT_GetTaskState)(MT_TASKHANDLE handle0);
    long             (* MT_SetPriority)(MT_TASKHANDLE handle0, unsigned long priority0);
    unsigned long    (* MT_GetTaskPrio)(MT_TASKHANDLE handle0);
    unsigned long    (* MT_GetTaskStack)(MT_TASKHANDLE handle0);
    unsigned long    (* MT_GetMinStack)(MT_TASKHANDLE handle0);
    long             (* MT_Delay)(unsigned long ms_timeout0);
    long             (* MT_DelayUntil)(unsigned long ms_timeout0);
    long             (* MT_Send)(MT_TASKHANDLE receiverhandle0, void * data0);
    long             (* MT_SendCond)(MT_TASKHANDLE receiverhandle0, void * data0);
    long             (* MT_SendTimed)(MT_TASKHANDLE receiverhandle0, void * data0, unsigned long ms_timeout0);
    long             (* MT_Receive)(void * data0, unsigned long length0);
    long             (* MT_ReceiveCond)(void * data0, unsigned long length0);
    long             (* MT_ReceiveTimed)(void * data0, unsigned long length0, unsigned long ms_timeout0);

    // Semaphore Functions: Creation, Termination, Signaling, Blocking,...
    MT_SEMAHANDLE    (* MT_CreateSemaphore)(LSL_MT_SEMATYPE  type0, unsigned long init0, unsigned long flags0, char * name0);
    long             (* MT_DeleteSemaphore)(MT_SEMAHANDLE semaphore0);
    unsigned long    (* MT_SemaValue)(MT_SEMAHANDLE semaphore0);
    MT_TASKHANDLE    (* MT_ResourceOwner)(MT_SEMAHANDLE semaphore0);
    long             (* MT_Signal)(MT_SEMAHANDLE semaphore0);
    long             (* MT_Pulse)(MT_SEMAHANDLE semaphore0);
    long             (* MT_Wait)(MT_SEMAHANDLE semaphore0);
    long             (* MT_WaitCond)(MT_SEMAHANDLE semaphore0);
    long             (* MT_WaitTimed)(MT_SEMAHANDLE semaphore0, unsigned long timeout0);
    long             (* MT_ResetEvent)(MT_SEMAHANDLE semaphore0);
    
    // Semaphore Functions: Creation, Termination, Message Passing, Blocking,...
    MT_MAILBOX       (* MT_CreateMailbox)(unsigned long messagelen0, unsigned long messageslots0, char * name0);
    long             (* MT_DeleteMailbox)(MT_MAILBOX mailbox0);
    long             (* MT_ClearMailbox)(MT_MAILBOX mailbox0);
    unsigned long    (* MT_Messages)(MT_MAILBOX mailbox0);
    long             (* MT_Put)(MT_MAILBOX mailbox0, void * data0);
    long             (* MT_PutFront)(MT_MAILBOX mailbox0, void * data0);
    long             (* MT_Get)(MT_MAILBOX mailbox0, void * data0);
    long             (* MT_PutCond)(MT_MAILBOX mailbox0, void * data0);
    long             (* MT_PutFrontCond)(MT_MAILBOX mailbox0, void * data0);
    long             (* MT_GetCond)(MT_MAILBOX mailbox0, void * data0);
    long             (* MT_NextCond)(MT_MAILBOX mailbox0, void * data0);
    long             (* MT_PutTimed)(MT_MAILBOX mailbox0, void * data0, unsigned long timeout0);
    long             (* MT_PutFrontTimed)(MT_MAILBOX mailbox0, void * data0, unsigned long timeout0);
    long             (* MT_GetTimed)(MT_MAILBOX mailbox0, void * data0, unsigned long timeout0);

    // richi, version >= 0x00010002
    long             (* MT_GetTaskPrioByID)(unsigned long ulTaskID);

}LSL_MT_TYPE;



#endif
