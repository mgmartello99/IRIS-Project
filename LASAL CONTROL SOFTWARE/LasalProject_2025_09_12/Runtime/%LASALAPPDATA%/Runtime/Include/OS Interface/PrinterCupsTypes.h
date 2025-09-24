#pragma once
#define  __PRINTERCUPSTYPES_H

// Error Values
#define PRINTERCUPS_ERR_OK			0		// Function was successful
#define PRINTERCUPS_ERR_PARAM		-1		// Parameters given to the function were not OK
#define PRINTERCUPS_ERR_INTERN		-2		// An internal error of the printing system occured
#define PRINTERCUPS_ERR_INDEX		-3		// There is no element with the given index
#define PRINTERCUPS_ERR_FOPEN		-4		// A file couldn't be opened by the printing system
#define PRINTERCUPS_ERR_NOTFOUND	-5		// An element couldn't be found by the printing system
#define PRINTERCUPS_ERR_NOPRINTERS	-6		// No printers are installed
#define PRINTERCUPS_ERR_MEM			-7		// Too less memory

// Printer States
#define PRINTERCUPS_STATE_IDLE		3		// Printer is in Idle state
#define PRINTERCUPS_STATE_PROC		4		// Printer is processing a job
#define PRINTERCUPS_STATE_STOP		5		// Printer is stopped

// Flags
#define PRINTERCUPS_FLAG_REM_ALL	0x01	// Flag to be used to remove everything