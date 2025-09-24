// The LASAL compiler will take the method headers defined here as
// a prototype for all standard methods of all classes in the current project.

// The compiler will assume the following order of functionalities:
// 
//	Method-No.	Default-Name	Function
//----------------------------------------------------------------------------------
//	1		Read		Reads the contents of the server channel
//	2		Write		Writes to a server channel
//	3*		Init		Initializes an object
//	4*		CyWork		low priority background work
//	5*		RtWork		high priority realtime work
//	6*		NewInstr	send interpreter commands to an object
//	7*		Kill		terminate all object activities
//	8*		GetState	observe execution of interpreter commands

// Remarks:   	*	these functions are only available for Server Channel Commands

#ifndef __CLASS__STANDARD__METHODS__
#define __CLASS__STANDARD__METHODS__

#include <RTOS_Interpreter.h>	// IprStates

CLASS
	// The 'Read' method provides the characteristic data 
	// of a server channel. It's default implementation will
	// return the contents of the channels 'dData'-element.
	// So 'Read' can be called for each server.
	//[<output(EAX)]a 4 byte return value
	FUNCTION VIRTUAL GLOBAL Read
		VAR_OUTPUT
			output(EAX)		: DINT; 	// data
		END_VAR;


	// The behaviour of the 'Write' method is not predefined.
	// Usually it will be used to write data to the channel's
	// 'dData'-element. This can be achieved by simply un-
	// checking the 'write-protection'-flag in the method's
	// context menu (right-click on 'Write').
	// The default implementation of 'Write' simply does nothing.
	// So 'Write' can be called for each server.
	//[>input(EAX)]a 4 byte input parameter
	//[<result(EAX)]a 4 byte return value with implementation specific meaning
	FUNCTION VIRTUAL GLOBAL Write
		VAR_INPUT
			input(EAX)		: DINT;		// data
		END_VAR
		VAR_OUTPUT
			result(EAX)		: DINT;		// result
		END_VAR;

	// The 'Init' method can be used to initialize an object
	// with data that can't be provided as constants in the
	// object's configuration.
	// The 'Init' method will be called for each configured
	// object at start-up of the system by the operating system.
	// At this time all the other objects are configured, too.
	// All the connections to other objects are established. So
	// the 'Init' method can already access the environment.
	// The default implementation of 'Init' simply does nothing.
	// So 'Init' can be called for each command server.
	FUNCTION VIRTUAL GLOBAL Init;

	// The 'CyWork' method is called by the operating system for
	// each object, that is configured to perfom a cyclic job.
	// Therfore the class has to have this method implemented, and
	// each object has to have a cyclic time set (see property sheet
	// of the object property dialog).
	// Of course a parent object can call the 'CyWork'-methods of it's
	// child-objects, too.
	// The default implementation of 'CyWork' simply does nothing.
	// So 'CyWork' can be called for each command server.
	//[>EAX]implementation dependant information, 4 byte, optional
	//[<state(EAX)]implementation dependant information, 4 byte
	FUNCTION VIRTUAL GLOBAL CyWork
		VAR_INPUT
			EAX				: UDINT := EAX;
		END_VAR
		VAR_OUTPUT
			state(EAX)		: UDINT;	// state
		END_VAR;

	// The 'RtWork' method is called by the operating system for
	// each object, that is configured to perfom a cyclic job in real-time.
	// Therfore the class has to have this method implemented, and
	// each object has to have a cyclic time set (see property sheet
	// of the object property dialog).
	// Of course a parent object can call the 'RtWork'-methods of it's
	// child-objects, too.
	// The default implementation of 'RtWork' simply does nothing.
	// So 'RtWork' can be called for each command server.
	//[>EAX]implementation dependant information, 4 byte, optional
	//[<state(EAX)]implementation dependant information, 4 byte
	FUNCTION VIRTUAL GLOBAL RtWork
		VAR_INPUT
			EAX				: UDINT := EAX;
		END_VAR
		VAR_OUTPUT
			state(EAX)		: UDINT;	// state
		END_VAR;

	// The 'NewInst'-method is used to implement class-specific
	// commands.
	// This method can provide a class interface besides the 
	// implemented class-methods. This interface will be used, when
	// working with interpreter code, or for remote calls to an
	// object.
	// Define the commands in the method editor, LASAL will create
	// a private data type with the command-enums. 'NewInst' will
	// typically contain a CASE-statement to evaluate each of these
	// commands. It has to return either READY, BUSY, ERROR or 
	// ERROR_BUSY to indicate, if the command was processed or if
	// it is still processed (i.e. a movement) or if an error occured.
	// However, 'NewInst' is just called once (per command) to 
	// initialize an operation. Eventually 'GetState' or 'Kill' are
	// called later on.
	// The default implementation of 'NewInst' simply does nothing.
	// So 'NewInst' can be called for each command server.
	//[>pPara]2 byte command enum, up to 80 bytes of parameter data
	//[>pResult]result data, length is buffer length+sizeof(UINT)
	//[<ret_code]indicates if operation is successfully completed
	FUNCTION VIRTUAL GLOBAL NewInst
		VAR_INPUT
			pPara			: ^CmdStruct;	// parameter buffer
			pResult			: ^Results;		// result buffer
		END_VAR
		VAR_OUTPUT
			ret_code		: IprStates;	// state
		END_VAR;

	// The 'Kill'-method is called by the operating system, if an
	// object is processing a 'NewInst'-command for more than just
	// one cycle, and needs now to prematurely terminate this 
	// operation. The function should enable the object to control
	// this abortion. It will be called as long as it returns BUSY.
	// The default implementation of 'Kill' simply does nothing.
	// So 'Kill' can be called for each command server.
	//[<ret_code(EAX)]indicats, if execution of a command is terminated
	FUNCTION VIRTUAL GLOBAL Kill
		VAR_OUTPUT
			ret_code(EAX)	: IprStates;	// state
		END_VAR;

	// The 'GetState'-method is called by the operating system, if an
	// object is processing a 'NewInst'-command for more than just
	// one cycle (i.e. a movement). 'GetState' is called as long as it
	// returns either BUSY or ERROR_BUSY. It gets passed the same 
	// parameter set as the 'NewInst'-method.
	// The default implementation of 'GetState' simply does nothing.
	// So 'GetState' can be called for each command server.
	//[>pPara]2 byte command enum, up to 80 bytes of parameter data
	//[>pResult]result data, length is buffer length+sizeof(UINT)
	//[<ret_code]indicates if operation is successfully completed
	FUNCTION VIRTUAL GLOBAL GetState
		VAR_INPUT
			pPara			: ^CmdStruct;		// parameter buffer
			pResult			: ^Results;	// result buffer
		END_VAR
		VAR_OUTPUT
			ret_code		: IprStates;	// state
		END_VAR;

END_CLASS

#endif // __CLASS__STANDARD__METHODS__