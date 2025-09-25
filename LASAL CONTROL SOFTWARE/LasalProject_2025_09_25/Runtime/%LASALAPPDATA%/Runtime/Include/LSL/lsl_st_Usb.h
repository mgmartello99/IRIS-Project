//****************************************************************************************/
//*                                                                                      */
//* UsbInit.h                                                                            */
//*                                                                                      */
//* enthält die Definitionen für den USB Bus                                             */
//*                                                                                      */
//* HH                                                                                   */
//*                                                                                      */
//*                                                                                      */
//****************************************************************************************/

#ifndef _SYSUSBCONF_H
#pragma once
#define _SYSUSBCONF_H

//
// DEFINES
//

// HC typen
#define  HC_TYPE_NONE  -1
#define  HC_TYPE_UHCI   0
#define  HC_TYPE_OHCI   1
#define  HC_TYPE_EHCI   2



// Max Anzahl der Einträge - ident mit RTUSB
#define  MAX_HC_DATA    8

// Max Anzahl der Device Einträge
#define  MAX_DEV_DATA  50


// Max. Länge eines StringDeskriptors
#define STRING_SIZE 128 

// bevorzugte Sprache - 1033 == English; 1031 == German; 1036 == French, etc.
#define DEF_LANGUAGAE = 1033; 


// vendorspezifische
#define USB_VENDOR_SPECIFIC_CLASS     0xff
#define USB_VENDOR_SPECIFIC_SUBCLASS  0xff
#define USB_VENDOR_SPECIFIC_PROTOCOL  0xff

// Pipe
#define USB_PIPE_DIR_SETUP            0x00
#define USB_PIPE_DIR_OUTPUT           0x01
#define USB_PIPE_DIR_INPUT            0x02
#define USB_PIPE_TYPE_CONTROL         0x00
#define USB_PIPE_TYPE_ISO             0x01
#define USB_PIPE_TYPE_BULK            0x02
#define USB_PIPE_TYPE_INT             0x03



//
// STRUKTUREN
//

#pragma pack(push, 1)

// Pseudo - vielleicht mal später
TYPE
	PDEVDATA	: pVoid;
END_TYPE

// Pseudo - vielleicht mal später
TYPE
	PINTDATA	: pVoid;
END_TYPE

// Pseudo - vielleicht mal später
TYPE
	PCONFDATA	: pVoid;
END_TYPE

// Pseudo - vielleicht mal später
TYPE
	PPIPEDATA	: pVoid;
END_TYPE



// HC Daten
TYPE
	HCDATA      : STRUCT
	
	// Vendor ID
	VendorID	: UINT;
	
	// Device ID
	DeviceID	: UINT;
	
	// type
	iType		: DINT;
	
	// bus
	Bus		: USINT;
	
	// DevType
	DevFunc		: USINT;
	
	// PhysAddr
	PhysAddr	: pVoid;
	
	// VirtAddr
	VirtAddr	: pVoid;
	
	// DevType
	Irq		: USINT;
	
	END_STRUCT;
	
	PHCDATA		: ^HCDATA;
END_TYPE

	

//
// USB Strukturen - diese sind allgmein und werden hier definiert, um von rtusb.h
// unabhängig zu sein
//
TYPE
	// Device Descriptor
	DEVDESC		: STRUCT
	
	bLength			: USINT;	// length in bytes of this descriptor
	bDescriptorType		: USINT;	// descriptor ID (RTU_DEVICE_DESCRIPTOR = 1)
   	bcdUSB			: UINT;		// USB version (BCD format)
   	bDeviceClass		: USINT;	// the device's class
   	bDeviceSubClass		: USINT;	// and subclass
   	bDeviceProtocol		: USINT;	// and protocol
   	bMaxPacketSize0		: USINT;	// default control pipe max packet len
   	idVendor		: UINT;		// vendor ID
   	idProduct		: UINT;		// product ID
   	bcdDevice		: UINT;		// device's version (BCD format)
   	iManufacturer		: USINT;	// index of vendor's string descriptor
   	iProduct		: USINT;	// index of product's string descriptor
   	iSerialNumber		: USINT;	// index of product's serial number
   	bNumConfigurations	: USINT;	// number of available configurations	
	
	END_STRUCT;
	
	PDEVDESC	: ^DEVDESC;

	// Configuration Descriptor
	CONFDESC		: STRUCT
	
	bLength			: USINT;	// length in bytes of this descriptor
   	bDescriptorType		: USINT;	// descriptor ID (RTU_CONFIGURATION_DESCRIPTOR = 2)
   	wTotalLength		: UINT;		// total length of all descriptors in this configuration
   	bNumInterfaces		: USINT;	// number of interfaces in this configuration
   	bConfigurationValue	: USINT;	// value to use for RTUSetConfiguration
   	iConfiguration		: USINT;	// index of string descriptor of this configuration
   	bmAttributes		: USINT;	// flags
   	bMaxPower		: USINT;	// power requirements in 2 mA units
   	
   	END_STRUCT;
   	
   	PCONFDESC	: ^CONFDESC;
   
	// Interface Deskriptor
	INTDESC		: STRUCT
	
    bLength			: USINT;	// length in bytes of this descriptor
   	bDescriptorType		: USINT;	// descriptor ID (RTU_INTERFACE_DESCRIPTOR = 4)
   	bInterfaceNumber	: USINT;	// number denoting this interface
   	bAlternateSetting	: USINT;	// if != 0, an alternate setting
   	bNumEndpoints		: USINT;	// number of enpoints in this interface
   	bInterfaceClass		: USINT;	// USB class
   	bInterfaceSubClass	: USINT;	// subclass (class specific)
   	bInterfaceProtocol	: USINT;	// protocol (class specific)
   	iInterface		: USINT;	// index of string descriptor of this interface
   	
   	END_STRUCT;
   	
   	PINTDESC	: ^INTDESC;
   	

	// Endpoint Deskriptor
	ENDPDESC		: STRUCT
	
	bLength			: USINT;	// length in bytes of this descriptor
   	bDescriptorType		: USINT;	// descriptor ID (RTU_ENDPOINT_DESCRIPTOR = 5)
   	bEndPointAddress	: USINT;	// address and direction
   	bmAttributes		: USINT;	// transfer, synch, and usage type
   	wMaxPacketSize		: UINT;		// max packet size and transactions per microframe
   	bInterval		: USINT;	// polling interval in [micro]frames
   	
   	END_STRUCT;
   	
   	PENDPDESC	: ^ENDPDESC;

END_TYPE


// Interface
TYPE

  pSINT       : ^SINT;

	IUSBCONFIG		: STRUCT

	//
	// Version
	//
	ulVersion		: UDINT;

	//
	// Daten
	//
	aHCNames		: ^pSINT;
	aSpeed			: ^pSINT;
	aEPType			: ^pSINT;
	aEPDir			: ^pSINT;

	//
	// Funktionen
	//

	// Anzahl der HC
	iUSBGetHCCount		: pVoid;

	// Daten eines HC
	iUSBGetHCDataByIndex	: pVoid;

	// Anzahl der Devs
	iUSBGetDevCount		: pVoid;

	// Daten eines Devs
	iUSBGetDevDataByIndex	: pVoid;

	// String
	iUSBGetString		: pVoid;

	// Speed
	iUSBGetSpeed		: pVoid;

	// Device Desc
	iUSBGetDevDesc		: pVoid;

	// Conf Desc
	iUSBGetConfDesc		: pVoid;

	// ExtrasDescs
	iUSBGetExtraDescs	: pVoid;

	// Anzahl der Interfaces
	iUSBGetInterfaceCount	: pVoid;

	// Interface Desc
	iUSBGetIntDesc		: pVoid;

	// Endpoint Descr
	iUSBGetEndpointDesc	: pVoid;

	// Interface handle
	iUSBFindInterface	: pVoid;

	// Pipe handle
	iUSBFindPipe		: pVoid;
	
	// Execute IO transfer
	iUSBExecuteIO		: pVoid;

  // Reset IO transfer
  iUSBResetIO : pVoid;


	//
	// Maus
	//
   
	// Position setzen
	iUSBMousePos		: pVoid;
	
	// Config
	USBMouseConfig		: pVoid;

	//	
	// Touch
	//

	// Config
	iUSBTouchConfig		: pVoid;

	//
	// Maus
	//
   
	// Position setzen
	iUSBMousePosWithoutEvent : pVoid;
			
	// LasalOS 01.02.229
	iUSBSendTestUnitReadyToUSBStick   : pVoid;
	iUSBResetTestUnitUSBStick         : pVoid;
  
	// ab Interface-Version 2.0.002

	// Execute Control Transfer
	iUSBExecuteControl : pVoid;
  
	// ab Interface-Version 2.0.003

	// Detach Driver 
	iUSBDetachDriver : pVoid;
  USBAttachDriver : pVoid;
  USBExecuteIOflag : pVoid;

	// ab Interface-Version 2.0.004
  iUSBResetDeviceOfPipe : pVoid;


	END_STRUCT;
	
	PIUSBCONFIG	: ^IUSBCONFIG;
END_TYPE




// Anzahl der HC
FUNCTION __CDECL GLOBAL iUSBGetHCCount
VAR_OUTPUT
	retval  : DINT;
END_VAR;
#define USB_iUSBGetHCCount()  	pUSB^.iUSBGetHCCount $ iUSBGetHCCount()


// Daten eines HC
FUNCTION __CDECL GLOBAL iUSBGetHCDataByIndex
VAR_INPUT
	iHc	: DINT;
END_VAR
VAR_OUTPUT
	retval  : PHCDATA;
END_VAR;
#define USB_iUSBGetHCDataByIndex(p1)  	pUSB^.iUSBGetHCDataByIndex $ iUSBGetHCDataByIndex(p1)


// Anzahl der Devs
FUNCTION __CDECL GLOBAL iUSBGetDevCount
VAR_OUTPUT
	retval  : DINT;
END_VAR;
#define USB_iUSBGetDevCount()  	pUSB^.iUSBGetDevCount $ iUSBGetDevCount()


// Daten eines Devs
FUNCTION __CDECL GLOBAL iUSBGetDevDataByIndex
VAR_INPUT
	iDev	: DINT;
END_VAR
VAR_OUTPUT
	retval  : PDEVDATA;
END_VAR;
#define USB_iUSBGetDevDataByIndex(p1)  	pUSB^.iUSBGetDevDataByIndex $ iUSBGetDevDataByIndex(p1)


// String
FUNCTION __CDECL GLOBAL iUSBGetString
VAR_INPUT
	pDD		: PDEVDATA;
	iLanguage	: DINT;
	iIndex		: DINT;
	pS		: ^SINT;
END_VAR
VAR_OUTPUT
	retval  : ^SINT;
END_VAR;
#define USB_iUSBGetString(p1,p2,p3,p4)  	pUSB^.iUSBGetString $ iUSBGetString(p1,p2,p3,p4)


// Speed
FUNCTION __CDECL GLOBAL iUSBGetSpeed
VAR_INPUT
	pDD	: PDEVDATA;
END_VAR
VAR_OUTPUT
	retval  : DINT;
END_VAR;
#define USB_iUSBGetSpeed(p1)  	pUSB^.iUSBGetSpeed $ iUSBGetSpeed(p1)


// Device Desc
FUNCTION __CDECL GLOBAL iUSBGetDevDesc
VAR_INPUT
	pDD	: PDEVDATA;
END_VAR
VAR_OUTPUT
	retval  : PDEVDESC;
END_VAR;
#define USB_iUSBGetDevDesc(p1)  	pUSB^.iUSBGetDevDesc $ iUSBGetDevDesc(p1)


// Device Desc
FUNCTION __CDECL GLOBAL iUSBGetConfDesc
VAR_INPUT
	pDD	: PDEVDATA;
	iConf	: DINT;
END_VAR
VAR_OUTPUT
	retval  : PCONFDESC;
END_VAR;
#define USB_iUSBGetConfDesc(p1,p2)  	pUSB^.iUSBGetConfDesc $ iUSBGetConfDesc(p1,p2)


// ExtrasDescs
FUNCTION __CDECL GLOBAL iUSBGetExtraDescs
VAR_INPUT
	pConf	: PCONFDESC;
	iMDT	: DINT;
	iI	: DINT;
END_VAR
VAR_OUTPUT
	retval  : pVoid;
END_VAR;
#define USB_iUSBGetExtraDescs(p1,p2,p3)  	pUSB^.iUSBGetExtraDescs $ iUSBGetExtraDescs(p1,p2,p3)


// Anzahl der Interfaces
FUNCTION __CDECL GLOBAL iUSBGetInterfaceCount
VAR_INPUT
	pConf	: PCONFDESC;
END_VAR
VAR_OUTPUT
	retval  : DINT;
END_VAR;
#define USB_iUSBGetInterfaceCount(p1)  	pUSB^.iUSBGetInterfaceCount $ iUSBGetInterfaceCount(p1)


// Interface Desc
FUNCTION __CDECL GLOBAL iUSBGetIntDesc
VAR_INPUT
	pConf	: PCONFDESC;
	iInt	: DINT;
END_VAR
VAR_OUTPUT
	retval  : PINTDESC;
END_VAR;
#define USB_iUSBGetIntDesc(p1,p2)  	pUSB^.iUSBGetIntDesc $ iUSBGetIntDesc(p1,p2)


// Endpoint Descr
FUNCTION __CDECL GLOBAL iUSBGetEndpointDesc
VAR_INPUT
	pI	: PINTDESC;
	iEP	: DINT;
END_VAR
VAR_OUTPUT
	retval  : PENDPDESC;
END_VAR;
#define USB_iUSBGetEndpointDesc(p1,p2)  	pUSB^.iUSBGetEndpointDesc $ iUSBGetEndpointDesc(p1,p2)


// Interface handle
FUNCTION __CDECL GLOBAL iUSBFindInterface
VAR_INPUT
	pD		: PDEVDATA;
	uiClass		: UDINT;
	uiSubClass	: UDINT;
	uiProtocol	: UDINT;
	uiIndex   	: UDINT;
	ppC		: ^PCONFDATA;
END_VAR
VAR_OUTPUT
	retval  : PINTDATA;
END_VAR;
#define USB_iUSBFindInterface(p1,p2,p3,p4,p5,p6)  	pUSB^.iUSBFindInterface $ iUSBFindInterface(p1,p2,p3,p4,p5,p6)


// Pipe handle
FUNCTION __CDECL GLOBAL iUSBFindPipe
VAR_INPUT
	pD		: PDEVDATA;
	pI		: PINTDATA;
	uiType		: UDINT;
	uiDirection	: UDINT;
	pCallback	: pVoid;
	pThis		: pVoid;
END_VAR
VAR_OUTPUT
	retval  : PPIPEDATA;
END_VAR;
#define USB_iUSBFindPipe(p1,p2,p3,p4,p5,p6)  	pUSB^.iUSBFindPipe $ iUSBFindPipe(p1,p2,p3,p4,p5,p6)


// Execute IO transfer
FUNCTION __CDECL GLOBAL iUSBExecuteIO
VAR_INPUT
	pP		: PPIPEDATA;
	pData		: pVoid;
	uiLen		: UDINT;
END_VAR
VAR_OUTPUT
	retval  : DINT;
END_VAR;
#define USB_iUSBExecuteIO(p1,p2,p3)  	pUSB^.iUSBExecuteIO $ iUSBExecuteIO(p1,p2,p3)


// Reset IO transfer
FUNCTION __CDECL GLOBAL iUSBResetIO
VAR_INPUT
	pP		: PPIPEDATA;
END_VAR
VAR_OUTPUT
	retval  : DINT;
END_VAR;
#define USB_iUSBResetIO(p1)  	pUSB^.iUSBResetIO $ iUSBResetIO(p1)



//
// Maus
//
   
// Position setzen
FUNCTION __CDECL GLOBAL iUSBMousePos
VAR_INPUT
	X		: DINT;
	Y		: DINT;
END_VAR;
#define USB_iUSBMousePos(p1,p2)  	pUSB^.iUSBMousePos $ iUSBMousePos(p1,p2)


// Config
FUNCTION __CDECL GLOBAL iUSBMouseConfig
VAR_INPUT
	MouseScaleX	: DINT;
	MouseScaleY	: DINT;
	DoubleClickTime	: DINT;
END_VAR;
#define USB_iUSBMouseConfig(p1,p2,p3)  	pUSB^.iUSBMouseConfig $ iUSBMouseConfig(p1,p2,p3)


//
// Touch
//

// Config
//void (* USBTouchConfig)( int (* pTouchEventUSB)( int iVID, int iPID, void *RawData ) );
FUNCTION __CDECL GLOBAL iUSBTouchConfig
VAR_INPUT
	pTouchEventUSB	: pVoid;
END_VAR;
#define USB_iUSBTouchConfig(p1)  	pUSB^.iUSBTouchConfig $ iUSBTouchConfig(p1)


//
// Maus
//
// Position setzen
FUNCTION __CDECL GLOBAL iUSBMousePosWithoutEvent
VAR_INPUT
	X		: DINT;
	Y		: DINT;
END_VAR;
#define USB_iUSBMousePosWithoutEvent(p1,p2)  	pUSB^.iUSBMousePosWithoutEvent $ iUSBMousePosWithoutEvent(p1,p2)

//
// LasalOS 01.02.229
// Functions to handle blinking of some special KINGSTON 4GB memory sticks.
//
FUNCTION __CDECL GLOBAL iUSBSendTestUnitReadyToUSBStick
VAR_INPUT
Drive		: CHAR;
END_VAR;
#define USB_iUSBSendTestUnitReadyToUSBStick(p1)  	pUSB^.iUSBSendTestUnitReadyToUSBStick $ iUSBSendTestUnitReadyToUSBStick(p1)

FUNCTION __CDECL GLOBAL iUSBResetTestUnitUSBStick
VAR_INPUT
Drive		: CHAR;
END_VAR;
#define USB_iUSBResetTestUnitUSBStick(p1)  	pUSB^.iUSBResetTestUnitUSBStick $ iUSBResetTestUnitUSBStick(p1)

// Execute control transfer
FUNCTION __CDECL GLOBAL iUSBExecuteControl
VAR_INPUT
	pP		        : PPIPEDATA;
  bmRequestType : USINT;
  bRequest      : USINT;
  wValue        : UINT;
  wIndex        : UINT;
	pData		      : pVoid;
	uiLen		      : UDINT;
END_VAR
VAR_OUTPUT
	retval  : DINT;
END_VAR;
#define USB_iUSBExecuteControl(p1,p2,p3,p4,p5,p6,p7)  pUSB^.iUSBExecuteControl $ iUSBExecuteControl(p1,p2,p3,p4,p5,p6,p7)

// Detach Driver
FUNCTION __CDECL GLOBAL iUSBDetachDriver
VAR_INPUT
	uiVendor	: UDINT;
	uiproduct  	: UDINT;
	iinterface	: DINT;
END_VAR
VAR_OUTPUT
	retval  : DINT;
END_VAR;
#define USB_iUSBDetachDriver(p1,p2,p3,)  	pUSB^.iUSBDetachDriver $ iUSBDetachDriver(p1,p2,p3)

// Reset Device of Pipe
FUNCTION __CDECL GLOBAL iUSBResetDeviceOfPipe
VAR_INPUT
	pP		: PPIPEDATA;
END_VAR;
#define USB_iUSBResetDeviceOfPipe(p1)  	pUSB^.iUSBResetDeviceOfPipe $ iUSBResetDeviceOfPipe(p1)

#pragma pack(pop)

#endif
