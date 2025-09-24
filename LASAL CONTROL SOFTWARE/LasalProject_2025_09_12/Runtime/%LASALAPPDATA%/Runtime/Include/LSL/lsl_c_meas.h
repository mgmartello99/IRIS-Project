// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Version  Date      Author    Comment
//   1.0      12112002  pr        initial version
// ------------------------------------------------------------

#ifndef __LSL_IF_MEAS_H
#pragma once
#define __LSL_IF_MEAS_H

#define LSLAPI_MEAS_VERSION   0x0001

#pragma pack (push, 1)

typedef struct
{
  unsigned short record_length; 
  unsigned char record_type;
} MEAS_REC_GENERIC;

typedef struct
{
  unsigned short record_length; 
  unsigned char record_type;
  unsigned short prio;
  unsigned long cpubusytime;
  unsigned long cpureadytime;
  char TaskName[1];
} MEAS_REC_0;

typedef struct
{
  unsigned short record_length; 
  unsigned char record_type;
  unsigned long c_heap_usage;
  char TaskName[1];
} MEAS_REC_1;

typedef struct
{
  unsigned long meastime;
  unsigned short numentries;
  MEAS_REC_GENERIC meas_rec_gen[1];
} MEAS_LIST;

typedef struct
{
  unsigned long version;
  // Retval: 0=ok, <>0=Error
  long (*StartMeasurement)(unsigned long meas_duration_ms);
  long (*StopMeasurement)(void);
  long (*MeasdataAvailable)(void);
  long (*GetMeasdata)(char *Buffer, unsigned BufferLen);
}LSLAPI_MEAS;
#pragma pack (pop)


#endif // __LSL_IF_MEAS_H
