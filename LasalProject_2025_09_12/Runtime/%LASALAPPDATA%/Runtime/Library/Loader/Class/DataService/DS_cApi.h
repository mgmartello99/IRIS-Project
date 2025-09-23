// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 20.09.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_cApiH
  #define _DS_cApiH

  #include "DS_Deeds.h"

  #ifdef cCompile

    #include "DS_ComDef.h"
    #include "DS_cComPort.h"

    cExtern uint32 Api_CTor(void);

    class cApi
    {
      private:

      public:
        void Init(void);
        bool RegisterClient(cComPort *port);
        void UnRegisterClient(cComPort *port);
        void RecieveData(cComPort *port, DsComData *pcd);
//        bool SendData(DsComData *pcd);
//        bool SendDataEx(cComPort *port, DsComData *pcd);
        uint32 SendDataWithResponse(DsComData *pcd, uint32 timeout);
        void SpyData(cComPort *port, DsComData *pcd, uint32 in_out, uint32 stack_level);
        void LevelWatchdog(cComPort *port, uint32 level);

    };

    extern cApi Api;
  #endif

#endif
