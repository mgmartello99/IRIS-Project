
//#include <RTOS_Private.h>
//#include "RTOS_PrivDefs.h"
//#include <RTOS_Interfaces.h>

//#include <RTOS_Variables.H>


VAR_GLOBAL

_RealMaximumTime	AT % M 0000		: UDINT;	// maximum duration of RT-methods
_RealAverageTime	AT % M 0004		: UDINT;	// average duration of RT-methods
_CyclicMaximumTime	AT % M 0008		: UDINT;	// maximum duration of Cy-methods
_CyclicAverageTime	AT % M 000C		: UDINT;	// average duration of Cy-methods
_BackgroundMaximumTime	AT % M 0010	: UDINT;	// maximum duration of Bg-methods
_BackgroundAverageTime	AT % M 0014	: UDINT;	// average duration of Bg-methods

_RealMaximumTimes	AT % M 0390		: ARRAY [0..15] OF UDINT;	// multicore maximum duration of RT-methods
_RealAverageTimes	AT % M 03D0		: ARRAY [0..15] OF UDINT;	// multicore average duration of RT-methods
_CyclicMaximumTimes	AT % M 0410		: ARRAY [0..15] OF UDINT;	// multicore maximum duration of Cy-methods
_CyclicAverageTimes	AT % M 0450		: ARRAY [0..15] OF UDINT;	// multicore average duration of Cy-methods

_UserHeapStartAddr  AT % M 0020		: UDINT;	// start address of UserHeapMemory
_UserHeapTotalSize  AT % M 0024		: UDINT;	// total size of UserHeapMemory
_UserHeapUsedMem    AT % M 0028		: UDINT;	// amount of used memory from UserHeap
_UserHeapFreeMem    AT % M 002C		: UDINT;	// amount of free memory from UserHeap

kernel 				AT % M 0030		:^void;
_MaxDataMem         AT % M 003C     : UDINT;    // maximum amount of the projects data memory
_MaxCodeMem         AT % M 0040     : UDINT;    // maximum amount of the projects code memory

_diskOperatingSeconds   AT % M 0108     : UDINT; // lifetime of disk in drive c: in seconds
_diskSectWrCnt          AT % M 010C     : UDINT; // total number of disc sector writes 
_diskSectWrCnt1000Sec   AT % M 0110     : UDINT; // number of disc sector writes in the last 1000 seconds
_diskSectWrCnt10Sec     AT % M 0114     : UDINT; // number of disc sector writes in the last 10 seconds 
_diskSectWrCntSramdisk  AT % M 0118     : UDINT; // number of disc sector writes due to sramdisk backup

_sramDiagFlags  AT % M 011C     : UDINT; // flags that indicate errors in os (Bit 0:sram not written at powerdown, Bit 1: no acces to sd card)

_onlineDiagFlags  AT % M 0120     : UDINT; // flags that indicate status of online server (Bit 0:status lasal online communication port (def. 1954), Bit 1:comlink refreshlist connection closed)

_RtOSPatchVersion   AT % M 02E4     : CHAR;     // patch version of lasal os ('A', 'B', 'C', usw.)

_WhoAmI				AT % M 02E8		: UDINT;    // What CPU type 

_RtOSversion		AT % M 02EC		: UDINT;	// < 3.47 ... 1, else 347 aso.

_UserProgPointer	AT % M 02F0		: ^USINT;	// address of executable program
_UserProgSize   	AT % M 02F4		:  UDINT;	// size of executable program
_UserDataPointer	AT % M 02F8		: ^USINT;	// address of memory
_UserDataSize		AT % M 02FC		:  UDINT;	// size of memory
_S_RAM_Hptr		    AT  %M 0300     : ^MRAM_DESCR ;   // -> Pointer auf S_RAM_Header  

_rtruntime          AT % M 0304     : USINT;
_swrtruntime        AT % M 0305     : USINT;
_RTrtIntVal         AT % M 0306     : UDINT;

_btruntime          AT % M 030A     : UDINT;
_swbtruntime        AT % M 030E     : UDINT;

_heapAllocCnt		AT % M 0312		: UDINT;  	// UserHeap allocate -> inc;
_rebootOnError		AT % M 0316		: USINT;  	// 0=kein Reboot on error, >0=Delay in Sek., bis nach einem Fehler ein Reboot ausgeführt wird
_isRebootCodeAvailable AT % M 0317  : USINT;    // 1=die Sysvar rebootCode ist gültig
_rebootCode         AT % M 0318		: USINT;    // CPU Status Code vor dem 'Reboot on error'
_SysTaskMeasEnable  AT % M 0319		: USINT;

_noUserOffscreen		AT % M 0330		: UDINT;  	// number of user created offscreens
_noUserGetImage  		AT % M 0334		: UDINT;  	// number of user created images
_noUserRegion   		AT % M 0338		: UDINT;  	// number of user created regions

_RTRuntimes    AT % M 0350 : ARRAY [0..15] OF USINT; // Multicore Realtime Runtime value (index = core-number; value for core 0 not used - see sysvar.RTRuntime)
_swRTRuntimes  AT % M 0360 : ARRAY [0..15] OF USINT; // Multicore Realtime Runtime Resetvalue (index = core-number; value for core 0 not used - see sysvar.swRTRuntime)
_runtimes      AT % M 0370 : ARRAY [0..15] OF USINT; // Multicore Cyclic Runtime value (index = core-number; value for core 0 not used - see sysvar.runtime)
_swRuntimes    AT % M 0380 : ARRAY [0..15] OF USINT; // Multicore Cyclic Runtime Resetvalue (index = core-number; value for core 0 not used - see sysvar.swRuntime)

_cpuLoad			AT % M 0EA6		: UINT;		// cpu load in promille
_runstatus			AT % M 0EAA		: USINT;	// program runs form ROM/RAM
_runtime			AT % M 0EAE		: USINT;	// runtime counter
_swruntime			AT % M 0EAF		: USINT;	// setpoint for runtime counter
_error_cnt_dias		AT % M 0E94		: USINT;	// counter for bus errors
_error_station_dias	AT % M 0E95		: USINT;	

_OnlineMap AT % M 0E96 : STRUCT	//counts how many users are online with the following interfaces
		bySerial					: USINT; //serial online counter
		byTCP						: USINT; //TCP online counter
		byCAN						: USINT; //CAN online counter
		byReserved					: USINT;
	END_STRUCT;

_ClockTicks			AT % M 0EAB		: UINT;		// microsec. pro ticks
_FirstScan			AT % M 0EB8		: USINT;	// high on initialization
_IOsegment			AT % M 0EBA		: UDINT;	// IO segment address
// contains DIAS-hardware-IDs for every possible DIAS-station number
_DIASconfig			AT % M 0EC0		: ARRAY [0..63] OF USINT;
_CDIASconfig    AT % M 0F00   : ARRAY [0..7] OF USINT;
_cpus             AT % M 0F20 : USINT; // Multicore count
_cpus_RtCyEnabled AT % M 0F21 : USINT; // Multicore count enabled for RT/CY Threads (0=information not available)
_CpuDisplay			AT % D 0001		: USINT;	// display on x386 PLC's

OPS					AT % D 00003	: OPSys;	// realtime task data


	// header and storage for RAM-cells
	spRetBlocks	AT % D 279	: ^_RetHdr;
	aRamHdr		AT % D 281	: RAMhdr;


_lsl_pOS			AT % M 0EA2		: ^LSL_OSDATA;

_enableMallocLog  AT % M 10E4		: UDINT;		// activate additional logging for malloc

_preHeapTotal     AT % M 10E8		: UDINT;		// full realtime heap memory
_preHeapFree      AT % M 10EC		: UDINT;		// free realtime heap memory
_preHeapUsed      AT % M 10F0		: UDINT;		// used realtime heap memory
_preHeapBiggest   AT % M 10F4		: UDINT;		// biggest available realtime heap memory block

_preMallocSize    AT % M 10F8		: UDINT;		// used realtime heap allocated by malloc
_PreMallocCount   AT % M 10FC		: UDINT;		// number of malloc calls
  
END_VAR

