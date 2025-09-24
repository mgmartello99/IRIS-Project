#ifndef LSL_LED
#define LSL_LED

//interface
#define INTERFACE_LED	"LED"

FUNCTION GLOBAL __cdecl LedSet
VAR_INPUT
  led   : UDINT;
  state : UDINT;
END_VAR
VAR_OUTPUT
    ret : DINT;
END_VAR;

FUNCTION GLOBAL __cdecl LedGet
VAR_INPUT
  led     : UDINT;
  pState  : ^UDINT;
END_VAR
VAR_OUTPUT
    ret : DINT;
END_VAR;

TYPE
  LSL_LED_TYPE   :STRUCT
    version : UDINT;
    LedSet  : ^LedSet;
    LedGet  : ^LedGet;
  END_STRUCT;
END_TYPE

#define LED_RUN			1
#define LED_DCOK		2
#define LED_STATUS		3
#define LED_ERROR		4

#define LED_OFF			0
#define LED_ON			1
#define LED_FLASH		2
#define LED_FLASH_FAST	3
#define LED_REFRESH		4

#define LED_INVALID_PARAMETER	-1
#define LED_INVALID_HARDWARE	-2

#define OS_LED_SET(pLed,p1,p2)     pLed^.LedSet $ LedSet(p1,p2)
#define OS_LED_GET(pLed,p1,p2)     pLed^.LedGet $ LedGet(p1,p2)
#endif
