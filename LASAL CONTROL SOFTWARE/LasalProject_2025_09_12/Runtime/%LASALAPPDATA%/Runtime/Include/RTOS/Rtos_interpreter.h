#pragma once
#define __RTOS__INTERPRETER__


// maximum number of DINT-parameters for one command
#define MAXCMDPARA       20

// This bit can be set in the command UINT of the CmdStruct
// that is passed to the NewInst()-method.
// The interpreter will keep a pointer to the 'parallel'
// object, and call a Kill() when the interpreter is stopped.
// Every sequential command will remove the object pointer
// from the interpreter list.

#define PARALLEL_CALL	16#8000	

// *************************************************************************
//			data types for interpreter operations
// *************************************************************************


TYPE
#pragma pack(push, 1)
		// structure of a command for an object,
		// these commands are executed by calling the NewInstr
		// of the respective class, and a pointer to such a 
		// structure is passed to the function
	CmdStruct                       : STRUCT
		// Usually class specific commands are passed in uiCmd.

		// Except the following list of system messages. The user
		// might want to check for these:
		// After a client channel of an object has been connected at
		// runtime, the OS will send a LSL_MSG_CONNECT to the object.
		// The 1st parameter in the list contains a pointer to the
		// connected client channel.
		#define LSL_MSG_CONNECTED	16#7FFF
		// When an object should be initialized with retentive data,
		// the OS will send a LSL_MSG_RETENTIVE to the object.
		// The parameters are the pointer to the allocated data buffer,
		// the size of this buffer and the pointer to the server channel, 
		// that gets the data.
		#define LSL_MSG_RETENTIVE	16#7FFE
		// After a client channel of an object was disconnected 
		// from it's server, the object will receive a LSL_MSG_DISCONNECT
		// message.
		// The 1st parameter in the list contains a pointer to the
		// disconnected client channel.
		#define LSL_MSG_DISCONNECT	16#7FFD

		// The most significant bit can be used for a variation
		// of the standard commands, i.e. MOVE+16#8000 = MOVE_OVER
	  uiCmd                         : UINT;
	  	// variable length and structure of parameters
	  aPara                         : ARRAY [0..MAXCMDPARA-1] OF DINT;
	 END_STRUCT;

		// the above mentioned functions may return values by
		// writing them to this buffer, a pointer to a result buffer
		// is also passed to the NewInstr-method		                          
	results                         : STRUCT
		// total length of used buffer incl. 2 bytes for uiLng
	  uiLng                         : UINT;
	  	// variable length and structure of return values
	  aData                         : ARRAY [0..16#FF-sizeof(UINT)] OF USINT;
	 END_STRUCT;

	iprStates                       : (
	  READY,		// interpreter command processed
	  ERROR,		// i.e. wrong parameters
	  ERROR_BUSY,	// i.e. stopping movement
	  BUSY,			// interpreter command processing
  	  QUIT,
	  STOP_BP,		// interpreter is on an Breakpoint
	  SINGLESTEP	// interpreter is in Singlestep
	  );

#pragma pack(pop)
END_TYPE

