//HFILTER:1
#ifndef __LSL_ST_PDFVIEW_H_
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define __LSL_ST_PDFVIEW_H_

// PDF Error values
#define PDF_ERR_NONE             0
#define PDF_ERR_PARAM           -1000
#define PDF_ERR_NOMEM           -1001
#define PDF_ERR_IFACE           -1002
#define PDF_ERR_INVALID         -1003
#define PDF_ERR_OFFSCREEN       -1004
#define PDF_ERR_LOAD            -1005
#define PDF_ERR_NOT_LOADED      -1006
#define PDF_ERR_PAGE            -1007
#define PDF_ERR_INUSE           -1008
#define PDF_ERR_NO_LINK         -1009

// PDF Options
#define PDF_OPT_FONTANTIALIAS   1
#define PDF_OPT_OWNER_PASS      2
#define PDF_OPT_USER_PASS       3
#define PDF_OPT_TEMP_DRV        4

// LINK Action
#define PDF_LINK_ACTION_GOTO        0			// go to destination
#define PDF_LINK_ACTION_GOTOR       1 		// go to destination in new file
#define PDF_LINK_ACTION_LAUNCH			2     // launch app (or open document)
#define PDF_LINK_ACTION_URL         3			// URL
#define PDF_LINK_ACTION_NAMED       4			// named action
#define PDF_LINK_ACTION_MOVIE       5			// movie action
#define PDF_LINK_ACTION_UNKNOWN     6			// anything else

// PDF Info
#define PDF_INFO_FILENAME       0x00000001
#define PDF_INFO_PATH           0x00000002
#define PDF_INFO_TITLE          0x00000004
#define PDF_INFO_AUTHOR         0x00000008
#define PDF_INFO_SUBJECT        0x00000010
#define PDF_INFO_KEYWORDS       0x00000020
#define PDF_INFO_CREATOR        0x00000040
#define PDF_INFO_PRODUCER       0x00000080
#define PDF_INFO_CREATIONDATE   0x00000100
#define PDF_INFO_MODDATE        0x00000200
#define PDF_INFO_TAGGED         0x00000400
#define PDF_INFO_PAGES          0x00000800
#define PDF_INFO_ENCRYPTED      0x00001000
#define PDF_INFO_PAGESIZE       0x00002000
#define PDF_INFO_FILESIZE       0x00004000
#define PDF_INFO_OPTIMIZED      0x00008000
#define PDF_INFO_PDFVERSION     0x00010000

#define PDF_INFO_ALL_V1         0x0001FFFF

#pragma pack (push,1)

TYPE

  LSL_PDF_DATETIME    : STRUCT

    _day            : USINT;
    _month          : USINT;
    _year           : UINT;
    _hour           : USINT;
    _min            : USINT;
    _sec            : USINT;

  END_STRUCT;

  LSL_PDF_ENCRYPTION  : STRUCT

    encrypted       : SINT;

    okToPrint       : SINT;
    okToCopy        : SINT;
    okToChange      : SINT;
    okToAddNotes    : SINT;

  END_STRUCT;

  LSL_PDF_INFO        : STRUCT

#   define LSL_PDF_INFO_VERSION 1

    udVersion       : UDINT;

    filename        :^CHAR;
    path            :^CHAR;
    title           :^CHAR;
    author          :^CHAR;
    subject         :^CHAR;
    keywords        :^CHAR;
    creator         :^CHAR;
    producer        :^CHAR;

    creationDate    : LSL_PDF_DATETIME;
    modDate         : LSL_PDF_DATETIME;

    tagged          : SINT;

    pages           : UDINT;

    encryption      : LSL_PDF_ENCRYPTION;

    pageSizeWidth   : UINT;
    pageSizeHeight  : UINT;

#   define PDF_PAGE_FORMAT_A0     1
#   define PDF_PAGE_FORMAT_A1     2
#   define PDF_PAGE_FORMAT_A2     3
#   define PDF_PAGE_FORMAT_A3     4
#   define PDF_PAGE_FORMAT_A4     5
#   define PDF_PAGE_FORMAT_A5     6
#   define PDF_PAGE_FORMAT_A6     7
#   define PDF_PAGE_FORMAT_LETTER 8

    pageFormat      : UDINT;

    optimized       : SINT;

    filesize        : UDINT;

    pdfVersion      : REAL;

  END_STRUCT;

  LSL_PDFVIEWER : STRUCT

    udVersion                             : UDINT;
    udSize                                : UDINT;

    pPDFCreateOffscreen                   : pvoid;
    pPDFDestroyOffscreen                  : pvoid;
    pPDFRegisterUpdateOffscreenCallback   : pvoid;
    pPDFGetCurrentOffscreen               : pvoid;
    pPDFSetCurrentOffscreen               : pvoid;
    pPDFGetAbsoluteResolution             : pvoid;
    pPDFGetRelativeResolution             : pvoid;
    pPDFLoad                              : pvoid;
    pPDFView                              : pvoid;
    pPDFDone                              : pvoid;
    pPDFSetOption                         : pvoid;
    pPDFGetOption                         : pvoid;
    pPDFInfo                              : pvoid;
    pPDFIsLink                            : pvoid;
    pPDFfindDest                          : pvoid;
    pPDFsetCallback                       : pvoid;

  END_STRUCT;

END_TYPE

#pragma pack (pop)


FUNCTION GLOBAL __CDECL P_PDFCreateOffscreen
VAR_INPUT
  pOffscreen      :^_NEWSCREEN;
  uWidth          : UINT;
  uHeight         : UINT;
  udSize          : UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFDestroyOffscreen
VAR_INPUT
  pOffscreen      :^_NEWSCREEN;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFRegisterUpdateOffscreenCallback
VAR_INPUT
  pCallback       : pvoid;
  pThis           : pvoid;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFGetCurrentOffscreen
VAR_INPUT
  pOffscreen      :^_NEWSCREEN;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFSetCurrentOffscreen
VAR_INPUT
  pOffscreen      :^_NEWSCREEN;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFGetAbsoluteResolution
VAR_INPUT
  udHDPI          : REAL;
  udVDPI          : REAL;
  udPage          : UDINT;
  pWidth          :^UINT;
  pHeight         :^UINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFGetRelativeResolution
VAR_INPUT
  uWidth          : UINT;
  uHeight         : UINT;
  udPage          : UDINT;
  pHDPI           :^REAL;
  pVDPI           :^REAL;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFLoad
VAR_INPUT
  strPDFFilename  :^CHAR;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFView
VAR_INPUT
  pOffscreen      :^_NEWSCREEN;
  udHDPI          : REAL;
  udVDPI          : REAL;
  udPage          : UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFDone
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFSetOption
VAR_INPUT
  udOption        : UDINT;
  optVal          :^VOID;
  optValLen       :^UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFGetOption
VAR_INPUT
  udOption        : UDINT;
  optVal          :^VOID;
  optValLen       :^UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFInfo
VAR_INPUT
  udInfoID        : UDINT;
  pPDFInfo        :^LSL_PDF_INFO;
  udVersion       : UDINT;
  pInfoData       :^CHAR;
  pSize           :^UDINT;
  udPage          : UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFIsLink
VAR_INPUT
  udX             : UDINT;
  udY             : UDINT;
  pType           :^UDINT;
  pData           : PVOID;
  pSize           :^UDINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFfindDest
VAR_INPUT
  pDest           :^CHAR;
  pData           : PVOID;
  pSize           :^UDINT;
  inLink          : UINT;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;

FUNCTION GLOBAL __CDECL P_PDFsetCallback
VAR_INPUT
  StopOpenIf      : ^VOID;
  ThisPointer     : ^VOID;
END_VAR
VAR_OUTPUT
  dRC             : DINT;
END_VAR;


#define OS_PDFVersion                                 lsl_pdf_viewer^.udVersion

#define OS_PDFCreateOffscreen(p1,p2,p3,p4)            lsl_pdf_viewer^.pPDFCreateOffscreen $ P_PDFCreateOffscreen(p1,p2,p3,p4)
#define OS_PDFDestroyOffscreen(p1)                    lsl_pdf_viewer^.pPDFDestroyOffscreen $ P_PDFDestroyOffscreen(p1)
#define OS_PDFRegisterUpdateOffscreenCallback(p1,p2)  lsl_pdf_viewer^.pPDFRegisterUpdateOffscreenCallback $ P_PDFRegisterUpdateOffscreenCallback(p1,p2)
#define OS_PDFGetCurrentOffscreen(p1)                 lsl_pdf_viewer^.pPDFGetCurrentOffscreen $ P_PDFGetCurrentOffscreen(p1)
#define OS_PDFSetCurrentOffscreen(p1)                 lsl_pdf_viewer^.pPDFSetCurrentOffscreen $ P_PDFSetCurrentOffscreen(p1)
#define OS_PDFGetAbsoluteResolution(p1,p2,p3,p4,p5)   lsl_pdf_viewer^.pPDFGetAbsoluteResolution $ P_PDFGetAbsoluteResolution(p1,p2,p3,p4,p5)
#define OS_PDFGetRelativeResolution(p1,p2,p3,p4,p5)   lsl_pdf_viewer^.pPDFGetRelativeResolution $ P_PDFGetRelativeResolution(p1,p2,p3,p4,p5)
#define OS_PDFLoad(p1)                                lsl_pdf_viewer^.pPDFLoad $ P_PDFLoad(p1)
#define OS_PDFView(p1,p2,p3,p4)                       lsl_pdf_viewer^.pPDFView $ P_PDFView(p1,p2,p3,p4)
#define OS_PDFDone()                                  lsl_pdf_viewer^.pPDFDone $ P_PDFDone()
#define OS_PDFSetOption(p1,p2,p3)                     lsl_pdf_viewer^.pPDFSetOption $ P_PDFSetOption(p1,p2,p3)
#define OS_PDFGetOption(p1,p2,p3)                     lsl_pdf_viewer^.pPDFGetOption $ P_PDFGetOption(p1,p2,p3)
#define OS_PDFInfo(p1,p2,p3,p4,p5,p6)                 lsl_pdf_viewer^.pPDFInfo $ P_PDFInfo(p1,p2,p3,p4,p5,p6)
#define OS_PDFIsLink(p1,p2,p3,p4,p5)                  lsl_pdf_viewer^.pPDFIsLink $ P_PDFIsLink(p1,p2,p3,p4,p5)
#define OS_PDFfindDest(p1,p2,p3,p4)                   lsl_pdf_viewer^.pPDFfindDest $ P_PDFfindDest(p1,p2,p3,p4)
#define OS_PDFsetCallback(p1,p2)                      lsl_pdf_viewer^.pPDFsetCallback $ P_PDFsetCallback(p1,p2)

//HFILTER:1
#endif // __LSL_ST_PDFVIEW_H_
//HFILTER:
