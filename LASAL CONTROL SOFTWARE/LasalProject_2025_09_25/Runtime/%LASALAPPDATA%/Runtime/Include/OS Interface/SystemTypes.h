#pragma once
#define __LSL__SYSTEM__DATA__TYPES__


#include ".\TimeTypes.h"


// defines for TaskTypes of Objects
#define OBJ_CT 1  // cyclic
#define OBJ_RT 2  // realtime
#define OBJ_TT 3  // foreground
#define OBJ_BG 4  // background
#define OBJ_HP 3  // high-priority user

// obj debug actions
#define ACTION_IDLE    0 // no action, but return value indicates wether obj exists or not
#define ACTION_NEWTIME 1 // set new period time
#define ACTION_SUSPEND 2 // suspend obj
#define ACTION_CONTNUE 3 // continue obj if suspended
#define ACTION_SIGNAL  4 // signal obj (run once)
#define ACTION_ADD     5 // add obj to updatelist
#define ACTION_ALWAYS  6 // set always flag

 TYPE
#pragma pack(push, 4)

  OBJTIME_MEAS : STRUCT
    lEvents : UDINT;		// count of events (times it has run)
	lTime   : UDINT;		// last execution time
	lAve    : UDINT;	    // average execution time (ave. of last 10)
	lMin    : UDINT;		// min execution time (in uSec)
	lMax    : UDINT;		// max execition time (in uSec)
  END_STRUCT;

  LSLOBJ_INFO  : STRUCT
    lastcall   : UDINT;     // uSec time since last call (for display)
    orgperiod  : UDINT;     // original Task execution time in uSec
    period     : UDINT;     // Task execution time in uSec
    orgstatus  : UDINT;     // Task status defintions
    status     : UDINT;     // Task status defintions
    statistics : OBJTIME_MEAS;  // Task time structure
  END_STRUCT;

#pragma pack(pop)

END_TYPE

