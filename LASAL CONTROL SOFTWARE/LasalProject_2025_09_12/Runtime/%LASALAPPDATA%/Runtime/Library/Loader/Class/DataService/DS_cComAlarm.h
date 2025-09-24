// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 23.10.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_cComAlarmH
#define _DS_cComAlarmH

  #include "DS_Deeds.h"

  #ifdef cCompile

    #include "DS_ComDef.h"
  
    class cComPort;
  
    class cComAlarm
    {
      private:
        cComPort *pComPort;            // pointer to instance of comport
        uint32   ObjLasalId;
        uint32   ApplicationId;
        uint32   Step;
        uint32   AlarmHandle;
        uint32   ConfigIndex;
        uint32   AlarmNo;
        uint32   TimexRunAlarm;
        uint32   AlarmRecordSize;      // Bytesize eines Alarmrecords
        uint32   ScanStartIndex;       // Index zum Abscannen der einzelnen Alarme
        uint32   ScanAlarmState;       // Status zum Abscannen der einzelnen Alarme (0,1)
        uint32   ScanAlarmAddState;    // AddState Abscannen der einzelnen Alarme (0,1,2)
        uint32   AlarmXBufferVersion;  // Version der AlarmXBuffer Klasse
        uint08   Result10K[10120];     // arbitrary resultbuffer for newinst
        DsComData10K ComData10K;       // arbitrary ComDataBuffer for newinst

        bool   Send(DsComData *pcd);
    
        uint32 GetVersion(void); // ermittle versionsnummer von _AlarmXBuffer Klasse
        uint32 GetNumberOfAlarm(void); // ermittle anzahl der Alarme in objekt von _AlarmXBuffer klasse
    
        bool   SendStartResult(uint16 transactionDest, uint32 messageId, uint32 errorcode, uint32 version, uint32 buffersize);
        bool   SendConfig(uint16 transactionDest, uint32 messageId, uint32 alarmlasalid, uint32 config, uint32 group, uint32 nopara, uint32 *para);
        bool   RunConfig(void);

        bool   SendAlarm202(uint16 transactionDest, uint32 messageId, uint16 minorId, uint32 startidx, uint32 no, uint08 *pa);
        bool   RunAlarmSub202(uint32 no);
        bool   SendAlarm203(uint16 transactionDest, uint32 messageId, uint16 minorId, uint32 startidx, uint32 no, uint08 *pa);
        bool   RunAlarmSub203(uint32 no); // erst ab AlarmXBuffer Version 5
        bool   RunAlarm(uint32 tabsolute);

      public:
        static cComAlarm* New(cComPort *pcp);
        static cComAlarm* Delete(cComAlarm *p);

        void cTor(cComPort *pcp);
        void dTor(void);

        void Run(uint32 tabsolute);
        bool Start(uint16 transactionDest, uint32 messageId, uint32 application_id, const ascii *label);
        bool Stop(uint16 transactionDest, uint32 messageId);
        bool QuitAlarm(uint16 transactionDest, uint32 messageId, uint32 no, uint32 *palaid);
        bool QuitAlarmAll(uint16 transactionDest, uint32 messageId);
        bool QuitAlarmGroup(uint16 transactionDest, uint32 messageId, uint32 group);
        bool ScanAlarmStateStart(uint32 alarmstate, uint32 addstate);
        uint32 ScanAlarmStateRun(uint16 transactionDest, uint32 messageId, uint32 startindex, uint32 alarmstate, uint32 addstate);
        void Sleep(void);
        uint32 GetAlarmHandle(void);
    
    };
    
  #endif
  
#endif

