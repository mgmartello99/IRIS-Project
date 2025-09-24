// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 16.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_cRefreshListH
  #define _DS_cRefreshListH

  #include "DS_Deeds.h"

  #ifdef cCompile

    #include "DS_ComDef.h"


    //#include "DS_Library.h"
    //#include "DS_CVariableList.h""
    //#include "DS_cComPort.h"

    #define RefList_NOP 0

    #define RefList_ADD 1
    // [1] ..... no
    // *[4] .... lasalid
    // *[4] .... userid
    // *[4] .... reftime
    // *[4] .... datasize

    #define RefList_SUB 2
    // [1] ..... no
    // *[4] .... userid

    #define RefList_EMPTY 3
    #define RefList_STARTUPDATE 4
    #define RefList_STOPUPDATE  5

    typedef struct
    {
      uint32 oldval;
      uint32 lasalid;
      uint32 userid;
      uint32 reftime;
      uint32 datasize;
      uint32 lasttimex;
    } RefCell;

    typedef struct
    {
      uint08 cmd;
      uint32 userid;
      uint32 lasalid;
      uint32 reftime;
      uint32 datasize;
    } FiFoCell;

    #pragma pack(push, 1)
      typedef struct
      {
        uint32 userid;
        uint32 value;
      } UpdateCellU32;

      typedef struct
      {
        uint08 cmd;
        uint08 channel;
        uint16 no;
        uint32 data[64];
      } RefListCmdStream;

      typedef struct : DsComDataHdr
      {
        uint32        no;
        UpdateCellU32 cell[128];
      } Cell32Update;  

    #pragma pack(pop)


    void init_RefListCmdStream(RefListCmdStream *p);
    void free_RefListCmdStream(RefListCmdStream *p);

    uint32 LasalIdToVarNo(uint32 lasalid);
    uint32 VarNoToLasalId(uint32 varno);

    class cComPort;


    class cRefreshList
    {
      #define FiFoCellNo (sizeof(FiFoData) / sizeof(FiFoData[0])) // grösse des FiFo
      #define RefCellGrowBy        1024  // number of cells to grow
      #define RefCellsPerCycle       48  // number how many cells should be updated in method run

    private:
      cComPort *pComPort;            // pointer to instance of comport
      uint32 FiFoUsed;        // number of used cells in fifo
      uint32 FiFoRd;          // position of readpointer in fifo
      FiFoCell FiFoData[1024];       // fifo

      uint32 CellNoAllocated; // number of allocated cells in refreshlist
      uint32 CellNo;          // number of active cells in refreshlist
      uint32 CellIndexer;     // indexer of next cell in refreshlist
      RefCell *pCell;                // pointer to allicated cells
      uint32 StopCounter;     // lock to de/activate update
      uint16 Channel;

      uint32 Count;
      Cell32Update CellUpdBuffer;

      uint32 SeekCell(uint32 userid);
      bool DoCell(uint32 no);
      bool Add(uint32 lasalid, uint32 userid, uint32 reftime, uint32 datasize);
      bool Sub(uint32 userid);
      void Empty(void);
      void SectionBegin(void);
      void SectionEnd(void);
      void DoFiFo(void);
      FiFoCell *GetFifoCell(void);
      uint32 RunCell(RefCell *pcell);

    protected:

      bool ReadServer(uint32 *pval, uint32 lasalid);
      bool ReadStringCrc(uint32 *pval, uint32 lasalid);
      bool ReadDataCrc(uint32 *pval, uint32 lasalid, uint32 datasize);
      bool ReadClient(uint32 *pval, uint32 lasalid);
      void UpdateU32(DsComData *pcd, uint32 no);
      void UpdateStr(UserIdLasalId *p, uint32 no);

    public:
      static cRefreshList* New(cComPort *pcp, uint16 channel0);
      static cRefreshList* Delete(cRefreshList* p);
      void cTor(cComPort *pcp, uint16 channel0);
      void dTor(void);

      uint32 GetCellNo(void);

      bool AddCommand(RefListCmdStream *p);
      void Run(void);

      //  static uint32 LasalIdToVarNo(uint32 lasalid);
      //  static uint32 VarNoToLasalId(uint32 varno);

    };

    #define MAX_CHANNEL 6 // anzahl der verfügbaren channels bei refreshlist

    class cRefreshListEx
    {
    private:
      cComPort     *pComPort;
      cRefreshList *Ptr[MAX_CHANNEL];

    public:
      static cRefreshListEx* New(cComPort *pcp);
      static cRefreshListEx* Delete(cRefreshListEx* p);
      void cTor(cComPort *pcp);
      void dTor(void);

      bool AddCommand(RefListCmdStream *p);
      void Run(void);
      uint32 GetCellNo(uint32 channel);
      uint32 GetCellInfo(ChannelNumber *pcn, uint32 maxno, uint32 channel);
    
      // static uint32 LasalIdToVarNo(uint32 lasalid);
      // static uint32 VarNoToLasalId(uint32 varno);

    };

  #endif

#endif