//<NewSigmatekCFileOptimize/>
// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 16.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "DS_cRefreshList.h"
#include "DS_cComPort.h"
#include "DS_Lib.h"

uint32 LasalIdToVarNo(uint32 lasalid)
{
	return lasalid - 10000;
}

uint32 VarNoToLasalId(uint32 varno)
{
	return varno + 10000;
}

void init_FiFoCell(FiFoCell *p)
{
	p->cmd = RefList_NOP;
	p->userid = 0;
	p->lasalid = 0;
	p->reftime = 0;
	p->datasize = 0;
}

void init_RefCell(RefCell *p)
{
	p->datasize = 0;
	p->lasalid = 0;
	p->lasttimex = 0;
	p->oldval = 0;
	p->reftime = 0;
	p->userid = 0xFFFFFFFF;
}

void init_RefListCmdStream(RefListCmdStream *p)
{
	p->cmd = 0;
	p->channel = 0;
	p->no = 0;
}

void free_RefListCmdStream(RefListCmdStream *p)
{
	init_RefListCmdStream(p);
}

cRefreshList* cRefreshList::New(cComPort *pcp, uint16 channel0)
{
  cRefreshList *retcode = NULL;
  retcode = (cRefreshList*)dslib_malloc(sizeof(cRefreshList));
  if(retcode != NULL)
  {
    retcode->cTor(pcp, channel0);
  }
  return retcode;
}

cRefreshList* cRefreshList::Delete(cRefreshList* p)
{
  if(p != NULL)
  {
    p->dTor();
    dslib_free(p);
  }
  return NULL;
}

void cRefreshList::cTor(cComPort *pcp, uint16 channel0)
{
  pComPort = pcp;
	FiFoUsed = 0;
	FiFoRd = 0;
	pCell = NULL;
	CellNo = 0;
	CellNoAllocated = 0;
	StopCounter = 0;
  Channel = channel0;
  CellUpdBuffer.no = 0;
  Count = 0;
}

void cRefreshList::dTor(void)
{
	FiFoUsed = 0;
	FiFoRd = 0;

	dslib_memory((void**)&pCell, 0);
	CellNo = 0;
	CellNoAllocated = 0;
	StopCounter = 0;
}

void cRefreshList::SectionBegin(void)
{
}

void cRefreshList::SectionEnd(void)
{
}

bool cRefreshList::DoCell(uint32 no)
{
	uint32 diff = CellNoAllocated - CellNo;

	if (no > diff)
	{
		uint32 blockno = (CellNo + no + RefCellGrowBy - 1) / RefCellGrowBy;
		uint32 no = blockno * RefCellGrowBy;
		if (dslib_memory((void**)&pCell, no * sizeof(RefCell)) == true)
		{
			//uint32 diff = no - CellNoAllocated;
			//RefCell *ph = &pCell[CellNoAllocated];
			//while (diff--)
			//{
			//	init_RefCell(ph);
			//}
			CellNoAllocated = no;
			return true;
		}
		return false;
	}

	return true;
}

bool cRefreshList::Add(uint32 lasalid, uint32 userid, uint32 reftime, uint32 datasize)
{
	if (DoCell(1) == true)
	{
		RefCell *pd = &pCell[CellNo];
		dslib_atomic_incU32(&CellNo);

		init_RefCell(pd);
		pd->lasalid = lasalid;
		pd->userid = userid;
		pd->reftime = reftime;
		pd->datasize = datasize;
		return true;
	}
	return false;
}

uint32 cRefreshList::SeekCell(uint32 userid)
{
	RefCell *ph = pCell;
	for (uint32 i = 0; i < CellNo; i++)
	{
		if (ph->userid == userid)
		{
			return i;
		}
		ph++;
	}
	return 0xFFFFFFFF;
}

bool cRefreshList::Sub(uint32 userid)
{
	uint32 idx = SeekCell(userid);
	if (idx < CellNo)
	{
		if ((idx + 1) < CellNo)
		{
			dslib_memcpy(&pCell[idx], &pCell[CellNo - 1], sizeof(RefCell));
		}
		CellNo--;
		return true;
	}
	return false;
}

void cRefreshList::Empty(void)
{
	CellNo = 0;
}

FiFoCell *cRefreshList::GetFifoCell(void)
{
	if (FiFoUsed < FiFoCellNo) // wenn noch platz im puffer ist
	{
		uint32 idx = FiFoRd + FiFoUsed;
		if (idx >= FiFoCellNo)
		{
			idx -= FiFoCellNo;
		}

		return &FiFoData[idx];
	}

	dslib_system_message("DS-ERROR: cRefreshList::Fifo too small");
	return NULL;
}

bool cRefreshList::AddCommand(RefListCmdStream *p)
{
	uint32 no;
	uint32 *ps;

	SectionBegin();

	switch (p->cmd)
	{
	case RefList_ADD:
		// [1] ..... command
		// [1] ..... channel
		// [2] ..... anzahl
		// *[4] .... lasalid
		// *[4] .... userid
		// *[4] .... reftime
		// *[4] .... datasize

		no = p->no;
		ps = p->data;
		while (no--)
		{
			FiFoCell *pfc = GetFifoCell();
			if (pfc != NULL)
			{
				init_FiFoCell(pfc);
				pfc->cmd = RefList_ADD;
				pfc->lasalid = *ps++;
				pfc->userid = *ps++;
				pfc->reftime = *ps++;
				pfc->datasize = *ps++;
				FiFoUsed++;
			}
		}
		break;

	case RefList_SUB:
		// [1] ..... command
		// [1] ..... channel
		// [2] ..... anzahl
		// *[4] .... userid

		no = p->no;
		ps = p->data;
		while (no--)
		{
			FiFoCell *pfc = GetFifoCell();
			if (pfc != NULL)
			{
				init_FiFoCell(pfc);
				pfc->cmd = RefList_SUB;
				pfc->userid = *ps++;
				FiFoUsed++;
			}
		}
		break;

	case RefList_EMPTY:
	case RefList_STARTUPDATE:
	case RefList_STOPUPDATE:
		FiFoCell *pfc = GetFifoCell();
		if (pfc != NULL)
		{
			init_FiFoCell(pfc);
			pfc->cmd = p->cmd;
      FiFoUsed++;
		}
		break;
	}
	SectionEnd();

	return true;
}

void cRefreshList::DoFiFo(void)
{
	SectionBegin();
	uint32 startidx = 0xFFFFFFFF;
	while (FiFoUsed > 0)
	{
		if (FiFoRd >= FiFoCellNo)
		{
			FiFoRd = 0;
		}
		FiFoCell *prec = &FiFoData[FiFoRd];

		switch (prec->cmd)
		{
		case RefList_ADD: // einzelne cell hinzufügen
			if (startidx == 0xFFFFFFFF)
			{
				startidx = CellNo;
			}
			Add(prec->lasalid, prec->userid, prec->reftime, prec->datasize);
			break;

		case RefList_SUB: // einzelne cell entfernen
			Sub(prec->userid);
			break;

		case RefList_EMPTY: // gesamte liste leeren
			Empty();
			break;

		case RefList_STARTUPDATE: // update wieder starten
			if (StopCounter > 0)
			{
				StopCounter--;
			}
			break;

		case RefList_STOPUPDATE: // update aussetzen
			StopCounter++;
			break;

		}

		FiFoRd++; // readposition erst nach berarbeitung von record incrementieren

		dslib_atomic_decU32(&FiFoUsed); // anzahl der records nach verarbeitung decrementieren
	}

	if (startidx < CellNo)
	{
		CellIndexer = startidx;
	}
	SectionEnd();
}

bool cRefreshList::ReadClient(uint32 *pval, uint32 lasalid)
{
  *pval = dslib_read_client_data(lasalid);
  return true;
}

bool cRefreshList::ReadServer(uint32 *pval, uint32 lasalid)
{
  *pval = dslib_call_read_method(lasalid);
  return true;
}

bool cRefreshList::ReadStringCrc(uint32 *pval, uint32 lasalid)
{
  *pval = dslib_get_string_crc(lasalid);
  return true;
}

bool cRefreshList::ReadDataCrc(uint32 *pval, uint32 lasalid, uint32 datasize)
{
  *pval = dslib_get_dataat_crc(lasalid, datasize);
	return true;
}

uint32 cRefreshList::RunCell(RefCell *pcell)
{
	bool state = false;
	uint32 att = pcell->reftime & 0xFF000000;
	uint32 value;
  
	if (att == RefTimeId_ServerNum) // server auslesen
	{
		state = ReadServer(&value, pcell->lasalid);
	}
	else if (att == RefTimeId_ServerString) // crc vom string auslesen
	{
		state = ReadStringCrc(&value, pcell->lasalid);
	}
	else if (att == RefTimeId_DataGlobal) // crc oder value von globaler variable auslesen
	{
		state = ReadDataCrc(&value, pcell->lasalid, pcell->datasize);
	}
  else if (att == RefTimeId_ClientNum)
  {
    state = ReadClient(&value, pcell->lasalid);
  }
  else if (att == RefTimeId_SysVariable)
  {
    if(pComPort != NULL)
    {
      state = pComPort->GetSysVar(&value, pcell->userid);
    }
  }
  
	if (state == true)
	{
		if ((pcell->oldval != value) || (pcell->lasttimex == 0))
		{
			pcell->oldval = value;
			return att; // nur bei änderung liefert diese methode einen wert !=0
		}
	}
  
	return 0xFFFFFFFF; // keine änderung oder wert nicht erreichbar
}

void cRefreshList::UpdateU32(DsComData *pcd, uint32 no)
{
  if(pComPort != NULL)
  {
	  pComPort->SendComCmdRefListUpdate(pcd, no, Channel);
  }
}

void cRefreshList::UpdateStr(UserIdLasalId *p, uint32 no)
{
  if(pComPort != NULL)
  {
    pComPort->SendComCmdRefListUpdateString(p, no, Channel);
  }
}


void cRefreshList::Run(void)
{
	if (FiFoUsed != 0) // bearbeite fifo falls cmd's enthalten sind
	{
		DoFiFo();
	}

	if (StopCounter == 0)
	{
		uint32        timex = dslib_tabsolute();
		uint32        no = (CellNo < RefCellsPerCycle) ? CellNo : RefCellsPerCycle;
    UserIdLasalId updstr[32];
    uint32        updstr_no = 0;

//    no = CellNo;

		while (no--)
		{
			if (CellIndexer >= CellNo)
			{
				CellIndexer = 0;
			}

			RefCell *pcell = &pCell[CellIndexer];
			if (pcell->lasalid != 0) // nur wenn zelle gültig
			{
        bool doit = false;
        if(pcell->lasttimex == 0)
        {
          doit = true;
          Count = 100; // datenpaket in diesem durchlauf senden, update soll bei neuer zelle so schnell wie möglich passieren
        }
        else if((timex - pcell->lasttimex) >= (pcell->reftime & 0x00FFFFFF))
        {
          doit = true;
        }
      
//				if ((pcell->lasttimex == 0) || (timex - pcell->lasttimex) >= (pcell->reftime & 0x00FFFFFF))
        if(doit == true)
				{
          switch(RunCell(pcell))
          {
            case RefTimeId_ClientNum : // ClientValue changed
            case RefTimeId_ServerNum : // ServerValue changed
            case RefTimeId_DataGlobal : // Data changed
            case RefTimeId_SysVariable : // systemvariable changed
              if(CellUpdBuffer.no >= ARRAY_SIZE(CellUpdBuffer.cell))
              {
			          UpdateU32((DsComData*)&CellUpdBuffer, CellUpdBuffer.no); // wertänderungen updaten
                CellUpdBuffer.no = 0;
                Count = 0;
              }
						  CellUpdBuffer.cell[CellUpdBuffer.no].userid = pcell->userid;
						  CellUpdBuffer.cell[CellUpdBuffer.no].value  = pcell->oldval;
						  CellUpdBuffer.no++;
              break;
              
            case RefTimeId_ServerString : // stringserver has changed
              if(updstr_no >= ARRAY_SIZE(updstr))
              {
                UpdateStr(updstr, updstr_no);
                updstr_no = 0;
              }
              updstr[updstr_no].userid = pcell->userid;
              updstr[updstr_no].lasalid = pcell->lasalid;
              updstr_no ++;
              break;
            
          }

					pcell->lasttimex = timex;
				}
			}

			CellIndexer++;
		}

    Count ++;
    if(Count > 4)
    {
      Count = 0;
      if(CellUpdBuffer.no > 0)
      {
        UpdateU32((DsComData*)&CellUpdBuffer, CellUpdBuffer.no); // wertänderungen updaten
        CellUpdBuffer.no = 0;
      }
    }
    
    if(updstr_no > 0)
    {
      UpdateStr(updstr, updstr_no); // stringänderung updaten
    }
	}
}


/*
void cRefreshList::Run(void)
{
	if (FiFoUsed != 0) // bearbeite fifo falls cmd's enthalten sind
	{
		DoFiFo();
	}

	if (StopCounter == 0)
	{
		uint32        timex = lib_tabsolute();
		uint32        no = (CellNo < RefCellsPerCycle) ? CellNo : RefCellsPerCycle;
    DsComData       updu32;
    UpdateCellU32 *pupdu32 = (UpdateCellU32*)&updu32.data[4];
		uint32        updu32_no = 0;
    UserIdLasalId updstr[32];
    uint32        updstr_no = 0;

//    no = CellNo;

		while (no--)
		{
			if (CellIndexer >= CellNo)
			{
				CellIndexer = 0;
			}

			RefCell *pcell = &pCell[CellIndexer];
			if (pcell->lasalid != 0) // nur wenn zelle gültig
			{
				if ((pcell->lasttimex == 0) || (timex - pcell->lasttimex) >= (pcell->reftime & 0x00FFFFFF))
				{
          switch(RunCell(pcell))
          {
            case RefTimeId_ClientNum : // ClientValue changed
            case RefTimeId_ServerNum : // ServerValue changed
            case RefTimeId_DataGlobal : // Data changed
            case RefTimeId_SysVariable : // systemvariable changed
              if(updu32_no >= ((sizeof(updu32.data) - 4) / sizeof(UpdateCellU32)))
              {
			          UpdateU32(&updu32, updu32_no); // wertänderungen updaten
                updu32_no = 0;
              }
						  pupdu32[updu32_no].userid = pcell->userid;
						  pupdu32[updu32_no].value  = pcell->oldval;
						  updu32_no++;
              break;
              
            case RefTimeId_ServerString : // stringserver has changed
              if(updstr_no >= ARRAY_SIZE(updstr))
              {
                UpdateStr(updstr, updstr_no);
                updstr_no = 0;
              }
              updstr[updstr_no].userid = pcell->userid;
              updstr[updstr_no].lasalid = pcell->lasalid;
              updstr_no ++;
              break;
            
          }

					pcell->lasttimex = timex;
				}
			}

			CellIndexer++;
		}

		if(updu32_no > 0)
		{
			UpdateU32(&updu32, updu32_no); // wertänderungen updaten
		}
    
    if(updstr_no > 0)
    {
      UpdateStr(updstr, updstr_no); // stringänderung updaten
    }
	}
}
*/


uint32 cRefreshList::GetCellNo(void)
{
  return CellNo;
}


// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************


cRefreshListEx* cRefreshListEx::New(cComPort *pcp)
{
  cRefreshListEx *retcode = NULL;
  retcode = (cRefreshListEx*)dslib_malloc(sizeof(cRefreshListEx));
  if(retcode != NULL)
  {
    retcode->cTor(pcp);
  }
  return retcode;
}

cRefreshListEx* cRefreshListEx::Delete(cRefreshListEx* p)
{
  if(p != NULL)
  {
    p->dTor();
    dslib_free(p);
  }
  return NULL;
}

void cRefreshListEx::cTor(cComPort *pcp)
{
  pComPort = pcp;
	uint32 nox = ARRAY_SIZE(Ptr); //sizeof(Ptr) / sizeof(Ptr[0]);
	while (nox--)
	{
		Ptr[nox] = NULL;
	}
}

void cRefreshListEx::dTor()
{
	uint32 nox = ARRAY_SIZE(Ptr); //sizeof(Ptr) / sizeof(Ptr[0]);
	while (nox--)
	{
		if (Ptr[nox] != NULL)
		{
			Ptr[nox] = cRefreshList::Delete(Ptr[nox]);
		}
	}
}

bool cRefreshListEx::AddCommand(RefListCmdStream *p)
{
	uint32 channel = p->channel;
  
  if(p->cmd == RefList_EMPTY) // refliste leeren
  {
    if(p->channel >= 0xFF)
    {
      for(uint08 ch = 0; ch < ARRAY_SIZE(Ptr); ch++)
      {
        if (Ptr[ch] != NULL)
        {
          p->channel = ch; 
          Ptr[ch]->AddCommand(p);
        }
      }
      return true;
    }
    else
    {
      if (channel < ARRAY_SIZE(Ptr)) // (sizeof(Ptr) / sizeof(Ptr[0])))
      {
        if (Ptr[channel] != NULL)
        {
          return Ptr[channel]->AddCommand(p);
        }
      }
    }
  }
	else if (channel < ARRAY_SIZE(Ptr)) // (sizeof(Ptr) / sizeof(Ptr[0])))
	{
		if (Ptr[channel] == NULL)
		{
			Ptr[channel] = cRefreshList::New(pComPort, channel);
		}

		if (Ptr[channel] != NULL)
		{
			return Ptr[channel]->AddCommand(p);
		}
	}
	return false;
}

void cRefreshListEx::Run(void)
{
	uint32 nox = ARRAY_SIZE(Ptr); //(sizeof(Ptr) / sizeof(Ptr[0]));
	while (nox--)
	{
		cRefreshList *prl = Ptr[nox];
		if (prl != NULL)
		{
			prl->Run();
		}
	}
}

uint32 cRefreshListEx::GetCellNo(uint32 channel)
{
  if(channel < ARRAY_SIZE(Ptr)) //(sizeof(Ptr) / sizeof(Ptr[0])))
  {
    if(Ptr[channel] != NULL)
    {
      return Ptr[channel]->GetCellNo();
    }
  }
  return 0;
}

uint32 cRefreshListEx::GetCellInfo(ChannelNumber *pcn, uint32 maxno, uint32 channel)
{
  uint32 nox = ARRAY_SIZE(Ptr);
  
  if(channel < nox)
  {
    if(maxno > 0)
    {
      pcn->channel = (uint16)channel;
      pcn->no = (uint16)GetCellNo(channel);
      return 1;
    }
    return 0;
  }

  uint32 retcode = 0;
  for(uint32 i=0; i<nox; i++)
  {
    if(maxno > 0)
    {
      maxno--;
      pcn->channel = (uint16)i;
      pcn->no = (uint16)GetCellNo(i);
      pcn++;
      retcode ++;
    }
  }
  
  return retcode;
}



