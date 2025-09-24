//<NewSigmatekCFileOptimize/>
// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 28.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "lsl_c_ifssr.h"
#include "RTOS_C_interfaces.h"
#include "DS_Lib.h"
#include "lsl_c_mt.h"

static LSL_MT_TYPE *Cil_LSL_MULTITASK = NULL;

// check size of types ****************************************************************************
#define STATIC_ASSERT3(x, line) typedef char check_condition_line_ ## line[(x) ? 1 : -1];
#define STATIC_ASSERT2(x, line) STATIC_ASSERT3(x, line)
#define STATIC_ASSERT(x)        STATIC_ASSERT2(x, __LINE__)

STATIC_ASSERT(sizeof(uint08) == 1)
STATIC_ASSERT(sizeof(uint16) == 2)
STATIC_ASSERT(sizeof(uint32) == 4)
STATIC_ASSERT(sizeof(int08) == 1)
STATIC_ASSERT(sizeof(int16) == 2)
STATIC_ASSERT(sizeof(int32) == 4)
STATIC_ASSERT(sizeof(ascii) == 1)
STATIC_ASSERT(sizeof(uni) == 2)
STATIC_ASSERT(sizeof(f32) == 4)
STATIC_ASSERT(sizeof(f64) == 8)
// ************************************************************************************************

#ifndef UNALIGNED32
#define UNALIGNED32(A, B) ((((uint32)A) & 3) | (((uint32)B) & 3))
#endif

// ************************************************************************************************
// ************************************************************************************************
// Atomic X86
// ************************************************************************************************
// ************************************************************************************************

#ifdef _LSL_TARGETARCH_X86

inline long dslib_atomic_fetch_add(volatile long *mem, long addVal)
{
 #ifdef _MSC_VER
    __asm{
      mov edi, mem
      mov eax, addVal
      lock xadd [edi], eax
    }
 #else
  asm volatile(
    "lock xadd %0, (%1);"
    : "=a"(addVal)
    : "r"(mem), "a"(addVal)
    : "memory"
  );
  return addVal;
 #endif
}

uint32 dslib_atomic_incU32(uint32 *pValue)
{
  return dslib_atomic_fetch_add((long*)pValue, 1);
}

uint32 dslib_atomic_decU32(uint32 *pValue)
{
  return dslib_atomic_fetch_add((long*)pValue, -1);
}

#endif

//uint32 dslib_get_systime(void)
//{
//  SYSTIME xtime;
//  
//  OS_SSR_GetSysTime(&xtime);
//  uint32 retcode = xtime.wHour;
//  retcode = (retcode << 8) | xtime.wMinute;
//  retcode = (retcode << 8) | xtime.wSecond;
//  return (retcode << 8); 
//}
//
//uint32 dslib_set_systime(uint32 ltime)
//{
//  SYSTIME xtime;
//  
//  ltime = (ltime >> 8);
//  xtime.wSecond = (uint16)(ltime & 0xFF);
//  ltime = (ltime >> 8);
//  xtime.wMinute = (uint16)(ltime & 0xFF);
//  ltime = (ltime >> 8);
//  xtime.wHour = (uint16)(ltime & 0xFF);
//  
//  if((xtime.wSecond < 60) && (xtime.wMinute < 60) && (xtime.wHour < 24))
//  {
//    OS_SSR_SetSysTime(&xtime);
//    return 1;
//  }
//  
//  return 0;
//}
//
//uint32 dslib_get_sysdate(void)
//{
//  SYSDATE xdate;
//  
//  OS_SSR_GetSysDate(&xdate);
//  uint32 retcode = xdate.wYear;
//  retcode = (retcode << 4) | xdate.wMonth;
//  retcode = (retcode << 8) | xdate.wDay;
//  retcode = (retcode << 4) | xdate.wDayOfWeek;
//  return retcode;
//}
//
//uint32 dslib_set_sysdate(uint32 ldate)
//{
//  SYSDATE xdate;
//  
//  ldate = ldate >> 4;
//  xdate.wDay = (uint16)(ldate & 0xFF);
//  ldate = ldate >> 8;
//  xdate.wMonth = (uint16)(ldate & 0x0F);
//  ldate = ldate >> 4;
//  xdate.wYear = (uint16)(ldate & 0xFFFF);
//  OS_SSR_SetSysDate(&xdate);
//  return 1;
//}


void dslib_free(void* ptr)
{
  if(ptr != NULL)
  {
    OS_SSR_Free(ptr);
  }
}

void* dslib_malloc(uint32 size)
{
  return OS_SSR_Malloc(size);
}

void* dslib_realloc(void* ptr, uint32 size)
{
  return OS_SSR_Realloc(ptr, size);
}

uint32 dslib_memory(void **pptr, uint32 size)
{
  void *ph;
  
  if(size == 0)
  {
    if(*pptr != 0)
    {
      dslib_free(*pptr);
    }
    
    *pptr = 0;
    return 1;
  }
  
  if(*pptr != 0)
  {
    ph = dslib_realloc(*pptr, size);
    if(ph != 0)
    {
      *pptr = ph;
      return 1;
    }
    
    return 0;
  }
  
  ph = dslib_malloc(size);
  if(ph != 0)
  {
    *pptr = ph;
    return 1;
  }
  
  return 0;
}

void *dslib_buffer_create(void *statbuff, uint32 statbuff_size, uint32 expected_size)
{
  if((expected_size > statbuff_size) || (statbuff == NULL))
  {
    return dslib_malloc(expected_size);
  }
  
  return statbuff;
}

void *dslib_buffer_destroy(void *statbuff, void *buffer)
{
	if (buffer != statbuff)
	{
		if (buffer != NULL)
		{
			dslib_free(buffer);
		}
	}
	return NULL;
}

void* dslib_memcpy(void* dst0, const void* src0, uint32 len0)
{
  ascii *dst = (ascii*)dst0;
  const ascii *src = (ascii*)src0;

  if((len0 >= 4) && !UNALIGNED32(src, dst))
  {
    uint32* aligned_dst = (uint32*)dst;
    uint32* aligned_src = (uint32*)src;

    while(len0 >= 16)
    {
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      len0 -= 16;
    }
    
    while(len0 >= 4)
    {
      *aligned_dst++ = *aligned_src++;
      len0 -= 4;
    }

    dst = (ascii*)aligned_dst;
    src = (ascii*)aligned_src;
  }

  while(len0--)
  {
    *dst++ = *src++;
  }
  
  return dst0;
}

void* dslib_memmove(void* dst0, const void *src0, uint32 len0)
{
  ascii *dst = (ascii*)dst0;
  const ascii *src = (const ascii*)src0;

  if(src < dst && dst < src + len0)
  {
    src += len0;
    dst += len0;
    while(len0--)
	  {
      *--dst = *--src;
    }
  }
  else
  {
    dslib_memcpy(dst0, src0, len0);
  }

  return dst0;
}

uint32 dslib_strlen(const ascii *txt)
{
  uint32 retcode = 0;
  ascii *ph = (ascii*)txt; 
  while(*ph++ != 0)
  {
    retcode++;
  }
  return retcode;
}

uint32 dslib_strlen16(const uint16 *txt)
{
  uint32 retcode = 0;
  uint16 *ph = (uint16*)txt; 
  while(*ph++ != 0)
  {
    retcode++;
  }
  return retcode;
}

void dslib_strcpy(void *dst, const void *src)
{
  uint08 *pd = (uint08*)dst;
  uint08 *ps = (uint08*)src;
  do
  {
    *pd++ = *ps;
  }
  while(*ps++);
}

uint32 dslib_strsize(const void *str, uint32 chrsize)
{
	if (chrsize == 1)
	{
		return (dslib_strlen((const ascii*)str) + 1);
	}

	return ((dslib_strlen16((const uint16*)str) + 1) * 2);
}

static void dslock_key(uint08 *data, uint32 bytes, uint08 key0)
{
	uint08 key = key0;
	uint08 i = 0;
	while (bytes--)
	{
		uint08 keynext = *data ^ (key0 + i);
		*data = *data ^ key;
		key = keynext;
		data++;
		i += 5;
	}
}

static void dsunlock_key(uint08 *data, uint32 bytes, uint08 key0)
{
	uint08 key = key0;
	uint08 i = 0;
	while (bytes--)
	{
		*data = *data ^ key;
		key = *data ^ (key0 + i);
		data++;
		i += 5;
	}
}

static void dslock_swap(uint08 *data, uint32 bytes)
{
	uint08 *pa = data;
	uint08 *pe = pa + bytes;

	bytes = bytes >> 1;
	while (bytes--)
	{
		pe--;
		uint08 tmp = *pa;
		*pa++ = *pe;
		*pe = tmp;
	}
}
 
uint32 dslib_encryptU32(uint32 value) // verschlüsseln
{
	uint32 retcode = value;
	dslock_swap((uint08*)&retcode, 4);
	dslock_key((uint08*)&retcode, 4, 0x96);
	return retcode;
}

uint32 dslib_decryptU32(uint32 value) // entschlüsseln
{
	uint32 retcode = value;
	dsunlock_key((uint08*)&retcode, 4, 0x96);
	dslock_swap((uint08*)&retcode, 4);
	return retcode;
}

static uint32 lastmagicno = 0;
uint32 dslib_magicnumber(void)  // magic random number
{
	lastmagicno = dslib_encryptU32(dslib_tabsolute() ^ lastmagicno);
	return lastmagicno;
}

uint32 dslib_merge32(uint32 value, uint32 newval, uint32 offset, uint32 width)
{
  if((width > 0) && (offset < 32))
  {
    uint32 mask = 2;
    while(--width)
    {
      mask = mask * 2;
    }
    mask -= 1;
    
    newval  = (newval & mask) << offset;
    mask    = mask << offset;
    value   = (value & (~mask)) | newval;
  }
  
  return value;
}

void* dslib_cilget(const char *name)
{
  if(lsl_pOS != NULL)
  {
    if(lsl_pOS->piCIL != NULL)
    {
      void *retcode = NULL;
      if(lsl_pOS->piCIL->CILGet((char*)name, &retcode) == SYS_ERR_NONE)
      {
        return retcode;
      }
    }
  }
  return NULL;
}

void* dslib_criticalsection_create(const char *name)
{
  MT_SEMAHANDLE retcode = NULL;
  
  if(Cil_LSL_MULTITASK == NULL)
  {
    Cil_LSL_MULTITASK = (LSL_MT_TYPE*)dslib_cilget("LSL_MULTITASK");
  }
  
  if(Cil_LSL_MULTITASK != NULL)
  {
    if(Cil_LSL_MULTITASK->MT_CreateSemaphore != NULL)
    {
      retcode = Cil_LSL_MULTITASK->MT_CreateSemaphore(MTSEMATYPE_MUTEX, 1, 0, (char*)name);
    }
  }
  
  return retcode;
}

int32 dslib_criticalsection_delete(void *hdl)
{
  if((Cil_LSL_MULTITASK != NULL) && (hdl != NULL))
  {
    return Cil_LSL_MULTITASK->MT_DeleteSemaphore((MT_SEMAHANDLE*)hdl);
  }
  return 0;
}

int32 dslib_criticalsection_start(void *hdl)
{
  if(hdl != NULL)
  {
    return Cil_LSL_MULTITASK->MT_Wait((MT_SEMAHANDLE*)hdl);
  }
  return 0;
}

int32 dslib_criticalsection_stop(void *hdl)
{
  if(hdl != NULL)
  {
     return Cil_LSL_MULTITASK->MT_Signal((MT_SEMAHANDLE*)hdl);
  }
  return 0;
}

char *dslib_namemerge(char *dst, const char *src, void *pth)
{
  LSL_POS->piCRT->sprintf(dst, "%s%p", src, pth);
  return dst;
}

#ifndef sigclib_LONG_MAX
 #define sigclib_LONG_MAX       2147483647L
#endif
#ifndef sigclib_LONG_MIN
 #define sigclib_LONG_MIN       (-(sigclib_LONG_MAX) -1L)
#endif

//  #define dslib_PRINT  0x0001
//  #define dslib_GRAPH  0x0002
//  #define dslib_PUNCT  0x0004 
//  #define dslib_ALNUM  0x0008
//  #define dslib_XDIGIT 0x0010 
#define dslib_DIGIT  0x0020 
#define dslib_ALPHA  0x0040
//  #define dslib_LOWER  0x0080 
#define dslib_UPPER  0x0100 
#define dslib_SPACE  0x0200 
//  #define dslib_CNTRL  0x0400 
  
const unsigned short dslib_TypeOfC[256] = 
{
  0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
  0x0601, 0x0601, 0x0601, 0x0601, 0x0601,
  0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
  0x0201, 
  0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 
  0x003B, 0x003B, 0x003B, 0x003B, 0x003B, 0x003B, 0x003B, 0x003B, 0x003B, 0x003B, 
  0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 
  0x015B, 0x015B, 0x015B, 0x015B, 0x015B, 0x015B, 
  0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 0x014B, 
  0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 
  0x00DB, 0x00DB, 0x00DB, 0x00DB, 0x00DB, 0x00DB, 
  0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 0x00CB, 
  0x0007, 0x0007, 0x0007, 0x0007, 
  0x0400,
  0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
  0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
  0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
  0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
  0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
  0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
  0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
  0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002
};

//  #define dslib_ISLOWER(A)  (dslib_TypeOfC[(unsigned char)(A)] & dslib_LOWER)
  #define dslib_ISUPPER(A)  (dslib_TypeOfC[(unsigned char)(A)] & dslib_UPPER)
  #define dslib_ISDIGIT(A)  (dslib_TypeOfC[(unsigned char)(A)] & dslib_DIGIT)
//  #define dslib_ISXDIGIT(A) (dslib_TypeOfC[(unsigned char)(A)] & dslib_XDIGIT)
  #define dslib_ISSPACE(A)  (dslib_TypeOfC[(unsigned char)(A)] & dslib_SPACE)
  #define dslib_ISALPHA(A)  (dslib_TypeOfC[(unsigned char)(A)] & dslib_ALPHA)
//  #define dslib_ISPUNCT(A)  (dslib_TypeOfC[(unsigned char)(A)] & dslib_PUNCT)
//  #define dslib_ISALNUM(A)  (dslib_TypeOfC[(unsigned char)(A)] & dslib_ALNUM)
//  #define dslib_ISPRINT(A)  (dslib_TypeOfC[(unsigned char)(A)] & dslib_PRINT)

long dslib_strtol(const char *nptr, char **endptr, int base)
{
  // vorangestellte Zeichen wie [SPACE], [TAB], [LF], [VT], [FF] und [CR] werden ignoriert
  // [+] und [-] funktioniert
  // 123 ....................... dezimal
  // 0x  ....................... hexadezimal beginnend mit 0x
  // 2#, 3#, ... 16# bis 32# ... vorangestellte Basis, getrennt durch #
  // wird 'base' mit 0 angegeben wird die Basis automatisch bestimmt, Annahme dezimal

  register const unsigned char *s = (const unsigned char *)nptr;
  register unsigned long acc;
  register int c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  do 
  {
    c = *s++;
  } 
  while(dslib_ISSPACE(c));
  
  if(c == '-') 
  {
    neg = 1;
    c = *s++;
  } 
  else if(c == '+')
  {
    c = *s++;
  }
  
  if((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) 
  {
    c = s[1];
    s += 2;
    base = 16;
  }
  
  if(base == 0)
  {
    base = 10;
  }
  
  cutoff = (unsigned long)(neg ? -(unsigned long)sigclib_LONG_MIN : sigclib_LONG_MAX);
  cutlim = cutoff % (unsigned long)base;
  cutoff /= (unsigned long)base;
  
  for(acc = 0, any = 0;; c = *s++) 
  {
    if(dslib_ISDIGIT(c))
    {
      c -= '0';
    }
    else if(dslib_ISALPHA(c))
    {
      c -= dslib_ISUPPER(c) ? 'A' - 10 : 'a' - 10;
    }
    else if(c == '#')
    {
      if((acc < 1) || (acc > 32) || (neg != 0))
      {
        break;
      }
      base = acc;
      acc  = 0;
      c    = 0;
    }
    else
    {
      break;
    }
    if(c >= base)
    {
      break;
    }
    if(any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
    {
      any = -1;
    }
    else 
    {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if(any < 0) 
  {
    acc = neg ? sigclib_LONG_MIN : sigclib_LONG_MAX;
   // rptr->_errno = ERANGE;
  }
  else if(neg)
  {
    acc = -acc;
  } 
  
  if(endptr != 0)
  {
    *endptr = (char *) (any ? (char *)s - 1 : nptr);
  }
  
  return (acc);
}

