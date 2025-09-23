// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 28.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_MainH
  #define _DS_MainH
  
  #include "DS_Deeds.h"
  #include "DS_ComDef.h"
  #include "DS_cComPort.h"
  
  #ifdef cCompile
 
    cExtern DsComData *LDR_DS_ComBufferCreate(DsComData *statbuff, uint32 expected_size);
    cExtern DsComData *LDR_DS_ComBufferDestroy(DsComData *statbuff, DsComData *buffer);
    
    cExtern void       LDR_DS_CTor(uint32 tcpip_port);
    cExtern void       LDR_DS_RunCy(void);
    cExtern void       LDR_DS_RunBk(void);

    cExtern uint32     LDR_DS_Send(DsComData *pcd);
    cExtern uint32     LDR_DS_SendWithResponse(DsComData *pcd, uint32 timeout);
    cExtern uint32     LDR_DS_Register_pCB_RecieveData(void *pfkt);
    cExtern uint32     LDR_DS_Register_pCB_SpyData(void *pfkt);
    cExtern uint32     LDR_DS_Register_pCB_LevelWatchDog(void *pfkt);
    
    // erst ab DATASERVICE_VERSION 3
    cExtern uint32     LDR_DS_Register_pCB_UnRegClient(void *pfkt);
    cExtern uint32     LDR_DS_Register_pCB_RecieveDataEx(void *pfkt);
    cExtern uint32     LDR_DS_SendEx(cComPort *port, DsComData *pcd);
    cExtern uint32     LDR_DS_GetClientId(cComPort *port);
    cExtern uint32     LDR_DS_Register(uint32 id, void *ptr);
            uint32     LDR_DS_CountAlarmHandle(uint32 alahdl);

    // erst ab DATASERVICE_VERSION 4
    cExtern char      *LDR_DS_GetUserText(cComPort *port);


  #else
    function global __cdecl LDR_DS_ComBufferCreate var_input statbuff:^DsComData; expected_size:udint; end_var var_output retcode:^DsComData; end_var;
    function global __cdecl LDR_DS_ComBufferDestroy var_input statbuff:^DsComData; buffer:^DsComData; end_var var_output retcode:^DsComData; end_var;   
  
    function global __cdecl LDR_DS_CTor var_input tcpip_port:udint; end_var;
    function global __cdecl LDR_DS_RunCy;
    function global __cdecl LDR_DS_RunBk;
    
    function global __cdecl LDR_DS_Send var_input pcd:^DsComData; end_var var_output retcode:udint; end_var;
    function global __cdecl LDR_DS_SendWithResponse var_input pcd:^DsComData; timeout:udint; end_var var_output retcode:udint; end_var;
  
    function global __cdecl LDR_DS_Register_pCB_RecieveData var_input pfkt:^void; end_var var_output retcode:udint; end_var;
    function global __cdecl LDR_DS_Register_pCB_SpyData var_input pfkt:^void; end_var var_output retcode:udint; end_var;
    function global __cdecl LDR_DS_Register_pCB_LevelWatchDog var_input pfkt:^void; end_var var_output retcode:udint; end_var;

    // erst ab DATASERVICE_VERSION 3
    function global __cdecl LDR_DS_Register_pCB_UnRegClient var_input pfkt:^void; end_var var_output retcode:udint; end_var;
    function global __cdecl LDR_DS_Register_pCB_RecieveDataEx var_input pfkt:^void; end_var var_output retcode:udint; end_var;
    function global __cdecl LDR_DS_SendEx var_input port:^void; pcd:^DsComData; end_var var_output retcode:udint; end_var;
    function global __cdecl LDR_DS_GetClientId var_input port:^void; end_var var_output retcode:udint; end_var;
    function global __cdecl LDR_DS_Register var_input id:udint; ptr:^void; end_var var_output retcode:udint; end_var;

    // erst ab DATASERVICE_VERSION 4
    function global __cdecl LDR_DS_GetUserText var_input port:^void; end_var var_output retcode:^char; end_var;
    
  #endif


#endif

