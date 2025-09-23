// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 23.10.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "DS_Main.h"
#include "DS_Lib.h"
#include "DS_cComAlarm.h"
#include "DS_cComPort.h"
#include "DS_cComDatBuffer.h"

cComAlarm *cComAlarm::New(cComPort *pcp)
{
  cComAlarm *retcode = NULL;
  retcode = (cComAlarm*)dslib_malloc(sizeof(cComAlarm));
  if(retcode != NULL)
  {
    retcode->cTor(pcp);
  }
  
  return retcode;
}

cComAlarm *cComAlarm::Delete(cComAlarm *p)
{
  if(p != NULL)
  {
    p->dTor();
    dslib_free(p);
  }
  return NULL;
}

void cComAlarm::cTor(cComPort *pcp)
{
  AlarmXBufferVersion = 0;
  ScanStartIndex  = 0; // 
  ScanAlarmState  = 1; // aktive alarme
  ScanAlarmAddState = 0; // ungeachtet ob im xBuffer oder nicht
  pComPort        = pcp;
  ObjLasalId      = 0;
  ApplicationId   = 0;
  Step            = 0;
  AlarmHandle     = 0xFFFFFFFFUL;
  ConfigIndex     = 0;
  AlarmNo         = 0;
  TimexRunAlarm   = 0;
  AlarmRecordSize = 29;
}

void cComAlarm::Sleep(void)
{
  Step = 998; // do nothing
}

void cComAlarm::dTor(void)
{
  // UnregHmi(); //  NOTE: Nicht bei Alarmsystem abmelden, Wunsch LVD (lesmax)
  cTor(pComPort);
}

//bool cComAlarm::UnregHmi(void)
//{
//  if(AlarmHandle != 0xFFFFFFFF)
//  {
//    if(LDR_DS_CountAlarmHandle(AlarmHandle) == 0) // Vorkommen von Alarmhandle zählen
//    {
//      // Alarmsubscriber wieder abmelden bei AlaXBuffer Klasse
//      uint08 data[32];
//      uint08 result[256];
//      
//      *(uint16*)&result[0] = 0;
//      *(uint32*)&result[2] = 0xFFFFFFFF; // default init
//  
//      *(uint16*)&data[0] = 201; // unregister hmi
//      *(uint32*)&data[2] = AlarmHandle;
//      
//      dslib_call_new_inst_direct(ObjLasalId, data, result);
//    }
//  }
//}

bool cComAlarm::Send(DsComData *pcd)
{
  if(pComPort != NULL)
  {
    return pComPort->Send(pcd);
  }
  return false;
}

bool cComAlarm::Stop(uint16 transactionDest, uint32 messageId)
{
  uint08 tmp[32];
  DsComData *pcd = (DsComData*)dslib_buffer_create(tmp, sizeof(tmp), ComDataHeaderSize);
  if(pcd != NULL)
  {
    pcd->transactionSrc  = 0;
    pcd->transactionDest = transactionDest;
    pcd->messageId       = messageId;
    pcd->id_major        = ComId_Major_DS;
    pcd->id_minor        = ComCmd_AlarmStop_Result;
    pcd->datalength      = ComDataHeaderSize;
      
    Send(pcd);
    dslib_buffer_destroy(tmp, pcd);
  }
  
  dTor();
  Sleep();
  
  return true;
}

uint32 cComAlarm::GetVersion(void)
{
  uint08 data[16];
  uint08 result[256];
  
  *(uint16*)&data[0] = 205; // GetVersion
  if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
  {
    return *(uint32*)&result[2];
  }
  return 0;
}

uint32 cComAlarm::GetNumberOfAlarm(void)
{
  uint08 data[16];
  uint08 result[256];
  
  *(uint32*)&result[2] = 0xFFFFFFFF;
  
  *(uint16*)&data[0] = (AlarmXBufferVersion < 4)? 204 : 207; // get number of alarms in buffer
  *(uint32*)&data[2] = AlarmHandle; // nur bei cmd 207
  if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
  {
    uint32 tmp = *(uint32*)&result[2];
    if(tmp != 0xFFFFFFFF)
    {
      return tmp;
    }
  }
  return AlarmNo;
}

bool cComAlarm::SendStartResult(uint16 transactionDest, uint32 messageId, uint32 errorcode, uint32 version, uint32 buffersize)
{
  bool    retcode = false;
  uint08  tmp[64];
  DsComData *pcd = (DsComData*)dslib_buffer_create(tmp, sizeof(tmp), ComDataHeaderSize + 12);
  
  if(pcd != NULL)
  {
    pcd->transactionSrc  = 0;
    pcd->transactionDest = transactionDest;
    pcd->messageId       = messageId;
    pcd->id_major        = ComId_Major_DS;
    pcd->id_minor        = ComCmd_AlarmStart_Result;

    *(uint32*)&pcd->data[0] = errorcode; // error
    *(uint32*)&pcd->data[4] = version; // version
    *(uint32*)&pcd->data[8] = buffersize; // max. anzahl der records im puffer

    pcd->datalength = ComDataHeaderSize + 12;

    retcode = Send(pcd);
    dslib_buffer_destroy(pcd, tmp);
  }
  return retcode;
}

uint32 get_serverlabel(ascii *dst, uint32 lasalid)
{
  if(lasalid != 0)
  {
    if(dslib_get_serverlabel(dst, lasalid) == 0)
    {
      dslib_strcpy(dst, "unknown");
    }
    return dslib_strlen(dst) + 1;
  }
  
  *dst = 0;
  return 1;
}

bool cComAlarm::SendConfig(uint16 transactionDest, uint32 messageId, uint32 alarmlasalid, uint32 config, uint32 group, uint32 nopara, uint32 *para)
{
  DsComData cbuff;
  
  cbuff.transactionSrc  = 0;
  cbuff.transactionDest = transactionDest;
  cbuff.messageId       = messageId;
  cbuff.id_major        = ComId_Major_DS;
  cbuff.id_minor        = ComCmd_AlarmConfig;

  *(uint32*)&cbuff.data[0]  = alarmlasalid; // lasalid vom alarmserver
  *(uint32*)&cbuff.data[4]  = config; // configuration
  *(uint32*)&cbuff.data[8]  = group; // gruppe
  *(uint32*)&cbuff.data[12] = nopara; // gruppe
    
  uint32 idx = 16;
  
  idx += get_serverlabel((ascii*)&cbuff.data[idx], alarmlasalid); // alarmservernamen eintragen
    
  while(nopara--)
  {
//    ascii label[256];
//    label[0]   = 0;
    uint32 lid = *para++;
    idx += get_serverlabel((ascii*)&cbuff.data[idx], lid);
    
//    {
//    
//      if(lib_get_serverlabel(label, lid) == 0) // ermittle servernamen zur lasalid
//      {
//        lib_strcpy(label, "unknown");
//      }
//    }
//    
//    uint32 len = lib_strlen(label) + 1;
//    lib_memcpy(&cbuff.data[idx], label, len); // namen der Parameter eintragen, für konstante einen 0-string 
//    idx += len;
  }

  cbuff.datalength = ComDataHeaderSize + idx;

  return Send(&cbuff);
}

bool cComAlarm::Start(uint16 transactionDest, uint32 messageId, uint32 application_id, const ascii *label)
{
  uint08 chmode;
  uint32 version = 0;
  uint32 error = DS_ERROR_UNKNOWN;
  
  if(label == NULL)
  {
    label = "_AlarmXBuffer1";
  }
  else if(label[0] == 0)
  {
    label = "_AlarmXBuffer1";
  }
  
  ObjLasalId = dslib_get_lasal_id(&chmode, label);
  
  if(ObjLasalId != 0)
  {
    ApplicationId = application_id;
    
    version = GetVersion(); 
    
    if(version >= 3) // version der klasse überprüfen
    {
      uint08 data[32];
      uint08 result[256];
  
      *(uint16*)&data[0] = 200; // register hmi
      *(uint32*)&data[2] = ApplicationId;
      *(uint32*)&data[6] = 0;
    
      if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
      {
        uint32 hdl = *(uint32*)&result[2]; // alarmhandle
        if(hdl != 0xFFFFFFFF)
        {
          // [2] ... length in bytes
          // [4] ... handle to get access (error = 16#FFFFFFFF)
          // [4] ... index of last validated element
          // [4] ... number of buffered elements
          // [4] ... size of single element in byte
          // [4] ... max. count of entries in buffer
          if(SendStartResult(transactionDest, messageId, DS_ERROR_NONE, version, *(uint32*)&result[18]) == true)
          {
            AlarmHandle     = hdl;
            AlarmNo         = *(uint32*)&result[6];
            AlarmRecordSize = *(uint32*)&result[14];
            AlarmXBufferVersion = version;
            Step = 1;
            return true;
          }
        }
      }
    }
  }
  else
  {
    error = DS_ERROR_INVALID_LASALID;
  }

  SendStartResult(transactionDest, messageId, error, version, 0);
 
  return false;
}

bool cComAlarm::RunConfig(void)
{
  uint08 data[32];
  uint08 result[256];
  
  *(uint16*)&data[0] = 210; // get configuration
  *(uint32*)&data[2] = AlarmHandle;
  *(uint32*)&data[6] = ConfigIndex;

  if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
  {
    // [4] ... index oder 0xFFFFFFFF
    // [4] ... lasalid
    // [4] ... config
    // [4] ... group
    // [4] ... number of parameter
    // *[4] .. parameter
     
    if(*(uint16*)&result[0] >= 6)
    {
      if(*(uint32*)&result[2] == ConfigIndex)
      {
        SendConfig(ID_TRANSACTION_ALL, 0xFFFFFFFF, *(uint32*)&result[6], *(uint32*)&result[10], *(uint32*)&result[14], *(uint32*)&result[18], (uint32*)&result[22]);
        ConfigIndex += 1;
      }
      else
      {
        Step = 2;
      }
      
      return true;
    }
  }  
  
  Step = 999; // error
  return false;
}

void decode_datetime(uint32 &date, uint32 &time, uint32 datim)
{
  // YY MM DD HH MM SS 
  //  6  4  5  5  6  6

  // JJJJJJJJJJJJJJJJ MMMMTTTTTTTTWWWW
  // HHHHHHHHMMMMMMMM SSSSSSSS00000000

  date  = (((datim >> 26) & 0x3F) + 2000) << 16;
  date |= ((datim >> 22) & 0x0F) << 12;
  date |= ((datim >> 17) & 0x1F) << 4;

  time  = ((datim >> 12) & 0x1F) << 24;
  time |= ((datim >> 6) & 0x3F) << 16;
  time |= (datim & 0x3F) << 8;
}


bool cComAlarm::SendAlarm202(uint16 transactionDest, uint32 messageId, uint16 minorId, uint32 startidx, uint32 no, uint08 *pa)
{
  DsComData cbuff;
  
  cbuff.transactionSrc  = 0;
  cbuff.transactionDest = transactionDest;
  cbuff.messageId       = messageId;
  cbuff.id_major        = ComId_Major_DS;
  cbuff.id_minor        = minorId;
  
  uint32 idx = 4;
  *(uint32*)&cbuff.data[0] = 0; // initialisiere die Anzahl der Alarme

  // [4] ........... anzahl
  // *[4] .......... lasalid
  // *[4] .......... alano
  // *[4] .......... group
  // *[4] .......... date 
  // *[4] .......... time 
  // *[1] .......... state (0=lo, 1=hi, 255=quit)
  // *[1] .......... groupbit
  // *[1] .......... parano
  // *[1] .......... mit 0 initialisiert, falls userdata vorhanden sind wird 1.Bit gesetzt (erst ab _AlarmXBuffer Version 5)
  // *[parano*4] ... parameter
  // *[4] .......... userdata type (erst ab _AlarmXBuffer Version 5)
  // *[4] .......... userdata length (erst ab _AlarmXBuffer Version 5)
  // *[ ............ userdata (erst ab _AlarmXBuffer Version 5)
  
  // Info: Ab _AlarmXBuffer Version 5 können beliebig lange UserDaten bei einem Alarm vorhanden sein.
  //       Aufgrund Kompatibilität zu bisherigen DataService Versionen muss ein Alarm welcher Userdaten beinhaltet immer am Ende des 
  //       hier zusammengebauten Datepaketes sein --> somit ist Kompatibilität gewährleistet (Bisherige DataService Version ignorieren 
  //       einfach diese Additional UserData)
  
  while(no--)
  {
    uint08 state  = *(uint08*)&pa[8];
    uint32 nopara = (state == 255)? 0 : ((state >> 1) & 0x3F);
    uint32 tmpidx = idx + 24 + (nopara * 4);
    
    if(tmpidx > sizeof(cbuff.data)) // falls puffer zu klein sein sollte --> paketweise senden
    {
      cbuff.datalength = ComDataHeaderSize + idx;
      Send(&cbuff);
      idx = 4;
      *(uint32*)&cbuff.data[0] = 0; // anzahl der alarme auf 0 setzen
    }
  
    *(uint32*)&cbuff.data[0]  += 1; // inc anzahl der alarme

    *(uint32*)&cbuff.data[idx] = *(uint32*)&pa[13];       idx += 4; // [4] ... lasalid
    *(uint32*)&cbuff.data[idx] = *(uint32*)&pa[0];        idx += 4; // [4] ... alano
    *(uint32*)&cbuff.data[idx] = *(uint32*)&pa[9];        idx += 4; // [4] ... group
    
    uint32 date, time;
    decode_datetime(date, time, *(uint32*)&pa[4]);
    *(uint32*)&cbuff.data[idx] = date;                    idx += 4; // [4] ... date
    *(uint32*)&cbuff.data[idx] = time;                    idx += 4; // [4] ... time

    if(state == 255) // quit
    {
      *(uint08*)&cbuff.data[idx] = 255;                   idx += 1; // [1] ... state
      *(uint08*)&cbuff.data[idx] = 0;                     idx += 1; // [1] ... groupbit
      *(uint08*)&cbuff.data[idx] = 0;                     idx += 1; // [1] ... parano
      *(uint08*)&cbuff.data[idx] = 0;                     idx += 1; // [1] ... 0
    }
    else
    {
      *(uint08*)&cbuff.data[idx] = (state & 0x01)? 1 : 0; idx += 1; // [1] ... state
      *(uint08*)&cbuff.data[idx] = (state & 0x80)? 1 : 0; idx += 1; // [1] ... groupbit
      *(uint08*)&cbuff.data[idx] = (uint08)nopara;        idx += 1; // [1] ... parano
      *(uint08*)&cbuff.data[idx] = 0;                     idx += 1; // [1] ... 0
      
      dslib_memcpy(&cbuff.data[idx], &pa[17], nopara*4);
      idx += nopara * 4;
    }
    
    pa += AlarmRecordSize;
  }

  cbuff.datalength = ComDataHeaderSize + idx;

  return Send(&cbuff);  
}

bool cComAlarm::SendAlarm203(uint16 transactionDest, uint32 messageId, uint16 minorId, uint32 startidx, uint32 no, uint08 *pa)
{
  DsComData10K *cbuff = &ComData10K;
  
  cbuff->transactionSrc  = 0;
  cbuff->transactionDest = transactionDest;
  cbuff->messageId       = messageId;
  cbuff->id_major        = ComId_Major_DS;
  cbuff->id_minor        = minorId;
  
  *(uint32*)&cbuff->data[0] = 0; // initialisiere die Anzahl der zu sendenden Alarme
  uint32 idx = 4;

  // out -->
  // [4] ........... anzahl
  // *[4] .......... lasalid
  // *[4] .......... alano
  // *[4] .......... group
  // *[4] .......... date 
  // *[4] .......... time 
  // *[1] .......... state (0=lo, 1=hi, 255=quit)
  // *[1] .......... groupbit
  // *[1] .......... parano
  // *[1] .......... mit 0 initialisiert, falls userdata vorhanden sind wird 1.Bit gesetzt (erst ab _AlarmXBuffer Version 5)
  // *[parano*4] ... parameter
  // *[4] .......... userdata type (erst ab _AlarmXBuffer Version 5)
  // *[4] .......... userdata length (erst ab _AlarmXBuffer Version 5)
  // *[ ............ userdata (erst ab _AlarmXBuffer Version 5)
  
  // Info: Ab _AlarmXBuffer Version 5 können beliebig lange UserDaten bei einem Alarm vorhanden sein.
  //       Aufgrund Kompatibilität zu bisherigen DataService Versionen muss ein Alarm welcher Userdaten beinhaltet immer am Ende des 
  //       hier zusammengebauten Datepaketes sein --> somit ist Kompatibilität gewährleistet (Bisherige DataService Version ignorieren 
  //       einfach diese Additional UserData)
  
  bool send = false;
  
  while(no--)
  {
#pragma pack(push, 1)
    typedef struct  // so wirds von SPS gesendet
    {
      uint32 alano;
      uint32 datim;
      uint16 bytelength;
      uint08 nc;
      uint08 state;
      uint32 group;
      uint32 lasalid;
      uint32 userdata_size;
      uint32 userdata_id;
      uint08 paradat[64];     // nopara * 4 + userdata_size
    } tPlcAla;
#pragma pack(pop)  
  
    tPlcAla *pi = (tPlcAla*)pa;
    uint08 state = pi->state;
    uint32 nopara = (state == 255)? 0 : ((state >> 1) & 0x3F);
    uint32 outsize = 6 * 4 + nopara * 4;
    
    if(pi->userdata_size > 0)
    {
      outsize += (8 + pi->userdata_size);
    }
    
    if((send == true) || ((idx + outsize) > sizeof(cbuff->data))) // falls puffer zu klein sein sollte --> paketweise senden
    {
      cbuff->datalength = ComDataHeaderSize + idx;
      Send((DsComData*)cbuff);
      idx = 4;
      *(uint32*)&cbuff->data[0] = 0; // anzahl der alarme auf 0 setzen
      send = false;
    }
  
    *(uint32*)&cbuff->data[0] += 1; // inc anzahl der alarme

    *(uint32*)&cbuff->data[idx] = pi->lasalid;                       idx += 4; // [4] ... lasalid
    *(uint32*)&cbuff->data[idx] = pi->alano;                         idx += 4; // [4] ... alano
    *(uint32*)&cbuff->data[idx] = pi->group;                         idx += 4; // [4] ... group
    
    uint32 date, time;
    decode_datetime(date, time, pi->datim);
    *(uint32*)&cbuff->data[idx] = date;                              idx += 4; // [4] ... date
    *(uint32*)&cbuff->data[idx] = time;                              idx += 4; // [4] ... time
    *(uint08*)&cbuff->data[idx] = (state == 255)? 255 : (state & 1); idx += 1; // [1] ... state
    *(uint08*)&cbuff->data[idx] = (state >> 7) & 0x01;               idx += 1; // [1] ... groupbit
    *(uint08*)&cbuff->data[idx] = (uint08)nopara;                    idx += 1; // [1] ... parano
      
    uint08 nc0 = (pi->userdata_size == 0)? 0 : 1; // ifnec set bit 0
    *(uint08*)&cbuff->data[idx] = nc0;                               idx += 1; // [1] ... 0 oder 1
      
    uint08 *ph = pi->paradat;
    dslib_memcpy(&cbuff->data[idx], ph, nopara * 4);                 idx += nopara * 4; // [ ... parameter

    if(nc0 & 1) // check Bit0
    {
      send = true; // legacy, siehe Info
      *(uint32*)&cbuff->data[idx] = pi->userdata_id;                 idx += 4; // [4] ... userdata_type
      *(uint32*)&cbuff->data[idx] = pi->userdata_size;               idx += 4; // [4] ... userdata_size
      ph = &pi->paradat[nopara * 4];
      uint32 byz = pi->userdata_size;
      dslib_memcpy(&cbuff->data[idx], ph, byz);                      idx += byz; // [ ... userdata
    }
    
    pa += pi->bytelength;
  }

  cbuff->datalength = ComDataHeaderSize + idx;

  return Send((DsComData*)cbuff);
}

bool cComAlarm::RunAlarmSub202(uint32 no)
{
  bool retcode = false;
  uint08 data[32];
  uint08 result[256];
  
  // read arbitrary entries
  // in:   [4] ... handle
  //       [4] ... startindex of entry
  //       [4] ... number of entries 
  
  *(uint16*)&data[0]   = 202; // get alarme
  *(uint32*)&data[2]   = AlarmHandle; // handle
  *(uint32*)&data[6]   = AlarmNo; // startindex
  *(uint32*)&data[10]  = no - AlarmNo; // soviele hätte ich gerne
  
  *(uint16*)&result[0] = 0; // default init
  *(uint32*)&result[2] = 0;
  *(uint32*)&result[6] = 0;

  if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
  {
    // [4] ..... index of first entry in resultbuffer
    // [4] ..... number of entries in resultbuffer + (msb is set if overflow occured)
    // *[4] .... alano
    // *[4] .... date and time
    // *[1] .... state, parano, group
    // *[4] .... group
    // *[4] .... lasalid
    // *[MAX_ALARMPARA*4] .. parameter     
      
    uint32 idx = *(uint32*)&result[2];
    uint32 nox = *(uint32*)&result[6] & 0x7FFFFFFFUL;
      
    if(nox > 0)
    {
      SendAlarm202(ID_TRANSACTION_ALL, 0xFFFFFFFF, ComCmd_AlarmRecord, idx, nox, &result[10]);
      AlarmNo = idx + nox;
      retcode = true;
    }
    else if(idx == 0)
    {
      AlarmNo = no; // Problem ist aufgetreten
    }
  }
  else
  {
    AlarmNo = no; // Problem ist aufgetreten
  }

  return retcode;
}

bool cComAlarm::RunAlarmSub203(uint32 no)
{
  bool retcode = false;
  uint08 data[32];
  uint08 *result = Result10K;

  // read arbitrary entrys
  // in:   [4] ... handle
  //       [4] ... startindex of entry
  //       [4] ... number of entries
  //       [4] ... bytesize of resultbuffer 
    
  *(uint16*)&data[0]  = 203; // get single alarm
  *(uint32*)&data[2]  = AlarmHandle; // handle
  *(uint32*)&data[6]  = AlarmNo; // startindex
  *(uint32*)&data[10] = no - AlarmNo; // soviele hätte ich gerne
  *(uint32*)&data[14] = sizeof(Result10K) - 2; // bytesize resultbuffer
  *(uint32*)&data[18] = 0; // nc, init with 0
  *(uint32*)&data[22] = 0; // nc, init with 0
  *(uint32*)&data[26] = 0; // nc, init with 0
    
  *(uint16*)&result[0] = 0; // default init
  *(uint32*)&result[2] = 0;
  *(uint32*)&result[6] = 0;
    
  if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
  {
    // out: [4] ................. index of entry in resultbuffer
    //      [4] ................. number of entries (1 oder 0) in resultbuffer + (msb is set if overflow occured)
    //       [4] ................ alano
    //       [4] ................ date and time
    //       [2] ................ bytelength of single record
    //       [1] ................ nc (init with 0)
    //       [1] ................ state (bit0), parano (bit 1-6), group (bit 7)
    //       [4] ................ group
    //       [4] ................ lasalid
    //       [parano * 4] ....... parameter
    //       [4] ................ userdata bytesize
    //       [4] ................ userdataid
    //       [ .................. userdata
  
    uint32 idx = *(uint32*)&result[2];
    uint32 nox = *(uint32*)&result[6] & 0x7FFFFFFFUL;
  
    if(nox > 0)
    {
      SendAlarm203(ID_TRANSACTION_ALL, 0xFFFFFFFF, ComCmd_AlarmRecord, idx, nox, &result[10]);
      AlarmNo = idx + nox;
      retcode = true;
    }
    else if(idx == 0)
    {
      AlarmNo = no; // Problem ist aufgetreten
    }
  }
  else
  {
    AlarmNo = no; // Problem ist aufgetreten
  }

  return retcode;
}

bool cComAlarm::RunAlarm(uint32 tabsolute)
{
  uint32 no = GetNumberOfAlarm();

  if(no != AlarmNo)
  {
    if(AlarmXBufferVersion >= 5)
    {
      RunAlarmSub203(no);  // erst ab Version 5
    }
    else
    {
      RunAlarmSub202(no);
    }
  }
  else if(ScanStartIndex != 0xFFFFFFFF)
  {
    ScanStartIndex = ScanAlarmStateRun(ID_TRANSACTION_ALL, 0xFFFFFFFF, ScanStartIndex, ScanAlarmState, ScanAlarmAddState);
  }
  else
  {
    TimexRunAlarm = tabsolute; // bremse aktivieren
  }

  return true;
}

void cComAlarm::Run(uint32 tabsolute)
{
  if(Step == 0)
  {
    uint32 client_id = 123456;
    if(pComPort != NULL)
    {
      client_id = pComPort->GetClientId();
    }
    if(Start(ID_TRANSACTION_DS, 0xFFFFFFFF, client_id, NULL) == false)
    {
      Step = 999; // error
    }
  }
  
  if(ObjLasalId != 0)
  {
    if(Step == 2)
    {
      if((tabsolute - TimexRunAlarm) > 77) // bremse
      {
        RunAlarm(tabsolute);
      }
    }
    else if(Step == 1)
    {
      RunConfig();
    }
  }
}

bool cComAlarm::QuitAlarmAll(uint16 transactionDest, uint32 messageId)
{
  // out:  [1] ... 0 = false, 1 = true
  
  uint08 data[32];
  uint08 result[256];  
  DsComData cbuff;
  
  cbuff.transactionSrc  = 0;
  cbuff.transactionDest = transactionDest;
  cbuff.messageId       = messageId;
  cbuff.id_major       = ComId_Major_DS;
  cbuff.id_minor       = ComCmd_AlarmQuitAll_Result;
    
  *(uint16*)&data[0] = 3; // quit alarm
  *(uint32*)&data[2] = AlarmHandle; // handle
  *(uint32*)&data[6] = 0xFFFFFFFF; // alle alarme quittieren
  *(uint32*)&cbuff.data[0] = DS_ERROR_UNKNOWN;
    
  if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
  {
    if(result[2] == 1)
    {
      *(uint32*)&cbuff.data[0] = DS_ERROR_NONE;
    }
  }

  cbuff.datalength = ComDataHeaderSize + 4;

  return Send(&cbuff);  
}

bool cComAlarm::QuitAlarmGroup(uint16 transactionDest, uint32 messageId, uint32 group)
{
  uint08 data[32];
  uint08 result[256];  
  DsComData cbuff;
  
  cbuff.transactionSrc  = 0;
  cbuff.transactionDest = transactionDest;
  cbuff.messageId       = messageId;
  cbuff.id_major       = ComId_Major_DS;
  cbuff.id_minor       = ComCmd_AlarmQuitGroup_Result;
    
  *(uint16*)&data[0] = 4; // quit alarmgroup
  *(uint32*)&data[2] = AlarmHandle; // handle
  *(uint32*)&data[6] = group; // alle alarme quittieren
  *(uint32*)&cbuff.data[0] = group;
  *(uint32*)&cbuff.data[4] = DS_ERROR_UNKNOWN;
    
  if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
  {
    if(result[2] == 1)
    {
      *(uint32*)&cbuff.data[4] = DS_ERROR_NONE;
    }
  }

  cbuff.datalength = ComDataHeaderSize + 8;

  return Send(&cbuff);
}

bool cComAlarm::QuitAlarm(uint16 transactionDest, uint32 messageId, uint32 no, uint32 *palaid)
{
  bool retcode = true;
  
  cComDatBuffer cd;
  cd.cTor(pComPort, transactionDest, messageId, ComCmd_AlarmQuit_Result);
  
  uint08 data[32];
  uint08 result[256];  

  *(uint16*)&data[0] = 3; // quit alarm
  *(uint32*)&data[2] = AlarmHandle; // handle
  
  while(no--)
  {
    uint32 alarmid = *palaid++;
    *(uint32*)&data[6] = alarmid; // alarmid
    uint32 error = DS_ERROR_INVALID_ALARMID;
    if(dslib_call_new_inst_direct(ObjLasalId, data, result) == 0)
    {
      if(result[2] == 1)
      {
        error = DS_ERROR_NONE;
      }
    }
      
    if(cd.AddU32U32Record(alarmid, error) == false)
    {
      retcode = false;
    }
  }
  
  if(cd.Finalize() == false)
  {
    retcode = false;
  }
  
  return retcode;
}

uint32 cComAlarm::ScanAlarmStateRun(uint16 transactionDest, uint32 messageId, uint32 startindex, uint32 alarmstate, uint32 addstate)
{
  uint08 data[32];
  bool   state = false;
  
  *(uint16*)&data[0]  = (AlarmXBufferVersion >= 5)? 208 : 206; // scan alle alarme
  *(uint32*)&data[2]  = AlarmHandle; // handle
  *(uint32*)&data[6]  = startindex;  // startindex
  *(uint32*)&data[10] = alarmstate;  // alarmstate
  *(uint32*)&data[14] = addstate;    // 0=alle eintragen, 1=nur wenn im Buffer gefunden, 2=nur wenn im buffer nicht gefunden
  *(uint32*)&data[18] = (AlarmXBufferVersion >= 5)? (sizeof(Result10K) - 2) : 0; 
  
  *(uint16*)&Result10K[0]  = 0;           // anzahl bytes
  *(uint32*)&Result10K[2]  = 0xFFFFFFFF;  // endindex 
  *(uint32*)&Result10K[6]  = 0;           // anzahl

  if(dslib_call_new_inst_direct(ObjLasalId, data, Result10K) == 0)
  {
    uint32 anzahl = *(uint32*)&Result10K[6];
    if(anzahl > 0)
    {
      startindex = *(uint32*)&Result10K[2];
      
      if(AlarmXBufferVersion >= 5)
      {
        state = SendAlarm203(transactionDest, messageId, ComCmd_AlarmRecordEx, 0, anzahl, &Result10K[10]);
      }
      else
      {
        state = SendAlarm202(transactionDest, messageId, ComCmd_AlarmRecordEx, 0, anzahl, &Result10K[10]);
      }
    }
  }
  
  return (state == true)? startindex : 0xFFFFFFFF;
}

bool cComAlarm::ScanAlarmStateStart(uint32 alarmstate, uint32 addstate)
{
  ScanAlarmState    = alarmstate;
  ScanAlarmAddState = addstate;
  ScanStartIndex    = 0;
  
  return true;
}

uint32 cComAlarm::GetAlarmHandle(void)
{
  return AlarmHandle;
}
