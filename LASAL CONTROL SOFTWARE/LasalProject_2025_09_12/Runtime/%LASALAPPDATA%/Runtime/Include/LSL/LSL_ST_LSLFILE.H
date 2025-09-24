//* $$!FNOTE 10 */
//****************************************************************************/
//*                                                                          */
//*  File: LSL_ST_LSLFILE.H                                                  */
//*  Date: Oct. 18, 2000                                                     */
//*  By:                                                                     */
//*  Description:                                                            */
//*		This file contains the LASAL LSLFILE Interface.                		*/
//*  Revision History:                                                       */
//*      Rev     By      Date        Description                             */
//*      ---     --      ----        -----------                             */
//*      1.00    THR     10/18/00    Initial Version                         */
//*                                                                          */
//****************************************************************************/
//* $$!ENDCMD */
#ifndef _LSL_ST_LSLFILE_H
#pragma once
#define _LSL_ST_LSLFILE_H

//
//	The TASK Interface provides access to the LASAL Data File Interface.
//
//	Interface Name: LSLFILE
//	Required: NO
//	References: UNLIMITED
//

//
//	LASAL File Interface (RTKernel V3.18)
//

TYPE
  // LSLFILE Interface Structure
  LSL_LSLFILE			 : STRUCT
	// LASAL File Interface Functions
	Save		: pVoid;
	Load		: pVoid;
	SaveActive	: pVoid;
	LoadActive	: pVoid;
	Read		: pVoid;
	ReadString	: pVoid;
	Write		: pVoid;
	SetFileMask	: pVoid;
	SetActivePath : pVoid;
	GetRecordData : pVoid;
    dummy1      : pVoid;
    dummy2      : pVoid;
    dummy3      : pVoid;
    dummy4      : pVoid;
    dummy5      : pVoid;
    dummy6      : pVoid;
    dummy7      : pVoid;
    dummy8      : pVoid;
    dummy9      : pVoid;
    dummy10     : pVoid;
    dummy11     : pVoid;
    dummy12     : pVoid;
    dummy13     : pVoid;
    dummy14     : pVoid;
    dummy15     : pVoid;
    dummy16     : pVoid;
    dummy17     : pVoid;
    dummy18     : pVoid;
    dummy19     : pVoid;
    // since OS version 5.24:
    GetActiveInfo : pVoid;
    // since OS version 5.27:
    dummy20     : pVoid;
    dummy21     : pVoid;
    dummy22     : pVoid;
    dummy23     : pVoid;
    dummy24     : pVoid;
    dummy25     : pVoid;
    dummy26     : pVoid;
    dummy27     : pVoid;
    dummy28     : pVoid;
    dummy29     : pVoid;
    dummy30     : pVoid;
    dummy31     : pVoid;
    dummy32     : pVoid;
    dummy33     : pVoid;  // unsigned long bLslLoad
  END_STRUCT;


  // LSLFILE Active Info Structure
  LSLFILE_ACTIVEINFO	: STRUCT
    version     : UDINT;
    // loadinfo: indicates from which file the active file has been loaded:
    //  <0: Active file could not be loaded (error)
    //   0: Active file is not loaded
    //   1: Active file has been loaded from ACTIVE.DAT
    //   2: Active file has been loaded from ACTIVE.SAV
    loadinfo    : DINT;

// since Version 1:
    // Filename: path and name of the active file
    filename    : ARRAY[0..79] of CHAR;

  END_STRUCT;

END_TYPE

//
//	File Mask Attributes
//
#define LSLFILE_ACTIVE	0x0001		// Active data file (power store)
#define LSLFILE_USER01	0x0002		// User File #1
#define LSLFILE_USER02	0x0004		// User File #2
#define LSLFILE_USER03	0x0008		// User File #3
#define LSLFILE_USER04	0x0010		// User File #4
#define LSLFILE_USER05	0x0020		// User File #5
#define LSLFILE_USER06	0x0040		// User File #6
#define LSLFILE_USER07	0x0080		// User File #7
#define LSLFILE_USER08	0x0100		// User File #8
#define LSLFILE_USER09	0x0200		// User File #9
#define LSLFILE_USER10	0x0400		// User File #10
#define LSLFILE_USER11	0x0800		// User File #11
#define LSLFILE_USER12	0x1000		// User File #12
#define LSLFILE_USER13	0x2000		// User File #13
#define LSLFILE_USER14	0x4000		// User File #14
#define LSLFILE_USER15	0x8000		// User File #15

//
// Error Codes
//
#define LSLFILE_INVALID_ACTIVEFILE      -1000
#define LSLFILE_LAST_WRITE_INCOMPLETE   -1001
#define LSLFILE_BUF_TOO_SMALL           -1002
#define LSLFILE_GENERAL_FAILURE         -1003
#define LSLFILE_PARAM_ERROR             -1004
#define LSLFILE_ACTIVESAVE_DISABLED     -1005
#define LSLFILE_FILEIO_ERROR            -1006
#define LSLFILE_SRAMDATA_NOT_VALID      -1007
#define LSLFILE_INV_SRAMIMAGE_SIZE      -1008
#define LSLFILE_NO_SRAM_AVAILABLE       -1009
#define LSLFILE_FORMAT_NEW              -1010

//
//	Prototypes and Macros
//

//
//	Macro results in TRUE if LASAL Data File System is installed
//
#define LSLFILE_IsInstalled _LSL_POS^.piLslFile<>NIL

//
//	Function to load active setup data file.
//	Called during system initialization.
//
FUNCTION GLOBAL __cdecl P_LoadActive
VAR_INPUT
  resv	: pVoid;	// Reserved parameter - ALWAYS pass as NULL!
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_LoadActive _LSL_POS^.piLslFile$^LSL_LSLFILE^.LoadActive $ P_LoadActive(NIL)

//
//	Function to save active setup data file.
//
FUNCTION GLOBAL __cdecl P_SaveActive
VAR_INPUT
  resv	: pVoid;	// Reserved parameter - ALWAYS pass as NULL!
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_SaveActive _LSL_POS^.piLslFile$^LSL_LSLFILE^.SaveActive $ P_SaveActive(NIL)

//
//	Function to save a data file.
//
FUNCTION GLOBAL __cdecl P_Save
VAR_INPUT
  path		: ^USINT;	// pointer to file path/name string
  fileatt	: UDINT;	// file mask attribute (UINT cast to UDINT)
  resv		: pVoid;	// reserved, ALWAYS PASS AS VOID
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_Save(p1,p2) _LSL_POS^.piLslFile$^LSL_LSLFILE^.Save $ P_Save(p1,p2,NIL)

//
//	Function to load a data file.
//
FUNCTION GLOBAL __cdecl P_Load
VAR_INPUT
  path		: ^USINT;	// pointer to file path/name string
  fileatt	: UDINT;	// file mask attribute (UINT cast to UDINT)
  resv		: pVoid;	// reserved, ALWAYS PASS AS VOID
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_Load(p1,p2) _LSL_POS^.piLslFile$^LSL_LSLFILE^.Load $ P_Load(p1,p2,NIL)


//
//	Function to read from the current data file.
//	Object servers can call this function to read any
//	extra data saved for the object server.
//
FUNCTION GLOBAL __cdecl P_Read
VAR_INPUT
  pdata		: pVoid;	// pointer to buffer to store the data
  dlen		: DINT;		// length of data to read
  fbytes	: ^DINT;	// pointer to store actual # bytes read
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_Read(p1,p2,p3) _LSL_POS^.piLslFile$^LSL_LSLFILE^.Read $ P_Read(p1,p2,p3)

//
//	Function to read a NIL terminated string from the current
//  data file.
//	Object servers can call this function to read any
//	extra string data saved for the object server.
//
FUNCTION GLOBAL __cdecl P_ReadString
VAR_INPUT
  pstr		: pVoid;	// pointer to buffer to store the string
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_ReadString(p1) _LSL_POS^.piLslFile$^LSL_LSLFILE^.ReadString $ P_ReadString(p1)


//
//	Function to write data to the current data file.
//	Object servers can call this function to write any
//	extra data to be saved for the object server.
//
FUNCTION GLOBAL __cdecl P_Write
VAR_INPUT
  pdata		: pVoid;	// pointer to data buffer to write
  dlen		: DINT;		// length of data to write
  fbytes	: ^DINT;	// pointer to store actual # bytes written
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_Write(p1,p2,p3) _LSL_POS^.piLslFile$^LSL_LSLFILE^.Write $ P_Write(p1,p2,p3)


//
//	Function to change the File Mask bits of a server.
//	The function is passed the SERVER pointer (the
//	'this' pointer for the main cmd server OR the SVRCH
//	pointer for the server) and the new file mask bits.
//  Note that these bits will OVERRIDE the previous settings.
//
FUNCTION GLOBAL __cdecl P_SetFileMask
VAR_INPUT
  psvr		: ^SVRCH;	// the server to set
  mask		: UDINT;	// the file mask (a UINT cast to a UDINT)
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_SetFileMask(p1,p2) _LSL_POS^.piLslFile$^LSL_LSLFILE^.SetFileMask $ P_SetFileMask(p1,p2)


//
//	Function to change the active data file path.  New
//	string must have full path including drive, directory,
//	file name and extension.
//
FUNCTION GLOBAL __cdecl P_SetActivePath
VAR_INPUT
  path		: pVoid;	// pointer to NIL terminated path string
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_SetActivePath(p1) _LSL_POS^.piLslFile$^LSL_LSLFILE^.SetActivePath $ P_SetActivePath(p1)

//
//	Function to retreive a pointer to the LSLFILE_DATA
//	structure that is set with data for the current record.
//	This function is valid ONLY suring a file load or save
//	operation.  A server can obtain a pointer to this data
//	structure during a load or save operation if it needs
//	to know the length of any optional extra data or needs
//	to check the class ID revision information.
//
//	The function returns a pointer to the structure (which
//	is valid ONLY during that single single call) or NULL
//	if no load or store operation is active.
//
FUNCTION GLOBAL __cdecl P_GetRecordData
VAR_OUTPUT
  EAX : DINT;	// ptr to LSLFILE_DATA struct or NIL if error
END_VAR;
#define LSLFILE_GetRecordData() _LSL_POS^.piLslFile$^LSL_LSLFILE^.GetRecordData $ P_GetRecordData()

//
// GetActiveInfo writes the structure LSLFILE_ACTIVEINFO to a buffer
//  Buffer      .. points to the structure to store the data in
//  BufferSize  .. Specifies the size of the buffer. 
// Returncode:
//   0 .. no error
//  <0 .. error
//        In the case of a LSLFILE_BUF_TOO_SMALL(-1002) eror, function should be called 
//        again with a larger BufferSize, because its possible that a newer 
//        OS version has additional elements in structure LSLFILE_ACTIVEINFO !
// Note:
//  The first element of LSLFILE_ACTIVEINFO indicates which version of the 
//  structure is returned.
FUNCTION GLOBAL __cdecl P_GetActiveInfo
VAR_INPUT
  Buffer	 : pVoid;	// Reserved parameter - ALWAYS pass as NULL!
  BufferSize : UDINT;
END_VAR
VAR_OUTPUT
  EAX : DINT;	// return 0 if OK, <0 if error
END_VAR;
#define LSLFILE_GetActiveInfo(p1,p2) _LSL_POS^.piLslFile$^LSL_LSLFILE^.GetActiveInfo $ P_GetActiveInfo(p1,p2)

#endif // #ifndef _LSL_ST_LSLFILE_H
