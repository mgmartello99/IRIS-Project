// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 28.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "DS_cComPort.h"
#include "DS_cComDatBuffer.h"
#include "DS_cApi.h"
#include "DS_Lib.h"
#include "DS_cFragmentHandler.h"

#define ComId_SignInIdSigmatek (ComId_SignInIdStandard ^ 0xFACE)

#define ACCESS_DENIED  0x80000010


cComPort *cComPort::New(int32 sock)
{
  cComPort *retcode = NULL;
  retcode = (cComPort*)dslib_malloc(sizeof(cComPort));
  if(retcode != NULL)
  {
    retcode->cTor(sock);
  }
  
  return retcode;
}

cComPort *cComPort::Delete(cComPort *p)
{
  if(p != NULL)
  {
    p->dTor();
    dslib_free(p);
  }
  return NULL;
}

void cComPort::cTor(int32 sock)
{
  uint32 timex = dslib_tabsolute();
  RefreshListEx.cTor(this);
  ComAlarm.cTor(this);
  FragmentMgr.cTor();
  
  AliveTimex   = timex;
  StartTimex   = timex;
  LastSendTime = timex;
  LastSendAliveTrgTime = timex;
  Socket       = sock;
  DataSize     = 0;
  ClientId     = 0;
  MagicNumber  = 0;
  CpuState     = 0xFFFFFFFF;
  FragmentId   = 0;
  DataStackLevel = 0;
  IsRegistered = 0;
  UserText[0]  = 0;

//  pFragmentHandler = FragmentHandler::cTor();
}

void cComPort::dTor(void)
{
  if(IsRegistered != 0)
  {
    Api.UnRegisterClient(this);
    IsRegistered = 0;
  }
  
  dslib_close_socket(Socket);
  RefreshListEx.dTor();
  ComAlarm.dTor();
  FragmentMgr.dTor();
 // cTor(-1);
  //init
  Socket       = -1;
  DataSize     = 0;
  ClientId     = 0;
  MagicNumber  = 0;
  CpuState     = 0xFFFFFFFF;
  FragmentId   = 0;
  DataStackLevel = 0;
  UserText[0]  = 0;
  
//  pFragmentHandler = FragmentHandler::dTor(pFragmentHandler);
}

uint32 cComPort::GetClientId(void)
{
  return ClientId;
}

DsComData *cComPort::ComDataBufferCreate(DsComData *pstat, uint32 expected_size)
{
  return (DsComData*)dslib_buffer_create(pstat, sizeof(DsComData), expected_size);
}

DsComData *cComPort::ComDataBufferDestroy(DsComData *pstat, DsComData *psendbuffer)
{
  return (DsComData*)dslib_buffer_destroy(pstat, psendbuffer);
}

bool cComPort::Send(DsComData* pcd) 
{
  if(pcd->datalength <= MaximumFragmentSize)
  {
    if(pcd->id_major != ComId_Major_NoSend)
    { 
      Api.SpyData(this, pcd, 0, 0xFFFFFFFF); // SPY: daten an client senden
    
      uint32 len = pcd->datalength;
      if(len > 0)
      {
        uint08* buffer = (uint08*)pcd;
        uint32 delay_cnt = 0;
      
        while(len != 0)
        {
          int32 nsent = dslib_tcp_send(Socket, buffer, len);
          if(nsent >= 0)
          {
            buffer   += (uint32)nsent;
            len      -= (uint32)nsent;
            delay_cnt = 0;
          }
          else 
          {
            if(nsent != -2000)
            {
              return false;
            }
          
            if(delay_cnt < 1024)
            {
              dslib_thread_delay_us(201); // delay 201µs
              delay_cnt += 1;
            }
            else
            {
              return false;
            }
          } 
        }
   
        LastSendTime = dslib_tabsolute();
      }
      return true;
    } 
  }
  else
  {
    return SendFragmented(pcd);
  }
  
  return false;
}

bool cComPort::SendFragmented(DsComData* pcd)
{
  bool   retcode = false;
  uint08 scratch[256];
  
  DsComData* pac = (DsComData*)dslib_buffer_create(scratch, sizeof(scratch), MaximumFragmentSize);
    
  if(pac != NULL)
  {
    uint32 rest   = pcd->datalength;
    uint32 max    = MaximumFragmentSize - ComDataHeaderSize - sizeof(tFragmentHeader);
    uint64 offset = 0;
    uint08 *pdata = (uint08*)pcd;

    retcode = true;
    
    pac->id_major        = ComId_Major_DS;
    pac->id_minor        = ComCmd_SendFragment;
    pac->transactionSrc  = 0;
    pac->transactionDest = pcd->transactionDest;
    pac->messageId       = 0xFFFFFFFF;

    tFragmentHeader *pfh = (tFragmentHeader*)&pac->data[0];
    pfh->fragment_id     = dslib_atomic_incU32(&FragmentId);
    pfh->nc              = 0xFFFFFFFF;
    pfh->bytesize_gesamt = pcd->datalength;
    pfh->bytesize_actual = 0;
    pfh->offset_actual   = 0;
    
    //  0 [4] .... id
    //  4 [4] .... immer 0xFFFFFFFF ??
    //  8 [8] .... Gesamtbytelänge der Daten
    // 16 [4] .... aktuelle Bytelänge
    // 20 [8] .... Offset der aktuellen Daten

//    *(uint32*)&pac->data[0] = dslib_atomic_incU32(&FragmentId); 
//    *(uint32*)&pac->data[4] = 0xFFFFFFFF;
//    *(uint64*)&pac->data[8] = pcd->datalength;
      
    do
    {
      uint32 part = (rest > max)? max : rest;
  
      pac->datalength = ComDataHeaderSize + 28 + part;

      pfh->bytesize_actual = part;
      pfh->offset_actual   = offset;
//      *(uint32*)&pac->data[16] = part;
//      *(uint64*)&pac->data[20] = offset;
      dslib_memcpy(&pac->data[28], pdata, part);

      pdata  += part;
      offset += part;
      rest   -= part;

      if(Send(pac) == false)
      {
        rest = 0;
        retcode = false;
      }
    }
    while(rest);
    
    dslib_buffer_destroy(scratch, pac);
  }    
  
  return retcode;
}


/*
bool cComPort::SendFragmented(DsComData* pcd)
{
  #define MaximumFragmentPayload (MaximumFragmentSize - ComDataHeaderSize - 28)
  
  bool res = false; 

  int32 remSize = pcd->datalength;
  uint64 offset = 0;
  
  DsComData temp;
  DsComData* data = ComDataBufferCreate(&temp, MaximumFragmentSize);
    
  if(data != NULL)
  {
    data->datalength = MaximumFragmentSize;
    data->id_major   = ComId_Major_DS;
    data->id_minor   = ComCmd_SendFragment;
    data->transactionSrc  = 0;
    data->transactionDest = pcd->transactionDest;
    data->messageId  = 0xFFFFFFFF;

    *(uint32*)&data->data[0] = id; 
    id += 1;
    *(uint32*)&data->data[4] = 0xFFFFFFFF;
    *(uint64*)&data->data[8] = pcd->datalength;

    uint32 currSize = MaximumFragmentPayload;
    *(uint32*)&data->data[16] = currSize;
      
    while(remSize > 0)
    {
      if(remSize < MaximumFragmentPayload)
      {
        currSize = remSize; 
        *(uint32*)&data->data[16] = currSize; 
        data->datalength = remSize + 28 + ComDataHeaderSize;
      }

      *(uint64*)&data->data[20] = offset;
      
      dslib_memcpy(&data->data[28], ((uint08*)pcd)+offset, currSize);
      
      res = Send(data); 
      
      if(res == false)
      {
        break;
      }
      
      offset += MaximumFragmentPayload;
      remSize -= MaximumFragmentPayload;
    }
  }    
  
  ComDataBufferDestroy(&temp, data);
  return res;
}
*/

bool cComPort::SetSysVar(uint32 value, uint32 varno)
{
  if(varno >= ComVarNo_SysVariable)
  {
    switch(varno & 0xFFFF00FFUL)
    {
      case ComVarNo_SysCpuLoad:
      case ComVarNo_SysCpuState :
      case ComVarNo_SysLoaderVersion :
      case ComVarNo_SysDscCrc :
        return false;

      case ComVarNo_SysDate :
        return (dslib_set_sysdate(value) != 0)? true : false;
    
      case ComVarNo_SysTime :
        return (dslib_set_systime(value) != 0)? true : false;
    }
  }
  
  return false;
}

bool cComPort::GetSysVar(uint32 *pvalue, uint32 varno)
{
  if(varno >= ComVarNo_SysVariable)
  {
    switch(varno & 0xFFFF00FFUL)
    {
      case ComVarNo_SysDscCrc :
        *pvalue = dslib_get_dsccrc();
        return true;
      
      case ComVarNo_SysLoaderVersion :
        *pvalue = dslib_get_loaderversion();
        return true;

      case ComVarNo_SysDate :
        *pvalue = dslib_get_sysdate();
        return true;
    
      case ComVarNo_SysTime :
        *pvalue = dslib_get_systime();
        return true;
      
      case ComVarNo_SysCpuState :
        *pvalue = dslib_get_cpustate(); 
        return true;
        
      case ComVarNo_SysCpuLoad:
        *pvalue = dslib_get_cpuload(); 
        return true;
        
      case ComVarNo_SysOpsysVersion:
        *pvalue = dslib_get_opsysversion();
        return true;
        
      case ComVarNo_SysInfo:
        *pvalue = dslib_get_sysinfo();
        return true;
        
    }
  }
  
  return false;
}

bool cComPort::ComCmdReadFromSysVariable(uint16 transactionDest, uint32 messageId, uint32 no, uint32 *pvarno)
{
  bool retcode = false;
  
  DsComData tcomdat;
  DsComData *pcd = ComDataBufferCreate(&tcomdat, ComDataHeaderSize + no * 12 + 4);

  if(pcd != NULL)
  {
    pcd->transactionDest = transactionDest;
    pcd->transactionSrc = 0;
    pcd->id_major   = ComId_Major_DS;
    pcd->id_minor   = ComCmd_ReadFromSysVariable_Result;
    pcd->datalength = ComDataHeaderSize + no * 12 + 4;
    pcd->messageId  = messageId;
    uint32 *ph = (uint32*)&pcd->data[0];
    *ph++ = no;
    while(no--)
    {
      uint32 varno = *pvarno++; 
      uint32 value = 0;
      uint32 error = (GetSysVar(&value, varno) == true)? DS_ERROR_NONE : DS_ERROR_INVALID_VARNO;
      
      *ph++ = varno;
      *ph++ = value;
      *ph++ = error;
    }

    retcode = Send(pcd);
  }
  
  ComDataBufferDestroy(&tcomdat, pcd);

  return retcode;
}

bool cComPort::ComCmdWriteToSysVariable(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdValue *p)
{
  bool retcode = true;
  cComDatBuffer comdat;
  
  comdat.cTor(this, transactionDest, messageId, ComCmd_WriteToSysVariable_Result);
  
  while(no--)
  {
    uint08 state = (SetSysVar(p->value, p->userid) == true)? 1 : 0;
    if(comdat.AddU32U32U08Record(p->userid, p->value, state) == false)
    {
      retcode = false;
    }

    p++;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
	return retcode;
}

bool cComPort::SendComCmdReadFromDataAt_Result(uint16 transactionDest, uint32 messageId, uint32 userid, uint32 lasalid, uint32 datalen)
{
  bool retcode = false;
  
  DsComData tcomdat;
  DsComData *pcd = ComDataBufferCreate(&tcomdat, ComDataHeaderSize + datalen + 8);

  if (pcd != NULL)
  {
    pcd->transactionSrc = 0;
    pcd->transactionDest = transactionDest;
    pcd->messageId = messageId;
    pcd->id_major   = ComId_Major_DS;
    pcd->id_minor   = ComCmd_ReadFromDataAt_Result;
    pcd->datalength = ComDataHeaderSize + datalen + 8;
    
    *(uint32*)&pcd->data[0] = userid;
    *(uint32*)&pcd->data[4] = datalen;
    dslib_read_dataat(&pcd->data[8], lasalid, datalen);
    retcode = Send(pcd);
  }
  
  ComDataBufferDestroy(&tcomdat, pcd);
  
  return retcode;
}

bool cComPort::SendComCmdCpuState(uint32 cpustate)
{
  DsComData comdat;
  comdat.transactionSrc = 0;
  comdat.transactionDest = ID_TRANSACTION_ALL_CLIENTS;
  comdat.messageId = 0xFFFFFFFF;
  comdat.id_major   = ComId_Major_DS;
  comdat.id_minor   = ComCmd_CpuState;
  comdat.datalength = ComDataHeaderSize + 4;
  *(uint32*)&comdat.data[0] = cpustate;
  return Send(&comdat);  
}

bool cComPort::SendComCmdAliveTrigger(void)
{
  DsComData comdat;
  comdat.transactionSrc = 0;
  comdat.transactionDest = ID_TRANSACTION_DS;
  comdat.messageId = 0xFFFFFFFF;
  comdat.id_major   = ComId_Major_DS;
  comdat.id_minor   = ComCmd_AliveTrigger;
  comdat.datalength = ComDataHeaderSize;
  return Send(&comdat);  
}

bool cComPort::SendComCmdRefListInfo(uint16 transactionDest, uint32 messageId, uint32 channel)
{
  DsComData comdat;
  comdat.transactionSrc = 0;
  comdat.transactionDest = transactionDest;
  comdat.messageId = 0xFFFFFFFF;
  comdat.id_major   = ComId_Major_DS;
  comdat.id_minor   = ComCmd_RefList_INFO;
  comdat.datalength = ComDataHeaderSize + 2;
  *(uint16*)&comdat.data[0] = (uint16)channel;
  return Send(&comdat);
}

bool cComPort::ComCmdGetLasalId(uint16 transactionDest, uint32 messageId, uint32 no, uint08 *data)
{
  bool retcode = true;
  cComDatBuffer comdat;
  uint08 chmod;

  comdat.cTor(this, transactionDest, messageId, ComCmd_GetLasalId_Result);

  while(no--)
  {
    uint32 userid = *(uint32*)&data[0];
    data += 4;
    uint32 lasalid = dslib_get_lasal_id(&chmod, (const ascii*)data);
    
//char label[256];
//lib_get_serverlabel(label, lasalid); // todo: test, wieder entfernen
    
    data += (dslib_strlen((const ascii*)data) + 1);

    if(comdat.AddU32U32U08Record(userid, lasalid, chmod) == false)
    {
      retcode = false;
    }
  }

  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
  return retcode;
}

bool cComPort::ComCmdReadFromServer(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalId *p)
{
  bool retcode = true;
  cComDatBuffer comdat;

  comdat.cTor(this, transactionDest, messageId, ComCmd_ReadFromServer_Result);

  while(no--)
  {
    if(p->lasalid != 0)
    {
      uint32 value = dslib_call_read_method(p->lasalid);
      if(comdat.AddU32U32Record(p->userid, value) == false)
      {
        retcode = false;
      }
    }
    
    p++;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
  return retcode;
}

bool cComPort::ComCmdWriteToServer(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdValue *p)
{
  bool retcode = true;
  cComDatBuffer comdat;
  
  comdat.cTor(this, transactionDest, messageId, ComCmd_WriteToServer_Result);
  
  while(no--)
  {
    uint08 state = 1;
    if(dslib_call_write_method(p->lasalid, p->value) == ACCESS_DENIED)
    {
      state = 0;
    }

    if(comdat.AddU32U32U08Record(p->userid, p->value, state) == false)
    {
      retcode = false;
    }

    p++;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
	return retcode;
}

bool cComPort::ComCmdWriteToServerPart(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdValueOffWid *p)
{
  bool retcode = true;
  cComDatBuffer comdat;
  
  comdat.cTor(this, transactionDest, messageId, ComCmd_WriteToServer_Result);
  
  while(no--)
  {
    uint32 tmpval = dslib_call_read_method(p->lasalid);
    tmpval = dslib_merge32(tmpval, p->value, p->offset, p->width);
    
    uint08 state = 1;
    if(dslib_call_write_method(p->lasalid, tmpval) == ACCESS_DENIED)
    {
      state = 0;
    }

    if(comdat.AddU32U32U08Record(p->userid, p->value, state) == false)
    {
      retcode = false;
    }

    p++;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
	return retcode;
}

bool cComPort::ComCmdReadFromClient(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalId *p)
{
  bool retcode = true;
  cComDatBuffer comdat;

  comdat.cTor(this, transactionDest, messageId, ComCmd_ReadFromClient_Result);

  while(no--)
  {
    if(p->lasalid != 0)
    {
      uint32 value = dslib_read_client_data(p->lasalid);
      if(comdat.AddU32U32Record(p->userid, value) == false)
      {
        retcode = false;
      }
    }
    
    p++;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
  return retcode;
}

bool cComPort::ComCmdWriteToClient(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdValue *p)
{
  bool retcode = true;
  cComDatBuffer comdat;
  
  comdat.cTor(this, transactionDest, messageId, ComCmd_WriteToClient_Result);
  
  while(no--)
  {
    uint08 state = 1;
    if(dslib_write_client_data(p->lasalid, p->value) == ACCESS_DENIED)
    {
      state = 0;
    }

    if(comdat.AddU32U32U08Record(p->userid, p->value, state) == false)
    {
      retcode = false;
    }

    p++;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
	return retcode;
}

bool cComPort::ComCmdReadFromDataAt(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdDataSize *p)
{
  bool retcode = true;

  while(no--)
  { 
    if (SendComCmdReadFromDataAt_Result(transactionDest, messageId, p->userid, p->lasalid, p->size) == false)
    {
      retcode = false;
    }
    p++;
  }

  return retcode;
}

bool cComPort::ComCmdWriteToDataAt(uint16 transactionDest, uint32 messageId, uint32 no, uint08 *p)
{
  bool retcode = true;
  cComDatBuffer comdat;

  comdat.cTor(this, transactionDest, messageId, ComCmd_WriteToDataAt_Result);

  while(no--)
  {
    // *[4]...userid, 
    // *[4]...lasalid, 
    // *[4]...datasize 
    // *[.....data
    uint32 userid = *(uint32*)&p[0];
    uint32 lasalid = *(uint32*)&p[4];
    uint32 datasize = *(uint32*)&p[8];
  
    bool state = (dslib_write_dataat(lasalid, &p[12], datasize) == 0)? false : true;
    
    if(comdat.AddU32DataU08Record(userid, datasize, state, &p[12]) == false)
    {
      retcode = false;
    }
    
    p += (12 + datasize);
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
  return retcode;
}

bool cComPort::ComCmdWriteToDataAtPart(uint16 transactionDest, uint32 messageId, uint32 no, uint08 *p)
{
  bool retcode = true;
  cComDatBuffer comdat;

  comdat.cTor(this, transactionDest, messageId, ComCmd_WriteToDataAt_Result);

  while(no--)
  {
    // *[4]...userid, 
    // *[4]...lasalid
    // *[4]...datasize
    // *[4]...bitmask
      
    uint32 userid   = *(uint32*)&p[0];  
    uint32 lasalid  = *(uint32*)&p[4];  
    uint32 datasize = *(uint32*)&p[8];  
    uint32 value    = *(uint32*)&p[12];  
    uint32 writeback = 0;
    
    bool state = (dslib_write_dataat_part(&writeback, lasalid, value, datasize) == 0)? false : true;
    
    if(comdat.AddU32DataU08Record(userid, datasize, state, (uint08*)&writeback) == false)
    {
      retcode = false;
    }
    
    p += 16;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
  return retcode;
}

bool cComPort::ComCmdReadFromString(uint16 transactionDest, uint32 messageId, uint16 command, uint32 no, UserIdLasalId *p)
{
  bool retcode = true;
  cComDatBuffer comdat;
  uint16 tmpdat[65];
  
  comdat.cTor(this, transactionDest, messageId, command);
  
  while(no--)
  {
    uint32 chrsize;
    tmpdat[0] = 0; // default 0-termination 
    void *pstr = dslib_read_string((void*)tmpdat, sizeof(tmpdat), &chrsize, p->lasalid);
  
    if(comdat.AddStrRecord(p->userid, chrsize, pstr, 0xFFFF) == false) // channel unbekannt
    {
      retcode = false;
    }

    if(pstr != tmpdat)
    {
      dslib_free(pstr); // ggf. allokierten puffer wieder freigeben
    }

    p++;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
  return retcode;
}

bool cComPort::ComCmdWriteToString(uint16 transactionDest, uint32 messageId, uint32 no, uint08 *p)
{
  bool retcode = true;
  cComDatBuffer comdat;

  comdat.cTor(this, transactionDest, messageId, ComCmd_WriteToString_Result);

  while(no--)
  {
		// *[4]...userid, 
		// *[4]...lasalid, 
		// *[1]...charsize, 
		// *[.....0-string
      
    uint32 userid  = *(uint32*)&p[0];
    uint32 lasalid = *(uint32*)&p[4];
    uint08 chrsize = *(uint08*)&p[8];
    void *text     = (void*)&p[9];
  
    bool state = (dslib_write_string(lasalid, text, chrsize) == 0)? false : true;
    
    if(comdat.AddStrStateRecord(userid, chrsize, text, state) == false)
    {
      retcode = false;
    }
    
    if(no > 0)
    {
      p += dslib_strsize(text, chrsize) + 9;
    }
  }

  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
	return retcode;
}

bool cComPort::ComCmdRefListAdd(uint16 transactionDest, uint32 messageId, uint32 channel, uint32 no, RefListAddCell *p)
{
  RefListCmdStream cmd;
  init_RefListCmdStream(&cmd);
  
  uint32 maxno = sizeof(cmd.data) / (4*4);
  
  while (no != 0)
  {
    uint32 nox = (no > maxno)? maxno : no;
    no -= nox;
  
    cmd.channel = (uint08)channel;
    cmd.cmd = RefList_ADD;
    cmd.no = nox;
    uint32 *ph = &cmd.data[0];
    
    while(nox--)
    {
      ph[0] = p->lasalid;
      ph[1] = p->userid;
      ph[2] = p->reftime;
      ph[3] = p->datasize;
      ph += 4;
      p++;
    }
    
    if(RefreshListEx.AddCommand(&cmd) == false)
    {
      // todo: send answer
      return false;
    }
  }
  return true;
}

bool cComPort::ComCmdRefListSub(uint16 transactionDest, uint32 messageId, uint32 channel, uint32 no, uint32 *pdat)
{
  RefListCmdStream cmd;
  init_RefListCmdStream(&cmd);

  uint32 maxno = sizeof(cmd.data) / 4;

  while (no != 0)
  {
    uint32 nox = (no > maxno)? maxno : no;
    no -= nox;
  
    cmd.no = nox;
    cmd.channel = (uint08)channel;
    cmd.cmd = RefList_SUB;
    uint32 *ph = &cmd.data[0];
    while(nox--)
    {
      *ph++ = *pdat++;
    }
    
    if (RefreshListEx.AddCommand(&cmd) == false)
    {
      // todo: send answer
      return false;
    }
  }
  return true;
}

bool cComPort::ComCmdRefListEmpty(uint16 transactionDest, uint32 messageId, uint32 no, uint16 *pchannel)
{
  bool retcode = true;
  
  RefListCmdStream cmd;
  init_RefListCmdStream(&cmd);
  
  while(no--)
  {
    cmd.no = 1;
    cmd.channel = (uint08)*pchannel;
    cmd.cmd = RefList_EMPTY;
    pchannel++;
    
    if(RefreshListEx.AddCommand(&cmd) == false)
    {
      retcode = false;
    }
  }
  // todo: send answer
  
  return retcode;
}

bool cComPort::ComCmdRefListStartUpdate(uint16 transactionDest, uint32 messageId, uint32 no, uint16 *pchannel)
{
  bool retcode = true;
  
  RefListCmdStream cmd;
  init_RefListCmdStream(&cmd);
  while(no--)
  {
    cmd.no = 1;
    cmd.channel = (uint08)*pchannel;
    cmd.cmd = RefList_STARTUPDATE;
    pchannel++;
    
    if(RefreshListEx.AddCommand(&cmd) == false)
    {
      retcode = false;
    }
  }
  // todo: send answer
  return retcode;
}

bool cComPort::ComCmdRefListStopUpdate(uint16 transactionDest, uint32 messageId, uint32 no, uint16 *pchannel)
{
  bool retcode = true;
  
  RefListCmdStream cmd;
  init_RefListCmdStream(&cmd);
  
  while(no--)
  {
    cmd.no = 1;
    cmd.channel = (uint08)*pchannel;
    cmd.cmd = RefList_STOPUPDATE;
    pchannel++;
    
    if(RefreshListEx.AddCommand(&cmd) == false)
    {
      retcode = false;
    }
  }
  // todo: send answer
  return retcode;
}

bool cComPort::ComCmdRefListGetInfo(uint16 transactionDest, uint32 messageId, uint32 channel)
{
  ChannelNumber cn[30];
  
  uint32 no = RefreshListEx.GetCellInfo(cn, ARRAY_SIZE(cn), channel);
  
  return SendComCmdRefListInfo_Result(transactionDest, messageId, cn, no);
}

bool cComPort::SendComCmdSignIn_Result(uint16 cmd, uint16 transactionDest, uint32 messageId, uint32 magic_no, uint32 client_id, uint32 client_station, bool result)
{
  DsComData comdat;
  comdat.transactionSrc = 0;
  comdat.transactionDest = transactionDest;
  comdat.id_major   = ComId_Major_DS;
  comdat.id_minor   = cmd;
  comdat.datalength = ComDataHeaderSize + 28;
  comdat.messageId  = messageId;
  *(uint32*)&comdat.data[0]  = (result == true)?  DS_ERROR_NONE : DS_ERROR_UNKNOWN;
  *(uint32*)&comdat.data[4]  = magic_no;
  *(uint32*)&comdat.data[8]  = client_id;
  *(uint32*)&comdat.data[12] = client_station;
  *(uint32*)&comdat.data[16] = COMDEF_VERSION;
  *(uint32*)&comdat.data[20] = dslib_get_dsccrc();
  *(uint32*)&comdat.data[24] = dslib_get_cpustate();
  return Send(&comdat);
}

bool cComPort::ComCmdUserEventResponse(uint32 uid, uint08 state)
{
  return false;
}

bool cComPort::SendComCmdRefListUpdateString(UserIdLasalId *p, uint32 no, uint16 channel)
{
  bool retcode = true;
  cComDatBuffer comdat;
  uint16 tmpdat[65];
  
  comdat.cTor(this, ID_TRANSACTION_ALL_CLIENTS, 0xFFFFFFFF, ComCmd_RefList_UPDATE_STRING);
  
  while(no--)
  {
    uint32 chrsize;
    tmpdat[0] = 0; // default 0-termination 
    void *pstr = dslib_read_string((void*)tmpdat, sizeof(tmpdat), &chrsize, p->lasalid);
  
    if(comdat.AddStrRecord(p->userid, chrsize, pstr, channel) == false)
    {
      retcode = false;
    }

    if(pstr != tmpdat)
    {
      dslib_free(pstr); // ggf. allokierten puffer wieder freigeben
    }

    p++;
  }
  
  if(comdat.Finalize() == false)
  {
    retcode = false;
  }
  
  return retcode;
}

bool cComPort::SendComCmdRefListUpdate(DsComData *pcd, uint32 no, uint16 channel)
{
  if(no > 0)
  {
    pcd->transactionSrc = 0;
    pcd->transactionDest = ID_TRANSACTION_ALL_CLIENTS;
    pcd->messageId = 0xFFFFFFFF;
    pcd->id_major       = ComId_Major_DS;
    pcd->id_minor       = ComCmd_RefList_UPDATE;
    pcd->datalength     = ComDataHeaderSize + 4 + (no * 8);
    *(uint16*)&pcd->data[0] = (uint16)no;
    *(uint16*)&pcd->data[2] = channel;

    return Send(pcd);
  }
  
  return true;
}

bool cComPort::SendComCmdRefListInfo_Result(uint16 transactionDest, uint32 messageId, ChannelNumber *pcn, uint32 no)
{
  // [4] .... no
  // *[2] ... channel
  // *[2] ... anzahl

  bool retcode = false;
  DsComData tcomdat;
  DsComData *pcd = ComDataBufferCreate(&tcomdat, (no * 4) + 4 + ComDataHeaderSize);

  if(pcd != NULL)
  {
    pcd->transactionSrc = 0;
    pcd->transactionDest = transactionDest;
    pcd->messageId = messageId;
    pcd->id_major   = ComId_Major_DS;
    pcd->id_minor   = ComCmd_RefList_INFO_Result;
    pcd->datalength = ComDataHeaderSize + 4 + no*4;
    *(uint32*)&pcd->data[0] = no;
    uint16 *ph = (uint16*)&pcd->data[4];
  
    while(no--)
    {
      *ph++ = pcn->channel;
      *ph++ = pcn->no;
      pcn++;
    }

    retcode = Send(pcd);
  }
  
  ComDataBufferDestroy(&tcomdat, pcd);
  return retcode;
}

void cComPort::DoUserText(const char *usertext)
{
  UserText[0] = 0;
  
  if(usertext != NULL)
  {
    unsigned long len = dslib_strlen(usertext);
    
    if(len >= sizeof(UserText))
    {
      len = sizeof(UserText) - 1;
    }
    
    dslib_memcpy(UserText, usertext, len);
    UserText[len] = 0;
  }
}

bool cComPort::ComCmdSignIn(uint16 cmd, uint16 transactionDest, uint32 messageId, uint32 constant_or_magic_id, uint32 client_id, uint32 client_station, uint32 client_version, const char *usertext)
{
  bool state = false;
  
	if (cmd == ComCmd_SignIn)
	{
    CpuState    = 0xFFFFFFFF;
    MagicNumber = 0;
    if(constant_or_magic_id == ComId_SignInIdStandard)
    {
      // standard login get:SignIn --> send:SignInResult+MagicNo --> get:SignInEx+decoded MagicNo --> send:SignInExResult
      MagicNumber = dslib_magicnumber();
      state = true;
    }
    else if(constant_or_magic_id == ComId_SignInIdSigmatek)
    {
      // master login get:SignIn --> send:SignInExResult
      ComAlarm.Sleep(); // keine Alarme, diese müssen extra gestartet werden
      uint32 memoid = ClientId;
      ClientId = client_id;
      DoUserText(usertext);
      state = Api.RegisterClient(this);
      IsRegistered = 1;
      if(state == false)
      {
        ClientId = memoid;
        UserText[0] = 0;
      }
      
      MagicNumber = dslib_magicnumber();
      return SendComCmdSignIn_Result(ComCmd_SignInExResult, transactionDest, messageId, MagicNumber, client_id, client_station, state);
    }
    return SendComCmdSignIn_Result(ComCmd_SignInResult, transactionDest, messageId, MagicNumber, client_id, client_station, state);
  }
  else if(cmd == ComCmd_SignInEx)
  {
		if(dslib_encryptU32(constant_or_magic_id) == MagicNumber)
		{
      uint32 memoid = ClientId;
      ClientId = client_id;
      DoUserText(usertext);
      state = Api.RegisterClient(this);
      IsRegistered = 1;
      if(state == false)
      {
        ClientId = memoid;
        UserText[0] = 0;
      }
    }
    return SendComCmdSignIn_Result(ComCmd_SignInExResult, transactionDest, messageId, MagicNumber, client_id, client_station, state);
  }
  
  return false;
}

bool cComPort::ComCmdSignOut(uint16 transactionDest, uint32 messageId)
{
//  DsComData tmp;
//  DsComData *pcd = ComDataBufferCreate(&tmp, ComDataHeaderSize);
//  if (pcd != NULL)
//  {
//    pcd->transactionSrc = 0;
//      pcd->transactionDest = transactionDest;
//      pcd->messageId = messageId;
//    pcd->id_major = ComId_DS_Major;
//    pcd->id_minor = ComCmd_SignOutResult;
//    pcd->datalength = ComDataHeaderSize;
//    Send(pcd);
//
//    ComDataBufferDestroy(&tmp, pcd);
//  }

  return false; // logout wurde akzeptiert
}


bool cComPort::ComCmdLoop(DsComData *pcd)
{
    pcd->id_minor = ComCmd_Loop_Result;
    return Send(pcd);
}

bool cComPort::ComCmdNewInst(uint16 transactionDest, uint32 messageId, uint32 userid, uint08 *data, uint32 datalength)
{
  // datalength... nur länge der daten in bytes ohne 2 byte für uiLnu
  UNUSED(datalength);

  bool retcode = false;
  
  DsComData tcomdat;
  DsComData *pcd = ComDataBufferCreate(&tcomdat, ComDataHeaderSize + 500);

  if(pcd != NULL)
  {
    pcd->transactionSrc = 0;
    pcd->transactionDest = transactionDest;
    pcd->messageId = messageId;
    pcd->id_major   = ComId_Major_DS;
    pcd->id_minor   = ComCmd_NewInst_Result;

    *(uint16*)&pcd->data[8] = 0; // init results.uiLen
    *(uint32*)&pcd->data[0] = userid;
    *(uint32*)&pcd->data[4] = dslib_call_new_inst(data, datalength, &pcd->data[8]);

    pcd->datalength = ComDataHeaderSize + 8 + *(uint16*)&pcd->data[8];

    // [4] ... userid
    // [4] ... result (iprstate)
    // [2] ... datalength (results.uilen) immer 2 + eigentliche datalen in bytes
    // [ ..... data

    retcode = Send(pcd);
  }
  
  ComDataBufferDestroy(&tcomdat, pcd);
  
  return retcode;
}

/* // otto
bool cComPort::ComCmdSendFragment(uint16 transactionDest, uint32 messageId, uint32 uid, DsComData* pcd, uint32 tabsolut)
{
  uint32 error = DS_ERROR_MEMORY;
  if(pFragmentHandler != NULL)
  {
    if(pcd->datalength > (FRAGMENTHEADER_SIZE + ComDataHeaderSize))
    {
      if(pFragmentHandler->addData(pcd, this, tabsolut))
      {
         error = DS_ERROR_NONE;
      }
    }
  }
  
  //send the rsp
  DsComData rsp;
  rsp.transactionSrc = 0;
  rsp.transactionDest = transactionDest;
  rsp.messageId = messageId;
  rsp.id_major = ComId_Major_DS;
  rsp.id_minor = ComCmd_SendFragment_Rsp;
  
  *(uint32*)&rsp.data[0] = uid; 
  *(uint32*)&rsp.data[4] = error;
  
  rsp.datalength = ComDataHeaderSize + 8;
  
  // [4]...uid
  // [4]...error
  
  return Send(&rsp);
}
*/  

bool cComPort::DataReciever(DsComData *pcd, uint32 tabsolute)
{
  // [4] .......... length
  // [2] .......... data_id major
  // [2] .......... data_id minor
  // [2] .......... transactionSrc
  // [2] .......... transactionDest
  // [4] .......... messageId
  // *[ ........... data

  if(ClientId == 0) // es gibt noch keinen erfolgreich angemeldeten client
  {
    if((pcd->id_minor != ComCmd_SignIn) && (pcd->id_minor != ComCmd_SignInEx))
    {
      // solange sich client noch nicht authentifiziert hat werden alle anderen kommandos geblockt
      return true;
    }
  }

  AliveTimex = tabsolute;

  Api.SpyData(this, pcd, 1, DataStackLevel); // SPY: daten vom Client empfangen

	if(pcd->id_major == ComId_Major_DS)
	{
		switch (pcd->id_minor)
		{
		case ComCmd_NOP:
		case ComCmd_AliveTrigger:
			break;
    
    case ComCmd_SignInEx:
    case ComCmd_SignIn:
      // [4] ... constant or magic id
      // [4] ... client_id
      // {4] ... station
      // [4] ... version of software in client
      ComCmdSignIn(pcd->id_minor, pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], *(uint32*)&pcd->data[4], *(uint32*)&pcd->data[8], *(uint32*)&pcd->data[12], (const char*)((pcd->datalength > (ComDataHeaderSize+16))? &pcd->data[16]:NULL));
      break;
    
    case ComCmd_SignOut:
      AliveTimex = tabsolute - (StandardTimeOut_AliveTrigger + 1); 
      ComCmdSignOut(pcd->transactionSrc, pcd->messageId);
      return false;
      
		case ComCmd_ReadFromSysVariable:
      // [4]....no
			// *[4]...varid
			ComCmdReadFromSysVariable(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (uint32*)&pcd->data[4]);
			break;

		case ComCmd_GetLasalId:
      // [4]....no
			// *[4]...userid, 
			// *[.....ascii-0-string

			// lasalid ermitteln und mit ComCmd_GetLasalId_Result beantworten
			ComCmdGetLasalId(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (uint08*)&pcd->data[4]);
			break;

		case ComCmd_ReadFromServer:
			// [4]....no
			// *[4]...userid 
			// *[4]...lasalid

			// serverwert auslesen und mit ComCmd_ReadFromServer_Result beantworten
			ComCmdReadFromServer(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (UserIdLasalId*)&pcd->data[4]);
			break;

		case ComCmd_WriteToServer:
			// [4]....no
			// *[4]...userid
			// *[4]...lasalid
			// *[4]...value 

			// serverwert schreiben und mit ComCmd_WriteToServer_Result beantworten
			ComCmdWriteToServer(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (UserIdLasalIdValue*)&pcd->data[4]);
			break;

    case ComCmd_WriteToSysVariable:
      // [4]....no
			// *[4]...userid
      // *[4]...lasalid
      // *[4]...value
      ComCmdWriteToSysVariable(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (UserIdLasalIdValue*)&pcd->data[4]);
      break;

    case ComCmd_WriteToServerPart:
      // [4]....no
			// *[4]...userid
			// *[4]...lasalid
			// *[4]...value 
      // *[2]...bit_offset
      // *[2]...bit_width
      
      // komponente von serverwert schreiben und mit ComCmd_WriteToServer_Result beantworten
			ComCmdWriteToServerPart(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (UserIdLasalIdValueOffWid*)&pcd->data[4]);
      break;

    case ComCmd_ReadFromClient:
      // [4]....no
      // *[4]...userid 
      // *[4]...lasalid
      
      // wert von client auslesen und mit ComCmd_ReadFromClient_Result beantworten
      ComCmdReadFromClient(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (UserIdLasalId*)&pcd->data[4]);
      break;
      
    case ComCmd_WriteToClient:
      // [4]....no
      // *[4]...userid
      // *[4]...lasalid
      // *[4]...value 
      
      // wert auf client schreiben und mit ComCmd_WriteToClient_Result beantworten
      ComCmdWriteToClient(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (UserIdLasalIdValue*)&pcd->data[4]);
      break;

		case ComCmd_ReadFromDataAt:
      // [4]....no
      // *[4]...userid 
      // *[4]...lasalid 
      // *[4]...datasize

			// daten von DataAt lesen und mit ComCmd_ReadFromDataAt_Result beantworten
      ComCmdReadFromDataAt(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (UserIdLasalIdDataSize*)&pcd->data[4]);
      break;

    case ComCmd_WriteToDataAt:
      // [4]....no
      // *[4]...userid 
      // *[4]...lasalid 
      // *[4]...datasize 
      // *[.....data
      
      //datat schreiben und mit ComCmd_WriteToDataAt_Result beantworten
      ComCmdWriteToDataAt(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], &pcd->data[4]);
      break;

    case ComCmd_WriteToDataAtPart:
      // [4]....no
      // *[4]...userid, 
      // *[4]...lasalid
      // *[4]...datasize
      // *[4]...bitmask
      
      // databit setzen und mit ComCmd_WriteToDataAt_Result beantworten
      ComCmdWriteToDataAtPart(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], &pcd->data[4]);
      break;

		case ComCmd_ReadFromString:
			// [4]....no
			// *[4]...userid
			// *[4]...lasalid

			// string auslesen und mit ComCmd_ReadFromString_Result beantworten
			ComCmdReadFromString(pcd->transactionSrc, pcd->messageId, ComCmd_ReadFromString_Result, *(uint32*)&pcd->data[0], (UserIdLasalId*)&pcd->data[4]);
			break;

		case ComCmd_WriteToString:
      // [4]....no
			// *[4]...userid, 
			// *[4]...lasalid, 
			// *[1]...charsize, 
			// *[.....0-string

			// string schreiben und mit ComCmd_WriteToServer_Result beantworten
			ComCmdWriteToString(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], &pcd->data[4]); //, *(uint08*)&pcd->data[8], (const void*)&pcd->data[9]);
			break;

		case ComCmd_RefList_ADD:
			// [2]...channel
			// [2]...no
			// *[4]...userid
			// *[4]...lasalid
			// *[4]...reftime
			// *[4]...bytesize

			// neue zellen in refreshliste einfügen
			ComCmdRefListAdd(pcd->transactionSrc, pcd->messageId, *(uint16*)&pcd->data[0], *(uint16*)&pcd->data[2], (RefListAddCell*)&pcd->data[4]);
			break;

		case ComCmd_RefList_SUB:
			// [2]....channel
			// [2]....no
			// *[4]...userid

			// bestehende zelle aus refreshliste entfernen
			ComCmdRefListSub(pcd->transactionSrc, pcd->messageId, *(uint16*)&pcd->data[0], *(uint16*)&pcd->data[2], (uint32*)&pcd->data[4]);
			break;

    case ComCmd_RefList_EMPTY:
      // [2]....no
			// *[2]...channel

			// alle refreshlisteneinträge löschen
			ComCmdRefListEmpty(pcd->transactionSrc, pcd->messageId, *(uint16*)&pcd->data[0], (uint16*)&pcd->data[2]);
			break;

		case ComCmd_RefList_STARTUPDATE:
      // [2]....no
			// *[2]...channel

			// update von refreshliste wieder starten
			ComCmdRefListStartUpdate(pcd->transactionSrc, pcd->messageId, *(uint16*)&pcd->data[0], (uint16*)&pcd->data[2]);
			break;

		case ComCmd_RefList_STOPUPDATE:
      // [2]....no
			// *[2]...channel

			// update von refreshliste aussetzen
			ComCmdRefListStopUpdate(pcd->transactionSrc, pcd->messageId, *(uint16*)&pcd->data[0], (uint16*)&pcd->data[2]);
			break;

    case ComCmd_RefList_INFO:
      // [2]...channel (0xFFFF ... alle channels)
			ComCmdRefListGetInfo(pcd->transactionSrc, pcd->messageId, *(uint16*)&pcd->data[0]);
      break;

    case ComCmd_Loop:
			//[......loopdata
			ComCmdLoop(pcd);
			break;
    
    case ComCmd_UserEventResponse:
      // [1]...state
      ComCmdUserEventResponse(pcd->messageId,*(uint08*)&pcd->data[0]);
      break;

    case ComCmd_SendFragment:
      // [4]...id
      // [4]...uid
      // [8]...originalLength
      // [4]...dataLength
      // [8]...offset
      FragmentMgr.Add(this, pcd, tabsolute);
//      ComCmdSendFragment(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[4], pcd, tabsolute);
      break;

    case ComCmd_NewInst:
      // [4]...userid
      // [4]...length of data in bytes
      // [4]...lasalid
      // [2]...cmd
      // [.....data
      ComCmdNewInst(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], &pcd->data[8], *(uint32*)&pcd->data[4]);
      break;
      
//		case ComCmd_GetSysVariable_Result: // wird auf server nicht benötigt
//		case ComCmd_WriteToServer_Result:
//		case ComCmd_ReadFromServer_Result:
//		case ComCmd_GetLasalId_Result:
//		case ComCmd_ReadFromDataAt_Result:
//    case ComCmd_RefList_INFO_Result:
//			break;

    case ComCmd_AlarmStart:
      // [4]...applikationid
      // [4]...label of alarmXbuffer object
      ComAlarm.Start(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (ascii*)&pcd->data[4]);
      break;
      
    case ComCmd_AlarmStop:
      ComAlarm.Stop(pcd->transactionSrc, pcd->messageId);
      break;

    case ComCmd_AlarmQuit:
      // [4]...no (no = 0xFFFFFFFF, alle alarme quittieren)
      // *[4]..alano
      
      // alarme quittieren und mit Kommando ComCmd_AlarmQuit_Result beantworten
      ComAlarm.QuitAlarm(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0], (uint32*)&pcd->data[4]);
      break;

    case ComCmd_AlarmQuitAll:
      // alarme quittieren und mit Kommando ComCmd_AlarmQuitAll_Result beantworten
      ComAlarm.QuitAlarmAll(pcd->transactionSrc, pcd->messageId);
      break;

    case ComCmd_AlarmQuitGroup:
      // [4]...group
      // alarmgruppe quittieren und mit Kommando ComCmd_AlarmQuitGroup_Result beantworten
      ComAlarm.QuitAlarmGroup(pcd->transactionSrc, pcd->messageId, *(uint32*)&pcd->data[0]);
      break;
      
    case ComCmd_AlarmScanRecordEx:
      // Alle vorhandenen Alarme werden auf 'state' überprüft, falls gleich wird Alarm im XAlarmRingBuffer gesucht und Parameter und Zeitstempel daraus übernommen.
      // [4]...state
      // [4]...0=egal, 1=in buffer, 2=nicht in buffer
      ComAlarm.ScanAlarmStateStart(*(uint32*)&pcd->data[0], *(uint32*)&pcd->data[4]);
      break;
		}
	}
  else
  {
    Api.RecieveData(this, pcd); // userdata empfangen
  }
  
  return true;
}


#define DataStackLevel_ALARM 50

bool cComPort::Run(void)
{
  uint32 tabsolute = dslib_tabsolute();

  if(ClientId == 0)
  {
    if((tabsolute - StartTimex) > StandardTimeOut_ClientNotify)
    {
      return false; // resourcen wieder freigeben, client hat sich nicht authentifiziert
    }
  }

  int32 st = dslib_tcp_recv(Socket, &Data[DataSize],  sizeof(Data)-DataSize);
  if(st > 0)
  {
    DataSize += (uint32)st;
  }
  
  uint32 repeat = 0;
  bool retcode = true;
  
RepeatCmd:
  
  tabsolute = dslib_tabsolute();
  if((tabsolute - LastSendTime) > StandardTimeOut_SendAliveTrigger)
  {
    if((tabsolute - LastSendAliveTrgTime) > 33) // Bremse
    {
      LastSendAliveTrgTime = tabsolute;
      SendComCmdAliveTrigger();
    }
  }

  if(DataSize >= ComDataHeaderSize)
  {
    uint32 tmpsize = *(uint32*)&Data[0];
    if(tmpsize <= DataSize)
    {
      if(DataReciever((DsComData*)&Data[0], tabsolute) == false)
      {
        retcode = false; // logout wurde empfangen
        return false;
      }
      
      DataSize -= tmpsize;
       
      if(DataSize > 0)
      {
        dslib_memmove(&Data[0], &Data[tmpsize], DataSize);
        if(repeat == 0)
        {
          repeat++;
          goto RepeatCmd;
        }
        
        DataStackLevel ++; // Füllstand incrementieren
        if(DataStackLevel > DataStackLevel_ALARM)
        {
          Api.LevelWatchdog(this, DataStackLevel-DataStackLevel_ALARM);
        }
      }
    }
    
    if(FragmentMgr.Usage != 0) // check FragmentManager und ggf. Fragmentspeicher freigeben
    {
      FragmentMgr.Check(tabsolute);
    }
  }
  
  if((DataStackLevel > 0) && (DataSize == 0))
  {
    if(DataStackLevel > DataStackLevel_ALARM)
    {
      Api.LevelWatchdog(this, 0);
    }
    DataStackLevel = 0; // Füllstand auf 0 setzen
  }
  
//  if(pFragmentHandler != NULL)
//  {
//    pFragmentHandler->checkTime(tabsolute);
//  }
  
  if((tabsolute - AliveTimex) > StandardTimeOut_AliveTrigger)
  {
    return false;
  }
  
  if(ClientId != 0) // nur wenn client angemeldet ist
  {
    uint32 act = dslib_get_cpustate();
    if(act != CpuState)
    {
      CpuState = act;
      SendComCmdCpuState(act);
    }
  
    RefreshListEx.Run();
    ComAlarm.Run(tabsolute);
  }
  
  return retcode;
}

uint32 cComPort::GetAlarmHandle(void)
{
  return ComAlarm.GetAlarmHandle();
}

char *cComPort::GetUserText(void)
{
  return UserText;
}
