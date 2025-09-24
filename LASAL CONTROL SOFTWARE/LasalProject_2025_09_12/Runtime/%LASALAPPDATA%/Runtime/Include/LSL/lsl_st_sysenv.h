// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//   Version  Date      Author    Comment
//   1.0      04102007  pr        initial version
// ------------------------------------------------------------

#ifndef __LSL_ST_SYSENV
#pragma once
#define __LSL_ST_SYSENV

FUNCTION __CDECL GLOBAL P_SysEnv_SetEnvVar
VAR_INPUT
  name : ^CHAR;
  value : ^CHAR;
END_VAR
VAR_OUTPUT
  retval : DINT;
END_VAR;

FUNCTION __CDECL GLOBAL P_SysEnv_GetEnvVar
VAR_INPUT
  name : ^CHAR;
  buffer : ^CHAR;
  size : UDINT;
END_VAR
VAR_OUTPUT
  retval : DINT;
END_VAR;

#define OS_SETENVVAR(itf,name,value)  itf^.SetEnvVar $ P_SysEnv_SetEnvVar(name,value)
#define OS_GETENVVAR(itf,name,buffer,size)  itf^.GetEnvVar $ P_SysEnv_GetEnvVar(name,buffer,size)

#pragma pack (push, 1)
TYPE
    OS_SYSENV : STRUCT
      size : UDINT;
      SetEnvVar : pVoid;
      GetEnvVar : pVoid;
    END_STRUCT;
END_TYPE
#pragma pack (pop)


#endif
