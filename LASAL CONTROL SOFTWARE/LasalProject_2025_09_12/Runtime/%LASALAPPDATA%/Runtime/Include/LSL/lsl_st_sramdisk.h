// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Date        Author    Comment
//   2010-03-25  pr        initial version
// ------------------------------------------------------------

//HFILTER:1
#ifndef __LSL_ST_SRAMDISK
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define __LSL_ST_SRAMDISK

(*
  Set the value of a SRAMDISK parameter (possible parameters: see CLI docu)
*)
FUNCTION __CDECL GLOBAL P_SRAMDISK_SetParam
VAR_INPUT
    key         : ^USINT;
    val         : DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

(*
  Get the value of a SRAMDISK parameter (possible parameters: see CLI docu)
*)
FUNCTION __CDECL GLOBAL P_SRAMDISK_GetParam
VAR_INPUT
    key         : ^USINT;
    pVal        : ^DINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

(*
  Write a sector to the disc area where the sram is stored (only for testing purposes !)
*)
FUNCTION __CDECL GLOBAL P_SRAMDISK_WriteSectors
VAR_INPUT
    sectorNum   : UDINT;
    sectorCnt   : UDINT;
    pData       : ^USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

(*
  Write a sector to the disc area where the sram is stored (fast version - only for testing purposes !)
*)
FUNCTION __CDECL GLOBAL P_SRAMDISK_WriteSectorsFast
VAR_INPUT
    sectorNum   : UDINT;
    sectorCnt   : UDINT;
    pData       : ^USINT;
END_VAR
VAR_OUTPUT
    retval      : DINT;
END_VAR;

(*
  Synchronous commit (write all modified sram data to disk and switch to a new version)
*)
FUNCTION __CDECL GLOBAL P_SRAMDISK_Commit
VAR_OUTPUT
    retval      : DINT;
END_VAR;

(*
  Synchronous commit (write all modified sram data to disk and switch to a new version)
*)
FUNCTION __CDECL GLOBAL P_SRAMDISK_RequestCommit
VAR_INPUT
    wait : UDINT;
END_VAR;


#define SRAMDISK_SET_PARAM(itf,p1,p2)         itf^.SetParam $ P_SRAMDISK_SetParam(p1,p2)
#define SRAMDISK_GET_PARAM(itf,p1,p2)         itf^.GetParam $ P_SRAMDISK_GetParam(p1,p2)
#define SRAMDISK_WRITE_SECTORS(itf,p1,p2,p3)  itf^.WriteSectors $ P_SRAMDISK_WriteSectors(p1,p2,p3)
#define SRAMDISK_WRITE_SECTORS_FAST(itf,p1,p2,p3)  itf^.WriteSectorsFast $ P_SRAMDISK_WriteSectorsFast(p1,p2,p3)
#define SRAMDISK_COMMIT(itf)                  itf^.Commit $ P_SRAMDISK_Commit()
#define SRAMDISK_REQUEST_COMMIT(itf,p1)       itf^.RequestCommit $ P_SRAMDISK_RequestCommit(p1)

#pragma pack (push, 1)
TYPE
    OS_SRAMDISK : STRUCT
        udVersion            : UDINT;       // version-number of this structure
        loadedAbbild         : DINT;        // diagnosis: nbr. of loaded sram abbild or -1, when none was loaded
        loadedAbbildCntr     : UDINT;       // diagnosis: counter of loaded sram abbild
        currentAbbld         : UDINT;       // diagnosis: nbr. of current sram abbild
        currentAbbildCntr    : UDINT;       // diagnosis: counter of current sram abbild
        lastErrorCode        : DINT;        // errorcode of last error
        lastReasonCode       : DINT;        // reasoncode of last error
        nbrOfModifiedSectors : UDINT;       // diagnosis: nbr of modified sectors
        nbrOfPendingSectors  : UDINT;       // diagnosis: nbr of write pending sectors
        nbrOfSectorWrites    : UDINT;       // diagnosis: nbr of sector writes since startup
        nbrOfCommits         : UDINT;       // diagnosis: nbr of commits since startup
        nbrOfUsedDiscSectors : UDINT;       // info: nbr of disc sectors used for sram save 
        nbrOfAvailableDiscSectors : UDINT;  // info: nbr of disc sectors available for sram save 
        SetParam             : pVoid;       // function ptr
        GetParam             : pVoid;       // function ptr
        WriteSectors         : pVoid;       // function ptr
        nbrOfOpenPages       : UDINT;       // diagnosis: nbr of open pages
        WriteSectorsFast     : pVoid;       // function ptr
        // since Version 2
        Commit               : pVoid;       // function ptr - synchronous commit
        RequestCommit        : pVoid;       // function ptr - asynchronous commit
        // since SRAMDISK_ITF_VERSION 3
// Flag wird gesetzt, wenn das Flag fullCopyAtPowerdown gesetzt ist, und das SRAM-Image
// nicht beim PowerDown weggeschrieben wird
#define DIAG_FLAG_NOT_WRITTEN_AT_POWER_DOWN     1
        diagFlags            : UDINT;       // Diagnose-Flags
    END_STRUCT;
END_TYPE
#pragma pack (pop)

// Errors
#define SRAMDISK_E_INVALID_PARAM          -1
#define SRAMDISK_E_NOMEM                  -2
#define SRAMDISK_E_NOMUTEX                -3
#define SRAMDISK_E_DISKWRITE              -4
#define SRAMDISK_E_DISKREAD               -5
#define SRAMDISK_E_RT                     -6
#define SRAMDISK_E_CANCELED               -7

//HFILTER:1
#endif
//HFILTER:
