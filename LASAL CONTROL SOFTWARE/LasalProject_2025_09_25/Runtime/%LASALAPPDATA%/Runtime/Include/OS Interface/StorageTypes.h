#pragma once
#define _STORAGE_H

#pragma pack(push, 4)
  TYPE
         LSLPROMINFO_TYPE       : STRUCT
           diasstation  : UDINT;
           flashcode    : UDINT;
           flashlen     : UDINT;
           blocklen     : UDINT;
           segmentlen   : UDINT;
         END_STRUCT;
  END_TYPE
#pragma pack(pop)
