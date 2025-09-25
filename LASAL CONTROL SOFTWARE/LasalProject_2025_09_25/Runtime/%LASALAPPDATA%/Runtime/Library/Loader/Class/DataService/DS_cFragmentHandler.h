// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] frirob                                                                       |
// +-[        date ] 28.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_cFragmentHandlerH
  #define _DS_cFragmentHandlerH

  #include "DS_Deeds.h"


//  #ifndef MAX_PACKAGE_SIZE
//    #define MAX_PACKAGE_SIZE 0x08000000
//  #endif

//  #define FRAGMENTHEADER_OFFSET_ORIGINALSIZE   8
//  #define FRAGMENTHEADER_OFFSET_DATALENGTH    16
//  #define FRAGMENTHEADER_OFFSET_DATAOFFSET    20
//  #define FRAGMENTHEADER_SIZE                 28
  
//  #define MAX_ALLOCABLE_NUMBER                268435440
  
  #ifdef cCompile
  
    #include "DS_ComDef.h"
    
    class cComPort;
//
//    class Fragment
//    {
//      public:      
//        static Fragment* cTor(uint64 originalSize);
//        static Fragment* dTor(Fragment* f); 
//        
//        bool addData(uint64 offset, uint32 lenght, uint08* data);
//        bool checkTime(uint32 t); 
//        
//        DsComData* GetComData(); 
//      
//      private:
//        uint64 originalSize;
//        uint64 currentSize;
//        uint32 timestamp;
//        
//        DsComData* pcd; 
//        
//    };
//    
//    class FragmentListEntry
//    {
//     public:
//      static FragmentListEntry* cTor(uint32 id, uint16 srcId, uint64 originalSize, cComPort* port);
//      static FragmentListEntry* dTor(FragmentListEntry* f); 
//      
//      FragmentListEntry* get(uint32 id, uint16 src, cComPort* port);
//      
//      FragmentListEntry* getNext();
//      void setNext(FragmentListEntry* f); 
//      
//      bool checkTime(uint32 t);
//      void addData(uint64 offset, uint32 lenght, uint08* data, uint32 tabsolut);
//      
//     private:
//        uint16 srcId;
//        uint32 id;
//        cComPort* port; 
//        
//        FragmentListEntry* next;
//        Fragment* fragment; 
//    };
//
//    class FragmentHandler
//    {
//    public:
//      static FragmentHandler* cTor();
//      static FragmentHandler* dTor(FragmentHandler* fh);
//      
//      
//      bool addData(DsComData* pcd, cComPort* port, uint32 tabsolut);
//      void checkTime(uint32 t);
//      
//      
//    private:
//      FragmentListEntry* list; 
//    };
    
    
    
// Otto
   #pragma pack(push, 1)
    typedef struct
    {
      uint32 fragment_id;     // FragmentId
      uint32 nc;              // ?? 
      uint64 bytesize_gesamt; // Gesamtanzahl der Bytes
      uint32 bytesize_actual; // aktuelle Anzahl der Bytes
      uint64 offset_actual;   // Byteoffset der aktuellen Bytes
    } tFragmentHeader;
   #pragma pack(pop)

    typedef struct
    {
      uint08 *ptr;
      uint32 bytesize_alloc;
      uint32 bytesize_rest;
      uint32 fragment_id;
      uint32 timex;
    } tFragment;
    
    class cFragmentManager
    {
      private : 
        tFragment Data[4];    // Handler
        
        bool   AddSub(cComPort *pcp, tFragment *p, tFragmentHeader *pfh, uint32 tabsolute);
        uint32 CheckSub(tFragment *p, uint32 tabsolute);
        
      public :
        uint32    Usage;      // Indikator ob Handler in Verwendung
        
        void cTor(void);
        void dTor(void);
        bool Add(cComPort *pcp, DsComData* pcd, uint32 tabsolute);
        void Check(uint32 tabsolute);
    };
    
  #endif
#endif