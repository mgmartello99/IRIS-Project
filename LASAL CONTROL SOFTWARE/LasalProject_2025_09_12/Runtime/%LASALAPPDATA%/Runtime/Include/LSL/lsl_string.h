// ===============================================================================================
// functions of the string library
// ===============================================================================================
//#define STRING_STR

FUNCTION GLOBAL _strcpy
VAR_INPUT
 dest            : ^CHAR;
 src             : ^CHAR;
END_VAR;

FUNCTION GLOBAL _strcat
VAR_INPUT
 dest            : ^CHAR;
 src             : ^CHAR;
END_VAR;

FUNCTION GLOBAL _memmove
VAR_INPUT
	dest            : ^void;
	src             : ^void;
	anz             : UDINT;
END_VAR;

FUNCTION GLOBAL _ismemset
VAR_INPUT
	src				: ^void;
	val				: usint;
	cntr			: udint;
END_VAR
VAR_OUTPUT
  retcode			: UDINT;
END_VAR;

FUNCTION GLOBAL _memswap
VAR_INPUT
	ptr1			: ^void;
	ptr2			: ^void;
	cntr			: UDINT;
END_VAR;

FUNCTION GLOBAL _strupr
VAR_INPUT
	str	: ^Char;
END_VAR;

FUNCTION GLOBAL _memicmp
VAR_INPUT
	str1            : ^void;
	str2            : ^void;
	len             : UDINT;
END_VAR
VAR_OUTPUT
	result          : UDINT;
END_VAR;

FUNCTION GLOBAL _strcmp
VAR_INPUT
	str1            : ^char;
	str2            : ^char;
END_VAR
VAR_OUTPUT
	retcode         : UINT;
END_VAR;

FUNCTION GLOBAL _stricmp
VAR_INPUT
	str1            : ^char;
	str2            : ^char;
END_VAR
VAR_OUTPUT
	retcode         : UDINT;
END_VAR;

FUNCTION GLOBAL _strchr
VAR_INPUT
	src             : ^char;
	chr             : CHAR;
END_VAR
VAR_OUTPUT
	dest            : ^CHAR;
END_VAR;

FUNCTION GLOBAL _strlen
VAR_INPUT
	src             : ^CHAR;
END_VAR
VAR_OUTPUT
	retcode         : UDINT;
END_VAR;

FUNCTION GLOBAL _memcmp
VAR_INPUT
	ptr1			: ^void;
	ptr2			: ^void;
	cntr			: UDINT;
END_VAR
VAR_OUTPUT
	notequal		: UDINT;
END_VAR;

FUNCTION GLOBAL _strncpy
VAR_INPUT
	dest            : ^CHAR;
	src             : ^CHAR;
	max             : UDINT;
END_VAR;

FUNCTION GLOBAL _memcpy
VAR_INPUT
	ptr1			: ^void;
	ptr2			: ^void;
	cntr			: UDINT;
END_VAR;

FUNCTION GLOBAL _memset
VAR_INPUT
	dest			: ^void;
	usByte			: USINT;
	cntr			: UDINT;
END_VAR;

FUNCTION GLOBAL _itoa
VAR_INPUT
	udNumber		: UDINT;
	pString			: ^CHAR;
END_VAR;

FUNCTION GLOBAL _Format
VAR_INPUT
  pFormat   : ^char;
  pTxt0     : ^char;
  pTxt1     : ^char;
  pTxt2     : ^char;
END_VAR
VAR_OUTPUT
  pText     : ^char;
END_VAR;
