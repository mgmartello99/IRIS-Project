// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 31.01.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _DS_DeedsH
 #define _DS_DeedsH

  #ifndef cCompile
    #ifdef _MSC_VER
      #define cCompile
    #endif
    #ifdef _GNUC
      #define cCompile
    #endif
  #endif

  #ifdef cCompile
    #ifndef cExtern
      #ifdef __cplusplus
        #define cExtern extern "C"
      #else
        #define cExtern extern 
      #endif
    #endif
  #endif

  #ifdef cCompile

    typedef unsigned char      uint08;
    typedef unsigned short     uint16;
    typedef unsigned long      uint32;
    typedef unsigned long long uint64;

    typedef char               int08;
    typedef short              int16;
    typedef long               int32;
    typedef long long          int64;

    typedef char               ascii;
    typedef unsigned short     uni;
    typedef float              f32;
    typedef double             f64;

    #ifndef NULL
      #define NULL 0
    #endif
    
    #ifndef UNUSED
      #define UNUSED(x) (void)(x)
    #endif
    
    #ifdef COMPILE_LINUX
      // linux (ubuntu oder salamander)
    #else
      // nicht linux (annahmem windows)
    #endif
    
  #endif

  #ifdef ARRAY_SIZE
   #undef ARRAY_SIZE
  #endif
  
  #define ARRAY_SIZE(p) (sizeof(p)/sizeof(p[0]))
  
#endif
