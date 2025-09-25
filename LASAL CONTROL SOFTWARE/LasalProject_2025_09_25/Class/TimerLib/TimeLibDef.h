

FUNCTION GLOBAL IsTimerFinished
VAR_INPUT
		pudStartTime 	: ^UDINT;
		udDuration 	: UDINT;
	END_VAR
	VAR_OUTPUT
		fFinished 	: BOOL;
	END_VAR;


FUNCTION GLOBAL TimerStart
	VAR_INPUT
		pudStartTime 	: ^UDINT;
	END_VAR;



FUNCTION GLOBAL TimerRest
	VAR_INPUT
		pudStartTime 	: ^UDINT;
		udDuration 	: UDINT;
	END_VAR
	VAR_OUTPUT
		udRemainingTime 	: UDINT;
	END_VAR;



FUNCTION GLOBAL TimerAct
	VAR_INPUT
		pudStartTime 	: ^UDINT;
	END_VAR
	VAR_OUTPUT
		udPassedTime 	: UDINT;
	END_VAR;

