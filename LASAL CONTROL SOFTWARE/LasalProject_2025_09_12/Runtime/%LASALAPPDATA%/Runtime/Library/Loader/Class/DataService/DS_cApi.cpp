// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 20.09.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "DS_cApi.h"
#include "DS_Main.h"
#include "DS_Lib.h"


typedef void   (*SpyPCB)(uint32, DsComData*, uint32, uint32);
typedef void   (*LevelWdPCB)(uint32, uint32);
typedef void   (*calbkfpt)(DsComData*);
typedef void   (*HookReceiveEx)(cComPort*, DsComData*);
typedef uint32 (*HookUnReg)(cComPort*, uint32);


cApi Api;
static calbkfpt      pCB_RecieveData = NULL;
static HookReceiveEx pCB_RecieveDataEx = NULL; // erst ab DATASERVICE_VERSION 3
static HookUnReg     pCB_UnRegClient = NULL; // erst ab DATASERVICE_VERSION 3
static SpyPCB        pCB_SpyData = NULL;
static LevelWdPCB    pCB_LevelWatchDog = NULL;

uint32 LDR_DS_Register_pCB_LevelWatchDog(void *pfkt)
{
  if(pCB_LevelWatchDog == NULL)
  {
    pCB_LevelWatchDog = (LevelWdPCB)pfkt;
    return 1;
  }
  
  if(pfkt == NULL)
  {
    pCB_LevelWatchDog = NULL;
    return 1;
  }
  
  return 0;
}

uint32 LDR_DS_Register_pCB_SpyData(void *pfkt)
{
  if(pCB_SpyData == NULL)
  {
    pCB_SpyData = (SpyPCB)pfkt;
    return 1;
  }
  
  if(pfkt == NULL)
  {
    pCB_SpyData = NULL;
    return 1;
  }
  
  return 0;
}

uint32 LDR_DS_Register_pCB_RecieveData(void *pfkt)
{
  if(pCB_RecieveData == NULL)
  {
    pCB_RecieveData = (calbkfpt)pfkt;
    return 1;
  }
  return 0;
}

uint32 LDR_DS_Register_pCB_RecieveDataEx(void *pfkt) // erst ab DATASERVICE_VERSION 3
{
  if(pCB_RecieveDataEx == NULL)
  {
    pCB_RecieveDataEx = (HookReceiveEx)pfkt;
    return 1;
  }
  
  if(pfkt == NULL)
  {
    pCB_RecieveDataEx = NULL;
    return 1;
  }
  
  return 0;
}

uint32 LDR_DS_Register_pCB_UnRegClient(void *pfkt) // erst ab DATASERVICE_VERSION 3
{
  if(pCB_UnRegClient == NULL)
  {
    pCB_UnRegClient = (HookUnReg)pfkt;
    return 1;
  }

  if(pfkt == NULL)
  {
    pCB_UnRegClient = NULL;
    return 1;
  }

  return 0;
}

uint32 LDR_DS_Register(uint32 id, void *ptr) // erst ab DATASERVICE_VERSION 3
{
  switch(id)
  {
    case 0: return LDR_DS_Register_pCB_LevelWatchDog(ptr);
    case 1: return LDR_DS_Register_pCB_SpyData(ptr);
    case 2: return LDR_DS_Register_pCB_RecieveData(ptr);
    case 3: return LDR_DS_Register_pCB_RecieveDataEx(ptr);
    case 4: return LDR_DS_Register_pCB_UnRegClient(ptr);
    // erweitern...
    case 998: return dslib_get_sysinfo(); // Systemabfrage ob x86, lars, linux, salamander, gecko
    case 999: return (unsigned long)LDR_DS_GetUserText((cComPort*)ptr); // hack, um an diese Daten zu kommen
  }
  
  return 0;
}

uint32 Api_CTor(void)
{
  Api.Init();
  return 1;
}

void cApi::Init(void)
{
}

bool cApi::RegisterClient(cComPort *port)
{
  if(pCB_UnRegClient != NULL)
  {
    return (pCB_UnRegClient(port, 1) != 0)? true : false;
  }

  return true;
}

void cApi::UnRegisterClient(cComPort *port)
{
  if(pCB_UnRegClient != NULL)
  {
    pCB_UnRegClient(port, 0);
  }
}

void cApi::RecieveData(cComPort *port, DsComData *pcd)
{
  if(pCB_RecieveData != NULL)
  {
    pCB_RecieveData(pcd);
  }
  if(pCB_RecieveDataEx != NULL)
  {
    pCB_RecieveDataEx(port, pcd);
  }
}

//bool cApi::SendData(DsComData *pcd)
//{
//  return LDR_DS_SendEx(NULL, pcd);
//}
//
//bool cApi::SendDataEx(cComPort *port, DsComData *pcd)
//{
//  return LDR_DS_SendEx(port, pcd);
//}

void cApi::SpyData(cComPort *port, DsComData *pcd, uint32 in_out, uint32 stack_level)
{
  if(pCB_SpyData != NULL)
  {
    if(((pcd->id_minor == ComCmd_SignIn) || (pcd->id_minor == ComCmd_SignInEx)) && (pcd->id_major == ComId_Major_DS))
    {
      uint32 clientid = *(uint32*)&pcd->data[4];
      pCB_SpyData(clientid, pcd, in_out, stack_level);
      return;
    }
  
    pCB_SpyData(port->GetClientId(), pcd, in_out, stack_level);
  }
}

void cApi::LevelWatchdog(cComPort *port, uint32 level)
{
  if(pCB_LevelWatchDog != NULL)
  {
    pCB_LevelWatchDog(port->GetClientId(), level);
  }
}

uint32 cApi::SendDataWithResponse(DsComData *pcd, uint32 timeout)
{
  return LDR_DS_SendWithResponse(pcd, timeout);
}

