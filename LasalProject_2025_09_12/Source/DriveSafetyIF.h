#ifndef DRIVESAFETYIF_H
#pragma once
#define DRIVESAFETYIF_H

#define DRIVESAFETYIF_PROTOCOLVERSION_1    1

//*************************************************************************************************
//** NewInst commandos from user classes (must be compatible with old ones see SDDDefinitions.h) **
//*************************************************************************************************

#define DRIVESAFETYIF_NEWINSTPDO               100
#define DRIVESAFETYIF_READPDODATA              101
#define DRIVESAFETYIF_WRITEPDODATA             102

//*************************************************************************************************
//** NewInst commandos from Para classes (must be compatible with MDD2000MngBase.h)              **
//*************************************************************************************************
#define DRIVESAFETYIF_ADD_ASYNC_ENTRY_DS402                 16#856B
#define DRIVESAFETYIF_ADD_ASYNC_ENTRY_DS402_VER_1           1                   //version 1
#define DRIVESAFETYIF_ADD_ASYNC_ENTRY_DS402_VER_MAX         DRIVESAFETYIF_ADD_ASYNC_ENTRY_DS402_VER_1   //newest implemented version

#define DRIVESAFETYIF_IS_ONLINE                             16#856C
#define DRIVESAFETYIF_IS_ONLINE_VER_1                       1                   //version 1
#define DRIVESAFETYIF_IS_ONLINE_VER_MAX                     DRIVESAFETYIF_IS_ONLINE_VER_1   //newest implemented version

#define DRIVESAFETYIF_GET_CLASS_STATE                       16#8574
#define DRIVESAFETYIF_GET_CLASS_STATE_VER_1                 1                   //version 1
#define DRIVESAFETYIF_GET_CLASS_STATE_VER_MAX               DRIVESAFETYIF_GET_CLASS_STATE_VER_1   //newest implemented version

#define DRIVESAFETYIF_IS_HARDWARE_AVAILABLE                 16#8575
#define DRIVESAFETYIF_IS_HARDWARE_AVAILABLE_VER_1           1                   //version 1
#define DRIVESAFETYIF_IS_HARDWARE_AVAILABLE_VER_MAX         DRIVESAFETYIF_IS_HARDWARE_AVAILABLE_VER_1   //newest implemented version

#define DRIVESAFETYIF_ASYNC_READY                           16#8576
#define DRIVESAFETYIF_ASYNC_READY_VER_1                     1                   //version 1
#define DRIVESAFETYIF_ASYNC_READY_VER_MAX                   DRIVESAFETYIF_ASYNC_READY_VER_1   //newest implemented version

#define DRIVESAFETYIF_ASYNC_ERROR                           16#8577
#define DRIVESAFETYIF_ASYNC_ERROR_VER_1                     1                   //version 1
#define DRIVESAFETYIF_ASYNC_ERROR_VER_MAX                   DRIVESAFETYIF_ASYNC_ERROR_VER_1   //newest implemented version

#define DRIVESAFETYIF_REFRESH_PARA_OBJECTS                  16#8578
#define DRIVESAFETYIF_REFRESH_PARA_OBJECTS_VER_1            1                   //version 1
#define DRIVESAFETYIF_REFRESH_PARA_OBJECTS_VER_MAX          DRIVESAFETYIF_REFRESH_PARA_OBJECTS_VER_1   //newest implemented version

#define DRIVESAFETYIF_ADD_PARA_OBJECT                       16#857B
#define DRIVESAFETYIF_ADD_PARA_OBJECT_VER_1                 1                   //version 1
#define DRIVESAFETYIF_ADD_PARA_OBJECT_VER_MAX               DRIVESAFETYIF_ADD_PARA_OBJECT_VER_1   //newest implemented version

#define DRIVESAFETYIF_IS_AXIS_READY_4_ASYPARA               16#857C
#define DRIVESAFETYIF_IS_AXIS_READY_4_ASYPARA_VER_1         1                   //version 1
#define DRIVESAFETYIF_IS_AXIS_READY_4_ASYPARA_VER_MAX       DRIVESAFETYIF_IS_AXIS_READY_4_ASYPARA_VER_1   //newest implemented version

#define DRIVESAFETYIF_UPDATEPARACLASS_STATE                 16#857E
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_VER_1           1                   //version 1
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_VER_MAX         DRIVESAFETYIF_UPDATEPARACLASS_STATE_VER_1   //newest implemented version

#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_GENERAL_ERROR                 1
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_ASYNC_BUSY                    3
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_GENERAL_READY                 5
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_UPDATE_CY                     6
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_SAFETY_READY                  10 
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_SAFETY_ERROR                  11
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_SAFETY_CHANGED                12
#define DRIVESAFETYIF_UPDATEPARACLASS_STATE_SAFETY_SETSERVICEMODE_PENDING 20

//*****************************************************************************
//** NewInst 0x8590-0x859F                                                   **
//*****************************************************************************

//*****************************************************************************
#define DRIVESAFETYIF_READAXISSTATE                     0x8590
#define DRIVESAFETYIF_READAXISSTATE_VER_1                    1                   //version 1
#define DRIVESAFETYIF_READAXISSTATE_VER_MAX             DRIVESAFETYIF_READAXISSTATE_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1  

//  results
//  uiLng         : 8 Byte
//  aData[0]$DINT : Command Version : max supported version
//  aData[4]$_MDD2000AXISSTATE : AxisState

//  retcode
//  READY
//  ERROR

//*****************************************************************************
#define DRIVESAFETYIF_DOAXERRORQUIT                     0x8591
#define DRIVESAFETYIF_DOAXERRORQUIT_VER_1                    1                   //version 1
#define DRIVESAFETYIF_DOAXERRORQUIT_VER_MAX             DRIVESAFETYIF_DOAXERRORQUIT_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1
//  aPara[1]$DINT     : 1 Quit Error

//  results
//  uiLng         : 8 Byte
//  aData[0]$DINT : Command Version : max supported version
//  aData[4]$DINT : Server AxErrorQuit


//  retcode
//  READY
//  BUSY
//  ERROR

//*****************************************************************************
#define DRIVESAFETYIF_DOSAFETYAXERRORQUIT               0x8592
#define DRIVESAFETYIF_DOSAFETYAXERRORQUIT_VER_1              1                   //version 1
#define DRIVESAFETYIF_DOSAFETYAXERRORQUIT_VER_MAX       DRIVESAFETYIF_DOSAFETYAXERRORQUIT_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1
//  aPara[1]$DINT     : Type of Quit error, see below
//  aPara[2]$DINT     : AxIndex, Not needed when NewIst of axis class is used, (Needed if NewInst if Mng is used.)

//  results
//  uiLng         : 4 Byte (sizeof(DINT))
//  aData[0]$DINT : Command Version : max supported version

//  retcode
//  READY
//  BUSY
//  ERROR

#define DRIVESAFETYIF_RESTART_ACK     1 // Restart Ackknowledge, used for STO, SOS, SS1, SS2
#define DRIVESAFETYIF_ERROR_ACK       2 // Error Ackknowledge, used when an error occoured in a safety function
#define DRIVESAFETYIF_ERROR_QUIT      4 // Quits an safety error on the drive, example: MDD2000_Mng.SafetyRunState = _ERROR

//*****************************************************************************
#define DRIVESAFETYIF_SETVERIFYAXIS                     0x8593
#define DRIVESAFETYIF_SETVERIFYAXIS_VER_1                    1                   //version 1
#define DRIVESAFETYIF_SETVERIFYAXIS_VER_MAX             DRIVESAFETYIF_SETVERIFYAXIS_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1
//  aPara[1]$DINT     : 0 .. Read VerifiedState from Drive, -1 .. Reset VerifiedState of all axes and Reset ReferenceState of this axis, -2 .. Reset VerifiedState of all axes, 1 .. SetVerified and Read VerifiedState from Drive

//  results
//  uiLng         : 4 Byte (sizeof(DINT))
//  aData[0]$DINT : Command Version : max supported version

//  retcode
//  READY
//  BUSY
//  ERROR

//*****************************************************************************
#define DRIVESAFETYIF_GETVERIFYSTATE                    0x8594
#define DRIVESAFETYIF_GETVERIFYSTATE_VER_1                   1                   //version 1
#define DRIVESAFETYIF_GETVERIFYSTATE_VER_MAX            DRIVESAFETYIF_GETVERIFYSTATE_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1

//  results
//  uiLng               : 20 Byte
//  aData[0]$DINT       : Command Version : max supported version
//  aData[4]$iprStates  : VERIFY CMDRSP Executionstate: READY .. Done, Busy, Error .. Asy Comm Failed 
//  aData[8]$DINT       : VERIFY CMDRSP, see below
//  aData[12]$iprStates : VERIFY STATE Executionstate: READY .. Done, Busy, Error .. Asy Comm Failed
//  aData[16]$DINT      : VERIFY STATE, see below

//  retcode
//  READY
//  BUSY
//  ERROR

// VERIFY CMDRSP
// General
#define DRIVESAFETYIF_VERCMD_OK                     0x40000000  // Command successfully processed
#define DRIVESAFETYIF_VERCMD_BUSY                   0x0000000B  // Response to command has not yet been recived
#define DRIVESAFETYIF_VERCMD_EXECUTING              0x4000000B  // Commmand ist still beeing processed ins safety FW
#define DRIVESAFETYIF_VERCMD_WRONG_RUNSTATE         0x40000003  // Command not allowed in current state
#define DRIVESAFETYIF_VERCMD_NOT_NEEDED             0x40000004  // Command executed, although no verification is necessary

#define DRIVESAFETYIF_VERCMD_ERROR_ENC_ID_PROG      0x40000005  // Programming of the encoder index failed

#define DRIVESAFETYIF_VERCMD_INCOMPLETE             0x40000001  // Not all axes requiring verification, have been verified
#define DRIVESAFETYIF_VERCMD_ERROR                  0x40000002  // Saving the HDSL verification status has failed

// VERIFY STATE
#define DRIVESAFETYIF_VERSTATE_UNKOWN               0x00000000  // State has not yet been read
#define DRIVESAFETYIF_VERSTATE_BUSY                 0x0000000B  // State is beeing read

#define DRIVESAFETYIF_VERSTATE_NOT_VERIFIED         0x1A8C5E7B  // Axis not verified, verification necessary                                                  
#define DRIVESAFETYIF_VERSTATE_SYSTEM_VERIFIED      0x2B9D6F8C  // Entire drive fully verified
#define DRIVESAFETYIF_VERSTATE_NOT_USED             0x30000000  // no verification necessary for this axis, as not configured as Hiperface DSL

#define DRIVESAFETYIF_VERSTATE_VERIFICATION_PENDING 0x30000001  // Verification has been started for this axis, Programming of the encoder index in progress
#define DRIVESAFETYIF_VERSTATE_AXIS_VERIFIED        0x30000002  // Encoder index has been successfully programmed, Verification completed for this axis (Overall verification necessary at the end)



//*****************************************************************************
#define DRIVESAFETYIF_SETREFERENCEAXIS                  0x8595
#define DRIVESAFETYIF_SETREFERENCEAXIS_VER_1                 1                   //version 1
#define DRIVESAFETYIF_SETREFERENCEAXIS_VER_MAX          DRIVESAFETYIF_SETREFERENCEAXIS_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1
//  aPara[1]$DINT     : 0 .. Read ReferenceState from Drive, -1 .. Reset ReferenceState of this axis, 1 .. SetReference and Read ReferenceState from Drive

//  results
//  uiLng         : 4 Byte (sizeof(DINT))
//  aData[0]$DINT : Command Version : max supported version

//  retcode
//  READY
//  BUSY
//  ERROR

//*****************************************************************************
#define DRIVESAFETYIF_GETREFERENCESTATE                 0x8596
#define DRIVESAFETYIF_GETREFERENCESTATE_VER_1                1                   //version 1
#define DRIVESAFETYIF_GETREFERENCESTATE_VER_MAX         DRIVESAFETYIF_GETREFERENCESTATE_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1

//  results
//  uiLng               : 20 Byte
//  aData[0]$DINT       : Command Version : max supported version
//  aData[4]$iprStates  : REFERENCE CMDRSP Executionstate: READY .. Done, Busy, Error .. Asy Comm Failed 
//  aData[8]$DINT       : REFERENCE CMDRSP, see below
//  aData[12]$iprStates : REFERENCE STATE Executionstate: READY .. Done, Busy, Error .. Asy Comm Failed
//  aData[16]$DINT      : REFERENCE STATE, see below

//  retcode
//  READY
//  BUSY
//  ERROR

// REFERENCE CMD
// Not used by NewInst Interface
#define DRIVESAFETYIF_REFCMD_SETZERO_POS            0x00000001  // Starts the procedure for absolute position referencing, current position is saved as zero position
#define DRIVESAFETYIF_REFCMD_ACKZERO_POS            0x00000002  // Must be executed when the zero position has been approached again in order to confirm and permanently store it, sequence is then completed
#define DRIVESAFETYIF_REFCMD_ABORT                  0x0000000A  // Aborts the previously started referencing process
#define DRIVESAFETYIF_REFCMD_RESET                  0x0000000F  // Resets the axis, in order to be able to carry out a new referencing afterwards.

// REFERENCE CMDRSP
// General
#define DRIVESAFETYIF_REFCMD_OK                     0x40000000  // Command successfully processed
#define DRIVESAFETYIF_REFCMD_BUSY                   0x0000000B  // Response to command has not yet been recived
#define DRIVESAFETYIF_REFCMD_PROGRAMMING_FAILED     0x40000002  // The saving of the offset or the referencing status has failed.
#define DRIVESAFETYIF_REFCMD_WRONG_RUNSTATE         0x40000003  // Command not allowed in current state
#define DRIVESAFETYIF_REFCMD_NOT_NEEDED             0x40000004  // Command executed, although no reference is necessary

// For DRIVESAFETYIF_REFCMD_ACKZERO_POS
#define DRIVESAFETYIF_REFCMD_NOT_ALLOWED            0x40000010  // Absolute position referencing was not started, command not yet allowed
#define DRIVESAFETYIF_REFCMD_NO_MOVEMENT            0x40000011  // Position has not moved far enough from the zero position since the start of referencing
#define DRIVESAFETYIF_REFCMD_WRONG_POSITION         0x40000012  // Newly approached zero position deviates more than permitted from initially approached zero position


// REFERENCE STATE
#define DRIVESAFETYIF_REFSTATE_UNKOWN               0x00000000  // 

#define DRIVESAFETYIF_REFSTATE_BUSY                 0x0000000B  // State is beeing read

#define DRIVESAFETYIF_STATE_NOT_REFERENCED          0x1A8C5E7B  // Axis not referenced, referencing necessary
#define DRIVESAFETYIF_STATE_AXIS_REFERENCED         0x2B9D6F8C  // Axis fully referenced
#define DRIVESAFETYIF_STATE_NOT_NEEDED              0x30000000  // No referencing necessary for this axis

#define DRIVESAFETYIF_STATE_WAIT_MOVEMENT           0x30000001  // Referencing has been started, it is waiting for the waiting for the current position to move away from the zero position
#define DRIVESAFETYIF_STATE_ACK_PENDING             0x30000002  // Current position far enough away from zero position, it waits for the zero position to be approached again and confirmed
#define DRIVESAFETYIF_STATE_REFERENCE_DONE          0x30000003  // Zero position successfully confirmed, referencing completed


//*****************************************************************************
#define DRIVESAFETYIF_SETACKZEROPOSITION                0x8597
#define DRIVESAFETYIF_SETACKZEROPOSITION_VER_1               1                   //version 1
#define DRIVESAFETYIF_SETACKZEROPOSITION_VER_MAX        DRIVESAFETYIF_SETACKZEROPOSITION_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1

//  results
//  uiLng         : 4 Byte (sizeof(DINT))
//  aData[0]$DINT : Command Version : max supported version
//  aPara[1]$DINT     : 0 .. Read ReferenceState from Drive, -1 .. Abort a prevously set DRIVESAFETYIF_SETREFERENCEAXIS cmd, 1 .. Ack Zeropost and Read ReferenceState from Drive

//  retcode
//  READY
//  BUSY
//  ERROR


//*****************************************************************************
#define DRIVESAFETYIF_GETSBTSTATE                       0x8598
#define DRIVESAFETYIF_GETSBTSTATE_VER_1                      1                   //version 1
#define DRIVESAFETYIF_GETSBTSTATE_VER_MAX               DRIVESAFETYIF_GETSBTSTATE_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1

//  results
//  uiLng               : 4, 8 or 24 Byte
//  aData[0]$DINT       : Command Version : max supported version
//  aData[4]$iprStates  : State of the Asy accesses, READY Data is valid, BUSY .. Data is still beeing read, ERROR .. An error occured during the asy communication (Only valid if uiLng >= 8 )
//  aData[8]$DINT       : SafeBrakeTestStatus (Only valid if uiLng >= 24 )
//  aData[12]$DINT      : BrakeTestStatus     (Only valid if uiLng >= 24 )
//  aData[16]$DINT      : TimeTilNextTest     (Only valid if uiLng >= 24 )
//  aData[20]$DINT      : BrakeTestErrorInfo  (Only valid if uiLng >= 24 )

//  retcode
//  READY 
//  BUSY
//  ERROR

//*****************************************************************************
#define DRIVESAFETYIF_SETSBTSTATE                       0x8599
#define DRIVESAFETYIF_SETSBTSTATE_VER_1                      1                   //version 1
#define DRIVESAFETYIF_SETSBTSTATE_VER_MAX               DRIVESAFETYIF_SETSBTSTATE_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT  : Command Version : 1
//  aPara[1]$DINT  : 0 .. Refresh teststates from the safety brake test, -1 .. Cancel active safety brake test, 1 ... Start a new safety brake test

//  results
//  uiLng         : 4 Byte (sizeof(DINT))
//  aData[0]$DINT : Command Version : max supported version

//  retcode
//  READY
//  BUSY
//  ERROR

//*****************************************************************************
#define DRIVESAFETYIF_GETGENERALPARA                    0x859A
#define DRIVESAFETYIF_GETGENERALPARA_VER_1                   1                   //version 1
#define DRIVESAFETYIF_GETGENERALPARA_VER_MAX            DRIVESAFETYIF_GETGENERALPARA_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT  :  Command Version : 1
//  aPara[1]$DINT  :  <> 0 .. Set New Request, 0 .. Poll Response of last request    
//  aPara[2]$UINT  :  ParaID      .. must not be changed between request and polling the response
//  aPara[3]$USINT :  ParaSubID   .. must not be changed between request and polling the response
//  aPara[4]$USINT :  0 Read Parameter, 1 Write Parameter
//  aPara[5]$^void := Pointer to Read / Write Data, Read Data is returen with Poll response and retcode = READY
//  aPara[6]$DINT  := size of parameter, 0 .. 4 Byte, .. 1 8 Byte, 2 .. 12 Byte

//  results
//  uiLng         : 4 Byte (sizeof(DINT))
//  aData[0]$DINT : Command Version : max supported version

//  retcode
//  READY
//  BUSY
//  ERROR_BUSY    : A new Request was set before the last request could be completed
//  ERROR
    
    

//*****************************************************************************
#define DRIVESAFETYIF_GETSAFETYERRORINFOS               0x859B
#define DRIVESAFETYIF_GETSAFETYERRORINFOS_VER_1              1                   //version 1
#define DRIVESAFETYIF_GETSAFETYERRORINFOS_VER_MAX       DRIVESAFETYIF_GETSAFETYERRORINFOS_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1
//  aPara[1]$DINT     : 0 .. Read previously Error Infos from HW-Class, else Start reading Error Infos from drive to HW-Class

//  results
//  uiLng         : 4, 36
//  aData[0]$DINT  : Command Version : max supported version
//  aData[4]$DINT  : SafetyErrorCode_SC1         .. Only Valid if uiLng = 36 and retcode = READY
//  aData[8]$DINT  : SafetyErrorInformation1_SC1 .. Only Valid if uiLng = 36 and retcode = READY
//  aData[12]$DINT : SafetyErrorInformation2_SC1 .. Only Valid if uiLng = 36 and retcode = READY
//  aData[16]$DINT : SafetyErrorInformation3_SC1 .. Only Valid if uiLng = 36 and retcode = READY
//  aData[20]$DINT : SafetyErrorCode_SC2         .. Only Valid if uiLng = 36 and retcode = READY
//  aData[24]$DINT : SafetyErrorInformation1_SC2 .. Only Valid if uiLng = 36 and retcode = READY
//  aData[28]$DINT : SafetyErrorInformation2_SC2 .. Only Valid if uiLng = 36 and retcode = READY
//  aData[32]$DINT : SafetyErrorInformation3_SC2 .. Only Valid if uiLng = 36 and retcode = READY

//  retcode
//  READY
//  BUSY
//  ERROR

//*****************************************************************************
#define DRIVESAFETYIF_GETSAFEFUNCTIONERRORINFOS         0x859C
#define DRIVESAFETYIF_GETSAFEFUNCTIONERRORINFOS_VER_1        1                   //version 1
#define DRIVESAFETYIF_GETSAFEFUNCTIONERRORINFOS_VER_MAX DRIVESAFETYIF_GETSAFEFUNCTIONERRORINFOS_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1
//  aPara[1]$DINT     : 0 .. Read previously Error Infos from HW-Class, else Start reading Error Infos from drive to HW-Class

//  results
//  uiLng         : 4, 40 Byte
//  aData[0]$DINT : Command Version : max supported version            
//  aData[4]$DINT  := SafeErrorWord             .. Only Valid if uiLng = 40 and retcode = READY
//  aData[8]$DINT  := SafeFunctionErrorID       .. Only Valid if uiLng = 40 and retcode = READY
//  aData[12]$DINT := SafeFunctionInErrID       .. Only Valid if uiLng = 40 and retcode = READY
//  aData[16]$DINT := SafeFunctionInErrSubID    .. Only Valid if uiLng = 40 and retcode = READY
//  aData[20]$DINT := SafeFunctionErrReactID    .. Only Valid if uiLng = 40 and retcode = READY
//  aData[24]$DINT := SafeFunctionErrReactSubID .. Only Valid if uiLng = 40 and retcode = READY
//  aData[28]$DINT := SafeFunctionErrorInfo1    .. Only Valid if uiLng = 40 and retcode = READY
//  aData[32]$DINT := SafeFunctionErrorInfo2    .. Only Valid if uiLng = 40 and retcode = READY
//  aData[36]$DINT := SafeFunctionErrorInfo3    .. Only Valid if uiLng = 40 and retcode = READY

//  retcode
//  READY
//  BUSY
//  ERROR

//*****************************************************************************
#define DRIVESAFETYIF_GETSAFECONTROLSTATEWORD           0x859D
#define DRIVESAFETYIF_GETSAFECONTROLSTATEWORD_VER_1          1                   //version 1
#define DRIVESAFETYIF_GETSAFECONTROLSTATEWORD_VER_MAX DRIVESAFETYIF_GETSAFECONTROLSTATEWORD_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1

//  results
//  uiLng         : 4, 12, 16 Byte
//  aData[0]$DINT : Command Version : max supported version            
//  aData[4]$DINT := SafeControlWord                         .. Only Valid if uiLng >= 12 and retcode = READY
//  aData[8]$DINT := SafeStateWord                           .. Only Valid if uiLng >= 12 and retcode = READY
//  aData[12]$MDD2000_AxisBase::gb_MDD2000ActiveSafeFkt := ActiveSafeFunctions  
//                                                           .. Only Valid if uiLng = 16  and retcode = READY, 
//                                                           .. The mapping info for the Safe Function is read asynchrone, 
//                                                           .. therefore ActiveSafeFunctions is not immediately available even if SafeControlWord and SafeStateWord are valid


//*****************************************************************************
#define DRIVESAFETYIF_CHANGESAFESERVICEMODE             0x859E
#define DRIVESAFETYIF_CHANGESAFESERVICEMODE_VER_1            1                   //version 1
#define DRIVESAFETYIF_CHANGESAFESERVICEMODE_VER_MAX DRIVESAFETYIF_CHANGESAFESERVICEMODE_VER_1   //newest implemented version
//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1
//  aPara[1]$DINT     : Requested Change: 0 .. No Change, 1 .. Enter service mode, 2 .. Leave service mode

//  results
//  uiLng         : 8 Byte
//  aData[0]$DINT : Command Version : max supported version
//  aData[4]$t_e_MDD2000SafetyRunState : SafetyRunState

//  retcode
//  READY
//  ERROR

//*****************************************************************************
//** State for cmd, VERIFYSTATE, REFERENCESTATE                              **
//*****************************************************************************

#define DRIVESAFETYIF_COMM_ERROR                    0x80000001

#define DRIVESAFETYIF_GET_AXISERROR                       0x859F
#define DRIVESAFETYIF_GET_AXISERROR_VER_1                 1                   //version 1
#define DRIVESAFETYIF_GET_AXISERROR_VER_MAX               DRIVESAFETYIF_GET_AXISERROR_VER_1   //newest implemented version

//  Command Version 1 **  
//  CmdStruct
//  aPara[0]$DINT     : Command Version : 1

//  results
//  uiLng         : 8 Byte
//  aData[0]$DINT : Command Version : max supported version
//  aData[4]$DINT : AxError

//  retcode
//  READY
//  ERROR

//*****************************************************************************
//** TYPES                                                                   **
//*****************************************************************************
TYPE
  // Global enum to define SafetyRunState structure to assign values from HWK
  t_e_DRIVESAFETYIF_SafetyRunState :
  (
    DRIVESAFETYIF_SRS_POST:=0,
    DRIVESAFETYIF_SRS_CHK_CFG:=1,
    DRIVESAFETYIF_SRS_PREOP:=2,
    DRIVESAFETYIF_SRS_TEMP_OP:=3,
    DRIVESAFETYIF_SRS_OP:=4,
    DRIVESAFETYIF_SRS_SERVICE:=5,
    DRIVESAFETYIF_SRS_ERROR:=6,
    DRIVESAFETYIF_SRS_INACTIVE:=255,
    DRIVESAFETYIF_SRS_FSOE_COMSETUPFAILED:=1073741824
  )$UDINT;
  // Global bit field to define Safefunction ID to assign values from HWK
  t_e_DRIVESAFETYIF_ActiveSafeFnct : BDINT
  [
    1 STO,
    2 SS1,
    3 SS2,
    4 SOS,
    5 SBC,
    6 SBT,
    7 SLS,
    8 SLP,
    9 SLA,
    10 SLT,
    11 SLI,
    12 SDI,
    13 SMS,
    14 SMP,
    15 SMA,
    16 SMT,
    17 SSM,
    18 SCA,
  ];
	  t_DRIVESAFETYIF_STATEWORD : BSINT  //! <Type Comment="Structure for Pdo read stateword.&#13;&#10;If structure changes, also the version number in the newinst command SDD_NEWINSTPDO has to be incremented." Name="_MDD2000STATEWORD"/>
	  [
	    1 ReadyToSwitchOn,
	    2 SwitchedOn,
	    3 OperationEnabled,
	    4 Fault,
	  ];
#pragma pack(push, 1)
	  t_DRIVESAFETYIF_PDOREAD : STRUCT  //! <Type Comment="Structure for PDO Read Data.&#13;&#10;If structure changes, also the version number in the newinst command SDD_NEWINSTPDO has to be incremented." Name="_MDD2000PDOREAD"/>
	    Online : BOOL;
	    StateWord : t_DRIVESAFETYIF_STATEWORD;
	    ActPos : DINT;
	    ActXw : DINT;
	  END_STRUCT;
#pragma pack(pop)
#pragma pack(push, 1)
	  t_DRIVESAFETYIF_PDOWRITENTRY : STRUCT  //! <Type Comment="Structure for PDO Write Entry Data.&#13;&#10;If structure changes, also the version number in the newinst command SDD_NEWINSTPDO has to be incremented." Name="_MDD2000PDOWRITENTRY"/>
	    Active : UINT;
	    Value : DINT;
	  END_STRUCT;
#pragma pack(pop)
#pragma pack(push, 1)
	  t_DRIVESAFETYIF_PDOWRITE : STRUCT  //! <Type Comment="Structure for PDO Write Data.&#13;&#10;If structure changes, also the version number in the newinst command SDD_NEWINSTPDO has to be incremented." Name="_MDD2000PDOWRITE"/>
	    SetPos : t_DRIVESAFETYIF_PDOWRITENTRY;
	    Enable : t_DRIVESAFETYIF_PDOWRITENTRY;
	    SetSpeed : t_DRIVESAFETYIF_PDOWRITENTRY;
	  END_STRUCT;
#pragma pack(pop)
END_TYPE
#endif
