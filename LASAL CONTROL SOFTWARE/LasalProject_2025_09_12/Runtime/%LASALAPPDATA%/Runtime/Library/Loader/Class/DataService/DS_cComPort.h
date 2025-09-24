// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 28.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_cComPortH
  #define _DS_cComPortH

  #include "DS_Deeds.h"

  #ifdef cCompile

    #include "DS_ComDef.h"
    #include "DS_cRefreshList.h"
    #include "DS_cComAlarm.h"
    #include "DS_cFragmentHandler.h"
    
    class cComPort
    {
      private:
        char   UserText[32];
        cRefreshListEx RefreshListEx;
        cComAlarm ComAlarm;
        int32  Socket;
        uint08 Data[MaximumSizeOfComPackage * 2];
        uint32 DataSize;
        uint32 LastSendTime;
        uint32 LastSendAliveTrgTime;
        uint32 AliveTimex;
        uint32 StartTimex;
        uint32 ClientId;
        uint32 MagicNumber;
        uint32 CpuState;
        uint32 FragmentId;
        uint32 DataStackLevel;
        uint32 IsRegistered; // Api::RegisterClient wurde aufgerufen
//        FragmentHandler* pFragmentHandler;
        cFragmentManager FragmentMgr;
      
      
      
        DsComData* ComDataBufferCreate(DsComData *pstat, uint32 expected_size);
        DsComData* ComDataBufferDestroy(DsComData *pstat, DsComData *psendbuffer);

        void DoUserText(const char *usertext);
        
        bool ComCmdWriteToSysVariable(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdValue *p);
        bool ComCmdReadFromSysVariable(uint16 transactionDest, uint32 messageId, uint32 no, uint32 *pvarno);
        bool ComCmdGetLasalId(uint16 transactionDest, uint32 messageId, uint32 no, uint08 *data);
        bool ComCmdReadFromServer(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalId *p);
        bool ComCmdWriteToServer(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdValue *p);
        bool ComCmdWriteToServerPart(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdValueOffWid *p);
        bool ComCmdReadFromClient(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalId *p);
        bool ComCmdWriteToClient(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdValue *p);
        bool ComCmdReadFromDataAt(uint16 transactionDest, uint32 messageId, uint32 no, UserIdLasalIdDataSize *p);
        bool ComCmdWriteToDataAt(uint16 transactionDest, uint32 messageId, uint32 no, uint08 *p);
        bool ComCmdWriteToDataAtPart(uint16 transactionDest, uint32 messageId, uint32 no, uint08 *p);
      
        bool ComCmdReadFromString(uint16 transactionDest, uint32 messageId, uint16 cmd, uint32 no, UserIdLasalId *p);
        bool ComCmdWriteToString(uint16 transactionDest, uint32 messageId, uint32 no, uint08 *p);
      
        bool ComCmdSignIn(uint16 cmd, uint16 transactionDest, uint32 messageId, uint32 constant_or_magic_id, uint32 client_id, uint32 client_station, uint32 client_version, const char *usertext);
        bool ComCmdSignOut(uint16 transactionDest, uint32 messageId);
        bool ComCmdNewInst(uint16 transactionDest, uint32 messageId, uint32 userid, uint08 *data, uint32 datalength);

        bool ComCmdRefListAdd(uint16 transactionDest, uint32 messageId, uint32 channel, uint32 no, RefListAddCell *p);
        bool ComCmdRefListSub(uint16 transactionDest, uint32 messageId, uint32 channel, uint32 no, uint32 *pdat);
        bool ComCmdRefListEmpty(uint16 transactionDest, uint32 messageId, uint32 no, uint16 *pchannel);
        bool ComCmdRefListStartUpdate(uint16 transactionDest, uint32 messageId, uint32 no, uint16 *pchannel);
        bool ComCmdRefListStopUpdate(uint16 transactionDest, uint32 messageId, uint32 no, uint16 *pchannel);
        bool ComCmdRefListGetInfo(uint16 transactionDest, uint32 messageId, uint32 channel);
        bool ComCmdLoop(DsComData *pcd);
//        bool ComCmdSendFragment(uint16 transactionDest, uint32 messageId, uint32 uid, DsComData* pcd, uint32 tabsolut);

        bool SendComCmdRefListInfo(uint16 transactionDest, uint32 messageId, uint32 channel);
        
        bool ComCmdUserEventResponse(uint32 uid, uint08 state);
      
        bool SendComCmdReadFromDataAt_Result(uint16 transactionDest, uint32 messageId, uint32 userid, uint32 lasalid, uint32 datalen);
        bool SendComCmdSignIn_Result(uint16 cmd, uint16 transactionDest, uint32 messageId, uint32 magic_no, uint32 client_id, uint32 client_station, bool result);
        bool SendComCmdAliveTrigger(void);
        bool SendComCmdCpuState(uint32 cpustate);
        

      public:
        static cComPort* New(int32 sock);
        static cComPort* Delete(cComPort *p);
        
        void cTor(int32 sock);
        void dTor(void);
        bool Run(void);
        bool Send(DsComData *pcd);
        bool SendFragmented(DsComData* pcd);
        bool SendComCmdRefListUpdate(DsComData *pcd, uint32 no, uint16 channel);
        bool SendComCmdRefListUpdateString(UserIdLasalId *p, uint32 no, uint16 channel);
        bool SendComCmdRefListInfo_Result(uint16 transactionDest, uint32 messageId, ChannelNumber *pcn, uint32 no);
        
        bool DataReciever(DsComData *pcd, uint32 tabsolute);
   
        bool SetSysVar(uint32 value, uint32 varno);
        bool GetSysVar(uint32 *pvalue, uint32 varno);
        uint32 GetClientId(void);
        uint32 GetAlarmHandle(void);
        char *GetUserText(void);
        
    };
  #endif
#endif
