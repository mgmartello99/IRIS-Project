//HFILTER:1
#ifndef __LSL_OSZI
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define __LSL_OSZI


#define LSLAPI_OSZI_VERSION   0x0001

typedef struct
{
           unsigned long version;

           int (* LslOsziAddChannel)( unsigned int uiChannelType, unsigned int uiChannelValueType, void *pChannelDescriptor );
           int (* LslOsziReplaceChannel)( unsigned int uiChannel, unsigned int uiChannelType, unsigned int uiChannelValueType, void *pChannelDescriptor );
           int (* LslOsziRemoveAllChannels)();
           int (* LslOsziGetChannels)( unsigned int *puiChannels );
           int (* LslOsziGetStatus)( unsigned int *puiStatus );
           int (* LslOsziSetBufferSize)( unsigned int uiBufferSize );
           int (* LslOsziResetBuffer)( void );
           int (* LslOsziIsBufferEmpty)( unsigned int *pbEmpty );
           int (* LslOsziGetUsedBufferSize)( unsigned int *puiUsedBufferSize );
           int (* LslOsziGetUsedBuffer)( unsigned int uiMaxSize, void *pBuffer, unsigned int *puiLen  );
           int (* LslOsziStart)();
          int (* LslOsziStop)();
           int (* LslOsziSetSampleRate)( unsigned int uiSampleRateMuSec );
           int (* LslOsziGetSampleRate)( unsigned int *puiSampleRate );
           int (* LslOsziSetTrigger)( unsigned int uiTriggerType, unsigned int uiChannel, unsigned int uiTriggerCond, const void *pValue );
           int (* LslOsziClearTrigger)( unsigned int uiTriggerType );
           int (* LslOsziSetCRC)( unsigned int uiLasalCRC );
           int (* LslOsziGetCRC)( unsigned int *puiCRC );
           int (* LslOsziInit)();
} LSL_OSZI;

//HFILTER:1
#endif
//HFILTER:
