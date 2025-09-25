#ifndef __RTOS__SCAN__
#define __RTOS__SCAN__

#ifdef	LASAL_VERSION
#if		LASAL_VERSION > 57
 #pragma once
#endif//LASAL_VERSION > 57
#endif//LASAL_VERSION

TYPE

	TokenAlt	: (			// result of the scanner function
	  S_END_CONF,		// end of configuration table
	  S_VAR,			// object configuration
	  S_END_VAR,		// end of one object
	  S_CONNECT,		// connect server and client
	  S_LINK,			// try to link unresolved connections
	  S_TASK,			// install rt- or cyclic task
	  S_NONE,
	  S_EOF,
	  S_NUM,
	  S_NUM_1,
	  S_NUM_2,
	  S_NUM_4,
	  S_ID,
	  S_PUNKT,
	  S_KLAMMERAUF,
	  S_KLAMMERZU,
	  S_EKLAUF,
	  S_EKLZU,
	  S_POINT,
	  S_MINUS,
	  S_MODUL,
	  S_KOMMA,
	  S_BSLASH,
	  S_IPR				// interpreter programs
	  )$UDINT;

#ifdef LASAL_ZWEI
	CltChCmd_File : STRUCT
		pCh			: ^File;
		dData		: DINT;
		pCmd		: ^File;
		END_STRUCT;

	ScanMode		: BDINT
	[
		TokenMode       ,	// each token is terminated by a zero
		ExprMode        ,	// every expression is terminated by a zero
	];

	SvrCh_ScanMode	: STRUCT
		pMeth		: ^void;
		dData		: ScanMode;
		pDsc		: ^void;
	END_STRUCT;
#endif // LASAL_ZWEI

END_TYPE

#ifdef LASAL_ZWEI


Scan			: CLASS
	nToken			: TOKEN;
	szLexem			: ARRAY [0..NLNG] OF CHAR;
	udNum			: UDINT;
	pFile			: CltChCmd_FILE;
	uiMode			: SvrCh_ScanMode;

// =========================================================================================
// Functions to scan encoded text
// =========================================================================================

	FUNCTION GLOBAL ScanText
		VAR_OUTPUT
			nScanned		: TOKEN;
		END_VAR;

	FUNCTION GLOBAL ScanCo
		VAR_INPUT
			pScanFile		: ^FILE;
		END_VAR;

	FUNCTION GLOBAL GetFile
		VAR_OUTPUT
			pFile			: ^FILE;
		END_VAR;

	FUNCTION GLOBAL GetLexem
		VAR_OUTPUT
			pLexem			: ^char;
		END_VAR;

	FUNCTION GLOBAL Scan::GetLastToken
		VAR_OUTPUT
			nToken		: TOKEN;
		END_VAR;
END_CLASS;
#endif //LASAL_ZWEI
#endif // __RTOS__SCAN__