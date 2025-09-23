#pragma once
#define __RTOS__PRIVATE__


#include "RTOS_globals.h"
#include "RTOS_Interpreter.h"

//#ifdef LSL_TEST		// debug-version
	// Function adds message to a queue, which is read by the 
	// LASAL debugger tool.
	//
	// GanBer100420: Line number is not displayed since Lasal Class version 02.02.053
	// (0 instead of __LINE__, so inserted empty lines don't lead to program differences any longer
  #define TRACE_MSG(l,f,m)      _OutMessage(l, f, m, OUTMESSAGE_MSGTYPE_INFO)
  #define TRACE_MSG_WARN(l,f,m) _OutMessage(l, f, m, OUTMESSAGE_MSGTYPE_WARNING)
  #define TRACE_MSG_ERR(l,f,m)  _OutMessage(l, f, m, OUTMESSAGE_MSGTYPE_ERROR)
  
  #define TRACE(m)			        TRACE_MSG(0,__FILE__ID__,m)
  #define TRACE_WARN(m)			    TRACE_MSG_WARN(0,__FILE__ID__,m)
  #define TRACE_ERR(m)			    TRACE_MSG_ERR(0,__FILE__ID__,m)
  
  VAR_EXTERNAL
    FormattedText    : ARRAY[0..255] OF char;
  END_VAR
  
  #define TRACE0(m, t0)               TRACE1(m, t0, NIL)
  #define TRACE1(m, t0, t1)           TRACE2(m, t0, t1, NIL)
  #define TRACE2(m, t0, t1, t2)       _Format(m, t0, t1, t2);  TRACE(#FormattedText[0]);
  
  #define TRACE0_WARN(m, t0)          TRACE1_WARN(m, t0, NIL)
  #define TRACE1_WARN(m, t0, t1)      TRACE2_WARN(m, t0, t1, NIL)
  #define TRACE2_WARN(m, t0, t1, t2)  _Format(m, t0, t1, t2);  TRACE_WARN(#FormattedText[0]);
  
  #define TRACE0_ERR(m, t0)           TRACE1_ERR(m, t0, NIL)
  #define TRACE1_ERR(m, t0, t1)       TRACE2_ERR(m, t0, t1, NIL)
  #define TRACE2_ERR(m, t0, t1, t2)   _Format(m, t0, t1, t2);  TRACE_ERR(#FormattedText[0]);
  
	// return with previously set return values when condition is false
	#define ASSERT(condition) IF !!(condition) then return; end_if;
(*#else

	// don't do anything in a release version
	#define TRACE(m)
  #define TRACE0(m,t0)
  #define TRACE1(m,t0,t1)
  #define TRACE2(m,t0,t1,t2)
 	#define ASSERT(condition)
#endif*)

TYPE
  TokenNew : ( SN_PUNKT, SN_ID, SN_BSLASH, SN_EOF, SN_INVALID_CHAR) $ UDINT;
END_TYPE

FUNCTION GLOBAL ScanLexem
VAR_INPUT
	ppScan		: ^pChar;
	Lexem		: pChar;
END_VAR
VAR_OUTPUT
	token		: TokenNew;
END_VAR;

FUNCTION GLOBAL F_RD_USINT
VAR_INPUT
  ppFile    : ^pVoid;
END_VAR
VAR_OUTPUT
  val       : USINT;
END_VAR;

FUNCTION GLOBAL F_RD_UDINT
VAR_INPUT
  ppFile    : ^pVoid;
END_VAR
VAR_OUTPUT
  val       : UDINT;
END_VAR;

FUNCTION GLOBAL F_RD_UINT
VAR_INPUT
  ppFile    : ^pVoid;
END_VAR
VAR_OUTPUT
  val       : UINT;
END_VAR;

