
//{{LSL_IMPLEMENTATION
FUNCTION CLexem::iCompare
VAR_INPUT
	pText		: ^CLexem;
END_VAR
VAR_OUTPUT
	equal		: BOOL;
END_VAR

	equal	:= udCRC = pText^.udCRC & _stricmp(#Text[0], #pText^.Text[0]) = 0;

END_FUNCTION
// Liefert die Stringlänge des CLexem - Textes
FUNCTION CLexem::Length
VAR_OUTPUT
	udLen		: UDINT;
END_VAR

	udLen	:= _strlen(#Text[0]);

END_FUNCTION
// Liefert die Gesamtgröße des CLexem - Objekts (CRC32 + Name + 0er)
FUNCTION CLexem::Size
VAR_OUTPUT
	udSize		: UDINT;
END_VAR

	udSize	:= sizeof(CLexem) + Length();

END_FUNCTION


FUNCTION CLocLex::InitText
VAR_INPUT
	pText		: ^char;
END_VAR

	_strcpy(#Text[0], pText);
//	udCRC	:= L2_CRC32(pText);
  udCRC := Crc32(pText);

END_FUNCTION
