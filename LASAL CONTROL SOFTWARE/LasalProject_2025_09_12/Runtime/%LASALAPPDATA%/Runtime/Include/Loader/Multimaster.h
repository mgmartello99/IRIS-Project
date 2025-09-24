#pragma once

#define __MMSTATE_SERVER_BEGIN_  16
TYPE
 	#pragma pack (push,1) 
    // status für den Callback
	  _MMState :
	  (
	    Offline                       := 0,  // Station offline
	    Online_Initializing           := 1,  // Station online, Server noch nicht geholt
		  Online                        := 2,  // Station online, alle Server vorhanden und initialisiert
		  Online_NotAllServerAvailable  := 3,  // Station online, nicht alle Server vorhanden, vorhandene initialisiert
      SvrNotFoundAtStation          := __MMSTATE_SERVER_BEGIN_   // keine gültige lasalid für den Server gefunden
	  )$UDINT;

		#pragma pack (pop)
END_TYPE