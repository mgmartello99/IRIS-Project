#ifndef  __LSL_PDFLIB_H
#pragma once
#define  __LSL_PDFLIB_H

// Supported file formats
// Ascii files CP1252 (ISO 8859-1)
// UTF-8 files with BOM
// UCS2 files (little endian) with BOM

(* Paper size *)
#define PDF_PAGE_SIZE_LETTER		0
#define PDF_PAGE_SIZE_LEGAL			1
#define PDF_PAGE_SIZE_A3			2
#define PDF_PAGE_SIZE_A4			3
#define PDF_PAGE_SIZE_A5			4
#define PDF_PAGE_SIZE_B4			5
#define PDF_PAGE_SIZE_B5			6
#define PDF_PAGE_SIZE_EXECUTIVE		7
#define PDF_PAGE_SIZE_US4x6			8
#define PDF_PAGE_SIZE_US4x8			9
#define PDF_PAGE_SIZE_US5x7			10
#define PDF_PAGE_SIZE_COMM10		11

(* Paper oriantation *)
#define PDF_PAGE_PORTRAIT			0
#define PDF_PAGE_LANDSCAPE			1

(* Errors *)
#define PDFLIB_IS_STILL_RUNNING					-1
#define PDFLIB_NO_OS_HEAP_AVAILABLE				-2
#define PDFLIB_WRONG_FONT_FILE					-3
#define PDFLIB_WRONG_TEXT_FILE					-4
#define PDFLIB_WRONG_PDF_FILE					-5
#define PDFLIB_CAN_NOT_CREATE_THREAD			-6
#define PDFLIB_CAN_NOT_OPEN_TEXT_FILE			-7
#define PDFLIB_WRONG_FILE_FORMAT				-8
#define PDFLIB_CAN_NOT_ALLOC_MEMORY				-9
#define PDFLIB_CAN_NOT_CREATE_PDF				-10
#define PDFLIB_CAN_NOT_SET_UTF_ENCODING			-11
#define PDFLIB_CAN_NOT_SET_COMPRESSION_MODE		-12
#define PDFLIB_CAN_NOT_LOAD_FONT				-13
#define PDFLIB_CAN_NOT_ACCESS_FONT				-14
#define PDFLIB_LINE_TO_LONG						-15
#define PDFLIB_CAN_NOT_CONVERT_STRING			-16
#define PDFLIB_CAN_NOT_CREATE_PAGE				-17
#define PDFLIB_INTERNAL_ERROR_SEE_LOG_FILE		-18


TYPE
	LSL_PDF_LIB_DATA_TYPE	:	STRUCT
		marginLeft		: UDINT;
		marginRight		: UDINT;
		marginTop		: UDINT;
		marginBottom	: UDINT;
		paperSize		: UDINT;
		orientation		: UDINT;
		pFontFile		: ^CHAR;
		fontSize		: UDINT;
		pTextFile		: ^CHAR;
		pPdfFile		: ^CHAR;
	END_STRUCT;
END_TYPE

TYPE
	LSL_PDF_LIB_TYPE	:		STRUCT
		version			: UDINT;
		TextToPdf		: pVoid;
		TextToPdfIsRunning	: pVoid;
	END_STRUCT;
END_TYPE

////////////////////////////////////////////////////////////////////////////////
// LASAL Function Prototypes
////////////////////////////////////////////////////////////////////////////////
FUNCTION __CDECL GLOBAL P_TextToPdf
	VAR_INPUT
		osPdf			: ^LSL_PDF_LIB_DATA_TYPE;
	END_VAR
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

FUNCTION __CDECL GLOBAL P_TextToPdfIsRunning
	VAR_OUTPUT
		retval (EAX) 	: DINT;
	END_VAR;

	
////////////////////////////////////////////////////////////////////////////////
// LASAL MACROS
////////////////////////////////////////////////////////////////////////////////
#define OS_PDF_TEXT_TO_PDF(p1) PDF_LIB^.TextToPdf $ P_TextToPdf(p1)
#define OS_PDF_TEXT_TO_PDF_IS_RUNNING() PDF_LIB^.TextToPdfIsRunning $ P_TextToPdfIsRunning()

#endif
