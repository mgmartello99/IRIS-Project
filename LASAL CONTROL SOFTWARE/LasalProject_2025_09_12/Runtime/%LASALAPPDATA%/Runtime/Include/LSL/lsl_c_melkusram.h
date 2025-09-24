// ------------------------------------------------------------
// Sigmatek GmbH & CoKG
//
//  Melkus Ram
//
//   Version  Date      Author    Comment
//   1.0      24012003  pr        initial version
// ------------------------------------------------------------

//HFILTER:1
#ifndef __LSL_IF_MELKUSRAM_H
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define __LSL_IF_MELKUSRAM_H

#define LSLAPI_MELKUSRAM_VERSION   0x0001    // Versionsnummer der Interface-Struktur
#define LSLAPI_MELKUSRAM_IFNAME    "MELKUSRAM"

//
// Interface Struktur
//
#pragma pack (push, 1)
typedef struct
{
  unsigned long version;
  long (* GetInfo)(unsigned long *startadr, unsigned long *length);
  void (* Reset)(unsigned long addr, unsigned long length);
  void (* AdjustDataStart)(unsigned long newaddr, unsigned long oldaddr, unsigned long newlength, unsigned long oldlength);
  long (* MakeFileHeader)(struct _LSLFILE_HEADER_MRAM *phdr);
  long (* DataStore_Callback)(void *CmdInterface,
                              unsigned long par1,
                              unsigned char *pData, 
                              unsigned long DataLen);
  long (* GetSramdatLen)();
  long (* GetSramdat)(unsigned char *dest, unsigned long ofs, unsigned long len);
}LSLAPI_MELKUSRAM;
#pragma pack (pop)

#define MELKUSRAM_DESCR_VER     0x0000    // Versionsnummer der MELKUSRAM_DESCR Struktur

//
// Melkus RAM Descriptor
//
#pragma pack (push, 1)
typedef struct S_MELKUSRAM_DESCR
{
  unsigned long version;
  unsigned long DataStart;
  unsigned long DataLength;
  unsigned long UsedData;
  unsigned short DataValid;
  unsigned short SramFormat;
  unsigned long reserved[11];    // auffüllen auf 64 Bytes
}MELKUSRAM_DESCR;
#pragma pack (pop)

// Defines

// Errors


//HFILTER:1
#endif // __LSL_IF_MELKUSRAM_H
//HFILTER:
