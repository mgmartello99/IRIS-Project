// SDEF Output From L:\RTKERNEL\LSL_OS\RTOS_C.H, line 1
//$$!SDEF ON
//
//	Lasal Application C Header File
//
//	Rev 1.0, 01/30/01
//
//	This file contains various definitions and includes for use by
//	Lasal C applications.
//
//

#pragma once

// ###%%%### THR 2/26/02: Required for VDDC Projects
//
//	Windows/MSVC Include Files
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>



//
//	Define Various Types
//
#define LSL_APPLICATION

//	Define misc. types for LSL_APPLICATION
#define RTKMailbox void*
#define RTKAPI
#define RTKSemaphore void
#define RTKTaskHandle long

typedef unsigned short	WORD;
typedef unsigned char	BYTE;
typedef unsigned long	DWORD;
typedef long 			LONG;
typedef int				BOOL;


typedef struct {
   unsigned long TimeLow;
   unsigned long TimeHigh;
} RTKFineTime;

typedef struct {                     // interrupt gate descriptor
   WORD  Offset_Lower;
   WORD  CodeSelector;
   unsigned int Id:13;
   unsigned int DPL:2;
   unsigned int P:1;
   unsigned int Offset_Upper:16;
} RTInterruptGate;



//
//	RTOS / LASAL Include Files
//
#include <RTOS_CGlobals.h>		// RTOS Global Def's for C
#include <RTOS_CMacros.h>

#include <LSL_C_IFSSR.h>	// THR LSE V1.40: Switched Order!
#include <LSL_CITask.h>

#include <LSL_C_IFLINKER.h>


// SDEF Output From L:\RTKERNEL\LSL_OS\LslDefs.h, line 30
//	$$!SDEF ON
//

//
//	Define some type definitions common to IEC-1131
//
typedef char           CHAR;
typedef unsigned char  USINT;
//typedef short          INT;
typedef unsigned short _UINT;
typedef long           DINT;
typedef unsigned long  UDINT;


//
//	Define Std. LASAL OS Calling Parameters
//
//		LSLREG: LASAL Register Parameter Calling Function - uses C fast call
//		LSLAPI: LASAL Task API - Uses RTTarget calling API
//
#define LSLREG _fastcall
#define LSLAPI RTTAPI


//
//	Define "freindly" names for some common RT Kernel functions
//
#define lslOutByte			RTOut
#define lslInByte			RTIn
#define lslSTI				RTEnableInterrupts();
#define lslCLI				RTDisableInterrupts();
#define lslIRQEnd			RTIRQEnd
#define lslEnableIRQ		RTEnableIRQ


//
//	Define Generic CPU LED Codes
//
#define LED_RAM     0   //  Run RAM (gelb)
#define LED_RAM_OFF 1
#define LED_ROM     2   //  Run ROM (gelb)
#define LED_ROM_OFF 3
#define LED_ERR     4   //  Error IRQ (rot)
#define LED_ERR_OFF 5

//
//	Define Generic CPU States
//

//     _1
//    ===
//   |   |
//  2| 4 |_2
//    ===
//  1|   |_4
//   |   |
//    ===  *8
//     _8
//
// Figures with "_" => lower nibble
//

#define LSL_DISP_ERROR_TXT       0x7950 // 0111 1001   0101 0000  "Er"
#define LSL_DISP_LINKER_TXT      0x3854 // 0011 1000   0011 0000  "Ln"
#define LSL_DISP_LINKRUN_TXT     0x7350 // 0111 1000   0011 0000  "Pr"
#define LSL_DISP_OPBURN_TXT      0x3F73 // 0111 1111   0011 0000  "OP"
#define ST_DEBUG    2
#define ST_ERROR    1
#define ST_RUN      0


//
//	Define CPU Status Codes
//
typedef enum
{
  CS_RUN_RAM = 0,
  CS_RUN_ROM,
  CS_RUNTIME,
  CS_POINTER,
  CS_CHKSUM,
  CS_WATCHDOG,
  CS_ERROR,
  CS_PROM_DEFECT,
  CS_RESET,
  CS_WD_DEFECT,
  CS_STOP,         //10
  CS_BUSY_PROG,
  CS_PGM_TOO_LONG,
  CS_PROG_END,
  CS_PROG_MEMO,
  CS_STOP_BRKPT,
  CS_CPU_STOP,
  CS_INTERROR,
  CS_SINGLESTEP,
  CS_READY,
  CS_LOAD,         //20
  CS_WRONG_MODULE,
  CS_MEMORY_FULL,
  CS_NOT_LINKED,
  CS_DIV_BY,
  CS_DIAS_ERROR,
  CS_WAIT,
  CS_OP_PROG,
  CS_OP_INSTALLED,
  CS_OP_TOO_LONG,
  CS_NO_OPSYS,     //30
  CS_SEARCH_OPSYS,
  CS_NO_DEVICE,
  CS_UNUSED_CODE,
  CS_MEMORY_ERROR,
  CS_MAX_IO,

  // Ab hier die LASAL-RTOS Meldungen
  CS_ACCESS = 50,
  CS_BND_EXCEED,
  CS_PRIV_INSTR,


  //Reserved for ScreenSaver:
  //scr_active   0x60
  //scr_         0x61
  //scr_         0x62

  CS_PRE_RUN   = 0xFD,
  CS_PRE_RESET,

  CS_LINKING   = 0xE0,
  CS_LINKERROR = 0xE1, 
  CS_LINKDONE  = 0xE2,

  CS_OP_BURN   = 230,

  CS_USVWAIT   = 0xF0,
  CS_REBOOT    = 0xF1,
  CS_RAMSAVE   = 0xF2,
  CS_RAMLOAD   = 0xF3,

  CS_NONE      = 0xFF,
}CP_STAT;


//
//	Define Command Status Codes
////////////////////////////////////
#define  COM_RUN_RAM		0
#define  COM_RUN_ROM		1
#define  COM_RUNTIME		2
#define  COM_POINTER		3
#define  COM_CHKSUM			4
#define  COM_WATCHDOG		5
#define  COM_ERROR			6		
#define  COM_PROM_DEFECT	7
#define  COM_RESET			8
#define  COM_WD_DEFECT		9
#define  COM_STOP			10
#define  COM_BUSY_PROG		11
#define  COM_PGM_TOO_LONG	12
#define  COM_PROG_END		13
#define  COM_PROG_MEMO		14
#define  COM_STOP_BRKPT		15
#define  COM_CPU_STOP		16
#define  COM_INTERROR		17
#define  COM_SINGLESTEP		18
#define  COM_READY			19
#define  COM_LOAD			20
#define  COM_WRONG_MODULE	21
#define  COM_MEMORY_FULL	22
#define  COM_NOT_LINKED		23
#define  COM_DIV_BY			24
#define  COM_DIAS_ERROR		25
#define  COM_WAIT			26
#define  COM_OP_PROG		27
#define  COM_OP_INSTALLED	28
#define  COM_OP_TOO_LONG	29
#define  COM_NO_OPSYS		30
#define  COM_SEARCH_OPSYS	31
#define  COM_NO_DEVICE		32
#define  COM_UNUSED_CODE	33
#define  COM_MEMORY_ERROR	34
#define  COM_MAX_IO			35

  // Ab hier die LASAL-RTOS Meldungen
#define  COM_ACCESS			50
#define  COM_BND_EXCEED		51
#define  COM_PRIV_INSTR		52

  //Reserved for ScreenSaver:
  //scr_active             0x60
  //scr_                   0x61
  //scr_                   0x62

#define  COM_PRE_RUN		253
#define  COM_PRE_RESET	    254
#define  COM_LINKING		224
#define  COM_LINKERROR	225
#define  COM_LINKDONE		226

#define  COM_OP_BURN		230

#define  COM_USVWAIT		240
#define  COM_REBOOT			241
#define  COM_RAMSAVE		242
#define  COM_RAMLOAD		243

#define  COM_NONE			255

  // can the next command be executed?
#define  COM_BUSY_FLAG		512
//////////////////////////////////////

typedef struct CMD_BUF_STRUCT
{
	unsigned long cmd;
	struct CMD_BUF_STRUCT* next;
} CMD_BUF;

//
//	The following structure is used to store the CPU registers.
//
typedef struct
{
   UDINT rGS;
   UDINT rFS;
   UDINT rES;
   UDINT rDS;
   UDINT rEDI;
   UDINT rESI;
   UDINT rEBP;
   UDINT rESP;
   UDINT rEBX;
   UDINT rEDX;
   UDINT rECX;
   UDINT rEAX;
// eip nur res (nicht am stack)
   UDINT rEIP;
   UDINT rCS;
// efl nur res (nicht am stack)
   UDINT rEFL;
   UDINT rSS;
   UDINT res0;
   UDINT res1;
   UDINT res2;
}LSL_REG;


// Structure of the registers in the Debugger routine:
// Never change this structure!!!
typedef struct
{
   UDINT rEDI;  // 04
   UDINT rESI;  // 08
   UDINT rEBP;  // 12
   UDINT rESP;  // 16
   UDINT rEBX;  // 20
   UDINT rEDX;  // 24
   UDINT rECX;  // 28
   UDINT rEAX;  // 32
   UDINT rEIP;  // 36
   UDINT  rCS;  // 40
   UDINT rEFL;  // 44
   UDINT  rSS;  // 48
   UDINT  rGS;  // 52
   UDINT  rFS;  // 56
   UDINT  rES;  // 60
   UDINT  rDS;  // 64
   UDINT rEIPrelativ; // 66
   UDINT ModulID;  // 68
}CP_REG;

//
//	Define Various Common Vectors
//	(common to ALL platforms)
//
// SW-INT Vectoren
#define DIV_VECTOR 0
#define TRP_VECTOR 1
#define NMI_VECTOR 2
#define BRK_VECTOR 3
#define BND_VECTOR 5
#define OPC_VECTOR 6
#define STK_VECTOR 12
#define GPE_VECTOR 13
#define PGF_VECTOR 14

// HW - Vectoren
#define HW_DEB_VECTOR 	1
#define HW_BRK_VECTOR   3
#define HW_BND_VECTOR   5


//
//	Define the LASAL Exception Structure
//
typedef struct
{
  USINT  type;
  #define LSL_EXC_HW         0x80
  #define LSL_EXC_SW         0x40
  #define LSL_EXC_ERROR      0x20
  #define LSL_EXC_WARNING    0x10

  USINT  source;
  #define LSL_EXC_DIAS       25
  #define LSL_EXC_CAN        200

  _UINT  code;
  // LSL_EXC_CAN:
  #define LSL_EXC_CAN_BOFF   1

  // LSL_EXC_DIAS:
//  #define LSL_EXC_DIAS_
}LSL_EXCEPT;


//
//--- Inline ASM Befehle -----------------------------------------------------
//	Define some in-line assmebly code macros
//
#define lslPUSHA    _asm pusha
#define lslPOPA     _asm popa
#define lslIRET     _asm iretd

//#define lslSaveESP  _asm mov dword ptr lsl_pOS->ESP, esp
#ifdef _BCC_
  #define lslPOPF     _asm popf
  #define lslPUSHF    _asm pushf
#else
  #define lslPOPF     _asm popfd
  #define lslPUSHF    _asm pushfd
#endif


//THR ###%%%### WHAT ARE THESE USED FOR???
//SOME ARE NOT LONGER NEEDED???
#define PLC_TASK          3
#define PLC_STACK_SIZE    0x200
typedef void (* LSL_PLC)(void);
//###%%%###///////////////////////////////////////////


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\SysUser.h, line 25
//	$$!SDEF ON
//



//
//	SYS_USER: User Configuration Definition
//		This structure is used to define the User Memory Area for the OS Layer.
//		This area is used to contain application program and data.  Each
//		platform must define the area according to its memory resources.
//		The structure contains the following entries:
//
//			LONG usrCodeStart - Start Address of User Code area
//			LONG usrCodeLength - Length (Bytes) of User Code area
//			LONG usrDataStart - Start Address of User Data area
//			LONG usrDataLength - Length (Bytes) of User Data area
//				These entries are used to define the starting address and length
//				of the User Code and Data Areas.  These areas are used by the
//				OS layer to locate and access the "application" (or "user")
//				memory areas.
//
//			LONG usrVersion: Platform OS Version
//				This entry is used to contain the version code of the platform OS.
//				The value can be any vresion code as desired.
//
//			LONG lClockTicks: Internal Interrupt Timer Ticks
//				This entry is to be set to the number of clock ticks (micro-seconds)
//				for the timer interrupt granularity.
//
//			LONG lRTPeriod: Real Time Interrupt Period
//				This entry must be set to the desired real time task interrupt
//				period in Milliseconds.
//
//			VOID* pDbgComPort: Debug COM Port String
//				This entry is set to a string to indicate the COMx port to use
//				for OS layer debugging.  The string should be set to a valid
//				communication port name (COM1, COM2, etc...).  The default port
//				(if set to a NULL pointer) is COM1.
//
//			LONG lDbgComBaud: Debug COM Baud Rate Index
//				This entry is set to the COM Baud Rate Index to be used for the
//				OS layer debug port.  The setting allows different platforms to
//				set a baud rate according to its capabilities.
//
//		Note that this structure also serves as the "USER" User Memory Area Interface
//		to allow the OS Layer to access the User Memory Area.
//

typedef struct
{
	unsigned long WHOAREYOU;      // Type Of Destination PLC
#define SYSUSER_DESTPLC_ISDCP 0x0000001
#define SYSUSER_DESTPLC_ISIPC 0x0000002
#define SYSUSER_DESTPLC_ISDSE 0x0000003
#define SYSUSER_DESTPLC_ISPCD 0x0000004

#define SYSUSER_DESTPLC_ISIPC_1024 0x0000005
#define SYSUSER_DESTPLC_ISIPC_800 0x0000006

  unsigned long LSLTASK_MAILBOXSLOTS;
  unsigned long usrCodeStart;		// Start address of User Code area
	unsigned long usrCodeLength;	// Length of User Code area
	unsigned long usrDataStart;		// Start address of User Data area
	unsigned long usrDataLength;	// Length of User Data area
	unsigned long usrVersion;		// Platform version code
	unsigned long lClockTicks;		// Clock interrupt ticks (uSecs)
	unsigned long lRTPeriod;		// Real time task period (mSecs)
	void* pDbgComPort;				// Pointer to Debug com port string
	unsigned long lDbgComBaud;		// Baud rate index
} SYS_USER;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\SysMain.h, line 23
//	$$!SDEF ON
//

//
//	RTKSYS.H: RT Kernel System Header File
//
//		This file contains various common definitions and references used by the
//		RTK Kernel Library and Platform Dependent Files.
//


//
//	Kernel Compilation Definitions
//
//		The following definitions are used to enable / disable various common
//		kernel options.
//
//	SYS_DEBUG: System Level Debug
//		When defined this symbol will cause the following debugger information and
//		functions to be included in the project.
//			CPUMonitorStart - The CPU Monitor Task is enabled
//			The Kernel Tracer is increased to 256 events
//			Source code symbols are loaded
//
#define SYS_DEBUG


//
//	Common Interface Library Definitions
//
#define SYS_INTERFACE_NAME	32		// Max. length of an interface name

//
//	Common Kernel Structures and Types
//

//
//	SYS_ERROR: System Error Enumeration
//
//		This enumeration is used to define all operating system error codes.
//
typedef enum
{
	SYS_ERR_NONE = 0,	// No error, OK
	SYS_ERR_NOMEM,		// Not enough memory
	SYS_ERR_IEXISTS,	// Interface exists
	SYS_ERR_INOEXISTS,	// Interface does not exist (not found)
	SYS_ERR_INOREF,		// Interface ref. count error (interface out of refs)
	SYS_ERR_IMODNUM,	// Invalid Module Number (DIAS address)
	SYS_ERR_DIASWORD,	// DIAS WORD access error
	SYS_ERR_DIASBYTE,	// DIAS BYTE access error
	SYS_ERR_DIASCTRL,	// DIAS CTRL access error
	SYS_ERR_DIASIRQ,	// DIAS IRQ error
	SYS_ERR_MAPMEM,		// RT Kernel Memory Mapping Error
	SYS_ERR_IPROJ,		// Invalid "user" project
	SYS_ERR_BAUD,		  // Invalid baud rate
	SYS_ERR_STN,      // Invalid station address

	SYS_ERR_CHKSUM,   // Invalid checksum
	SYS_ERR_ANY,      // General Error, not really specified

	SYS_ERR_LNK_NOACCESS,      // LINKER has no Memory Acces
	SYS_ERR_LNK_NOPROJECT,     // invalid project
	SYS_ERR_LNK_GENERAL,       // General Linker Error => Linker Error List

	SYS_ERR_FILEACCESS  // General File Error, not really specified

} SYS_ERROR;


//
//	SYS_CONFIG: System Configuration Structure
//
//		This structure is used to define various platform specific system information
//		required by the common kernel.  The platform dependent main() function must
//		form this structure then pass it to the Sys_Entry() function at system startup.
//		The structure contains the following entries:
//
//			NOTES: These settings MUST be set BEFORE the call to Sys_Entry();
//
//			WORD wHookedIRQs: Hooked IRQ Flag
//				This entry should have a bit set for any IRQ's that the OS expects
//				to install.  The flag word must be set before the RT Kernel is
//				initialized so that the kernel will set various low-level data for
//				the IRQ.  Note that the platform MUST NOT yet install any IRQ's -
//				this is done during the platform initialization phase later.
//				The flag MUST be set to zero if no IRQ's are to be hooked.
//
//			LONG lClockUps - System Clock Units / Second
//			LONG lClockDiv - System Clock Divisor
//				These equates are used to set the kernel clock to a period of 1ms with
//				a granularity of 1 usec.  The values should be set as follows:
//					timer unit = seconds * SYS_CLK_UPS / SYS_CLK_DIV
//				See the RTK Manual for ClkSetResolution() more information.
//				NOTE: Pentium systems with a BIOS do not seem to need to set
//						the timing (it seems to be read from the BIOS and
//						is properly initialized).  Setting these entries to
//						ZERO can be used to use the default timing.
//
//			NOTE: These settings can be set AFTER the call to Sys_Entry() during
//				  the call to Sys_PlatformInit() to allow the settings to be loaded
//				  by the platform from external media.
//
//			SYS_USER sysUser - User Configuration
//				This entry is used by the platform to define the User
//				Memory Area and User Timer Periods.
//
typedef struct
{
	// RTKernel Info (set BEFORE Sys_Entry() call)
	WORD wHookedIRQs;	// Hooked IRQ flag - set to 0 for none
	LONG lClockUps;		// Clock units / sec
	LONG lClockDiv;		// Clock divisor

	// Application Info
	SYS_USER sysUser;	// User Memory Area defs
} SYS_CONFIG;


//
//	RTOS System Priority
//
#define SYS_PRIORITY	3


//
//	Kernel Common API Function References
//
		
void Sys_Entry(SYS_CONFIG *pCfg);	// RT Kernel System Entry Call



/*$$!FDEF
   Name: Sys_PlatformInit
   Header:
   C Call: result = Sys_PlatformInit();
   Input:
   Return:
	SYS_ERROR result = SYS_ERR_NONE if no error
   Description:
	This is the function prototype for the Platform Specific Initialization Function.
	All platforms must define this function.  The RT Kernel will call into this
	function once the RT Kernel system, including timers, tasks, resources, CIL, etc.,
	have been prepared.  At this point the platform may install interfaces, tasks,
	create resources, etc.  The function must return ERR_SYS_NONE if all initialization
	was completed or one of the system error codes if any error is detected to
	prevent the system from running.
$$!ENDCMD*/
SYS_ERROR Sys_PlatformInit();		// Platform Init Function - defined in platform source


//
//	CIL Definitions
//
#define SYS_CIL_UNLIMITED	-1		// Unlimted interface ref's allowed
#define SYS_CIL_EXCLUSIVE	1		// Exclusive (single) ref allowed


//
//	Kernel Interfaces
//
//	CIL: Common Interface Library
//
//		This structure is used as the CIL Interface structure.  The
//		structure exposes the CIL API.
//
typedef struct
{
	// CIL Interface
	SYS_ERROR (* CILRegister)(char *pName, void *pInterface, WORD wRefLimit);
	SYS_ERROR (* CILGet)(char *pName, void **pInterface);
	SYS_ERROR (* CILRelease)(char *pName);
} SYS_ICIL;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\Syscil.h, line 22
//	$$!SDEF ON
//

//
//	RTKCIL.H: Common Interface Library
//
		
//
//	SYS_CILNODE: Common Interface List Node
//
//		This structure is used as the CIL List Node.  The CIL List contains a
//		node entry for all registered interfaces.  The structure contains the
//		following entries:
//
//			char szName[] - The NULL terminated device name, <= SYS_INTERFACE_NAME chars
//			void *pInterface - A pointer to the interface structure
//			WORD wRefLimit - Total number of allowed references, -1 for unlimited
//			WORD wRefCount - The current counter of references (0 for none)
//

typedef struct
{
	char szName[SYS_INTERFACE_NAME+1];	// Interface name
	void *pInterface;					// Pointer to STATIC interface structure
	WORD wRefLimit;						// Reference limit, -1 for unlimited
	WORD wRefCount;						// Current ref count
	void *pNext;						// List next node pointer
	void *pPrev;						// List prev node pointer
} SYS_CILNODE;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\Dled.h, line 24
//	$$!SDEF ON
//

//
//	The following structure is the DIAS DCP LED Device Interface.  The
//	device is installed as "DLED".  Note that not all platforms will
//	support this device.
//

typedef struct
{
	// LED Variables
	BYTE nLeds;			//
	WORD nDisplay;		//
	BYTE ledID;			//
	BYTE value;			//

	// LED Functions
	void (* Display)(void *piLed, BYTE nLeds, WORD nDisplay);	// Display LED data
	void (* SetLed)(void *piLed, BYTE ledID, BYTE value);		// Set a single LED
} DLED_IDLED;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\SysTime.h, line 41
//	$$!SDEF ON
//
//
//	The following structure is the System Time Measurement structure.
//	This structure is used by SYSTIME interface for event timing.
//

typedef struct
{
	unsigned long lEvents;		// count of events (times it has run)
	RTKFineTime ftStart;		// start execution time register
	unsigned long lTime;		// last execution time
	unsigned long lAve;			// average execution time (ave. of last 10)
	unsigned long lMin;			// min execution time (in uSec)
	unsigned long lMax;			// max execition time (in uSec)
} SYSTIME_MEAS;


//
//	The following structure is the System Time Elapsed Time
//	structure.  This structure is used to calculate elapsed time,
//	in uSec, since the last call.
//
typedef RTKFineTime SYSTIME_ELAPSED; // Time elasped base


//
//	SYSTIME Interface Structure
//
//	The following structure is the SYSTIME Interface Structure.  The structure
//	provides the API for the SYSTIME module.
//
typedef struct
{
	// Time Measurement Functions
	void (* ResetMeas)(SYSTIME_MEAS* pMeas);
	void (* StartMeas)(SYSTIME_MEAS* pMeas);
	void (* StopMeas)(SYSTIME_MEAS* pMeas);
	void (* ResetElapsed)(SYSTIME_ELAPSED* pElp);
	unsigned long (* Elapsed)(SYSTIME_ELAPSED* pElp);
	unsigned long (* ReadMicroSec)(void);
} SYS_ISYSTIME;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\SysQueue.h, line 22
//	$$!SDEF ON
//


//
//	SYS_QNODE: Queue Node Structure
//
typedef struct
{
	void* pNext;	// Pointer to next node in list
	void* pPrev;	// Pointer to prev node in list
	void* pData;	// Pointer to actual data
} SYS_QNODE;


//
//	LSLTSK_QUEUE: Queue Structure
//
typedef struct
{
	SYS_QNODE* pHead;	// Pointer to list header
	SYS_QNODE* pTail;	// Pointer to list tail
} SYS_QUEUE;

//
//	Task List Macros
//
// Reset a node to empty
#define Sys_ResetNode(nd) (nd)->pNext = (nd)->pPrev = NULL
#define Sys_NextNode(nd) (nd)->pNext

// Reset a list to empty
#define Sys_ResetQueue(q) (q)->pHead = (q)->pTail = NULL
#define Sys_GetQueueHead(q) (q)->pHead


//
//	SYSQUEUE Interface Structure
//
//	The following structure is the SYSQUEUE Interface Structure.  The structure
//	provides the API for the SYSQUEUE module.
//
typedef struct
{
	// Queue Functions
	SYS_QNODE* (* AddQueueNode)(SYS_QUEUE* pQueue, void* pData);
	void* (* RemoveQueueHead)(SYS_QUEUE* pQueue);
	void* (* RemoveQueueNode)(SYS_QUEUE* pQueue, SYS_QNODE* pNode);
	SYS_QNODE* (* FindQueueNode)(SYS_QUEUE* pQueue, void* pData);
	SYS_QNODE* (* InsertQueueNode)(SYS_QUEUE* pQueue, SYS_QNODE* pNd, void* pData);
} SYS_ISYSQUEUE;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\SysCRT.h, line 27
//	$$!SDEF ON
//

//
//	SYSCRT Interface Structure
//
//	The following structure is the SYSCRT Interface Structure.  The structure simply
//	wraps various C Runtime functions.
//
typedef struct
{
	// Math Functions
	long (* ftol)(double);
	int     (* abs)(int);
	double  (* acos)(double);
	double  (* asin)(double);
	double  (* atan)(double);
	double  (* atan2)(double, double);
	double  (* cos)(double);
	double  (* cosh)(double);
	double  (* exp)(double);
	double  (* fabs)(double);
	double  (* fmod)(double, double);
	long    (* labs)(long);
	double  (* log)(double);
	double  (* log10)(double);
	double  (* pow)(double, double);
	double  (* sin)(double);
	double  (* sinh)(double);
	double  (* tan)(double);
	double  (* tanh)(double);
	double  (* sqrt)(double);
	double  (* atof)(const char *);
	double  (* ceil)(double);
	double  (* floor)(double);
	double  (* frexp)(double, int *);
	double  (* _hypot)(double, double);
	double  (* _j0)(double);
	double  (* _j1)(double);
	double  (* _jn)(int, double);
	double  (* ldexp)(double, int);
	double  (* modf)(double, double *);
	double  (* _y0)(double);
	double  (* _y1)(double);
	double  (* _yn)(int, double);
	int (* sprintf)(char *s, const char *format, ... );

} SYS_ICRT;

//
//	Initialization Function
//	(not callable by application)
//
SYS_ERROR Sys_CRTInit();


//
//	Macros for use by C Applications
//
#define FTOL(dbl)		(LSL_POS->piCRT->ftol)(dbl)
#define SIN(dbl)		(LSL_POS->piCRT->sin)(dbl)
#define COS(dbl)		(LSL_POS->piCRT->cos)(dbl)
#define ABS(val)		(LSL_POS->piCRT->abs)(val)
#define ACOS(dbl)		(LSL_POS->piCRT->acos)(dbl)
#define SQRT(dbl)		(LSL_POS->piCRT->sqrt)(dbl)
#define POW(dbl1,dbl2)	(LSL_POS->piCRT->pow)(dbl1,dbl2)
#define LOG(dbl)		(LSL_POS->piCRT->log)(dbl)
#define ATOF(str)		(LSL_POS->piCRT->atof)(str)
#define SPRINTF			 LSL_POS->piCRT->sprintf

//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\SysCon.h, line 28
//	$$!SDEF ON
//

//
//	Always include the RTTextIO and RTKeyBrd Headers
//

#ifndef WUserInputFunction
typedef int (RTKAPI * WUserInputFunction)(void);
#endif

//
//	SYSCON Interface Structure
//
//	The following structure is the SYSCON Interface Structure.  The structure simply
//	wraps the RTTextIO and RTKeyBrd Module APIs.  See the RTT docs for information on these
//	functions.
//
typedef struct
{
	// RTTextIO Module
	void (* WSetUserInput)(WUserInputFunction GetInput);
	void (* WClearScreen)(int Attr);
	void * (* WNewWindow)(int FirstCol, int FirstRow, int LastCol, int LastRow, int BufferSize, int Attr, char * Title);
	void * (* WOpenWindow)(int FirstCol, int FirstRow, int LastCol, int LastRow, int BufferSize);
	void (* WCloseWindow)(void *W);
	void (* WFrame)(void *W, char *Title);
	void (* WGotoXY)(void *W, int Col, int Row);
	void (* WCursorXY)(int Col, int Row);
	void (* WCursorON)(void);
	void (* WCursorOFF)(void);
	void (* WSetCursor)(void *W);
	void (* WSetColor)(void *W, int color);
	void (* WSetScreenSize)(int Cols, int Rows);
	void (* WSetVideoRAMAddress)(unsigned short Segment,
									unsigned int Offset,
									unsigned short CRTController);
	int (* WPutC)(void *W, int c);
	int (* WPutS)(void *W, const char *s);
	char * (* WGetS)(void *W, char *s, int maxstrlen);
	int (* Wprintf)(void *W, const char *format, ... );
	void (* WDefineFunctionKey)(char ScanCode, char *S);

	// RTKeyBrd Module
	BOOL bIsColor;		// Set at init: TRUE if color
	RTKSemaphore *KBKeyAvailable;
	void (* KBInit)(void);
	BOOL (* KBKeyPressed)(void);
	int (* KBGetCh)(void);
	void (* KBPutCh)(unsigned char C, unsigned char ScanCode);

	// Special CLI Function - set to NULL if not used
	BOOL (* OpenCLIWindow)(void** wCmd, void** wStatus);
  SYS_ERROR (* LSE_Initialization)(void);
  int (* GetMouse)(int , int , void **);
  int (* SetMouse)(int , int , void *);
  int (* InitMouse)(int );
  int (* CalibTouch)(void);
} SYS_ISYSCON;



//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\COMMON\SysFile.h, line 23
//	$$!SDEF ON
//

//
//	The SYSFILE Interfac provides access to the RTFILES32 File System.
//
//	System File Notes:
//
//	1)	For now, the RTFILES default driver list is used.  This driver list
//		is set as follows:
//			Device 0, A:, Floppy Drive
//			Device 1, B:, Floppy Drive
//			Device 2, C:, IDE Fixed Disk
//			Device 3, D:, IDE Fixed Disk.
//		See the RTFILES32 documentation for a complete descrption of the default
//		driver list.
//
//	2)	The default file data settings are overridden in the PLATFORM maiun file.
//		See this code for more information.
//
//	Interface Name: SYSFILE
//	Required: NO
//	References: UNLIMITED
//

#pragma pack(push, 1)


//
//	SYS_IFILE: System File Interface Structure
//
//		This structure is the System File Interface structure.  The structure is
//		used to provide an interface to the System File functions.  The interface
//		provides the following entries:
//
//		File System Interface
//			The IFILE interface provides access to the
//			LASAL OS File System.  Access is given via the C Runtime Sysytem.
//			All threads that use this interface MUST be C RTL "aware".  It
//			is up to the user of the interface to properly handle all file
//			operations and to ensure that files are closed.
//
//		(list functions here!!!)
//
//		

typedef struct
{
	// File System Interface
	void (* clearerr)(FILE *);
	int (* fclose)(FILE *);
	int (* _fcloseall)(void);
	int (* feof)(FILE *);
	int (* ferror)(FILE *);
	int (* fflush)(FILE *);
	int (* fgetc)(FILE *);
	int (* _fgetchar)(void);
	int (* fgetpos)(FILE *, fpos_t *);
	char * (* fgets)(char *, int, FILE *);
	int (* _fileno)(FILE *);
	int (* _flushall)(void);
	FILE * (* fopen)(const char *, const char *);
	int (* fprintf)(FILE *, const char *, ...);
	int (* fputc)(int, FILE *);
	int (* fputs)(const char *, FILE *);
	size_t (* fread)(void *, size_t, size_t, FILE *);
	FILE * (* freopen)(const char *, const char *, FILE *);
	int (* fscanf)(FILE *, const char *, ...);
	int (* fsetpos)(FILE *, const fpos_t *);
	int (* fseek)(FILE *, long, int);
	long (* ftell)(FILE *);
	size_t (* fwrite)(const void *, size_t, size_t, FILE *);
	int (* remove)(const char *);
	int (* rename)(const char *, const char *);
	void (* rewind)(FILE *);
	int (* _rmtmp)(void);
	void (* setbuf)(FILE *, char *);
	int (* setvbuf)(FILE *, char *, int, size_t);
	char * (* _tempnam)(const char *, const char *);
	FILE * (* tmpfile)(void);
	char * (* tmpnam)(char *);
	int (* ungetc)(int, FILE *);
} SYS_IFILE;


//
//	End of File
//
#pragma pack(pop)

//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\DIAS\DIAS.h, line 21
//	$$!SDEF ON
//

//
//	DIAS.H: Device Device Interface
//
#pragma pack(push, 1)

//
//	The DIAS Device Interface provides a generic interface to the DIAS Master Device.
//	The interface is implemented according to the specific platform it is to be used
//	on.  The interface provides common access to the DIAS master regardless of the
//	specific platform it is implemented on.
//


//
//	Define DIAS ID Codes
//	(Physical ID's reported by DIAS protocol)
//
#define DIAS_ID_DDIxxx   0x00
#define DIAS_ID_DTOxxx   0x01
#define DIAS_ID_DDMxxx   0x02
#define DIAS_ID_DAI082   0x03
#define DIAS_ID_DAM122   0x0B
#define DIAS_ID_DNCxxx   0x13
#define DIAS_ID_DNC0xx   0x1B
#define DIAS_ID_DAIxxx   0x2B
#define DIAS_ID_DVIxxx   0x33
#define DIAS_ID_DDI162   0x3B
#define DIAS_ID_DEZxxx   0xDB
#define DIAS_ID_DNC4xx   0xFB
#define DIAS_ID_EMPTY_   0xFF


#define DIAS_ID_NONE	255	// No DIAS module detected


//
//	Define Misc. DIAS Equates
//
#define DIAS_MAX_MODULE	64	// Max number of DIAS module


//
//	DIAS Register Equates
//
//	(01/22/01 THR: Corrected from bit 0 to bit 1)
//
#define DIAS_IRQ_ERROR	2	// Bit 1: Set if Master Error IRQ TRUE

#define DIAS_ERR_WORD	0	// Word write error
#define DIAS_ERR_BYTE	1	// Byte R/W error
#define DIAS_ERR_CTRL	2	// Control W error
#define DIAS_ERR_IRQ	3	// IRQ Scan

// IRQ Scan
#define IRQ_SCAN_ID       0xC0
#define IRQ_MAX_SCAN_CNT  10
#define IRQ_INIT_SCAN_CNT 0


//
//	DIAS IRQ Structure
//
typedef struct
{
    BYTE ScanError;
    BYTE ScanErrorCnt;
    BYTE MasterOff;
}DIAS_IRQ;


typedef void (*ACTIONDIASIRQ) (unsigned short stationnumber ,void* thispointer);

typedef struct
{
	unsigned short station;
	void * thispointer;
	ACTIONDIASIRQ  actionpointer; 	
}ONE_DIAS_IRQSCAN;


typedef struct
{
	short int scananz;
	unsigned short IRQVector;
	ONE_DIAS_IRQSCAN scans[8]; 
}DIAS_IRQSCAN;


//
//	Initialization Function
//
//		Each platform specific DIAS driver implementation must design an
//		initialization function.  This function must prepare the DIAS interface
//		and register it.  The prototype for this function is platform dependent.
//
//	DIAS Interface Data
//
//		The DIAS Interface has been designed to support multiple DIAS Master ports
//		(if supported by the platform).  To support this the DIAS Interface Pointer
//		is passed as the first parameter to ALL DIAS Functions (much like the 'this'
//		pointer in c++).  The platform specific functions can make use of the
//		lParam member of the interface structure to contain port specific information.
//
//	DIAS Interface Functions
//
//		These functions define the various command and services provided by the DIAS
//		Device Driver libraries.  Again note that all function MUST have a DIAS_IDIAS*
//		pointer passed as the first parameter.
//
//	DIAS Interface
//
//		The following structure is the common DIAS Device Driver Interface.  All
//		DIAS device driver implementations will use this interface structure.
//			

typedef struct
{
	// Platform specific driver data
	LONG lParam;						// Platform specific parameter
	// DIAS Data
	DIAS_IRQ irq;							// DIAS IRQ info
    // Interface Functions
	SYS_ERROR (* ReadByte)(void* pDias, BYTE mod, WORD ofs, BYTE* pData);
	SYS_ERROR (* ReadWord)(void* pDias, BYTE mod, WORD ofs, WORD* pData);
	SYS_ERROR (* WriteByte)(void* pDias, BYTE mod, WORD ofs, BYTE bData);
	SYS_ERROR (* WriteWord)(void* pDias, BYTE mod, WORD ofs, WORD wData);
	SYS_ERROR (* ReadCtrl)(void* pDias, BYTE mod, WORD ofs, BYTE* pData);
	SYS_ERROR (* WriteCtrl)(void* pDias, BYTE mod, WORD ofs, BYTE bData);
	SYS_ERROR (* ReadRegister)(void* pDias, WORD ofs, BYTE* pData);
	SYS_ERROR (* WriteRegister)(void* pDias, WORD ofs, BYTE bData);
	BYTE (* GetModID)(void* pDias, BYTE mod);		// Get module ID code
	SYS_ERROR (* CheckError)(void* pDias);
	void  (* Lsl_DiasIRQ_Init)(void);
	unsigned short (*Lsl_DiasIRQ)(unsigned short Station,void* actionp, void* thispointer);

// above here you can add something  
	DIAS_IRQSCAN irqscan;					// Interrupt scan on the DIAS Bus
} DIAS_IDIAS;

#pragma pack(pop)

//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\LSL_OS\LslApp.h, line 33
//	$$!SDEF ON
//


//
//	LSL_HEADER: LASAL Application Project Header
//
//		This structure is used to define the header of an application
//		project.
//
//	THR: Updated structure on 12/05/00
//
typedef struct
{
  UDINT  nSections;       // number of sections
  UDINT  nProgramSize;    // incl. header size (0x100)
  UDINT  chk;				// CRC32 check register of hdr + code
  UDINT  chkModular;		// New: Modular CRC
  UDINT  dwCodeAddress;		// Start of code address
  UDINT  dwDataAddress;		// Start of data address
  USINT  free[0x100-(6*sizeof(UDINT))];	// reserved/unused header bytes
}LSL_HEADER;              // Zwischenpuffer zum Laden eines Modul-Headers

#define HEADER_SIZE 0x100


//
//	LSL_APP: LASAL Application Program Structure
//
//	This structure is used to contain various data for the LASAL Application
//	Program.
//

#define MAX_MODULE       0x100    // Max. Anzahl der Module

typedef struct
{
	void *pNothing;			// USED TO POINT AT MODULE HEADER - NO LONGER USED
    LSL_PLC pMain;        // pointer to main application function
    USINT   *pData;       // pointer to user data
	USINT   linkStep;		// Run-time linker/load step counter
    UDINT   nPrjSize;     // project length
    USINT   *pLoad;       // pointer to actual memoryblock for loading
    USINT   *pModule[MAX_MODULE]; // pointers to the modules

	// ###%%%### BELOW NEEDED???    
    UDINT   nHeaderSize;  // size of program header
}LSL_APP;


// Status fuer Projekt bzw. Module laden und linken
#define LINK_NONE            0x00
#define LINK_LOAD_PRJ_HEADER 0x01
#define LINK_LOAD_MOD_HEADER 0x02
#define LINK_LOAD_MODULE     0x03
#define LINK_LOAD_LINK       0x04
#define LINK_LOAD_EXE_MOD_HEADER	0x05


//
//	Checksum Return Codes
//
typedef enum
{
  CHK_OK = 0,
  CHK_POINTER_ERROR,
  CHK_CHECKSUM_ERROR,
  CHK_PROGRAM_LEN,
  CHK_LINKER_ERROR,
}LSL_CHK;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\LSL_OS\LslFile.h, line 23
//	$$!SDEF ON
//

//
//  File Mask Attributes
//
#define LSLFILE_ACTIVE  0x0001    // Active data file (power store)
#define LSLFILE_USER01  0x0002    // User File #1
#define LSLFILE_USER02  0x0004    // User File #2
#define LSLFILE_USER03  0x0008    // User File #3
#define LSLFILE_USER04  0x0010    // User File #4
#define LSLFILE_USER05  0x0020    // User File #5
#define LSLFILE_USER06  0x0040    // User File #6
#define LSLFILE_USER07  0x0080    // User File #7
#define LSLFILE_USER08  0x0100    // User File #8
#define LSLFILE_USER09  0x0200    // User File #9
#define LSLFILE_USER10  0x0400    // User File #10
#define LSLFILE_USER11  0x0800    // User File #11
#define LSLFILE_USER12  0x1000    // User File #12
#define LSLFILE_USER13  0x2000    // User File #13
#define LSLFILE_USER14  0x4000    // User File #14
#define LSLFILE_USER15  0x8000    // User File #15


//
//  File Format:
//
//  +00: File Header
//  +nn: Start of Records
//    +00: Class ID of Object (cGUID bytes)
//    +KK: Object Name (NULL terminated string, variable length)
//    +LL: 4 byte server data value (4 bytes)
//    +MM: Length of optional record data (4 bytes, set to 0 if no optional data)
//    +NN: Optional object specific data, variable length
//  +XX: Records...
//  +YY: Class ID = 0: End of file
//
#define LSLFILE_REV    0    // file format revision number
#define LSLFILE_SIG    "LASAL DATA FILE"
#define LSLFILE_SIG_LEN  16

//
//  LASAL Data File Header Structure
//
typedef struct
{
  unsigned char sig[LSLFILE_SIG_LEN];    // header signiture string
  unsigned long crc32;          // CRC of all file data after header
  unsigned long rev;            // LSLFILE_REV File format revision code
  unsigned long records;          // Count of data records in file
  unsigned long resv2;          // reserved/unused, set to 0
} LSLFILE_HEADER;



//
//  LASAL File Data Structure
//
//    This structure is used to contain the data used by the LASAL file system.
//    When a file function is activated, a new data structure is created to
//    contain the data for the active file.
//
#ifndef RTFHANDLE
typedef int RTFHANDLE;
#endif
typedef struct
{
  RTFHANDLE rtfh;        // Active file handle
  void* pSaveData;      // Pointer to saved LSLFILE_DATA structure
  long rec_dlen;        // Current record extra data length in bytes
  cGUID rec_id;        // Current record ID
  unsigned short FileMask;  // Current file mask
} LSLFILE_DATA;



//
//  LASAL File Interface Structure
//
typedef struct
{
  int (* Save)(char* fpath, _UINT FileAtt, void(*pPuts)(char*));
  int (* Load)(char* fpath, _UINT FileAtt, void(*pPuts)(char*));
  int (* SaveActive)(void(*pPuts)(char*));
  int (* LoadActive)(void(*pPuts)(char*));
  int (* Read)(void* pData, long dlen, long* pfbytes);
  int (* ReadString)(char* pstr);
  int (* Write)(void* pData, long dlen, long* pfbytes);
  void (* SetFileMask)(SVRCH* pSvr, _UINT uiFileMask);
  void (* SetActivePath)(char* path);
  LSLFILE_DATA* (* GetRecordData)();
} LSLFILE_ILSLFILE;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\LSL_OS\LslCLI.h, line 21
//	$$!SDEF ON
//

//
//	LASAL CLI Data
//
//	This structure is used to contain various LASAL CLI data.
//
typedef struct
{
	// Task status and exit flags
	BOOL bCLI;		// TRUE if CLI task is running
	BOOL bExit;		// Task terminate command flag (terminates both)
	BOOL bAutoexec;	// TRUE if running an auto-exec batch file

	// CLI Settings
	BOOL bReset;	// TRUE to start CLI on RESET
	BOOL bRun;		// TRUE to start CLI on RUN
} LSLCLI;

//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\LSL_OS\LslDiag.h, line 23
//	$$!SDEF ON
//

//
//	LASAL Diagnostic Services
//
//		This file contains the LASAL Diagnostics Services definitions.  These
//		services are used to provide various diagnostic functions and information.
//

//
//	Diagnsotics Data
//		This data structure is used to contain various diagnostics information.
//		The main LASAL lsl_pOS variable contains this structure.  Various
//		runtime OS functions will update the data in this structure.
//
typedef struct
{
	// Diagnsotic Display Data
	BOOL bDiagnosticWindow;			// TRUE if window/display task is open
	SYS_ISYSCON* piSysCon;			// console interface
	void* Window;				// window
	RTKTaskHandle DiagDisplayHandle; // dsplay task handle
} LSL_DIAG;


//
//	The following structure is the LASAL Diagnostics Interface Struture.
//	This interface is installed as an EXCLUSIVE interface and is named "LSLDIAG".
//	The interface is installed at startup.  An application can get this interface
//	to open / close the diagnostics window.  By default, the system starts with
//	the window closed.
//

typedef struct
{
	// Diagnsotic Display Data
	BOOL bDiagnosticWindow;			// TRUE if window/display task is open
	SYS_ISYSCON* piSysCon;			// console interface
	void* Window;				// window
	RTKTaskHandle DiagDisplayHandle; // dsplay task handle

	// Diagnostic Interface Function Pointers
	void (* DiagReset)(void);
	void (* DiagOpenWindow)(void);
	void (* DiagCloseWindow)(void);
	void (* DiagOpenCLI)(char*);
	void (* DiagCloseCLI)(void);
	void (* DiagResetCLI)(void);
	BOOL (* FindAutoExec)(char* pfname);
	LSLCLI* pCLI;
} LSL_IDIAG;


//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
// SDEF Output From L:\RTKERNEL\LSL_OS\Lsl_os.h, line 92
//	$$!SDEF ON
//
//
//	LSL_OSDATA: LASAL O/S Data Structure
//
//		The following structure is the LASAL OS Data Structure.  This is the main data
//		structure used by the LASAL OS.  It contains various entries common to various
//		LASAL OS tasks, services and functions.
//
//
//
//
typedef struct
{
	// Kernel Layer Data
	SYS_ICIL* piCIL;			// a pointer to the CIL interface from the kernel

	// DIAS Device Data
	DIAS_IDIAS* piDias;			// pointer to the main DIAS Bus master device interface

	// DLED Device Interface
	DLED_IDLED* piDLed;			// LED device
	
	// User Memory Area Definition
	SYS_USER* pUser;			// pointer to user mem def struct interface
	
	// Pointer to System Time Interface
	SYS_ISYSTIME* piSysTime;	// System Time interface

	// Pointer to System Queue Interface
	SYS_ISYSQUEUE* piSysQueue;	// System Queue interface

	// Pointer to LASAL Task Interface
	LSL_ITASK* piTask;

	// Pointer to C Runtime Tinterface
	SYS_ICRT* piCRT;

	// Pointer to LASAL System Service Interface  // by CGCS
	LSL_SSR* piSSR;

    // LASAL Diagnostic Interface Information
	LSL_IDIAG* piDiag;			// diagnostic Interface information

    // LASAL Linker Information 
	LSL_LINKER* piLinker;

	// LASAL File Interface (V3.18)
	LSLFILE_ILSLFILE* piLslFile;		// interface is OPTIONAL and may not exist!

	// LASALCOM Interface Pointer (V3.23)
	void* piLASALCOM;					// LASALCOM interface pointer

	// *** Add interface pointers here! ***

	// *** Add APP to LASAL_OS pointers here! ***

	// LASAL Application Program Data
	LSL_APP userAppl;			// LASAL Application Program Data
	
	// Debug LED value
	USINT lslDebug;				// Last debug LED value
  
	// Exception Processing Data
	RTKMailbox lslExceptionMail;	// Exception Mailbox
	unsigned long lLastExceptionCode;			// code of last exception
	unsigned long lExpectionCount;	// count of exceptions
	char szExceptThread[16];		// Name of thread at exception

	// Debug Data
	RTKMailbox debMail;
	RTInterruptGate OrgDebVector;   // to save original vector
	BOOL debMode;					// TRUE for LASAL bp's, FALSE for RTD32 bp's
	
	// Misc. Variables
	BYTE wdBit;					// Watchdog error bit
	unsigned long ESP;			// Saved EXP register
	BYTE statusExtended1;
	_UINT lslErrorVT;

	// Fatal error "jump buffer"
	//jmp_buf jmpError;
	//jmp_buf jmpSAQ;
	int tls_jmpbuf;			// task local data "jmp_buf" index

	unsigned long commStatus;  

} LSL_OSDATA;



//
//	Global Data References
//

#ifndef lslEntry_c
extern LSL_OSDATA* lsl_pOS;		// pointer to LASAL O/S Data Structure
extern LSL_OSDATA* LSL_POS;		// pointer to LASAL O/S Data Structure for APPLICATIONS
#endif

//
//	LEAVE THIS COMMENT OR DIE A SLOW AND PAINFUL DEATH
//	Turn AUTODOC.EXE symbol generation OFF
