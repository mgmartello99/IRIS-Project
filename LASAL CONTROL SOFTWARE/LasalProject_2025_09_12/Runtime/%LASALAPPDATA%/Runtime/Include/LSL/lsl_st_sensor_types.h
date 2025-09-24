/////////////////////////////////////////////////////////////////////
//
// filename: lsl_st_sensor_types.h
// 
// category: osinterface for sysinfo
//
// about file: read system information
//
// department: ...
//
// contact person:
//
// created: 
//
// history:
//    v1.0 obejoh  initiator
//
/////////////////////////////////////////////////////////////////////

#ifndef  __LSL_SENSOR_TYPES_H
#pragma once
#define  __LSL_SENSOR_TYPES_H


#ifdef cCompile
#define MAX_SENSORS_NAME 32 ///< max. namelength of a sensor element name (NICHT Verändern, weil APPLI darauf zugreift.)

#pragma pack(push,1)

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!
//! Diese 2 Strukturen nicht verändern, sondern höchstes erweitern.
//!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!
typedef struct {
	uint32_t cbsize;        ///< version fo this struct
	unsigned int type;      ///< ESensorValueType...
	int retcode;            ///< value read return code
	union {
		int32_t i32Value;   ///< placeholder for future
		uint32_t ui32Value; ///< placeholder for future
		int64_t i64Value;   ///< placeholder for future
		uint64_t ui64Value; ///< placeholder for future
		int nValue2[2];     ///< placeholder for future
		int nValue3[3];     ///< placeholder for future
		int nValue4[4];     ///< placeholder for future
		int nValue8[8];     ///< placeholder for future
	};
	// cbsize here : 44 Byte , first version
} sensor_value_t;

typedef struct {
	uint32_t cbsize;             ///< version fo this struct
	char name[MAX_SENSORS_NAME]; ///< name of the sensor
	uint32_t type;               ///< enum ESensorType...
	uint32_t subtype;            ///< enum ESensorSubType...
	uint32_t index;              ///< Index of element
	uint32_t unit;               ///< ESensorUnit...
	int unitfact;                ///< ESensorUnitFact...
	// cbsize here : 56 Byte , first version
	uint32_t flags;              ///< some spezial / optional flags
	// cbsize here : 60 Byte
} sensor_value_info_t;

#pragma pack(pop)

///< ESensorValueType...
enum {
	ESensorValueTypeInt32  = 0x00000000,
	ESensorValueTypeUInt32,
	ESensorValueTypeInt64,
	ESensorValueTypeUInt64,
	// ESensorValueType3Int32, // 3x int
	// Any for Filter
	ESensorValueTypeAny = 0xFFFF0001,
};

///< ESensorType...
enum {
	ESensorTypeTemperature = 0x00000000,
	ESensorTypeFan         = 0x00000001,
	ESensorTypeVoltage     = 0x00000002,
	// Any for Filter
	ESensorTypeAny = 0xFFFF0002,
};

///< ESensorSubType...
enum {
	// ESensorTypeTemperature
	ESensorSubTypeTemperatureCpu = 0,      ///< CPU internal temperature sensor
	ESensorSubTypeTemperatureBoard,        ///< sensor somewhere in the system board
	ESensorSubTypeTemperatureCpuPackage,   ///< Intel has some internal cpu-package sensors
	ESensorSubTypeTemperatureFpgaExternal, ///< fpga temperature sensors connected to external sensor
	ESensorSubTypeTemperatureFpgaInternal, ///< fpga temperature sensors in the chip (ADC)
	ESensorSubTypeTemperatureAcpiTZ,       ///<
	ESensorSubTypeTemperatureSocDTS,       ///< Intel SoC DTS
	ESensorSubTypeTemperatureStek,         ///< Sigmatek LPC or other

	// ESensorTypeFan
	ESensorSubTypeFanCpu = 0, ///< fan on the cpu
	ESensorSubTypeFanBoard,   ///< fan somewhere on the board
	ESensorSubTypeFanChassis, ///< fan on the chassis (external)
	ESensorSubTypeFanStek,    ///< fan external (sigmatek device (usb, lpc, ...))

	// ESensorTypeVoltage
	ESensorSubTypeVoltageVCC = 0,
	ESensorSubTypeVoltageOther,

	ESensorSubTypeAny = 0xFFFF0003,
};

enum {
	ESensorIndexAny = 0xFFFF0004,
};

///< ESensorUnit...
enum {
   ESensorUnitCelsius = 0, ///<
   ESensorUnitVoltage,     ///<
   ESensorUnitCurrent,     ///<
   ESensorUnitPower,       ///<
   ESensorUnitRPM,         ///<
   ESensorUnitPWM,         ///<
   EStekSensorUnitPercent,     ///< range 0..100
   EStekSensorUnitPromille,    ///< range 0..1000

   ESensorUnitAny = 0xFFFF0005,
};

///< ESensorUnitFact...
enum {
	ESensorUnitFactNano = -3,  ///<
	ESensorUnitFactMicro = -2, ///<
	ESensorUnitFactMilli = -1, ///<
	ESensorUnitFactDef = 0,    ///<
	ESensorUnitFactKilo = 1,   ///<
	ESensorUnitFactMega = 2,   ///<
	ESensorUnitFactGiga = 3,   ///<
	ESensorUnitFactTera = 4,   ///<
};

#else

#define MAX_SENSORS_NAME 32 ///< max. namelength of a sensor element name (NICHT Verändern, weil APPLI darauf zugreift.)

#pragma pack(push,1)

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!
//! Diese 2 Strukturen nicht verändern, sondern höchstes erweitern.
//!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!
TYPE
	#pragma pack (push, 1)
	LSL_SENSOR_VALUE_TYPE : STRUCT
		// version 0x00010000
		cbsize	: UDINT;
		valtype	: UDINT;
		retcode : DINT;
		i32Value : DINT;
	END_STRUCT;
	#pragma pack (pop)
END_TYPE

TYPE
	#pragma pack (push, 1)
	LSL_SENSOR_VALUE_INFO_TYPE : STRUCT
		cbsize	: UDINT;
		vi_name :  ARRAY [0..(MAX_SENSORS_NAME-1)] OF CHAR;
		vi_type	: UDINT;
		vi_subtype : UDINT;
		vi_index : UDINT;
		vi_unit : UDINT;
		vi_unitfact : DINT;
		// cbsize here : 56 Byte , first version
		vi_flags : UDINT;              ///< some spezial / optional flags
		// cbsize here : 60 Byte
	END_STRUCT;
	#pragma pack (pop)
END_TYPE

#pragma pack(pop)

///< ESensorValueType...
TYPE
  ESensorValueType :
  (
	ESensorValueTypeInt32  := 0x00000000,
	ESensorValueTypeUInt32,
	ESensorValueTypeInt64,
	ESensorValueTypeUInt64,
	// ESensorValueType3Int32, // 3x int
	// Any for Filter
	ESensorValueTypeAny := 0xFFFF0001
  )$UDINT;
END_TYPE

///< ESensorType...
TYPE
  ESensorType :
  (
	ESensorTypeTemperature := 0x00000000,
	ESensorTypeFan         := 0x00000001,
	ESensorTypeVoltage     := 0x00000002,
	// Any for Filter
	ESensorTypeAny := 0xFFFF0002
  )$UDINT;
END_TYPE

///< ESensorSubType...
TYPE
  ESensorSubType :
  (
	// ESensorTypeTemperature
	ESensorSubTypeTemperatureCpu := 0,      ///< CPU internal temperature sensor
	ESensorSubTypeTemperatureBoard,        ///< sensor somewhere in the system board
	ESensorSubTypeTemperatureCpuPackage,   ///< Intel has some internal cpu-package sensors
	ESensorSubTypeTemperatureFpgaExternal, ///< fpga temperature sensors connected to external sensor
	ESensorSubTypeTemperatureFpgaInternal, ///< fpga temperature sensors in the chip (ADC)
	ESensorSubTypeTemperatureAcpiTZ,       ///<
	ESensorSubTypeTemperatureSocDTS,       ///< Intel SoC DTS
	ESensorSubTypeTemperatureStek,         ///< Sigmatek LPC or other

	// ESensorTypeFan
	ESensorSubTypeFanCpu := 0, ///< fan on the cpu
	ESensorSubTypeFanBoard,   ///< fan somewhere on the board
	ESensorSubTypeFanChassis, ///< fan on the chassis (external)
	ESensorSubTypeFanStek,    ///< fan external (sigmatek device (usb, lpc, ...))

	// ESensorTypeVoltage
	ESensorSubTypeVoltageVCC := 0,
	ESensorSubTypeVoltageOther,

	ESensorSubTypeAny := 0xFFFF0003
  )$UDINT;
END_TYPE

TYPE
  ESensorIndex :
  (
	ESensorIndexAny := 0xFFFF0004
  )$UDINT;
END_TYPE

///< ESensorUnit...
TYPE
  ESensorUnit :
  (
   ESensorUnitCelsius := 0, ///<
   ESensorUnitVoltage,     ///<
   ESensorUnitCurrent,     ///<
   ESensorUnitPower,       ///<
   ESensorUnitRPM,         ///<
   ESensorUnitPWM,         ///<
   EStekSensorUnitPercent,     ///< range 0..100
   EStekSensorUnitPromille,    ///< range 0..1000

   ESensorUnitAny := 0xFFFF0005
  )$UDINT;
END_TYPE

///< ESensorUnitFact...
TYPE
  ESensorUnitFact :
  (
	ESensorUnitFactNano := -3,  ///<
	ESensorUnitFactMicro := -2, ///<
	ESensorUnitFactMilli := -1, ///<
	ESensorUnitFactDef := 0,    ///<
	ESensorUnitFactKilo := 1,   ///<
	ESensorUnitFactMega := 2,   ///<
	ESensorUnitFactGiga := 3,   ///<
	ESensorUnitFactTera := 4    ///<
  )$DINT;
END_TYPE

#endif

#endif
