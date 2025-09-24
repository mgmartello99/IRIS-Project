#include "RTOS_PrivHeader.h"
//#include <RTOS_Header.h>
//#include <LSL_string.h>
#include <RTOS_Private.h>
#include ".\include\HashTab.h"

TYPE
  EL_TAB_MAP : STRUCT
    nEntries : UDINT; // Anzahl der Elemente in der Map
    elements : ARRAY [0..1] OF ^CHAR; // Elemente (Anzahl wird erst zur Laufzeit beim Alloc festgelegt)
  END_STRUCT;
  P_EL_TAB_MAP : ^EL_TAB_MAP;
  // Struktur zum Abspeichern von Zusatzinfo eines Advanced IO Elements
  ADV_IO_EL_INFO : STRUCT
    pObj : ^VirtualBase; // Zeiger auf ein Advanced IO Element
    uiCmdHwConn : UINT; // Kommando zur Prüfung mit LDR_IsHWConnected
    uiCmdSimConn : UINT; // Kommando zur Prüfung mit LDR_IsSimConnected
  END_STRUCT;
END_TYPE


VAR_EXTERNAL
	pMapper				: ^CRCMap;		 // to find faster the CRC and the objects
  clsHashTab          : HashTable;
END_VAR

FUNCTION GLOBAL _LookUpObjLex
VAR_INPUT
	pLexem          : ^CLexem;
END_VAR
VAR_OUTPUT
	ret				: ^SvrCh;
END_VAR;


FUNCTION GLOBAL LDR_MM_StatNameGetNr
VAR_INPUT
  pszName : ^CHAR;
END_VAR
VAR_OUTPUT
  udStationId : UDINT;
END_VAR;


FUNCTION Search_MM_ELEMENTS_TAB
VAR_INPUT
	pKey : pChar;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR;

FUNCTION Search_CH_REDIRECT_EMB_TAB
VAR_INPUT
	pKey : pChar;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR;

FUNCTION InitIoElementsTab;
FUNCTION InitMmElementsTab;
FUNCTION InitChRedirectEmbTab;


VAR_PRIVATE
	ioElSvrMap : ^EL_TAB_MAP; // Sortierte List mit Adressen von Server IO-Elementen (_IO_ELEMENTS_TAB)
	ioElCltMap : ^EL_TAB_MAP; // Sortierte List mit Adressen von Client IO-Elementen (_IO_ELEMENTS_TAB)
	ioElAdvMap : ^EL_TAB_MAP; // Sortierte List mit Adressen von Advanced IO-Elementen (_IO_ELEMENTS_TAB)
	mmElMap : ^EL_TAB_MAP; // Sortierte List mit Adressen von MM-Elementen (_MM_ELEMENTS_TAB)
  chRedirectEmbMap : ^EL_TAB_MAP; // Sortierte List mit Adressen von CH-Redirect-Embeddes-Elementen (_CH_REDIRECT_EMB_TAB)
  AdvIoElWithNewinstList : ^ADV_IO_EL_INFO; // Liste mit Advanced IO Element Objekten, bei denen ein Newinst Aufruf möglich ist
  AdvIoElWithNewinstListSize : UDINT;
  AdvIoElWithNewinstListAlloced : UDINT;
END_VAR

(* Initialisierung des Lookup.st Moduls *)
FUNCTION GLOBAL LDR_InitLookup
  InitIoElementsTab();
  InitMmElementsTab();
  InitChRedirectEmbTab();
  AdvIoElWithNewinstList := NIL;
  AdvIoElWithNewinstListSize := 0;
  AdvIoElWithNewinstListAlloced := 0;
END_FUNCTION

#ifdef OTTOSPEED

FUNCTION GLOBAL TAB _chrUpTab;

(* Funktion, mit der ein Hashwert eines ClsHdr's gebildet wird *)
#ifdef _LSL_TARGETARCH_ARM
FUNCTION ClsHash
VAR_INPUT
	Key : pVoid;  // ClsHdrConst
END_VAR  
VAR_OUTPUT
  idx : UDINT;
END_VAR
VAR
  pDsc : ^ClsHdrConst;
  c    : ^char;
  cup  : char;
  pct  : ^usint;
END_VAR

  idx := 5381;
  pct := #_chrUpTab() + 4;
  
  pDsc := Key$^ClsHdrConst;
  c    := #pDsc^.Name.Text[0];
  cup  := c^;
  
  WHILE cup DO 
    idx := ((idx ROL 5) + idx) + (pct + cup)^;
    c   += 1;
    cup := c^;
  END_WHILE;
  
  idx := idx AND 0x7FFFFFFF;

END_FUNCTION
#else // _LSL_TARGETARCH_ARM
FUNCTION AWL ClsHash
VAR_INPUT
	key : ^void;  // ClsHdrConst
END_VAR  
VAR_OUTPUT
  idx : UDINT;
END_VAR

  push     esi
  l.ebx    5381
  l.edi    key
  add.edi  ClsHdrConst.Name.Text
  l.esi    #_chrUpTab()
  add.esi  4
  
_clshdr_loop
  lzxb.eax (edi)
  o.eax    eax
  jz       _clshdr_fin
  lzxb.eax (esi + eax)
  
  inc      edi
  l.ecx    ebx
  rol.ebx  5
  add.ebx  ecx
  add.ebx  eax
  jmp      _clshdr_loop
  
_clshdr_fin
  and.ebx  16#7FFFFFFF
  s.ebx    idx
  pop      esi
END_FUNCTION
#endif // _LSL_TARGETARCH_ARM

#else // OTTOSPEED
FUNCTION ClsHash
VAR_INPUT
	Key : pVoid;  // ClsHdrConst
END_VAR  
VAR_OUTPUT
  idx : UDINT;
END_VAR
VAR
  pDsc : ^ClsHdrConst;
  c : ^CHAR;
  c_upper : CHAR;
END_VAR

  idx := 5381;
  
  pDsc := Key$^ClsHdrConst;
  c := #pDsc^.Name.Text[0];
  WHILE c^ DO 
    IF c^ >= 'a' & c^ <= 'z' THEN
      c_upper := c^ - 'a' + 'A';
    ELSE
      c_upper := c^;
    END_IF;
    idx := ((idx ROL 5) + idx) + c_upper;  
    c += 1;
  END_WHILE;
  
  idx := idx AND 0x7FFFFFFF;

END_FUNCTION
#endif // OTTOSPEED

(* Vergleichsfunktion für die ClsHdr Elemente in der Hashtabelle *)
FUNCTION ClsCmp
VAR_INPUT
	Element : pVoid;
	Key : pVoid;
  cookie : pVoid;
END_VAR  
VAR_OUTPUT
  result : DINT;
END_VAR
VAR
  pDscKey : ^ClsHdrConst;
  pDsc : ^ClsHdrConst;
	pName					: ^CLexem;
	udRev					: Revision;
	nExact					: UDINT;
END_VAR

  result := 0;

  pDsc	:= Element$^ClsHdrConst;
  pDscKey := Key$^ClsHdrConst;
  pName := #pDscKey^.Name;
  udRev := pDscKey^.udRev;
  IF cookie THEN
    nExact := cookie$^UDINT^;
  ELSE
    nExact := 2;
  END_IF;

#ifndef DUMMY_LOADER
  IF pDsc^.Name.iCompare(pName) THEN
    IF nExact >= 2 & udRev    = pDsc^.udRev    |	// exact revision checking
       nExact =  1 & udRev.Hi = pDsc^.udRev.Hi |	// higher revision checked
       nExact =  0 THEN							// no revision checking
      result := 1;
      RETURN;
    END_IF;
  END_IF;
#endif  

END_FUNCTION

(* Initialisiert die Hashtabelle für die ClsHdr Elemente *)
FUNCTION GLOBAL LDR_ClsHashTable_Initialize
VAR_OUTPUT
  clsHashTab : HashTable;
END_VAR
  clsHashTab := HashTable_Initialize(300, #ClsHash(), #ClsCmp());
END_FUNCTION

(* Fügt ein ClsHdr Element in die Hashtable ein *)
FUNCTION GLOBAL LDR_ClsHashTable_Insert
VAR_INPUT
  clsHashTab : HashTable;
	pCls : ^ClsHdr;
END_VAR

  IF clsHashTab = NIL THEN
    RETURN;
  END_IF;
  
  IF HashTable_Insert(pCls^.pDsc(*Key*), pCls(*Value*), clsHashTab, NIL) < 0 THEN
    // Das Einfügen ist nicht gut gegangen, die Hashtabelle wird daher zerstört, 
    // damit später keine falschen Suchergebnisse entstehen
    HashTable_Destroy(#clsHashTab);    
  END_IF;

END_FUNCTION

//    
//      pCls := _LookupCls(pName);
//    
//    Searches for the classes technical name throughout the
//    class table.
//    
//    -> pName: technical class name
//    <- pCls : pointer to the class descriptor if found
//              NIL if the name was not found

FUNCTION GLOBAL _LookupCls
VAR_INPUT
	pName           : ^CHAR;
END_VAR
VAR_OUTPUT
	pCls            : ^ClsHdr;
END_VAR
VAR
  ClsName         : CLocLex;
	nExact					: UDINT;
  searchRes : pVoid;
  pDscKey : ^ClsHdrConst;
  placeholder : ARRAY[0..SIZEOF(ClsHdrConst)+SIZEOF(CLocLex)] OF CHAR;
END_VAR

#ifndef DUMMY_LOADER
  ClsName.InitText(pName);
#endif  
  
  IF clsHashTab THEN
    pDscKey := #placeholder$ClsHdrConst;
    _memcpy(#pDscKey^.Name, #ClsName, SIZEOF(CLocLex));
    nExact := 0; // // no revision checking
    searchRes := HashTable_Find(pDscKey, clsHashTab, #nExact);
    IF searchRes THEN
      pCls := HashTable_RetrieveVal(searchRes)$^ClsHdr;
    ELSE
      pCls := NIL;
    END_IF;
    RETURN;
  ELSE
    pCls  := _GetClsList();

    WHILE pCls <> NIL DO

  #ifndef DUMMY_LOADER
      IF ClsName.iCompare(#pCls^.pDsc^.Name) THEN
        RETURN;
      END_IF;
  #endif    

      pCls := pCls^.pNxtCls;

    END_WHILE;
  END_IF;

END_FUNCTION
//		 uiChNo := _LookUpChannelC
//	
//	Searches for a channel name in a given class.
// -> pName : channel name
// -> ppCls : pointerpointer to class header
// <- uiChNo: -1 if not found
//				else: name is a channel of the class
//
FUNCTION	_LookUpChannelC
VAR_INPUT
	pName			: ^CHAR;		// channel name
	ppCls           : ^pClsHdr;		// return value
END_VAR
VAR_OUTPUT
	uiChno			: UINT;			// channel number
END_VAR
VAR
	lexem			: CLocLex;
END_VAR

#ifndef DUMMY_LOADER
	lexem.InitText(pName);
#endif  
	uiChNo	:= _LookUpChannel((#lexem) $ ^CLexem, ppCls);

END_FUNCTION
// Search for a channel name in a given class and return a pointer to it's
// descriptor, or NIL if the name is not a known channel name.
FUNCTION GLOBAL _LookupChPtr
VAR_INPUT
	pCls		: ^ClsHdr;
	pName		: ^char;
END_VAR
VAR_OUTPUT
	pCh			: ^ChDsc;
END_VAR
VAR
	uiCh		: UINT;
END_VAR

	uiCh	:= _LookupChannelC(pName, #pCls);
	IF uiCh = 16#FFFF THEN
		pCh	:= NIL;
		RETURN;
	END_IF;

	pCh	:= #pCls^.aDscChs[uiCh];

END_FUNCTION
//    
//       (pCh, eChMode) := _LookupExpr(pExpr);
//    
//    This function parses an ASCII - string and returns the
//    address of the object or channel it describes. The result
//    has to be casted to the appropriate data type, which is
//    given by the channel mode.
//    
//    
//    -> pText   : pointer to the text buffer
//    <- pObj_Ch : pointer to the object or channel described
//                 by the given name
//    <- pObj    : if the text describes a channel and pObj is
//                 <> NIL, then it contains the pointer to the
//                 command channel of the object
//    <- chMode  : fixes the type of pObj_Ch
//    
//    Examples: "Control"             // object
//              "Control.SetValue"    // data
//              "Control.Cmd"         // command
//              "HeatingZone.DH"      // object channel
FUNCTION GLOBAL __CDECL _LookUpEmbed_C
VAR_INPUT
	pText           : PCHAR;
	pObj_ch			: ^PVOID;
	pObj			: ^pVirtualBase;
END_VAR
VAR_OUTPUT
	chMode          : CHMODE;
END_VAR

	chMode := _LookupEmbedded(#pText, pObj_Ch, pObj);

END_FUNCTION

FUNCTION GLOBAL F_RD_USINT
VAR_INPUT
  ppFile    : ^pVoid;
END_VAR
VAR_OUTPUT
  val       : USINT;
END_VAR

  val     := ppFile^^ $ USINT;
  ppFile^  += sizeof(USINT);
  
END_FUNCTION
FUNCTION GLOBAL F_RD_UDINT
VAR_INPUT
  ppFile    : ^pVoid;
END_VAR
VAR_OUTPUT
  val       : UDINT;
END_VAR

  val     := ppFile^^ $ UDINT;
  ppFile^  += sizeof(UDINT);
  
END_FUNCTION
FUNCTION GLOBAL F_RD_UINT
VAR_INPUT
  ppFile    : ^pVoid;
END_VAR
VAR_OUTPUT
  val       : UINT;
END_VAR

  val     := ppFile^^ $ UINT;
  ppFile^  += sizeof(UINT);
  
END_FUNCTION

FUNCTION ScanLexem
VAR_INPUT
	ppScan		: ^pChar;
	Lexem		: pChar;
END_VAR
VAR_OUTPUT
	token		: TokenNew;
END_VAR
VAR
	c			: CHAR;
	pScan		: pChar;
END_VAR

  token := SN_INVALID_CHAR;

	Lexem^	:= 0;
  
	pScan	:= ppScan^;

	CASE pScan^ OF
		'A'..'Z',
		'a'..'z',
		'1'..'9',
		'_'
#ifdef LDR_RETSVR    
    ,'#'
#endif
    :
			WHILE TRUE DO
				c	:= pScan^;
				CASE c OF
					'A'..'Z',
					'a'..'z',
					'0'..'9',
					'_'
#ifdef LDR_RETSVR    
          ,'#'
#endif
          ,' '  // Ab 02.02.196 kann auch ein Leerzeichen im Namensteil enthalten sein, da bei 
                // Multimaster Objekten der Projektname enthalten ist, welcher Leerzeichen enthalten kann.
          :
						Lexem^	:= c;
						Lexem	+= 1;
						pScan	+= 1;
					ELSE
						Lexem^	:= 0;
						EXIT;
				END_CASE;
			END_WHILE;
			token	:= SN_ID;

		'.':
			pScan	+= 1;
			token	:= SN_PUNKT;

		'\':
			pScan	+= 1;
			token	:= SN_BSLASH;

		0:
			pScan	+= 1;
			token	:= SN_EOF;
	END_CASE;

	ppScan^	:= pScan;

END_FUNCTION

// Wie LDR_LookupEmbedded3, nur dass zusätzlich ein Zeiger auf einen ChDsc-Zeiger 
// übergeben werden kann (ppCD), der mit der Adresse des ChDsc des gefundenen Channels 
// beschrieben wird. Der Parameter ppCD kann auch NIL sein, dann wird nichts zurückgegeben.
//    
FUNCTION GLOBAL LDR_LookupEmbedded4
VAR_INPUT
	pObj            : ^VirtualBase;
	ppScan          : ^pChar;
	pObj_ch         : ^pVoid;
	ppObj           : ^pVirtualBase;
	ppCD            : ^pVoid;
	pErrInfo        : pChar := Nil;
END_VAR
VAR_OUTPUT
	chMode          : CHMODE;
END_VAR
VAR
	eState          : (
		_OBJECT,
		_DOT,
		_CHANNEL,
		_SLASH,
		_CONSTANT)$UDINT;
	uiCh            : UINT;
	pCls            : ^CLSHDR;
	pCD			        : ^ChDsc;
	pChDsc          : ^ChDscEntry;
	bLastObjCh      : UDINT;
	pObjDsc         : ^OBJ;
	pThis           : ^VirtualBase;
	bDoScan			: BOOL;
	Lexem			: ARRAY [0..NLNG+1] OF CHAR;
	token			: TokenNew;
    uiStationID :  UDINT;
	pScan   : pChar;
END_VAR

  // merken falls MultimasterServer
  pScan := ppScan^;
  pCD := NIL;
  
	token		:= ScanLexem(ppScan, #Lexem[0]);
  IF token = SN_INVALID_CHAR THEN
    //TRACE0_ERR("Invalid name in lookup: '{0}'", pScan);
    GOTO OBJ_NOT_FOUND;
  END_IF;
  
  IF (token = SN_ID) THEN
    // falls das erste Token ein '.' ist, dann prüfen ob Station, und dann nach MultimasterServer suchen
    // StationsName.Objectname.Servername
    uiStationID := LDR_MM_StatNameGetNr(#Lexem[0]);
    IF (uiStationID <> (-1$UDINT)) THEN
      pObj$^SvrCh := _LookUpObj(pScan);
      IF (pObj = NIL) THEN
        pScan := Search_MM_ELEMENTS_TAB(pScan);
        IF (pScan <> NIL) THEN
          pObj$^SvrCh := _LookUpObj(pScan);
        END_IF;
      END_IF;
      IF (pObj <> NIL) THEN
        pObjDsc    := LSL_GetHdr(pObj);
        pCls       := pObjDsc^.pClsHdr;
        pThis      := pObj$^VirtualBase;
        chMode := _CH_OBJ; 
        GOTO ENDE;
       END_IF;
    END_IF;
  END_IF;

  IF pObj = NIL THEN
    pObj$^SvrCh := _LookUpObj(#Lexem[0]);
  END_IF;
  IF pObj = NIL THEN
    GOTO OBJ_NOT_FOUND;
  END_IF;
	pObjDsc    := LSL_GetHdr(pObj);
	pCls       := pObjDsc^.pClsHdr;
	eState     := _OBJECT;
	bLastObjCh := TRUE;
	pChDsc     := NIL;
	chMode     := _CH_OBJ;
	uiCh       := 0;
	pThis      := pObj$^VirtualBase;
	bDoScan	 := TRUE;


  WHILE TRUE DO
    IF bDoScan THEN
        token	:= ScanLexem(ppScan, #Lexem[0]);
        IF token = SN_INVALID_CHAR THEN
          //TRACE0_ERR("Invalid name in lookup: '{0}'", pScan);
          GOTO OBJ_NOT_FOUND;
        END_IF;
    ELSE
      bDoScan	:= TRUE;
    END_IF;

    CASE eState OF                                      // scanner state

      _OBJECT:

        CASE token OF                   	// "Object."
          SN_EOF:
            EXIT;                          	// exit for "object1\object2"

          SN_BSLASH:                               // next object follows
            eState := _SLASH;

          SN_PUNKT:
            eState := _DOT;

          ELSE
OBJ_NOT_FOUND:
            chMode := _CH_NONE;      // return at end of connection list
            pObj   := NIL;
            pThis  := NIL;
            uiCh   := 0;
            GOTO ENDE;

        END_CASE;                           // case Token of for objects

      _SLASH:                      // a typed client channel must follow

        CASE token OF

          SN_ID:
            uiCh  := _LookUpChannelC(#Lexem[0], #pCls);

            IF uiCh = -1$UINT THEN
              GOTO OBJ_NOT_FOUND;
            END_IF;

            pCD	:= #pCls^.aDscChs[uiCh];
            pChDsc	:= pCD^.pDsc;
            pObj	+= pChDsc^.uiOff;

CHECK_FOR_OBJ:
            pThis	:= pObj+CltCh.pCmd; // ptr to connected object

            IF pThis$UDINT = pObj$^CltCh^.pCh$UDINT THEN
              eState := _CONSTANT;
            ELSE
              eState := _OBJECT;
            END_IF;
            pThis    := pThis^$^VirtualBase;

            IF pThis = NIL THEN					// channel not yet connected ??
              GOTO OBJ_NOT_FOUND;
            END_IF;

            pObj    := pThis;
            pObjDsc := LSL_GetHdr(pObj);
            pCls    := pObjDsc^.pClsHdr;

          SN_BSLASH:
            chMode := _CH_OBJ_DIR;   // Pg50 must evaluate the rest of
            EXIT;                    // the string

          ELSE
            GOTO OBJ_NOT_FOUND;

        END_CASE;                     // case token of for dot - channel

      _DOT:

        CASE token OF
          SN_EOF:                    // return pointer to command channel
            uiCh := 0;
            GOTO CHANNEL;

          SN_ID:
            uiCh  := _LookUpChannelC(#Lexem[0], #pCls);

            IF uiCh = -1$UINT THEN
              GOTO OBJ_NOT_FOUND;
            END_IF;

          CHANNEL:
            pCD	  := #pCls^.aDscChs[uiCh];
            pChDsc  := pCD^.pDsc;
            chMode  := pChDsc^.uiMode;

            pObj    += pChDsc^.uiOff;
            eState  := _OBJECT;

            token := ScanLexem(ppScan, #Lexem[0]);
            CASE token OF
              SN_PUNKT:	// check if '.chnl' is an object channel
                bDoScan	:= FALSE;	// the dot is already scanned
                
                IF pCD^.pCls & pCD^.pCls^.pDsc & (pCD^.pCls^.pDsc^.ClsType AND CTY_OSI_CLASS) THEN
                  // 8D-8123:
                  // Bei einem Channel, der mit einer OSInterface (OSI) Klasse verbunden ist, 
                  // kann keine weitere Komponente abgefragt werden, weil es zu einer OSI Klasse 
                  // kein Objekt gibt.
				  //[8D-9332] Trnrob/Praerw, 01.08.2024: Suppress trace messages, 
				  // because there is a second way to find server
                  IF pErrInfo = Nil THEN
                    TRACE0_ERR("Name specifies channel with OSI class type!: '{0}'", pScan);
                    LSLCLI_LoaderTracePrint("Name specifies channel with OSI class type!", 12);
                  ELSE
                    pErrInfo^ := 1;
                  END_IF;
                  GOTO OBJ_NOT_FOUND;
                END_IF;

                  CASE chMode OF
                  _CH_CLT_OBJ,		// typed object channel for external objects
                  _CH_CLT_EMB,		// typed object channel for embedded objects 
                  _CH_TEMP_VBI_OBJ,	// for internal use, obj-ch to VBI-obj
                  _CH_TEMP_VBI_EMB:	// for internal use, cmb-ch to VBI-obj
                    GOTO CHECK_FOR_OBJ;
                  ELSE
                    // Only for typed client channels (object channels) an 
                    // additional component can be specified. For all other
                    // channels only the 4 byte data element (.dData) can be
                    // accessed.
 				  //[8D-9332] Trnrob/Praerw, 01.08.2024: Suppress trace messages, 
				  // because there is a second way to find server
                   IF pErrInfo = Nil THEN
                      TRACE0_ERR("Name specifies channel with wrong type, expecting object channel!: '{0}'", pScan);
                      LSLCLI_LoaderTracePrint("Name specifies channel with wrong type, expecting object channel!", 12);
                    ELSE
                      pErrInfo^ := 2;
                    END_IF;
                    GOTO OBJ_NOT_FOUND;
                END_CASE;
              SN_EOF:		// end of object/channel-name
              ELSE		// invalid token
                //TRACE0_ERR("Invalid name in lookup: '{0}'", pScan);
                GOTO OBJ_NOT_FOUND;
            END_CASE;
    
          EXIT;                 // exit for "object1\object2.channelx"
    
        END_CASE;

      _CONSTANT:
        IF token = SN_EOF THEN
          chMode := _CH_CNST;
          EXIT;
        ELSE
          GOTO OBJ_NOT_FOUND;
        END_IF;
  
    END_CASE;                                   // case scanner state of

  END_WHILE;

ENDE:
  IF pObj = NIL | pThis = NIL THEN
    chMode := _CH_NONE;
  END_IF;
  pObj_CH^ := pObj;
  IF ppObj <> NIL THEN
    ppObj^ := pThis;
  END_IF;
  IF ppCD <> NIL THEN
    ppCD^ := pCD;
  END_IF;

END_FUNCTION

//    
//       (pCh, eChMode) := _LookupExpr(pExpr);
//    
//    This function parses an ASCII - string and returns the
//    address of the object or channel it describes. The result
//    has to be casted to the appropriate data type, which is
//    given by the channel mode.
//    
//    -> pObj    : Pointer to the object described by the given name.
//                 Can be NIL when the object is not known, in this case the object 
//                 is searched with _LookUpObj.
//                 When the object is specified, the expensive call to _LookUpObj 
//                 can be skipped. The caller must ensure that the pointer corresponds 
//                 to the name. 
//    -> pScan   : pointer to the text buffer
//    <- pObj_Ch : pointer to the object or channel described
//                 by the given name
//    <- ppObj   : if the text describes a channel and pObj is
//                 <> NIL, then it contains the pointer to the
//                 command channel of the object
//    <- chMode  : returns the type of pObj_Ch
//    
//    Examples: "Control"             // object
//              "Control.SetValue"    // data
//              "Control.Cmd"         // command
//              "HeatingZone\DH"      // object channel

FUNCTION GLOBAL LDR_LookupEmbedded3
VAR_INPUT
	pObj            : ^VirtualBase;
	ppScan          : ^pChar;
	pObj_ch         : ^pVoid;
	ppObj           : ^pVirtualBase;
	pErrInfo        : pChar := Nil;
END_VAR
VAR_OUTPUT
	chMode          : CHMODE;
END_VAR
VAR
	pCD			        : ^ChDsc;
	pScan           : pChar;
	pRedirected           : pChar;
  textbuf : ARRAY [0..255] OF CHAR;
	pObj2           : ^VirtualBase;
	pObjDsc         : ^OBJ;
  str1            : pChar;
  str2            : pChar;
END_VAR
  
  pScan := ppScan^;

  // Da der Parameter ppObj NIL sein kann, hier aber benötigt wird, wird der 
  // Zeiger auf eine lokale Variable gesetzt.
  IF ppObj = NIL THEN
    ppObj := #pObj2;
  END_IF;
  
  chMode := LDR_LookupEmbedded4(pObj, ppScan, pObj_ch, ppObj, #pCD, pErrInfo);
  IF chMode <> _CH_NONE & pCD <> NIL THEN
    IF pCD^.pDsc^.uiFlags.Redirect THEN
      (* Das Objekt wurde gefunden, die Anfrage soll aber mit einem anderen Namen 
         durchgeführt werden - Redirect von emb. String Objekten *)
      pObjDsc    := LSL_GetHdr(ppObj^);
      
      // Stringlängn prüfen, ob Platz in textbuf ist
      str1 := #pObjDsc^.pClsHdr^.pDsc^.Name.Text[0];
      str2 := #pCD^.pDsc^.Name.Text[0];
      IF _strlen(str1) + 1 + _strlen(str2) + 1 > SIZEOF(textbuf) THEN
        RETURN;
      END_IF;
      
      // Key für die CH_REDIRECT_EMB_TAB zusammenbauen ("Klassen-Name;Channel-Name")
      _strcpy(#textbuf[0], str1);
      _strcat(#textbuf[0], ";");
      _strcat(#textbuf[0], str2);
      
      // Wert anhand des Keys suchen
      pRedirected := Search_CH_REDIRECT_EMB_TAB(#textbuf[0]);
      IF pRedirected <> NIL THEN
        // Stringlängn prüfen, ob Platz in textbuf ist
        str1 := #pObjDsc^.pObjDsc^.SymName.Text[0];
        str2 := pRedirected;
        IF _strlen(str1) + _strlen(str2) + 1 > SIZEOF(textbuf) THEN
          RETURN;
        END_IF;
      
        // Der neue Name ist der ursprüngliche Objektname plus der Channel-Name in der CH_REDIRECT_EMB_TAB
        _strcpy(#textbuf[0], str1);
        _strcat(#textbuf[0], str2);
        pRedirected := #textbuf[0];
        chMode := LDR_LookupEmbedded4(NIL, #pRedirected, pObj_ch, ppObj, NIL, pErrInfo);
      END_IF;
    END_IF;
  END_IF;

END_FUNCTION

(*
  Vergleich 2er Strings ohne Beachtung der Groß-/Kleinschreibung.
  Bei Gleichheit wird 0 zurückgegeben, sonst wenn der erste String größer als 
  der zweite ist eine positive Zahl, sonst eine negative.
  Für die Beurteilung welcher String größer ist gilt folgende Reihenfolge:
      1. Zahlen
      2. Unterstrich
      3. Buchstaben (case insensitive)
  Innerhalb der Zahlen und Buchstaben wird der Ascii Wert verwendet. 
  
  Der Unterstrich wäre lt. Ascii Code nach den Zahlen und Buchstaben. 
  Das Lasal sortiert aber auf eine eigene Art, daher ist hier eine Sonderbehandlung 
  erforderlich.
*)
FUNCTION LDR_MyStricmp
VAR_INPUT
	s_1 : pChar;
	s_2 : pChar;
END_VAR
VAR_OUTPUT
	result : DINT;
END_VAR
VAR
  c1 : DINT;
  c2 : DINT;
END_VAR

  // Schleife bis ein String-Ende oder ein Unterschied festgestellt wird
  WHILE TRUE DO
    IF (s_1^ >= 'a') & (s_1^ <= 'z') THEN
      c1 := s_1^ + 'A' - 'a';
    ELSE
      c1 := s_1^;
    END_IF;
    
    IF (s_2^ >= 'a') & (s_2^ <= 'z') THEN
      c2 := s_2^ + 'A' - 'a';
    ELSE
      c2 := s_2^;
    END_IF;
    
    IF (c1 = 0) | (c1 <> c2) THEN
      // String-Ende oder Unterschied festgestellt -> Schleife verlassen
      EXIT;
    END_IF;
    
    s_1 += 1;
    s_2 += 1;
  END_WHILE;
  
  (*
    Lasal sortiert nicht nach dem Ascii Code, sondern auf eine eigene Art:
      1. Zahlen
      2. Unterstrich
      3. Buchstaben
  *)
  IF c1 = '_' THEN
    c1 += 1000;
  ELSIF (c1 >= 'A') & (c1 <= 'Z') THEN
    c1 += 2000;
  END_IF;
  IF c2 = '_' THEN
    c2 += 1000;
  ELSIF (c2 >= 'A') & (c2 <= 'Z') THEN
    c2 += 2000;
  END_IF;
  
  result := c1 - c2;

END_FUNCTION

#include <RTOS_Mem.h>

FUNCTION InitElementsTabMap
VAR_INPUT
  pTab : ^void;
  ppTab : ^pVoid;
END_VAR
VAR_OUTPUT
  pMap : ^EL_TAB_MAP;
END_VAR
VAR
  nEntries : UINT;
  pTemp : pChar;
  i : UINT;
END_VAR

  nEntries := pTab$^UINT^;
	pTab += sizeof(UINT); // Anzahl der Tabelleneinträge

  pMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP) + nEntries * sizeof(UDINT))$^EL_TAB_MAP;
  pMap^.nEntries := nEntries;
  pTemp := pTab$pChar;
  IF nEntries > 0 THEN
    FOR i := 0 TO nEntries - 1 DO
      pMap^.elements[i] := pTemp;
      pTemp := pTemp + _strlen(pTemp) + 1;
      pTemp := pTemp + _strlen(pTemp) + 1;
    END_FOR;
  END_IF;
  
  IF ppTab <> NIL THEN
    ppTab^ := pTemp;
  END_IF;

END_FUNCTION

(* Damit die Suche nach dem Element schneller geht, wird das erste mal eine 
   Tabelle (ioElSvrMap) erstellt, in der die Anfangsadressen der Key/Value 
   Paare gespeichert wird. Da die Einträge in der Tabelle nach dem IO-Element Namen 
   sortiert sind, kann mit dieser ioElSvrMap Tabelle eine binäre Suche 
   durchgeführt werden.
   Achtung:
   Die _ELEMENTS_TAB ist nicht genau nach dem Ascii Wert sortiert sondern folgendermaßen:
    1. Zahlen
    2. Unterstrich
    3. Buchstaben
*)
FUNCTION InitIoElementsTab
VAR
  pTab : ^void;
  version : UINT;
END_VAR

  pTab := _FindFctName("_IO_ELEMENTS_TAB");

  IF pTab = NIL THEN
    // keine Tabelle vorhanden, kann bei älteren Lasal Versionen der Fall sein
    ioElSvrMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP))$^EL_TAB_MAP;
    ioElSvrMap^.nEntries := 0;
    ioElCltMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP))$^EL_TAB_MAP;
    ioElCltMap^.nEntries := 0;
    ioElAdvMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP))$^EL_TAB_MAP;
    ioElAdvMap^.nEntries := 0;
    RETURN;
  END_IF;
  
  (* 
  Aufbau der Tabelle _ELEMENTS_TAB:
    4   Tabellengröße
    2   Versionsnummer
    2   Anzahl der Tabelleneinträge
    n-mal:
      0-terminierter String: ElementName
      0-terminierter String: Objektname(Server)
      
    ab Version >= 1:
    2   Anzahl der Tabelleneinträge
    n-mal:
      0-terminierter String: ElementName
      0-terminierter String: Objektname(Client)
      
    ab Version >= 2:
    2   Anzahl der Tabelleneinträge
    n-mal:
      0-terminierter String: ElementName
      0-terminierter String: Objektname(Name Objekts/Client, das mit dem Advanced IO Element verknüpft ist)
  *)
	pTab += sizeof(UDINT); // Tabellengröße
  version := pTab$^UINT^;
	pTab += sizeof(UINT); // Versionsnummer
  
  ioElSvrMap := InitElementsTabMap(pTab, #pTab);
  
  IF version = 0 THEN
    ioElCltMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP))$^EL_TAB_MAP;
    ioElCltMap^.nEntries := 0;
    ioElAdvMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP))$^EL_TAB_MAP;
    ioElAdvMap^.nEntries := 0;
  ELSE
    ioElCltMap := InitElementsTabMap(pTab, #pTab);
    IF version = 1 THEN
      ioElAdvMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP))$^EL_TAB_MAP;
      ioElAdvMap^.nEntries := 0;
    ELSE
      ioElAdvMap := InitElementsTabMap(pTab, NIL);
    END_IF;
  END_IF;

END_FUNCTION

(* Damit die Suche nach dem Element schneller geht, wird das erste mal eine 
   Tabelle (mmElMap) erstellt, in der die Anfangsadressen der Key/Value 
   Paare gespeichert wird. Da die Einträge in der Tabelle nach dem IO-Element Namen 
   sortiert sind, kann mit dieser mmElMap Tabelle eine binäre Suche 
   durchgeführt werden.
   Achtung:
   Die _ELEMENTS_TAB ist nicht genau nach dem Ascii Wert sortiert sondern folgendermaßen:
    1. Zahlen
    2. Unterstrich
    3. Buchstaben
*)
FUNCTION InitMmElementsTab
VAR
  pTab : ^void;
END_VAR

  pTab := _FindFctName("_MM_ELEMENTS_TAB");

  IF pTab = NIL THEN
    // keine Tabelle vorhanden, kann bei älteren Lasal Versionen der Fall sein
    mmElMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP))$^EL_TAB_MAP;
    mmElMap^.nEntries := 0;
    RETURN;
  END_IF;
  
  (* 
  Aufbau der Tabelle _ELEMENTS_TAB:
    4   Tabellengröße
    2   Versionsnummer
    2   Anzahl der Tabelleneinträge
    n-mal:
      0-terminierter String: ElementName
      0-terminierter String: Objektname(Server)
  *)
	pTab += sizeof(UDINT); // Tabellengröße
	pTab += sizeof(UINT); // Versionsnummer
  
  mmElMap := InitElementsTabMap(pTab, NIL);

END_FUNCTION

(* Damit die Suche nach dem Element schneller geht, wird das erste mal eine 
   Tabelle (chRedirectEmbMap) erstellt, in der die Anfangsadressen der Key/Value 
   Paare gespeichert wird. Da die Einträge in der Tabelle nach dem IO-Element Namen 
   sortiert sind, kann mit dieser chRedirectEmbMap Tabelle eine binäre Suche 
   durchgeführt werden.
   Achtung:
   Die _ELEMENTS_TAB ist nicht genau nach dem Ascii Wert sortiert sondern folgendermaßen:
    1. Zahlen
    2. Unterstrich
    3. Buchstaben
*)
FUNCTION InitChRedirectEmbTab
VAR
  pTab : ^void;
END_VAR

  pTab := _FindFctName("CH_REDIRECT_EMB_TAB");

  IF pTab = NIL THEN
    // keine Tabelle vorhanden, kann bei älteren Lasal Versionen der Fall sein
    chRedirectEmbMap := _alloc_resize_LDR(sizeof(EL_TAB_MAP))$^EL_TAB_MAP;
    chRedirectEmbMap^.nEntries := 0;
    RETURN;
  END_IF;
  
  (* 
  Aufbau der Tabelle _ELEMENTS_TAB:
    4   Tabellengröße
    2   Versionsnummer
    2   Anzahl der Tabelleneinträge
    n-mal:
      0-terminierter String: Name des äusseren Servers
      0-terminierter String: Name des Servers des embedded Objekts, auf den umgeleitet werden soll
  *)
	pTab += sizeof(UDINT); // Tabellengröße
	pTab += sizeof(UINT); // Versionsnummer
  
  chRedirectEmbMap := InitElementsTabMap(pTab, NIL);

END_FUNCTION

(* Ersetzt einen Eintrag in einer EL_TAB_MAP *)
FUNCTION Replace_ELEMENTS_TAB
VAR_INPUT
	pKey : pChar;
	pValue : pChar;
  pMap : ^EL_TAB_MAP;
  index : DINT;
END_VAR
VAR
	lenKey : UDINT;
	lenValue : UDINT;
  pEntry : pChar;
END_VAR

  lenKey := _strlen(pKey);
  lenValue := _strlen(pValue);
  
  pEntry := _alloc_resize_LDR(lenKey + 1(* '\0' *) + lenValue + 1(* '\0' *))$pChar;
  _strcpy(pEntry, pKey);
  _strcpy(pEntry + lenKey + 1, pValue);
  pMap^.elements[index] := pEntry;

END_FUNCTION

(* Fügt einen Eintrag in einer EL_TAB_MAP hinzu *)
FUNCTION Add_ELEMENTS_TAB
VAR_INPUT
	pKey : pChar;
	pValue : pChar;
  ppMap : ^P_EL_TAB_MAP;
END_VAR
VAR
  pMap : ^EL_TAB_MAP;
	lenKey : UDINT;
	lenValue : UDINT;
  pEntry : pChar;
  index : DINT;
  mitte : DINT;
  links : DINT;
  rechts : DINT;
  cmp : DINT;
  pTemp : pChar;
END_VAR

  pMap := ppMap^;

  lenKey := _strlen(pKey);
  lenValue := _strlen(pValue);
  
  pEntry := _alloc_resize_LDR(lenKey + 1(* '\0' *) + lenValue + 1(* '\0' *))$pChar;
  _strcpy(pEntry, pKey);
  _strcpy(pEntry + lenKey + 1, pValue);
  
  IF pMap^.nEntries = 0 THEN
    index := 0;
  ELSE
    // binäre Suche nach dem ersten IO-Element Namen, der größer oder gleich ist
    links := 0;
    rechts := pMap^.nEntries$DINT - 1;
   
    WHILE links <= rechts DO 
      mitte := links + ((rechts - links) / 2);
   
      pTemp := pMap^.elements[mitte];
      cmp := LDR_MyStricmp(pTemp, pKey);
      IF cmp = 0 THEN
        // Eintrag wurde gefunden
        index := mitte;
        EXIT;
      ELSE
        IF cmp > 0 THEN
          // im linken Teil weitersuchen
          rechts := mitte - 1;
          index := mitte;
        ELSE
          // im rechten Teil weitersuchen
          links := mitte + 1;
          index := mitte + 1;
        END_IF;
      END_IF;
    END_WHILE;
  END_IF;
  
  pMap := _realloc_LDR(pMap, sizeof(EL_TAB_MAP) * (pMap^.nEntries + 1))$^EL_TAB_MAP;
	_memmove(#pMap^.elements[index + 1], #pMap^.elements[index], sizeof(EL_TAB_MAP) * (pMap^.nEntries - index$UDINT));
  pMap^.nEntries += 1;
  pMap^.elements[index] := pEntry;
  
  ppMap^ := pMap;

END_FUNCTION

(* Sucht einen Eintrag in einer EL_TAB_MAP anhand des Keys.
   Zurückgegeben wird der gefundene Wert und der Index des Wertes 
   (oder NIL/-1  wenn nicht gefunden).
   Wenn mehrere Einträge mit dem gleichen Key existieren, dann wird 
   der erste (mit dem niedrigsten Index) zurückgegeben.
 *)
FUNCTION Search_ELEMENTS_TAB
VAR_INPUT
	pKey : pChar;
  pMap : ^EL_TAB_MAP;
  pIndex : ^DINT; // optional (wenn NIL dann wird nichts beschrieben)
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR
VAR
  mitte : DINT;
  links : DINT;
  rechts : DINT;
  cmp : DINT;
  pTemp : pChar;
  pTemp2 : pChar;
END_VAR

  pValue := NIL;
  IF pIndex <> NIL THEN
    pIndex^ := -1;
  END_IF;

  IF pMap^.nEntries = 0 THEN
    RETURN;
  END_IF;
  
  // binäre Suche nach dem IO-Element Namen
  links := 0;
  rechts := pMap^.nEntries$DINT - 1;
 
  WHILE links <= rechts DO 
    mitte := links + ((rechts - links) / 2);
 
    pTemp := pMap^.elements[mitte];
    cmp := LDR_MyStricmp(pTemp, pKey);
    IF cmp = 0 THEN
      // Eintrag wurde gefunden
      
      // prüfen, ob davor noch Einträge mit gleichem Key existieren
      WHILE mitte > 0 DO
        pTemp2 := pMap^.elements[mitte-1];
        cmp := LDR_MyStricmp(pTemp2, pKey);
        IF cmp <> 0 THEN
          EXIT;
        END_IF;
        pTemp := pTemp2;
        mitte -= 1;
      END_WHILE;
      
      pValue := pTemp + _strlen(pTemp) + 1;
      IF pIndex <> NIL THEN
        pIndex^ := mitte;
      END_IF;
      EXIT;
    ELSE
      IF cmp > 0 THEN
        // im linken Teil weitersuchen
        rechts := mitte - 1;
      ELSE
        // im rechten Teil weitersuchen
        links := mitte + 1;
      END_IF;
    END_IF;
  END_WHILE;
  
END_FUNCTION

(* Sucht einen Eintrag in einer EL_TAB_MAP anhand des Werts.
   Zurückgegeben wird der gefundene Key und der Index des Keys 
   (oder NIL/-1  wenn nicht gefunden)
 *)
FUNCTION Search_ELEMENTS_TAB_value
VAR_INPUT
	pValue : pChar;
  pMap : ^EL_TAB_MAP;
  pIndex : ^DINT; // optional (wenn NIL dann wird nichts beschrieben)
END_VAR
VAR_OUTPUT
	pKey : pChar;
END_VAR
VAR
  i : UDINT;
  cmp : DINT;
  pTemp : pChar;
END_VAR

  pKey := NIL;
  IF pIndex <> NIL THEN
    pIndex^ := -1;
  END_IF;

  IF pMap^.nEntries = 0 THEN
    RETURN;
  END_IF;
  
  // Suche nach dem Wert
  FOR i:= 0 TO pMap^.nEntries - 1 DO
    pKey := pMap^.elements[i];
    pTemp := pKey + _strlen(pKey) + 1;
    cmp := LDR_MyStricmp(pTemp, pValue);
    IF cmp = 0 THEN
      // Eintrag wurde gefunden
      IF pIndex <> NIL THEN
        pIndex^ := i$DINT;
      END_IF;
      RETURN;
    END_IF;
  END_FOR;

  pKey := NIL;
  
END_FUNCTION

(* Sucht einen Eintrag in einer EL_TAB_MAP anhand eines Indexes.
   Zurückgegeben wird der Wert an der Stelle des Index
   (oder NIL  wenn der Index ausserhalb der Grenzen liegt)
 *)
FUNCTION Get_ELEMENTS_TAB
VAR_INPUT
  index : DINT;
  pMap : ^EL_TAB_MAP;
	ppKey : ^pChar;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR
VAR
  pTemp : pChar;
END_VAR

  pValue := NIL;
  IF ppKey <> NIL THEN
    ppKey^ := NIL;
  END_IF;

  IF index >= pMap^.nEntries$DINT THEN
    RETURN;
  END_IF;
  
  pTemp := pMap^.elements[index];
  IF ppKey <> NIL THEN
    ppKey^ := pTemp;
  END_IF;
  pValue := pTemp + _strlen(pTemp) + 1;
  
END_FUNCTION

(* Ersetzt einen Eintrag in der ioElSvrMap *)
FUNCTION GLOBAL LDR_ReplaceIoElSvr
VAR_INPUT
	pKey : pChar;
	pValue : pChar;
  index : DINT;
END_VAR

  Replace_ELEMENTS_TAB(pKey, pValue, ioElSvrMap, index);

END_FUNCTION

(* Ersetzt einen Eintrag in der ioElCltMap *)
FUNCTION GLOBAL LDR_ReplaceIoElClt
VAR_INPUT
	pKey : pChar;
	pValue : pChar;
  index : DINT;
END_VAR

  Replace_ELEMENTS_TAB(pKey, pValue, ioElCltMap, index);

END_FUNCTION

(* Fügt einen Eintrag in der ioElCltMap hinzu *)
FUNCTION GLOBAL LDR_AddIoElClt
VAR_INPUT
	pKey : pChar;
	pValue : pChar;
END_VAR

  Add_ELEMENTS_TAB(pKey, pValue, #ioElCltMap);

END_FUNCTION

(* Fügt einen Eintrag in der ioElSvrMap hinzu *)
FUNCTION GLOBAL LDR_AddIoElSvr
VAR_INPUT
	pKey : pChar;
	pValue : pChar;
END_VAR

  Add_ELEMENTS_TAB(pKey, pValue, #ioElSvrMap);

END_FUNCTION

(*
  Sucht nach einem IO-Element das mit einem Server verknüpft ist.
  Zurückgegeben wird der Name des Servers oder NIL, wenn kein IO-Element-Server 
  mit dem angegebenen Namen vorhanden ist.
  
  !!!   WICHTIG   !!!
  Funktion nicht ändern, da von den HW-Klassen über den Linker die 
  Adresse der Funktion ermittelt wird und falls vorhanden über diesen 
  Funktionszeiger aufgerufen wird.
  !!!   WICHTIG   !!!
      
*)
FUNCTION GLOBAL LDR_SearchIoElSvr
VAR_INPUT
	pKey : pChar;
  pIndex : ^DINT;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR

  pValue := Search_ELEMENTS_TAB(pKey, ioElSvrMap, pIndex);
  
END_FUNCTION

(*
  Sucht nach einem IO-Element das mit einem Client verknüpft ist.
  Zurückgegeben wird der Name des Clients oder NIL, wenn kein IO-Element-Client 
  mit dem angegebenen Namen vorhanden ist.
*)
FUNCTION GLOBAL LDR_SearchIoElClt
VAR_INPUT
	pKey : pChar;
  pIndex : ^DINT;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR

  pValue := Search_ELEMENTS_TAB(pKey, ioElCltMap, pIndex);
  
END_FUNCTION

(*
  Sucht nach einem Advanced IO-Element.
  Zurückgegeben wird der Name des Objekts(Clients) das mit dem Advanced IO-Element 
  verknüpft ist oder NIL, wenn kein IO-Element-Advanced mit dem angegebenen Namen 
  vorhanden ist.
*)
FUNCTION GLOBAL LDR_SearchIoElAdv
VAR_INPUT
	pKey : pChar;
  pIndex : ^DINT;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR

  pValue := Search_ELEMENTS_TAB(pKey, ioElAdvMap, pIndex);
  
END_FUNCTION


(*
  Mit dieser Funktion wird dem Loader mitgeteilt, dass das angegebene 
  Objekt ein Advanced IO Element ist, bei dem man auch mit einem Newinst 
  Befehl den Zustand der Verbindung hinter dem Advanced IO Element abfragen 
  kann.
  
  \param uiCmdHwConn Das Newinst Kommando das an ein Advanced IO Element
      geschickt wird um eine Hardwareverbindung abzufragen
  \param uiCmdSimConn Das Newinst Kommando das an ein Advanced IO Element
      geschickt wird um eine Simulationsverbindung abzufragen
*)
FUNCTION GLOBAL LDR_RegisterAdvIoElWithNewinst
VAR_INPUT
  pObj : ^VirtualBase;
  uiCmdHwConn : UINT; // Kommando zur Prüfung mit LDR_IsHWConnected
  uiCmdSimConn : UINT; // Kommando zur Prüfung mit LDR_IsSimConnected
END_VAR
VAR
  index : DINT;
  mitte : DINT;
  links : DINT;
  rechts : DINT;
  pTemp : ^ADV_IO_EL_INFO;
END_VAR

  IF AdvIoElWithNewinstListSize = 0 THEN
    index := 0;
  ELSE
    // binäre Suche nach dem ersten IO-Element Namen, der größer oder gleich ist
    links := 0;
    rechts := AdvIoElWithNewinstListSize$DINT - 1;
   
    WHILE links <= rechts DO 
      mitte := links + ((rechts - links) / 2);
   
      pTemp := AdvIoElWithNewinstList + mitte * sizeof(ADV_IO_EL_INFO);
      IF pTemp^.pObj = pObj THEN
        // Eintrag wurde gefunden, Werte aktualisieren
        pTemp^.uiCmdHwConn := uiCmdHwConn;
        pTemp^.uiCmdSimConn := uiCmdSimConn;
        RETURN;
      ELSE
        IF pTemp^.pObj > pObj THEN
          // im linken Teil weitersuchen
          rechts := mitte - 1;
          index := mitte;
        ELSE
          // im rechten Teil weitersuchen
          links := mitte + 1;
          index := mitte + 1;
        END_IF;
      END_IF;
    END_WHILE;
  END_IF;

  IF AdvIoElWithNewinstListAlloced <= AdvIoElWithNewinstListSize THEN
    AdvIoElWithNewinstListAlloced += 64;
    AdvIoElWithNewinstList := _realloc_LDR(AdvIoElWithNewinstList, AdvIoElWithNewinstListAlloced * sizeof(ADV_IO_EL_INFO))$^ADV_IO_EL_INFO;
    IF AdvIoElWithNewinstList = NIL THEN
      TRACE_ERR("out of memory for AdvIoElWithNewinstList");
      LSLCLI_LoaderTracePrint("out of memory for AdvIoElWithNewinstList", 12);
    END_IF;
  END_IF;
  
	_memmove(AdvIoElWithNewinstList + (index + 1) * sizeof(ADV_IO_EL_INFO), 
           AdvIoElWithNewinstList +  index      * sizeof(ADV_IO_EL_INFO), 
           sizeof(ADV_IO_EL_INFO) * (AdvIoElWithNewinstListSize - index$UDINT));
  AdvIoElWithNewinstListSize += 1;

  pTemp := AdvIoElWithNewinstList + index * sizeof(ADV_IO_EL_INFO);
  pTemp^.pObj := pObj;
  pTemp^.uiCmdHwConn := uiCmdHwConn;
  pTemp^.uiCmdSimConn := uiCmdSimConn;

END_FUNCTION


(*
  Prüft, ob das angegebene Objekt ein Objekt ist, welches mit 
  LDR_RegisterAdvIoElWithNewinst registriert wurde.
*)
FUNCTION IsAdvIoElWithNewinst
VAR_INPUT
  pObj : ^VirtualBase;
  uiCmdHwConn : ^UINT; // Kommando zur Prüfung mit LDR_IsHWConnected
  uiCmdSimConn : ^UINT; // Kommando zur Prüfung mit LDR_IsSimConnected
END_VAR
VAR_OUTPUT
	found : BOOL;
END_VAR
VAR
  mitte : DINT;
  links : DINT;
  rechts : DINT;
  pTemp : ^ADV_IO_EL_INFO;
END_VAR

  found := FALSE;

  IF AdvIoElWithNewinstListSize = 0 THEN
    RETURN;
  END_IF;
  
  // binäre Suche nach dem IO-Element Namen
  links := 0;
  rechts := AdvIoElWithNewinstListSize$DINT - 1;
 
  WHILE links <= rechts DO 
    mitte := links + ((rechts - links) / 2);
 
    pTemp := AdvIoElWithNewinstList + mitte * sizeof(ADV_IO_EL_INFO);
    IF pTemp^.pObj = pObj THEN
      // Eintrag wurde gefunden
      uiCmdHwConn^ := pTemp^.uiCmdHwConn;
      uiCmdSimConn^ := pTemp^.uiCmdSimConn;
      found := TRUE;
      RETURN;
    ELSE
      IF pTemp^.pObj > pObj THEN
        // im linken Teil weitersuchen
        rechts := mitte - 1;
      ELSE
        // im rechten Teil weitersuchen
        links := mitte + 1;
      END_IF;
    END_IF;
  END_WHILE;

END_FUNCTION

FUNCTION IsHwOrSimConnected 
VAR_INPUT
	input : ^void;	// pointer to the channels data element
  hw : UDINT;
END_VAR
VAR_OUTPUT
	connected : DINT;		// 0 .. not connected, > 0 .. connected
END_VAR
VAR
  pObj : ^VirtualBase;
  uiCmdHwConn : UINT; // Kommando zur Prüfung mit LDR_IsHWConnected
  uiCmdSimConn : UINT; // Kommando zur Prüfung mit LDR_IsSimConnected
  cmd : CmdStruct;
  resu : results;
END_VAR

  IF !!IsClientConnected(input) THEN
    // nicht verbunden
    connected := 0;
    RETURN;
  ELSE
    connected := 1;
  END_IF;
  
  input$udint := (input+CltCh.pCmd - CltCh.dData)$udint;
  input$udint := input$^udint^;
  pObj := input$^VirtualBase;
  
  IF IsAdvIoElWithNewinst(pObj, #uiCmdHwConn, #uiCmdSimConn) THEN
    IF hw THEN
      cmd.uiCmd := uiCmdHwConn;
    ELSE
      cmd.uiCmd := uiCmdSimConn;
    END_IF;

    cmd.aPara[0] := 0; // eigene Versionsnummer
    IF pObj^.NewInst(#cmd, #resu) = READY THEN
      IF resu.uiLng >= 2 + sizeof(connected) THEN
        // mit einem Adv.IO-El verbunden und Newinst ok -> Result der Newinst zurückgeben
        connected := resu.aData[0]$DINT;
        RETURN;
      END_IF;
    END_IF;
  END_IF;
  
  IF !!hw THEN
    // Man ist zwar verbunden, aber es wird eine Simulationsverbindung abgefragt. 
    // Diese ist 0, wenn man mit keinem Adv.IO-El verbunden ist.
    connected := 0;
  END_IF;

END_FUNCTION


(*
  Prüft, ob ein Client mit einem Hardware-Server verbunden ist. 
  
  \return
    nicht verbunden: 0
    mit einem Advanced IO Element verbunden: Ergebnis vom Newinst Aufruf
    mit einem anderen Server verbunden: 1
*)
FUNCTION GLOBAL LDR_IsHWConnected 
VAR_INPUT
	input : ^void;	// pointer to the channels data element
END_VAR
VAR_OUTPUT
	connected : DINT;
END_VAR

  connected := IsHwOrSimConnected(input, 1);

END_FUNCTION


(*
  Prüft, ob ein Client mit einem Simulations-Server verbunden ist. 
  
  \return
    nicht verbunden: 0
    mit einem Advanced IO Element verbunden: Ergebnis vom Newinst Aufruf
    mit einem anderen Server verbunden: 0
*)
FUNCTION GLOBAL LDR_IsSimConnected 
VAR_INPUT
	input : ^void;	// pointer to the channels data element
END_VAR
VAR_OUTPUT
	connected : DINT;		// 0 .. not connected, > 0 .. connected
END_VAR

  connected := IsHwOrSimConnected(input, 0);

END_FUNCTION


(*
  Liefert ein IO-Element-Client mit einem bestimmten Index.
  Zurückgegeben wird der Name der Clients oder NIL, wenn kein IO-Element-Client 
  mit dem angegebenen Index vorhanden ist.
*)
FUNCTION GLOBAL LDR_GetIoElClt
VAR_INPUT
  index : DINT;
	ppKey : ^pChar;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR

  pValue := Get_ELEMENTS_TAB(index, ioElCltMap, ppKey);
  
END_FUNCTION

(*
  Sucht bei den IO-Element/Clients nach einem Client.
  Zurückgegeben wird der Name der IO-Elements oder NIL, wenn kein IO-Element-Client 
  mit dem angegebenen Namen vorhanden ist.
*)
FUNCTION GLOBAL LDR_SearchCltIoEl
VAR_INPUT
	pValue : pChar;
  pIndex : ^DINT;
END_VAR
VAR_OUTPUT
	pKey : pChar;
END_VAR

  pKey := Search_ELEMENTS_TAB_value(pValue, ioElCltMap, pIndex);
  
END_FUNCTION

FUNCTION Search_MM_ELEMENTS_TAB
VAR_INPUT
	pKey : pChar;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR

  pValue := Search_ELEMENTS_TAB(pKey, mmElMap, NIL);
  
END_FUNCTION


(* 
  Search Multimaster Server Object
*)
#pragma warning(disable:74); // wegen pObj
FUNCTION Search_MM_ServerObj
VAR_INPUT
	pObj            : ^VirtualBase;
	pScan           : pChar;
	pObj_ch         : ^pVoid;
	ppObj           : ^pVirtualBase;
END_VAR
VAR_OUTPUT
	chMode          : CHMODE;
END_VAR
VAR
	pCls            : ^CLSHDR;
	pObjDsc         : ^OBJ;
  szTxt			      : ARRAY [0..NLNG+1] OF CHAR;
 	Lexem			      : ARRAY [0..NLNG+1] OF CHAR;
  uiStationID     : UDINT;
  pTxt            : pChar;
END_VAR

  _strcpy(#szTxt[0], pScan);
  
  pTxt := #szTxt[0];

  chMode := _CH_NONE;      

	ScanLexem(#pTxt, #Lexem[0]);

  uiStationID := LDR_MM_StatNameGetNr(#Lexem[0]);
  
  IF (uiStationID <> (-1$UDINT)) THEN
  
    pObj$^SvrCh := _LookUpObj(pScan);
    IF pObj = NIL THEN
      RETURN;
    END_IF;
  
    pObjDsc    := LSL_GetHdr(pObj);
    pCls       := pObjDsc^.pClsHdr;
     
    IF pObj_CH <> NIL THEN
      pObj_CH^ := pObj;
    END_IF;

    IF ppObj <> NIL THEN
      ppObj^ := pObj;
    END_IF;

    chMode := _CH_OBJ;      

  END_IF;

END_FUNCTION
#pragma warning(default:74);

(*
  Sucht nach einem Channel, bei dem eine Lookup-Abfrage mit einem anderen 
  Namen durchgeführt werden soll.
  
  Wird für embedded Strings verwendet, damit man über den äusseren Server die 
  Methoden (zb. NEWINST) des embedded String Objekts aufrufen kann.
*)
FUNCTION Search_CH_REDIRECT_EMB_TAB
VAR_INPUT
	pKey : pChar;
END_VAR
VAR_OUTPUT
	pValue : pChar;
END_VAR

  pValue := Search_ELEMENTS_TAB(pKey, chRedirectEmbMap, NIL);
  
END_FUNCTION



FUNCTION GLOBAL __CDECL _LookUpMMServer
  VAR_INPUT
    pObj            : ^VirtualBase;
    pText           : pChar;
    pObj_ch         : ^pVoid;
    ppObj           : ^pVirtualBase;
  END_VAR
  VAR_OUTPUT
    chMode    : CHMODE;
  END_VAR

  // vielleich ein MultimasterServer?
  // zuerst prüfen, da ppScan in LDR_LookupEmbedded3 verändert wird und
  // zusätzlich eine Fehlermeldung ausgegeben würde
 chMode := Search_MM_ServerObj(pObj, pText, pObj_ch, ppObj);
 IF (chMode = _CH_NONE) THEN
    pText := Search_MM_ELEMENTS_TAB(pText);
    IF pText <> NIL THEN
      chMode := Search_MM_ServerObj(pObj, pText, pObj_ch, ppObj);
    END_IF;
 END_IF;

END_FUNCTION
//    
//    Bei dieser Funktion wird im Unterschied zu LDR_LookupEmbedded3 auch noch 
//    bei den IO-Elementen nachgeschaut, ob es ein Objekt mit diesem Namen gibt (SA22084).
//    
//       (pCh, eChMode) := _LookupExpr(pExpr);
//    
//    This function parses an ASCII - string and returns the
//    address of the object or channel it describes. The result
//    has to be casted to the appropriate data type, which is
//    given by the channel mode.
//    
//    -> pObj    : Pointer to the object described by the given name.
//                 Can be NIL when the object is not known, in this case the object 
//                 is searched with _LookUpObj.
//                 When the object is specified, the expensive call to _LookUpObj 
//                 can be skipped. The caller must ensure that the pointer corresponds 
//                 to the name. 
//    -> pScan   : pointer to the text buffer
//    <- pObj_Ch : pointer to the object or channel described
//                 by the given name
//    <- ppObj   : if the text describes a channel and pObj is
//                 <> NIL, then it contains the pointer to the
//                 command channel of the object
//    <- chMode  : returns the type of pObj_Ch
//    
//    Examples: "Control"             // object
//              "Control.SetValue"    // data
//              "Control.Cmd"         // command
//              "HeatingZone\DH"      // object channel

FUNCTION GLOBAL LDR_LookupEmbedded2
VAR_INPUT
	pObj            : ^VirtualBase;
	ppScan          : ^pChar;
	pObj_ch         : ^pVoid;
	ppObj           : ^pVirtualBase;
END_VAR
VAR_OUTPUT
	chMode          : CHMODE;
END_VAR
VAR
	pScan     : pChar;
	pIoEl     : pChar;
	chErrInfo : Char;
END_VAR
  
  pScan := ppScan^;
  //[8D-9332] Trnrob/Praerw, 01.08.2024: Suppress trace messages, 
  // because there is a second way to find channel in IO-Element table
  chErrInfo := 0;
  chMode := LDR_LookupEmbedded3(pObj, ppScan, pObj_ch, ppObj, #chErrInfo);
  IF chMode = _CH_NONE THEN
    (*SA22084: Wenn der Name nicht gefunden wurde, wird bei den IO Elementen (Server, Client) weitergesucht. *)
    pIoEl := LDR_SearchIoElSvr(pScan, NIL);
    IF pIoEl <> NIL THEN
      chMode := LDR_LookupEmbedded3(NIL, #pIoEl, pObj_ch, ppObj);
    ELSE 
      //[8D-9332] Trnrob/Praerw, 01.08.2024: run search a second time to write trace message
      IF chErrInfo > 0 THEN
        chMode := LDR_LookupEmbedded3(pObj, ppScan, pObj_ch, ppObj);
      END_IF;
    END_IF;
  END_IF;

END_FUNCTION


//    
//       (pCh, eChMode) := _LookupExpr(pExpr);
//    
//    This function parses an ASCII - string and returns the
//    address of the object or channel it describes. The result
//    has to be casted to the appropriate data type, which is
//    given by the channel mode.
//    
//    
//    -> pScan   : pointer to the text buffer
//    <- pObj_Ch : pointer to the object or channel described
//                 by the given name
//    <- ppObj   : if the text describes a channel and pObj is
//                 <> NIL, then it contains the pointer to the
//                 command channel of the object
//    <- chMode  : returns the type of pObj_Ch
//    
//    Examples: "Control"             // object
//              "Control.SetValue"    // data
//              "Control.Cmd"         // command
//              "HeatingZone\DH"      // object channel

FUNCTION GLOBAL _LookupEmbedded
VAR_INPUT
	ppScan          : ^pChar;
	pObj_ch         : ^pVoid;
	ppObj           : ^pVirtualBase;
END_VAR
VAR_OUTPUT
	chMode          : CHMODE;
END_VAR

  chMode := LDR_LookupEmbedded2(NIL, ppScan, pObj_ch, ppObj);

END_FUNCTION


//    
//      uiOff := _LookUpChannel(pName, udCRC, ppCls);
//    
//    Searches for a name within the channel descriptor table of
//    the given class. The name must be upper case. A channel
//    name must be unique throughout one class.
//    
//    -> pName:	pointer to the name to search for
//    -> udCRC: checksum of the pName-string
//    <> ppCls:	pointer to the derived class where the 'pName'-channel
//             	should belong to, 
//				the pointer to the class where it really belongs to, is returned
//    <- uiChNo: =  16#FFFF - name not found
//               <> 16#FFFF - number of the channel with the
//                            appropriate name, counted within one single class
//
FUNCTION GLOBAL _LookUpChannel
VAR_INPUT
	pName			: ^CLexem;
	ppCls           : ^pClsHdr;
END_VAR
VAR_OUTPUT
	uiChNo			: UINT;
END_VAR
VAR
	uiAnzChs		: UINT;
	pCls			: ^ClsHdr;
	pDsc			: ^ClsHdrConst;
	ppCD			: ^ChDsc;
END_VAR

	pCls		:=  ppCls^;
	pDsc		:=	pCls^.pDsc;
	uiAnzChs	:=  pDsc^.nSvrChs+
					pDsc^.nCltChs;
	ppCD		:=  #pCls^.aDscChs[0];
	uiChNo		:=  0;

	WHILE uiChNo < uiAnzChs DO

#ifndef DUMMY_LOADER
		IF pName^.iCompare(#ppCD^.pDsc^.Name) THEN
			RETURN;
		END_IF;
#endif    

		uiChNo	+= 1;
		ppCD	+= sizeof(ChDsc);

	END_WHILE;

	pCls := pCls^.pBaseCls;
	IF pCls <> NIL THEN
		ppCls^ := pCls;
		uiChNo := _LookUpChannel(pName, ppCls);
	ELSE
		uiChNo := -1$UINT;
	END_IF;

END_FUNCTION
//    
//      pCls := _LookUpClsIDExact(pName, udRev, nExact);	
//    
//    This function searches for the given class ID throughout
//    the class list. It returns a NIL pointer if the class
//    ID is not defined in the existing class list. Otherwise
//    a pointer to the corresponding class descriptor is
//    returned.
//    
//	  ->pStart	: start search at this entry of the class table
//    ->pName	: search for this class name in the class table
//	  ->nExact	: which part of the revision should be checked ?
//						0..none, 1..high, 2..exact
//    <-pCls  	: = NIL if the class was not found
//              	<>NIL class exists

FUNCTION GLOBAL _LookUpClsIDExact
VAR_INPUT
	pStart					: ^ClsHdr;
	pName					: ^CLexem;
	udRev					: Revision;
	nExact					: UDINT;
END_VAR
VAR_OUTPUT
	pCls             		: ^CLSHDR;
END_VAR
VAR
	pDsc					: ^ClsHdrConst;
  searchRes : pVoid;
  pDscKey : ^ClsHdrConst;
  placeholder : ARRAY[0..SIZEOF(ClsHdrConst)+SIZEOF(CLocLex)] OF CHAR;
END_VAR

	IF pStart = NIL THEN
		pCls := _GetClsList();
	ELSE
		pCls := pStart;
	END_IF;

  (* Wenn vom Start weg gesucht werden soll und die Hashtab vorhanden ist, dann in der 
     Hashtab suchen, ansonsten die lineare Liste durchwurschteln *)
  IF pStart = NIL & clsHashTab THEN
    pDscKey := #placeholder$ClsHdrConst;
    _memcpy(#pDscKey^.Name, pName, SIZEOF(CLocLex));
    pDscKey^.udRev := udRev;
    searchRes := HashTable_Find(pDscKey, clsHashTab, #nExact);
    IF searchRes THEN
      pCls := HashTable_RetrieveVal(searchRes)$^ClsHdr;
    ELSE
      pCls := NIL;
    END_IF;
    RETURN;
  ELSE
    WHILE pCls <> NIL DO

      pDsc	:= pCls^.pDsc;
  #ifndef DUMMY_LOADER
      IF pDsc^.Name.iCompare(pName) THEN
        IF 	nExact >= 2 & udRev    = pDsc^.udRev |		// exact revision checking
          nExact =  1 & udRev.Hi = pDsc^.udRev.Hi |	// higher revision checked
          nExact =  0 THEN							// no revision checking
          RETURN;
        END_IF;
      END_IF;
  #endif    

      pCls := pCls^.pNxtCls;

    END_WHILE;
  END_IF;

END_FUNCTION
//    
//      pCls := _LookupClsIdNewest(pClsID);
//    
//		This function searches for the latest version of the class with the
//		given ID. Only the Guid and the high revision are compared.
//    
//    ->pClsId: search for this class guid and RevHi in the class table
//    <-pCls  : = NIL if the class index was not found
//              <>NIL class index exists

FUNCTION GLOBAL _LookUpClsIDnewest
VAR_INPUT
	pDsc				: ^ClsHdrConst;
END_VAR
VAR_OUTPUT
	pCls				: ^ClsHdr;
END_VAR

	pCls  := NIL;			// search for a class descriptor with the same high revision
	REPEAT
		pCls  := _LookUpClsIDExact(pCls, #pDsc^.Name, pDsc^.udRev, 1);
		IF pCls = NIL | 
		   pCls^.ClsState AND CTY_NEWEST_CLS THEN
			RETURN;		
		END_IF;
		pCls	:= pCls^.pNxtCls;	// start with next class
	UNTIL FALSE 
	END_REPEAT;

END_FUNCTION
//    
//      pObj := _LookUpObj(pName);
//    
//    This function looks for an object by comparing the CRC-
//    checksums of the names. It will either return a NIL-pointer
//    if the object was not found, or the pointer to the channels
//    server command channel.
//    
//    -> pName : object name
//    <- EAX   : =  NIL object doesn't exist
//               <> NIL object found
FUNCTION __cdecl GLOBAL _LookUpObj
VAR_INPUT
	pName           : ^char;
END_VAR
VAR_OUTPUT
	ret				: ^SvrCh;
END_VAR
VAR
	lexem			: CLocLex;
END_VAR

#ifndef DUMMY_LOADER
	lexem.InitText(pName);
#endif  

	ret	:= _LookupObjLex((#lexem) $ ^CLexem);

END_FUNCTION

#ifdef _LSL_TARGETARCH_ARM

FUNCTION GLOBAL _LookUpObjLex
VAR_INPUT
	pLexem          : ^CLexem;
END_VAR
VAR_OUTPUT
	ret				: ^SvrCh;
END_VAR
VAR
  pCls  : ^ClsHdr;
  pObj  : ^Obj;
  udCrc : UDINT;
  pName : ^char;
END_VAR

  ret := NIL;
  if(pMapper <> NIL) then
    pObj := FindCRCinMap(pLexem^.udCRC$dint);
    if(pObj <> NIL) then
      ret := (pObj + sizeof(Obj))$^SvrCh;
    end_if;
  else
  
    udCRC := pLexem^.udCRC;
    pName := #pLexem^.Text[0];
    pCls  := _GetClsList();
    
    while(pCls <> NIL) do
      if((pCls^.pDsc^.ClsType and CTY_OSI_CLASS) = 0) then
      
        pObj := pCls^.pFirstObj;
        while(pObj <> NIL) do
        
          if(pObj^.pObjDsc^.SymName.udCRC = udCRC) then
            if(_stricmp(pname, #pObj^.pObjDsc^.SymName.Text[0]) = 0) then
              ret := pObj + sizeof(Obj);
              return;
            end_if;
          end_if;
          pObj := pObj^.pNxtObj;
          
        end_while;
        
      end_if;
      pCls := pCls^.pNxtCls;
    end_while;
    
  end_if;

END_FUNCTION 
#else
FUNCTION AWL GLOBAL _LookUpObjLex
VAR_INPUT
	pLexem          : ^CLexem;
END_VAR
VAR_OUTPUT
	ret				: ^SvrCh;
END_VAR
VAR
	pCls        	: ^ClsHdr;
	pObj			: ^Obj;
	udCrc           : UDINT;
	pName			: ^char;
END_VAR

		L.EDI		pLexem					// CRC in lokale Variable speichern
		L.EAX		(EDI + CLexem.udCRC)
		S.EAX		udCRC

		ADD.EDI		CLexem.Text				// pName in lokale Variable speichern
		S.EDI		pName

		CMP.D		pMapper, 0
		JZ			LookUpCRC_OLD
				
		sub.esp		4
		push		udCRC
		call		FindCRCinMap
		add.esp		4
		pop			edi
		o.edi		edi
		jz			ende

		ADD.EDI		sizeof(Obj)	//12
		JMP			ende	



LookUpCRC_OLD
    SUB.ESP 4
		CALL		_GetClsList
    POP     EAX

CLS_LOOP
		O.EAX		EAX			// es gibt keine (weiteren) Klassen mehr
		JZ			ENDE

    // Eine OSI Klasse hat keine echten Objekte sondern 
    // nur Dummy Objekte, die nur aus einem Server bestehen.
    // Der Obj-Descriptor fehlt ebenfalls.
    L.ESI   (EAX + ClsHdr.pDsc) 
    L.BX    (ESI + ClsHdrConst.ClsType) 
    A.BX    CTY_OSI_CLASS
    JNZ     NEXT_CLS

		L.EDI		(EAX + ClsHdr.pFirstObj)	// 1. Objekt der Klasse


OBJ_LOOP
		O.EDI		EDI
		JZ			NEXT_CLS

		L.EBX		udCRC

		L.ESI		(EDI+Obj.pObjDsc)	// es werden Objekt-Köpfe gespeichert
		CMP.EBX		(ESI+ObjDsc.SymName.udCRC)
		JNE			NEXT_OBJ

//			_stricmp(pName, #pObjList^.SymName[0]) = 0 THEN
		S.EDI		pObj			// Zeiger auf Objekt-Kopf sichern
		S.EAX		pCls			// Zeiger auf die Klasse sichern

		SUB.ESP		4
		PUSH		pName
		ADD.ESI		ObjDsc.SymName.Text
		PUSH		ESI

		CALL		_stricmp
		ADD.ESP		8

		L.EDI		pObj			// restore registers
		L.EBX		udCRC
		L.EAX		pCls

		POP			ECX
		O.ECX		ECX				// _stricmp liefert <> 0 bei Ungleichheit
		JNZ			NEXT_OBJ

		ADD.EDI		sizeof(Obj)		 	// Objekt gefunden
		JMP			ENDE

NEXT_OBJ
		L.EDI		(EDI+Obj.pNxtObj)
		JMP			OBJ_LOOP

NEXT_CLS
		L.EAX		(EAX + ClsHdr.pNxtCls)
		JMP			CLS_LOOP

ENDE
		S.EDI		ret
END_FUNCTION 
#endif