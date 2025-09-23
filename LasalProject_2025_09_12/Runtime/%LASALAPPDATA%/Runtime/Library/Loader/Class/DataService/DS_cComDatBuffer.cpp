// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 08.05.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "DS_cComDatBuffer.h"
#include "DS_cComPort.h"
#include "DS_Lib.h"

void cComDatBuffer::cTor(cComPort *pcp, uint16 transactionDest, uint32 messageId, uint16 cmd)
{
	Init(pcp, transactionDest, messageId, cmd);
}

void cComDatBuffer::Init(cComPort *pcp, uint16 transactionDest, uint32 messageId, uint16 cmd)
{
	PCp = pcp;
	ComDat.transactionSrc = 0;
    ComDat.transactionDest = transactionDest;
	ComDat.id_major = ComId_Major_DS;
	ComDat.id_minor = cmd;
    ComDat.messageId = messageId;
	ComDat.datalength = ComDataHeaderSize + 4;
	*(uint32*)&ComDat.data[0] = 0;
}

uint32 cComDatBuffer::GetBytesFree(void)
{
	return (sizeof(ComDat.data) - ComDat.datalength);
}

bool cComDatBuffer::Finalize(void)
{
	if (*(uint32*)&ComDat.data[0] != 0)
	{
		if (PCp->Send(&ComDat) == true)
		{
			Init(PCp, ComDat.transactionDest, ComDat.messageId, ComDat.id_minor);
			return true;
		}
		return false;
	}
	return true;
}

void cComDatBuffer::UserCntInc(void)
{
  *(uint32*)&ComDat.data[0] = (*(uint32*)&ComDat.data[0]) + 1;
}

bool cComDatBuffer::AddU32Record(uint32 u32)
{
	if (GetBytesFree() < 4)
	{
		Finalize();
	}

	if (GetBytesFree() >= 4)
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = u32;
		ComDat.datalength += 4;
		UserCntInc();
		return true;
	}
	return false;
}

bool cComDatBuffer::AddU32U32Record(uint32 u32_0, uint32 u32_1)
{
	// [4]...... u32_0  
	// [4]...... u32_1
	// usercount++

	if (GetBytesFree() < 8)
	{
		Finalize();
	}

	if (GetBytesFree() >= 8)
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = u32_0;
		*(uint32*)&ph[4] = u32_1;
		ComDat.datalength += 8;
		UserCntInc();
		return true;
	}
	return false;
}

bool cComDatBuffer::AddU32U32U32Record(uint32 u32_0, uint32 u32_1, uint32 u32_2)
{
	// [4]...... u32_0 
	// [4]...... u32_1
	// [4]...... u32_2
	// usercount++

	if (GetBytesFree() < 12)
	{
		Finalize();
	}

	if (GetBytesFree() >= 12)
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = u32_0;
		*(uint32*)&ph[4] = u32_1;
		*(uint32*)&ph[8] = u32_2;
		ComDat.datalength += 12;
		UserCntInc();
		return true;
	}
	return false;
}

bool cComDatBuffer::AddU32U32U32U32Record(uint32 u32_0, uint32 u32_1, uint32 u32_2, uint32 u32_3)
{
	// [4]...... u32_0 
	// [4]...... u32_1
	// [4]...... u32_2
	// [4]...... u32_3
	// usercount++

	if (GetBytesFree() < 16)
	{
		Finalize();
	}

	if (GetBytesFree() >= 16)
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = u32_0;
		*(uint32*)&ph[4] = u32_1;
		*(uint32*)&ph[8] = u32_2;
		*(uint32*)&ph[12] = u32_2;
		ComDat.datalength += 16;
		UserCntInc();
		return true;
	}
	return false;
}

bool cComDatBuffer::AddU32DataU08Record(uint32 userid, uint32 datalen, uint08 u08, uint08 *data)
{
	// [4]...... userid 
  // [4] ..... datalen
	// [1]...... u08_2
  // [........ data
	// usercount++

  uint32 len = 9+datalen;

	if (GetBytesFree() < len)
	{
		Finalize();
	}

	if (GetBytesFree() >= len)
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = userid;
		*(uint32*)&ph[4] = datalen;
		*(uint08*)&ph[8] = u08;
		dslib_memcpy(&ph[9], data, datalen);
    
		ComDat.datalength += len;
		UserCntInc();
		return true;
	}
	return false;
}

bool cComDatBuffer::AddU32U32U08Record(uint32 u32_0, uint32 u32_1, uint08 u08_2)
{
	// [4]...... u32_0 
	// [4]...... u32_1
	// [1]...... u08_2
	// usercount++

	if (GetBytesFree() < 9)
	{
		Finalize();
	}

	if (GetBytesFree() >= 9)
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = u32_0;
		*(uint32*)&ph[4] = u32_1;
		*(uint08*)&ph[8] = u08_2;
		ComDat.datalength += 9;
		UserCntInc();
		return true;
	}
	return false;
}

bool cComDatBuffer::AddAsciiRecord(uint32 userid, const ascii *text)
{
	// [4]...... userid  
	// [........ ASCII-0-String
	// usercount++

	bool retcode = true;
	ascii tmptxt;

	if (text == NULL)
	{
		tmptxt = 0;
		text = &tmptxt;
	}

	uint32 size = dslib_strlen(text) + 1;

	if (GetBytesFree() < (size + 4))
	{
		Finalize();
	}

	if (GetBytesFree() >= (size + 4))
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = userid;
		dslib_memcpy(&ph[4], text, size);
		ComDat.datalength += (size + 4);
		UserCntInc();
	}
	else
	{
		retcode = false;
		DsComData *phcd = (DsComData*)dslib_malloc(size + 4 + 4 + ComDataHeaderSize);
		if (phcd != NULL)
		{
			dslib_memcpy(phcd, &ComDat, ComDataHeaderSize);
			uint08 *ph = phcd->data;
			*(uint32*)&ph[0] = 1;
			*(uint32*)&ph[4] = userid;
			dslib_memcpy(&ph[8], text, size);
			phcd->datalength = ComDataHeaderSize + 4 + 4 + size;
			retcode = PCp->Send(phcd);
			dslib_free(phcd);
		}
	}

	return retcode;
}

bool cComDatBuffer::AddStrRecord(uint32 userid, uint32 chrsize, void *text, uint16 channel)
{
	// [4]...... userid  
	// [1] ..... chrsize
	// [........ ASCII-0-String oder UNI-0-String
	// usercount++

	bool retcode = true;
	uint16 tmptxt;

	if (text == NULL)
	{
		tmptxt = 0;
		text = &tmptxt;
	}

  chrsize = (chrsize == 1)? 1 : 2; // ifnec correct chrsize 
  uint32 size = dslib_strsize((const void*)text, chrsize);

	if (GetBytesFree() < (size + 5))
	{
		Finalize();
	}

	if (GetBytesFree() >= (size + 5))
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = userid;
		*(uint08*)&ph[4] = (uint08)chrsize;
		dslib_memcpy(&ph[5], text, size);
		ComDat.datalength += (size + 5);
		UserCntInc();
    if(channel < 0xFFFF)
    {
      *(uint16*)&ComDat.data[2] = channel;
    }
	}
	else
	{
		retcode = false;
		DsComData *phcd = (DsComData*)dslib_malloc(size + 5 + 4 + ComDataHeaderSize);
		if (phcd != NULL)
		{
			dslib_memcpy(phcd, &ComDat, ComDataHeaderSize);
			uint08 *ph = phcd->data;
			*(uint32*)&ph[0] = 1;
      
      if(channel < 0xFFFF)
      {
        *(uint16*)&ph[2] = channel;
      }
      
			*(uint32*)&ph[4] = userid;
			*(uint08*)&ph[8] = (uint08)chrsize;
			dslib_memcpy(&ph[9], text, size);
			phcd->datalength = ComDataHeaderSize + 4 + 5 + size;
			retcode = PCp->Send(phcd);
			dslib_free(phcd);
		}
	}

	return retcode;
}

bool cComDatBuffer::AddStrStateRecord(uint32 userid, uint32 chrsize, void *text, bool state)
{
	// [4] ..... userid  
  // [1] ..... state
	// [1] ..... chrsize
	// [........ ASCII-0-String oder UNI-0-String
	// usercount++

	bool retcode = true;
	uint16 tmptxt;

	if (text == NULL)
	{
		tmptxt = 0;
		text = &tmptxt;
	}

  chrsize = (chrsize == 1)? 1 : 2; // ifnec correct chrsize 
  uint32 size = dslib_strsize((const void*)text, chrsize);

	if (GetBytesFree() < (size + 6))
	{
		Finalize();
	}

	if (GetBytesFree() >= (size + 6))
	{
		uint08 *ph = &ComDat.data[ComDat.datalength - ComDataHeaderSize];
		*(uint32*)&ph[0] = userid;
		*(uint08*)&ph[4] = (state == true)? 1 : 0;
		*(uint08*)&ph[5] = (uint08)chrsize;
		dslib_memcpy(&ph[6], text, size);
		ComDat.datalength += (size + 6);
		UserCntInc();
	}
	else
	{
		retcode = false;
		DsComData *phcd = (DsComData*)dslib_malloc(size + 6 + 4 + ComDataHeaderSize);
		if (phcd != NULL)
		{
			dslib_memcpy(phcd, &ComDat, ComDataHeaderSize);
			uint08 *ph = phcd->data;
			*(uint32*)&ph[0] = 1;
			*(uint32*)&ph[4] = userid;
			*(uint08*)&ph[8] = (state == true)? 1 : 0;
			*(uint08*)&ph[9] = (uint08)chrsize;
			dslib_memcpy(&ph[10], text, size);
			phcd->datalength = ComDataHeaderSize + 4 + 6 + size;
			retcode = PCp->Send(phcd);
			dslib_free(phcd);
		}
	}

	return retcode;
}

