//****************************************************************************/
//*                                                                          */
//*  File: LSL_ST_OSFILE.H                                                   */
//*  Date: JAN 27, 2003                                                      */
//*  Description:                                                            */
//*	This file contains the LASAL OS FILE Routine Interface               */
//*  Revision History:                                                       */
//*      Rev     By      Date        Description                             */
//*      ---     --      ----        -----------                             */
//*      1.00    MENK    27/01/03    Initial Version                         */
//*                                                                          */
//****************************************************************************/
#ifndef LSL_IFOSFILE_H
#pragma once
#define LSL_IFOSFILE_H

/////////////////////////////////////////////////////////////////////////////////////
//
//
//                          Function prototype for OS_IFILE
//
//
/////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// FILEOPEN
//-----------------------------------------------------------------------------
// Description:
//   This function opens a File on a local drive.
//   Parameter FileName must point to the name of the file to open/create. 
//   File names are not case sensitive; they will be converted to upper case. 
//   Also it must not contain wildcards!
//   The attributes can be a combination of the available attributes given below.
// 
//   If the function succeeds, the return value is a file handle 
//   for the opened file and the file pointer of the file is set to 0. 
//   If the return value is less than 0, 
//   the function has failed and the return value is the error code.
//   All possible error codes are given below.
//____________________________________________________________________________       
//<CDECL>: int SFR_Fopen(char *dpne, unsigned long fs, unsigned long atts0)
FUNCTION __CDECL GLOBAL P_FileOpen
VAR_INPUT
	filename0	: ^CHAR;
	state0          : UDINT;  // 0: RO; 1: CreateAlways; else use userattr0
	userattr0       : UDINT;  // => see Appendix
END_VAR
VAR_OUTPUT
	handle0		: DINT;   // If the function succeeds, the return value is a 
	                          // file handle for the opened file and the file pointer 
				  // of the file is set to 0. If the return value is less 
				  // than 0, the function has failed and the return value 
				  // is the error code. All possible error codes are given 
				  // in RTFiles-32 Error Codes (see Appendix)
END_VAR;



//-----------------------------------------------------------------------------
// FILEREAD
//-----------------------------------------------------------------------------
// Description:
//   This function reads data from a file.
//   Parameter Handle references the open file from which to read.
//   Buffer specifies the address for the data to be read.
//   Length specifies the number of bytes to read.
// 
//   Reading past the end of file is not regarded as an error. 
//   If the function returns a positive value, but its less than the readsize, 
//   then probably the end of the file was encountered.
// 
//   If the function succeeds, the return value is the value of read bytes. 
//   If the function fails, the return value is a negative error code, as given below.
//____________________________________________________________________________       
//<CDECL>: unsigned long SFR_Fread(int hand, void *dest, unsigned long size)
FUNCTION __CDECL GLOBAL P_FileRead
VAR_INPUT
	handle0		: DINT;       // References the open file from which to read.
	buffer0		: pvoid;      // Specifies the address for the data to be read.
	length0		: UDINT;      // Specifies the number of bytes to read.
END_VAR
VAR_OUTPUT
	read0		: UDINT;      // Unsigned integer to receive the number of bytes actually read. 
	                              // Usually, read0 will contain Length after the call. 
	                              // However, in case of an error or if the end of file is encountered 
	                              // during the read, the value may be less. 
	                              // Read0 may be set to NULL if this information is not required by an application.
END_VAR;



//-----------------------------------------------------------------------------
// FILEWRITE
//-----------------------------------------------------------------------------
// Description:
// This function writes data to a file.
// Parameter Handle references the open file to write to.
// Buffer specifies the address of the data to be written.
// Length specifies the number of bytes to write.
// Writing past the current end of file will automatically extend the file. 
// 
// When the file size exceeds the current allocated file size, 
// new clusters are allocated for the file. The FileSystem will allocate new clusters
// immediately following the current last cluster, if possible.
// 
// If the function succeeds, the return value is the count of written bytes. 
// If the function fails, the return value is a negative error code, as given below.
//____________________________________________________________________________       
//<CDECL>: unsigned long SFR_Fwrite(int hand, void *dest, unsigned long size)
FUNCTION __CDECL GLOBAL P_FileWrite
VAR_INPUT
	handle0		: DINT;       // References the open file to write to.
	buffer0		: pvoid;      // Specifies the address of the data to be written.
	length0		: UDINT;      // Specifies the number of bytes to write.
END_VAR
VAR_OUTPUT
	written0	: UDINT;      // Unsigned integer to receive the number of bytes actually written. 
	                              // Usually, written0 will contain Length after the call. 
	                              // However, in case of an error, the returned value may be less. 
	                              // Written0 may be set to 0 if this information is not required by an application.
END_VAR;


//-----------------------------------------------------------------------------
// FILECLOSE
//-----------------------------------------------------------------------------
// Description:
//   This function closes an open file.
//   Parameter Handle must have been assigned in a previous successful call to FileOpen. 
//   The functione will write any unflushed file data to the disk
//   (except for ATT_DEVICE_LAZY_WRITE devices) and release all resources 
//   associated with the file handle.
//____________________________________________________________________________       
//<CDECL>: void SFR_Fclose(int hand)
FUNCTION __CDECL GLOBAL P_FileClose
VAR_INPUT
	handle0		: DINT;       // Must have been assigned in a previous successful call to FileOpen. 
	                              // FileClose will write any unflushed file data to the disk 
	                              // (except for RTF_DEVICE_LAZY_WRITE devices) and release all resources 
	                              // associated with the file handle.
END_VAR;


//-----------------------------------------------------------------------------
// FILEDELETE
//-----------------------------------------------------------------------------
// Description:
//   This function deletes a file.
//   Parameter FileName must point to the name of the file to be deleted 
//   and may NOT contain wildcards.
//   This function CANNOT delete directories.
//   If the function succeeds, the return value is 0. 
//   If the function fails, the return value is a negative error code, as given below.
//____________________________________________________________________________       
//<CDECL>: long SFR_Fdel(char* file)
FUNCTION __CDECL GLOBAL P_FileDelete
VAR_INPUT
	filename0	: ^CHAR;      // Must point to the name of the file to be deleted and may not 
	                              // contain wildcards. This function cannot delete directories
END_VAR
VAR_OUTPUT
	retval0		: DINT;       // If the function succeeds, the return value is RTF_NO_ERROR. 
	                              // If the function fails, the return value is a negative error code.
END_VAR;


//-----------------------------------------------------------------------------
// FILESEEK
//-----------------------------------------------------------------------------
// Description:
//   FileLSeek repositions a file pointer and possibly extends a file.
//   Parameter Handle references the open file for which to reposition the file pointer.
//   Offset specifies how far the file pointer should be moved. 
//   Please note that Offset is a signed integer value.
//   Parameter FromWhere specifies Offset’s meaning. The following values are allowed:
// 
//     FILE_BEGIN(0)   -Offset is an absolute file pointer value.
// 
//     FILE_CURRENT(1) -Offset should be added to the current file pointer value.
// 
//     FILE_END(2)     -Offset should be added to the current file size.
// 
//   Moving the file pointer before the beginning of the file is an error. 
//   However, moving it beyond the current file size is supported. 
//   In this case, the file is extended. The data between the previous file size and the
//   new file size is undefined. This method to extend a file is much faster than actually writing data to it.
//   Please note that this function cannot move the file pointer by more than 231-1.
//   If the function succeeds, the return value is the new file pointer value, or, if the file pointer is larger than
//   231-1, LONG_FILE_POS is returned. 
//   If the function fails, the return value is some other negative error code, as given below.
//____________________________________________________________________________       
//<CDECL>: long SFR_Flseek(int hand, unsigned long offset, unsigned long fromwhere)
FUNCTION __CDECL GLOBAL P_Fileseek
VAR_INPUT
	handle0		: DINT;       // References the open file for which to reposition the file pointer.
	offset0		: UDINT;      // Specifies how far the file pointer should be moved. 
	                              // Please note that Offset is a signed long value
	fromwhere0	: UDINT;      // Specifies Offset's meaning.
END_VAR
VAR_OUTPUT
	retval0		: UDINT;      // If the function succeeds, the return value is the new file pointer value, 
	                              // or, if the file pointer is larger than 2^31-1, RTF_LONG_FILE_POS is returned. 
	                              // If the function fails, the return value is some other negative error code.
END_VAR;



//-----------------------------------------------------------------------------
// FILETELL
//-----------------------------------------------------------------------------
// Description:
//   This function returns the current postiotn of the FilePointer
//   referenced by Handle, or -1 if it fails.
//____________________________________________________________________________       
//<CDECL>: long SFR_Ftell(int hand)
FUNCTION __CDECL GLOBAL P_FileTell
VAR_INPUT
	handle0		: DINT;       // References the open file for which to reposition the file pointer.
END_VAR
VAR_OUTPUT
	retval0		: DINT;
END_VAR;



//-----------------------------------------------------------------------------
// FILELENGTH
//-----------------------------------------------------------------------------
// Description:
//   This function returns the length of the File
//   referenced by Handle, or -1 if it fails.
//____________________________________________________________________________       
//<CDECL>: long SFR_Ffilelength(int hand)
FUNCTION __CDECL GLOBAL P_FileLength
VAR_INPUT
	handle0		: DINT;       // References the open file for which to reposition the file pointer.
END_VAR
VAR_OUTPUT
	retval0		: DINT;       // Receive the file's size.
END_VAR;

FUNCTION __CDECL GLOBAL P_FindFirst
VAR_INPUT
	path		: ^CHAR;
	file		: ^CHAR;
	infostruct		: ^_DDE_INFO;
	atts_inkl		: USINT;
	atts_exkl		: USINT;
END_VAR
VAR_OUTPUT
	handle		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FindNext
VAR_INPUT
	foundhandle		: DINT;
	infostruct		: ^_DDE_INFO;
END_VAR
VAR_OUTPUT
	handle		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FindClose
VAR_INPUT
	handle		: DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_MDir
VAR_INPUT
	dirname		: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_RDir
VAR_INPUT
	dirname		: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_Rename
VAR_INPUT
	actname		: ^CHAR;
	newname		: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_CloseAll;


FUNCTION __CDECL GLOBAL P_GetDriveListShort
VAR_INPUT
	pErg			: PVOID;
END_VAR
VAR_OUTPUT
	lErg			: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_Extend
VAR_INPUT
	handle : DINT;
	size : UDINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_SetDefaultOpenFlags
VAR_INPUT
	flags : DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;



FUNCTION __CDECL GLOBAL P_FileOpen_A
VAR_INPUT
  Async : DINT;
	filename0	: ^CHAR;
	userattr0       : UDINT;
END_VAR
VAR_OUTPUT
	handle0		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FileRead_A
VAR_INPUT
  Async : DINT;
	handle0		: DINT;
	buffer0		: pvoid;
	length0		: UDINT;
END_VAR
VAR_OUTPUT
	read0		: UDINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FileWrite_A
VAR_INPUT
  Async : DINT;
	handle0		: DINT;
	buffer0		: pvoid;
	length0		: UDINT;
END_VAR
VAR_OUTPUT
	written0	: UDINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FileClose_A
VAR_INPUT
  Async : DINT;
	handle0		: DINT;
END_VAR
VAR_OUTPUT
	retval0		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FileDelete_A
VAR_INPUT
  Async : DINT;
	filename0	: ^CHAR;
END_VAR
VAR_OUTPUT
	retval0		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_Fileseek_A
VAR_INPUT
  Async : DINT;
	handle0		: DINT;
	offset0		: UDINT;
	fromwhere0	: UDINT;
END_VAR
VAR_OUTPUT
	retval0		: UDINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FileTell_A
VAR_INPUT
  Async : DINT;
	handle0		: DINT;
END_VAR
VAR_OUTPUT
	retval0		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FileLength_A
VAR_INPUT
  Async : DINT;
	handle0		: DINT;
END_VAR
VAR_OUTPUT
	retval0		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FindFirst_A
VAR_INPUT
  Async : DINT;
	path		: ^CHAR;
	file		: ^CHAR;
	infostruct		: ^_DDE_INFO;
	atts_inkl		: USINT;
	atts_exkl		: USINT;
END_VAR
VAR_OUTPUT
	handle		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FindNext_A
VAR_INPUT
  Async : DINT;
	foundhandle		: DINT;
	infostruct		: ^_DDE_INFO;
END_VAR
VAR_OUTPUT
	handle		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FindClose_A
VAR_INPUT
  Async : DINT;
	handle		: DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_MDir_A
VAR_INPUT
  Async : DINT;
	dirname		: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_RDir_A
VAR_INPUT
  Async : DINT;
	dirname		: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_RDir_F
VAR_INPUT
	dirname		: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_RDir_F_A
VAR_INPUT
  Async : DINT;
	dirname		: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_Rename_A
VAR_INPUT
  Async : DINT;
	actname		: ^CHAR;
	newname		: ^CHAR;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_CloseAll_A
VAR_INPUT
  Async : DINT;
END_VAR
VAR_OUTPUT
	retval0		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL P_GetAsyncState
VAR_INPUT
  ID : UDINT;
	Erg	: ^DINT;
END_VAR
VAR_OUTPUT
	retval		: DINT;
END_VAR;



FUNCTION __CDECL GLOBAL P_FileTruncate
VAR_INPUT
	handle0		: DINT;
END_VAR
VAR_OUTPUT
	retval0		: DINT;
END_VAR;
FUNCTION __CDECL GLOBAL P_FileTruncate_A
VAR_INPUT
  Async : DINT;
	handle0		: DINT;
END_VAR
VAR_OUTPUT
	retval0		: DINT;
END_VAR;


FUNCTION __CDECL GLOBAL P_FindFirstEx
VAR_INPUT
  namepattern0 :^CHAR;
  attr0        : USINT;
  attrmask0    : USINT;
  fileinfo0    :^_DDE_INFO;
  filename     :^CHAR;
  maxlength0   : UDINT;
END_VAR
VAR_OUTPUT
  retval0      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FindNextEx
VAR_INPUT
  fhandle0    : DINT;
  fileinfo0   :^_DDE_INFO;
  filename0   :^CHAR;
  maxlength0  : UDINT;
END_VAR
VAR_OUTPUT
  retval0     : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FindFirstEx_A
VAR_INPUT
  Async0       : DINT;
  namepattern0 :^CHAR;
  attr0        : USINT;
  attrmask0    : USINT;
  fileinfo0    :^_DDE_INFO;
  filename     :^CHAR;
  maxlength0   : UDINT;
END_VAR
VAR_OUTPUT
  retval0      : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FindNextEx_A
VAR_INPUT
  Async0      : DINT;
  fhandle0    : DINT;
  fileinfo0   :^_DDE_INFO;
  filename0   :^CHAR;
  maxlength0  : UDINT;
END_VAR
VAR_OUTPUT
  retval0     : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_GetAttributes
VAR_INPUT
  filename    :^CHAR;
  Attributes  :^USINT;
END_VAR
VAR_OUTPUT
  retval0     : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_SetAttributes
VAR_INPUT
  filename    :^CHAR;
  Attributes  : USINT;
END_VAR
VAR_OUTPUT
  retval0     : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_GetDiskSpace
VAR_INPUT
  drive             :^CHAR;
  BytesPerSector    : UDINT;
  SectorsPerCluster : UDINT;
  TotalClusters     : UDINT;
  FreeClusters      : UDINT;
END_VAR
VAR_OUTPUT
  retval0           : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FileReadV1
VAR_INPUT
  fhandle0  : DINT;
  buffer0   : pvoid;
  length0   : UDINT;
END_VAR
VAR_OUTPUT
  retval    : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FileRead_AV1
VAR_INPUT
  Async0    : DINT;
  fhandle0  : DINT;
  buffer0   : pvoid;
  length0   : UDINT;
END_VAR
VAR_OUTPUT
  retval    : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FileWriteV1
VAR_INPUT
  fhandle0  : DINT;
  buffer0   : pvoid;
  length0   : UDINT;
END_VAR
VAR_OUTPUT
  retval    : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FileWrite_AV1
VAR_INPUT
  Async0    : DINT;
  fhandle0  : DINT;
  buffer0   : pvoid;
  length0   : UDINT;
END_VAR
VAR_OUTPUT
  retval    : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FileseekV1
VAR_INPUT
	handle0		  : DINT;
	offset0		  : DINT;
	fromwhere0	: UDINT;
END_VAR
VAR_OUTPUT
	retval0		  : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_Fileseek_AV1
VAR_INPUT
  Async0      : DINT;	
  handle0		  : DINT;
	offset0		  : DINT;
	fromwhere0	: UDINT;
END_VAR
VAR_OUTPUT
	retval0		  : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_FileOpenV1
VAR_INPUT
  filename0   :^CHAR;
  attributes0 : UDINT;
END_VAR
VAR_OUTPUT
  retval0     : DINT;
END_VAR;


/////////////////////////////////////////////////////////////////////////////////////
//
//
//                          MACRO defintions for OS_IFILE
//
//
/////////////////////////////////////////////////////////////////////////////////////

#define OS_FILE_OPEN(p1,p2,p3)                _LSL_POS^.piFILE^.SFR_Fopen  $ P_FileOpen(p1,p2,p3)
#define OS_FILE_READ(p1,p2,p3)                _LSL_POS^.piFILE^.SFR_Fread  $ P_FileRead(p1,p2,p3)
#define OS_FILE_WRITE(p1,p2,p3)               _LSL_POS^.piFILE^.SFR_Fwrite $ P_FileWrite(p1,p2,p3)
#define OS_FILE_CLOSE(p1)                     _LSL_POS^.piFILE^.SFR_Fclose $ P_FileClose(p1)
#define OS_FILE_DELETE(p1)                    _LSL_POS^.piFILE^.SFR_Fdel   $ P_FileDelete(p1)
#define OS_FILE_SEEK(p1,p2,p3)                _LSL_POS^.piFILE^.SFR_Flseek $ P_FileSeek(p1,p2,p3)
#define OS_FILE_TELL(p1)                      _LSL_POS^.piFILE^.SFR_Ftell  $ P_FileTell(p1)
#define OS_FILE_LENGTH(p1)                    _LSL_POS^.piFILE^.SFR_Ffilelength $ P_FileLength(p1)
#define OS_FILE_FINDFIRST(p1,p2,p3,p4,p5)     _LSL_POS^.piFILE^.SFR_FindFirst $ P_FindFirst(p1,p2,p3,p4,p5)
#define OS_FILE_FINDNEXT(p1,p2)               _LSL_POS^.piFILE^.SFR_FindNext $ P_FindNext(p1,p2)
#define OS_FILE_FINDCLOSE(p1)                 _LSL_POS^.piFILE^.SFR_FindClose $ P_FindClose(p1)
#define OS_FILE_CREATEDIR(p1)                 _LSL_POS^.piFILE^.SFR_CreateDir $ P_MDir(p1)
#define OS_FILE_REMOVEDIR(p1)                 _LSL_POS^.piFILE^.SFR_RemoveDir $ P_RDir(p1)
#define OS_FILE_RENAME(p1,p2)                 _LSL_POS^.piFILE^.SFR_Rename $ P_Rename(p1,p2)
#define OS_FILE_CLOSEALL()                    _LSL_POS^.piFILE^.SFR_CloseAllOpenFiles $ P_CloseAll()

#define OS_FILE_GETDRIVELISTSHORT(p1)         _LSL_POS^.piFILE^.SFR_GetDriveListShort $ P_GetDriveListShort(p1)
#define OS_FILE_EXTEND(p1,p2)                 _LSL_POS^.piFILE^.SFR_Ffilelength $ P_Extend(p1,p2)
#define OS_FILE_SETDEFAULTOPENFLAGS(p1)       _LSL_POS^.piFILE^.SFR_SetDefaultOpenFlags $ P_SetDefaultOpenFlags(p1)

#define OS_FILE_OPEN_A(p1,p2,p3)                 _LSL_POS^.piFILE^.SFR_Fopen_A  $ P_FileOpen_A(p1,p2,p3)
#define OS_FILE_READ_A(p1,p2,p3,p4)              _LSL_POS^.piFILE^.SFR_Fread_A  $ P_FileRead_A(p1,p2,p3,p4)
#define OS_FILE_WRITE_A(p1,p2,p3,p4)             _LSL_POS^.piFILE^.SFR_Fwrite_A $ P_FileWrite_A(p1,p2,p3,p4)
#define OS_FILE_CLOSE_A(p1,p2)                   _LSL_POS^.piFILE^.SFR_Fclose_A $ P_FileClose_A(p1,p2)
#define OS_FILE_DELETE_A(p1,p2)                  _LSL_POS^.piFILE^.SFR_Fdel_A   $ P_FileDelete_A(p1,p2)
#define OS_FILE_SEEK_A(p1,p2,p3,p4)              _LSL_POS^.piFILE^.SFR_Flseek_A $ P_FileSeek_A(p1,p2,p3,p4)
#define OS_FILE_TELL_A(p1,p2)                    _LSL_POS^.piFILE^.SFR_Ftell_A  $ P_FileTell_A(p1,p2)
#define OS_FILE_LENGTH_A(p1,p2)                  _LSL_POS^.piFILE^.SFR_Ffilelength_A $ P_FileLength_A(p1,p2)
#define OS_FILE_FINDFIRST_A(p1,p2,p3,p4,p5,p6)   _LSL_POS^.piFILE^.SFR_FindFirst_A $ P_FindFirst_A(p1,p2,p3,p4,p5,p6)
#define OS_FILE_FINDNEXT_A(p1,p2,p3)             _LSL_POS^.piFILE^.SFR_FindNext_A $ P_FindNext_A(p1,p2,p3)
#define OS_FILE_FINDCLOSE_A(p1,p2)               _LSL_POS^.piFILE^.SFR_FindClose_A $ P_FindClose_A(p1,p2)
#define OS_FILE_CREATEDIR_A(p1,p2)               _LSL_POS^.piFILE^.SFR_CreateDir_A $ P_MDir_A(p1,p2)
#define OS_FILE_REMOVEDIR_A(p1,p2)               _LSL_POS^.piFILE^.SFR_RemoveDir_A $ P_RDir_A(p1,p2)
#define OS_FILE_RENAME_A(p1,p2,p3)               _LSL_POS^.piFILE^.SFR_Rename_A $ P_Rename_A(p1,p2,p3)
#define OS_FILE_CLOSEALL_A(p1)                   _LSL_POS^.piFILE^.SFR_CloseAllOpenFiles_A $ P_CloseAll_A(p1)

#define OS_FILE_GETASYNCSTATE(p1,p2)             _LSL_POS^.piFILE^.SFR_GetState $ P_GetAsyncState(p1,p2)

#define OS_FILE_TRUNCATE(p1)                     _LSL_POS^.piFILE^.SFR_Ftruncate  $ P_FileTruncate(p1)
#define OS_FILE_TRUNCATE_A(p1,p2)                _LSL_POS^.piFILE^.SFR_Ftruncate_A $ P_FileTruncate_A(p1,p2)

#define OS_FILE_FINDFIRSTEX(p1,p2,p3,p4,p5,p6)      _LSL_POS^.piFILE^.SFR_FindFirstEx $ P_FindFirstEx(p1,p2,p3,p4,p5,p6)
#define OS_FILE_FINDNEXTEX(p1,p2,p3,p4)             _LSL_POS^.piFILE^.SFR_FindNextEx $ P_FindNextEx(p1,p2,p3,p4)
#define OS_FILE_FINDFIRSTEX_A(p1,p2,p3,p4,p5,p6,p7) _LSL_POS^.piFILE^.SFR_FindFirstEx_A $ P_FindFirstEx_A(p1,p2,p3,p4,p5,p6,p7)
#define OS_FILE_FINDNEXTEX_A(p1,p2,p3,p4,p5)        _LSL_POS^.piFILE^.SFR_FindNextEx_A $ P_FindNextEx_A(p1,p2,p3,p4,p5)

#define OS_FILE_GETATTRIBUTE(p1,p2)           _LSL_POS^.piFILE^.SFR_GetAttributes $ P_GetAttributes(p1,p2)
#define OS_FILE_SETATTRIBUTE(p1,p2)           _LSL_POS^.piFILE^.SFR_SetAttributes $ P_SetAttributes(p1,p2)
#define OS_FILE_GETDISKSPACE(p1,p2,p3,p4,p5)  _LSL_POS^.piFILE^.SFR_GetDiskSpace $ P_GetDiskSpace(p1,p2,p3,p4,p5)

#define OS_FILE_READV1(p1,p2,p3)              _LSL_POS^.piFILE^.SFR_FreadV1 $ P_FileReadV1(p1,p2,p3)
#define OS_FILE_READ_AV1(p1,p2,p3,p4)         _LSL_POS^.piFILE^.SFR_Fread_AV1 $ P_FileRead_AV1(p1,p2,p3,p4)
#define OS_FILE_WRITEV1(p1,p2,p3)             _LSL_POS^.piFILE^.SFR_Fwrite_X $ P_FileWriteV1(p1,p2,p3)
#define OS_FILE_WRITE_AV1(p1,p2,p3,p4)        _LSL_POS^.piFILE^.SFR_Fwrite_AV1 $ P_FileWrite_AV1(p1,p2,p3,p4)
#define OS_FILE_SEEKV1(p1,p2,p3)              _LSL_POS^.piFILE^.SFR_Flseek $ P_FileseekV1(p1,p2,p3)
#define OS_FILE_SEEK_AV1(p1,p2,p3,p4)         _LSL_POS^.piFILE^.SFR_Flseek_A $ P_Fileseek_AV1(p1,p2,p3,p4)

#define OS_FILE_OPENV1(p1,p2)                 _LSL_POS^.piFILE^.SFR_FopenX $ P_FileOpenV1(p1,p2)

#define OS_FILE_REMOVEDIR_FULL(p1)                 _LSL_POS^.piFILE^.SFR_RemoveDir_FULL $ P_RDir_F(p1)
#define OS_FILE_REMOVEDIR_FULL_A(p1,p2)               _LSL_POS^.piFILE^.SFR_RemoveDir_FULL_A $ P_RDir_F_A(p1,p2)

/////////////////////////////////////////////////////////////////////////////////////
//
//
//                          CONSTANT and #DEFINE for OS_IFILE
//
//
/////////////////////////////////////////////////////////////////////////////////////
#include ".\OS Interface\File_IOTypes.h"

//////////////////////////////////////////////////////////////////
//       * LasalOS File Error Return Codes *
//////////////////////////////////////////////////////////////////

//  0 RTF_NO_ERROR
//        Not an error. This value indicates success of an operation.
// -1 RTF_ERROR_RESERVED
//        This error code is reserved and will not be returned by the FileI/O-Class.
// -2 RTF_PARAM_ERROR
//        The parameters passed to an FileI/O-Class function are invalid. For example, the flags passed to
//        RTFOpen are contradictory or the size of a string buffer is too small.
// -3 RTF_INVALID_FILENAME
//        A device, directory, or file name passed to the FileI/O-Class has an invalid syntax, contains illegal
//        characters, or exceeds RTF_MAX_PATH (80) characters.
// -4 RTF_DRIVE_NOT_FOUND
//        The program attempted to access a logical drive which is not mounted.
// -5 RTF_TOO_MANY_FILES
//        The program attempted to open more files than slots were available in the file table. Changing
// -6 RTF_NO_MORE_FILES
//        This value is returned by RTFFindFirst and RTFFindNext when no more files satisfy the search
//        criteria.
// -7 RTF_WRONG_MEDIA
//        A diskette has been replaced in a diskette drive, and the serial number of the new disk does not
//        match the serial number of the original disk. To correct this error, the original diskette must be
//        inserted or the operation must be failed.
// -8 RTF_INVALID_FILE_SYSTEM
//        The information found in the boot record of a logical drive is inconsistent and probably corrupted.
//        The drive cannot be mounted.
// -9 RTF_FILE_NOT_FOUND
//        The requested file name was not found on the disk.
// -10 RTF_INVALID_FILE_HANDLE
//        A file handle passed to an the FileI/O-Class API function is invalid. Either it has been closed already or
//        it was not returned by a previous successful call to RTFOpen or RTFFindFirst, or it was closed
//        automatically due to the removal of the media hosting the file.
// -11 RTF_UNSUPPORTED_DEVICE
//        A device in the device list specified a device other than RTF_DEVICE_DISKETTE or RTF_DEVICE_
//        FDISK in the DeviceType field.
// -12 RTF_UNSUPPORTED_DRIVER_FUNCTION
//        This error is returned by device drivers or system drivers. For example, a device driver for readonly
//        devices would return this error on attempts to write to the device.
// -13 RTF_CORRUPTED_PARTITION_TABLE
//        the FileI/O-Class has found inconsistent values in a device’s partition table. The device cannot be
//        mounted.
// -14 RTF_TOO_MANY_DRIVES
//        The number of logical drives found on all devices given in the device list exceeds the FileI/O-Class’s
//        internal drive table. 
// -15 RTF_INVALID_FILE_POS
//        A call to RTFSeek attempted to position the file pointer before the start of the file.
// -16 RTF_ACCESS_DENIED
//    This error is returned whenever a security check fails. A few such checks are:
//        * Attempt to open a file for read/write access, but the read-only attribute is set.
//        * Attempt to open an already open file, and at least one of the file instances requires write
//        access, and RTF_SHARED is not specified for all instances of the file.
//        * Attempt to create a file and the specified file already exists with the read-only attribute set.
//        * Attempt to open a volume label.
//        * Attempt to open a directory with read/write access.
//        * Attempt to open a directory with flag RTF_OPEN_NO_DIR.
//        * Attempt to delete a file with attribute read-only, volume label, or directory set.
//        * Attempt to rename a file across drives.
//        * Attempt to rename a volume label.
//        * Attempt to rename a directory to one of its child directories.
//        * Attempt to rename the current directory.
//        * Attempt to set attributes, date and time, or file size of a root directory.
//        * Attempt to change a volume label or directory attributes.
//        * Attempt to write to, truncate, or extend a file open for read-only access.
//        * Attempt to remove a directory which is not empty.
//        * Attempt to remove a directory with the read-only attribute set.
//        * Attempt to remove the root or the current directory.
//        * RTFResetDisk was called while files are open on the target drive.
// -17 RTF_STRING_BUFFER_TOO_SMALL
//        The size of a string buffer passed to an the FileI/O-Class function is too small to hold the result.
// -18 RTF_GENERAL_FAILURE
//        A device driver reported an error without supplying additional information about the cause. For
//        example, non-existing hardware or fatal hardware failures could generate this error.
// -19 RTF_PATH_NOT_FOUND
//        The path for a file or a directory passed to the FileI/O-Class was not found.
// -20 RTF_FAT_ALLOC_ERROR
//        the FileI/O-Class has found invalid values in a partition’s FAT. The FAT is most likely corrupted and
//        the partition must be reformatted.
// -21 RTF_ROOT_DIR_FULL
//        It was attempted to create a new file in a root directory, but the directory is full. Unlike subdirectories,
//        root directories have a fixed size and cannot be extended.
// -22 RTF_DISK_FULL
//        It was attempted to extend a file or directory, but not enough free clusters to satisfy the request
//        were found. For function RTFExtend, this error is returned when not enough contiguous clusters
//        are found.
// -23 RTF_TIMEOUT
//        This device error is reported when a device fails to respond within a reasonable period of time.
// -24 RTF_BAD_SECTOR
//        A device driver has reported that a sector on the disk could not be read or written.
// -25 RTF_DATA_ERROR
//        A device driver has reported that a sector read from disk has failed a data integrity check.
//        Typically, data is stored with CRC check sums which are used for such checks.
// -26 RTF_MEDIA_CHANGED
//        During a device access, the driver has detected that the media in the drive has been removed or
//        exchanged.
// -27 RTF_SECTOR_NOT_FOUND
//        A device driver was not able to locate a requested sector. Either a corrupted boot sector or
//        corrupted low-level formatting can cause this error.
// -28 RTF_ADDRESS_MARK_NOT_FOUND
//        The address mark normally written during a low-level format was not found during a disk read or
//        write operation.
// -29 RTF_DRIVE_NOT_READY
//        A disk device does not respond, either because it is not present, the media is not inserted, or
//        because of a hardware failure.
// -30 RTF_WRITE_PROTECTION
//        It was attempted to write to a write-protected media.
// -31 RTF_DMA_OVERRUN
//        A DMA controller has reported this error. This can happen when a DMA buffer spans a
//        64k address boundary.
// -32 RTF_CRC_ERROR
//        A CRC check failed during a device read or write operation.
// -33 RTF_DEVICE_RESOURCE_ERROR
//        A device driver has reported that some resource it requires is not available. For example, the
//        floppy driver was unable to allocate a DMA buffer or the RAM disk driver was unable to allocate
//        space for new sectors.
// -34 RTF_INVALID_SECTOR_SIZE
//        A device reported itself to be formatted with a non-standard sector size. Usually, FAT volumes
//        should use sectors of 512 bytes size. Please contact On Time for information about how
//        the FileI/O-Class can support other sector sizes.
// -35 RTF_OUT_OF_BUFFERS
//        The FileI/O-Class was unable to allocate a new buffer in its internal buffer cache. This situation can
//        only occur if all buffers are dirty and none of the dirty buffers reside on the same physical device
//        for which a new buffer is required. To avoid this error, increase the number of buffers, close
//        some files, or flush buffers before the failing function is called.
// -36 RTF_FILE_EXISTS
//        This error is reported on an attempt to rename a file to an existing file name or create a directory
//        with the name of an existing directory or file.
// -37 RTF_LONG_FILE_POS
//        This return code does not constitute an error. It is returned by RTFSeek when the new file
//        pointer value exceeds 231-1 (2G minus one). However, the function has succeeded when this
//        value is returned. Use function RTFGetFileInfo to retrieve the actual file pointer.
// -38 RTF_FILE_TOO_LARGE
//        The application has attempted to extend a file to contain 4G or more bytes. However, FAT file
//        systems only support file sizes up to FFFFFFFFh bytes. This restriction also applies to FAT-32
//        partitions.


// Error codes for FileCopy


// Error codes for RAM Disk

//#define RD_NO_IFACE			-1000
		// RAM Disk Interface not available
//#define RD_NO_FNC			-1001
		// Interface function not available
//#define RD_IN_USE			-1002
		// RAM Disk in use (reserved)
//#define RD_NOT_INSTALLED	-1003
		// RAM Disk not installed



#endif // LSL_IFOSFILE_H
