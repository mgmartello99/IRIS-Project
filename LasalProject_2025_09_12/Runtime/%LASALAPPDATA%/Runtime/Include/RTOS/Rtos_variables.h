#pragma once
#define __RTOS__VARIABLES__


#include "RTOS_Header.h"
#include "RTOS_Interfaces.h"



// S-RAM Descriptor
#define RAM_MAX_CELL     1000     // maximum # of ramcells wegen alt eingefroren !
#define X_RAM_CELLS		  500	  // directory size for X_RAM_CELLS	
#define X_FILE_CELLS	49152	  // belet genau die 192 kByte S-RAM	
// DATE TIME DATA TYPE FOR SYSDATETIMECLASS ENABLE
#define USELSLDATETIME 1
#pragma pack (push,1)
TYPE
   MRAM_DESCR :STRUCT
     Version   :UDINT;
     DataStart :^XRamHdr;
     DataLength: UDINT;
     UsedData  : UDINT;
     DataValid : UDINT;
   END_STRUCT;


   RamCell 	: STRUCT
     udChk 		: UDINT;    // = udCRC32+udData
     udCRC32 	: UDINT;
     udData 	: UDINT;
   END_STRUCT;

   XRamCell 	: STRUCT    // for all entries from Nr 1000 on
     udChk 		: UDINT;    // = udCRC32+udData
     udCRC32 	: UDINT;
     udData 	: UDINT;    // Xram it is Length of Datablock 
	 udType		: UDINT;	// bit 31 = RAM 4Btye  1=Extended ram (more than 4 bytes long) 0-31= offset of data from RAMHDR
   END_STRUCT;

   pXRamCell_t : ^XRamCell;

   XRAMHDR     : STRUCT
     udEntries  : UDINT;
     udChk      : UDINT;     // = NOT udEntries  
     aData      : ARRAY [0..X_RAM_CELLS] OF XRamCell;
   END_STRUCT;


   XFileCell 	: STRUCT    
     udCRC32 	: UDINT;	// CRC of Name	
	 udType		: UDINT;	// bit 31 = RAM 4Btye  1=Extended ram (more than 4 bytes long) 0-31= offset of data from RAMHDR
	 udpRelSram : UDINT;	// relativ offset in S-Ram
   END_STRUCT;

   pXFileCell_t : ^XFileCell;

   XFILEHDR     : STRUCT
     udEntries  : UDINT;	// anzahl
	 uiChange	: UINT;		// change flag	
	 CRC32		: UDINT;    // CRC über gesamtes FILE
	 pLastEntry	: UDINT;	// Zaehler auf letzten gefundenen eintrag
     aData      : ARRAY [0..(X_FILE_CELLS-1)] OF XFileCell;
   END_STRUCT;
  	 	
   	

   RAMHDR     : STRUCT
     udEntries  : UDINT;
     udChk      : UDINT;     // = NOT udEntries  
     aData      : ARRAY [0..RAM_MAX_CELL] OF RAMCELL;
   END_STRUCT;

	// header for blocks of retentive memory
   _RetHdr		: STRUCT
	 pNxt			: ^_RetHdr;	// can be NIL
     dUnused			: DINT;		// to keep header size const.
	 udCRC			: UDINT;	// i.e. object name
	 udSize			: UDINT;	// including header
	 udFlags			: BDINT[
	 bUsed
			];
		udChk			: UDINT;	// of header without udChk
	END_STRUCT;

	// header for collected blocks of read-only descriptors
	_DscHdr				: STRUCT
		udSize			: UDINT;
		pNxt			: ^_DscHdr;
	END_STRUCT;

	pRetHdr				: ^_RetHdr;

  sRoutingEntry : STRUCT
    usKnown : usint;
    pConnectString : ^usint;
    usQuality : usint;
  end_struct;

  sRoutingTable : STRUCT
    usID  : USINT;
    sCPU  : Array[ 0..255 ] of sRoutingEntry;
  end_struct;
  
  DATE_LSL  : HDINT;
  TIME_LSL	: HDINT;
  
END_TYPE
#pragma pack (pop)




VAR_EXTERNAL

_RealMaximumTime	(*AT % M 0000*)		: UDINT;	// maximum duration of RT-methods
_RealAverageTime	(*AT % M 0004*)		: UDINT;	// average duration of RT-methods
_CyclicMaximumTime	(*AT % M 0008*)		: UDINT;	// maximum duration of Cy-methods
_CyclicAverageTime	(*AT % M 000C*)		: UDINT;	// average duration of Cy-methods
_BackgroundMaximumTime	(*AT % M 0010*)		: UDINT;	// maximum duration of Bg-methods
_BackgroundAverageTime	(*AT % M 0014*)		: UDINT;	// average duration of Bg-methods

_RealMaximumTimes	(*AT % M 0390*)		: ARRAY [0..15] OF UDINT;	// multicore maximum duration of RT-methods
_RealAverageTimes	(*AT % M 03D0*)		: ARRAY [0..15] OF UDINT;	// multicore average duration of RT-methods
_CyclicMaximumTimes	(*AT % M 0410*)		: ARRAY [0..15] OF UDINT;	// multicore maximum duration of Cy-methods
_CyclicAverageTimes	(*AT % M 0450*)		: ARRAY [0..15] OF UDINT;	// multicore average duration of Cy-methods

_UserHeapStartAddr      (*AT % M 0020*)		: UDINT;	// start address of UserHeapMemory
_UserHeapTotalSize      (*AT % M 0024*)		: UDINT;	// total size of UserHeapMemory
_UserHeapUsedMem        (*AT % M 0028*)		: UDINT;	// amount of used memory from UserHeap
_UserHeapFreeMem        (*AT % M 002C*)		: UDINT;	// amount of free memory from UserHeap

_diskOperatingSeconds   (*AT % M 0108*)     : UDINT; // lifetime of disk in drive c: in seconds
_diskSectWrCnt          (*AT % M 010C*)     : UDINT; // total number of disc sector writes 
_diskSectWrCnt1000Sec   (*AT % M 0110*)     : UDINT; // number of disc sector writes in the last 1000 seconds
_diskSectWrCnt10Sec     (*AT % M 0114*)     : UDINT; // number of disc sector writes in the last 10 seconds 
_diskSectWrCntSramdisk  (*AT % M 0118*)     : UDINT; // number of disc sector writes due to sramdisk backup

_sramDiagFlags  (*AT % M 011C*)     : UDINT; // flags that indicate errors in os (Bit 0:sram not written at powerdown, Bit 1: no acces to sd card)

_onlineDiagFlags  (*AT % M 0120*)   : UDINT; // flags that indicate status of online server (Bit 0:status lasal online communication port (def. 1954), Bit 1:comlink refreshlist connection closed)

_MaxDataMem             (*AT % M 003C*)         : UDINT;        // maximum amount of the projects data memory
_MaxCodeMem             (*AT % M 0040*)         : UDINT;        // maximum amount of the projects code memory

_RtOSPatchVersion       (*AT % M 02E4*)         : CHAR;         // patch version of lasal os ('A', 'B', 'C', usw.)

_WhoAmI			(*AT % M 02E8*)		: UDINT;        // What CPU type 
// => see Defines DESTPLC_xxxx
//
//#define DESTPLC_IPC_TEXT    0x0000001
//#define DESTPLC_IPC_800     0x0000002
//#define DESTPLC_IPC_1024    0x0000003
//#define DESTPLC_PCD         0x0000004
//#define DESTPLC_DSE         0x0000005
//#define DESTPLC_DCP         0x0000006
//#define DESTPLC_DCC080      0x0000007
//#define DESTPLC_DCP645      0x0000008
//#define DESTPLC_AK500       0x0000009
//#define DESTPLC_TSTROM      0x000000A
//#define DESTPLC_ELAN        0x0000010
//#define DESTPLC_WINPC       0x0000012
//#define DESTPLC_TERMINAL    0x0000013
//#define DESTPLC_DTC281      0x0000014
//#define DESTPLC_ETT321      0x0000017
//#define DESTPLC_HERZ        0x0000018
//#define DESTPLC_ETT261      0x0000019
//#define DESTPLC_ETT431      0x000001A
//#define DESTPLC_WINPC_98    0x000001B // LARS, Standard PC, Win98/ME
//#define DESTPLC_WINPC_NT    0x000001C // LARS, Standard PC, WinNT/Win2000/WinXP
//#define DESTPLC_WINIPC_98   0x000001D // LARS, IPC, Win98/ME
//#define DESTPLC_WINIPC_NT   0x000001E // LARS, IPC, WinNT/Win2000/WinXP
//#define DESTPLC_IPC_320     0x000001F
//#define DESTPLC_C_IPC       0x0000020 // C-IPC
//#define DESTPLC_RK500       0x0000022
//#define DESTPLC_CCL721      0x0000023
//#define DESTPLC_VEMAGTE     0x0000024
//#define DESTPLC_BDF2000     0x0000025
//#define DESTPLC_STANDARDPC  0x0000026
//#define DESTPLC_CCL081CET   0x0000027


_RtOSversion		(*AT % M 02EC*)		: UDINT;	// < 3.47 ... 1, else 347 aso.

_UserProgPointer	(*AT % M 02F0*)		: ^USINT;	// address of executable program
_UserProgSize   	(*AT % M 02F4*)		:  UDINT;	// size of executable program
_UserDataPointer	(*AT % M 02F8*)		: ^USINT;	// address of memory ( equals % M 0000)
_UserDataSize		(*AT % M 02FC*)		:  UDINT;	// size of memory
_S_RAM_Hptr			(*AT  %M 0300*)     : ^MRAM_DESCR ;   // -> Pointer auf S_RAM_Header  

_rtruntime			(*AT % M 0304*)     : USINT;
_swrtruntime		(*AT % M 0305*)     : USINT;
_RTrtIntVal			(*AT % M 0306*)     : UDINT;

_btruntime			(*AT % M 030A*)		: UDINT;
_swbtruntime		(*AT % M 030E*)		: UDINT;

_heapAllocCnt		(*AT % M 0312*)		: UDINT;  	// UserHeap allocate -> inc;
_rebootOnError		(*AT % M 0316*)		: USINT;  	// 0=kein Reboot on error, >0=Delay in Sek., bis nach einem Fehler ein Reboot ausgeführt wird
_isRebootCodeAvailable (*AT % M 0317*)  : USINT;    // 1=die Sysvar rebootCode ist gültig
_rebootCode         (*AT % M 0318*)		: USINT;    // CPU Status Code vor dem 'Reboot on error'
_SysTaskMeasEnable  (*AT % M 0319*)		: USINT;

_noUserOffscreen	(*AT % M 0330*)		: UDINT;  	// number of user created offscreens
_noUserGetImage  	(*AT % M 0334*)		: UDINT;  	// number of user created images
_noUserRegion   	(*AT % M 0338*)		: UDINT;  	// number of user created regions

_RTRuntimes    (*AT % M 0350*) : ARRAY [0..15] OF USINT; // Multicore Realtime Runtime value (index = core-number; value for core 0 not used - see sysvar.RTRuntime)
_swRTRuntimes  (*AT % M 0360*) : ARRAY [0..15] OF USINT; // Multicore Realtime Runtime Resetvalue (index = core-number; value for core 0 not used - see sysvar.swRTRuntime)
_runtimes      (*AT % M 0370*) : ARRAY [0..15] OF USINT; // Multicore Cyclic Runtime value (index = core-number; value for core 0 not used - see sysvar.runtime)
_swRuntimes    (*AT % M 0380*) : ARRAY [0..15] OF USINT; // Multicore Cyclic Runtime Resetvalue (index = core-number; value for core 0 not used - see sysvar.swRuntime)

_cpuLoad			(*AT % M 0EA6*)		: UINT;	// cpu load in promille
_runstatus			(*AT % M 0EAA*)		: USINT;	// program runs form ROM/RAM
_runtime			(*AT % M 0EAE*)		: USINT;	// runtime counter
_swruntime			(*AT % M 0EAF*)		: USINT;	// setpoint for runtime counter
_error_cnt_dias		(*AT % M 0E94*)		: USINT;	// counter for bus errors
_error_station_dias	(*AT % M 0E95*)		: USINT;	

_OnlineMap		(*AT % M 0E96*) : STRUCT	//counts how many users are online with the following interfaces
		bySerial						: USINT; //serial online counter
		byTCP							: USINT; //TCP online counter
		byCAN							: USINT; //CAN online counter
		byReserved						: USINT;
	END_STRUCT;

_ClockTicks			(*AT % M 0EAB*)		: UINT;		// microsec. pro ticks
_FirstScan			(*AT % M 0EB8*)		: USINT;	// high on initialization
_IOsegment			(*AT % M 0EBA*)		: UDINT;	// IO segment address
// contains DIAS-hardware-IDs for every possible DIAS-station number
_DIASconfig			(*AT % M 0EC0*)		: ARRAY [0..63] OF USINT;
_CDIASconfig    (*AT % M 0F00*)   : ARRAY [0..7] OF USINT;
_station        (*AT % M 0F08*)   : USINT;
_routingtable   (*AT % M 0F09*)   : ^sRoutingTable;
_cpus             (* AT % M 0F20 *) : USINT; // Multicore count
_cpus_RtCyEnabled (* AT % M 0F21 *) : USINT; // Multicore count enabled for RT/CY Threads (0=information not available)
_CpuDisplay			(*AT % D 0001*)		: USINT;	// display on x386 PLC's

OPS					(*AT % D 00003*)	: OPSys;	// realtime task data

	// header and storage for RAM-cells
	spRetBlocks	   (*AT % D 279*)	   : ^_RetHdr;     // Reitmeier erbe auf CP386
	aRamHdr		   (*AT % D 281*)	   : RAMhdr;


_lsl_pOS			(*AT % M 0EA2*)		: ^LSL_OSDATA;

_preHeapTotal     (*AT % M 10E8*)		: UDINT;		// full realtime heap memory
_preHeapFree      (*AT % M 10EC*)		: UDINT;		// free realtime heap memory
_preHeapUsed      (*AT % M 10F0*)		: UDINT;		// used realtime heap memory
_preHeapBiggest   (*AT % M 10F4*)		: UDINT;		// biggest available realtime heap memory block

_preMallocSize    (*AT % M 10F8*)		: UDINT;		// used realtime heap allocated by malloc
_PreMallocCount   (*AT % M 10FC*)		: UDINT;		// number of malloc calls
  
ThisMem									: ^VirtualBase;   // for installation of groups

END_VAR
