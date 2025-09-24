#ifndef LSE_IF
#pragma once
#define LSE_IF

#ifndef _LSE_SYS_H
#pragma once
#define _LSE_SYS_H

typedef void LSECOM_IRQPLC_FUNCTYPE(void *, char *);
typedef void LSECOM_CLOSE_FUNCTYPE(void);


#ifndef _T_GRAPH_H
#define _T_GRAPH_H

#ifndef IPC_SINGLEDATATYPE
typedef char                 _BYTE;                                // 1 byte
typedef unsigned char        _UBYTE;                               // 1 byte
typedef short int            _WORD;                                // 2 byte
typedef unsigned short       _UWORD;                               // 2 byte
typedef long                 _DWORD;                               // 4 byte
typedef unsigned long        _UDWORD;                              // 4 byte
typedef float                _REAL;                 // 4 byte floating point
#define IPC_SINGLEDATATYPE
#endif

typedef void*                _REGION;                   // region (viewport)
typedef _UDWORD              _ATTRIB;                           // attribute
typedef _UDWORD              _COLOR;             // fore and backgroundcolor
typedef _WORD                _KOORD;                    // single koordinate
typedef _UBYTE               _FONTPTR;         // fontdata (used as pointer)


typedef struct { _WORD dashSize;
                 _WORD dashRsvd;
                 _UBYTE *dashList;
               }
_DASH;

typedef struct { _KOORD  x;
                 _KOORD  y;
               }
_DOT;

typedef struct { _DOT    xy1;
                 _DOT    xy2;
               }
_ROOM;

#pragma pack (push, 1)
typedef struct { _UBYTE  no;
                 _UWORD  col;
               }
_BMPDAT;

#define BMP_TURN           0x0001
#define BMP_HICOLOR        0x0002
#define BMP_NO_TRANSPARENT 0x0004

typedef struct { _UWORD  width;
                 _UWORD  height;
                 _UWORD  transparent;
                 _UWORD  state;
                 _UDWORD datano;
                 _BMPDAT *ptr;
               }
_BMP;
#pragma pack (pop)

typedef struct { _UWORD  cred;
                 _UWORD  cgreen;
                 _UWORD  cblue;
               }
_RGB;

typedef struct { void    *ptr;
                 _ROOM   room;
               }
_IMAGE;

typedef struct { void  *ptr;
                 _UWORD width;
                 _UWORD height;
               }
_NEWSCREEN;


#endif //_T_GRAPH_H


#ifndef __DEF_DDE_
 #define __DEF_DDE_

#pragma pack (push, 1)

   typedef struct { _UDWORD     Second:5; 
                    _UDWORD     Minute:6; 
                    _UDWORD     Hour:5; 
                    _UDWORD     Day:5; 
                    _UDWORD     Month:4; 
                    _UDWORD     Year:7; 
   } _DDE_DATIM; 

   typedef struct { char       FileName[8]; 
                   char       Extension[3]; 
                   _UBYTE     Attributes; 
                   char       Reserved[8]; 
                   _UWORD     FirstClusterHi; 
                   _DDE_DATIM DateTime; 
                   _UWORD     FirstCluster;
                   _DWORD     FileSize; 
  }
  _DDE_INFO; 

#pragma pack (pop)

#endif //__DEF_DDE_
#ifndef WIN32
#include ".\RTOS\RTOS_C_DDE_Type.h"
#endif

//
////the following structure is the LSE Interface:
typedef struct
{
	// Interface Data Entries
	unsigned long lActive;		// MAE runtime active if non-zero
	unsigned char* pIniFile;	// MAE INI file buffer pointer
	unsigned long lEnableTask;	// Non-zero if MAE Task is enabled to run (V3.05)

	// Interface Function Pointers
	void* (* pLSE_InstallTask)(void);	// Pointer to MAE task install function
	void  (* pLSE_Close)(void);
	void  (* pLSE_Restart)(void);
	unsigned int (* pStoreLST)(char* ); // Pointer to MAE list store function

  /////////////////////////////////////////
	void* (* pLSEOS_MemMove)(void*, const void*, unsigned long);
	void* (* pLSEOS_MemCpy)(void*, const void*, unsigned long);
  void* (* pLSEOS_MemSet)(void*, unsigned char, unsigned long);
  unsigned short (* pLSEOS_MemCmp)(const void*, const void*, unsigned long); 
	unsigned long (* pLSEOS_GetAbsTime)(void);

	unsigned long (* pLSEOS_Fread)(int, void*, unsigned long); 
	unsigned long (* pLSEOS_Fwrite)(int, void*, unsigned long); 
	int (* pLSEOS_Fopen)(char*, unsigned long, unsigned long);   
	void (* pLSEOS_Fclose)(int); 
	long (* pLSEOS_Flseek)(int, unsigned long, unsigned long); 
	long (* pLSEOS_Ftell)(int); 
	long (* pLSEOS_Ffilelength)(int); 
	int  (* pCreateDir)(char*);

  long (* pOS_FindFirst)(char*, char*, _DDE_INFO*, unsigned char, unsigned char);
  int (* pOS_FindNext)(long, _DDE_INFO*);
  int (* pOS_FindClose)(long);

	void* (* pLSEOS_GetSRAMInfo)(unsigned long*); 
	void* (* pLSEOS_GetPointerToSRAM)(unsigned long); 
	unsigned char* (* pLSEOS_OSGetPointerToKeyboard)(void);
	void (* pLSEOS_SetTime)(void*);
	void (* pLSEOS_GetTime)(void*);
	void (* pLSEOS_Wait)(void);

	unsigned long (* pLSECOM_GetAddressVar)(char *, void*, void*);
	unsigned long (* pLSECOM_GetAddressObj)(char *, void*, void*);
	unsigned long (* pLSECOM_RegisterVar)(void *, void*);
	unsigned long (* pLSECOM_RegisterObj)(void *, void*);
	void (* pLSECOM_DeleteQueue)(unsigned char ,void*);
	long (* pLSECOM_Login)(void*);
	void (* pSYSMAE_SetExternFunction)(LSECOM_IRQPLC_FUNCTYPE*, LSECOM_CLOSE_FUNCTYPE*);
	unsigned long (* pLSECOM_SetData)(void*, unsigned short , void*, void *);
	unsigned long (* pLSECOM_GetData)(void*,  unsigned short , void*, void *);

	void (* pLSEOS_MetaEvent)(void);

	long (* pLSEMAIN_pMain)(void);

	// set at Startup from LSE
	LSECOM_IRQPLC_FUNCTYPE (* pLSECOM_IRQ_PLC);
	LSECOM_CLOSE_FUNCTYPE (* pLSEMAIN_Close);

	long (* pLSETimer)(void* , unsigned long );
	void (* pLSE_ScreenSaver)(void);
  _UWORD (* pGetPalette)(_UBYTE*, _UWORD, _UWORD);

	///////////functions from t_graph.h/////
  void (* pHideMouse)(void); 
  void (* pShowMouse)(void);
  _UWORD (* pIfNecHideMouse)(_ROOM*); 
  void (* pIfNecShowMouse)(_UWORD); 
  void (* pSetFontAttribute)(_ATTRIB); 
	_UWORD (* pStrWidth)(void*, _UWORD); 
	_UWORD (* pInitGraph)(void);
  void (* pCloseGraph)(void);
  void (* pGetScreenSize)(_ROOM*);
  _COLOR (* pGetPixel)(_KOORD, _KOORD);
    void (* pPutPixel)(_KOORD, _KOORD, _ATTRIB, _COLOR);
  void (* pLine)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
  void (* pPolygon)(_UWORD, _DOT*, _ATTRIB, _COLOR);
  void (* pRectangle)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
  void (* pBar)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
  void (* pOutTextXY)(_KOORD, _KOORD, const void*, _ATTRIB, _COLOR, _UWORD);
  void (* pOutTextLen)(_KOORD, _KOORD, const void*, _UWORD, _ATTRIB, _COLOR, _UWORD); 
  void (* pArc)(_KOORD, _KOORD, _KOORD, _KOORD, _WORD, _WORD, _UWORD, _ATTRIB, _COLOR); 
  void (* pOval)(_KOORD, _KOORD, _KOORD, _KOORD, _WORD, _WORD, _ATTRIB, _COLOR);
  void (* pButton)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR, _COLOR);
  void (* pScroll)(_KOORD, _KOORD, _KOORD, _KOORD, _KOORD, _KOORD, _COLOR);
  void (* pBitmap)(_KOORD, _KOORD, _BMP*, _ATTRIB, _COLOR);
  void (* pActivateFont)(_FONTPTR*);
  void (* pGetFontSize)(_UWORD*, _UWORD*, _FONTPTR*);
  void (* pFreeRegion)(_REGION*);
  void (* pActivateRegion)(_REGION);
  void (* pDeactivateRegion)(void);
  void (* pStartRegion)(void);
  void (* pAddRegion)(_ROOM*);
  _REGION (* pEndRegion)(void);
  _REGION (* pSubRegion)(_REGION, _REGION);
  _REGION (* pAndRegion)(_REGION, _REGION); 
  _REGION (* pXorRegion)(_REGION, _REGION); 
  _REGION (* pOrRegion)(_REGION, _REGION); 
  _REGION (* pGetActiveRegion)(void); 
  _UDWORD (* pGetNoColors)(void); 

  _UWORD (* pGetImage)(_IMAGE*, _KOORD, _KOORD, _KOORD, _KOORD);
  void (* pPutImage)(_IMAGE*, _KOORD, _KOORD);
  _UWORD (* pGetRetrace)(void); 
  void (* pWaitForRetrace)(void);
  _UWORD (* pIsGraphInit)(void);
  _UWORD (* pMemory)(void**, const _UDWORD);                            
  void (* pOutXPixelPool)(_KOORD, _KOORD*, _UWORD, _UWORD, _ATTRIB, _COLOR); 
  void (* pOutYPixelPool)(_KOORD*, _KOORD, _UWORD, _UWORD, _ATTRIB, _COLOR); 
  void (* pOutPixelLine)(_KOORD, const _KOORD, _UBYTE*, _UWORD); 
  _UWORD (* pRoomInRegion)(_ROOM*, _REGION); 
  _UWORD (* pMakeTrueColor)(_UWORD, _UWORD, _UWORD); 

  void (* pOS_ShowCursor)(void);
  void (* pOS_MoveCursor)(_KOORD x, _KOORD y);
  void (* pOS_LimitMouse)(_KOORD x1, _KOORD y1, _KOORD x2, _KOORD y2);
  void (* pOS_StopMouse)(void);
  void (* pOS_StopEvent)(void);
  void (* pOS_StartEvent)(_UWORD e);
  _UWORD (* pOS_StoreEvent)(void *p);
  _UWORD (* pOS_KeyEvent)(_UWORD wait, void *p);
  _UWORD (* pOS_InitMouse)(void);

  _COLOR (* pReadPixel)(_KOORD, _KOORD);                   // = getpixel
  void (* pWritePixel)(_KOORD, _KOORD, _ATTRIB, _COLOR); // = writepixel
  _COLOR (* pMakeColor)(_COLOR, _COLOR);

  void (* pMouseCursorStyle)(_UWORD);
  void (* pDestroyOffScreen)(_NEWSCREEN*);
  _UWORD (* pCreateOffScreen)(_NEWSCREEN*, _UWORD, _UWORD);
  void (* pGetScreen)(_NEWSCREEN*);
  _UWORD (* pSetScreen)(_NEWSCREEN*);
  _UWORD (* pCopyScreen)(_NEWSCREEN *dest, _NEWSCREEN *src, _ROOM *pd, _ROOM *ps);
  long (* pLCD_Contrast)(unsigned short, unsigned long);
  long (* pLCD_Brightness)(unsigned short, unsigned long);
  void (* pDrawTLBR)(_KOORD x1, _KOORD y1, _KOORD x2, _KOORD y2, _ATTRIB att, _COLOR frame);
  unsigned long (* pSetNewCalibration)(void* newdat);
  long (* pLSE_SetTouchKoords)(unsigned long raw0);
  unsigned short  (* pChrWidth)(unsigned short param0, unsigned short param1);
  unsigned long (* pSetGraphicSystem)(unsigned long attr0, unsigned long attr1, void* param0);
  void (* pOutChar)(_KOORD, _KOORD, _UWORD, _ATTRIB, _COLOR, _UWORD);
  _UWORD (* TxtWidth)(void *p, _UWORD len, _UWORD chr_size);

  void (*pRefreshRectangle)(_ROOM *r);
  void (*pPenDashSet)( _UDWORD iD );
  void (*pPenDashDefine)( _UDWORD iD, _DASH *pD );
  _UWORD (*pRotateScreen)(_NEWSCREEN *dest, _NEWSCREEN *src, _ROOM *pd, _ROOM *ps, _WORD Degree );

  // Ab LasalOS Version 5.70 gibt es eine Versionsnummer und eine Länge der IMAE Struktur.
  // Bei früheren Betriebssystem-Versionen wurden diese Felder nicht initialisiert und dürfen 
  // dann auch nicht abgefragt werden !
  unsigned long size;
#define IMAE_VERSION    9
  unsigned long version;

  // ab IMAE Version 1:  
  long (*pSetLed)(unsigned long ledCode, unsigned long op); // op: 0=aus, 2=blink, >=3=ein
  long (*pGetPotiVal)(unsigned long potiNr);
  long (*pGetDipswitchVal)(unsigned long dipNr);

  // ab IMAE Version 2:  
  long (* pOS_FindFirstEx)(char *, unsigned char, unsigned char, _DDE_INFO *, char *, unsigned long);
  int  (* pOS_FindNextEx)(long , _DDE_INFO *, char *, unsigned long);

  // ab IMAE Version 3 - OS 01.01.025
  _UWORD (* pMemoryV1)(void **dest, const _UDWORD size, unsigned char Kennung);

//SGraphic // ab IMAE Version 4 - OS 01.01.052
	unsigned long (* pmyInitGraph)(_KOORD, _KOORD, _COLOR);
	void* (* pmyGetBuffer)(void);
	unsigned long (* pmyGetPixel)(_KOORD, _KOORD);
	unsigned long (* pmyGetImage)(void*, _KOORD, _KOORD, _KOORD, _KOORD);	
	unsigned long (* pmyGetImageSize )( _KOORD, _KOORD, _KOORD, _KOORD );
	void (* pmyLoadUserFont )( void* );
	void (* pmyTextDimensions)( _DOT*, char*, unsigned long, unsigned long );
	unsigned long (* pmyTextHeight)(short);
	void (* pmyClrScr)(_COLOR);	
	void (* pmyPutPixel)(_KOORD, _KOORD, _ATTRIB, _COLOR);
	void (* pmyLine)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
	void (* pmyPolygon)(_UDWORD, _DOT*, _ATTRIB, _COLOR);
	void (* pmyRectangle)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
	void (* pmyBar)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
	void (* pmyOutTextXY)(_KOORD, _KOORD, void*, _ATTRIB, _COLOR, _UWORD);
	void (* pmyScroll)(_KOORD, _KOORD, _KOORD, _KOORD, _KOORD, _KOORD, _COLOR);
	void (* pmyPutImage)(void*, _KOORD, _KOORD);
	void (* pmyBitmap)(_KOORD, _KOORD, _BMP*);
	unsigned long (* pmyMakeRegion)(void**);
	void (* pmyKillRegion)(void**);
	void (* pmyEmptyRegion)(void**, unsigned long);
	void (* pmyActivateRegion)(void**);		
	void (* pmyDefaultRegion)();
	void (* pmyAndRegion)(void**,void**);
	void (* pmyXorRegion)(void**,void**);
	void (* pmyOrRegion)(void**,void**); 
//	void (* pmyPutPixelRegion)(void**, _KOORD, _KOORD, _ATTRIB, _COLOR);
//	void (* pmyLineRegion)(void**, _KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
//	void (* pmyPolyLineRegion)(void**, _KOORD*, _KOORD*, _UDWORD, _ATTRIB, _COLOR);
//	void (* pmyRectangleRegion)(void**, _KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
	void (* pmyBarRegion)(void**, _KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR);
	unsigned long (* pmyGetXSize)(void);
	unsigned long (* pmyGetYSize)(void);
	unsigned long (* pmyLCDUpdateDone)(void);
	void (* pmyLCDRefresh)(const unsigned char);
  void (* pmyInvert)(const unsigned char);	//ws: invert
  
  // ab IMAE Version 5
  unsigned long (* pDefineMouseCursorStyle)(unsigned long, unsigned long, unsigned long, void*, void*);

  // ab IMEA Version 6
  _UWORD (* pOS_StoreEventEx)(void * p, unsigned long size);
  _UWORD (* pOS_KeyEventEx)(_UWORD wait, void *p, unsigned long size);

  // ab IMAE Version 7
	unsigned long (* pmyInitGraphEx)(_KOORD, _KOORD, _COLOR, unsigned long*);
	void* (* pmyGetBufferEx)(unsigned long);
	unsigned long (* pmyGetPixelEx)(_KOORD, _KOORD, unsigned long);
	unsigned long (* pmyGetImageEx)(void*, _KOORD, _KOORD, _KOORD, _KOORD, unsigned long);	
	unsigned long (* pmyGetImageSizeEx)( _KOORD, _KOORD, _KOORD, _KOORD, unsigned long);
	void (* pmyLoadUserFontEx)( void*, unsigned long);
	void (* pmyTextDimensionsEx)( _DOT*, char*, unsigned long, unsigned long, unsigned long);
	unsigned long (* pmyTextHeightEx)(short, unsigned long);
	void (* pmyClrScrEx)(_COLOR, unsigned long);	
	void (* pmyPutPixelEx)(_KOORD, _KOORD, _ATTRIB, _COLOR, unsigned long);
	void (* pmyLineEx)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR, unsigned long);
	void (* pmyPolygonEx)(_UDWORD, _DOT*, _ATTRIB, _COLOR, unsigned long);
	void (* pmyRectangleEx)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR, unsigned long);
	void (* pmyBarEx)(_KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR, unsigned long);
	void (* pmyOutTextXYEx)(_KOORD, _KOORD, void*, _ATTRIB, _COLOR, _UWORD, unsigned long);
	void (* pmyScrollEx)(_KOORD, _KOORD, _KOORD, _KOORD, _KOORD, _KOORD, _COLOR, unsigned long);
	void (* pmyPutImageEx)(void*, _KOORD, _KOORD, unsigned long);
	void (* pmyBitmapEx)(_KOORD, _KOORD, _BMP*, unsigned long);
	unsigned long (* pmyMakeRegionEx)(void**, unsigned long);
	void (* pmyKillRegionEx)(void**, unsigned long);
	void (* pmyEmptyRegionEx)(void**, unsigned long, unsigned long);
	void (* pmyActivateRegionEx)(void**, unsigned long);		
	void (* pmyDefaultRegionEx)(unsigned long);
	void (* pmyAndRegionEx)(void**,void**, unsigned long);
	void (* pmyXorRegionEx)(void**,void**, unsigned long);
	void (* pmyOrRegionEx)(void**,void**, unsigned long); 
	void (* pmyBarRegionEx)(void**, _KOORD, _KOORD, _KOORD, _KOORD, _ATTRIB, _COLOR, unsigned long);
	unsigned long (* pmyGetXSizeEx)(unsigned long);
	unsigned long (* pmyGetYSizeEx)(unsigned long);
	unsigned long (* pmyLCDUpdateDoneEx)(unsigned long);
	void (* pmyLCDRefreshEx)(const unsigned char, unsigned long);
  void (* pmyInvertEx)(const unsigned char, unsigned long);	
  int (* pmyHasChanged)( unsigned long );
  // ab IMAE Version 8
  int (* pmyInitHourGlass)(void*, void*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
  int (* pmyActivateHourGlass)(void);
  int (* pmyDeactivateHourGlass)(void);
  int (* pmyCopyBitmap) (_NEWSCREEN*);
  // ab IMAE Version 9
  unsigned long (* pSetNewCalibrationEx)(struct xykoords *pDimension, struct xykoords gridSoll[], struct xykoords gridAct[]);
  
  // Ab IMAE Version 10
  void  (* pFreeImage)(_IMAGE *pimg);

  // Ab IMAE Version 11
  void* (* pCreateRawBuf)(unsigned long width, unsigned long height);
  long  (* pFreeRawBuf)(void *praw);
  long  (* pDisplayRaw)(unsigned long rawFmt, void *pRaw, unsigned long srcOffx, unsigned long srcOffy, unsigned long srcWidth, unsigned long srcHeight,
                        void *tgtImg, unsigned long tgtOffx, unsigned long tgtOffy, unsigned long tgtWidth, unsigned long tgtHeight, void *pRawBuf);
} IMAE;
//end of if-structure

#endif //_LSE_SYS_H

#endif // LSE_IF
