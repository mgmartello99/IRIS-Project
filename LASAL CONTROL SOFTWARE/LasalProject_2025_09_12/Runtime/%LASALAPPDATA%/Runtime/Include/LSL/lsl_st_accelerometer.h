#ifndef  __LSL_ACCELEROMETER_H
#pragma once
#define  __LSL_ACCELEROMETER_H

// reserved return value - this error value will be returned from every method of the Accelerometer
// class, if no Accelerometer interface is available.
// Starting the application without the Accelerometer is possible
#define ACC_ERR_NO_INTERFACE  -1015   // error, no Accelerometer Interface available

(* Modes that can be enabled *)
#define LSL_ACC_MODE_DATA   16#01
#define LSL_ACC_MODE_FFMO	  16#04
#define LSL_ACC_MODE_TAP	  16#08
#define LSL_ACC_MODE_ORIENT	16#10
#define LSL_ACC_MODE_TRANS	16#20

(* Possible states of the driver *)
#define LSL_ACC_STAT_STOPPED      0x00
#define LSL_ACC_STAT_CONFIGURING	0x01
#define LSL_ACC_STAT_RUNNING		  0x02

(* Defines for Configuring the Accelerometer *)
#define LSL_ACC_FFMO_CONF_X			0x08	// Detect events in X
#define LSL_ACC_FFMO_CONF_Y			0x10	// Detect events in Y
#define LSL_ACC_FFMO_CONF_Z			0x20	// Detect events in Z
#define LSL_ACC_FFMO_CONF_COMB		0x40	// If Bit is set, Axes have OR combination, otherwise AND (AND for freefall detection)
#define LSL_ACC_FFMO_CONF_LATCH		0x80	// Latch events until they're read

#define LSL_ACC_TRANS_CONF_HPF		0x01	// Bypass High Pass Filter (similar to motion detection)
#define LSL_ACC_TRANS_CONF_X		0x02	// Detect events in X
#define LSL_ACC_TRANS_CONF_Y		0x04	// Detect events in Y
#define LSL_ACC_TRANS_CONF_Z		0x08	// Detect events in Z

#define LSL_ACC_TAP_CONF_SPX		0x01	// Detect single pulse on X
#define LSL_ACC_TAP_CONF_DPX		0x02	// Detect double pulse on X
#define LSL_ACC_TAP_CONF_SPY		0x04	// Detect single pulse on Y
#define LSL_ACC_TAP_CONF_DPY		0x08	// Detect double pulse on Y
#define LSL_ACC_TAP_CONF_SPZ		0x10	// Detect single pulse on Z
#define LSL_ACC_TAP_CONF_DPZ		0x20	// Detect double pulse on Z


(* Possible states of the Accelerometer *)
#define LSL_ACC_ORSTAT_PU			0
#define LSL_ACC_ORSTAT_PD			2
#define LSL_ACC_ORSTAT_LR			4
#define LSL_ACC_ORSTAT_LL			6
#define LSL_ACC_ORSTAT_BACK		1

#define LSL_ACC_FFMOSRC_XPOL		0x01  // If Bit is set, the X Motion event was negative
#define LSL_ACC_FFMOSRC_XMOT		0x02  // If Bit is set, X Motion event was detected
#define LSL_ACC_FFMOSRC_YPOL		0x04  // If Bit is set, the Y Motion event was negative
#define LSL_ACC_FFMOSRC_YMOT		0x08  // If Bit is set, Y Motion event was detected
#define LSL_ACC_FFMOSRC_ZPOL		0x10  // If Bit is set, the Z Motion event was negative
#define LSL_ACC_FFMOSRC_ZMOT		0x20  // If Bit is set, Z Motion event was detected
#define LSL_ACC_FFMO_EVACT			0x80  // If Bit is set, An event occured (Depending on LSL_ACC_FFMO_CONF_COMB)

#define LSL_ACC_TRANSSRC_XPOL		0x01  // If Bit is set, the X Motion event was negative
#define LSL_ACC_TRANSSRC_XMOT		0x02  // If Bit is set, X Motion event was detected
#define LSL_ACC_TRANSSRC_YPOL		0x04  // If Bit is set, the Y Motion event was negative
#define LSL_ACC_TRANSSRC_YMOT		0x08  // If Bit is set, Y Motion event was detected
#define LSL_ACC_TRANSSRC_ZPOL		0x10  // If Bit is set, the Z Motion event was negative
#define LSL_ACC_TRANSSRC_ZMOT		0x20  // If Bit is set, Z Motion event was detected
#define LSL_ACC_TRANS_EVACT			0x40  // If Bit is set, An event occured

#define LSL_ACC_TAPSRC_XPOL			0x01  // If Bit is set, the X Pulse event was negative
#define LSL_ACC_TAPSRC_YPOL			0x02  // If Bit is set, the Y Pulse event was negative
#define LSL_ACC_TAPSRC_ZPOL			0x04  // If Bit is set, the Z Pulse event was negative
#define LSL_ACC_TAPSRC_DOUBLE		0x08  // If Bit is set, a double Pulse triggered event
#define LSL_ACC_TAPSRC_XTAP			0x10  // If Bit is set, a Tap event in X was detected
#define LSL_ACC_TAPSRC_YTAP			0x20  // If Bit is set, a Tap event in X was detected
#define LSL_ACC_TAPSRC_ZTAP			0x40  // If Bit is set, a Tap event in X was detected
#define LSL_ACC_TAP_EVACT			0x80  // If Bit is set, an event occured

TYPE
	LSL_ACC_STATUS_TYPE	:	STRUCT
    status : UDINT;
		timestamp	:	UDINT;
		x	:	DINT;
		y	:	DINT;
		z	:	DINT;
    statOrient : UDINT;
    statFFMO : UDINT;
    statTrans: UDINT;
    statTap : UDINT;
	END_STRUCT;
END_TYPE

TYPE
	LSL_ACCELEROMETER_TYPE	:	STRUCT
	    version : UDINT;
	    Acc_FF_Init : pVoid;
	    Acc_FF_SetThreshold : pVoid;
	    Acc_FF_SetDebounce : pVoid;
      
      (* since Version 0x00010001 
          If the new Functions are used, Acc_FF_Init, Acc_FF_SetThreshold and Acc_FF_SetDebounce must not be used anymore *)
	  Acc_Control : pVoid;
      Acc_ConfigureGeneral : pVoid;
      Acc_GetStatus : pVoid;
      Acc_ConfigureOrient : pVoid;
      Acc_ConfigureFFMO : pVoid;
      Acc_ConfigureTransient : pVoid;
      Acc_ConfigureTap : pVoid;
	END_STRUCT;
END_TYPE

////////////////////////////////////////////////////////////////////////////////
// LASAL Function Prototypes
////////////////////////////////////////////////////////////////////////////////
FUNCTION __CDECL GLOBAL P_Acc_Control
	VAR_INPUT
		enable 	: UDINT;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_Acc_ConfigureGeneral
	VAR_INPUT
		pCallback 	: pVoid;
		pCookie 	: pVoid;
		range 	: UDINT;
		rtMode 	: UDINT;
		drPeriod 	: UDINT;
		flags 	: UDINT;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_Acc_GetStatus
	VAR_INPUT
		pStatus 	: ^LSL_ACC_STATUS_TYPE;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_Acc_ConfigureOrient
	VAR_INPUT
		debounceCnt 	: UDINT;
		flags 	: UDINT;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_Acc_ConfigureFFMO
	VAR_INPUT
		eventCfg 	: UDINT;
		debounceCnt 	: UDINT;
		threshold 	: UDINT;
		flags 	: UDINT;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_Acc_ConfigureTransient
	VAR_INPUT
		eventCfg 	: UDINT;
		debounceCnt 	: UDINT;
		threshold 	: UDINT;
		flags 	: UDINT;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_Acc_ConfigureTap
	VAR_INPUT
		eventCfg 	: UDINT;
		thresholdX 	: UDINT;
		thresholdY 	: UDINT;
		thresholdZ 	: UDINT;
		pulseTime 	: UDINT;
		pulseLatency 	: UDINT;
		pulseWindow 	: UDINT;
		flags 	: UDINT;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;
	
////////////////////////////////////////////////////////////////////////////////
// LASAL MACROS
////////////////////////////////////////////////////////////////////////////////
#define OS_ACC_CONTROL(p1) ACC_API^.Acc_Control $ P_Acc_Control(p1)
#define OS_ACC_CONFIGGEN(p1, p2, p3, p4, p5, p6) ACC_API^.Acc_ConfigureGeneral $ P_Acc_ConfigureGeneral(p1, p2, p3, p4, p5, p6)
#define OS_ACC_GETSTATUS(p1) ACC_API^.Acc_GetStatus $ P_Acc_GetStatus(p1)
#define OS_ACC_CONFIGORIENT(p1, p2) ACC_API^.Acc_ConfigureOrient $ P_Acc_ConfigureOrient(p1, p2)
#define OS_ACC_CONFIGFFMO(p1, p2, p3, p4) ACC_API^.Acc_ConfigureFFMO $ P_Acc_ConfigureFFMO(p1, p2, p3, p4)
#define OS_ACC_CONFIGTRANS(p1, p2, p3, p4) ACC_API^.Acc_ConfigureTransient $ P_Acc_ConfigureTransient(p1, p2, p3, p4)
#define OS_ACC_CONFIGTAP(p1, p2, p3, p4, p5, p6, p7, p8) ACC_API^.Acc_ConfigureTap $ P_Acc_ConfigureTap(p1, p2, p3, p4, p5, p6, p7, p8)


#endif