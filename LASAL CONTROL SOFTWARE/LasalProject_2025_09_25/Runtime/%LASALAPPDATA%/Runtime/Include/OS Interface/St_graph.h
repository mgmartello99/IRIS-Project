#pragma once
#define _T_TYPES_H


#include "ST_GRCONST.H"   // constants used by graphics

TYPE

 #pragma pack (push, 1)

  _BYTE      : SINT;                                               // 1 byte
  _UBYTE     : USINT;                                              // 1 byte
  _WORD      : INT;                                                // 2 byte
  _UWORD     : UINT;                                               // 2 byte
  _DWORD     : DINT;                                               // 4 byte
  _UDWORD    : UDINT;                                              // 4 byte
  _REAL      : REAL;                                // 4 byte floating point
  _HANDLE    : DINT;                                               // 4 byte

	_LANGUAGE  : _UWORD;                                    // language
	_FONT      : _UBYTE;                                    // no of font
	_ENABLE    : _UBYTE;                                    // key access
	_IVALUE    : _DWORD;                                    // value datatype
	_ASCII     : CHAR;                             // always 1 byte (ascii-code)
	_UNI       : _UWORD;                           // always 2 byte (uni-code)
	_REGION    : pVoid;                                     // region (viewport)
	_ATTRIB    : _UDWORD;                                   // attribute
	_COLOR     : _UDWORD;                          // fore and backgroundcolor
	_KOORD     : _WORD;                                     // single koordinate
	_FONTPTR   : _UBYTE;                           // fontdata (used as pointer)

 _DASH:STRUCT
   dashSize    : _WORD;
   dashRsvd    : _WORD;
   dashList    :^_UBYTE;
 END_STRUCT;

 _DOT:STRUCT
   x           : _KOORD;
   y           : _KOORD;
 END_STRUCT;

 _ROOM : STRUCT
   xy1         : _DOT;
   xy2         : _DOT;
 END_STRUCT;

 _BMPDAT : STRUCT
   no          : _UBYTE;
   col         : _UWORD;
 END_STRUCT;

 #define BMP_TURN           0x0001
 #define BMP_HICOLOR        0x0002
 #define BMP_NO_TRANSPARENT 0x0004

 _BMP : STRUCT
   width       : _UWORD;
   height      : _UWORD;
   transparent : _UWORD;
   state       : _UWORD;
   datano      : _UDWORD;
   ptr         : ^_BMPDAT;
 END_STRUCT;

 _RGB:STRUCT
   cred        : _UWORD;
   cgreen      : _UWORD;
   cblue       : _UWORD;
 END_STRUCT;

 _IMAGE:STRUCT
   ptr         : ^void;
   room        : _ROOM;
 END_STRUCT;

 _NEWSCREEN : STRUCT
   ptr         : ^void;
   width       : _UWORD;
   height      : _UWORD;
 END_STRUCT;

 XYKOORDS : STRUCT
   X	: DINT;
   Y	: DINT;
 END_STRUCT;

 CALIBDATA : STRUCT
	soll	: ARRAY[0..2] OF XYKOORDS;
	act	 	: ARRAY[0..2] OF XYKOORDS;
 END_STRUCT;

 #pragma pack (pop)

END_TYPE

