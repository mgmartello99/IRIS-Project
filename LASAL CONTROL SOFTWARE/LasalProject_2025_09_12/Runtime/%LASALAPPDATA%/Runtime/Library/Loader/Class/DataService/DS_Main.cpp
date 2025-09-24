//<NewSigmatekCFileOptimize/>
// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 28.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "DS_Main.h"
#include "DS_cComPort.h"
#include "DS_Lib.h"
#include "DS_cApi.h"

static cComPort *ComPortList[16];
static uint32   ComPortListUsed = 0;
static void     *ComPortListMux = NULL;
static uint32   ComPortListStep = 0;
static int32    ComPortListSocket = -1;
static int32    ComPortListTcpIpPort = 2402;

static uint32 ListSeek(cComPort *pcp)
{
  uint32 retcode = 0xFFFFFFFF;

  for(uint32 i=0; i<ARRAY_SIZE(ComPortList); i++)
  {
    if(ComPortList[i] == pcp)
    {
      retcode = i;
      break;
    }
  }
  
  return retcode;
}

static uint32 DS_ClientAdd(int32 socket)
{
  uint32 retcode = 0;
  dslib_criticalsection_start(ComPortListMux); // ************************ [START CRITICAL SECTION]
  uint32 idx = ListSeek(NULL);
  if(idx < ARRAY_SIZE(ComPortList))
  {
    cComPort *pcp = cComPort::New(socket);
    if(pcp != NULL)
    {
      ComPortList[idx] = pcp;
      if(idx >= ComPortListUsed)
      {
        ComPortListUsed = idx + 1;
      }
      retcode = 1;
    }
  }
  dslib_criticalsection_stop(ComPortListMux); // ************************** [STOP CRITICAL SECTION]
  
  return retcode;
}

uint32 LDR_DS_CountAlarmHandle(uint32 alahdl)
{
  // Vorkommen eines bestimmten AlarmHandles zählen
  
  uint32 retcode = 0;
  
  dslib_criticalsection_start(ComPortListMux); // ************************ [START CRITICAL SECTION]
    
  for(uint32 i=0; i<ARRAY_SIZE(ComPortList); i++)
  {
    cComPort *pi = ComPortList[i];
    if(pi != NULL)
    {
      if(pi->GetAlarmHandle() == alahdl)
      {
        retcode ++;
      }
    }
  }
  dslib_criticalsection_stop(ComPortListMux); // ************************** [STOP CRITICAL SECTION]
  
  return retcode;
}

static void DS_ClientSub(cComPort *pcp)
{
  uint32 used = 0;
  
  dslib_criticalsection_start(ComPortListMux); // ************************ [START CRITICAL SECTION]
  
  for(uint32 i=0; i<ARRAY_SIZE(ComPortList); i++)
  {
    if(ComPortList[i] == pcp)
    {
      ComPortList[i] = NULL; // NOTE: vorher auf NULL setzen, erst dann Delete aufrufen
      cComPort::Delete(pcp);
    }
    
    if(ComPortList[i] != NULL)
    {
      used = i+1;
    }
  }
  
  ComPortListUsed = used;  
  dslib_criticalsection_stop(ComPortListMux); // ************************** [STOP CRITICAL SECTION]
}

// #include "RTOS_C_interfaces.h"
// #define dslib_sprintf LSL_POS->piCRT->sprintf

void LDR_DS_CTor(uint32 tcpip_port)
{
  char portval[17];

  // 8D-1732
  // Bsp.: Autoexec.lsl SETENV DataServiceTcpPort 5020
  if(dslib_getenvvar("DataServiceTcpPort", portval, sizeof(portval)) != 0) 
  {
    tcpip_port = dslib_strtol(portval, NULL, 0);
  }
  
//  dslib_sprintf(portval, "LDR::Port=%d", tcpip_port);
//  dslib_system_message(portval);

  ComPortListTcpIpPort = tcpip_port;
  ComPortListSocket    = -1;
  ComPortListStep      = 0;
  ComPortListUsed      = 0;
  ComPortListMux       = dslib_criticalsection_create("DS_ComPort");
 
  Api_CTor();
  dslib_CTor();
 
  dslib_criticalsection_start(ComPortListMux); // ************************ [START CRITICAL SECTION]
  uint32 nox = ARRAY_SIZE(ComPortList);
  while(nox--)
  {
    ComPortList[nox] = NULL;
  }
  dslib_criticalsection_stop(ComPortListMux); // ************************** [STOP CRITICAL SECTION]
}

void LDR_DS_RunCy(void)
{
  if(ComPortListStep == 1)
  {
    uint32 nox = ComPortListUsed;
    while(nox--)
    {
      if(ComPortList[nox] != NULL)
      {
        if(ComPortList[nox]->Run() == false)
        {
          DS_ClientSub(ComPortList[nox]);
        }
      }
    }
  }
}


void LDR_DS_RunBk(void)
{
  if(ComPortListStep == 0)
  {
    ComPortListStep   = 99;
    ComPortListSocket = dslib_tcp_create_socket();
    
    if(ComPortListSocket >= 0)
    {
      if(dslib_tcp_user_listen(ComPortListSocket, ComPortListTcpIpPort, 5) == 0)
      {
        ComPortListStep = 1;
      }
    }
  }
  
  if(ComPortListStep == 1)
  {
    int32 sock = dslib_tcp_user_accept(ComPortListSocket, 0);
    if(sock >= 0)
    {
      if(DS_ClientAdd(sock) == 0)
      {
        dslib_system_message("DS-ERROR: Can't Add Client");
      }
    }
  }
}

uint32 LDR_DS_SendEx(cComPort *port, DsComData *pcd)
{   
  uint32 retcode = 1;
  
  dslib_criticalsection_start(ComPortListMux); // ************************ [START CRITICAL SECTION]
  uint32 nox = ComPortListUsed;
  
  if(port == NULL)
  {
    while(nox--)
    {
      if(ComPortList[nox] != NULL)
      {
        if(ComPortList[nox]->Send(pcd) == false)
        {
          retcode = 0;
        }
      }
    }
  }
  else
  {
    retcode = 0;
    
    while(nox--)
    {
      if(ComPortList[nox] == port)
      {
        retcode = (port->Send(pcd) == true)? 1 : 0;
        nox = 0;
      }
    }
  }
  
  dslib_criticalsection_stop(ComPortListMux); // ************************** [STOP CRITICAL SECTION]
  
  return retcode;
}

char *LDR_DS_GetUserText(cComPort *port)
{
  char *retcode = NULL;
  
  if(port != NULL)
  {
    dslib_criticalsection_start(ComPortListMux); // ************************ [START CRITICAL SECTION]
    uint32 nox = ComPortListUsed;
  
    while(nox--)
    {
      if(ComPortList[nox] == port)
      {
        retcode = port->GetUserText();
      }
    }
  
    dslib_criticalsection_stop(ComPortListMux); // ************************** [STOP CRITICAL SECTION]
  }
  
  return retcode;
}

uint32 LDR_DS_GetClientId(cComPort *port)
{
  uint32 retcode = 0;

  if(port != NULL)
  {
    dslib_criticalsection_start(ComPortListMux); // ************************ [START CRITICAL SECTION]
    uint32 nox = ComPortListUsed;
  
    while(nox--)
    {
      if(ComPortList[nox] == port)
      {
        retcode = port->GetClientId();
        nox = 0;
      }
    }
  
    dslib_criticalsection_stop(ComPortListMux); // ************************** [STOP CRITICAL SECTION]
  }
  
  return retcode;
}

uint32 LDR_DS_Send(DsComData *pcd)
{   
  return LDR_DS_SendEx(NULL, pcd);
}

uint32 LDR_DS_SendWithResponse(DsComData *pcd, uint32 timeout)
{
  dslib_system_message("DS-ERROR: LDR_DS_SendWithResponse is currently not supported!");
  return 0;
}

DsComData *LDR_DS_ComBufferCreate(DsComData *pstat, uint32 expected_size)
{
  return (DsComData*)dslib_buffer_create(pstat, sizeof(DsComData), expected_size);
}

DsComData *LDR_DS_ComBufferDestroy(DsComData *pstat, DsComData *psendbuffer)
{
  return (DsComData*)dslib_buffer_destroy(pstat, psendbuffer);
}


