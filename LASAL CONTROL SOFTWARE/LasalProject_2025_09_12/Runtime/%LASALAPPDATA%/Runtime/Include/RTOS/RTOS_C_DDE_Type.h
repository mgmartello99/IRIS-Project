#pragma once
#ifndef __DEF_DDE_
	#define __DEF_DDE_

#pragma pack (push, 1)

	 typedef struct { unsigned long     Second:5; 
										unsigned long     Minute:6; 
										unsigned long     Hour:5; 
										unsigned long     Day:5; 
										unsigned long     Month:4; 
										unsigned long     Year:7; 
	 } _DDE_DATIM; 


	 typedef struct 
	 { 
			 char               FileName[8]; 
			 char               Extension[3]; 
			 unsigned char      Attributes; 
			 char               Reserved[8]; 
			 unsigned short     FirstClusterHi; 
			 _DDE_DATIM         DateTime; 
			 unsigned short     FirstCluster; 
			 long               FileSize; 
	 }_DDE_INFO; 
#pragma pack (pop)

#endif	// __DEF_DDE_