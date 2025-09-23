FUNCTION GLOBAL TAB MMServerBase::@CT_
0$UINT,
2#0100000000000000$UINT, //TY_MMSERVERBASE
0$UINT, 0$UINT, (SIZEOF(::MMServerBase))$UINT, 
1$UINT, 0$UINT, 0$UINT, 
TO_UDINT(2479275025), "MMServerBase", //Class
TO_UDINT(0), 0, 0$UINT, 0$UINT, //Baseclass
//Servers:
(::MMServerBase.Data.pMeth)$UINT, _CH_CMD$UINT, 2#0000000000000000$UINT, TO_UDINT(2607161047), "Data", 
//Clients:
END_FUNCTION


#define USER_CNT_MMServerBase 7

TYPE
	_LSL_STD_VMETH	: STRUCT
			CmdTable	: CMDMETH;
			UserFcts	: ARRAY[0..USER_CNT_MMServerBase] OF ^Void;
	END_STRUCT;
END_TYPE

FUNCTION MMServerBase::@STD
	VAR_OUTPUT
		ret_code	: CONFSTATES;
	END_VAR
	VAR
		vmt	: _LSL_STD_VMETH;
	END_VAR

	//Command Methods
	InitCmdTable (nCmd := nSTDCMD + USER_CNT_MMServerBase, pCmd := #vmt.CmdTable);
	vmt.CmdTable.Write		:= #M_WR_DIRECT();
#pragma warning (disable : 74)
	vmt.UserFcts[0]		:= #WriteDataOff();

#pragma warning (default : 74)
#pragma warning (disable : 74)
	vmt.UserFcts[1]		:= #GetCharSize();

#pragma warning (default : 74)
#pragma warning (disable : 74)
	vmt.UserFcts[2]		:= #GetCRC();

#pragma warning (default : 74)
#pragma warning (disable : 74)
	vmt.UserFcts[3]		:= #GetDataAt();

#pragma warning (default : 74)
#pragma warning (disable : 74)
	vmt.UserFcts[4]		:= #GetBufferLen();

#pragma warning (default : 74)
#pragma warning (disable : 74)
	vmt.UserFcts[5]		:= #ReadDataOff();

#pragma warning (default : 74)
#pragma warning (disable : 74)
	vmt.UserFcts[6]		:= #GetDataPtrIn();

#pragma warning (default : 74)
	Data.pMeth		:= StoreCmd (pCmd := #vmt.CmdTable, SHARED);

	IF Data.pMeth THEN
		ret_code	:= C_OK;
	ELSE
		ret_code	:= C_OUTOF_NEAR;
		RETURN;
	END_IF;

END_FUNCTION

//{{LSL_IMPLEMENTATION
#pragma warning(disable:73);

FUNCTION VIRTUAL GLOBAL MMServerBase::WriteDataOff
	VAR_INPUT
		udLen 	: UDINT;
		udOff 	: UDINT;
		pData 	: ^USINT;
	END_VAR
  
END_FUNCTION

FUNCTION VIRTUAL GLOBAL MMServerBase::GetCharSize
	VAR_INPUT
		nBufferId 	: UDINT;
	END_VAR
	VAR_OUTPUT
		usSize 	: USINT;
	END_VAR
  
  usSize := 0;

END_FUNCTION

FUNCTION VIRTUAL GLOBAL MMServerBase::GetCRC
	VAR_OUTPUT
		udCRC 	: UDINT;
	END_VAR
  
  udCRC := 0;

END_FUNCTION

FUNCTION VIRTUAL GLOBAL MMServerBase::GetDataAt
	VAR_INPUT
		pData 	: ^USINT;
		udSize 	: UDINT;
		udAt 	: UDINT;
	END_VAR

END_FUNCTION

FUNCTION VIRTUAL GLOBAL MMServerBase::GetBufferLen
	VAR_INPUT
		nBufferId 	: UDINT;
	END_VAR
	VAR_OUTPUT
		udLen 	: UDINT;
	END_VAR
  
  udLen:= 0;
  
END_FUNCTION

FUNCTION VIRTUAL GLOBAL MMServerBase::ReadDataOff
	VAR_INPUT
		udOff 	: UDINT;
		pData 	: ^USINT;
		udMax 	: UDINT;
	END_VAR
	VAR_OUTPUT
		udLen 	: UDINT;
	END_VAR
  
  udLen := 0;
  
END_FUNCTION


FUNCTION VIRTUAL GLOBAL MMServerBase::GetDataPtrIn
	VAR_OUTPUT
		pData 	: ^USINT;
	END_VAR
  
  pData := NIL;
   
END_FUNCTION

#pragma warning(default:73);