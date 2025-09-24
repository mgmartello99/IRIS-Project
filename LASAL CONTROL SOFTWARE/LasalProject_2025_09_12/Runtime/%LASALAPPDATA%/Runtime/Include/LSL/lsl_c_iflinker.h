//****************************************************************************/
//*                                                                          */
//*  File: LSL_C_IFLINKER.H                                                  */
//*  Date: JAN 29, 2001                                                      */
//*  By:                                                                     */
//*  Description:                                                            */
//*		This file contains the LASAL OS System Linker Routine Interface      */
//*  Revision History:                                                       */
//*      Rev     By      Date        Description                             */
//*      ---     --      ----        -----------                             */
//*      1.10    MENK    29/01/01    Initial Version                         */
//*                                                                          */
//****************************************************************************/
#pragma once
#define LSL_IFLINKER_H


typedef struct
{
  unsigned long  address;
  unsigned long  size;
  unsigned long  status;
  void          *initptr;
} LSLLNK_VariableInfo;

typedef enum 
{
	PRJ_ISNOTAVAIL,   
	PRJ_ISEXE,   
	PRJ_ISNOTLINKED,   
  PRJ_ISLINKED
}LSLLNK_ProjectState;

typedef enum
{
    CH_OBJ,
	  CH_CMD,
	  CH_SVR,
	  CH_CLT_CMD,
	  CH_CLT_DATA,
	  CH_CLT_OBJ,
	  CH_OBJ_DIR,
	  CH_CNST,
	  CH_NONE
}	LSLLNK_ChMode;


typedef LSLLNK_ChMode FCTTYP_LookupEmbedded(void *, void**, void**);   
//	MAINSTARTTYPE *Adresse;

//
//	Define Linker Error Message Length
//
#define LNK_ERRORMSG_LEN 132


/***************************************************************************\
> CIL API Interface                                                         <
\***************************************************************************/
typedef struct
{
	// Interface Function Pointers
   unsigned long       (* pLNK_GetFirstError)(char *);         // ->pointer destination msg.buffer
                                              			   // <-return 1 if message loaded
   unsigned long       (* pLNK_GetNextError)(char *);          // ->pointer destination msg.buffer
                                                               // <-return 1 if message loaded
   unsigned long       (* pLNK_GetAddress_Function)(char *, char *);
   unsigned long       (* pLNK_GetAddress_Variable)(char *);
   long                (* pLNK_GetVariableInfo)(char *, LSLLNK_VariableInfo *);
   unsigned long       (* pLNK_GetDataMemoryAddress)(void);
   unsigned long       (* pLNK_GetCodeMemoryAddress)(void);
   LSLLNK_ProjectState (* pLNK_GetProjectState)(void);
   
   FCTTYP_LookupEmbedded(* pLNK_LookupEmbedded);
   void*               (* pLNK_GetObjDscList)(void); // V3.18

   unsigned long       (* crc32)(unsigned long, void*, unsigned long);
   unsigned long       (* pLNK_AddError2List)(unsigned long, char*);
   void                (* pLNK_FreeErrorList)(void);
   unsigned long       (* pLNK_GetErrorCount)(void);
   void*               (* pLNK_GetNextChannel)(void*, void*);  // for RAM-FileSave V3.47
   void                (* pLNK_GetObjectName)(void*, void*, char*); // for RAM-FileSave V3.47
   unsigned long       (* pLNK_GetChkByModulID)(unsigned long ID);
   unsigned long       (* pLNK_GetPrjChk)(void);
   void*               (* pLNK_XCH_MethPtr)(void *ptr0);
   unsigned long       (* pLNK_LOADER_PrepareLink)(unsigned long start0, unsigned long end0);
   unsigned long       (* pLNK_LOADER_ApplyLink)(void);
   unsigned long       (* pLNK_GetPrjChk_WithModOrder)(void);
   unsigned long       (* pLNK_GetVarCrc)(void);
   void                (* pLNK_DUMMYTRAKTOR)(void);
   void                (* pLNK_DUMMYBMW)(void);
   // v01.02.234
   unsigned long       (* pLNK_GetChkSumFromClassName)(const char *ccClassName);
   // v01.03.080
   unsigned long       (* pLNK_GetChkSumByType)(unsigned long modTypeMask, unsigned long modTypeEq, unsigned long *pCntMod);
} LSL_LINKER;




//
//
//	End of File
//
