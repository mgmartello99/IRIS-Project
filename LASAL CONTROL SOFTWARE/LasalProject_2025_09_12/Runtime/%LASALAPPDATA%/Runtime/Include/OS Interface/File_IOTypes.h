//**********************************************************************************//
// OpSys-Classes:
//
// Header for Class: *_File*

#pragma once
#define FILEIO_TYPE


// File open flags (do not confuse with file attributes ATTR_... !)
#define ATT_READ_WRITE          0x00000000
#define ATT_READ_ONLY           0x00000100
#define ATT_OPEN_SHARED         0x00000200
#define ATT_OPEN_NO_DIR         0x00000400
#define ATT_OPEN_DIR            0x00000800

#define ATT_CREATE              0x00010000 // create if it does not exist
#define ATT_CREATE_ALWAYS       0x00020000 // always create

#define ATT_COMMITTED           0x01000000 // commit all buffers after every write
#define ATT_CACHE_DATA          0x02000000 // do not discard data buffers
#define ATT_LAZY_DATA           0x04000000 // do not commit buffer when file pointer leaves sector

// File attributes (do not confuse with file open flags ATT_... !)
#define ATTR_READ_ONLY      	0x01
#define ATTR_HIDDEN         	0x02
#define ATTR_SYSTEM         	0x04
#define ATTR_VOLUME         	0x08
#define ATTR_DIR            	0x10
#define ATTR_ARCHIVE        	0x20
#define LONGNAME_ATTR       	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME)

#define RTF_ATTR_ANY        	0x3F

// GetAsyncState return werte
#define RT_INVALID_ID     		-160
#define RT_NOT_STARTED    		-161
#define RT_IN_PROGRESS    		-162
#define RT_ERG_DELETED    		-163

// file pointer movements(for FileSeek):
#define FILE_BEGIN   0
#define FILE_CURRENT 1
#define FILE_END     2

// Option Flags for FileCopy
#define FC_CONTINUE_ON_ERROR     0x01  // Continue copy on error (more than 1 file)
#define FC_NO_OVERWRITE          0x02  // Return an error if file exists
#define FC_SUBDIRECTORIES        0x10  // copy subdirectories

//////////////////////////////////////////////////////////////////
//       * Notes on FileI/O-Class functions  *
//////////////////////////////////////////////////////////////////

//  If a parameter is a pointer to a drive letter ( e.g. GetDiskSpace ),
//  use the format "<drive>:" ( e.g. "C:" )

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
// -150 RT_ERROR_SRAM_FULL
//        Internal Buffer full, try a smaller value
// Error codes for RAM Disk

#define RD_NO_IFACE			-1000
		// RAM Disk Interface not available
#define RD_NO_FNC			-1001
		// Interface function not available
#define RD_IN_USE			-1002
		// RAM Disk in use (reserved)
#define RD_NOT_INSTALLED	-1003
		// RAM Disk not installed

// Error Codes for SplitFileName
#define SNE_ERR_NOERROR                 0x0

#define SNE_ERR_NULLPTR                 -1000
#define SNE_ERR_FILENAMEGT256           -1001
#define SNE_ERR_NOFILENAME              -1002
#define SNE_ERR_FILENAMEFIELD_TOO_SMALL -1003
#define SNE_ERR_EXTFIELD_TOO_SMALL      -1004
#define SNE_ERR_SIZEZERONOTALLOWED      -1005
#define SNE_INFO_EXTNOTSET              -1006
#define SNE_NOT_IMPLEMENTED             -1007

