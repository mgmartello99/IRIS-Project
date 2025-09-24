// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Version     Date    Author     Comment
//     1.0               Mr. X      Creating this file
//     2.0     29072002  Menk       Terminal Functions for i386, Shuttle Wheel, Key, LEDs
//
// ------------------------------------------------------------



#pragma once


#include "SysCRT.H"

#ifdef __cplusplus
 #define EXTERN_C extern "C"
#else
 #define EXTERN_C extern
#endif

#ifndef __OS_FILE_IO
 #define __OS_FILE_IO


#include ".\RTOS\RTOS_C_DDE_Type.h"

// Struktur zum vergleich
typedef struct tagDriverList
  {
  // Zeiger
  void  *Driver;
  // kurze ID
  int   ID;
  // Name
  char  *DriverName;
  } DriverList;

typedef void (* RTFFormatCallback)(const char * DeviceName, int Action, unsigned long Total, unsigned long Completed);
typedef unsigned long (* RTFCheckDiskCallback)(unsigned long Flag, const char * Path, const _DDE_INFO * FileInfo, unsigned long N1, unsigned long N2);
typedef void (* RTFFormatCallback4Lasal)(const char * DeviceName, int Action, unsigned long Total, unsigned long Completed, void *objThisPtr);


typedef struct
{
  char DeviceName[32];
  long Action;
  unsigned long Total;
  unsigned long Completed;

#define SFR_FORMAT_PROGRESS_READY   0   // READY, bereit für Formatierung
#define SFR_FORMAT_PROGRESS_BUSY    1   // BUSY,  laufende Formatierung
#define SFR_FORMAT_PROGRESS_DONE    2   // DONE,  Formatierung erfolgreich beendet
#define SFR_FORMAT_PROGRESS_ERROR   3   // ERROR, Formatierung nicht erfolgreich beendet

  unsigned long Status;
  long RetCode;

} SFR_FORMAT_PROGRESS_INFO;

#define SFR_FORMAT_OPTION_DEFCB     0x01  // Default Format Callback wird verwendet
#define SFR_FORMAT_OPTION_CURRSPC   0x02  // Verwende aktuellen Wert für SectorsPerCluster

typedef struct 
{
    unsigned long (* pSFR_Fread)(int, void*, unsigned long);
    unsigned long (* pSFR_Fwrite)(int, void*, unsigned long);
    int   (* pSFR_Fopen)(char *, unsigned long, unsigned long);
    void  (* pSFR_Fclose)(int);
    long  (* pSFR_Fdel)(char*);
    long  (* pSFR_Flseek)(int, unsigned long, unsigned long);
    long  (* pSFR_Ftell)(int);
    long  (* pSFR_Ffilelength)(int);
    int   (* pSFR_CreateDir)(char*);
    long  (* pSFR_RemoveDir)(char*);
    long  (* pSFR_FindFirst)(char*, char*, _DDE_INFO*, unsigned char, unsigned char);
    int   (* pSFR_FindNext)(long, _DDE_INFO*);
    int   (* pSFR_FindClose)(long);
    void  (* pSFR_CloseAllOpenFiles)(void);
    long  (* pSFR_Rename)(char*, char*);

    long  (* pSFR_FopenX)(char*, unsigned long);
    long  (* pSFR_Fwrite_X)(int hand, void *dest, unsigned long size);
    void  (* pOS_SetTell)(void *);
    int   (* pSFR_GetDriveListShort)( char *);
    void  (* SFR_SetDriverList)( DriverList* pDriverList );
    DriverList*  (*SFR_GetDriverList)( void );
    long  (* SFR_FExtend)( int hand, unsigned long size);
    long  lAppDefOpenFlags;
    void  (* SFR_SetDefaultOpenFlags)( long lFlags );

	  unsigned long (* pSFR_Fread_A)(int, int, void*, unsigned long);
	  unsigned long (* pSFR_Fwrite_A)(int, int, void*, unsigned long);
	  int   (* pSFR_Fopen_A)(int, char*, unsigned long);  
	  void  (* pSFR_Fclose_A)(int, int);
	  long  (* pSFR_Fdel_A)(int, char*);
	  long  (* pSFR_Flseek_A)(int, int, unsigned long, unsigned long);
	  long  (* pSFR_Ftell_A)(int, int);
	  long  (* pSFR_Ffilelength_A)(int, int);
	  int   (* pSFR_CreateDir_A)(int, char*);
	  long  (* pSFR_RemoveDir_A)(int, char*);
	  long  (* pSFR_FindFirst_A)(int, char*, char*, _DDE_INFO*, unsigned char, unsigned char);
	  int   (* pSFR_FindNext_A)(int, long, _DDE_INFO*);
	  int   (* pSFR_FindClose_A)(int, long);
	  void  (* pSFR_CloseAllOpenFiles_A)(int);
    long  (* pSFR_Rename_A)(int, char*, char*);
    long  (* SFR_GetState)( unsigned int iID, long *plErg);
    long  (* pSFR_Ftruncate)(int);
    long  (* pSFR_Ftruncate_A)(int);

    long  (* pSFR_FindFirstEx)(char *, unsigned char, unsigned char, _DDE_INFO *, char *, unsigned long);
    int   (* pSFR_FindNextEx)(long , _DDE_INFO *, char *, unsigned long);
    long  (* pSFR_FindFirstEx_A)(int, char *, unsigned char, unsigned char, _DDE_INFO *, char *, unsigned long);
    int   (* pSFR_FindNextEx_A)(int, long, _DDE_INFO *, char *, unsigned long);

    long  (* pSFR_FileCopy)(char *source0, char *dest0, unsigned long option0);

    int   (* SFR_GetAttributes)( char *FileName0, unsigned char *Attributes);
    int   (* SFR_SetAttributes)( char *FileName0, unsigned char Attributes );
    int   (* SFR_GetDiskSpace)( char *Drive, unsigned long *pBpS, unsigned long *pSpC, unsigned long *pTC, unsigned long *pFC );

    // 01.01.025 - 17.01.2005
    long  (* pSFR_FreadV1)(int handle0, void *dest, unsigned long size0);
    long  (* pSFR_Fread_AV1)(int iA, int handle0, void *dest, unsigned long size0);
    long  (* pSFR_Fwrite_AV1)(int iA, int handle0, void *dest, unsigned long size0);

	  // 01.01.056
    long  (* pSFR_CreateRAMDisk)(unsigned char * pRAMDiskData, unsigned long ulRAMDiskSize, unsigned char * ucDriveLetter);
    long  (* pSFR_DestroyRAMDisk)(unsigned char ucDriveLetter);

    // 01.01.063
    long  (* pSFR_FileCopy_A)(int iA, char *source0, char *dest0, unsigned long option0);

    // 01.01.067
    long  (* pSFR_Format)(const char * ccDriveName, unsigned long ulMinSectorsPerCluster, RTFFormatCallback ProgressCallback, unsigned long ulOption, unsigned long ulFlags);
    long  (* pSFR_FormatProgressInfo)(SFR_FORMAT_PROGRESS_INFO * pFormatProgressInfo, unsigned long ulLength, unsigned long ulReset);

    // 01.01.078
    int   (* pSFR_GetDiskSpace_A)(int iA, char *Drive, unsigned long *pBpS, unsigned long *pSpC, unsigned long *pTC, unsigned long *pFC);
    long  (* pSFR_GetDriveListShort_A)(int iA, char* Erg);
    long  (* pSFR_GetDriveListShortEx)(unsigned long * Buffer, unsigned long BufferSize, unsigned long ulOption);
    long  (* pSFR_GetDriveListShortEx_A)(int iA, unsigned long * Buffer, unsigned long BufferSize, unsigned long ulOption);
    long  (* pSFR_CheckDisk)(char * DriveName, RTFCheckDiskCallback ErrorHandler, unsigned long ulOption, unsigned long ulFlags, unsigned long * ulCheckDiskAgain, unsigned long * ulStatus);
    long  (* pSFR_CheckDisk_A)(int iA, char * DriveName, RTFCheckDiskCallback ErrorHandler, unsigned long ulOption, unsigned long ulFlags, unsigned long * ulCheckDiskAgain, unsigned long * ulStatus);
    long  (* pSFR_SRamLoad)(const char * filename);
    long  (* pSFR_SRamSave)(const char * filename);
    long  (* pSFR_FormatDrive)(const char * ccDriveName, unsigned long ulMinSectorsPerCluster, RTFFormatCallback4Lasal ProgressCallback, unsigned long ulOption, unsigned long ulFlags, void *objThisPtr);

    // 01.02.045
    int   (* pSFR_SetVolumeLabel)(char * DriveName, char * VolumeLabel);
    int   (* pSFR_GetVolumeLabel)(char * DriveName, char * VolumeLabel);

    // 01.02.070
    int   (* pSFR_GetFATType)(char * DriveName, unsigned long * ulFATType);

    // 01.02.086
    long  (* pSFR_SRamLoadV1)(const char * filename, unsigned short cmd_after_load);

    // 01.02.195
    long  (* pSFR_CreateFile)(const char *dpne, const unsigned long ulFileSize, const unsigned char ucPattern, const unsigned long ulAttr);
    long  (* pSFR_CreateFile_A)(int iA, const char *dpne, const unsigned long ulFileSize, const unsigned char ucPattern, const unsigned long ulAttr);
    
    // 01.02.225
    long  (* pSFR_SplitNameExtension)(const char* const apucNameExtension, char* const apucName, const unsigned long ulMaxNameLen, char* const apucExtension, const unsigned long ulMaxExtensionLen);

    // 01.03.015
    long  (* pSFR_RemoveDir_Full)(char*);
    long  (* pSFR_RemoveDir_Full_A)(int, char*);

    // 01.03.141
    int   (* pSFR_GetVolumeSerialNumber)(char * DriveName, unsigned int * VolumeSerialNumber);

} OS_IFILE;


#endif // __OS_FILE_IO

typedef enum
{
    SYS_ERR_NONE,        // No error, OK
    SYS_ERR_NOMEM,        // Not enough memory
    SYS_ERR_IEXISTS,    // Interface exists
    SYS_ERR_INOEXISTS,    // Interface does not exist (not found)
    SYS_ERR_INOREF,        // Interface ref. count error (interface out of refs)
    SYS_ERR_IMODNUM,    // Invalid Module Number (DIAS address)
    SYS_ERR_DIASWORD,    // DIAS WORD access error
    SYS_ERR_DIASBYTE,    // DIAS BYTE access error
    SYS_ERR_DIASCTRL,    // DIAS CTRL access error
    SYS_ERR_DIASIRQ,    // DIAS IRQ error
    SYS_ERR_MAPMEM,        // RT Kernel Memory Mapping Error
    SYS_ERR_IPROJ,        // Invalid "user" project
    SYS_ERR_BAUD,        // Invalid baud rate
    SYS_ERR_STN            // Invalid station address
}_SYS_ERROR;

//
//  CIL Definitions
//
#define SYS_CIL_UNLIMITED  -1    // Unlimted interface ref's allowed
#define SYS_CIL_EXCLUSIVE  1    // Exclusive (single) ref allowed


//
//  Kernel Interfaces
//
//  CIL: Common Interface Library
//
//    This structure is used as the CIL Interface structure.  The
//    structure exposes the CIL API.
//
typedef struct
{
  // CIL Interface
  _SYS_ERROR (* CILRegister)(char *pName, void *pInterface, unsigned short wRefLimit);
  _SYS_ERROR (* CILGet)(char *pName, void **pInterface);
  _SYS_ERROR (* CILRelease)(char *pName);
} _SYS_ICIL;



//
//    Common LASAL OS Layer Structures and Definitions
//
//    LSL_OSDATA: LASAL O/S Data Structure
//
//        The following structure is the LASAL OS Data Structure.  This is the main data
//        structure used by the LASAL OS.  It contains various entries common to various
//        LASAL OS tasks, services and functions.
//
typedef struct
{
 // Kernel Layer Data
    _SYS_ICIL* piCIL;            // a pointer to the CIL interface from the kernel

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
    #ifdef LSL_ITASK_H
        LSL_ITASK* piTask;
    #else
        void* piTask;
    #endif

 // Pointer to C Runtime Tinterface
    SYS_ICRT* piCRT;

 // Pointer to LASAL System Service Interface  // by CGCS
    #ifdef _LSL_IFSSR_H_
      LSL_SSR* piSSR; 
    #else
      void* piSSR;
    #endif

 // LASAL Diagnostic Interface Information
    void* piDiag;            // diagnostic Interface information

 // LASAL Linker Information 
    #ifdef LSL_IFLINKER_H
      LSL_LINKER* piLinker;
    #else
      void* piLinker;
    #endif

 // LASAL File Interface (V3.18)
#ifndef __IFFILE_C_H
    void* piLslFile;        // interface is OPTIONAL and may not exist!
#else
    SYS_IFILE* piLslFile;        // interface is OPTIONAL and may not exist!
#endif
 // LASALCOM Interface Pointer (V3.23)
    void* piLASALCOM;

 // Pointer to MAE/LASAL Sreen Editor Interface (V3.45)
    #ifdef LSE_IF
      IMAE* piLSE;
    #else
      void* piLSE;
    #endif

    OS_IFILE* piFILE;

    // OS-Terminal interface pointer (v4)
    void* piTerminal;

    // Printer API interface pointer (v4)
    void* piPrinter;

    // FTP API
    void* piFTP;

} LSL_OSDATA;

//
//    Global Data References
//
EXTERN_C LSL_OSDATA* lsl_pOS;        // pointer to LASAL O/S Data Structure
EXTERN_C LSL_OSDATA* LSL_POS;        // pointer to LASAL O/S Data Structure for APPLICATIONS


#ifdef __IFFILE_C_H
  #include "file_c.h"
#endif


//
//  MACRO Defines for the used Interfaces

#ifdef _LSL_IFSSR_H_
//    SSR Interface Macros for C
//
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

#define OS_SSR_Malloc   LSL_POS->piSSR->SSR_Malloc
#define OS_SSR_Free     LSL_POS->piSSR->SSR_Free
#define OS_SSR_Realloc  LSL_POS->piSSR->SSR_ReAlloc

#define OS_SSR_SetSysTime LSL_POS->piSSR->SSR_SetSysTime
#define OS_SSR_SetSysDate LSL_POS->piSSR->SSR_SetSysDate
#define OS_SSR_GetSysTime LSL_POS->piSSR->SSR_GetSysTime
#define OS_SSR_GetSysDate LSL_POS->piSSR->SSR_GetSysDate

#define OS_SSR_Fread LSL_POS->piSSR->SSR_Fread
#define OS_SSR_Fwrite LSL_POS->piSSR->SSR_Fwrite
#define OS_SSR_Fopen LSL_POS->piSSR->SSR_Fopen
#define OS_SSR_Fclose LSL_POS->piSSR->SSR_Fclose
#define OS_SSR_Flseek LSL_POS->piSSR->SSR_Flseek
#define OS_SSR_Ftell LSL_POS->piSSR->SSR_Ftell
#define OS_SSR_Ffilelength LSL_POS->piSSR->SSR_Ffilelength

#define OS_SSR_InstallIRQLASAL LSL_POS->piSSR->SSR_InstallIRQLASAL

#define OS_SSR_MemMove LSL_POS->piSSR->pMemMove
#define OS_SSR_MemCpy LSL_POS->piSSR->pMemCpy
#define OS_SSR_MemSet LSL_POS->piSSR->pMemSet
#define OS_SSR_MemCmp LSL_POS->piSSR->pMemCmp


#define OS_SSR_PROMDataSave LSL_POS->piSSR->PROMSaveData
#define OS_SSR_PROMDataLoad LSL_POS->piSSR->PROMLoadData

#define OS_SSR_MallocV1  LSL_POS->piSSR->AppHeapAllocV1
#define OS_SSR_ReallocV1 LSL_POS->piSSR->AppHeapReAllocV1

#endif


#ifdef LSE_IF
//    MAE Interface Macros for C
//
#define MAE_API LSL_POS->piLSE

#define MemMove lsl_pOS->piLSE->pLSEOS_MemMove
#define MemCpy lsl_pOS->piLSE->pLSEOS_MemCpy
#define MemSet lsl_pOS->piLSE->pLSEOS_MemSet
#define MemCmp lsl_pOS->piLSE->pLSEOS_MemCmp
#define get_abstime lsl_pOS->piLSE->pLSEOS_GetAbsTime

#define Sys_SetTime lsl_pOS->piLSE->pLSEOS_SetTime
#define Sys_GetTime lsl_pOS->piLSE->pLSEOS_GetTime

#define Fread lsl_pOS->piLSE->pLSEOS_Fread
#define Fwrite lsl_pOS->piLSE->pLSEOS_Fwrite
#define Fopen lsl_pOS->piLSE->pLSEOS_Fopen
#define Fclose lsl_pOS->piLSE->pLSEOS_Fclose
#define Flseek lsl_pOS->piLSE->pLSEOS_Flseek
#define Ftell lsl_pOS->piLSE->pLSEOS_Ftell
#define Ffilelength lsl_pOS->piLSE->pLSEOS_Ffilelength
#define CreateDir lsl_pOS->piLSE->pCreateDir
#define FindFirst lsl_pOS->piLSE->pOS_FindFirst
#define FindNext lsl_pOS->piLSE->pOS_FindNext
#define FindClose lsl_pOS->piLSE->pOS_FindClose

#define Sys_GetSRAMInfo lsl_pOS->piLSE->pLSEOS_GetSRAMInfo
#define Sys_GetPointerToSRAM lsl_pOS->piLSE->pLSEOS_GetPointerToSRAM
#define Sys_GetPointerToKeyboard lsl_pOS->piLSE->pLSEOS_GetPointerToKeyboard
#define Sys_SetTime lsl_pOS->piLSE->pLSEOS_SetTime
#define Sys_GetTime lsl_pOS->piLSE->pLSEOS_GetTime
#define OS_Wait lsl_pOS->piLSE->pLSEOS_Wait

#define SYSMAE_GetAddressVar lsl_pOS->piLSE->pLSECOM_GetAddressVar
#define SYSMAE_GetAddressObj lsl_pOS->piLSE->pLSECOM_GetAddressObj
#define SYSMAE_RegisterVar lsl_pOS->piLSE->pLSECOM_RegisterVar
#define SYSMAE_RegisterObj lsl_pOS->piLSE->pLSECOM_RegisterObj
#define SYSMAE_DeleteQueue lsl_pOS->piLSE->pLSECOM_DeleteQueue
#define SYSMAE_Login lsl_pOS->piLSE->pLSECOM_Login
#define SYSMAE_SetExternFunction lsl_pOS->piLSE->pSYSMAE_SetExternFunction
#define SYSMAE_SetData lsl_pOS->piLSE->pLSECOM_SetData
#define SYSMAE_GetData lsl_pOS->piLSE->pLSECOM_GetData
#define SYSMAE_MetaEvent lsl_pOS->piLSE->pLSEOS_MetaEvent

#define  GetPalette lsl_pOS->piLSE->pGetPalette
#define  HideMouse lsl_pOS->piLSE->pHideMouse
#define  ShowMouse lsl_pOS->piLSE->pShowMouse
#define  IfNecHideMouse lsl_pOS->piLSE->pIfNecHideMouse
#define  IfNecShowMouse lsl_pOS->piLSE->pIfNecShowMouse
#define  SetFontAttribute lsl_pOS->piLSE->pSetFontAttribute
#define  StrWidth  lsl_pOS->piLSE->pStrWidth
#define  InitGraph  lsl_pOS->piLSE->pInitGraph
#define  CloseGraph  lsl_pOS->piLSE->pCloseGraph
#define  GetScreenSize  lsl_pOS->piLSE->pGetScreenSize
#define  GetPixel lsl_pOS->piLSE->pGetPixel
#define  PutPixel lsl_pOS->piLSE->pPutPixel
#define  ReadPixel lsl_pOS->piLSE->pReadPixel
#define  WritePixel lsl_pOS->piLSE->pWritePixel
#define  Line lsl_pOS->piLSE->pLine
#define  Polygon lsl_pOS->piLSE->pPolygon
#define  Rectangle lsl_pOS->piLSE->pRectangle
#define  Bar lsl_pOS->piLSE->pBar
#define  OutTextXY lsl_pOS->piLSE->pOutTextXY
#define  OutTextLen lsl_pOS->piLSE->pOutTextLen
#define  Arc lsl_pOS->piLSE->pArc
#define  Oval lsl_pOS->piLSE->pOval
#define  Button lsl_pOS->piLSE->pButton
#define  Scroll lsl_pOS->piLSE->pScroll
#define  Bitmap lsl_pOS->piLSE->pBitmap
#define  ActivateFont lsl_pOS->piLSE->pActivateFont
#define  GetFontSize lsl_pOS->piLSE->pGetFontSize
#define  FreeRegion lsl_pOS->piLSE->pFreeRegion
#define  ActivateRegion lsl_pOS->piLSE->pActivateRegion
#define  DeactivateRegion lsl_pOS->piLSE->pDeactivateRegion
#define  StartRegion lsl_pOS->piLSE->pStartRegion
#define  AddRegion lsl_pOS->piLSE->pAddRegion
#define  EndRegion lsl_pOS->piLSE->pEndRegion
#define  SubRegion lsl_pOS->piLSE->pSubRegion
#define  AndRegion lsl_pOS->piLSE->pAndRegion
#define  XorRegion lsl_pOS->piLSE->pXorRegion
#define  OrRegion lsl_pOS->piLSE->pOrRegion
#define  GetActiveRegion lsl_pOS->piLSE->pGetActiveRegion
#define  GetNoColors lsl_pOS->piLSE->pGetNoColors

#define  GetImage lsl_pOS->piLSE->pGetImage
#define  PutImage lsl_pOS->piLSE->pPutImage
#define  GetRetrace lsl_pOS->piLSE->pGetRetrace
#define  WaitForRetrace lsl_pOS->piLSE->pWaitForRetrace
#define  IsGraphInit lsl_pOS->piLSE->pIsGraphInit
#define  Memory lsl_pOS->piLSE->pMemory
#define  OutXPixelPool lsl_pOS->piLSE->pOutXPixelPool
#define  OutYPixelPool lsl_pOS->piLSE->pOutYPixelPool
#define  OutPixelLine lsl_pOS->piLSE->pOutPixelLine
#define  RoomInRegion lsl_pOS->piLSE->pRoomInRegion
#define  MakeTrueColor lsl_pOS->piLSE->pMakeTrueColor

#define OS_ShowCursor lsl_pOS->piLSE->pOS_ShowCursor
#define OS_MoveCursor lsl_pOS->piLSE->pOS_MoveCursor
#define OS_LimitMouse lsl_pOS->piLSE->pOS_LimitMouse
#define OS_StopMouse lsl_pOS->piLSE->pOS_StopMouse
#define OS_StopEvent lsl_pOS->piLSE->pOS_StopEvent
#define OS_StartEvent lsl_pOS->piLSE->pOS_StartEvent
#define OS_StoreEvent lsl_pOS->piLSE->pOS_StoreEvent
#define OS_KeyEvent lsl_pOS->piLSE->pOS_KeyEvent
#define OS_InitMouse lsl_pOS->piLSE->pOS_InitMouse

//#define MakeColor lsl_pOS->piLSE->pMakeColor

#define OS_MouseCursorStyle lsl_pOS->piLSE->pMouseCursorStyle
#define DestroyOffScreen lsl_pOS->piLSE->pDestroyOffScreen
#define CreateOffScreen lsl_pOS->piLSE->pCreateOffScreen
#define GetScreen lsl_pOS->piLSE->pGetScreen
#define SetScreen lsl_pOS->piLSE->pSetScreen
#define CopyScreen lsl_pOS->piLSE->pCopyScreen

#define LCD_Brightness lsl_pOS->piLSE->pLCD_Brightness
#define LCD_Contrast  lsl_pOS->piLSE->pLCD_Contrast
#define DrawTLBR  lsl_pOS->piLSE->pDrawTLBR

#define SetCalibration lsl_pOS->piLSE->pSetNewCalibration
#define SetTouchKoords lsl_pOS->piLSE->pLSE_SetTouchKoords

#define SetGraphicSystem lsl_pOS->piLSE->pSetGraphicSystem

#define ChrWidth lsl_pOS->piLSE->pChrWidth
#define OutChar lsl_pOS->piLSE->pOutChar
#define TxtWidth lsl_pOS->piLSE->TxtWidth

#define RefreshRectangle lsl_pOS->piLSE->pRefreshRectangle
#define PenDashSet lsl_pOS->piLSE->pPenDashSet
#define PenDashDefine lsl_pOS->piLSE->pPenDashDefine
#define RotateScreen lsl_pOS->piLSE->pRotateScreen


// Dateihandling
#define OS_GetDriveListShort    lsl_pOS->piFILE->pSFR_GetDriveListShort

#define FindFirstEx lsl_pOS->piLSE->pOS_FindFirstEx
#define FindNextEx lsl_pOS->piLSE->pOS_FindNextEx

#define MemoryV1 lsl_pOS->piLSE->pMemoryV1

// ab LasalOS 5.70 gibt es die Elemente size und version
#define OS_IMAE_size              lsl_pOS->piLSE->size
#define OS_IMAE_version           lsl_pOS->piLSE->version

// ab IMAE Version 1:  
#define OS_IMAE_SetLed            lsl_pOS->piLSE->pSetLed
#define OS_IMAE_GetPotiVal        lsl_pOS->piLSE->pGetPotiVal
#define OS_IMAE_GetDipswitchVal   lsl_pOS->piLSE->pGetDipswitchVal

// ab IMAE Version 6
#define OS_StoreEventEx           lsl_pOS->piLSE->pOS_StoreEventEx
#define OS_KeyEventEx             lsl_pOS->piLSE->pOS_KeyEventEx

//----------------------
// Macros for Terminal Functions i386, Menk-19072002
//--------------------------
// udint
#define TRM_ShuttleKeyVersion lsl_pOS->piLSE->ShuttleKeyVersion
//--------------------------
// void (*_SetRepeatKey)(unsigned long delayms0, unsigned long charpersec0);
#define TRM_SetRepeatKey lsl_pOS->piLSE->_SetRepeatKey

//--------------------------
// void (*_SetShuttle)(unsigned long ticksperevent0, unsigned long timeout0);
#define TRM_SetShuttle lsl_pOS->piLSE->_SetShuttle

//--------------------------
// unsigned long (*_SetShuttleEnabled)(unsigned long enabled0);
#define TRM_SetShuttleEnabled lsl_pOS->piLSE->_SetShuttleEnabled

//--------------------------
// unsigned long (*_GetShuttleWheel)(void);
#define TRM_GetShuttleWheel lsl_pOS->piLSE->_GetShuttleWheel

//--------------------------
// long (*_SetLed)(unsigned long ledcode0, unsigned long op0);
#define TRM_SetLed lsl_pOS->piLSE->_SetLed


//--------------------------
// void (*_Pfeiferl)(unsigned long delayms0);
#define TRM_Beep lsl_pOS->piLSE->_Pfeiferl

//--------------------------
// Docu:
//   TRM_ShuttleKeyVersion:  <= init Version with value 0x00010001, also check OSVersion and WHOAMI Variable for the right Sigmatek PLC
//
//   TRM_SetRepeatKey:       => delayms0: delay between first key and auto repeating key in milliseconds
//                           => charpersec0: number of keys per second while auto repeating key 
//
//   TRM_SetShuttle:         => ticksperevent0: number of shuttle rotates before an Move-Event is being raised
//                           => timeout0: timeout in milliseconds before OS raises a RELEASED-Event. Timeout after Moving Event
//
//   TRM_SetShuttleEnabled   => enabled0: enables Shuttle Wheel to store Events in OS Queue, otherwise no events are stored => lower CPU-Load
//                           <= actual state of shuttle wheel events
//   TRM_GetShuttleWheel     <= actual Value of Shuttle Wheel as LONG Value
//
//   TRM_SetLed              => ledcode0: Led Code of KeyLed => see Hardware Docu
//                           => op0: led state: 0: LED OFF
//                                              1: LED ON
//                                              2: LED BLINKING
//                           <= -1: ERROR leds not supported 
//                               0: internal LED mailbox is full
//                               1: led set
//
//   TRM_Beep                => delayms0: beep for delayms0 Milliseconds 
//                              if delayms0 = 0 Beep is switched off immediatly
//
#endif

#define OS_CILRegister lsl_pOS->piCIL->CILRegister
#define OS_CILGet lsl_pOS->piCIL->CILGet
#define OS_CILRelease lsl_pOS->piCIL->CILRelease

//end of file
