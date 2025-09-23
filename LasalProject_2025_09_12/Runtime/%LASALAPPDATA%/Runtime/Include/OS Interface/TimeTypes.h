#pragma once
#ifndef __SYS_TIME_TYPES
#define __SYS_TIME_TYPES

TYPE

	  SYSTIME		: STRUCT
	   wHour        : UINT; 
	   wMinute      : UINT; 
	   wSecond      : UINT;
	  END_STRUCT;
	  
	  SYSDATE       : STRUCT
	   wYear        : UINT; //must be between 1990 and 2199 !!
	   wMonth       : UINT; 
	   wDay         : UINT; 
	   wDayOfWeek   : UINT;
         END_STRUCT;

	SYSDATETIMEMS       : STRUCT
		wYear           : UINT;
		wMonth          : UINT;
		wDay            : UINT;
		wDayOfWeek   	: UINT;
		wHour      	 	: UINT; 
		wMinute     	: UINT; 
		wSecond     	: UINT;
		wMillisecond	: UINT;
	END_STRUCT; 				  

END_TYPE

#endif
