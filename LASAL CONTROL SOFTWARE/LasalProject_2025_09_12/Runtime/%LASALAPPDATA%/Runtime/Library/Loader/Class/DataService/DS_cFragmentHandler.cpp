// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] frirob                                                                       |
// +-[        date ] 10.07.2019                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "DS_cFragmentHandler.h"
#include "DS_Lib.h"
#include "DS_cComPort.h"

/*
//-----------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------  FRAGMENT  --------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------

Fragment* Fragment::cTor(uint64 originalSize)
{
  if(originalSize < MAX_ALLOCABLE_NUMBER)
  {
    Fragment* f = (Fragment*)dslib_malloc(sizeof(Fragment));
    
    if(f != NULL)
    {
      f->originalSize = originalSize; 
      f->currentSize = 0;
      f->timestamp = dslib_tabsolute();
      
      f->pcd = (DsComData*)dslib_malloc((uint32)originalSize);
      if(f->pcd == NULL)
      {
         dslib_free(f);
         f= NULL;
      }
    }
     
    return f;
  }
  
  return NULL;
}

Fragment* Fragment::dTor(Fragment* f)
{
  if(f != NULL)
  {
    if(f->pcd != NULL)
    {
      dslib_free(f->pcd);
    }
    
    dslib_free(f);
    f= NULL;
  }
  
  return f;
}

bool Fragment::addData(uint64 offset, uint32 length, uint08* data)
{
  bool retCode = false;
  
  if(pcd != NULL)
  {
    if(originalSize >= (offset + length))
    {
      dslib_memcpy(((uint08*)pcd) + offset, data, length);
      timestamp = dslib_tabsolute();
      currentSize += length;
      if(currentSize == originalSize)
      {
        timestamp -= StandardTimeout_ms; //remove the object next time checkTime is called
        retCode = true; 
      }
    }
  }
  
  return retCode; 
}

bool Fragment::checkTime(uint32 t) 
{
  return ((t - timestamp) > StandardTimeout_ms);
}

DsComData* Fragment::GetComData()
{
  return pcd; 
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------  FRAGMENT LIST ENTRY  ------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------

FragmentListEntry* FragmentListEntry::cTor(uint32 id, uint16 srcId, uint64 originalSize, cComPort* port)
{
  FragmentListEntry* retcode = (FragmentListEntry*)dslib_malloc(sizeof(FragmentListEntry));
  if(retcode != NULL)
  {
    retcode->fragment = Fragment::cTor(originalSize);
    
    if(retcode->fragment == NULL)
    {
      dslib_free(retcode);
      retcode = NULL;
      
      return retcode;
    }
    
    retcode->id = id;
    retcode->srcId = srcId;
    retcode->port = port; 
    
    retcode->next = NULL;
   }
  
  return retcode;
}

FragmentListEntry* FragmentListEntry::dTor(FragmentListEntry* p)
{
  if(p != NULL)
  {
      Fragment::dTor(p->fragment);
      if(p->next!= NULL)
      {
        FragmentListEntry::dTor(p->next);
      }
      dslib_free(p);
      p = NULL;
  }  
  
  return p;
}

FragmentListEntry* FragmentListEntry::get(uint32 id, uint16 srcId, cComPort* port)
{
  FragmentListEntry* temp = this;
  while(temp != NULL) 
  {
    if((temp->id == id) && (temp->srcId == srcId) && (temp->port == port))
    {
      return temp;
    }
    temp = temp->getNext();
  }
  return NULL;
}

FragmentListEntry* FragmentListEntry::getNext()
{
  return next;
}

void FragmentListEntry::setNext(FragmentListEntry* f)
{
  next = f; 
}

bool FragmentListEntry::checkTime(uint32 t) 
{
  return fragment->checkTime(t); 
}

void FragmentListEntry::addData(uint64 offset, uint32 length, uint08* data, uint32 tabsolut) 
{
   if(fragment->addData(offset, length, data))
   { 
      DsComData* pcd = fragment->GetComData();
      if(pcd != NULL)
      {
        port->DataReciever(pcd, tabsolut); 
      }
   }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------  FRAGMENT Handler  -------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------

FragmentHandler* FragmentHandler::cTor()
{
  FragmentHandler* fh = (FragmentHandler*)dslib_malloc(sizeof(FragmentHandler));
  
  if(fh != NULL)
  { 
    fh->list = NULL;
  }
  
  return fh;
}

FragmentHandler* FragmentHandler::dTor(FragmentHandler* fh)
{
  if(fh != NULL)
  {
    if(fh->list != NULL)
    {
      fh->list = FragmentListEntry::dTor(fh->list);
    }

    dslib_free(fh);
    fh = NULL;
  }
  
  return fh;   
}


bool FragmentHandler::addData(DsComData* pcd, cComPort* port, uint32 tabsolut)
{
  bool retcode = false; 
  
  if(port != NULL)
  {
    if(pcd != NULL)
    {
      uint32 id = *(uint32*)&pcd->data[0];
      uint64 origDataSize = *(uint64*)&pcd->data[FRAGMENTHEADER_OFFSET_ORIGINALSIZE];
      
      if(origDataSize <= MAX_PACKAGE_SIZE)
      {    
        FragmentListEntry* f = NULL;
      
        if(list != NULL)
        {
          f = list->get(id, pcd->transactionSrc, port);
        } 
        
        if(f == NULL)
        {
          f = FragmentListEntry::cTor(id, pcd->transactionSrc, (uint32)origDataSize, port);
          
          if(f != NULL) 
          {
            if(list == NULL) 
            {
              list = f;
            } 
            else 
            {
              FragmentListEntry* temp = list; 
              while(temp->getNext() != NULL)
              {
                temp = temp->getNext(); 
              }
              temp->setNext(f); 
            }
    
          }
          else
          {
            dslib_system_message("DS-ERROR: Couldn't allocat Memory for FragmentListEntry!");
            retcode = false;
          }
          
          if(retcode != false)
          {
            f->addData(*(uint64*)&pcd->data[FRAGMENTHEADER_OFFSET_DATAOFFSET], *(uint32*)&pcd->data[FRAGMENTHEADER_OFFSET_DATALENGTH], &pcd->data[FRAGMENTHEADER_SIZE], tabsolut);
            retcode = true;
          }
        }
      } 
      else
      {
        dslib_system_message("DS-ERROR: Fragment Package too big!");
        retcode = false;
      }
    }
  }  
  
  return retcode;
}

void FragmentHandler::checkTime(uint32 t)
{
  if(list != NULL)
  {

    FragmentListEntry* temp = list;
    FragmentListEntry* prev = NULL;
    
    while(temp != NULL)
    {
      if(temp->checkTime(t) == false) 
      { 
        if(prev != NULL) 
        {
          prev->setNext(temp->getNext());
          temp->setNext(NULL);
          FragmentListEntry::dTor(temp); 
          temp = prev->getNext(); 
        } 
        else 
        {
          list = temp->getNext();
          temp->setNext(NULL);
          FragmentListEntry::dTor(temp); 
          temp = list; 
        }
      }
      else
      {
        prev = temp;
        temp = temp->getNext();
      }
    }
  }
}
*/

// Otto

static void init_tFragment(tFragment *p)
{
  p->ptr            = NULL;
  p->bytesize_alloc = 0;
  p->bytesize_rest  = 0;
  p->fragment_id    = 0;
  p->timex          = 0;
}

static void free_tFragment(tFragment *p)
{
  if(p->ptr != NULL)
  {
    dslib_free(p->ptr);
  }
  init_tFragment(p);
}
    
void cFragmentManager::cTor(void)
{
  Usage = 0;
  uint32 nox = ARRAY_SIZE(Data);
  while(nox--)
  {
    init_tFragment(&Data[nox]);
  }
}

void cFragmentManager::dTor(void)
{
  uint32 nox = ARRAY_SIZE(Data);
  while(nox--)
  {
    free_tFragment(&Data[nox]);
  }
  Usage = 0;
}

bool cFragmentManager::AddSub(cComPort *pcp, tFragment *p, tFragmentHeader *pfh, uint32 tabsolute)
{
  if((pfh->offset_actual + pfh->bytesize_actual) <= pfh->bytesize_gesamt) // ist enthaltene Datenmenge konsistent
  {
    if(pfh->bytesize_gesamt > p->bytesize_alloc) // alloc / realloc notwendig
    {
      if(dslib_memory((void**)&p->ptr, pfh->bytesize_gesamt) != 0)
      {
        p->bytesize_alloc = pfh->bytesize_gesamt;
        p->fragment_id    = pfh->fragment_id ^ 0xCAFEBABE; // id zerstören
      }
    }
  
    if(pfh->bytesize_gesamt <= p->bytesize_alloc) // passt paket auch in allokierten speicher
    {
      if((pfh->offset_actual + pfh->bytesize_actual) <= p->bytesize_alloc) // ist offset korrekt
      {
        if(pfh->fragment_id != p->fragment_id) // neues paket ?
        {
          p->fragment_id   = pfh->fragment_id;
          p->bytesize_rest = pfh->bytesize_gesamt;
        }

        if(pfh->bytesize_actual <= p->bytesize_rest)
        {
          dslib_memcpy(&p->ptr[pfh->offset_actual], pfh+1, pfh->bytesize_actual);
          p->bytesize_rest -= pfh->bytesize_actual;
          p->timex = dslib_tabsolute();
          
          if(p->bytesize_rest == 0)
          {
            pcp->DataReciever((DsComData*)p->ptr, tabsolute);
            free_tFragment(p);
            Usage--;
          }
          return true;
        }
      }
    }
  }

  return false;
}

bool cFragmentManager::Add(cComPort *pcp, DsComData* pcd, uint32 tabsolute)
{
  if(pcd->datalength >= (ComDataHeaderSize + sizeof(tFragmentHeader)))
  {
    tFragmentHeader *pfh = (tFragmentHeader*)pcd->data;
  
    uint32 nox = ARRAY_SIZE(Data);
    while(nox--) // suche Bestehenden
    {
      if((Data[nox].fragment_id == pfh->fragment_id) && (Data[nox].ptr != NULL))
      {
        return AddSub(pcp, &Data[nox], pfh, tabsolute);
      }
    }
    
    nox = ARRAY_SIZE(Data);
    while(nox--) // suche ersten Freien
    {
      if(Data[nox].ptr == NULL)
      {
        Usage ++;
        return AddSub(pcp, &Data[nox], pfh, tabsolute);
      }
    }
  }
  
  return false;
}

uint32 cFragmentManager::CheckSub(tFragment *p, uint32 tabsolute)
{
  if(p->ptr != NULL)
  {
    if((tabsolute - p->timex) < 7000) // nur wenn innerhalb der letzten 7 Sekunden eine Aktivität
    {
      return 1;
    }
    free_tFragment(p); // freigeben, weil keine Aktivität ...
  }
  
  return 0;
}

void cFragmentManager::Check(uint32 tabsolute)
{
  if(Usage != 0)
  {
    Usage = 0;
    uint32 nox = ARRAY_SIZE(Data);
    while(nox--)
    {
      Usage += CheckSub(&Data[nox], tabsolute);
    }
  }
}
