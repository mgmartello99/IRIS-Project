//****************************************************************************/
//*                                                                          */
//*  File: LSL_C_KERNEL.H                                                    */
//*  Date: SEP 17, 2001                                                      */
//*  By:                                                                     */
//*  Description:                                                            */
//*	KERNEL Information                                                   */
//*  Revision History:                                                       */
//*      Rev     By      Date        Description                             */
//*      ---     --      ----        -----------                             */
//*      1.00    MENK    17/09/01    Initial Version                         */
//*      2.00    MENK    13/03/01    Changes without any exceptable comments */
//*                                                                          */
//****************************************************************************/

#ifndef LSL_KERNEL_H
#pragma once
#define LSL_KERNEL_H

#pragma pack(push, 1)

typedef struct
{
   unsigned long  setbp;      // Ofs: 0000
   unsigned long  handler;    // Ofs: 0004
   unsigned long  stopbreak;  // Ofs: 0008
   unsigned long  singlestep; // Ofs: 0012
   unsigned char  fill[16];   // Ofs: 0016
}KRNL_DEBUGINFO;              // Summe = 32 Byte


typedef struct
{
   unsigned long  max_datamem;  // Ofs: 0000
   unsigned long  max_codemem;  // Ofs: 0004
}KRNL_APPLMEM;

typedef struct
{
   unsigned long freestack;
   unsigned long minstack;
   unsigned long state;
   unsigned long cputime;
}KRNL_LSLTASKINFO;                   // 16 Byte

typedef struct
{
   unsigned char    state;
   unsigned char    tabsolutedebug;
   unsigned short   tabsolutecounter;// Summe 4 Byte
   KRNL_LSLTASKINFO rt;              // 16 Byte
   KRNL_LSLTASKINFO ct;              // 16 Byte
   KRNL_LSLTASKINFO bt;              // 16 Byte
   KRNL_LSLTASKINFO tt;              // 16 Byte
   void            *traceinfo;       // 4 Byte
   unsigned char fill[56];           // 56 Byte
}KRNL_LSLTASK;                       // Summe head:4 + rt:16 + ct:16 + bt:16 + tt:16 + trace:4 + fill:56 => 128 Byte


typedef struct
{
   unsigned char index;
   unsigned char slotsize;
   unsigned char slotnumber;
   unsigned char dummy;
}KRNL_MSGBUFHEAD;

typedef struct
{
   KRNL_MSGBUFHEAD head;
// msg slot added here   
}KRNL_MSGBUF;

typedef struct
{                               // starting at m0100
   KRNL_APPLMEM   applmem;      // m0100 => 00008
   KRNL_LSLTASK   task;         // m0108 => 00128
   KRNL_DEBUGINFO debug;        // m0128 => 00032
   KRNL_MSGBUF   *systemmsg;    // m0148 => 00004
   KRNL_MSGBUF   *errormsg;     // m014A => 00004
   unsigned char  fill[16];    // => struct hat nun genau 0xC0 Byte Größe
}KRNL_SYSTEMINFO;              // Struct Größe darf 192 nicht überschreiten

typedef struct LSLAPPHEAPINFO_TYPE
{
   unsigned long startaddr;
   unsigned long totalsize;
   unsigned long usedmem;
   unsigned long freemem;
}LSLAPPHEAPINFO_TYPE;

// Interface Struktur "HEAPMAT"
typedef struct 
{
    void *HEAP_MAT;
    LSLAPPHEAPINFO_TYPE *applmeminfo;
    void (* CheckHeapList)();
} HEAP_TYPE;

typedef struct _BlockHeader 
{
  struct _BlockHeader * Next;
  unsigned BlockSize : 28;
  unsigned Kennung : 4;
} BlockHeader;

typedef struct 
{
  BlockHeader *UsedList;
  BlockHeader *FreeList;
} MAT_TYPE;

//#define HEAP_STATISTIC
#undef HEAP_STATISTIC

#ifdef HEAP_STATISTIC
typedef struct
{
  unsigned long resetTime_ms;           // Zeipunkt, an dem dei Statistik resettiert wurde
  unsigned long nbrOfAllocs;            // Anzahl der Alloc-Aufrufe
  unsigned long nbrOfFrees;             // Anzahl der Free-Aufrufe
  unsigned long nbrOfReallocs;          // Anzahl der Realloc-Aufrufe
  unsigned long nbrOfUsedBlocksCurrent; // Anzahl der Used-Blocks (aktuell)
  unsigned long nbrOfFreeBlocksCurrent; // Anzahl der Free-Blocks (aktuall)
  unsigned long nbrOfUsedBlocksHighest; // Anzahl der Used-Blocks (höchster Wert)
  unsigned long nbrOfFreeBlocksHighest; // Anzahl der Free-Blocks (höchster Wert)
  unsigned long sizeofLargestFreeBlock; // Größe des größten freien Blocks
} HEAP_STAT;
#endif

typedef struct MAT_TYPE_V2
{
  BlockHeader *UsedList;
  BlockHeader *FreeList;
  unsigned long dbgHeapFlags; //ctrlFlags;
  unsigned long sizeofDbgHeapHdr;
  unsigned long sizeofDbgHeapInfo;    // size of DbgHeap Header + Trailer
  unsigned long triggerAddr;
#ifdef HEAP_STATISTIC
  HEAP_STAT stat;
#endif
} MAT_TYPE_V2;

#pragma pack(pop)


#endif
