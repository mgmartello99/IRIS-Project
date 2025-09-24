#pragma once
#define __RTOS__PRIV__DEFS__

// ========================================================================
//	constants for the configuration tables
// ========================================================================
// attributes of channels
#define _NO_ATTR  (16#0000 $ UINT)  // indicates no attributes are defined

// Connection States for Client Channels
#define  C_CONST   0 $UINT    // data is a constant
#define  C_OPT     1 $UINT    // connection is an option
#define  C_DIR     2 $UINT    // is direct connected and required
#define  C_OBLI    3 $UINT    // must be connected later
#define  C_INTERN  4 $UINT    // object is connected to base object
#define  C_IS_CON  5 $UINT    // it is connected
#define  C_INVALID 6 $UINT    // invalid name
#define  C_TRY     7 $UINT    // can be connected later or never
#define  C_EMBEDDED 8$UINT		// object is embedded to another object

#define NORM				0	// HW-signal is not inverted
#define INV					1	// HW-signal is inverted

// definition of IPR-loading modes
#define LOAD_DEFAULT		0	// IPR is only loaded if not valid in memory
#define LOAD_ALWAYS		1	// IPR is always loaded

//	flags for contents of configuration tables
#define CLS_CFG		16#0001		// configuration table exists
#define ONE_CFG		16#0002		// one configuration exists
#define TASK_CFG	16#0004		// task configuration table
#define IPR_CFG		16#0008		// IPR-configuration table
#define PRJ_CFG		16#0010		// PRJ-configuration table for Revision

// definition of keyword-symbols for the configuration tables
#define _K_KEY_      16#FF

#define _K_CLASS		(_K_KEY_ + (S_CLASS     $ USINT) * 256) $ UINT
#define _K_END_CONF	(_K_KEY_ + (S_END_CONF  $ USINT) * 256) $ UINT
#define _K_VAR		(_K_KEY_ + (S_VAR       $ USINT) * 256) $ UINT
#define _K_END_VAR	(_K_KEY_ + (S_END_VAR   $ USINT) * 256) $ UINT
#define _K_CONNECT	(_K_KEY_ + (S_CONNECT   $ USINT) * 256) $ UINT
#define _K_LINK		(_K_KEY_ + (S_LINK      $ USINT) * 256) $ UINT
#define _K_TASK		(_K_KEY_ + (S_TASK      $ USINT) * 256) $ UINT
#define _K_IPR		(_K_KEY_ + (S_IPR		$ USINT) * 256) $ UINT

//	keywords for task lists
#define _K_CYCLIC			0	// add object to cyclic task list
#define _K_REAL			1	// add object to realtime tasks
#define _K_BACKGROUND		2	// add object to background task list

// define für Timeslotkonfig
#define __LSLLDR_TIMESLOTS__	1
// define for IO-Elements
#define __LSLLDR_IOELEMENTS__	1
// flag keywords for Multimaster
#define  __LSLLDR_MULTIMASTER__		1 // Multimaster Version
#define  _MM_CHTY_4BYTE   0x00    // 4 byte Server, default
#define  _MM_CHTY_NBYTE   0x01    // n-Byte Server
#define  _MM_CHTY_STRING  0x02    // String Server
#define  _MM_CHTY_BIN     0x04    // binär Server
#define  _MM_CHTY_MERKER  0x08    // merker Server
#define  _MM_PROP_RO        0x01  // ReadOnly or WriteProtected
#define  _MM_PROP_NEWINST   0x02  // has NewInst commands
#define  _MM_PROP_REQUIRED  0x04  // is required 
// define for Multicore
#define __LSLLDR_TASKID__ 1
 
#ifndef OBJECTS_CONFIG
TYPE
	TCallInit		: STRUCT	// standard method lists
		pCmdMeth	: ^CmdMeth;
		pDataMeth	: ^ChMeth;
		pCltMeth	: ^CmdMeth;
		pMagicFus 	: ^CmdMeth;
		END_STRUCT;
END_TYPE
#endif // OBJECTS_CONFIG
