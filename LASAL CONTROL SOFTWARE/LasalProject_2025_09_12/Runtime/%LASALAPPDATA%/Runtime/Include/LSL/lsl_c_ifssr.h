#ifndef _LSL_IFSSR_H_
#pragma once
#define _LSL_IFSSR_H_

typedef struct _SYSTIME 
{
    unsigned short wHour; 
    unsigned short wMinute; 
    unsigned short wSecond; 
} SYSTIME;

typedef struct _SYSDATE 
{
    unsigned short wYear; 
    unsigned short wMonth; 
    unsigned short wDay; 
    unsigned short wDayOfWeek; 
} SYSDATE;

typedef struct _LSLSYSTEMTIME
{
    unsigned short wYear; 
    unsigned short wMonth; 
    unsigned short wDay; 
    unsigned short wDayOfWeek; 
    unsigned short wHour; 
    unsigned short wMinute; 
    unsigned short wSecond; 
    unsigned short wMilliSecond; 
}LSLSYSTEMTIME;

typedef struct _LSLTIMESTAMP
{
  unsigned long    secondPart;
  long             seconds;
} LSLTIMESTAMP;

// defines for TaskTypes of Objects
#define OBJ_CT 1  // cyclic
#define OBJ_RT 2  // realtime
#define OBJ_TT 3  // foreground
#define OBJ_BG 4  // background
#define OBJ_HP 5  // high-priority user

#pragma pack(push, 4)

typedef struct
{
    unsigned long lEvents;         // count of events (times it has run)
    unsigned long lTime;           // last execution time
    unsigned long lAve;            // average execution time (ave. of last 10)
    unsigned long lMin;            // min execution time (in uSec)
    unsigned long lMax;            // max execition time (in uSec)
} OBJTIME_MEAS;

typedef struct {
   long TotalFree;
   long TotalUsed;
   long FreeBlocks;
   long UsedBlocks;
   unsigned long LargestFree;
} AppHeapInfoRec;


typedef struct
{
  unsigned long      lastcall;    // uSec time since last call (for display)
  unsigned long      orgperiod;   // original Task execution time in tickcounts
  unsigned long      period;      // Task execution time in tickcounts
  unsigned long      orgstatus;   // Task status defintions
  unsigned long      status;      // Task status defintions
  OBJTIME_MEAS       statistics;  // Task time structure
} LSLOBJ_INFO;

#pragma pack(pop)


typedef struct
{
  unsigned long diasstation;
  unsigned long flashcode;
  unsigned long flashlen;
  unsigned long blocklen;
  unsigned long segmentlen;
}LSLPROMINFO_TYPE;

#define LSLPROMERROR_NONE          0
#define LSLPROMERROR_NODIAS       -1
#define LSLPROMERROR_IOADDR       -2
#define LSLPROMERROR_DEVID        -3
#define LSLPROMERROR_NOMOREMEM    -4
#define LSLPROMERROR_NOINFO       -5

#define LSLPROMERROR_ADDR        -10
#define LSLPROMERROR_PROG        -11
#define LSLPROMERROR_LENISNULL   -12
#define LSLPROMERROR_LENTOOSMALL -13

#define LSLPROMDEVCODE_128K  0xd5 //device code
#define LSLPROMDEVCODE_256K  0xda
#define LSLPROMDEVCODE_512K  0xa4
#define LSLPROMDEVCODE_1024K 0xFF
//**HH
// ist zwar ein Blödsinn, aber nicht besser zu lösen!
#define LSLPROMDEVCODE_2048K     0xFE


//
//    System Service Routine Structure
//
//    The following structure is the LSL_SSR Interface Structure.  The structure
//    provides the API for the LSL_SSR module.
//
typedef struct
{    
    // System Service Function
    int   (* SSR_InstallIRQ)(unsigned short IRQNumber0, 
                           void *IRQFunction0);

    int   (* SSR_InstallIRQObject)(unsigned short IRQNumber0, 
                           void *IRQFunction0);

    void *(* SSR_CreateMailbox)(char *MailboxName0, 
                              unsigned MailboxDataLen0, 
                              unsigned MailboxSlots0);
    int     (* SSR_MailboxGet)(void *Mailbox0,  
                           void *pMailboxData0);
    int      (* SSR_MailboxPut)(void *Mailbox0, 
                           void *pMailboxData0);

    int   (* SSR_MailboxDelete)(void *Mailbox0);
  
  
    void *(* SSR_CreateSemaphore)(char *SemName0, unsigned short SemType0);

    int   (* SSR_SemSignal)(void *Semaphore0);
    int   (* SSR_SemPulse)(void *Semaphore0);
    int   (* SSR_SemWait)(void *Semaphore0);
    int   (* SSR_SemWaitCond)(void *Semaphore0);
    int   (* SSR_SemDelete)(void *Semaphore0);

    void    (* SSR_OutW)(unsigned short PortNumber0, 
                     unsigned short Value0);
    void    (* SSR_OutB)(unsigned short PortNumber0, 
                     unsigned short Value0);
    unsigned short (* SSR_InpW)(unsigned short PortNumber0);
    unsigned short (* SSR_InpB)(unsigned short PortNumber0);

// ...............................................................................................
// ... function to give user mem read/write access
// ...............................................................................................
    int   (* SSR_MemoryAccess)(unsigned long Addr0, unsigned long Length0);

    short int (* SSR_CanTxObj)(unsigned char, short int, unsigned char, void* );
    int   (* SSR_AddCanObj)(unsigned char, short int, unsigned char, unsigned char, void*, void*);

    void *(* SSR_Malloc)(unsigned long size0);
    void  (* SSR_Free)(void *ptr0);

    void  (* SSR_SetSysTime)(SYSTIME* time0);
    void  (* SSR_SetSysDate)(SYSDATE* date0);
    void  (* SSR_GetSysTime)(SYSTIME* time0);
    void  (* SSR_GetSysDate)(SYSDATE* date0);

    void *(* SSR_ReAlloc)(void*, unsigned long);

    int   (* SSR_InstallIRQLASAL)(unsigned short IRQNumber0, void *IRQFunction0, void* thispointer0);

    void* (* pMemMove)(void *d, void *s, unsigned long size);
    void* (* pMemCpy)(void *d, void *s, unsigned long size);
    void* (* pMemSet)(void *p, unsigned char c, unsigned long size);
    unsigned short (* pMemCmp)(void *d, void *s, unsigned long size);

    unsigned short (* pSSR_LoginIntoCANNew)(unsigned char cannr0,unsigned char canstation0, short int txobjnr0);

    unsigned long (* SSR_GetObjectInfo)(void* thisobj0, unsigned long tasktype0, LSLOBJ_INFO* objinfo0);
    void* (* malloc)(unsigned long heapId, unsigned long size);
    void  (* free)(unsigned long heapId, void *memblock);
    void* (* realloc)(unsigned long heapId, void *memblock, unsigned long size);
    void  (* KernelLog)(char *msg0, ...);

    void  (* pSSR_Delay)(unsigned long msecs0);
    void  (* InstallDiasHandler)(void *IntFunction0, void* param0);
    //14.10.2002: rc not supported
    //5.59f supported again
    long  (* AddToKernelServiceProvider)(char *cmd0, unsigned long immedialty0);
    int   (* SetHandler)(int type, void *fct, void *param); //pr, 1.1.18
    int   (* SkipOverIDIV)(unsigned long exceptionCode,     //pr, 1.1.26
                           void *exceptionInfo,
                           unsigned long quotient,
                           unsigned long remainder);
    unsigned long (* EE_Read)(unsigned short addr0, unsigned short *data0);    // for PLC Seriel EEProm read
    unsigned long (* EE_Write)(unsigned short addr0, unsigned short data0);    // for PLC Seriel EEProm write
    unsigned long (* ButtonPressed)(void);                             // for PLC Check Button Pressed
    unsigned long (* SetTimerIntVal)(unsigned long ClockTicks0);
    unsigned long (* GetTimerIntVal)(void);
    void (* SSR_SetCompatLevel)(unsigned short level);
    unsigned long sizeofStruct; // 01.01.080
    unsigned long (* SSR_CurrentTaskHandle)();

    // EPROM on Dias PLC Socket => Dias Station 62
    long (* PROMGetInfo_Socket)(LSLPROMINFO_TYPE *info0);
    long (* PROMSetData_Socket)(unsigned long addr0, void *data0, unsigned long datalen0);
    long (* PROMGetData_Socket)(void *data0, unsigned long addr0, unsigned long datalen0);
    // EPROM on DiasBus as DIAS Modul => Dias Station 61
    long (* PROMGetInfo_Modul)(LSLPROMINFO_TYPE *info0);
    long (* PROMSetData_Modul)(unsigned long addr0, void *data0);
    long (* PROMGetData_Modul)(void *data0, unsigned long addr0);
   
    // FLASHPROM ONBOARD 
    long (* PROMGetInfo_Flash)(LSLPROMINFO_TYPE *info0);
    long (* PROMSetData_Flash)(unsigned long addr0, void *data0, unsigned long datalen0);
    long (* PROMGetData_Flash)(void *data0, unsigned long addr0, unsigned long datalen0);

    int  (* InitBasicCanObj)(unsigned char cannr , short int objnr, 
			       short int mask,void* actionptr, void* thispointer);  

    unsigned long (* SetBreakPoint)(int nr, 
                                    unsigned long addr, 
                                    unsigned long len, 
                                    int type,
                                    unsigned long stackOfs, 
                                    unsigned long stackSize, 
                                    unsigned long cntr, 
                                    int action, 
                                    char *pCode,
                                    unsigned long codeLen,
                                    long allowDataBpInSystemtask);
    void (* pDummy32)(void);

    // 01.01.025
    void * (* pAppHeapAllocV1)(unsigned long size, unsigned char Kennung);
    void * (* pAppHeapReAllocV1)(void* space0, unsigned long newsize0, unsigned char Kennung);
    void * (* pMallocV1)(unsigned long heapId, unsigned long size, unsigned char Kennung);
    void * (* pReallocV1)(unsigned long heapId, void *memblock, unsigned long size, unsigned char Kennung);

    // 01.01.079
    void (* SSR_Reboot)( void );

    // 01.01.80
    int (* ReadDateTime)(/*LSLSYSTEMTIME*/void *systemTime);
    int (* SetDateTime)(/*LSLSYSTEMTIME*/void *systemTime);
    void (* DateTime2Timestamp)(/*LSLSYSTEMTIME*/void *systemTime, LSLTIMESTAMP *ts);
    void (* Timestamp2DateTime)(/*LSLSYSTEMTIME*/void *systemTime, LSLTIMESTAMP *ts);
    void (* AdjustDateTime)(LSLTIMESTAMP *timestampDiff);
    void (* CompareDateTime)(/*LSLSYSTEMTIME*/void *sysTime1, /*LSLSYSTEMTIME*/void *sysTime2, 
                               LSLTIMESTAMP *timestampDiff);
    void (* TimestampAdd)(LSLTIMESTAMP *tsSumme, LSLTIMESTAMP *tsSummand1, LSLTIMESTAMP *tsSummand2);
    void (* TimestampSub)(LSLTIMESTAMP *tsDifferenz, LSLTIMESTAMP *tsMinuend, LSLTIMESTAMP *tsSubtrahend);
    void (* TimestampDiv)(LSLTIMESTAMP *tsQuotient, LSLTIMESTAMP *tsDividend, unsigned long divisor);
    void (* TimestampMult)(LSLTIMESTAMP *tsProdukt, LSLTIMESTAMP *tsFaktor, unsigned long faktor);
    
    void (* SSR_Reset)( void );
    void (* SSR_ResetApp)();
    long (* SSR_SetDIASError)(long enable);

    // 01.01.098
    void *(* GetSysvar)();

    // since 01.01.144
    void (* HeapOutOfMemCallback)(void *pThis, void *pCallback);

    // 01.01.156
    long (*GetDivErrorInfo)( void *pExceptionInfo, unsigned long *pDividend, unsigned long *pDividendHigh );
    long (*GetModuleInfoByIp)( unsigned long *pInstPtr, char *pModuleName, unsigned long ulSizeOfModuleName, char *pFuncName, unsigned long ulSizeOfFuncName, unsigned long *pModuleOffset );

    // ab 01.01.185
    void* (*pAppHeapMarkAllocV1)(void *space, unsigned long size, unsigned char Kennung);
    void* (*pAppHeapShrinkAlloc)(void* space0, unsigned long newsize0);

    // ab 01.02.195
    void (* SSR_ResetRunApp)();

} LSL_SSR;






#ifdef _RTOS_GLOBALS_H_
//
//    Common LASAL OS Layer Structures and Definitions
//


//
//
//    LSL_OSDATA: LASAL O/S Data Structure
//
//        The following structure is the LASAL OS Data Structure.  This is the main data
//        structure used by the LASAL OS.  It contains various entries common to various
//        LASAL OS tasks, services and functions.
//
//
//
//
typedef struct
{
    // Kernel Layer Data
    void* piCIL;            // a pointer to the CIL interface from the kernel

    // DIAS Device Data
    void* piDias;            // pointer to the main DIAS Bus master device interface

    // DLED Device Interface
    void* piDLed;            // LED device
    
    // User Memory Area Definition
    void* pUser;            // pointer to user mem def struct interface
    
    // Pointer to System Time Interface
    void* piSysTime;    // System Time interface

    // Pointer to System Queue Interface
    void* piSysQueue;    // System Queue interface

    // Pointer to LASAL Task Interface
    void* piTask;

    // Pointer to LASAL Task Interface
    void* piCRT;

    // Pointer to LASAL System Service Interface  // by CGCS
    LSL_SSR* piSSR;

    // Pointer to LASAL System Diagnostics Interface
    void* piDiag;

} LSL_OSDATA;

//
//    Global Data References
//
extern LSL_OSDATA* lsl_pOS;        // pointer to LASAL O/S Data Structure
extern LSL_OSDATA* LSL_POS;        // pointer to LASAL O/S Data Structure for APPLICATIONS

//
//    SSR Interface Macros for C
//

//LSL_SSR *lsl_cos = _lsl_pOS->piSSR;

#define OS_SSR_InstallIRQ LSL_POS->piSSR->SSR_InstallIRQ

#define OS_SSR_CreateMailbox LSL_POS->piSSR->SSR_CreateMailbox
#define OS_SSR_MailboxGet LSL_POS->piSSR->SSR_MailboxGet
#define OS_SSR_MailboxPut LSL_POS->piSSR->SSR_MailboxPut
#define OS_SSR_MailboxDelete LSL_POS->piSSR->SSR_MailboxDelete

#define OS_SSR_CreateSemaphore LSL_POS->piSSR->SSR_CreateSemaphore
#define OS_SSR_SemSignal LSL_POS->piSSR->SSR_SemSignal
#define OS_SSR_SemPulse LSL_POS->piSSR->SSR_SemPulse
#define OS_SSR_SemWait LSL_POS->piSSR->SSR_SemWait
#define OS_SSR_SemWaitCond LSL_POS->piSSR->SSR_SemWaitCond
#define OS_SSR_SemDelete LSL_POS->piSSR->SSR_SemDelete

#define OS_SSR_OutW LSL_POS->piSSR->SSR_OutW
#define OS_SSR_OutB LSL_POS->piSSR->SSR_OutB
#define OS_SSR_InpW LSL_POS->piSSR->SSR_InpW
#define OS_SSR_InpB LSL_POS->piSSR->SSR_InpB

#define OS_SSR_MemoryAccess LSL_POS->piSSR->SSR_MemoryAccess

#define OS_SSR_Malloc LSL_POS->piSSR->SSR_Malloc
#define OS_SSR_Free LSL_POS->piSSR->SSR_Free
#define OS_SSR_Realloc LSL_POS->piSSR->SSR_ReAlloc

#define OS_SSR_EE_Read LSL_POS->piSSR->EE_Read
#define OS_SSR_EE_Write LSL_POS->piSSR->EE_Write
#define OS_SSR_ButPressed LSL_POS->piSSR->ButtonPressed

#define OS_SSR_SetTimerIntVal LSL_POS->piSSR->SetTimerIntVal
#define OS_SSR_GetTimerIntVal LSL_POS->piSSR->GetTimerIntVal

#define OS_SSR_InstallDiasHandler LSL_POS->piSSR->InstallDiasHandler

#endif


//
//
//    End of File
//
#endif    // end of #define LSL_IFSSR_H
