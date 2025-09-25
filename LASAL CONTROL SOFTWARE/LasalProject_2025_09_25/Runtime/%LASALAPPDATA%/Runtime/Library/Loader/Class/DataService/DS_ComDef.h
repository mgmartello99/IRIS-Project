// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 13.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_cComDefH
#define _DS_cComDefH

  #include "DS_Deeds.h"

  #define COMDEF_VERSION                   0x00000200

  #define ID_TRANSACTION_ALL_STATIONS      0x0100
  #define ID_TRANSACTION_DS                0x01FF
  #define ID_TRANSACTION_ALL_LOCAL_CLIENTS 0x0400
  #define ID_TRANSACTION_ALL_OTHER_CLIENTS 0x4000
  #define ID_TRANSACTION_ALL_CLIENTS       0x8000
  #define ID_TRANSACTION_ALL               0xFFFF

  #ifdef cCompile

    #pragma pack(push, 1)

      typedef struct
      {
        uint32 datalength;
        uint16 id_major;
        uint16 id_minor;
        uint16 transactionSrc;
        uint16 transactionDest;
        uint32 messageId;
      } DsComDataHdr;

      typedef struct : DsComDataHdr
      {
        //uint32 datalength;
        //uint16 id_major;
        //uint16 id_minor;
        //uint16 id_transactionSrc;
        //uint16 id_transectionDest;
        //uint32 messageId;
        uint08 data[1024];
      } DsComData;
      
      typedef struct : DsComDataHdr
      {
        //uint32 datalength;
        //uint16 id_major;
        //uint16 id_minor;
        //uint16 id_transactionSrc;
        //uint16 id_transectionDest;
        //uint32 messageId;
        uint08 data[12000];
      } DsComData10K;
      
      #define ComDataHeaderSize  sizeof(DsComDataHdr) // headergrösse von sendepaket in bytes

      typedef struct
      {
        uint32 userid;
        uint32 lasalid;
        uint32 reftime;
        uint32 datasize;
      } RefListAddCell;

      typedef struct
      {
        uint32 userid;
        uint32 lasalid;
        uint32 value;
      } UserIdLasalIdValue;

      typedef struct
      {
        uint32 userid;
        uint32 lasalid;
        uint32 value;
        uint16 offset;
        uint16 width;
      } UserIdLasalIdValueOffWid;

      typedef struct
      {
        uint32 userid;
        uint32 lasalid;
        uint32 size;
      } UserIdLasalIdDataSize;

      typedef struct
      {
        uint32 userid;
        uint32 value;
      } UserIdValue;

      typedef struct
      {
        uint32 userid;
        uint32 value;
        uint08 state;
      } UserIdValueState;

      typedef struct
      {
        uint32 userid;
        uint32 lasalid;
      } UserIdLasalId;

      typedef struct
      {
        uint16 channel;
        uint16 no;
      } ChannelNumber;

    #pragma pack(pop)
  
  #else
  
    type
      #pragma pack (push, 1)
      
        DsComDataHdr : struct
          datalength      : udint;
          id_major        : uint;
          id_minor        : uint;
          transactionSrc  : uint;
          transactionDest : uint;
          messageId       : udint;
        end_struct;
      
        DsComData : struct
          datalength      : udint;
          id_major        : uint;
          id_minor        : uint;
          transactionSrc  : uint;
          transactionDest : uint;
          messageId       : udint;
          data            : array[0..1023] of usint;
        end_struct;
      
      #pragma pack (pop)
      
    end_type
    
    #define ComDataHeaderSize 16 // headergrösse von sendepaket in bytes
  
  #endif

  #define MaximumSizeOfComPackage           0x4000 // weil 17520 Byte ist der Defaultwert bei windows, somit Grenze auf 16kbyte
  #define MaximumFragmentSize               0x2000
  #define StandardTimeOut_ClientNotify        5000 // Client hat 5 sekunden um sich zu authentifizieren
  #define StandardTimeOut_AliveTrigger        6000 // nach 6 sekunden ohne kommunikation wird Socket geschlossen
  #define StandardTimeOut_SendAliveTrigger    1237 // es wird alle 1237 millisekunden ein alivetrigger gesendet
  #define StandardTimeout_ms                  3000

  #define ComId_Major_DS                    0xFF00 // user:0x0000-0x7FFF, appli:0x8000-0xEFFF reserved:0xF000-0xFFFF
  #define ComId_Major_NoSend                0xFFFF // pakete mit dieser Majorid werden nicht versendet, sendefunktion retouniert aber true
  #define ComId_SignInIdStandard        0x2D3C4B5A
  #define ComId_Port                          2402

  #define ComCmd_NOP                        0x0000
  #define ComCmd_CpuState                   0x0002
  #define ComCmd_SignIn                     0x0003
  #define ComCmd_SignInResult               0x0004
  #define ComCmd_SignInEx                   0x0005
  #define ComCmd_SignInExResult             0x0006
  // 0x0007, 0x0008 reserved
  //#define ComCmd_SignOutResult              0x0009
  #define ComCmd_AliveTrigger               0x000A
  #define ComCmd_SignOut                    0x000B


  #define ComCmd_GetLasalId                 0x0012
  #define ComCmd_GetLasalId_Result          0x0013
  #define ComCmd_RefList_ADD                0x0014
  #define ComCmd_RefList_SUB                0x0015
  #define ComCmd_RefList_EMPTY              0x0016
  #define ComCmd_RefList_STARTUPDATE        0x0017
  #define ComCmd_RefList_STOPUPDATE         0x0018
  #define ComCmd_RefList_UPDATE             0x0019
  #define ComCmd_RefList_UPDATE_STRING      0x001A
  #define ComCmd_RefList_INFO               0x001B
  #define ComCmd_RefList_INFO_Result        0x001C
  
  #define ComCmd_UserEventResponse          0x0020
  
  #define ComCmd_SendFragment               0x0031
  #define ComCmd_SendFragment_Rsp           0x0032

  #define ComCmd_NewInst                    0x0060
  #define ComCmd_NewInst_Result             0x0061
  #define ComCmd_Loop                       0x0062
  #define ComCmd_Loop_Result                0x0063
  #define ComCmd_ReadFromServer             0x0064
  #define ComCmd_ReadFromServer_Result      0x0065
  #define ComCmd_ReadFromString             0x0066
  #define ComCmd_ReadFromString_Result      0x0067
  #define ComCmd_ReadFromDataAt             0x0068
  #define ComCmd_ReadFromDataAt_Result      0x0069
  #define ComCmd_ReadFromClient             0x006A
  #define ComCmd_ReadFromClient_Result      0x006B

  #define ComCmd_WriteToServer              0x006C
  #define ComCmd_WriteToServer_Result       0x006D
  #define ComCmd_WriteToString              0x006E
  #define ComCmd_WriteToString_Result       0x006F
  #define ComCmd_WriteToDataAt              0x0070
  #define ComCmd_WriteToDataAt_Result       0x0071
  #define ComCmd_WriteToClient              0x0072
  #define ComCmd_WriteToClient_Result       0x0073
  #define ComCmd_WriteToServerPart          0x0074
  #define ComCmd_WriteToDataAtPart          0x0075

  #define ComCmd_ReadFromSysVariable        0x0080
  #define ComCmd_ReadFromSysVariable_Result 0x0081
  #define ComCmd_WriteToSysVariable         0x0082
  #define ComCmd_WriteToSysVariable_Result  0x0083

  #define ComCmd_AlarmStart                 0x00A0
  #define ComCmd_AlarmStart_Result          0x00A1
  #define ComCmd_AlarmStop                  0x00A2
  #define ComCmd_AlarmStop_Result           0x00A3
  #define ComCmd_AlarmQuit                  0x00A4
  #define ComCmd_AlarmQuit_Result           0x00A5
  #define ComCmd_AlarmQuitAll               0x00A6
  #define ComCmd_AlarmQuitAll_Result        0x00A7
  #define ComCmd_AlarmQuitGroup             0x00A8
  #define ComCmd_AlarmQuitGroup_Result      0x00A9
  #define ComCmd_AlarmConfig                0x00AA
  #define ComCmd_AlarmRecord                0x00AB
  #define ComCmd_AlarmRecordEx              0x00AC
  #define ComCmd_AlarmScanRecordEx          0x00AD

  // variablennummern für ComCmd_GetSysVariable, ComCmd_GetVariableResult
  #define ComVarNo_SysVariable         0xFFFE0000UL // beginn der systemvariablen wobei FF00 die stationsnummer und 00FF die variablennummer beinhalten muss
  #define ComVarNo_SysDate             0xFFFE0000UL
  #define ComVarNo_SysTime             0xFFFE0001UL
  #define ComVarNo_SysLoaderVersion    0xFFFE0002UL
  #define ComVarNo_SysDscCrc           0xFFFE0003UL
  #define ComVarNo_SysCpuState         0xFFFE0004UL
  #define ComVarNo_SysCpuLoad          0xFFFE0005UL
  #define ComVarNo_SysOpsysVersion     0xFFFE0006UL  // Betriebssystemversion
  #define ComVarNo_SysInfo             0xFFFE0007UL  // Lars, Salamander, Gecko, x86, Linux
    #define ComVarNo_SysInfo_Bit_x86         0x0001  // Antwortbit ob eine x86 Architektur vorliegt
    #define ComVarNo_SysInfo_Bit_Lars        0x0002  // Antwortbit ob ein LARS-System vorliegt
    #define ComVarNo_SysInfo_Bit_Linux       0x0004  // Antwortbit ob es ein  Linux-basierendes System ist
    #define ComVarNo_SysInfo_Bit_Salamander  0x0008  // Antwortbit ob es ein SalamanderOpSys ist
    #define ComVarNo_SysInfo_Bit_Gecko       0x0010  // Antwortbit ob es ein GeckoOpSys ist
  #define ComVarNo_Default             0xFFFFFFFFUL

  // bitmask für reftime
  #define RefTimeId_ServerNum          0x00000000
  #define RefTimeId_ServerString       0x01000000
  #define RefTimeId_DataGlobal         0x02000000
  #define RefTimeId_ClientNum          0x03000000
  #define RefTimeId_SysVariable        0xFF000000

  // errorstate
  #define DS_ERROR_NONE                    0x0000 // kein fehler aufgetreten
  #define DS_ERROR_UNKNOWN                 0x0001 // unbekannter fehler
  #define DS_ERROR_INVALID_VARNO           0x0002 // ungültige variablennummer 
  #define DS_ERROR_INVALID_LASALID         0x0003 // keine lasalid
  #define DS_ERROR_INVALID_VARTYPE         0x0004 // ungültiger variablentyp
  #define DS_ERROR_ACCESS_DENIED           0x0005 // ACCESS_DENIED in writemethode 
  #define DS_ERROR_INVALID_LIMIT           0x0006 // wert zu gross oder zu klein
  #define DS_ERROR_COMMUNICATION           0x0007 // kommunikationsfehler
  #define DS_ERROR_NOT_IN_REFLIST          0x0008 // variable nicht in reflist
  #define DS_ERROR_INVALID_CHANNEL         0x0009 // ungültiger channel
  
  #define DS_ERROR_MEMORY                  0x0019 // memory allocation failed

  #define DS_ERROR_INVALID_ALARMID         0x0030 // alarmid ungültig

#endif