// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Version  Date      Author    Comment
//   1.0      23092005  pr        initial version
// ------------------------------------------------------------

#ifndef __LSL_IF_SYSTRACE_H
#pragma once
#define __LSL_IF_SYSTRACE_H

#define LSLAPI_SYSTRACE_VERSION   1
//
// Trace-Record type-groups
// 
#define TRECGRP_LSE0        0x0080
#define TRECGRP_LSE1        0x0100
#define TRECGRP_USER0       0x0200
#define TRECGRP_USER1       0x0400

#pragma pack (push, 1)
typedef struct
{
  unsigned long version;
  unsigned long size;
  void (*MakeTraceEntry)(unsigned long recType, unsigned long key, unsigned long data);
  void (*ReportTraceEvent)(unsigned long event);
  int (*AddTraceKeyVal)(unsigned short typeGroup, unsigned long key, char *text);
  int (*SetTraceBufSize)(unsigned long size);
  void (*GetTraceBufSize)(unsigned long *pSize, unsigned long *pMaxSize);
}LSLAPI_SYSTRACE;
#pragma pack (pop)

#define OS_MakeTraceEntry(itf,type,key,data)   {if (itf) (itf)->MakeTraceEntry(type,key,data);}
#define OS_ReportTraceEvent(itf,event)         {if (itf) (itf)->ReportTraceEvent(event);}
#define OS_AddTraceKeyVal(itf,group,key,text)  {if (itf) (itf)->AddTraceKeyVal(group,key,text);}
#define OS_SetTraceBufSize(itf,size)           {if (itf) (itf)->SetTraceBufSize(size);}
#define OS_GetTraceBufSize(itf,pSize,pMaxSize) {if (itf) (itf)->GetTraceBufSize(pSize,pMaxSize);}

#endif // __LSL_IF_SYSTRACE_H

