// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 08.05.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_cComDatBufferH
  #define _DS_cComDatBufferH

  #include "DS_Deeds.h"

  #ifdef cCompile

    #include "DS_ComDef.h"

    class cComPort;

    class cComDatBuffer 
    {
    private:
      cComPort *PCp;
      DsComData  ComDat;

      void Init(cComPort *pcp, uint16 transactionDest, uint32 messageId, uint16 cmd);

    public:
      //  cComDatBuffer(cComPort *pcp, uint32 id_transaction, uint16 cmd);
      void cTor(cComPort *pcp, uint16 transactionDest, uint32 messageId, uint16 cmd); // für lasal-class

      uint32 GetBytesFree(void);
      void UserCntInc(void);
      bool Finalize(void);
      bool AddU32Record(uint32 u32);
      bool AddU32U32Record(uint32 u32_0, uint32 u32_1);
      bool AddU32U32U32Record(uint32 u32_0, uint32 u32_1, uint32 u32_2);
      bool AddU32U32U32U32Record(uint32 u32_0, uint32 u32_1, uint32 u32_2, uint32 u32_3);
      bool AddU32DataU08Record(uint32 userid, uint32 datalen, uint08 u08, uint08 *data);
      bool AddU32U32U08Record(uint32 u32_0, uint32 u32_1, uint08 u08_2);
      bool AddAsciiRecord(uint32 userid, const ascii *text);
      bool AddStrRecord(uint32 userid, uint32 chrsize, void *text, uint16 channel);
      bool AddStrStateRecord(uint32 userid, uint32 chrsize, void *text, bool state);
    };
    
  #endif
#endif

