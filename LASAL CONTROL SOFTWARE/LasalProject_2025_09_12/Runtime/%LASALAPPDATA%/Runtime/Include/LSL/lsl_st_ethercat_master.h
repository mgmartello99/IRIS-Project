

type
  sEthercatMaster : struct
    ud_Version : udint;
    piEthercatMasterInit : ^void;
    piEthercatRegisterClientCallback : ^void;
    piEthercatGetSlaveData : ^void;
    piEthercatSetMasterState : ^void;
    piEthercatReadProcessData : ^void;
    piEthercatWriteProcessData : ^void;
    piEthercatSendSDO :^void;
    piEthercatReceiveSDO : ^void;
    piResetApp : ^void;
    piGetLastError : ^void;
    piEthercatSetManagerError  : ^void;
    piEthercatGetManagerError  : ^void;
    piEthercatGetMasterState  : ^void;
    piEthercatGetSlaveState  : ^void;
    piEthercatCreateMailboxTransferObject  : ^void;
    piEthercatDeleteMailboxTransferObject  : ^void;
    piEthercatSendSDOReq  : ^void;
    piEthercatReceiveSDOReq  : ^void;
    piEthercatOsTaskBorder  : ^void;
    piEthercatTriggerManagerTask  : ^void;
    piEthercatSetSlaveState  : ^void;		// ab Interface-Version 1.3
    piEthercatFoeFileUpload  : ^void;
    piEthercatFoeFileDownload  : ^void;
    piEthercatReadSlaveEEPRom  : ^void;
    piEthercatWriteSlaveEEPRom  : ^void;
    piEthercatAssignSlaveEEPRom  : ^void;
    piEthercatActiveSlaveEEPRom  : ^void;
  end_struct;
end_type

//
// Defines
//

// Interface
#define INTERFACE_ETHERCATMANAGER                "IETHERCATMANAGER"

// Ergebniscodes
#define ETHERCATMANAGER_OK                        0
#define ETHERCATMANAGER_ERROR                    -1
#define ETHERCATMANAGER_INSTANCE_INVALID         -2
#define ETHERCATMANAGER_MODE_INVALID             -3
#define ETHERCATMANAGER_CANT_INIT_MASTER         -4
#define ETHERCATMANAGER_CANT_REGISTER_CLIENT     -5
#define ETHERCATMANAGER_CANT_SET_MASTER_STATE    -6
#define ETHERCATMANAGER_BUFFER_TOO_SMALL         -7
#define ETHERCATMANAGER_SLAVE_NOT_READY          -8
#define ETHERCATMANAGER_CANT_GET_PHYS_ADDR       -9
#define ETHERCATMANAGER_CANT_GET_IN_OFS          -10
#define ETHERCATMANAGER_CANT_GET_IN_LEN          -11
#define ETHERCATMANAGER_CANT_GET_OUT_OFS         -12
#define ETHERCATMANAGER_CANT_GET_OUT_LEN         -13
#define ETHERCATMANAGER_SEND_SDO_FAILED          -14
#define ETHERCATMANAGER_RECV_SDO_FAILED          -15
#define ETHERCATMANAGER_CANT_INIT_SYSTEM         -16
#define ETHERCATMANAGER_CANT_CONF_MAC_BC         -17
#define ETHERCATMANAGER_CANT_ENABLE_SB           -18
#define ETHERCATMANAGER_SB_FAILED                -19
#define ETHERCATMANAGER_CANT_ENABLE_DC           -20
#define ETHERCATMANAGER_CANT_GET_SLAVE_STATE     -21
#define ETHERCATMANAGER_SENDREQ_SDO_FAILED       -22
#define ETHERCATMANAGER_RECVREQ_SDO_FAILED       -23
#define ETHERCATMANAGER_CANT_SET_SLAVE_STATE     -24
#define ETHERCATMANAGER_CANT_FOE_UPLOAD_FILE     -25
#define ETHERCATMANAGER_CANT_FOE_DOWNLOAD_FILE   -26
#define ETHERCATMANAGER_CANT_READ_SLAVE_EEPROM   -27
#define ETHERCATMANAGER_CANT_WRITE_SLAVE_EEPROM  -28
#define ETHERCATMANAGER_CANT_ASSIGN_SLAVE_EEPROM -29
#define ETHERCATMANAGER_CANT_ACTIVE_SLAVE_EEPROM -30

// textual description of slave device states
#define DEVICE_STATE_INIT                   0x0001
#define DEVICE_STATE_PREOP                  0x0002
#define DEVICE_STATE_BOOTSTRAP              0x0003
#define DEVICE_STATE_SAFEOP                 0x0004
#define DEVICE_STATE_OP                     0x0008
#define DEVICE_STATE_ERROR                  0x0010


#define INTERFACE_ETHERCATMASTER  "IETHERCATMANAGER"

#define ETHERCATMASTER_NOMEMORY   -1
#define ETHERCATMASTER_NOCONFIG   -2
#define ETHERCATMASTER_CALLBACK   -3
#define ETHERCATMASTER_INTERFACE  -4
#define ETHERCATMASTER_PROCIMAGE  -5
#define ETHERCATMASTER_NOTONLINE  -99

#define ETHERCATMASTER_STARTTIMEOUT 10000
#define ETHERCATMASTER_STATE_INIT   1
#define ETHERCATMASTER_STATE_PREOP  2
#define ETHERCATMASTER_STATE_SAFEOP 4
#define ETHERCATMASTER_STATE_OP     8



  FUNCTION __CDECL P_iEthercatMasterInit
	VAR_INPUT
	  p1	:^CHAR;
    p2  : udint;
	END_VAR
  VAR_OUTPUT
    p3 : dint;
  END_VAR;

	#define iEthercatMasterInit(p1,p2) 					p_EthercatMaster^.piEthercatMasterInit $ P_iEthercatMasterInit(p1,p2)



	FUNCTION __CDECL P_iEthercatRegisterClientCallback
	VAR_INPUT
	  p1	:^void;
    p2  :^void; 
    p3	:^udint;
	  p4	:^dint;
    p5  :^dint; 
    p6  :^udint;
    p7  :^udint;
	END_VAR
  VAR_OUTPUT
    p8 : dint;
  END_VAR;

	#define iEthercatRegisterClientCallback(p1,p2,p3,p4,p5,p6,p7) 		p_EthercatMaster^.piEthercatRegisterClientCallback $ P_iEthercatRegisterClientCallback(p1,p2,p3,p4,p5,p6,p7)



	FUNCTION __CDECL P_iEthercatGetSlaveData
	VAR_INPUT
	  p1	:udint;
    p2  :^usint; 
    p3  :udint;
	END_VAR
  VAR_OUTPUT
    p4 : dint;
  END_VAR;

	#define iEthercatGetSlaveData(p1,p2,p3) 					p_EthercatMaster^.piEthercatGetSlaveData $ P_iEthercatGetSlaveData(p1,p2,p3)



	FUNCTION __CDECL P_iEthercatSetMasterState
	VAR_INPUT
	  p1	:udint;
    p2  :udint; 
	END_VAR
  VAR_OUTPUT
    p3 : dint;
  END_VAR;

	#define iEthercatSetMasterState(p1,p2) 					  p_EthercatMaster^.piEthercatSetMasterState $ P_iEthercatSetMasterState(p1,p2)



  FUNCTION __CDECL P_iEthercatReadProcessData;

	#define iEthercatReadProcessData() 					p_EthercatMaster^.piEthercatReadProcessData $ P_iEthercatReadProcessData()



  FUNCTION __CDECL P_iEthercatWriteProcessData;

	#define iEthercatWriteProcessData() 					p_EthercatMaster^.piEthercatWriteProcessData $ P_iEthercatWriteProcessData()
  
  
	FUNCTION __CDECL P_iEthercatSendSDO
	VAR_INPUT
	  p1	: udint;
    p2  : udint; 
    p3	: udint;
	  p4	: ^char;
    p5  : udint; 
    p6  : udint;
    p7  : udint;
	END_VAR
  VAR_OUTPUT
    p8 : dint;
  END_VAR;

	#define iEthercatSendSDO(p1,p2,p3,p4,p5,p6,p7) 		p_EthercatMaster^.piEthercatSendSDO $ P_iEthercatSendSDO(p1,p2,p3,p4,p5,p6,p7)
  

	FUNCTION __CDECL P_iEthercatReceiveSDO
	VAR_INPUT
	  p1	: udint;
    p2  : udint; 
    p3	: udint;
	  p4	: ^char;
    p5  : udint; 
    p6  : ^udint;
    p7  : udint;
    p8  : udint;
	END_VAR
  VAR_OUTPUT
    p9 : dint;
  END_VAR;

	#define iEthercatReceiveSDO(p1,p2,p3,p4,p5,p6,p7,p8) 		p_EthercatMaster^.piEthercatReceiveSDO $ P_iEthercatReceiveSDO(p1,p2,p3,p4,p5,p6,p7,p8)


  FUNCTION __CDECL P_iEthercatMasterGetLastError
  VAR_OUTPUT
    p1 : dint;
  END_VAR;

	#define iEthercatMasterGetLastError() 					p_EthercatMaster^.piGetLastError $ P_iEthercatMasterGetLastError()

    FUNCTION __CDECL P_iEthercatGetSlaveState
    VAR_INPUT
      udSlaveId        : udint;
      puiCurrDevState  : ^uint; 
      puiReqDevState   : ^uint;
      udTimeout        : udint;
    END_VAR
    VAR_OUTPUT
      retCode          : dint;
    END_VAR;

	#define iEthercatGetSlaveState(p1,p2,p3,p4) 		p_EthercatMaster^.piEthercatGetSlaveState $ P_iEthercatGetSlaveState(p1,p2,p3,p4)

    FUNCTION __CDECL P_iEthercatSetSlaveState
    VAR_INPUT
      udSlaveId        : udint;
      uiNewReqDevState : uint; 
      udTimeout        : udint;
    END_VAR
    VAR_OUTPUT
      retCode          : dint;
    END_VAR;

	#define iEthercatSetSlaveState(p1,p2,p3) 		p_EthercatMaster^.piEthercatSetSlaveState $ P_iEthercatSetSlaveState(p1,p2,p3)

    FUNCTION __CDECL P_iEthercatFoeFileUpload
    VAR_INPUT
      udSlaveId        : udint;
      pcFileName       : ^CHAR;
      udFileNameLen    : udint; 
      pucData          : ^UCHAR;
      udDataLen        : udint;
      pudOutDataLen    : ^udint;
      udPassWd         : udint;
      udTimeout        : udint;
    END_VAR
    VAR_OUTPUT
      retCode          : dint;
    END_VAR;

    #define iEthercatFoeFileUpload(p1,p2,p3,p4,p5,p6,p7,p8) 		p_EthercatMaster^.piEthercatFoeFileUpload $ P_iEthercatFoeFileUpload(p1,p2,p3,p4,p5,p6,p7,p8)

    FUNCTION __CDECL P_iEthercatFoeFileDownload
    VAR_INPUT
      udSlaveId        : udint;
      pcFileName       : ^CHAR;
      udFileNameLen    : udint; 
      pucData          : ^UCHAR;
      udDataLen        : udint;
      udPassWd         : udint;
      udTimeout        : udint;
    END_VAR
    VAR_OUTPUT
      retCode          : dint;
    END_VAR;

    #define iEthercatFoeFileDownload(p1,p2,p3,p4,p5,p6,p7) 		p_EthercatMaster^.piEthercatFoeFileDownload $ P_iEthercatFoeFileDownload(p1,p2,p3,p4,p5,p6,p7)

    FUNCTION __CDECL P_iEthercatReadSlaveEEPRom
    VAR_INPUT
      bFixedAddressing     : BOOL;
      uiSlaveAddress       : UINT;
      uiEEPromStartOffset  : UINT;
      puiReadData          : ^UINT;
      udReadLen            : UDINT;
      pudNumOutData        : ^UDINT;
      udTimeout            : UDINT;
    END_VAR
    VAR_OUTPUT
      retCode              : DINT;
    END_VAR;

    #define iEthercatReadSlaveEEPRom(p1,p2,p3,p4,p5,p6,p7) 		p_EthercatMaster^.piEthercatReadSlaveEEPRom $ P_iEthercatReadSlaveEEPRom(p1,p2,p3,p4,p5,p6,p7)

    FUNCTION __CDECL P_iEthercatWriteSlaveEEPRom
    VAR_INPUT
      bFixedAddressing     : BOOL;
      uiSlaveAddress       : UINT;
      uiEEPromStartOffset  : UINT;
      puiWriteData         : ^UINT;
      udWriteLen           : UDINT;
      udTimeout            : UDINT;
    END_VAR
    VAR_OUTPUT
      retCode              : DINT;
    END_VAR;

    #define iEthercatWriteSlaveEEPRom(p1,p2,p3,p4,p5,p6) 		p_EthercatMaster^.piEthercatWriteSlaveEEPRom $ P_iEthercatWriteSlaveEEPRom(p1,p2,p3,p4,p5,p6)

    FUNCTION __CDECL P_iEthercatAssignSlaveEEPRom
    VAR_INPUT
      bFixedAddressing      : BOOL;
      uiSlaveAddress        : UINT;
      bSlavePDIAccessEnable : BOOL;
      bForceAssign          : BOOL;
      udTimeout             : UDINT;
    END_VAR
    VAR_OUTPUT
      retCode               : DINT;
    END_VAR;

    #define iEthercatAssignSlaveEEPRom(p1,p2,p3,p4,p5) 		p_EthercatMaster^.piEthercatAssignSlaveEEPRom $ P_iEthercatAssignSlaveEEPRom(p1,p2,p3,p4,p5)

    FUNCTION __CDECL P_iEthercatActiveSlaveEEPRom
    VAR_INPUT
      bFixedAddressing       : BOOL;
      uiSlaveAddress         : UINT;
      pbSlavePDIAccessActive : ^BOOL;
      udTimeout              : UDINT;
    END_VAR
    VAR_OUTPUT
      retCode                : DINT;
    END_VAR;

    #define iEthercatActiveSlaveEEPRom(p1,p2,p3,p4) 		p_EthercatMaster^.piEthercatActiveSlaveEEPRom $ P_iEthercatActiveSlaveEEPRom(p1,p2,p3,p4)
