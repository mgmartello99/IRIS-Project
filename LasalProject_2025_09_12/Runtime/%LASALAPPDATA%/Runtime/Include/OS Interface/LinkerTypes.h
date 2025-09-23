//**********************************************************************************//
// OpSys-Classes:
//
// Header for Class: *Linker*
#pragma once
#define LNKTYPE


  TYPE
    LSLLNK_VariableInfo   : STRUCT
      address             : UDINT;
      size                : UDINT;
      status              : UDINT;
      initptr             : pVOID;
    END_STRUCT;

    LSLLNK_ProjectState   : (PRJ_ISNOTAVAIL, 
                             PRJ_ISEXE, 
                             PRJ_ISNOTLINKED,   
                             PRJ_ISLINKED
                            )$UINT;
  END_TYPE

//
//	Linker Error Message Length
//
#define LNK_ERRORMSG_LEN 132

//
// Modultypen
// werden bei #GetChksum benötigt
#define MODTYPE_LOADER    0x00000001
#define MODTYPE_APPL      0x00010000

