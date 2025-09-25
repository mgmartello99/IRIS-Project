#ifdef LASAL_VERSION
#if LASAL_VERSION > 57
//LASAL_INCLUDES
 
#endif//LASALVERSION > 57
#endif//LASAL_VERSION
#include "RTOS_PrivHeader.h"
#include <RTOS_Header.h>
#include <RTOS_Interfaces.h>
#include <RTOS_Variables.h>
#include <LSL_string.h>
#include <LSL_ST_LSLFile.h> // RTKernel V3.18
#include <LSL_St_IfLinker.h>
#include <RTOS_Private.h>

#include "RTOS_MemIntern.h"
#include <Lsl_st_IfLinker.h>
#include <LSL_ST_MT.H>

//#include ".\OS Interface\Header Files\_TaskObjectControl.h"
#ifndef DUMMY_LOADER
#pragma using _TaskObjectControl
#endif
#include ".\OS Interface\SystemTypes.h"
#include ".\include\HashTab.h"

#ifndef DUMMY_LOADER
#pragma using PtrChk
#pragma using ObjectReader
#endif

// server attributes:
#define ATTR_RETENTIVE		1	// object is stored to active.dat on power down
								// and reloaded on power up
#define ATTR_DATA_BUFFER	2	// more than the 4 bytes of dData are transmitted
								// when this channel is added to the refresh-list
								// The NewInst-method is called with command 0
								// (i.e. CMD_GET_DATA) which should set the result 
								// buffer.
// attribute access macros:
#define SetFlag(s, flag)		s.pSvrDsc$UDINT := s.pSvrDsc$UDINT OR flag;
#define GetFlagC(c, flag)	  c.pSvrDsc$UDINT AND flag
#define GetFlag(flag)		    GetFlagC(pObj^.pSvrChCmd, flag)


TYPE
#pragma pack (push, 1)

ObjEntry                        : STRUCT
	udAddr						: PVOID;
	szName						: ARRAY [0..NLNG] OF CHAR;
	END_STRUCT;

ClsEntry                        : STRUCT
	udAddr						: pClsHdr;  // address of class descriptor
	udBase						: pClsHdr;	// address of base class descriptor
	guid						  : GUID;			// class GUID ... //ws002: Compatible with Lasal class
	uiType						: UINT;			// class type

//  uiChLng                       : UINT;  		// total length of channel name
//  uiMethLng                     : UINT;  		// total length of method names


	szName						: ARRAY [0..NLNG] OF CHAR;
 END_STRUCT;

ChEntry			: STRUCT
	udAddr		: ^void;
	uiMode		: CHMODE;
	szName		: ARRAY[0..NLNG] OF CHAR;
 END_STRUCT;

#pragma pack(pop)
END_TYPE

VAR_GLOBAL

	pFirstCls           : ^ClsHdr;		// to find classes
	pLastCls						: ^ClsHdr;		// last class inserted
  clsHashTab          : HashTable;
	pFirstCmd           : ^CMDMETH;
	pMapper							: ^CRCMap;		// pointer onto map to find fast obj
	// pointer to OS-task interface
#ifndef DUMMY_LOADER
  s_pTaskCIL				  : ^_TaskObjectControl::OS_TASK;
#else  
  s_pTaskCIL				  : pVoid;
#endif  
END_VAR



FUNCTION GLOBAL LDR_ClsHashTable_Insert
VAR_INPUT
  clsHashTab : HashTable;
	pCls : ^ClsHdr;
END_VAR;

FUNCTION _InsertObjDsc
VAR_INPUT
 pObjName        : PCHAR;
 pParent         : ^OBJ;
END_VAR
VAR_OUTPUT
 pObj            : ^OBJDSC;
END_VAR;

FUNCTION _InsertObjData
VAR_INPUT
 pCls            : ^ClsHdr;
 pObj            : ^OBJDSC;
END_VAR
VAR_OUTPUT
 pObjDsc         : ^OBJDSC;
END_VAR;


FUNCTION _GetSymName
VAR_INPUT
 pObjDsc         : ^OBJDSC;
 pName           : PCHAR;
END_VAR;

FUNCTION GLOBAL _LookupCls
VAR_INPUT
	pName		: ^char;
END_VAR
VAR_OUTPUT
	pCls		: ^ClsHdr;
END_VAR;

FUNCTION PrepLnk_IterateVMTs
VAR_INPUT
  pCls      : ^ClsHdr;
  pClassSvr : ^SvrChCmd;
END_VAR;

FUNCTION PrepLnk_UpdateDSCs
VAR_INPUT
  pCls        : ^ClsHdr;
  pNewClsDsc  : ^ClsHdrConst;
END_VAR;

FUNCTION GLOBAL retentiveWriteSplitter
	VAR_INPUT
		input (EAX) 	: DINT;
	END_VAR
	VAR_OUTPUT
		result (EAX) 	: DINT;
	END_VAR;


FUNCTION IsBetween
VAR_INPUT
  val : pVoid;
  min_ : pVoid;
  size : UDINT;
END_VAR
VAR_OUTPUT
  ret : BOOL;
END_VAR

	IF val < min_ THEN
		ret	:= FALSE;
	ELSIF val < min_+size THEN
		ret	:= TRUE;
	ELSE
		ret	:= FALSE;
	END_IF;

END_FUNCTION

// Checks if the given addresse is in the range of minimum and
// maximum user or prog address.
//[>udAddr] address to check
//[<bValid] true if min_user_addr <= addr <= max_user_addr
FUNCTION _IsAddrValid2
VAR_INPUT
	udAddr		: ^void;
END_VAR
VAR_OUTPUT
	bValid		: BOOL;
END_VAR

  IF !!IsBetween(udAddr,_UserDataPointer,_UserDataSize) &
     !!IsBetween(udAddr,_UserProgPointer,_UserProgSize) THEN
		bValid	:= FALSE;
	ELSE
		bValid	:= TRUE;
	END_IF;

END_FUNCTION

// Checks if the given addresse is in the range of minimum and
// maximum user address.
//[>udAddr] address to check
//[<bValid] true if min_user_addr <= addr <= max_user_addr
FUNCTION GLOBAL __CDECL _IsAddrValid
VAR_INPUT
	udAddr		: ^void;
END_VAR
VAR_OUTPUT
	bValid		: BOOL;
END_VAR

	IF udAddr < _UserDataPointer THEN
		bValid	:= FALSE;
	ELSIF udAddr < _UserDataPointer+_UserDataSize THEN
		bValid	:= TRUE;
	ELSE
		bValid	:= FALSE;
	END_IF;

END_FUNCTION
//    
//      pCls := _GetClsList();
//    
//    This function just returns a pointer to the first entry
//    of the class table.
//    
//    <- pCls: pointer to the start of the class list

FUNCTION GLOBAL _GetClsList
VAR_OUTPUT
 pCls            : ^ClsHdr;
END_VAR

	pCls := pFirstCls;

END_FUNCTION
//    
//      pCls := _InsertClass(pNewCls);
//    
//    The function allocates a new class descriptor.
//    If there already exists a class with the given class ID,
//    the function returnes without doing anything.
//    If the class ID is new, then the class dscriptor is
//    allocated and added to the class list. On the far heap
//    the header of a channel descriptor is allocated (number
//     of channels and the class name pointer).
//    
//     <- pCls    : pointer to the new class descriptor

FUNCTION GLOBAL _InsertClass
VAR_INPUT
	pNewCls			: ^ClsHdr;		// new allocated class
END_VAR
VAR_OUTPUT
	pCls			: ^ClsHdr;
END_VAR

	pCls := _LookUpClsIDExact(NIL, #pNewCls^.pDsc^.Name, pNewCls^.pDsc^.udRev, 1);

	IF pCls = NIL THEN  	// i.e. class is loaded once more after startup
												// -> new class index, but same GUID and revision
		pCls	:= pNewCls;

		IF pLastCls <> NIL THEN					// equals nCls > 0
			pLastCls^.pNxtCls := pCls;			// chaining pointers
		ELSE
			pFirstCls := pCls;

		 	IF OS_CILGet("LASALTASK", #s_pTaskCIL$void) <> SYS_ERR_NONE THEN
				s_pTaskCIL := NIL;
			END_IF;
		END_IF;
    LDR_ClsHashTable_Insert(clsHashTab, pCls);  // in die Hashtabelle eintragen

		pLastCls	:= pCls;

	ELSE

		_free(pNewCls);

	END_IF;

END_FUNCTION
// return the name of a channel
//[>pCls]	pointer to class descriptor
//[>uiCh]	number of channel (server 0..n-1, clients n..m-1)
//[<pName]	channel name, NIL in case of any error
FUNCTION GLOBAL _GetChName
VAR_INPUT
	pCls            : ^ClsHdr;
	uiCh            : UINT;
END_VAR
VAR_OUTPUT
	pName           : PCHAR;
END_VAR
VAR
	uiChs           : UINT;
END_VAR

	uiChs := pCls^.pDsc^.nSvrChs+pCls^.pDsc^.nCltChs;
	IF uiCh >= uiChs THEN
		pName := NIL;
	ELSE
		pName := #pCls^.aDscChs[uiCh].pDsc^.Name.Text[0];
	END_IF;

END_FUNCTION
//	Object pObj is checked for a connection to the embedded object
//	pThis.
//
// -> pThis	: object to search for
// -> pObj  : object, for which the connections are checked
// <- ppCls	: pointer to (base-)class or NIL if not found
//
FUNCTION IsConnectedTo
VAR_INPUT
	pThis			: ^VirtualBase;
	pObj			: pSvrCh;
	ppCls			:^pClsHdr;
END_VAR
VAR_OUTPUT
	uiCh			: UINT;
END_VAR
VAR
	uiSvr			: UINT;
	uiClt			: UINT;
	pChDsc			: ^ChDscEntry;
	pCh				: ^CLTCH;
	pCls			: ^ClsHdr;
	pDsc			: ^ClsHdrConst;
	ppCD			: ^ChDsc;
END_VAR

	uiCh	:= 0;
	pCls	:= ppCls^;
	pDsc	:= pCls^.pDsc;
	uiSvr	:= pDsc^.nSvrChs;
	uiClt	:= pDsc^.nCltChs;

	ppCD	:= #pCls^.aDscChs[uiSvr];

	WHILE uiCh < uiClt DO

		pChDsc	:= ppCD^.pDsc;

		// only check embedded channels
		IF pChDsc^.uiMode = _CH_CLT_EMB THEN
			pCh	:= pObj+pChDsc^.uiOff;
			IF pCh^.pCmd = pThis THEN
				uiCh += uiSvr;
				RETURN;
			END_IF;
		END_IF;

		uiCh	+= 1;
		ppCD	+= SIZEOF(ChDsc);

	END_WHILE;

	pCls := pCls^.pBaseCls;
	IF pCls <> NIL THEN
		ppCls^	:= pCls;
		uiCh	:= IsConnectedTo(pThis, pObj, ppCls);
	ELSE
		uiCh	:= -1$UINT;		// not connected
	END_IF;

END_FUNCTION
//    
//      pName := _GetEmbeddedName(this, #pParent);
//    
//    The function returns the name of the object and a pointer
//    to its parent object if it is embedded.
//    
//    -> this      : pointer to the current object
//    <- ppParent  : pointer to the parent object or NIL
//    <- pName     : pointer to the name
//    
//    ATTENTION: The function doesn't work properly, if objects
//    are referenced from different typed client channels.
// ToDo: Nachdem der Name eines Objekts nicht davon abhängt, über welchen Objektkanal
// man es sich ansieht, und der vollständige Name aller Objekte jetzt mit jedem Objekt
// gepeichert wird, sollte diese Funktion eigentlich überflüssig geworden sein.
// Außerdem funktioniert sie sowieso nicht richtig (siehe oben, ATTENTION).
FUNCTION _GetEmbeddedName
VAR_INPUT
	pThis			: ^VirtualBase;
	pParent			: ^pVirtualBase;
END_VAR
VAR_OUTPUT
	pName			: PCHAR;
END_VAR
VAR
	pObjHdr			: ^OBJ;
	pRCls,
	pCls			: pClsHdr;
	pObj			: pVirtualBase;
	uiCh			: UINT;
END_VAR

	pObjHdr := LSL_GetHdr(pThis);
	pName   := #pObjHdr^.pObjDsc^.SymName.Text[0];
(*
	#ifndef pgtest
	IF pName^ <> 0 THEN                                 // ordinary object
		pParent^ := NIL;
		GR: change this later IF names are NOT stored FOR emb. objects
		RETURN;
	END_IF;
	#endif
*)
	IF pObjHdr^.pObjDsc^.Flags.Embedded = FALSE THEN
		pParent^ := NIL;
		RETURN;
	END_IF;

	pCls := _GetClsList();
	WHILE pCls <> NIL DO

		IF pCls^.nObjs THEN
			pObjHdr  := pCls^.pFirstObj;

			WHILE pObjHdr <> NIL DO
				pObj  := LSL_GetPtr(pObjHdr);
				pRCls := pCls;
				uiCh  := IsConnectedTo(pThis, pObj$^SvrCh, #pRCls);
				IF uiCh <> -1$UINT THEN
					pName    := _GetChName(pRCls, uiCh);
					pParent^ := pObj;
					RETURN;
				END_IF;

				pObjHdr  := pObjHdr^.pNxtObj;
			END_WHILE;
		END_IF;

		pCls    := pCls^.pNxtCls;
	END_WHILE;

	pParent^ := NIL;

END_FUNCTION

FUNCTION LSL_GetObjName
VAR_INPUT
  pThis   : ^VirtualBase;
END_VAR
VAR_OUTPUT
  pName   : ^char;
END_VAR
VAR
  pObj    : ^Obj;
END_VAR

  pObj    := LSL_GetHdr(pThis);
  pName   := #pObj^.pObjDsc^.SymName.Text[0];

END_FUNCTION
//    
//      _GetObjName(this, pName);
//    
//    This function copies the concanated parts of an embedded
//    objects name into the 'pName' buffer. The channel names
//    are separated by a '\'. The buffer must be provided with
//    sufficient size (at maximum 255 characters).
// ToDo: Diese Funktion muß sehr viel einfacher werden, siehe Bemerkung ToDo oben.
FUNCTION GLOBAL _GetObjName
VAR_INPUT
	pThis			: ^VirtualBase;
	pName			: ^char;
END_VAR
VAR_OUTPUT
	udLen			: UDINT;
END_VAR
VAR
	pParent			: ^VirtualBase;
	pEmbName		: PCHAR;
	udPart			: UDINT;
END_VAR

	udLen	:= 0;
	pParent := pThis;

	WHILE pParent <> NIL DO

		pEmbName	:= _GetEmbeddedName(pParent, #pParent);
		udPart		:= _strlen(pEmbName)+1;
		IF udPart = 1 | udLen+udPart > 255 THEN
			EXIT;
		END_IF;
		_memmove(pName+udPart, pName,   udLen);
		_memcpy (pName,        pEmbName, udPart);
		IF udLen THEN
			(pName+udPart-1)^ := '\';
		END_IF;
		udLen += udPart;

	END_WHILE;

END_FUNCTION
(* LSL2
// Returns the pointer to a method name
//[>pCls]	pointer to the class descriptor
//[>uiMeth]	number of the method
FUNCTION GLOBAL _GetMethName
VAR_INPUT
	pCls            : ^ClsHdr;
	uiMeth          : UINT;
END_VAR
VAR_OUTPUT
	pName           : pChar;
END_VAR
VAR
	pMethDsc		: ^MethDscEntry;
END_VAR

	pMethDsc	:= _GetMethDsc(pCls, uiMeth);
	IF pMethDsc = NIL THEN
		pName	:= NIL;
		RETURN;
	END_IF;

	pName		:= pMethDsc^.pName;

END_FUNCTION
*)
//    
//      ret_code := _GetClsNameN(pCls, pClsEntry);
//    
//    This function copies the address of the class header, the
//    length of the channel and method names and the name
//    of the class into the buffer specified by the 'pClsEntry'
//    parameter.
//    
//    -> pCls   	: pointer to the previous class descriptor
//										!!! NIL returns the first class !!!
//    <- pClsEntry: pointer to the result buffer
//    <- ret_code : C_OK : class header found
//                  C_SYNTAX_ERROR : else
//    <- EAX      : total length of the result

FUNCTION GLOBAL _GetClsNameN
VAR_INPUT
#ifdef _LSL_TARGETARCH_ARM
 pLng        : ^UINT;
#endif 
 pCls		     : ^ClsHdr;
 pClsEntry_      : PVOID;
END_VAR
VAR_OUTPUT
 ret_code        : CONFSTATES;
END_VAR

	#define pClsEntry pClsEntry_ $ ^ClsEntry

	if( pCls = NIL ) THEN 
		pCls     := _GetClsList();
	ELSE
		pCls     := pCls^.pNxtCls;
	END_IF;

	IF pCls <> NIL THEN
		pClsEntry^.udAddr	:= pCls;
		pClsEntry^.udBase	:= pCls^.pBaseCls;

    pClsEntry^.guid.dw0	:= NIL; //ws002: Compatible with Lasal class
    pClsEntry^.guid.dw1	:= NIL; //ws002: Compatible with Lasal class
    pClsEntry^.guid.dw2	:= NIL; //ws002: Compatible with Lasal class
    pClsEntry^.guid.dw3	:= NIL; //ws002: Compatible with Lasal class

		pClsEntry^.uiType	:= pCls^.pDsc^.ClsType;

    _strcpy(#pClsEntry^.szName[0], _GetClsName(pCls));
      
   #ifdef _LSL_TARGETARCH_ARM
    pLng^ := to_uint(_strlen(#pClsEntry^.szName[0])+(to_uint(ClsEntry.szName)+sizeof(UINT)));
   #else
    EAX   := _strlen(#pClsEntry^.szName[0])+(to_uint(ClsEntry.szName)+sizeof(UINT));
   #endif 
      
    ret_code := C_OK;

    RETURN;
  END_IF;

	pClsEntry^.udAddr := NIL;
 #ifdef _LSL_TARGETARCH_ARM
	pLng^ := SIZEOF(UDINT)+4*SIZEOF(UINT);
 #else
	EAX   := SIZEOF(UDINT)+4*SIZEOF(UINT);
 #endif
	ret_code := C_OK;

	#undef pClsEntry

END_FUNCTION
//    
//      ret_code := _GetObjNameN(pCls, uiObj, pObjDsc);
//    
//    This function copies the address and the full path name
//    of an object into the buffer specified by the 'pObjDsc'
//    parameter.
//    
//    -> pCls    : pointer to the class descriptor
//    -> pObj		 : pointer to the previous object descriptor
//									!!! NIL returns the first object !!!
//    <- pObjDsc : pointer to the result buffer
//    <- ret_code: C_OK : object found
//                 C_SYNTAX_ERROR : else
//    <- EAX     : total length of the result

FUNCTION GLOBAL _GetObjNameN
VAR_INPUT
#ifdef _LSL_TARGETARCH_ARM
 pLng            : ^UINT;
#endif
 pCls            : ^CLSHDR;
 pObj            : ^OBJ;
 pObjDsc_        : pVoid;
END_VAR
VAR_OUTPUT
 ret_code        : CONFSTATES;
END_VAR

#define pObjDsc  pObjDsc_$^OBJENTRY

	if pObj = NIL THEN
		pObj := pCls^.pFirstObj;
	ELSE
		pObj  := pObj^.pNxtObj;
	END_IF;
 		
	IF pObj <> NIL THEN
	    pObjDsc^.udAddr := pObj $ pVoid;
	    _GetObjName(LSL_GetPtr(pObj), #pObjDsc^.szName[0]);
	#ifdef _LSL_TARGETARCH_ARM
    	pLng^    := to_uint(_strlen(#pObjDsc^.szName[0]) + (SIZEOF(pVoid)+SIZEOF(UINT)));
    #else
       	EAX      := _strlen(#pObjDsc^.szName[0])+(SIZEOF(pVoid)+SIZEOF(UINT));
    #endif 
	    ret_code := C_OK;
	    RETURN;
	END_IF;

	pObjDsc^.udAddr := NIL;
 #ifdef _LSL_TARGETARCH_ARM 
	pLng^ := SIZEOF(pVoid)+SIZEOF(UINT);
 #else
	EAX   := SIZEOF(pVoid)+SIZEOF(UINT);
 #endif
 
	ret_code := C_OK;

#undef pObjDsc

END_FUNCTION
//    
//      ret_code := _GetClsChan(pCls, uiObj, pObjDsc);
//    
//    This function copies the address and the full path name
//    of an object into the buffer specified by the 'pObjDsc'
//    parameter.
//    
//    -> pCls    	: pointer to the class descriptor
//    -> pCh		 	: pointer to the previous channel descriptor
//									!!! NIL returns the first channel !!!
//    <- pChDsc 	: pointer to the result buffer
//    <- ret_code	: C_OK : object found
//                 	C_SYNTAX_ERROR : else
//    <- EAX     	: total length of the result

FUNCTION GLOBAL _GetClsChan
VAR_INPUT
#ifdef _LSL_TARGETARCH_ARM
 pLng            : ^UINT;
#endif
 pCls            : ^CLSHDR;
 pCh             : ^ChDsc;
 pChDsc_         : ^void;
END_VAR
VAR_OUTPUT
 ret_code        : CONFSTATES;
END_VAR
VAR
	uiCh,
	uiChs				: UINT;
	pFirstCh			: ^ChDsc;
	pDsc				: ^ClsHdrConst;
END_VAR

	#define pChDsc pChDsc_$^ChEntry

	pDsc		:= pCls^.pDsc;
	pFirstCh	:= #pCls^.aDscChs[0];

	IF pCh = NIL THEN
		pCh := pFirstCh;
	ELSE
		pCh += sizeof(ChDsc);
	END_IF;

	uiChs := pDsc^.nSvrChs+pDsc^.nCltChs;
	uiCh  := (pCh$UDINT-pFirstCh$UDINT)$UINT / sizeof(ChDsc);	// get channel number from offsets
 		
	IF uiCh < uiChs THEN
		pChDsc^.udAddr 	:= pCh $ pVoid;
    // Der ChMode muß als 4 Byte Wert zurückgeschickt werden, damit man mit Lasal1 kompatibel ist
    pChDsc^.uiMode$UDINT := 0;
		pChDsc^.uiMode 	:= pCh^.pDsc^.uiMode;
		_strcpy(#pChDsc^.szName[0]+2, _GetChName(pCls, uiCh));
    
   #ifdef _LSL_TARGETARCH_ARM
		pLng^ := to_uint(_strlen(#pChDsc^.szName[0]+2)+(SIZEOF(pVoid)+SIZEOF(CHMODE)+2+SIZEOF(UINT)));
   #else
		EAX := _strlen(#pChDsc^.szName[0]+2)+(SIZEOF(pVoid)+SIZEOF(CHMODE)+2+SIZEOF(UINT));
   #endif									
		ret_code := C_OK;
		RETURN;
	END_IF;

	pChDsc^.udAddr := NIL;
  
 #ifdef _LSL_TARGETARCH_ARM
  pLng^ := SIZEOF(pVoid) + SIZEOF(UINT);
 #else 
	EAX := SIZEOF(pVoid) + SIZEOF(UINT);
 #endif
 
	ret_code := C_OK;

	#undef pChDsc

END_FUNCTION
//    
//      EAX := _GetMethNameN(pCls, uiMeth);
//    
//    This function retrieves information about the 'uiMeth'-
//    method from the method list of class 'pCls'.
//    
//    -> pCls     : pointer to the descriptor of the class
//    -> uiMeth   : number of the method
//    <- pResu    : pointer to the result buffer
//    <- EAX      : total length of return information
(* LSL2
FUNCTION GLOBAL _GetMethNameN
VAR_INPUT
	pCls			: ^ClsHdr;
	uiMeth			: UINT;
	pResu			: ^MethDscEntry;
END_VAR
VAR
	pMethName		: PCHAR;
END_VAR

#if sizeof(MethDscEntry.nParas) <> 1 | sizeof(MethDscEntry.nRets) <> 1
	#pragma message ("Zuweisung kontrollieren!");
#endif

  IF pCls^.nMeths <= uiMeth THEN
    pResu^.nRets := 16#FF;
    EAX := 0;
    RETURN;
  END_IF;

  pResu^    := _GetMethDsc(pCls, uiMeth)^;
  pMethName := _GetMethName(pCls, uiMeth);
  _strcpy(pResu$^char + MethDscEntry.pName, pMethName);
  EAX       := _strlen(pMethName)+(2*sizeof(usint)+sizeof(UINT));

END_FUNCTION
//    
//      pMeth := _GetMethDsc(pCls, uiMeth);
//    
//    This function returns a pointer to the method descriptor
//    of the specified class.
//    
//    -> pCls     : pointer to the class descriptor
//    -> uiMeth   : index of the method
//    <- pMeth    : pointer to the method descriptor
FUNCTION _GetMethDsc
VAR_INPUT
	pCls		: ^ClsHdr;
	uiMeth		: UINT;
END_VAR
VAR_OUTPUT
	pMeth		:^MethDscEntry;
END_VAR

	IF uiMeth >= pCls^.nMeths THEN
		pMeth	:= NIL;
		RETURN;
	END_IF;

	pMeth	:= (#pCls^.aDscChs[pCls^.nInitValues])$^MethDscEntry;
	pMeth	+= sizeof(MethDscEntry) * uiMeth;

END_FUNCTION
*)
// Returns a CRC from all descriptor entries. This CRC can 
// be us used on restart, to check if the list of objects remained
// the same, and if it's therfore not necessary to retrieve all
// the object IDs once more.
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL _GetDesc_CRC
VAR_INPUT
	pCrc            : ^UDINT;
END_VAR
VAR_OUTPUT
  retc : UDINT;
END_VAR

	pCrc^ := OPS.udDescCRC;
	retc  := SIZEOF(results.uiLng)+SIZEOF(OPS.udDescCRC);
  
END_FUNCTION
#else
FUNCTION GLOBAL _GetDesc_CRC
VAR_INPUT
	pCrc            : ^UDINT;
END_VAR

	pCrc^ := OPS.udDescCRC;
	EAX   := SIZEOF(results.uiLng)+SIZEOF(OPS.udDescCRC);
  
END_FUNCTION
#endif

//    
//      pLastObj := _GetObjIdx(pCls, uiIdx);
//    
//    This function returns a pointer to the n-th object of a
//    class. If the number of elements is less than uiIdx, the
//    pointer to the last object will be returned.
//    If there are no objects defined for a class, then a
//    pointer to the end of the class descriptor is returned.
//    This is the place in memory, where the first object
//    should be stored.
//    
//    -> pCls:  pointer to the class descriptor
//    -> uiIdx: index of the object that should be retrieved
//              (1 .. number of objects)
//    <- pObj:  pointer to the object
//
#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL _GetObjIdx
VAR_INPUT
	pCls			: ^ClsHdr;
	uiIdx			: UINT;
END_VAR
VAR_OUTPUT
	pLastObj		: ^OBJ;
END_VAR
VAR
	pObj			: ^OBJ;
END_VAR

  if(uiIdx = pCls^.nObjs) then
    pLastObj := pCls^.pLastObj;
  else
    pObj := pCls^.pFirstObj;
    while(uiIdx > 0) do
      if(pObj = NIL) then
        exit;
      end_if;
      pObj := pObj^.pNxtObj;
      uiIdx -= 1;
    end_while;
    pLastObj := pObj;
  end_if;

END_FUNCTION
#else
FUNCTION AWL GLOBAL _GetObjIdx
VAR_INPUT
	pCls			: ^ClsHdr;
	uiIdx			: UINT;
END_VAR
VAR_OUTPUT
	pLastObj		: ^OBJ;
END_VAR
VAR
	pObj			: ^OBJ;
END_VAR

//  pObj     := pCls^.pFirstObj;
		L.EDI		pCls
		lzxw.ecx	uiIdx

		cmp.CX		(EDI+ClsHdr.nObjs)
		jne			_GetObjIdx_search
// der letzte pointer !!!
		l.ESI		(edi+ClsHdr.pLastObj)
		jmp			ENDE

_GetObjIdx_search
		L.EDI		(EDI+ClsHdr.pFirstObj)


//  pLastObj := pObj;
		S.EDI		ESI
		JECXZ		ENDE

OBJ_LOOP
//  WHILE uiIdx <> 0 & pObj <> NIL DO
		O.EDI		EDI
		JZ			ENDE
		
//    pLastObj := pObj;
		S.EDI		ESI

//    pObj     := pObj^.pNxtObj;
		L.EDI		(EDI+Obj.pNxtObj)

//    uiIdx    -= 1;
		DEC			CX
		JNZ			OBJ_LOOP
//  END_WHILE;

ENDE
		S.ESI		pLastObj

END_FUNCTION
#endif
//    
//      eChMode := _GetChFromOff (uiOff, #pCls);
//    
//    Calculates the pointer to a channel (data structure within
//    an object) form the offset within the object.
//    The mode of the channel is detected and set to _CH_NONE
//    if the given offset doesn't point to a channel at all.
//    
//    -> uiOff: offset to the channel within the object
//    -> ppCls: pointer to the class of the object
//    
//    <- eChMode: _CH_NONE offset doesn't correspond with
//                         the channel descriptor structure off
//                         the class
//                _CH_SRV  data channel
//                _CH_CMD  command channel
//    
//    ==========================================================
//    GR, 08.09.99
//
FUNCTION GLOBAL _GetChFromOff
VAR_INPUT
	uiOff           : UINT;
	ppCls           : ^pClsHdr;
END_VAR
VAR_OUTPUT
	uiCh            : UINT;
END_VAR
VAR
	pCls            : ^ClsHdr;
	uiAnzCh         : UINT;
	pEntry          : ^ChDsc;
	pDsc			: ^ClsHdrConst;
END_VAR

	pCls		:= ppCls^;
	pDsc		:= pCls^.pDsc;
	pEntry  	:= #pCls^.aDscChs[0];
	uiAnzCh 	:= pDsc^.nSvrChs + pDsc^.nCltChs;

	FOR uiCh := 1 TO uiAnzCh DO

		IF uiOff = pEntry^.pDsc^.uiOff THEN
			uiCh	-= 1;
			RETURN;
		END_IF;

		pEntry += SIZEOF(ChDsc);
	END_FOR;

	pCls := pCls^.pBaseCls;
	IF pCls <> NIL THEN
		ppCls^ := pCls;
		uiCh := _GetChFromOff(uiOff, ppCls);
	ELSE
		uiCh := -1 $ UINT;
	END_IF;

END_FUNCTION

(*
  Liefert die CRC der Tabelle _IO_ELEMENTS_TAB.
*)
FUNCTION GetIoElementsTabCrc
VAR_OUTPUT
	crc : UDINT;
END_VAR
VAR
  pTab : ^void;
  tabSize : UDINT;
END_VAR

  crc := 0;

	pTab := _FindFctName("_IO_ELEMENTS_TAB");
  IF pTab = NIL THEN
    // keine Tabelle vorhanden, kann bei älteren Lasal Versionen der Fall sein
    RETURN;
  END_IF;
  
  (* 
  Aufbau der Tabelle _IO_ELEMENTS_TAB:
    4   Tabellengröße
    2   Versionsnummer
    2   Anzahl der Tabelleneinträge
    n-mal:
      0-terminierter String: IO-Element
      0-terminierter String: Objektname(Server)
  *)
  tabSize := pTab$^UDINT^;
	pTab += sizeof(UDINT); // Tabellengröße überspringen
  
  crc := CRC32_Buffer(pTab, tabSize);
  
END_FUNCTION

FUNCTION GLOBAL _CalcDescCRC
VAR
	pDsc		: ^OBJDSC;
	pCls		: ^ClsHdr;
	pCmd		: ^CMDMETH;
	uiCnt		: UINT;
	ppCD		: ^ChDsc;
	udConnCRC	: UDINT;
	pObj		: ^Obj;
	pThis		: ^VirtualBase;
	pCh			: ^CltCh;
	pCDsc		: ^ClsHdrConst;
	pChDsc		: ^ChDscEntry;
END_VAR

	udConnCRC	:= 0;
	pCls		:= _GetClsList();
	OPS.udDescCRC  := 0;

	WHILE pCls <> NIL DO        // calculate crc of object descriptor list
    IF pCls^.nObjs > 0 THEN
      pObj	:= pCls^.pFirstObj;
  
      WHILE pObj <> NIL DO
        pDsc	:= pObj^.pObjDsc;
        OPS.udDescCRC  += CRC32_Buffer(pDsc, TO_UDINT(ObjDsc.SymName.Text));
        pObj	:= pObj^.pNxtObj;
      END_WHILE;
    END_IF;
		pCls := pCls^.pNxtCls;
	END_WHILE;

	pCmd := pFirstCmd;              // calculate crc of the command tables
	WHILE pCmd <> NIL DO
		OPS.udDescCRC   += CRC32_Buffer(#pCmd^.nCmds, sizeof(CMDMETHSIZE));
		pCmd := pCmd^.pNxt;
	END_WHILE;
  
	pCls := _GetClsList();
	OPS.udDescCRC += OPS.udDescCRC SHL 16;

	WHILE pCls <> NIL DO                    // calculate crc of class list

    // DEVOS-993, 02.02.223: Der Zeiger auf ClsHdrConst wird nicht mehr in die CRC mit 
    // eingerechnet. Dadurch hat die Codeadresse der LOBs keine Auswirkung mehr auf die CRC.
    //  alt:
		//    OPS.udDescCRC   += CRC32_Buffer(pCls, TO_UDINT(ClsHdr.pConstructor));
    //  neu:
		OPS.udDescCRC   += CRC32_Buffer(pCls+4, TO_UDINT(ClsHdr.pConstructor)-4);

		pCDsc		:= pCls^.pDsc;
		uiCnt		:= pCDsc^.nSvrChs + pCDsc^.nCltChs;
		ppCD		:= #pCls^.aDscChs[0];
		WHILE uiCnt DO
			uiCnt	-= 1;
			pChDsc	:= ppCD^.pDsc;
#if 0      
      (* Änderung in 02.02.154:
         Ab Compilerversion bewirkt der Ausdruck to_udint(ChDscEntry) eine Compilerwarnung.
         Das Ergebnis des Ausdrucks ist immer 0. Möglicherweise war das nicht die Absicht des 
         Erstellers. Es wird hier nichts geändert, weil sich dann die CRC von bestehenden 
         Projekten ändern würde. Es wird einfach to_udint(ChDscEntry) durch 0 ersetzt, damit 
         die Compilerwarnung verschwindet. *)
			OPS.udDescCRC := LDR_CRC32_BufferEx(OPS.udDescCRC, pChDsc, to_udint(ChDscEntry));
#else      
			OPS.udDescCRC := LDR_CRC32_BufferEx(OPS.udDescCRC, pChDsc, 0);
#endif      
			OPS.udDescCRC := LDR_CRC32_BufferEx(OPS.udDescCRC, #pChDsc^.Name.udCRC, SIZEOF(pChDsc^.Name.udCRC));
			ppCD			+= sizeof(ChDsc);
		END_WHILE;

		IF pCls^.nObjs <> 0 THEN
			// calculate CRCs for
			pObj		:= pCls^.pFirstObj;
			WHILE pObj DO
        //ab 2.2.57 werden auch die Adressen aller Objekte (=LasalIDs) mit eingerechnet
        OPS.udDescCRC := LDR_CRC32_BufferEx(OPS.udDescCRC, #pObj, 4);
				uiCnt		:= pCDsc^.nSvrChs;
				ppCD		:= #pCls^.aDscChs[uiCnt];
				uiCnt		:= pCDsc^.nCltChs;
				pThis		:= LSL_GetPtr(pObj);
				WHILE uiCnt DO
					uiCnt	-= 1;
					pCh			:= (pThis+ppCD^.pDsc^.uiOff)$^CltCh;
					// if all connections and all addresses of the objects
					// are still the same, then the CRC is the same, too.
					udConnCRC	+= pCh^.pCh$UDINT;
					udConnCRC	+= (pCh^.pCmd$UDINT) SHL 8;
					ppCD		+= sizeof(ChDsc);
				END_WHILE;
				pObj	:= pObj^.pNxtObj;
			END_WHILE;
		END_IF;

		pCls  := pCls^.pNxtCls;
	END_WHILE;

  if (_LSL_POS^.piLINKER^.LNK_GetVarChecksum <> NIL) then
    Ops.udDescCRC	+= OS_LNK_GetVarChk();
  end_if;

	Ops.udDescCRC	+= udConnCRC;

  // CRC der Tabelle der IO-Elemente mit einrechnen
  Ops.udDescCRC	+= GetIoElementsTabCrc();

END_FUNCTION
// This function returns a pointer to the next object.
// -> pointer to an object (data area!!)
// <- pointer to the next object

#ifdef _LSL_TARGETARCH_ARM
FUNCTION GLOBAL LSL_GetNxtObj
VAR_INPUT
	pObjI			: ^VirtualBase;
END_VAR
VAR_OUTPUT
	retcode			: ^VirtualBase;
END_VAR

  retcode$udint := (pObjI$udint - sizeof(Obj) + Obj.pNxtObj)$^udint^;
  if(retcode <> NIL) then
    retcode += sizeof(Obj);
  end_if;

END_FUNCTION
#else
FUNCTION AWL GLOBAL LSL_GetNxtObj
VAR_INPUT
	EDI			: ^VirtualBase;
END_VAR
VAR_OUTPUT
	EDI			: ^VirtualBase;
END_VAR

		L.EDI		  (EDI-sizeof(Obj)+Obj.pNxtObj)
		OR.EDI		EDI
		JZ			  END_OF_OBJ_LIST
    ADD.EDI	  SIZEOF(Obj)

END_OF_OBJ_LIST
END_FUNCTION
#endif
//
// 		_GetChConnection(pObjDsc, pChDsc, pName);
//
// The function retrieves the name of the connected channel.
//

FUNCTION GLOBAL _GetChConnection
VAR_INPUT
	pObjDsc			: ^ObjDsc;
	pChDsc			: ^ChDscEntry;
	pName				: pChar;
END_VAR
#ifdef _LSL_TARGETARCH_ARM
VAR_OUTPUT
  retcode : UDINT;
END_VAR
#endif
VAR
	pCh				: ^CltCh;
	pParent			: ^VirtualBase;
	pConn			: pChar;
	uiChNr			: UINT;
	pObj			: ^Obj;
	pConnObj		: ^Obj;
	pThis			: ^VirtualBase;
	pCls			: ^ClsHdr;
END_VAR
	
	pThis := LSL_GetPtr(pObjDsc);
	pCh 	:= pThis+pChDsc^.uiOff;

	IF pCh^.pCh = #pCh^.pCmd$SvrCh THEN	// not connected channel
   #ifdef _LSL_TARGETARCH_ARM
    retcode := sizeof(UINT);
   #else
		EAX := sizeof(UINT);	
   #endif
	ELSE
	  pConn := _GetEmbeddedName(pCh^.pCmd, #pParent);
		_strcpy(pName, pConn);

		pObj := LSL_GetHdr(pThis);
		pCls := pObj^.pClsHdr;
		uiChNr := _GetChFromOff(TO_UINT(pCh^.pCh$DINT-pCh^.pCmd$DINT), #pCls);
		IF uiChNr <> 16#FFFF THEN
			_strcat(pName, ".");
			pConnObj := LSL_GetHdr(pCh^.pCmd);
			_strcat(pName, _GetChName(pConnObj^.pClsHdr, uiChNr));
		END_IF;
   #ifdef _LSL_TARGETARCH_ARM 
		retcode := _strlen(pName)+sizeof(UINT);
   #else
		EAX := _strlen(pName)+sizeof(UINT);
   #endif
	END_IF;

END_FUNCTION
// This function retrieves the object name for an object with a
// given address.
//
FUNCTION GLOBAL _GetObjNameFromAddr
VAR_INPUT
	pObj		: ^VirtualBase;
	pObjName	: pChar;
END_VAR
#ifdef _LSL_TARGETARCH_ARM
VAR_OUTPUT
  retcode : udint;  
END_VAR
#else
VAR_OUTPUT
	AX			: UINT;
END_VAR
#endif
VAR
	pO			: ^Obj;
	pDsc		: ^ObjDsc;
END_VAR

	IF !!_IsAddrValid(pObj) THEN
   #ifdef _LSL_TARGETARCH_ARM
		retcode := sizeof(UINT);	// length of name is 0
   #else
		EAX := sizeof(UINT);	// length of name is 0
   #endif
		RETURN;
	END_IF;

	pO		:= LSL_GetHdr(pObj);
	pDsc	:= pO^.pObjDsc;

	IF !!_IsAddrValid2(pDsc) THEN
   #ifdef _LSL_TARGETARCH_ARM
		retcode := sizeof(UINT);
   #else
		EAX := sizeof(UINT);
   #endif
		RETURN;
	END_IF;

 #ifdef _LSL_TARGETARCH_ARM
	retcode := _GetObjName(pObj, pObjName)+sizeof(UINT);
 #else
	EAX 	:= _GetObjName(pObj, pObjName)+sizeof(UINT);
 #endif

END_FUNCTION	
// The function checks, if a given class is derived from 
// another class (pBase).
// -> pCurCls	: class to be checked
// -> pBase		: base class
// <- bIsBase	: TRUE .. classes pCurCls and pBase are the same
//							or pCurCls is derived from pBase
//				  FALSE.. else
//
FUNCTION GLOBAL __cdecl _IsBaseCls
VAR_INPUT
	pCurCls	: pClsHdr;
	pBase	: pClsHdr;
END_VAR
VAR_OUTPUT
	bIsBase	: BOOL;
END_VAR

	IF pCurCls = NIL THEN
		bIsBase := FALSE;
	ELSIF pCurCls = pBase THEN
		bIsBase := TRUE;
	ELSE
		bIsBase := _IsBaseCls(pCurCls^.pBaseCls, pBase);
	END_IF;

END_FUNCTION

//
//	THR 090101
//	_IsKindOf: Check for Parent - Child Class Relationship
//		--> pObj: A pointer to the child object (any class casted)
//		--> pBase: A pointer to the base class header
//		<-- bResult: TRUE if pChild is derived from pBase, FALSE if not
//
//	Note: This function is C friendly!
//
FUNCTION GLOBAL __CDECL _IsKindOf
VAR_INPUT
	pChild	: ^VirtualBase;
	pBase	: ^ClsHdr;
END_VAR
VAR_OUTPUT
	bResult	: UDINT;
END_VAR
VAR
	pODsc	: ^Obj;
END_VAR

	// Get pointer to the class headers
	pODsc := LSL_GetHdr(pChild);
	bResult := _IsBaseCls(pODsc^.pClsHdr, pBase);

END_FUNCTION
//Get all class pointers that are derived form the given base class.
//[>pFirst]	NIL for the first class, else the previous entry
//[>pBase]	pointer to the base class
//[<pNext]	pointer to the next derived class
FUNCTION GLOBAL _GetNextDerivedClass
VAR_INPUT
	pFirst	: pClsHdr;
	pBase	: pClsHdr;
END_VAR
VAR_OUTPUT
	pNext	: pClsHdr;
END_VAR

	IF pFirst = NIL THEN
		pNext := _GetClsList();	// start from beginning
	ELSE
		pNext := pFirst^.pNxtCls;	// start from last class
	END_IF;

	WHILE pNext <> NIL DO			// still in class list

		IF _IsBaseCls(pNext, pBase) THEN
			EXIT;
		END_IF;

		pNext := pNext^.pNxtCls;

	END_WHILE;

END_FUNCTION
//Get all object pointers of classes that are derived form the given base class.
//[>pObj]	NIL for the first object, else the previous entry
//[>pBase]	pointer to the base class
//[<pNext]	pointer to the next derived object
FUNCTION GLOBAL _GetNextDerivedObj
VAR_INPUT
	pObj            : ^VirtualBase;
	pBase           : ^ClsHdr;
END_VAR
VAR_OUTPUT
	pNext			: ^VirtualBase;
END_VAR
VAR
	pCls		: pClsHdr;
	pOOb		: ^Obj;
END_VAR

	IF pObj = NIL THEN
		pCls	:= NIL;
		pOOb	:= NIL;
	ELSE
		pNext	:= LSL_GetNxtObj(pObj);

		IF pNext = NIL THEN	// no more objects of current class
			pOOb	:= LSL_GetHdr(pObj);	// in case pNext=NIL
			pCls	:= pOOb^.pClsHdr;
			pOOb	:= NIL;
		ELSE
			RETURN;
		END_IF;

	END_IF;

	WHILE pOOb = NIL DO					// no object found

		pCls	:= _GetNextDerivedClass(pCls, pBase);

		IF pCls = NIL THEN				// no more derived classes
			pNext := NIL;
			RETURN;
		END_IF;

		IF pCls^.nObjs THEN
			pOOb := pCls^.pFirstObj;	// 1st obj of next class
		END_IF;							// else ...continue

	END_WHILE;

	pNext := LSL_GetPtr(pOOb);

END_FUNCTION
// Called by the OS to get all servers with retentive data. This is just a
// helper function, see _GetNextRAMch.
//[>pCls]
//[>pObj]
//[>dOff]
//[<pNext]
FUNCTION	_GetNextRAMChannel
VAR_INPUT
	pCls		: ^ClsHdr;
	pObj		: ^VirtualBase;
	dOff		: DINT;
END_VAR
VAR_OUTPUT
	pNext		: ^SvrCh;
END_VAR

VAR
	pBase		: ^ClsHdr;
	pDsc		: ^ClsHdrConst;
	ppCD		: ^ChDsc;
	uiAnzCh		: UINT;
	dAct		: DINT;
END_VAR

	pBase		:= pCls^.pBaseCls;
	IF pBase <> NIL THEN
		pNext := _GetNextRAMChannel(pBase, pObj, dOff);
		IF pNext THEN
			RETURN;
		END_IF;
	END_IF;

	ppCD  		:= #pCls^.aDscChs[0];
	pDsc		:= pCls^.pDsc;
	uiAnzCh 	:= pDsc^.nSvrChs;

	FOR uiAnzCh := uiAnzCh TO 1 BY -1 DO

		dAct	:= ppCD^.pDsc^.uiOff;
		IF dOff < dAct THEN
			pNext := (pObj$UDINT+dAct$UDINT) $ ^SvrCh;
			IF GetFlagC( pNext^, ATTR_RETENTIVE ) THEN
				RETURN;
			END_IF;
		END_IF;

		ppCD	+= sizeof(^ChDsc);

	END_FOR;

	pNext		:= NIL;

END_FUNCTION
// The function checks, if a channel has retentive data.
// -> pObj	: pointer to the current object
// -> pCh	: pointer to any server channel of the object or
//				to NIL
// <- pNext	: pointer to the next (first) channel with retentive
//				data, NIL if none was found
//
FUNCTION __CDECL GLOBAL _GetNextRAMch
VAR_INPUT
	pO			: ^Obj;				// pointer to an object
	pCh			: ^SvrCh;			// pointer to current channel
END_VAR
VAR_OUTPUT
	pNext		: ^SvrCh;			// pointer to next channel
END_VAR

VAR
	pCls		: ^ClsHdr;
	pObj		: ^VirtualBase;
	dOff		: DINT;
END_VAR

	pObj		:= LSL_GetPtr(pO);
	pCls		:= pO^.pClsHdr;

	IF pCh = NIL THEN		// 1st call
		dOff 	:= -1;
	ELSE
		dOff	:= (pCh$UDINT-pObj$UDINT)$DINT;
	END_IF;

	pNext		:= _GetNextRAMChannel(pCls, pObj, dOff);

END_FUNCTION
//	Gets the object name with the appended channel name.
//	The pName-buffer must be sufficiently big, at maximum
//	255 characters.
//
FUNCTION GLOBAL __CDECL _GetObjectName
VAR_INPUT
	pO			: ^Obj;				// pointer to an object header
	pCh			: ^void;			// pointer to current channel
	pName		: ^CHAR;			// pointer to char buffer
END_VAR

VAR
	pCls		: ^ClsHdr;
	pObj		: ^VirtualBase;
	uiCh		: UINT;
	uiLen		: UINT;
	chName		: ARRAY[0..254] OF CHAR;
END_VAR

	pObj := LSL_GetPtr(pO);
	_GetObjName(pObj, pName);  // unconnected channel
	pCls    := pO^.pClsHdr;
	uiCh    := _GetChFromOff((pCh$UDINT-pObj$UDINT)$UINT,
					           #pCls);
	IF uiCh <> -1$UINT THEN
		_strcpy(#chName[0], _GetChName(pCls, uiCh));
		uiCh 	:= _strlen(#chName[0])$UINT;
		uiLen	:= _strlen(pName)$UINT;
		IF uiCh+uiLen < 255 THEN
			(pName+uiLen)^ := '.';
			_memcpy(pName+uiLen+1, #chName[0], uiCh+1);
		END_IF;
	END_IF;

END_FUNCTION
// Set a flag, which is checked at power down or reset,
// and causes the OS to store the channels data to a file.
// The OS calls the read method of the class to get the
// actual data of the channel.
// If more than 4 bytes are to store, the read method itself 
// can call LSLFILE_Write to store additional data.
// This mechanism only works on IPC's. To store retentive data
// on other platforms the RAMex - class has to be used.
//
FUNCTION GLOBAL __CDECL SetRetentiveFlag
VAR_INPUT
	pSvr		: ^SvrCh;
END_VAR
VAR_OUTPUT
	bSet		: BOOL;
END_VAR
VAR
	pObj		: ^OBJ;
END_VAR

	SetFlag(pSvr^, ATTR_RETENTIVE);

	IF LSLFILE_IsInstalled THEN
		bSet := TRUE;
	ELSE
		pObj := LSL_GetHdr(pSvr);	// assuming it's an object pointer
		IF _memicmp(_GetClsName(pObj^.pClsHdr), "RAMex", 6) &
		   _memicmp(_GetClsName(pObj^.pClsHdr), "RAM",   4) THEN
		// If the passed server is not an object pointer, or if the class
		// of the object is different to RAMex, then a warning is prompted
		// at run-time.
		// The SetRetentiveFlag - function is supported on the IPC platform
		// only. Use sigmatek classes RAM or RAMex on all other platforms!
			TRACE_WARN("Use the RAM/RAMex - class to store retentive data on this platform!");
      LSLCLI_LoaderTracePrint("Use the RAM/RAMex - class to store retentive data on this platform!", 12);
		END_IF;
		bSet := FALSE;
	END_IF;

END_FUNCTION
// The 2 succeeding function set and check a flag, that indicates
// whether an object has got a data buffer to transmit when it's
// displayed on a LSE-screen or not.
// If the flag is set, the class must be derived from VirtualBaseInit.
// It's NewInst-method must be implemented, and it must set the result 
// buffer with the data to display on command 0 (which is COPY_DATA for 
// the String-class, it performs a strcpy and sets the length with the 
// string-length).
//
FUNCTION GLOBAL __CDECL SetDataBufferFlag
VAR_INPUT
	pObj		: ^VirtualBase;
END_VAR
VAR
	pCls		: ^ClsHdr;
	pOO			: ^Obj;
END_VAR

	pOO		:= LSL_GetHdr(pObj);
	pCls	:= pOO^.pClsHdr;

	IF pCls^.pBaseCls THEN	// just a little check
		SetFlag( pObj^.pSvrChCmd, ATTR_DATA_BUFFER );
	END_IF;

END_FUNCTION
// for objects derived from VBI, see above
FUNCTION GLOBAL __CDECL GetDataBufferFlag
VAR_INPUT
	pObj		: ^VirtualBase;
END_VAR
VAR_OUTPUT
	bSet		: BOOL;
END_VAR
	
	IF GetFlag( ATTR_DATA_BUFFER ) THEN
		bSet	:= TRUE;
	ELSE
		bSet	:= FALSE;
	END_IF;

END_FUNCTION
//Helper function for deleting an object. Here all channels, that are connected
//to the object are disconnected.
FUNCTION GLOBAL SetDisConnect
VAR_INPUT
	pObj_ChClt		: ^CltCh;
	pObjClt			: ^VirtualBase;
END_VAR
VAR_OUTPUT
	ret_code		: ConfStates;
END_VAR;

#define pBackupObject	pBackupObjFu $ _TaskObjectControl::BackupObject

#define OS_RemORtTask(t)	pBackupObject(t, OBJ_RT)
#define OS_RemOCyTask(t)	pBackupObject(t, OBJ_CT)
#define OS_RemOBgTask(t)	pBackupObject(t, OBJ_BG)

//[#ENGLISH]
// First all the tasks of the object and it's embedded objects
// are suspended. Not before this is done, the objects may be
// removed.
//[>pThis]		pointer to the object for which tasks are to suspend
//[>pFu]		pointer to OS-function for suspending tasks
FUNCTION _RemoveObjTask
VAR_INPUT
	pThis			: ^VirtualBase;
	pBackupObjFu	: ^void;
END_VAR
VAR
	uiChs			: UINT;
	uiCh			: UINT;
	pCls			: ^ClsHdr;
	pObj			: ^Obj;
	pCh				: ^CltCh;
	pDsc			: ^ClsHdrConst;
	ppCD			: ^ChDsc;
END_VAR

	pObj	:= LSL_GetHdr(pThis);
	pCls	:= pObj^.pClsHdr;

	// remove the object from the various update lists in the OS
#ifndef DUMMY_LOADER
	OS_RemORtTask(pThis);
	OS_RemOCyTask(pThis);
	OS_RemOBgTask(pThis);
#endif  

	// delete all the embedded objects of the object
	pDsc		:= pCls^.pDsc;
	uiChs		:= pDsc^.nCltChs;
	IF uiChs THEN
		uiChs		-= 1;
		ppCD	:= #pCls^.aDscChs[pDsc^.nSvrChs];
		FOR uiCh := 0 TO uiChs DO
			IF ppCD^.pDsc^.uiMode = _CH_CLT_EMB THEN
				pCh	:= pThis+ppCD^.pDsc^.uiOff;

				// delete the embedded object
				_RemoveObjTask(pCh^.pCmd, pBackupObjFu);
			END_IF;
			ppCD	+= sizeof(ChDsc);
		END_FOR;
	END_IF;

END_FUNCTION
//[#ENGLISH]
// This function removes an object from the RTDB. The object is
// given by it's address.
//[>pThis]		pointer to the object to remove
//[<bRet]	true if object could be removed
FUNCTION _RemoveObj
VAR_INPUT
	pThis			: ^VirtualBase;
END_VAR
VAR_OUTPUT
	bRet			: BOOL;
END_VAR
VAR
	pFirst,
	pLast,
	pObj		: ^Obj;
	pThisCls,
	pCls		: ^ClsHdr;
	pOtherObj	: ^Obj;
	uiChs		: UINT;
	uiCh		: UINT;
	pCh			: ^CltCh;
	pFrom,
	pTo			: ^SvrCh;
	pThisObj	: ^VirtualBase;
	pDsc		: ^ClsHdrConst;
	ppCD		: ^ChDsc;
END_VAR
	
	pObj	:= LSL_GetHdr(pThis);
	pThisCls	:=
	pCls	:= pObj^.pClsHdr;
	pFirst	:= pCls^.pFirstObj;
	pLast	:= NIL;

	// search for the succeeding object descriptor
	WHILE pFirst DO
		IF pFirst = pObj THEN
			// unchain current object header
			IF pLast = NIL THEN	// remove 1st entry of list
				pCls^.pFirstObj	:= pObj^.pNxtObj;
			ELSE
				// look for previous object of the same class
				pLast^.pNxtObj	:= pObj^.pNxtObj;
			END_IF;

			// decrement number of objects of the class
			pCls^.nObjs		-= 1;

			GOTO DELETE_OBJ;
		END_IF;
		pLast	:= pFirst;
		pFirst	:= pFirst^.pNxtObj;
	END_WHILE;

	// Don't delete the object, if there is no descriptor.
	// So the very first object of the RTDB can't be deleted
	bRet	:= FALSE;
	RETURN;

DELETE_OBJ:
	// iterate through all objects of all classes to
	// find those clients, which are connected to the
	// servers of the deleted object.
	pFrom			:= pThis$^SvrCh;
	pTo				:= pThis+pThisCls^.pDsc^.lObj;

	pCls			:= _GetClsList();

	WHILE pCls DO
		pDsc		:= pCls^.pDsc;
		uiChs		:= pDsc^.nCltChs;
		IF uiChs > 0 THEN	// are there any client channels?

			uiChs		-= 1;
			pOtherObj	:= pCls^.pFirstObj;

			WHILE pOtherObj DO
				pThisObj := LSL_GetPtr(pOtherObj);
			
				ppCD	:= #pCls^.aDscChs[pDsc^.nSvrChs];
				FOR uiCh	:= 0 TO uiChs DO

					pCh	:= pThisObj + ppCD^.pDsc^.uiOff;

					// the channel is connected to an address within
					// the deleted object
					IF pCh^.pCh >= pFrom & pCh^.pCh < pTo THEN
						SetDisConnect(pCh, pThisObj);
					END_IF;

					ppCD	+= sizeof(ChDsc);
				END_FOR;

				pOtherObj	:= pOtherObj^.pNxtObj;
			END_WHILE;

		END_IF;
		pCls	:= pCls^.pNxtCls;
	END_WHILE;

	// delete all the embedded objects of the object
	pDsc		:= pThisCls^.pDsc;
	uiChs		:= pDsc^.nCltChs;
	IF uiChs THEN
		uiChs	-= 1;
		ppCD	:= #pThisCls^.aDscChs[pDsc^.nSvrChs];
		FOR uiCh := 0 TO uiChs DO
			IF ppCD^.pDsc^.uiMode = _CH_CLT_EMB THEN
				pCh	:= pThis+ppCD^.pDsc^.uiOff;

				// delete the embedded object
				_RemoveObj(pCh^.pCmd);
			END_IF;
			ppCD	+= sizeof(ChDsc);
		END_FOR;
	END_IF;

	// object header and data are not any longer needed
	_free(pObj);

	bRet	:= TRUE;

END_FUNCTION

FUNCTION GLOBAL __cdecl NewObj
VAR_INPUT
  pClsName    : ^char;
  pObjName    : ^char (* := NIL *);
END_VAR
VAR_OUTPUT
  pNewObj     : ^VirtualBase;
END_VAR

#ifndef DUMMY_LOADER
  pNewObj := ObjectReader::NewObj(pClsName, pObjName);
#endif  

END_FUNCTION

// This function removes an object from the RTDB. It also removes
// all the connections from other object's client channels to the
// removed object's servers. Of course all embedded objects are 
// deleted.
//[>pThis]		pointer to the object
//[<ret_code]	TRUE if object was found, else FALSE
FUNCTION GLOBAL __CDECL DelThis
VAR_INPUT
	pThis			: ^VirtualBase;	
END_VAR
VAR_OUTPUT
	ret_code		: BOOL;
END_VAR

	IF _RtOSversion < 16#510 | s_pTaskCIL = NIL THEN
		TRACE_ERR("Function 'DelObj' not available with this version of RTOS");
    LSLCLI_LoaderTracePrint("Function 'DelObj' not available with this version of RTOS", 12);
		ret_code	:= FALSE;
		RETURN;
	END_IF;

#ifndef DUMMY_LOADER
	_RemoveObjTask(pThis, s_pTaskCIL^.BackupObject);
#endif  
	ret_code	:= _RemoveObj(pThis);

END_FUNCTION
// This function removes an object from the RTDB. It also removes
// all the connections from other object's client channels to the
// removed object's servers. Of course all embedded objects are 
// deleted.
//[>pObjName]	0-terminated name of the object to delete
//[<ret_code]	TRUE if object was found, else FALSE
FUNCTION GLOBAL __CDECL DelObj
VAR_INPUT
	pObjName		: ^char;
END_VAR
VAR_OUTPUT
	ret_code		: BOOL;
END_VAR
VAR
	pThis			: ^VirtualBase;	
END_VAR;

 #ifndef _LSL_USECLISTI
  CriticalLoaderSectionStart();
 #endif
 
	pThis := _LookUpObj(pObjName)$^VirtualBase;
	IF pThis = NIL THEN
		TRACE_WARN("DelObj was called with an unknown object name");
    LSLCLI_LoaderTracePrint("DelObj was called with an unknown object name", 12);
		ret_code := FALSE;
  else
	  ret_code := DelThis(pThis);
	END_IF;

 #ifndef _LSL_USECLISTI
  CriticalLoaderSectionStop();
 #endif

END_FUNCTION
//Does an object of the given class exist. Objects of derived classes are 
//counted, too.
//[>pBase]	pointer to the current class
//[<bFirst]	TRUE / FALSE
FUNCTION _IsFirstObjInCls
VAR_INPUT
	pBase		: ^ClsHdr;
END_VAR
VAR_OUTPUT
	bFirst		: BOOL;
END_VAR
VAR
	pCls		: ^ClsHdr;
END_VAR

	// as soon as the constructor is called for an object,
	// the object count is already set to 1
	IF pBase^.nObjs > 1 THEN
		bFirst	:= FALSE;
		RETURN;
	END_IF;

	pCls	:= _GetClsList();

	WHILE pCls DO
		IF pCls^.pBaseCls = pBase &	// class is derived from base
		   _IsFirstObjInCls(pCls) = FALSE THEN
			bFirst	:= FALSE;
			RETURN;
		END_IF;
		pCls	:= pCls^.pNxtCls;
	END_WHILE;

	bFirst	:= TRUE;

END_FUNCTION
// Checks if the object is the first one of this class. All the
// derived classes are investigated, too. To specify a base class
// of the current object's class the name of this class can be given.
// If no class name is specified, the current class is investigated.
//[>pThis]	pointer to object's data
//[>pName]  class name, must be class of the object or a base class
//[<bFirst]	TRUE if class is not yet used, FALSE if there are objects
FUNCTION GLOBAL __CDECL _IsFirstObj
VAR_INPUT
	pThis		: ^VirtualBase;
	pName		: ^char;
END_VAR
VAR_OUTPUT
	bFirst		: BOOL;
END_VAR
VAR
	pObj		: ^Obj;
	pCls		: ^ClsHdr;
END_VAR

	pObj	:= LSL_GetHdr(pThis);
	pCls	:= pObj^.pClsHdr;
	IF pName THEN
		WHILE pCls^.pBaseCls DO
			IF _stricmp(_GetClsName(pCls), pName) = 0 THEN
				EXIT;
			END_IF;
			pCls	:= pCls^.pBaseCls;
		END_WHILE;
	END_IF;
	bFirst	:= _IsFirstObjInCls(pCls);

END_FUNCTION
// definiert in set_up.st
FUNCTION GLOBAL _ReadConnect
VAR_INPUT
	pScan           : ^char;
	bOnePair		: BOOL := FALSE;
END_VAR
VAR_OUTPUT
	ret_code        : CONFSTATES;
END_VAR;

// Checks, if the two names are channel names and connects them.
// This function should only be called by the interpreter, because
// it writes to the passed string. For constant strings this will
// result in an exception.
// [>pChNames]	name of the channels, separated by ',', 0-terminated
FUNCTION GLOBAL ConnectChs
VAR_INPUT
	pChNames		: ^char;
END_VAR
VAR_OUTPUT
	ret_code		: ConfStates;
END_VAR

 #ifdef _LSL_USECLISTI
	ret_code	:= _ReadConnect(pChNames, TRUE);
 #else
  CriticalLoaderSectionStart();
	ret_code	:= _ReadConnect(pChNames, TRUE);
  CriticalLoaderSectionStop();
 #endif

END_FUNCTION
// Checks, if the 2 strings are valid channel names and connects them
// The whole name of the channels must be specified, i.e.
// "Object\Embedded.Channel". If the server name is an empty string, then
// an existing connection is removed.
//[>pCltName]	full name of the client channel
//[>pSvrName]	full name of the server channel
//[<ret_code]	TRUE if successful
FUNCTION GLOBAL __CDECL ConnectCltSvr
VAR_INPUT
	pCltName	: ^char;
	pSvrName	: ^char;
END_VAR
VAR_OUTPUT
	ret_code	: BOOL;
END_VAR
VAR
	udCltLen	: UDINT;
	udSvrLen	: UDINT;
	pTemp		: ^char;
END_VAR

	IF _RtOSversion < 16#0500 THEN	// OS too old for alloc
	NO_MEM:
		ret_code	:= FALSE;
		RETURN;
	END_IF;

	udCltLen	:= _strlen(pCltName)+1;
	udSvrLen	:= _strlen(pSvrName)+1;

	IF udSvrLen = 1 THEN	// disconnect clt/svr
		pSvrName	:= "^";
		udSvrLen	+= 1;
	END_IF;

	pTemp		:= _alloc_resize_LDR(udCltLen+udSvrLen)$^char;
	IF pTemp = NIL THEN
		GOTO NO_MEM;
	END_IF;

	_memcpy(pTemp, pCltName, udCltLen);
	_memcpy(pTemp+udCltLen, pSvrName, udSvrLen);

	IF _ReadConnect(pTemp, TRUE) = C_OK THEN
		ret_code	:= TRUE;
	ELSE
		ret_code	:= FALSE;
	END_IF;

	_free(pTemp);

END_FUNCTION

VAR_PRIVATE
#ifndef DUMMY_LOADER
	sPtrs		: ^PtrChk;
#else
	sPtrs		: pVoid;
#endif
  crcMapMutex : MT_SEMAHANDLE;
#ifndef DUMMY_LOADER
  mt_api          : ^LSL_MT_TYPE; // multitask interface (needed for semaphores)
#endif  
END_VAR

//[#ENGLISH]
//This function iterates through all classes and checks if a method
//pointer is within the specified range of udStart/udEndModule. If so,
//then the pointer is stored to a list and exchanged when _ApplyLink
//is called. Also the pointer to the descriptors in the lob's class table
//are updated.
//The function does not check for non-shared command tables.
//[>udStartModule]starting address of the old module
//[>udEndModule]last address of the old module
//[<bDone]TRUE.. no allocation problems, FALSE ..do not exchange module
FUNCTION GLOBAL __CDECL _PrepareLink
VAR_INPUT
	udStartModule	: UDINT;
	udEndModule		: UDINT;
END_VAR
VAR_OUTPUT
	bDone			: BOOL;
END_VAR
VAR
	pCls      : ^ClsHdr;
  pObj      : ^Obj;
  szClassTable  : ARRAY [0..NLNG] OF CHAR;
  pNewClsDsc    : ^ClsHdrConst;
END_VAR


#ifndef DUMMY_LOADER

  // allocate PtrChk object
	sPtrs	:= _alloc(sizeof(PtrChk))$^PtrChk;
	IF sPtrs = NIL THEN
		bDone	:= FALSE;
		RETURN;
	END_IF;

  // set range of code-module which has to be exchanged
	sPtrs^.InitPtr(udStartModule, udEndModule);

  // get pointer to first class
	pCls	:= _GetClsList();

  // check all classes
  while (pCls) do
    pObj := pCls^.pFirstObj;
    
    // at least one object has to exist for virtual address corrections
    while (pObj) do
      PrepLnk_IterateVMTs(pCls, LSL_GetPtr(pObj)$^SvrChCmd);    // search through addresses in all virtual method tables
      if pCls^.nObjs = 0 then // nObjs is 0 in _OSI classes
        exit;
      end_if;
      // loop over all objects of the class because method-tables could be EXCLUSIVE (not SHARED)
			pObj	:= pObj^.pNxtObj;
    end_while;
    
    // check if pointer to class and channel descriptors has to be updated
   #ifdef OTTOSPEED 
    szClassTable[0]$UDINT := 16#5F544340;
    szClassTable[4]       := 16#40;       // "@CT_@"
   #else
    _strcpy( #szClassTable[ 0 ], "@CT_@" ); 
   #endif
    _strcpy( #szClassTable[ 5 ], #pCls^.pDsc^.Name$CLexem.Text[ 0 ] );
    pNewClsDsc := _FindFctName( #szClassTable[ 0 ] )$^ClsHdrConst;        // get the beginning of the class header table
    pNewClsDsc += sizeof( udint ) + sizeof( uint );                       // ClsHdrConst starts after 6 bytes (Tablelength: 4 Bytes, Table-ID, 2 Bytes)
    if (pCls^.pDsc <> pNewClsDsc) then
      PrepLnk_UpdateDSCs(pCls, pNewClsDsc);                               // if Cls-Descriptor differs, update all descriptors of class
    end_if;
  
    pCls := pCls^.pNxtCls;
  end_while;
  
  bDone := sPtrs^.IsOk();

#endif  // DUMMY_LOADER

END_FUNCTION

// This function iterates through all servers of one class (also of the base classes),
// and searches for virtual method address which has to be updated.
FUNCTION PrepLnk_IterateVMTs
VAR_INPUT
  pCls      : ^ClsHdr;
  pClassSvr : ^SvrChCmd;
END_VAR
VAR
	uiNrSvrChs, i, j   : UINT;
  pChnlDsc          : ^ChDsc;
  usChMode          : USINT;
  pActSvr           : ^SvrChCmd;
  pMeth             : ^CmdMeth;
  uiNrMeths         : UINT;
END_VAR

#ifndef DUMMY_LOADER

  // iterate all server of class and eventually existing base class(es) and their virtual methods.
  // base class(es) is important because first (or more) server are declared at base class.
  while pCls do
    uiNrSvrChs := pCls^.pDsc^.nSvrChs;      // nr of server of this class
    pChnlDsc := #pCls^.aDscChs[0];          // descriptor of first server of this class
    
    i := 0;
    while (i < uiNrSvrChs) do               // check every server of this class
    
      // check channel mode
      case pChnlDsc^.pDsc^.uiMode of
      _CH_SVR:
        usChMode := 1;                      // signature for "only Read- and Write-methods"
        
      _CH_SVR_OBJ,
      _CH_CMD:
        usChMode := 2;                      // signature for more virtual methods
      
      else
        usChMode := 0;                      // signature for other channel mode -> do nothing
        
      end_case;

      // Mode has to be _CH_SVR, _CH_SVR_OBJ or _CH_CMD, otherwise do nothing
      if usChMode <> 0 then
        pActSvr := pClassSvr + pChnlDsc^.pDsc^.uiOff;
        pMeth := pActSvr^.pMeth;
        
        sPtrs^.CheckPtr(#pMeth^.Read);          // read and write method has to be checked for every server
        // When the write-method points to retentiveWriteSplitter, then we have to check 
        // the original pointer that is stored in pSvrDsc (points to a RetSvr structure)
        if pMeth^.Write = #retentiveWriteSplitter() then
          if pActSvr^.pSvrDsc <> NIL then
            sPtrs^.CheckPtr(#pActSvr^.pSvrDsc$^RetSvr^.pMeth);
          end_if;
        else
          sPtrs^.CheckPtr(#pMeth^.Write);
        end_if;
        
        // if server has more virtual methods, check
        if (usChMode = 2) then
        
          uiNrMeths := pMeth^.nCmds$UINT;
          pMeth := (#pMeth^.Init)$^CmdMeth;
          j := 0;
          while j < uiNrMeths do                // check all additional virtual methods
            sPtrs^.CheckPtr(pMeth$^pVoid);
            pMeth += sizeof(pVoid);
            j += 1;
          end_while;
          
        end_if;
      
      end_if;
    
      pChnlDsc += sizeof(ChDsc);                // iterate to next server description
      i += 1;
    end_while;

     
    // iterate through all possibly existing base classes
    pCls := pCls^.pBaseCls;
  end_while;

#endif  // DUMMY_LOADER

END_FUNCTION

// This function updates all pointers to descriptors in the lob-file during download on the fly.
// Descriptors are available for class, server and clients.
FUNCTION PrepLnk_UpdateDSCs
VAR_INPUT
  pCls        : ^ClsHdr;
  pNewClsDsc  : ^ClsHdrConst;
END_VAR
VAR
	uiNrChannels  : UINT;
  pLexem      : ^CLexem;
  pChnDsc     : ^ChDsc;
  pNewChDsc   : ^ChDscEntry;
  i           : UINT;
END_VAR

#ifndef DUMMY_LOADER

  uiNrChannels := pCls^.pDsc^.nSvrChs + pCls^.pDsc^.nCltChs;    // determine the number of channels
  // only update if there is at least one channel defined (derived class with no additional channels has no chnl-descriptions)
  if (uiNrChannels > 0) then
    pChnDsc := #pCls^.aDscChs[0];     // set pointer to first channel description array entry in class description
    pLexem := #pNewClsDsc^.Name;      // set lexem-pointer to class name
    pLexem += pLexem^.Size();         // overread name of class
    pLexem += pLexem^.Size();         // overread name of base class
    pLexem += 2*sizeof(UINT);         // overread revision of base class
    pNewChDsc := pLexem$^ChDscEntry;  // this is the first channel description in new lob-file
    
    // update description pointer for all channels
    i := 0;
    while (i < uiNrChannels) do
      pChnDsc^.pDsc := pNewChDsc;                                   // update description pointer
      pChnDsc += sizeof(ChDsc);                                     // adjust pointer to next channel
      pNewChDsc += sizeof(ChDscEntry) + pNewChDsc^.Name.Length();   // search for next channel description in new lob-file (0-termination of string is included in sizeof(ChDscEntry))
      i += 1;
    end_while;
  
  end_if;
  
  // update pointer to class description
  pCls^.pDsc := pNewClsDsc;

#endif  // DUMMY_LOADER

END_FUNCTION


//[#ENGLISH]
//This function can only be called after a call to _PrepareLink. It
//takes the entries from the pointer-buffer allocated and initialized
//by the PrepareLink-function and exchanges the method pointers.
//The OS must not perform any calls to any user-method at the time this
//function is called.
FUNCTION GLOBAL __CDECL _ApplyLink
VAR_OUTPUT
	bDone			: BOOL;
END_VAR

#ifndef DUMMY_LOADER
	IF sPtrs = NIL | sPtrs^.IsOk() = FALSE THEN
		bDone	:= FALSE;
		RETURN;
	END_IF;

	sPtrs^.Apply();
	sPtrs^.Delete();

	bDone	:= TRUE;
#endif  

END_FUNCTION
// ************************************************
//       eintragen einer CRC in einen Mapper
// ************************************************



FUNCTION GLOBAL MyXSort VAR_INPUT p1:^void; p2:^void; END_VAR VAR_OUTPUT retcode : dint; END_VAR;

FUNCTION GLOBAL MySort
	VAR_INPUT
		src 	: ^USINT;
		no 	: UDINT;
		itemsize 	: UDINT;
		sortfunct 	: ^void;
	END_VAR
	VAR_OUTPUT
		retcode 	: BOOL;
	END_VAR
  var
    tmp, hp : ^usint;
    mx, nx, wx, parent, child, max, i : dint;
    spuf : array[0..511] of usint;
  end_var

  retcode := false;
  
  if(itemsize <= sizeof(spuf)) then
  
    nx      := to_dint(no);
    mx      := (nx + 6) / 8;
    src     := src;
    tmp     := #spuf[0];
    retcode := true;

    while(1) do
      if(mx) then
        mx     -= 1;
        parent := mx;
        _memcpy(tmp, src + (parent$udint*itemsize), itemsize);
      else
        nx -= 1;
        if(nx) then
          hp := src + (nx$udint*itemsize);
          _memcpy(tmp, hp, itemsize);
          _memmove(hp, src, itemsize);
          parent := 0;
        else
          exit;
        end_if;
      end_if;

      child := parent * 8 + 1;
      while(child < nx) do
        wx := nx - child;
        if(wx > 8) then
          wx := 8;
        end_if;
        
        max := 0;
        if(wx > 1) then
          for i:=1 to wx-1 do 
            if(sortfunct$MyXSort(src+(to_udint(child+i)*itemsize), src+to_udint(child+max)*itemsize) > 0) then
              max := i;
            end_if;
          end_for;
        end_if;

        child += max;

        if(sortfunct$MyXSort(src+(child$udint*itemsize), tmp) <= 0) then
          exit;
        end_if;
        
        _memcpy(src+(parent$udint*itemsize), src+(child$udint*itemsize), itemsize);
        parent := child;
        child  := parent * 8 + 1;
      end_while;

      _memcpy(src+(parent$udint*itemsize), tmp, itemsize);
    end_while;
  end_if;
  
END_FUNCTION

FUNCTION GLOBAL VglONEOBJ 
  VAR_INPUT 
    p1 : ^ONEOBJ; 
    p2 : ^ONEOBJ; 
  END_VAR 
  VAR_OUTPUT 
    retcode : dint; 
  END_VAR

  retcode := -1;
  if(p1^.crc$UDINT > p2^.crc$UDINT) then
    retcode := 1;
  elsif(p1^.crc$UDINT = p2^.crc$UDINT) then
    retcode := 0;
    pMapper^.errorcnt += 1; // doppelt vorhandene crc's
    
    LSLCLI_LoaderTracePrint("ERROR: Same Crc at following Objects, please change Name...", 12);
    LSLCLI_LoaderTracePrint(#p1^.pOBJ^.pObjDsc^.SymName.Text[0], 12);
    LSLCLI_LoaderTracePrint(#p2^.pOBJ^.pObjDsc^.SymName.Text[0], 12);
    
    TRACE_ERR("ERROR: Same Crc at following Objects, please change Name...");
    TRACE0_ERR("'{0}'", #p1^.pOBJ^.pObjDsc^.SymName.Text[0]);
    TRACE0_ERR("'{0}'", #p2^.pOBJ^.pObjDsc^.SymName.Text[0]);
  end_if;

END_FUNCTION

FUNCTION GLOBAL LDR_LockCRCMap

  IF crcMapMutex <> NIL THEN
    OS_MT_Wait(crcMapMutex);
  END_IF;
  
END_FUNCTION

FUNCTION GLOBAL LDR_UnlockCRCMap

  IF crcMapMutex <> NIL THEN
    OS_MT_Signal(crcMapMutex);
  END_IF;
  
END_FUNCTION

FUNCTION SortCRCMap_intern
  VAR_OUTPUT
    retcode : UDINT;
  END_VAR
//  VAR
//  	timex1, timex2 : UDINT;
//  END_VAR

//  timex1 := ops.tabsolute;

  retcode := 0;
  if(pMapper <> NIL) then
  
    pMapper^.errorcnt := 0; // init error counter
    
    if(pMapper <> NIL) then
       if(pMapper^.nr > 1) then
         MySort((#pMapper^.OBJs[0])$^USINT, pMapper^.nr$UDINT, sizeof(ONEOBJ), #VglONEOBJ());
       end_if;
    end_if;
  
    pMapper^.is_sorted        := 1; // map ist sortiert
    pMapper^.has_to_be_sorted := 1; // ab jetzt muss der mapper immer sortiert sein
    retcode                   := pMapper^.errorcnt; // anzahl der aufgetretenen fehler
  end_if;
 
//  timex2 := ops.tabsolute;

END_FUNCTION

FUNCTION GLOBAL SortCRCMap
  VAR_OUTPUT
    retcode : UDINT;
  END_VAR

  LDR_LockCRCMap();

  retcode := SortCRCMap_intern();

  LDR_UnlockCRCMap();

END_FUNCTION

FUNCTION PutCRCintoMap_intern
VAR_INPUT
	CRC		:  UDINT;
	pObj	: ^Obj;
END_VAR
VAR
	ph : ^ONEOBJ;
  anz, size : udint;
  nptr : ^void;
END_VAR

  if(pMapper <> NIL) then
    if(pMapper^.nr < pMapper^.maxnr) then
      ph := #pMapper^.OBJs[pMapper^.nr];
      ph^.crc$UDINT := CRC;
      ph^.pOBJ := pOBJ;
      pMapper^.nr += 1;
      pMapper^.is_sorted := 0; // map is not sorted
      return;
    end_if;
    
    // reserviere speicher für zusätzliche 16000 objekte
    anz  := pMapper^.maxnr + 16000;
    size := CRCMap.OBJs + anz * sizeof(ONEOBJ);
    nptr := _realloc_LDR(pMapper, size);
    if(nptr <> NIL) then
      pMapper$^void := nptr;
      pMapper^.maxnr := anz;
      PutCRCintoMap_intern(crc, pObj);
    else
      if(pMapper^.errorcnt = 0) then
        LSLCLI_LoaderTracePrint("ERROR: Too much Objects", 12);
        TRACE_ERR("ERROR: Too many Objects");
      end_if;
      pMapper^.errorcnt += 1; // too much objects in map
    end_if;
  end_if;

END_FUNCTION

FUNCTION GLOBAL PutCRCintoMap
VAR_INPUT
	CRC		:  UDINT;
	pObj	: ^Obj;
END_VAR

  LDR_LockCRCMap();
  
  PutCRCintoMap_intern(CRC, pObj);

  LDR_UnlockCRCMap();

END_FUNCTION

(*
FUNCTION FindSorted
  VAR_INPUT
    start : UDINT;
    anz : UDINT;
    crc : UDINT;
  END_VAR
  VAR_OUTPUT
    retcode : ^Obj;
  END_VAR
  VAR
  	index : UDINT;
    tmpcrc : UDINT;
  END_VAR
  
  if(anz > 0) then
  
    index  := start + (anz / 2);
    tmpcrc := pMapper^.OBJs[index].crc$UDINT;

    if(crc < tmpcrc) then
      retcode := FindSorted(start, (anz / 2), crc);
      return;
    elsif (crc > tmpcrc) then
      retcode := FindSorted(index + 1, ((anz - 1) / 2), crc);
      return;
    end_if;
    
    retcode := pMapper^.OBJs[index].pOBJ;
    return;
  end_if;

  retcode := NIL;

END_FUNCTION
*)

FUNCTION FindSorted
  VAR_INPUT
    start : UDINT;
    anz : UDINT;
    crc : UDINT;
  END_VAR
  VAR_OUTPUT
    retcode : ^Obj;
  END_VAR
  VAR
  	index : UDINT;
    tmpcrc : UDINT;
  END_VAR
  
  retcode := NIL;
  
  ECONOMIZE_RECURSION :
  
  if(anz > 0) then
  
    index  := start + (anz / 2);
    tmpcrc := pMapper^.OBJs[index].crc$UDINT;

    if(crc < tmpcrc) then
      //retcode := FindSorted(start, (anz / 2), crc);
      anz := anz / 2;
      goto ECONOMIZE_RECURSION;
    elsif (crc > tmpcrc) then
      // retcode := FindSorted(index + 1, ((anz - 1) / 2), crc);
      start := index + 1;
      anz   := ((anz - 1) / 2);
      goto ECONOMIZE_RECURSION;
    else
      retcode := pMapper^.OBJs[index].pOBJ;
    end_if;
  end_if;

END_FUNCTION

FUNCTION FindUnsorted
  VAR_INPUT
    crc : UDINT;
  END_VAR
  VAR_OUTPUT
    retcode : ^Obj;
  END_VAR
  VAR
    run : UDINT;
	  ph : ^ONEOBJ;
  END_VAR

  run := pMapper^.nr;
  ph := #pMapper^.OBJs[0];

  while(run) do
    if(ph^.crc$UDINT = crc) then
      retcode := ph^.pOBJ;
      return;
    end_if;
    run -= 1;
    ph  += sizeof(ONEOBJ);
  end_while;

  retcode := NIL;

END_FUNCTION

FUNCTION FindCRCinMap_intern
VAR_INPUT
	CRC		: DINT;
END_VAR
VAR_OUTPUT
	pObj		: ^Obj;
END_VAR

  pObj := NIL;

  if(pMapper <> NIL) then
  
    if(pMapper^.is_sorted = 0) then
      if(pMapper^.has_to_be_sorted = 1) then
        SortCRCMap_intern();
      else
        pObj := FindUnsorted(CRC$UDINT);
        return;
      end_if;
    end_if;
  
    if(pMapper^.nr > 1) then
      pObj := FindSorted(0, pMapper^.nr, crc$UDINT);
    elsif(pMapper^.nr = 1) then
      if(pMapper^.OBJs[0].crc$UDINT = crc$UDINT) then
        pObj := pMapper^.OBJs[0].pOBJ;
      end_if;
    end_if;
  end_if;

END_FUNCTION

FUNCTION GLOBAL FindCRCinMap
VAR_INPUT
	CRC		: DINT;
END_VAR
VAR_OUTPUT
	pObj		: ^Obj;
END_VAR

  LDR_LockCRCMap();
  
  pObj := FindCRCinMap_intern(CRC);

  LDR_UnlockCRCMap();

END_FUNCTION


(* Sortiert die CRCMap, falls diese in einem Zustand ist, in dem sie sortiert werden soll.
   Diese Funktion wird am Ende der Loaderinitialisierung aufgerufen, um sicherzustellen, 
   dass im zyklischen Betrieb kein Sortiervorgang mehr notwendig ist. 
   Bei einem Sortiervorgang im zyklischen Betrieb könnte nämlich ein Runtime-Error auftreten.
*)
FUNCTION GLOBAL LDR_SortCRCMapIfNececcary

  LDR_LockCRCMap();
  
  if(pMapper <> NIL) then
  
    if(pMapper^.is_sorted = 0) then
      if(pMapper^.has_to_be_sorted = 1) then
        SortCRCMap_intern();
      end_if;
    end_if;
  
  end_if;

  LDR_UnlockCRCMap();

END_FUNCTION

(*
  Initialisiert die CRC Map (pMapper) anhand der aus einer CRC eines Objektnamens 
  eine Objektadresse ermittelt werden kann.
*)
FUNCTION GLOBAL LDR_InitCRCMap

  crcMapMutex := NIL;

  IF _UserDataSize >  (5*1024*1024) THEN
    // wir haben mehr als 5 MB also 512k nehmen
    pMapper := _alloc_resize_LDR(512*1024)$^CRCMap;
    _memset(pMapper, 0, 512*1024);   // loeschen des eintragsbuffers
    pMapper^.nr := 0;	       		// kein eintrag !!!
    pMapper^.maxnr := ((512*1024 - CRCMap.OBJs) / sizeof(ONEOBJ))$UDINT; // mehr ONEOBJs haben in Map nicht platz
    
    OS_CILGet("LSL_MULTITASK", #mt_api$void);
    crcMapMutex := OS_MT_CreateSemaphore(MTSEMATYPE_MUTEX, 
                                         1,    // initvalue
                                         0,    // flags: 0 means create unconitionally
                                         "crcMapMutex");
    
  ELSE
    pMapper:=NIL; 	
	END_IF;

END_FUNCTION