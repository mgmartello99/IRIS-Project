#ifndef  __DISPLAY_H
#pragma once
#define  __DISPLAY_H

// Interface
#define INTERFACE_DISPLAY                       "LSL_DISPLAY"

#define LSL_DISPLAY_TYPE_VERSION	0x00010001

#define LSL_DISPLAY_ERR_INVALID_OPTION		-1		// Invalid Option

#define LSL_DISPLAY_MAX_VAL				100			// Max. Value for Brightness / Contrast / Saturation / Color Weights
#define LSL_DISPLAY_DEF_BRIGHT			50			// Default Value for Brightness
#define LSL_DISPLAY_DEF_CONTR			100			// Default Value for Contrast
#define LSL_DISPLAY_DEF_SATUR			50			// Default Value for Saturation
#define LSL_DISPLAY_MAX_HUE				360			// Max. Value for Hue
#define LSL_DISPLAY_DEF_HUE				0			// Default Value for Hue
#define LSL_DISPLAY_DEF_COL_WEIGHT		100			// Default Value for weights of Colors

TYPE
  display_option : (
    disp_brightness  := 0,
    disp_contrast,
    disp_saturation,
    disp_hue,
    disp_red_weight,
    disp_green_weight,
    disp_blue_weight
  ) $UDINT;
END_TYPE


TYPE
  #pragma pack (push, 1)
  LSL_DISPLAY_TYPE : STRUCT
    version	        	: UDINT;
    DISP_GetSetting  	: pVoid;
    DISP_SetSetting		: pVoid;
  END_STRUCT;
  #pragma pack (pop)
END_TYPE


FUNCTION __CDECL GLOBAL P_DISP_GetSetting
  VAR_INPUT
    option   : display_option;
    value    : ^DINT;
    flags    : UDINT;
  END_VAR
  VAR_OUTPUT
    retcode     : DINT;
  END_VAR;

FUNCTION __CDECL GLOBAL P_DISP_SetSetting
  VAR_INPUT
    option   : display_option;
    value    : DINT;
    flags    : UDINT;
  END_VAR
  VAR_OUTPUT
    retcode     : DINT;
  END_VAR;

#define OS_DISPLAY_CHECK_VERSION(pDISP)				pDISP^.version <> LSL_DISPLAY_TYPE_VERSION
#define OS_DISP_GetSetting(pDISP,p1,p2,p3)		pDISP^.DISP_GetSetting $ P_DISP_GetSetting(p1,p2,p3)
#define OS_DISP_SetSetting(pDISP,p1,p2,p3)		pDISP^.DISP_SetSetting $ P_DISP_SetSetting(p1,p2,p3)

#endif
