//HFILTER:1
#ifndef __RTOS_INTERFACES_HEADER__97757206_BFE3_4A9D_BC11_B80C76042F7C__INCLUDED_
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define __RTOS_INTERFACES_HEADER__97757206_BFE3_4A9D_BC11_B80C76042F7C__INCLUDED_

// ***********************************************
//      RTOS and DIAS interface structures
// ***********************************************
// 
// Rev. 1.01 2 new function for the DIAS IRQ Scan (Aschl 10.11.2000)
//
//


TYPE
  SYS_ERROR                : (
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
    )$UDINT;

  SYS_ICIL                : STRUCT
        CILRegister        : pVoid;
        CILGet            : pVoid;
        CILRelease        : pVoid;
   END_STRUCT;

#pragma pack( push, 1 )

   _DDE_DATIM : STRUCT
   		DateTime: UDINT;
   END_STRUCT; 

//  _DDE_DATIM: this structure is bit-masked(!!):
//     first 5 bits: Second
//     next  6 bits: Minute
//     next  5 bits: Hour
//     next  5 bits: Day
//     next  4 bits: Month
//     next  7 bits: Year, counting from 1980

	_DDE_INFO :STRUCT
		FileName		: ARRAY[0..7] OF CHAR;
		Extension		: ARRAY[0..2] OF CHAR;
		Attributes		: USINT;
		Reserved		: ARRAY[0..7] OF CHAR;
		FirstClusterHi	: UINT;
		DateTime		: _DDE_DATIM;
		FirstCluster	: UINT;
		FileSize		: UDINT;
	END_STRUCT;

  DIAS_IRQ                : STRUCT
    ScanError            : USINT;
    ScanErrorCnt        : BYTE;
    MasterOff            : USINT;
   END_STRUCT;

  DIAS_IDIAS            : STRUCT
        lParam                : DINT;
        irq                        : DIAS_IRQ;
        ReadByte            : pVoid;
        ReadWord            : pVoid;
        WriteByte            : pVoid;
        WriteWord            : pVoid;
        ReadCtrl            : pVoid;
        WriteCtrl            : pVoid;
        ReadRegister    : pVoid;
        WriteRegister    : pVoid;
        GetModID            : pVoid;
        CheckError          :pVoid;
        Lsl_DiasIRQ_Init    :pVoid;
        Lsl_DiasIRQ         :PVoid; 
     END_STRUCT;
#pragma pack(pop)

  SYS_USER                : STRUCT
        usrCodeStart_Phys   : UDINT;  // Start address of User Code area (physical address) 
        usrCodeStart    : UDINT;        // Start address of User Code area
        usrCodeLength    : UDINT;    // Length of User Code area
        usrDataStart_Phys   : UDINT;  // Start address of User Data area (physical address) 
        usrDataStart    : UDINT;        // Start address of User Data area
        usrDataLength    : UDINT;    // Length of User Data area
        usrSramStart_Phys   : UDINT;  // Start address of User Sram area (physical address)
        usrSramStart        : UDINT;  // Start address of User Sram area (virtual address)
        usrSramLength       : UDINT;  // Length of User Data area
        usrVersion        : UDINT;        // Platform version code
        usrPriority        : UDINT;        // Main priority
        usrSramRetainStart  : UDINT;
        usrSramRetainLength : UDINT;
        usrSramRetainSramStart: UDINT;
        usrSramRetainSramLength: UDINT;
   END_STRUCT;

  DLED_IDLED            : STRUCT
        Display                : pVoid;    // Display LED data
        SetLed                : pVoid;    // Set a single LED
   END_STRUCT;

  LSL_ITASK                : STRUCT
        AddCyclicFunction         : pVoid;
        AddCyclicObject           : pVoid;
        RemoveCyclicTask          : pVoid;
        SignalCyclicTask          : pVoid;
        AddRealTimeFunction       : pVoid;
        AddRealTimeObject         : pVoid;
        RemoveRealTimeTask        : pVoid;
        SignalRealTimeTask        : pVoid;
        StartMeasCyclicTask       : pVoid;
        StopMeasCyclicTask        : pVoid;
        StartMeasRealTimeTask     : pVoid;
        StopMeasRealTimeTask      : pVoid;
        AddFrgFunction            : pVoid;
        AddFrgObject              : pVoid;
        SuspendFrgTask            : pVoid;
        ResumeFrgTask             : pVoid;
        InitTaskModul             : pVoid;
        GetPointerToTaskList      : pVoid;
        AddHPUserObject           : pVoid;  // THR RTOS V5.06: High Priority User task object
        MoveObject                : pVoid;  // pr: V5.43
        SetNbrOfLdrPhaseObjects   : pVoid;  // pr: V5.43
        udVersion                 : UDINT;  // Anm.: Beim Salamander ist das keine Versionsnummer sondern ein Zeiger auf eine Dummy-Funktion !
        GetKernelCyclic           : pVoid;
        SuspendKernelCyclic       : PVoid;
        Dummy5                    : pVoid;
        Dummy6                    : pVoid;
        Dummy7                    : pVoid;
        BackUpObject              : pVoid;
        RestoreObject             : pVoid;
        AddTask                   : pVoid;
        AddCallback               : pVoid;
        // Ab ITASK_VERSION 0x11
        unused0                   : pVoid;
        IsStopping                : pVoid;
        SetStopwaitCntr           : pVoid;
        // Ab ITASK_VERSION 0x12
        AddRealTimeFunctionEx     : pVoid;
        AddRealTimeObjectEx       : pVoid;
        AddCyclicFunctionEx       : pVoid;
        AddCyclicObjectEx         : pVoid;
        MoveFunctionEx            : pVoid;
        MoveObjectEx              : pVoid;
        SetNbrOfLdrPhaseObjectsEx : pVoid;
  END_STRUCT;

  LSL_SSR                : STRUCT
    // System Service Function
    SSR_InstallIRQ       : pVoid;
    SSR_InstallIRQObject : pVoid;

    SSR_CreateMailbox    : pVoid;
    SSR_MailboxGet       : pVoid;
    SSR_MailboxPut       : pVoid;
    SSR_MailboxDelete    : pVoid;

    SSR_CreateSemaphore  : pVoid;
    SSR_SemSignal        : pVoid;
    SSR_SemPulse         : pVoid;
    SSR_SemWait          : pVoid;
    SSR_SemWaitCond      : pVoid;
    SSR_SemDelete        : pVoid;

    SSR_OutW             : pVoid;
    SSR_OutB             : pVoid;
    SSR_InpW             : pVoid;
    SSR_InpB             : pVoid;

    SSR_MemoryAccess     : pVoid;

    SSR_CanTxObj         : pVoid;
    SSR_AddCanObj        : pVoid;

    SSR_Malloc           : pVoid;
    SSR_Free             : pVoid;

    SSR_SetSysTime       : pVoid;
    SSR_SetSysDate       : pVoid;
    SSR_GetSysTime       : pVoid;
    SSR_GetSysDate       : pVoid;

    SSR_Realloc          : pVoid;

    SSR_InstallIRQLASAL  : pVoid;

    pMemMove             : pVoid;
    pMemCpy              : pVoid;
    pMemSet              : pVoid;
    pMemCmp              : pVoid;

    SSR_LoginIntoCANNew  : pVoid;
    SSR_GetObjectInfo    : pVoid;
    malloc               : pVoid;
    free                 : pVoid;
    realloc              : pVoid;
    KernelLog            : pVoid;
    SSR_Delay            : pVoid;
    InstallDiasHandler   : pVoid;
    AddToKernelSP        : pVoid;
    SetHandler           : pVoid;
    SkipOverIDIV         : pVoid;
    EE_Read              : pVoid;   // for PLC Seriel EEProm read
    EE_Write             : pVoid;   // for PLC Seriel EEProm write
    ButtonPressed        : pVoid;   // for PLC Check Button Pressed
    SetTimerInterval     : pVoid;
    GetTimerInterval     : pVoid;
    SetCompatLevel       : pVoid;
    sizeofStruct         : UDINT;
    SSR_CurrentTaskHandle: pVoid;

    // EPROM on Dias PLC Socket => Dias Station 62
    PROMGetInfo_Socket   : pVoid;
    PROMSetData_Socket   : pVoid;
    PROMGetData_Socket   : pVoid;
    // EPROM on DiasBus as DIAS Modul => Dias Station 61
    PROMGetInfo_Modul    : pVoid;
    PROMSetData_Modul    : pVoid;
    PROMGetData_Modul    : pVoid;
   
    LSLPROM_GetInfo_Flash :pVoid;
    LSLPROM_SetData_Flash :pVoid;
    LSLPROM_GetData_Flash :pVoid;
    
    InitBasicCAN         : pVoid;
    SetBreakPoint        : pVoid; // since 1.1.123
    Dummy32              : pVoid;

    // 01.01.025
    SSR_MallocV1         : pvoid;
    SSR_ReallocV1        : pvoid;
    mallocV1             : pvoid;
    reallocV1            : pvoid;

    // 01.01.079
    SSR_Reboot           : pvoid;

    // 01.01.080
    ReadDateTime          : pvoid;
    SetDateTime           : pvoid;
    DateTime2Timestamp    : pvoid;
    Timestamp2DateTime    : pvoid;
    AdjustDateTime        : pvoid;
    CompareDateTime       : pvoid;
    TimestampAdd          : pvoid;
    TimestampSub          : pvoid;
    TimestampDiv          : pvoid;
    TimestampMult         : pvoid;

    SSR_Reset             : pvoid;
    SSR_ResetApp          : pvoid;
    SSR_SetDIASError      : pvoid;

    // 01.01.098
    GetSysvar             : pvoid;

    // 01.01.144
    HeapOutOfMemSetCallback : pvoid;

    // 01.01.156
    GetDivErrorInfo       : pvoid;
    GetModuleInfoByIp     : pvoid;

    // 01.01.185
    SSR_MarkAllocV1       : pvoid;
    SSR_ShrinkAlloc       : pvoid;

  END_STRUCT;


  LSL_LINKER                     : STRUCT
    // Linker Function
    LNK_GetFirstError          : pVoid;
    LNK_GetNextError           : pVoid;
    LNK_GetAddress_Function    : pVoid;
    LNK_GetAddress_Variable    : pVoid;
    LNK_GetVariableInfo        : pVoid;
    LNK_GetDataMemoryAddress   : pVoid;
    LNK_GetCodeMemoryAddress   : pVoid;
    LNK_GetProjectState        : pVoid;
    LNK_LookupEmbedded         : pVoid;
    LNK_GetObjDscList          : pVoid;  // RTKernel V3.18
    LNK_CRC32                  : pVoid;
    LNK_Add2ErrorList          : pVoid;
    LNK_FreeErrorList          : pVoid;
    LNK_GetErrorCount          : pVoid;
    LNK_GetNextChannel         : pVoid;
    LNK_GetObjectName          : pVoid;
    LNK_GetChkByModulID        : pVoid;
    LNK_GetChecksum            : pVoid;
    LNK_xch_ptrs               : pVoid;
    LNK_PrepareLink            : pVoid;
    LNK_ApplyLink              : pVoid;
    LNK_GetChecksum_WithModOrder : pVoid;
    LNK_GetVarChecksum         : pVoid;
    LNK_Dummy0                 : pVoid;
    LNK_Dummy1                 : pVoid;
    // 01.02.234
    LNK_GetChkSumFromClassName : pVoid;
    // 01.03.080
    LNK_GetChkSumByType        : pVoid;
  END_STRUCT;


  IMAE                         : STRUCT
    // Interface Data Entries
    lActive                    : pVoid;
    pIniFile                   : pVoid;
    lEnableTask                : pVoid;    
    // Interface Function Pointers
    pLSE_InstallTask           : pVoid;
    pLSE_Close                 : pVoid;
    pLSE_Restart               : pVoid;
    pStoreLST                  : pVoid;

    pLSEOS_MemMove             : pVoid;
    pLSEOS_MemCpy              : pVoid;
    pLSEOS_MemSet              : pVoid;
    pLSEOS_MemCmp              : pVoid;
    pLSEOS_GetAbsTime          : pVoid;

    pLSEOS_Fread               : pVoid;
    pLSEOS_Fwrite              : pVoid;
    pLSEOS_Fopen               : pVoid;
    pLSEOS_Fclose              : pVoid;
    pLSEOS_Flseek              : pVoid;
    pLSEOS_Ftell               : pVoid;
    pLSEOS_Ffilelength         : pVoid;
    pCreateDir                 : pVoid;
    pOS_FindFirst              : pVoid;
    pOS_FindNext               : pVoid;
    pOS_FindClose              : pVoid;
    pLSEOS_GetSRAMInfo         : pVoid;
    pLSEOS_GetPointerToSRAM    : pVoid;
    pLSEOS_OSGetPointerToKeyboard  : pVoid;
    pLSEOS_SetTime             : pVoid;
    pLSEOS_GetTime             : pVoid;
    pLSEOS_OS_Wait             : PVoid;

    pLSECOM_GetAddressVar      : pVoid;
    pLSECOM_GetAddressObj      : pVoid;
    pLSECOM_RegisterVar        : pVoid;
    pLSECOM_RegisterObj        : pVoid;
    pLSECOM_DeleteQueue        : pVoid;
    pLSECOM_Login              : pVoid;
    pSYSMAE_SetExternFunction  : pVoid;
    pLSECOM_SetData            : pVoid;
    pLSECOM_GetData            : pVoid;
    pLSEOS_MetaEvent           : pVoid;
    pLSEMAIN_pMain             : pVoid;
    // set at Startup from LSE
    pLSECOM_IRQ_PLC            : pVoid;
    pLSEMAIN_Close             : pVoid;
    pLSETimer                  : pVoid;
    pLSE_ScreenSaver           : pVoid;
    pGetPalette                : pVoid;

    pHideMouse           : pVoid;
    pShowMouse           : pVoid;
    pIfNecHideMouse      : pVoid;
    pIfNecShowMouse      : pVoid; 
    pSetFontAttribute    : pVoid;
    pStrWidth            : pVoid;
    pInitGraph           : pVoid;
    pCloseGraph          : pVoid;
    pGetScreenSize       : pVoid;
    pGetPixel            : pVoid;
    pPutPixel            : pVoid;
    pLine                : pVoid;
    pPolygon             : pVoid;
    pRectangle           : pVoid;
    pBar                 : pVoid;
    pOutTextXY           : pVoid;
    pOutTextLen          : pVoid;
    pArc                 : pVoid;
    pOval                : pVoid;
    pButton              : pVoid;
    pScroll              : pVoid;
    pBitmap              : pVoid;
    pActivateFont        : pVoid;
    pGetFontSize         : pVoid;
    pFreeRegion          : pVoid;
    pActivateRegion      : pVoid;
    pDeactivateRegion    : pVoid;
    pStartRegion         : pVoid;
    pAddRegion           : pVoid;
    pEndRegion           : pVoid;
    pSubRegion           : pVoid;
    pAndRegion           : pVoid;
    pXorRegion           : pVoid;
    pOrRegion            : pVoid;
    pGetActiveRegion     : pVoid;
    pGetNoColors         : pVoid;

    pGetImage            : pVoid;
    pPutImage            : pVoid;
    pGetRetrace          : pVoid;
    pWaitForRetrace      : pVoid;
    pIsGraphInit         : pVoid;
    pMemory              : pVoid;
    pOutXPixelPool       : pVoid;
    pOutYPixelPool       : pVoid;
    pOutPixelLine        : pVoid;
    pRoomInRegion        : pVoid;
    pMakeTrueColor       : pVoid;

    pOS_ShowCursor       : pVoid;
    pOS_MoveCursor       : pVoid;
    pOS_LimitMouse       : pVoid;
    pOS_StopMouse        : pVoid;
    pOS_StopEvent        : pVoid;
    pOS_StartEvent       : pVoid;
    pOS_StoreEvent       : pVoid;
    pOS_KeyEvent         : pVoid;
    pOS_InitMouse        : pVoid;

    pReadPixel           : pVoid;
    pWrtiePixel          : pVoid;
    pMakeColor           : pVoid;
    pMouseCursorStyle    : pVoid; 

    pDestroyOffScreen    : pVoid;
    pCreateOffScreen     : pVoid;
    pGetScreen           : pVoid;
    pSetScreen           : pVoid;
    pCopyScreen          : pVoid;
    pLCD_contrast        : pVoid;
    pLCD_brightness      : pVoid;
    pDrawTLBR            : pVoid;
    pSetNewCalibration   : pVoid;
    pLSE_SetTouchKoords  : pVoid;
   
    pChrWidth            : pVoid;
    pSetGraphicSystem    : pVoid;
    pOutChar             : pVoid;
    pTxtWidth            : pVoid;

    pRefreshRectangle    : pVoid;
    pPenDashSet          : pVoid;
    pPenDashDefine       : pVoid;
    pRotateScreen        : pVoid;

    // ab LasalOS 5.70 gibt es die Elemente size und version
    size                 : UDINT;
#define IMAE_VERSION    7
    version              : UDINT;

    // ab IMAE Version 1:  
    pSetLed              : pVoid;
    pGetPotiVal          : pVoid;
    pGetDipswitchVal     : pVoid;

    // ab IMAE Version 2:  
    pOS_FindFirstEx      : pVoid;
    pOS_FindNextEx       : pVoid;

    // ab IMAE Version 3:
    pMemoryV1            : pvoid;
                           
	  //SGraphic, ab IMAE Version 4
    pmyInitGraph		 : pvoid;
    pmyGetBuffer		 : pvoid;
    pmyGetPixel			 : pvoid;
    pmyGetImage			 : pvoid;
    pmyGetImageSize		 : pvoid;
    pmyLoadUserFont		 : pvoid;
    pmyTextDimensions	 : pvoid;
    pmyTextHeight		 : pvoid;

    pmyClrScr			 : pvoid;
    pmyPutPixel			 : pvoid;
    pmyLine			 	 : pvoid;
    pmyPolygon			 : pvoid;
    pmyRectangle		 : pvoid;
    pmyBar			 	 : pvoid;
    pmyOutTextXY		 : pvoid;
    pmyScroll			 : pvoid;

    pmyPutImage			 : pvoid;
    pmyBitmap			 : pvoid;
    pmyMakeRegion		 : pvoid;
    pmyKillRegion		 : pvoid;
    pmyEmptyRegion		 : pvoid;
    pmyActivateRegion	 : pvoid;
    pmyDefaultRegion	 : pvoid;
    pmyAndRegion		 : pvoid;

    pmyXorRegion		 : pvoid;
    pmyOrRegion			 : pvoid;
    pmyBarRegion  		 : pvoid;
   	pmyGetXSize		 	 : pvoid;
   	pmyGetYSize 		 : pvoid;
   	pmyUpdateDone		 : pvoid;
   	pmyRefresh           : pvoid;
   	pmyInvert			 : pvoid;	//ws: invert

    // ab IMAE Version 5
    pDefineMouseCursorStyle   : pvoid;

    // ab IMAE Version 6
    pOS_StoreEventEx    : pvoid;
    pOS_KeyEventEx      : pvoid;

    // ab IMAE Version 7
    pmyInitGraphEx		  : pvoid;
    pmyGetBufferEx		  : pvoid;
    pmyGetPixelEx			  : pvoid;
    pmyGetImageEx			  : pvoid;
    pmyGetImageSizeEx		: pvoid;
    pmyLoadUserFontEx		: pvoid;
    pmyTextDimensionsEx	: pvoid;
    pmyTextHeightEx		  : pvoid;

    pmyClrScrEx			    : pvoid;
    pmyPutPixelEx			  : pvoid;
    pmyLineEx			 	    : pvoid;
    pmyPolygonEx			  : pvoid;
    pmyRectangleEx		  : pvoid;
    pmyBarEx			 	    : pvoid;
    pmyOutTextXYEx		  : pvoid;
    pmyScrollEx			    : pvoid;

    pmyPutImageEx			  : pvoid;
    pmyBitmapEx			    : pvoid;
    pmyMakeRegionEx		  : pvoid;
    pmyKillRegionEx		  : pvoid;
    pmyEmptyRegionEx		: pvoid;
    pmyActivateRegionEx	: pvoid;
    pmyDefaultRegionEx	: pvoid;
    pmyAndRegionEx		  : pvoid;

    pmyXorRegionEx		  : pvoid;
    pmyOrRegionEx			  : pvoid;
    pmyBarRegionEx  		: pvoid;
   	pmyGetXSizeEx		 	  : pvoid;
   	pmyGetYSizeEx 		  : pvoid;
   	pmyUpdateDoneEx	 		: pvoid;
   	pmyRefreshEx        : pvoid;
   	pmyInvertEx					: pvoid;
    pmyHasChanged       : pvoid;
    
   // ab IMAE Version 8
   
   pmyInitHourGlass     :pvoid;
   pmyActivateHourGlass :pvoid;
   pmyDeactivateHourGlass : pvoid;
   pmyCopyBitmap        :pvoid;

   // ab IMAE Version 9
   
    pSetNewCalibrationEx   : pVoid;
    
    // Ab IMAE Version 10
    pFreeImage : pVoid;
    
    // Ab IMAE Version 11
	pCreateRawBuf : pVoid;
	pFreeRawBuf : pVoid;
    pDisplayRaw : pVoid;
END_STRUCT;

  
    LSL_IDIAG          : STRUCT
        bDiagWindowOpen  : UDINT;
        piSysCon         : pVoid;
        Window           : pVoid;
        TaskHandle       : pVoid;
        DiagReset        : pVoid;
        DiagOpenWindow   : pVoid;
        DiagCloseWindow  : pVoid;
        DiagOpenCLI      : pVoid;
        DiagCloseCLI     : pVoid;
        DiagResetCLI     : pVoid;
    END_STRUCT;

    LSL_SYSTIME         : STRUCT
        ResetMeas       : pVoid;
        StartMeas       : pVoid;
        StopMeas        : pVoid;
        ResetElapsed    : pVoid;
        Elapsed         : pVoid;
        ReadMicroSec    : pVoid;
    END_STRUCT;

    OS_IFILE  : STRUCT
        SFR_Fread     : pVoid;
        SFR_Fwrite    : pVoid;
        SFR_Fopen     : pVoid;
        SFR_Fclose    : pVoid;
        SFR_Fdel      : pVoid;
        SFR_Flseek    : pVoid;
        SFR_Ftell     : pVoid;
        SFR_Ffilelength : pVoid;
        SFR_CreateDir : pVoid;
        SFR_RemoveDir : pVoid;
        SFR_FindFirst : pVoid;
        SFR_FindNext  : pVoid;
        SFR_FindClose : pVoid;
        SFR_CloseAllOpenFiles : pVoid;
        SFR_Rename    : pVoid;
        SFR_FopenX   : pVoid;
        SFR_Fwrite_X : pVoid;
        OS_SetTell   : pVoid;
        //**HH
        // ab LasalOS V.51
        //            V.50 CIPC für TIG
        SFR_GetDriveListShort : pVoid;
        SFR_SetDriverList : pVoid;
        SFR_GetDriverList : pVoid;
        SFR_FExtend : pVoid;
        lAppDefOpenFlags   : UDINT;
        SFR_SetDefaultOpenFlags   : pVoid;
        //**HH
        // ab LasalOS V.555h
        //            V.56 CIPC für KM
        SFR_Fread_A     : pVoid;
        SFR_Fwrite_A    : pVoid;
        SFR_Fopen_A     : pVoid;
        SFR_Fclose_A    : pVoid;
        SFR_Fdel_A      : pVoid;
        SFR_Flseek_A    : pVoid;
        SFR_Ftell_A     : pVoid;
        SFR_Ffilelength_A : pVoid;
        SFR_CreateDir_A : pVoid;
        SFR_RemoveDir_A : pVoid;
        SFR_FindFirst_A : pVoid;
        SFR_FindNext_A  : pVoid;
        SFR_FindClose_A : pVoid;
        SFR_CloseAllOpenFiles_A : pVoid;
        SFR_Rename_A    : pVoid;
        SFR_GetState    : pVoid;

        SFR_Ftruncate   : pVoid;
        SFR_Ftruncate_A : pVoid;

        // ab LasalOS 5.59d
        SFR_FindFirstEx   : pVoid;
        SFR_FindNextEx    : pVoid;
        SFR_FindFirstEx_A : pVoid;
        SFR_FindNextEx_A  : pVoid;

        SFR_FileCopy      : pVoid;

        // ab V 1.1.4
        SFR_GetAttributes : pVoid;
        SFR_SetAttributes : pVoid;
        SFR_GetDiskSpace  : pVoid;

        // 01.01.025
        SFR_FreadV1       : pVoid;
        SFR_Fread_AV1     : pVoid;
        SFR_Fwrite_AV1    : pVoid;

        // 01.01.056
        SFR_CreateRAMDisk   : pVoid;
        SFR_DestroyRAMDisk  : pVoid;

        // 01.01.063
        SFR_FileCopy_A      : pVoid;

        // 01.01.067
        SFR_Reserved01      : pVoid;
        SFR_Reserved02      : pVoid;

        // 01.01.078
        SFR_GetDiskSpace_A        : pVoid;
        SFR_GetDriveListShort_A   : pVoid;
        SFR_GetDriveListShortEx   : pVoid;
        SFR_GetDriveListShortEx_A : pVoid;
        SFR_CheckDisk             : pVoid;
        SFR_CheckDisk_A           : pVoid;
        SFR_Dummy1                : pVoid;
        SFR_Dummy2                : pVoid;
        SFR_Dummy3                : pVoid;

        // 01.02.045
        SFR_SetVolumeLabel        : pVoid;
        SFR_GetVolumeLabel        : pVoid;

        // 01.02.070
        SFR_GetFATType            : pVoid;

        // 01.02.086
        SFR_SRamLoadV1            : pVoid;

        // 01.02.195
        SFR_CreateFile            : pVoid;
        SFR_CreateFile_A          : pVoid;
        
        // 01.02.225
        SFR_SplitNameExtension    : pVoid;

        // 01.03.015
        SFR_RemoveDir_Full        : pVoid;
        SFR_RemoveDir_Full_A       : pVoid;

        // 01.03.141
        SFR_GetVolumeSerialNumber : pVoid;

    END_STRUCT;

    LSLTERMINAL : STRUCT
        bEnabled             : UDINT;
        pSIMUKEY_Init        : pVoid;
        pSIMUKEY_GetCh       : pVoid;
        pSIMUKEY_KeyPressed  : pVoid;
        pSIMUKEY_PutCh       : pVoid;
        pGetTerminalBuffer   : pVoid;
        // standard keyboard funcs for spec. platform
        std_KBInit           : pVoid;
        std_KBKeyPressed     : pVoid;
        std_KBGetCh          : pVoid;
        std_KBPutCh          : pVoid;
        ActPos               : UDINT;
        ActRows              : ARRAY[0..22] of USINT;
        ActCols              : UDINT;
    END_STRUCT;

    LSL_PRINT : STRUCT
        pSetPrinterIOBase : pVoid;
        pPrinterInit      : pVoid;
        pPrinterStatus    : pVoid;
        pPrintByte        : pVoid;
        pUSBPrinterInfo   : pVoid;
        pUSBPrinterInit   : pVoid;
        pUSBPrinterStatus : pVoid;
        pUSBPrintData     : pVoid;
        pUSBPrintCancel   : pVoid;
    END_STRUCT;

    LSL_PRINTER_CUPS : STRUCT
        version           : UDINT;
        pAddPrinter       : pVoid;
        pRemovePrinter    : pVoid;
        pGetState         : pVoid;
        pAddOption        : pVoid;
        pRemoveOption     : pVoid;
        pGetNrPrinters    : pVoid;
        pListPrinters     : pVoid;
        pGetNrOptions     : pVoid;
        pListOptions      : pVoid;
        pGetNrConnUSB     : pVoid;
        pListConnUSB      : pVoid;
        pGetNrJobs        : pVoid;
        pListJobs         : pVoid;
        pRemoveJob        : pVoid;
        pStartJob         : pVoid;
        pEndJob           : pVoid;
        pPrintFiles       : pVoid;
        pPrintScreenshot  : pVoid;
        pPrintOffscreen   : pVoid;
    END_STRUCT;

    LSL_FTP_API: STRUCT
        pftp_Connect    : pVoid;
        pftp_XConnect   : pVoid;
        pftp_CWD        : pVoid;
        pftp_PWD        : pVoid;
        pftp_MKDIR      : pVoid;
        pftp_RMDIR      : pVoid;
        pftp_DELE       : pVoid;
        pftp_BYE        : pVoid;
        pftp_LIST       : pVoid;
        pftp_NLIST      : pVoid;
        pftp_PUT        : pVoid;
        pftp_APPEND     : pVoid;
        pftp_GET        : pVoid;
        pftp_TRANSFTYPE : pVoid;
        pftp_dummy01    : pVoid;
        pftp_dummy02    : pVoid;
        // ab LasalOS version 1.1.15 gibt es eine Versionsnummer und eine Länge der FTP Struktur
        size            : UDINT;
        version         : UDINT;
        // ab version 1
        pftp_REN        : pVoid;
        // ab version 2
        pftp_ClientCfg  : pVoid;
    END_STRUCT;

    LSLAPI_SERIAL : STRUCT
       pInitSerial      : pVoid;
       pGetError        : pVoid;
       pGetHandle       : pVoid;
       pIsInitialized   : pVoid;
       pGetInfo         : pVoid;
       pSetBufferRecv   : pVoid;
       pSend            : pVoid;
       pRecvChar        : pVoid;
       pRecvBlock       : pVoid;
       pGetSendStatus   : pVoid;
       pGetRecvStatus   : pVoid;
       pClearRecvBuffer : pVoid;
       pGetRecvPointer  : pVoid;
       pClose           : pVoid;
       pReset           : pVoid;
       pSetOnline       : pVoid;
       pSetFunction     : pVoid;
       pSetModemControl : pVoid;
       pGetModemControl : pVoid;
       pSet422Mode      : pVoid;
       pGet422Mode      : pVoid;
       pGetModemStatus  : pVoid;
       pHasFIFO         : pVoid;
       pEnableFIFO      : pVoid;
       pSetBreak        : pVoid;
       pSetMDPMode      : pVoid; 
       pSet485Mode      : pVoid; 
       // ab OS Version 1.1.227
       pGetNrByPath     : pVoid;
       // ab OS Version 01.02.195
       dummy1           : pVoid;
       dummy2           : pVoid;
       pRecvTimingChar  : pVoid;
       pRecvTimingBlock : pVoid;
       pClearTimingBuffer: pVoid;
       pSetTimingBufferRecv : pVoid;
    END_STRUCT;

    SYS_QNODE : STRUCT
	pNext		: pVoid;	// pointer to Next in list
	pPrev		: pVoid;	// pointer to Prev in list
	pData		: pVoid;	// pointer to node data
    END_STRUCT;

    SYS_QUEUE : STRUCT
	pHead		: ^SYS_QNODE;	// pointer to queue head node
	pTail		: ^SYS_QNODE;	// pointer to queue tail node
    END_STRUCT;

    LSL_ISYSQUEUE        : STRUCT
        AddQueueNode    : pVoid;
	RemoveQueueHead	: pVoid;
	RemoveQueueNode	: pVoid;
	FindQueueNode	: pVoid;
	InsertQueueNode	: pVoid;
    END_STRUCT;

  LSL_OSDATA      : STRUCT
        // Kernel Layer Data
        piCIL        : ^SYS_ICIL;    // a pointer to the CIL interface from the kernel

        // DIAS Device Data
        piDias        : ^DIAS_IDIAS;    // pointer to the main DIAS Bus master device interface

        // DLED Device Interface
        piDLed        : ^DLED_IDLED;
    
        // User Area Definition
         pUser        : ^SYS_USER;    // pointer to user def struct interface

        // System Time Interface Pointer
        piSysTime     : ^LSL_SYSTIME;        // System Time interface

        // Pointer to System Queue Interface
        piSysQueue    : ^LSL_ISYSQUEUE;        // System Queue interface (THR)

        // Pointer to LASAL Task Interface
        piTask        : ^LSL_ITASK;    // Task interface

        // Pointer to C Runtime Interface
        piCRT         : pVoid;    // Task interface

        // Pointer to LASAL OS System Services
        piSSR         : ^LSL_SSR;

        // Pointer to LSLDIAG Interface
        piDiag        : pVoid;    // Diag interface

           // Pointer to LINKER Interface
        piLINKER      : ^LSL_LINKER;  // Linker Interface

        // Pointer to LASAL File Interface (RTKernel V3.18)
        piLslFile     : pVoid; // LASAL Data File Interface

        // LASALCOM Interface Pointer (V3.23)
        piLASALCOM    : pVoid;

         // pointer to MAE interface
        piLSE         : ^IMAE;

         // pointer to FILE I/O interface
        piFILE        : ^OS_IFILE;

        // OS-Terminal interface pointer (v4)
        piTerminal    : ^LSLTERMINAL;

        // Printer API interface pointer (v4)
        piPrinter     : ^LSL_PRINT;

        // USER Serial (COM) API interface pointer (v4 / Menk)
        piSerial      : ^LSLAPI_SERIAL;

        // FTP API pointer
        piFTP         : ^LSL_FTP_API;

        // FTP API pointer
        piTCP         : pVoid;

     END_STRUCT;

END_TYPE


//
//    Prototypes and Macros for CIL Access
//

// Function prototype for CILRegister()
FUNCTION GLOBAL P_CILRegister
VAR_INPUT
    pName        : pVoid;
    pInterface    : pVoid;
    wRefLimit    : UINT;
END_VAR;

// Function prototype for CILGet()
FUNCTION GLOBAL P_CILGet
VAR_INPUT
    pName        : pVoid;
    pInterface    : pVoid;
END_VAR
VAR_OUTPUT
    EAX            : SYS_ERROR;
END_VAR;


// Function prototype for CILRelease()
FUNCTION GLOBAL P_CILRelease
VAR_INPUT
    pName        : pVoid;
END_VAR;

// Function prototype for LslDiasIRQ() 
FUNCTION GLOBAL __cdecl P_LslDiasIRQ
VAR_INPUT
    Station      :UINT;
    actionp      :pVoid;
    thispointer  :PVoid;
END_VAR
VAR_OUTPUT
    ret          :UINT;    
END_VAR;

// Function prototype for LslDiasIRQ_Init() 
FUNCTION GLOBAL __cdecl P_LslDiasIRQ_Init;

FUNCTION GLOBAL __cdecl P_LslDiasWriteCtrl
VAR_INPUT
    usStation     : USINT;
    actionp       : pVoid;
    thispointer   : pVoid;
END_VAR;

// CIL Interface Macros
#define OS_CILRegister(p1,p2,p3) _LSL_POS^.piCIL^.CILRegister $ P_CILRegister(p1,p2,p3)
#define OS_CILGet(p1,p2) _LSL_POS^.piCIL^.CILGet $ P_CILGet(p1,p2)
#define OS_CILRelease(p1) _LSL_POS^.piCIL^.CILRelease $ P_CILRelease(p1)

#define OS_DiasIRQ_Init() _LSL_POS^.piDIAS^.Lsl_DiasIRQ_Init $ P_LslDiasIRQ_Init()
#define OS_DiasIRQ(p1,p2,p3) _LSL_POS^.piDIAS^.Lsl_DiasIRQ $ P_LslDiasIRQ(p1,p2,p3)  

#define OS_DIAS_WRITECTRL(p1,p2,p3) _LSL_POS^.piDIAS^.Lsl_DiasIRQ $ P_LslDiasWriteCtrl(p1,p2,p3)

// SysTime Interface Macros
FUNCTION GLOBAL P_ReadMicroSec
VAR_OUTPUT
    EAX            : UDINT;
END_VAR;

#define OS_ReadMicroSec() _LSL_POS^.piSysTime^.ReadMicroSec $ P_ReadMicroSec();
#define OS_GetMicroSec() _LSL_POS^.piSysTime^.ReadMicroSec $ P_ReadMicroSec()

#define OS_IMAE_version   _LSL_POS^.piLSE^.version
#define OS_IMAE_size      _LSL_POS^.piLSE^.size

//
//	Prototypes and Macros for System Queue Access
//
// Add a Node to the Queue
//SYS_QNODE* (* AddQueueNode)(SYS_QUEUE* pQueue, void* pData);
FUNCTION GLOBAL __cdecl P_AddQueueNode
VAR_INPUT
    pQueue			: ^SYS_QUEUE;
    pData			: pVoid;
END_VAR
VAR_OUTPUT
	pNode			: ^SYS_QNODE;
END_VAR;
#define OS_AddQueueNode(p1,p2) _LSL_POS^.piSysQueue^.AddQueueNode $ P_AddQueueNode(p1,p2)

// Remove Node at Head
//	void* (* RemoveQueueHead)(SYS_QUEUE* pQueue);
FUNCTION GLOBAL __cdecl P_RemoveQueueHead
VAR_INPUT
    pQueue			: ^SYS_QUEUE;
END_VAR
VAR_OUTPUT
	pData			: pVoid;
END_VAR;
#define OS_RemoveQueueHead(p1) _LSL_POS^.piSysQueue^.RemoveQueueHead $ P_RemoveQueueHead(p1)


// Remove Node
//void* (* RemoveQueueNode)(SYS_QUEUE* pQueue, SYS_QNODE* pNode);
FUNCTION GLOBAL __cdecl P_RemoveQueueNode
VAR_INPUT
    pQueue			: ^SYS_QUEUE;
	pNode			: ^SYS_QNODE;
END_VAR
VAR_OUTPUT
	pData			: pVoid;
END_VAR;
#define OS_RemoveQueueNode(p1,p2) _LSL_POS^.piSysQueue^.RemoveQueueNode $ P_RemoveQueueNode(p1,p2)


// Find Node
//SYS_QNODE* (* FindQueueNode)(SYS_QUEUE* pQueue, void* pData);
FUNCTION GLOBAL __cdecl P_FindQueueNode
VAR_INPUT
    pQueue			: ^SYS_QUEUE;
	pData			: pVoid;
END_VAR
VAR_OUTPUT
	pNode			: ^SYS_QNODE;
END_VAR;
#define OS_FindQueueNode(p1,p2) _LSL_POS^.piSysQueue^.FindQueueNode $ P_FindQueueNode(p1,p2)

// Insert a node
//SYS_QNODE* (* InsertQueueNode)(SYS_QUEUE* pQueue, SYS_QNODE* pNd, void* pData);
FUNCTION GLOBAL __cdecl P_InsertQueueNode
VAR_INPUT
    pQueue			: ^SYS_QUEUE;
	pNd				: ^SYS_QNODE;
	pData			: pVoid;
END_VAR
VAR_OUTPUT
	pNode			: ^SYS_QNODE;
END_VAR;
#define OS_InsertQueueNode(p1,p2) _LSL_POS^.piSysQueue^.InsertQueueNode $ P_InsertQueueNode(p1,p2,p3)


//
// GetDriveListShort
//
FUNCTION GLOBAL __cdecl P_GetDriveListShort
VAR_INPUT
	pErg			: PVOID;
END_VAR
VAR_OUTPUT
	lErg			: DINT;
END_VAR;
#define OS_GetDriveListShort(p1) _LSL_POS^.piFile^.SFR_GetDriveListShort $ P_GetDriveListShort(p1)

//
// FileOpen
//
FUNCTION GLOBAL __cdecl P_FileOpenX
VAR_INPUT
	path			: ^char;
	attribs			: udint;
END_VAR
VAR_OUTPUT
	retcode			: DINT;
END_VAR;
#define OS_FILEOPENX(p1,p2) _LSL_POS^.piFile^.SFR_FopenX $ P_FileOpenX(p1,p2)

//
// FExtend
//
FUNCTION GLOBAL __cdecl P_FileExtend
VAR_INPUT
	handle			: DINT;
	size			: udint;
END_VAR
VAR_OUTPUT
	retcode			: DINT;
END_VAR;
#define OS_FILEEXTEND(p1,p2) _LSL_POS^.piFile^.SFR_FExtend $ P_FileExtend(p1,p2)

//
// RefreshRectangle
//
FUNCTION GLOBAL __cdecl P_RefreshRectangle
VAR_INPUT
  x : ^_ROOM;
END_VAR;
#define RefreshRectangle(p1) _LSL_POS^.piLSE^.pRefreshRectangle $ P_RefreshRectangle(p1)

//
// SetLed
//
FUNCTION GLOBAL __cdecl P_SetLed
VAR_INPUT
  ledCode   : UDINT;
  op        : UDINT;  // 0=aus, 2=blink, >=3=ein
END_VAR;
#define OS_IMAE_SetLed(p1,p2) _LSL_POS^.piLSE^.pSetLed $ P_SetLed(p1,p2)

//
// GetPotiVal
//
FUNCTION GLOBAL __cdecl P_GetPotiVal
VAR_INPUT
  potiNr : UDINT; // 0-based
END_VAR
VAR_OUTPUT
  value    : UDINT;
END_VAR;
#define OS_IMAE_GetPotiVal _LSL_POS^.piLSE^.pGetPotiVal $ P_GetPotiVal

//
// GetDipswitchVal
//
FUNCTION GLOBAL __cdecl P_GetDipswitchVal
VAR_INPUT
  dipNr : UDINT; // 0-based
END_VAR
VAR_OUTPUT
  value    : UDINT;
END_VAR;
#define OS_IMAE_GetDipswitchVal _LSL_POS^.piLSE^.pGetDipswitchVal $ P_GetDipswitchVal

//
// LCD_Contrast
//
FUNCTION GLOBAL __cdecl P_LCD_Contrast
VAR_INPUT
  cmd       : UINT;
  value     : UDINT;
END_VAR
VAR_OUTPUT
  retVal    : UDINT;
END_VAR;
#define OS_IMAE_LcdContrast _LSL_POS^.piLSE^.pLCD_Contrast $ P_LCD_Contrast

//////////////SGraphic

// define ST prototypes for API
//General
FUNCTION GLOBAL P_GetXSize
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetYSize
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetPixel
VAR_INPUT
	x : DINT;
	y : DINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetImage
VAR_INPUT
	img : pvoid;
	x1 : DINT;
	y1 : DINT;
	x2 : DINT;
	y2 : DINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetImageSize
VAR_INPUT
	x1 : DINT;
	y1 : DINT;
	x2 : DINT;
	y2 : DINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_LoadUserFont
VAR_INPUT
	font : pVoid;
END_VAR;

FUNCTION GLOBAL P_TextDimensions
VAR_INPUT
	pdot : pVoid;
    txt : pVoid;
    att : udint;
    len	: DINT;
END_VAR;

FUNCTION GLOBAL P_TextHeight;


FUNCTION GLOBAL __cdecl P_LCDContrast
VAR_INPUT
  cmd       : USINT;
  value     : UDINT;
END_VAR
VAR_OUTPUT
  EAX    : UDINT;
END_VAR;

FUNCTION GLOBAL P_LCDRefresh
VAR_INPUT
	doit : udint;
END_VAR;
  
FUNCTION GLOBAL P_LCDRefreshDone
VAR_OUTPUT
	EAX : udint;
END_VAR;


//Drawing
FUNCTION GLOBAL P_ClrScr
VAR_INPUT
	Col : DINT;
END_VAR;

FUNCTION GLOBAL P_PutPixel
VAR_INPUT
    x   : DINT;
    y	: DINT;
    att : udint;
    col : udint;
END_VAR;

FUNCTION GLOBAL P_Line
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    x2 	: DINT;
    y2	: DINT;
    att : udint;
    col : udint;
END_VAR;

FUNCTION GLOBAL P_Polygon
VAR_INPUT
    c  : UDINT;
    dot	: pVoid;
    att : udint;
    col : udint;
END_VAR;

FUNCTION GLOBAL P_Rectangle
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    x2 	: DINT;
    y2	: DINT;
    att : udint;
    col : udint;
END_VAR;

FUNCTION GLOBAL P_Bar
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    x2 	: DINT;
    y2	: DINT;
    att : udint;
    col : udint;
END_VAR;

FUNCTION GLOBAL P_OutText
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    txt : pVoid;
    att : udint; 
    col : udint;
    len	: DINT;
END_VAR;

FUNCTION GLOBAL P_Scroll
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    x2	: DINT;
    y2	: DINT;
    xx	: DINT;
    yy	: DINT;
    col	: UDINT;
END_VAR;

FUNCTION GLOBAL P_PutImage
VAR_INPUT
	data : pVoid;
    x  : DINT;
    y	: DINT;
END_VAR;

FUNCTION GLOBAL P_PutPixmap
VAR_INPUT
    x  : DINT;
    y	: DINT;
    data : pVoid;
END_VAR;


//Region Clipping
FUNCTION GLOBAL P_MakeRegion
VAR_INPUT
	r	: ^pVoid;
END_VAR
VAR_OUTPUT
	EAX : udint;
END_VAR;

FUNCTION GLOBAL P_KillRegion
VAR_INPUT
	r	: pVoid;
END_VAR;

FUNCTION GLOBAL P_EmptyRegion
VAR_INPUT
	r	: pVoid;
	col	: udint;
END_VAR;

FUNCTION GLOBAL P_ActivateRegion
VAR_INPUT
	r	: pVoid;
END_VAR;

FUNCTION GLOBAL P_DefaultRegion;

FUNCTION GLOBAL P_AndRegion
VAR_INPUT
	pr1	: ^pVoid;
	pr2	: ^pVoid;
END_VAR;

FUNCTION GLOBAL P_OrRegion
VAR_INPUT
	pr1	: ^pVoid;
	pr2	: ^pVoid;
END_VAR;

FUNCTION GLOBAL P_XorRegion
VAR_INPUT
	pr1	: ^pVoid;
	pr2	: ^pVoid;
END_VAR;

FUNCTION GLOBAL P_BeamRegion
VAR_INPUT
	r	: pVoid;
    x1  : DINT;
    y1	: DINT;
    x2 	: DINT;
    y2	: DINT;
    att : udint;
    col : udint;
END_VAR;

FUNCTION GLOBAL P_InitGraph
VAR_INPUT
	x	: DINT;
    y	: DINT;
    c	: UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetBuffer
VAR_OUTPUT
	EAX : pVoid;
END_VAR;

FUNCTION GLOBAL P_Invert	//ws: invert
VAR_INPUT
  in : dint;
END_VAR;

////---------------
//General
FUNCTION GLOBAL P_GetXSizeEx
VAR_INPUT
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetYSizeEx
VAR_INPUT
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetPixelEx
VAR_INPUT
	x : DINT;
	y : DINT;
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetImageEx
VAR_INPUT
	img : pvoid;
	x1 : DINT;
	y1 : DINT;
	x2 : DINT;
	y2 : DINT;
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetImageSizeEx
VAR_INPUT
	x1 : DINT;
	y1 : DINT;
	x2 : DINT;
	y2 : DINT;
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_LoadUserFontEx
VAR_INPUT
	font : pVoid;
	id: UDINT;
END_VAR;

FUNCTION GLOBAL P_TextDimensionsEx
VAR_INPUT
	pdot : pVoid;
    txt : pVoid;
    att : udint;
    len	: DINT;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_TextHeightEx
VAR_INPUT
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL __cdecl P_LCDContrastEx
VAR_INPUT
  cmd       : USINT;
  value     : UDINT;
  id				: UDINT;
END_VAR
VAR_OUTPUT
  EAX    : UDINT;
END_VAR;

FUNCTION GLOBAL P_LCDRefreshEx
VAR_INPUT
	doit : udint;
	id	 : UDINT;
END_VAR;
  
FUNCTION GLOBAL P_LCDRefreshDoneEx
VAR_INPUT
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : udint;
END_VAR;

//Drawing
FUNCTION GLOBAL P_ClrScrEx
VAR_INPUT
	Col : DINT;
	id : UDINT;
END_VAR;

FUNCTION GLOBAL P_PutPixelEx
VAR_INPUT
    x   : DINT;
    y	: DINT;
    att : udint;
    col : udint;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_LineEx
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    x2 	: DINT;
    y2	: DINT;
    att : udint;
    col : udint;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_PolygonEx
VAR_INPUT
    c  : UDINT;
    dot	: pVoid;
    att : udint;
    col : udint;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_RectangleEx
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    x2 	: DINT;
    y2	: DINT;
    att : udint;
    col : udint;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_BarEx
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    x2 	: DINT;
    y2	: DINT;
    att : udint;
    col : udint;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_OutTextEx
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    txt : pVoid;
    att : udint; 
    col : udint;
    len	: DINT;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_ScrollEx
VAR_INPUT
    x1  : DINT;
    y1	: DINT;
    x2	: DINT;
    y2	: DINT;
    xx	: DINT;
    yy	: DINT;
    col	: UDINT;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_PutImageEx
VAR_INPUT
	data : pVoid;
    x  : DINT;
    y	: DINT;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_PutPixmapEx
VAR_INPUT
    x  : DINT;
    y	: DINT;
    data : pVoid;
    id : UDINT;
END_VAR;


//Region Clipping
FUNCTION GLOBAL P_MakeRegionEx
VAR_INPUT
	r	: ^pVoid;
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : udint;
END_VAR;

FUNCTION GLOBAL P_KillRegionEx
VAR_INPUT
	r	: pVoid;
	id : UDINT;
END_VAR;

FUNCTION GLOBAL P_EmptyRegionEx
VAR_INPUT
	r	: pVoid;
	col	: udint;
	id : UDINT;
END_VAR;

FUNCTION GLOBAL P_ActivateRegionEx
VAR_INPUT
	r	: pVoid;
	id : UDINT;
END_VAR;

FUNCTION GLOBAL P_DefaultRegionEx
VAR_INPUT
	id : UDINT;
END_VAR;

FUNCTION GLOBAL P_AndRegionEx
VAR_INPUT
	pr1	: ^pVoid;
	pr2	: ^pVoid;
	id : UDINT;
END_VAR;

FUNCTION GLOBAL P_OrRegionEx
VAR_INPUT
	pr1	: ^pVoid;
	pr2	: ^pVoid;
	id : UDINT;
END_VAR;

FUNCTION GLOBAL P_XorRegionEx
VAR_INPUT
	pr1	: ^pVoid;
	pr2	: ^pVoid;
	id : UDINT;
END_VAR;

FUNCTION GLOBAL P_BeamRegionEx
VAR_INPUT
	r	: pVoid;
    x1  : DINT;
    y1	: DINT;
    x2 	: DINT;
    y2	: DINT;
    att : udint;
    col : udint;
    id : UDINT;
END_VAR;

FUNCTION GLOBAL P_InitGraphEx
VAR_INPUT
	x	: DINT;
    y	: DINT;
    c	: UDINT;
    id : ^UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;

FUNCTION GLOBAL P_GetBufferEx
VAR_INPUT
	id : UDINT;
END_VAR
VAR_OUTPUT
	EAX : pVoid;
END_VAR;



FUNCTION GLOBAL P_InitHourGlass
VAR_INPUT
    thisobj : pvoid;
    callbackfct	: pvoid;
    x	: UDINT;
    y	: UDINT;
    xsize : UDINT;
    ysize	: UDINT;
    calltime : UDINT;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;



FUNCTION GLOBAL P_ActivateHourGlass
VAR_OUTPUT
	EAX : UDINT;
END_VAR;


FUNCTION GLOBAL P_DeactivateHourGlass
VAR_OUTPUT
	EAX : UDINT;
END_VAR;



FUNCTION GLOBAL P_CopyBitmap
VAR_INPUT
    thisobj : ^_NEWSCREEN;
END_VAR
VAR_OUTPUT
	EAX : UDINT;
END_VAR;
	

FUNCTION GLOBAL P_CreateRawBuf
VAR_INPUT
  usWidth	: UDINT;
  usHeight	: UDINT;
END_VAR
VAR_OUTPUT
  retval	: pVoid;
END_VAR;
	

FUNCTION GLOBAL P_FreeRawBuf
VAR_INPUT
  pRawBuf	: pVoid;
END_VAR
VAR_OUTPUT
  retval	: DINT;
END_VAR;


FUNCTION GLOBAL P_DisplayRaw
VAR_INPUT
  rawFmt 	: UDINT;
  pRaw 	: pVoid;
  srcOffx 	: UDINT;
  srcOffy 	: UDINT;
  srcWidth 	: UDINT;
  srcHeight 	: UDINT;
  tgtImg 	: pVoid;
  tgtOffx 	: UDINT;
  tgtOffy 	: UDINT;
  tgtWidth 	: UDINT;
  tgtHeight 	: UDINT;
  pRawBuf	: pVoid;
END_VAR
VAR_OUTPUT
  retval 	: DINT;
END_VAR;


// use static pointers to access excessivly often called grafic functions
// access with 2 de-referenced pointers is too slow
// these pointers must be set before accessing any of the functions
// exception: myInitGraph(), myGetBuffer(), myLCDContrast() <-- will not be called too often
VAR_EXTERNAL	
	ppmyGetXSize 				: pvoid;
	ppmyGetYSize 				: pvoid;
	ppmyGetPixel 				: pvoid;
	ppmyGetImage 				: pvoid;
	ppmyGetImageSize 		: pvoid;
	ppmyLoadUserFont 		: pvoid;
	ppmyTextDimensions 	: pvoid;
	ppmyRefresh 				: pvoid;
	ppmyRefreshDone 		: pvoid;
	ppmyClrScr 					: pvoid;
	ppmyPutPixel 				: pvoid;
	ppmyLine 						: pvoid;
	ppmyPolygon 				: pvoid;
	ppmyRectangle 			: pvoid;
	ppmyBar 						: pvoid;
	ppmyOutTextXY 			: pvoid;
	ppmyScroll 					: pvoid;
	ppmyPutImage 				: pvoid;
	ppmyBitmap 					: pvoid;
	ppmyMakeRegion 			: pvoid;
	ppmyKillRegion 			: pvoid;
	ppmyEmptyRegion 		: pvoid;
	ppmyActivateRegion 	: pvoid;
	ppmyDefaultRegion 	: pvoid;
	ppmyAndRegion 			: pvoid;
	ppmyOrRegion 				: pvoid;
	ppmyXorRegion 			: pvoid;
	ppmyBarRegion 			: pvoid;
	
	ppmyGetXSizeEx 			: pvoid;
	ppmyGetYSizeEx 			: pvoid;
	ppmyGetPixelEx 			: pvoid;
	ppmyGetImageEx 			: pvoid;
	ppmyGetImageSizeEx 	: pvoid;
	ppmyLoadUserFontEx 	: pvoid;
	ppmyTextDimensionsEx: pvoid;
	ppmyRefreshEx 			: pvoid;
	ppmyRefreshDoneEx		: pvoid;
	ppmyClrScrEx 				: pvoid;
	ppmyPutPixelEx 			: pvoid;
	ppmyLineEx 					: pvoid;
	ppmyPolygonEx 			: pvoid;
	ppmyRectangleEx 		: pvoid;
	ppmyBarEx 					: pvoid;
	ppmyOutTextXYEx 		: pvoid;
	ppmyScrollEx 				: pvoid;
	ppmyPutImageEx 			: pvoid;
	ppmyBitmapEx 				: pvoid;
	ppmyMakeRegionEx 		: pvoid;
	ppmyKillRegionEx 		: pvoid;
	ppmyEmptyRegionEx 	: pvoid;
	ppmyActivateRegionEx: pvoid;
	ppmyDefaultRegionEx : pvoid;
	ppmyAndRegionEx 		: pvoid;
	ppmyOrRegionEx 			: pvoid;
	ppmyXorRegionEx 		: pvoid;
	ppmyBarRegionEx 		: pvoid;	
end_var

#define OS_SGraphic_InitGraph(p1,p2,p3)	_LSL_POS^.piLSE^.pmyInitGraph $ P_InitGraph(p1,p2,p3)
#define OS_SGraphic_GetBuffer()					_LSL_POS^.piLSE^.pmyGetBuffer $ P_GetBuffer()
#define OS_SGraphic_Invert(p1)          _LSL_POS^.piLSE^.pmyInvert $ P_Invert(p1) //ws: invert



#define OS_SGraphic_InitHourGlass(p1,p2,p3,p4,p5,p6,p7)   _LSL_POS^.piLSE^.pmyInitHourGlass $ P_InitHourGlass(p1,p2,p3,p4,p5,p6,p7) 
#define OS_SGraphic_ActivateHourGlass()                   _LSL_POS^.piLSE^.pmyActivateHourGlass $ P_ActivateHourGlass() 
#define OS_SGraphic_DeactivateHourGlass()                 _LSL_POS^.piLSE^.pmyDeactivateHourGlass $ P_DeactivateHourGlass() 
#define OS_SGraphic_CopyBitmap(p1)                        _LSL_POS^.piLSE^.pmyCopyBitmap $ P_CopyBitmap(p1) 
#define OS_SGraphic_CreateRawBuf(p1,p2)				  _LSL_POS^.piLSE^.pCreateRawBuf $ P_CreateRawBuf(p1,p2)
#define OS_SGraphic_FreeRawBuf(p1)                        _LSL_POS^.piLSE^.pFreeRawBuf $ P_FreeRawBuf(p1)
#define OS_SGraphic_DisplayRaw(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)      _LSL_POS^.piLSE^.pDisplayRaw $ P_DisplayRaw(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)


//General:
#define OS_SGraphic_GetXSize()									ppmyGetXSize $ P_GetXSize()
#define OS_SGraphic_GetYSize()									ppmyGetYSize $ P_GetYSize()
#define OS_SGraphic_ReadPixel(p1,p2)						ppmyGetPixel $ P_GetPixel(p1,p2)
#define OS_SGraphic_GetImage(p1,p2,p3,p4,p5)		ppmyGetImage $ P_GetImage(p1,p2,p3,p4,p5)
#define OS_SGraphic_GetImageSize(p1,p2,p3,p4)		ppmyGetImageSize $ P_GetImageSize(p1,p2,p3,p4)
#define OS_SGraphic_LoadUserFont(p1)						ppmyLoadUserFont $ P_LoadUserFont(p1)
#define OS_SGraphic_TextDimensions(p1,p2,p3,p4)	ppmyTextDimensions $ P_TextDimensions(p1,p2,p3,p4)
#define OS_SGraphic_LCDContrast(p1,p2)					_LSL_POS^.piLSE^.pLCD_contrast $ P_LCDContrast(p1,p2)
#define OS_SGraphic_LCDRefresh(p1)							ppmyRefresh $ P_LCDRefresh(p1)
#define OS_SGraphic_LCDRefreshDone()      			ppmyRefreshDone $ P_LCDRefreshDone()
#define OS_SGraphic_TextHeight()								ppmyTextHeight $ P_TextHeight()

//Drawing:
#define OS_SGraphic_ClrScr(p1)										ppmyClrScr $ P_ClrScr(p1)
#define OS_SGraphic_WritePixel(p1,p2,p3,p4)   		ppmyPutPixel $ P_PutPixel(p1,p2,p3,p4)
#define OS_SGraphic_Line(p1,p2,p3,p4,p5,p6) 			ppmyLine $ P_Line(p1,p2,p3,p4,p5,p6)
#define OS_SGraphic_Polygon(p1,p2,p3,p4)					ppmyPolygon $ P_Polygon(p1,p2,p3,p4)
#define OS_SGraphic_Rectangle(p1,p2,p3,p4,p5,p6)	ppmyRectangle $ P_Rectangle(p1,p2,p3,p4,p5,p6)
#define OS_SGraphic_Beam(p1,p2,p3,p4,p5,p6)				ppmyBar $ P_Bar(p1,p2,p3,p4,p5,p6)
#define OS_SGraphic_OutTextXY(p1,p2,p3,p4,p5,p6)	ppmyOutTextXY $ P_OutText(p1,p2,p3,p4,p5,p6)
#define OS_SGraphic_Scroll(p1,p2,p3,p4,p5,p6,p7)	ppmyScroll $ P_Scroll(p1,p2,p3,p4,p5,p6,p7)
#define OS_SGraphic_PutImage(p1,p2,p3)						ppmyPutImage $ P_PutImage(p1,p2,p3)
#define OS_SGraphic_Bitmap(p1,p2,p3)							ppmyBitmap $ P_PutPixmap(p1,p2,p3)

//Region clipping:
#define OS_SGraphic_MakeRegion(p1)										ppmyMakeRegion $ P_MakeRegion(p1)
#define OS_SGraphic_KillRegion(p1)										ppmyKillRegion $ P_KillRegion(p1)
#define OS_SGraphic_EmptyRegion(p1,p2)								ppmyEmptyRegion $ P_EmptyRegion(p1,p2)
#define OS_SGraphic_ActivateRegion(p1)								ppmyActivateRegion $ P_ActivateRegion(p1)
#define OS_SGraphic_DefaultRegion()										ppmyDefaultRegion $ P_DefaultRegion()
#define OS_SGraphic_AndRegion(p1,p2)									ppmyAndRegion $ P_AndRegion(p1,p2)
#define OS_SGraphic_OrRegion(p1,p2)										ppmyOrRegion $ P_OrRegion(p1,p2)
#define OS_SGraphic_XorRegion(p1,p2)									ppmyXorRegion $ P_XorRegion(p1,p2)
#define OS_SGraphic_BeamRegion(p1,p2,p3,p4,p5,p6,p7)	ppmyBarRegion $ P_BeamRegion(p1,p2,p3,p4,p5,p6,p7)

#define OS_SGraphic_InitGraphEx(p1,p2,p3,p4)	_LSL_POS^.piLSE^.pmyInitGraphEx $ P_InitGraphEx(p1,p2,p3,p4)
#define OS_SGraphic_GetBufferEx(p1)					_LSL_POS^.piLSE^.pmyGetBufferEx $ P_GetBufferEx(p1)
#define OS_SGraphic_InvertEx(p1,p2)          _LSL_POS^.piLSE^.pmyInvertEx $ P_InvertEx(p1,p2) 

//General:
#define OS_SGraphic_GetXSizeEx(p1)									ppmyGetXSizeEx $ P_GetXSizeEx(p1)
#define OS_SGraphic_GetYSizeEx(p1)									ppmyGetYSizeEx $ P_GetYSizeEx(p1)
#define OS_SGraphic_ReadPixelEx(p1,p2,p3)						ppmyGetPixelEx $ P_GetPixelEx(p1,p2,p3)
#define OS_SGraphic_GetImageEx(p1,p2,p3,p4,p5,p6)		ppmyGetImageEx $ P_GetImageEx(p1,p2,p3,p4,p5,p6)
#define OS_SGraphic_GetImageSizeEx(p1,p2,p3,p4,p5)		ppmyGetImageSizeEx $ P_GetImageSizeEx(p1,p2,p3,p4,p5)
#define OS_SGraphic_LoadUserFontEx(p1,p2)						ppmyLoadUserFontEx $ P_LoadUserFontEx(p1,p2)
#define OS_SGraphic_TextDimensionsEx(p1,p2,p3,p4,p5)	ppmyTextDimensionsEx $ P_TextDimensionsEx(p1,p2,p3,p4,p5)
#define OS_SGraphic_LCDContrastEx(p1,p2,p3)					_LSL_POS^.piLSE^.pLCD_contrastEx $ P_LCDContrastEx(p1,p2,p3)
#define OS_SGraphic_LCDRefreshEx(p1,p2)							ppmyRefreshEx $ P_LCDRefreshEx(p1,p2)
#define OS_SGraphic_LCDRefreshDoneEx(p1)      			ppmyRefreshDoneEx $ P_LCDRefreshDoneEx(p1)
#define OS_SGraphic_TextHeightEx(p1)								ppmyTextHeightEx $ P_TextHeightEx(p1)

//Drawing:
#define OS_SGraphic_ClrScrEx(p1,p2)										ppmyClrScrEx $ P_ClrScrEx(p1,p2)
#define OS_SGraphic_WritePixelEx(p1,p2,p3,p4,p5)   		ppmyPutPixelEx $ P_PutPixelEx(p1,p2,p3,p4,p5)
#define OS_SGraphic_LineEx(p1,p2,p3,p4,p5,p6,p7) 			ppmyLineEx $ P_LineEx(p1,p2,p3,p4,p5,p6,p7)
#define OS_SGraphic_PolygonEx(p1,p2,p3,p4,p5)					ppmyPolygonEx $ P_PolygonEx(p1,p2,p3,p4,p5)
#define OS_SGraphic_RectangleEx(p1,p2,p3,p4,p5,p6,p7)	ppmyRectangleEx $ P_RectangleEx(p1,p2,p3,p4,p5,p6,p7)
#define OS_SGraphic_BeamEx(p1,p2,p3,p4,p5,p6,p7)				ppmyBarEx $ P_BarEx(p1,p2,p3,p4,p5,p6,p7)
#define OS_SGraphic_OutTextXYEx(p1,p2,p3,p4,p5,p6,p7)	ppmyOutTextXYEx $ P_OutTextEx(p1,p2,p3,p4,p5,p6,p7)
#define OS_SGraphic_ScrollEx(p1,p2,p3,p4,p5,p6,p7,p8)	ppmyScrollEx $ P_ScrollEx(p1,p2,p3,p4,p5,p6,p7,p8)
#define OS_SGraphic_PutImageEx(p1,p2,p3,p4)						ppmyPutImageEx $ P_PutImageEx(p1,p2,p3,p4)
#define OS_SGraphic_BitmapEx(p1,p2,p3,p4)							ppmyBitmapEx $ P_PutPixmapEx(p1,p2,p3,p4)

//Region clipping:
#define OS_SGraphic_MakeRegionEx(p1,p2)										ppmyMakeRegionEx $ P_MakeRegionEx(p1,p2)
#define OS_SGraphic_KillRegionEx(p1,p2)										ppmyKillRegionEx $ P_KillRegionEx(p1,p2)
#define OS_SGraphic_EmptyRegionEx(p1,p2,p3)								ppmyEmptyRegionEx $ P_EmptyRegionEx(p1,p2,p3)
#define OS_SGraphic_ActivateRegionEx(p1,p2)								ppmyActivateRegionEx $ P_ActivateRegionEx(p1,p2)
#define OS_SGraphic_DefaultRegionEx(p1)										ppmyDefaultRegionEx $ P_DefaultRegionEx(p1)
#define OS_SGraphic_AndRegionEx(p1,p2,p3)									ppmyAndRegionEx $ P_AndRegionEx(p1,p2,p3)
#define OS_SGraphic_OrRegionEx(p1,p2,p3)										ppmyOrRegionEx $ P_OrRegionEx(p1,p2,p3)
#define OS_SGraphic_XorRegionEx(p1,p2,p3)									ppmyXorRegionEx $ P_XorRegionEx(p1,p2,p3)
#define OS_SGraphic_BeamRegionEx(p1,p2,p3,p4,p5,p6,p7,p8)	ppmyBarRegionEx $ P_BeamRegionEx(p1,p2,p3,p4,p5,p6,p7,p8)

//HFILTER:1
#endif // __RTOS_INTERFACES_HEADER__97757206_BFE3_4A9D_BC11_B80C76042F7C__INCLUDED_
//HFILTER:
