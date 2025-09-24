
#include <LSL_string.h>


#ifdef STRING_STR
FUNCTION _memcpyrev
VAR_INPUT
 dest            : ^USINT;
 src             : ^USINT;
 anz             : UINT;
END_VAR;
#else
FUNCTION GLOBAL AWL _memcpyrev
VAR_INPUT
 dest            : ^USINT;
 src             : ^USINT;
 anz             : UDINT;
END_VAR;
#endif

//    ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//    บ Kopieren von Speicherbloeken (auch ueberlappend)       บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ --> : dest .... --> auf Ziel                           บ
//    บ       src ..... --> auf Quelle                         บ
//    บ       anz ..... Anzahl der zu kopierenden Bytes        บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ Name: OK     ณ Entwickelt:          ณ Update:          บ
//    ศออออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ

FUNCTION GLOBAL _memmove
VAR_INPUT
 dest            : ^void;
 src             : ^void;
 anz             : UDINT;
END_VAR

	IF (src < dest) THEN
	  _memcpyrev((dest$UDINT+anz-1)$^USINT, (src$UDINT+anz-1)$^USINT, anz);
	ELSE
	  _memcpy(dest, src, anz);
	END_IF;

END_FUNCTION

//    
//    This function conconates two strings.

FUNCTION GLOBAL _strcat
VAR_INPUT
 dest            : ^CHAR;
 src             : ^CHAR;
END_VAR
VAR
 len             : UDINT;
END_VAR

  len := _strlen(dest);
  _strcpy(dest+len, src);

END_FUNCTION

// 
// -> ptr	:
// -> count	:
// <- crc_16:
//
FUNCTION GLOBAL _memicmp
VAR_INPUT
	str1		: ^void;
	str2		: ^void;
	len			: UDINT;
END_VAR
VAR_OUTPUT
	result		: UDINT;
END_VAR
VAR	
	c1			: CHAR;
	c2			: CHAR;
END_VAR

	WHILE len DO
		c1 := str1^$char;
		c2 := str2^$char;

		IF c1 >= 'a' & c1 <= 'z' THEN
			c1 -= 'a'-'A';
		END_IF;
		IF c2 >= 'a' & c2 <= 'z' THEN
			c2 -= 'a'-'A';
		END_IF;

		IF c1 <> c2 THEN
			result := 1;
			RETURN;
		END_IF;

		str1 += 1;
		str2 += 1;
		len  -= 1;

	END_WHILE;

	result := 0;

END_FUNCTION
// ....................................................................................
// vergleich 2 strings miteinander aber gro฿ kleinschreibung wir koriegiert A= a und B=b
// der erste String ist gültig für die Lไnge
// ....................................................................................
// aschl
FUNCTION AWL GLOBAL _stricmp
VAR_INPUT
 str1            : ^char;
 str2            : ^char;
END_VAR
VAR_OUTPUT
 retcode         : UDINT;
END_VAR

	l.edi		str1
	l.ebx		str2
	l.d			retcode,0		// gültig
_stricmp_loop	
	l			(edi)
	o.al		al
	jz			_stricmp_ret

// first try it could be the same
	cmp.al		(ebx)
	je			_stricmp_3

	cmp.al		'a'
	jb			_stricmp_1
	cmp.al		'z'
	ja			_stricmp_1
	add.al		'A'-'a'		
	
_stricmp_1
	l.ah		(ebx)
	cmp.ah		'a'
	jb			_stricmp_2
	cmp.ah		'z'
	ja			_stricmp_2
	add.ah		'A'-'a'		
	
_stricmp_2
	cmp.al		ah
	jnz			_stricmp_fault
_stricmp_3
	inc			edi
	inc			ebx
	jmp			_stricmp_loop
			
_stricmp_ret
	cmp.al		(ebx)			// ascii-0
	jz			_stricmp_end
_stricmp_fault	
	l.d			retcode,1
_stricmp_end

//  retcode := _memicmp(str1, str2, _strlen(str1)+1);

END_FUNCTION



#ifdef STRING_STR


//    Zeichenketten (Nullstrings) vergleichen. Zwischen Gross-
//    und Kleinschreibung wird unterschieden.
//    
//    -> str1: Zeiger auf 1. Nullstring
//    -> str2: Zeiger auf 2. Nullstring
//    
//    <- 0...Zeichenketten stimmen ueberein
//       1...Zeichenketten stimmen nicht ueberein
//    
//    Erstellt: Engl Roland, 27.11.97
//    
//    Updates :

FUNCTION GLOBAL _STRCMP
VAR_INPUT
 str1            : ^SINT;
 str2            : ^SINT;
END_VAR
VAR_OUTPUT
 retcode         : UINT;
END_VAR

	ret_code := _memcmp(str1, str2, _strlen(str1)+1);

END_FUNCTION


//    ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//    บ ermittelt erstes Vorkommen von 'chr' im String 'src'.  บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ --> : src ..... --> auf String                         บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ <-- : Zeiger auf das gefundene Zeichen oder NULL       บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ Name: OK     ณ Entwickelt:          ณ Update:          บ
//    ศออออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ

FUNCTION GLOBAL _STRCHR
VAR_INPUT
 src             : ^USINT;
 chr             : CHAR;
END_VAR
VAR_OUTPUT
 dest            : ^CHAR;
END_VAR

  WHILE src^ DO
    IF src^ = chr THEN
      dest := src;
      RETURN;
    END_IF;
    src += 1;
  END_WHILE;

  dest := NIL;

END_FUNCTION


//    ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//    บ ermittelt Stringlaenge                                 บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ --> : src ..... --> auf String                         บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ <-- : Stringlaenge (ohne 'final 0')                    บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ Name: OK     ณ Entwickelt:          ณ Update:          บ
//    ศออออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ

FUNCTION GLOBAL _STRLEN
VAR_INPUT
 src             : ^CHAR;
END_VAR
VAR_OUTPUT
 retcode         : UINT;
END_VAR

  retcode := 0;
  WHILE src^ DO
    src += 1;
    retcode += 1;
  END_WHILE;

END_FUNCTION


//    ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//    บ Speicherbloeke vergleichen                             บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ --> : dest, src .... --> auf Speicherblock             บ
//    บ       anzahl ....... Anzahl der zu vergleichenden Bytesบ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ <-- : 0, oder ungleich 0                               บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ Name: OK     ณ Entwickelt:          ณ Update:          บ
//    ศออออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ

FUNCTION GLOBAL _MEMCMP
VAR_INPUT
 DEST            : ^USINT;
 SRC             : ^USINT;
 anzahl          : UINT;
END_VAR
VAR_OUTPUT
 retcode         : UINT;
END_VAR

WHILE anzahl >= 4 & dest^$UDINT = src^$UDINT DO
  anzahl -= 4;
  dest += 4;
  src  += 4;
END_WHILE;

WHILE anzahl & dest^ = src^ DO
  anzahl -= 1;
  dest += 1;
  src  += 1;
END_WHILE;

IF anzahl THEN
  retcode := 1;
ELSE
  retcode := 0;
END_IF;

END_FUNCTION

//     ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//     บ Stringcopy                                            บ
//     วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//     บ --> : dest ..... pointer to destination               บ
//     บ       src ...... pointer to source                    บ
//     บ       max ...... maximum number of characters to copy บ
//     วฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//     บ Name: OK    ณ Developed:           ณ Update:          บ
//     ศอออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ
//    
//     This function copies the 'src'-string to the 'dest'-
//     string. If the length of 'src' exceeds 'max', then only
//     the first 'max' characters are copied. The destination
//     string will always be 0-terminated.

FUNCTION GLOBAL _STRNCPY
VAR_INPUT
 dest            : ^CHAR;
 src             : ^CHAR;
 max             : UINT;
END_VAR
VAR
 len                             : UINT;
END_VAR

  len := _strlen(src);
  IF len > max THEN
    len := max;
  END_IF;

  _MEMCPY(dest, src, len + 1);

  (dest+len)^ := 0;

END_FUNCTION


//     ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//     บ Stringcopy                                            บ
//     วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//     บ --> : dest ..... Pointer auf Zielstring               บ
//     บ       src ...... Pointer auf Quellstring              บ
//     วฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//     บ Name: OK    ณ Entwickelt:          ณ Update:          บ
//     ศอออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ

FUNCTION GLOBAL _STRCPY
VAR_INPUT
 dest            : ^CHAR;
 src             : ^CHAR;
END_VAR

  _MEMCPY(dest, src, _STRLEN(src) + 1);

END_FUNCTION


//    ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//    บ Speicher kopieren                                      บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ --> : dest, src ... --> auf Speicher                   บ
//    บ       anzahl ...... Anzahl der Bytes                   บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ Name: OK     ณ Entwickelt:          ณ Update:          บ
//    ศออออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ
FUNCTION GLOBAL _MEMCPY
VAR_INPUT
 DEST            : ^USINT;
 SRC             : ^USINT;
 anzahl          : UINT;
END_VAR

  WHILE anzahl >= 4 DO
    dest^$UDINT := src^$UDINT;
    anzahl -= 4;
    dest += 4;
    src  += 4;
  END_WHILE;

  WHILE anzahl DO
    dest^ := src^;
    anzahl -= 1;
	dest += 1;
	src += 1;
  END_WHILE;

END_FUNCTION

//    ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//    บ Speicher fuellen                                       บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ --> : dest .... --> auf Speicher                       บ
//    บ       byte .... Fuellwert                              บ
//    บ       anzahl .. Anzahl der Bytes                       บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ Name: OK     ณ Entwickelt:          ณ Update:          บ
//    ศออออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ
FUNCTION GLOBAL _MEMSET
VAR_INPUT
 dest            : ^USINT;
 usbyte          : USINT;
 anzahl          : UINT;
END_VAR


  WHILE anzahl DO

    dest^ := usbyte;

    dest += 1;
    anzahl -= 1;

  END_WHILE;

END_FUNCTION

//    ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
//    บ Speicher 'rueckwaerts' kopieren                        บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ --> : dest, src ... --> auf Speicher                   บ
//    บ       anz ......... Anzahl der Bytes                   บ
//    วฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//    บ Name: OK     ณ Entwickelt:          ณ Update:          บ
//    ศออออออออออออออฯออออออออออออออออออออออฯออออออออออออออออออผ

FUNCTION _MEMCPYREV
VAR_INPUT
 dest            : ^USINT;
 src             : ^USINT;
 anz             : UDINT;
END_VAR

  dest += anz;
  src  += anz;

  WHILE anz >= 4 DO
    dest -= 4;
    src  -= 4;
    anz -= 4;

    dest^$UDINT := src^$UDINT;

  END_WHILE;

  WHILE anz DO
    dest -= 1;
    src  -= 1;
    anz -= 1;

    dest^ := src^;

  END_WHILE;

END_FUNCTION
#endif


// verbessert in AWL 16.04.2003 //

FUNCTION AWL _strupr
VAR_INPUT
 str				: ^CHAR;
END_VAR


	l.EDI		str
_stupr_loop
	L.AL		(edi)
	o.al		al
	jz			_stupr_ret
	cmp.al		'a'
	jb			_stupr_next
	cmp.al		'z'
	ja			_stupr_next
	add.b		(edi),('A'-'a')
_stupr_next
	inc			edi
	jmp			_stupr_loop
_stupr_ret
END_FUNCTION
// converts a UDINT to a ascii-string in hexadecimal format without prefix
// i.e. 16#1234	-> "1234$00" (with terminating 0)
// #FA23122003 Fehler Bei wandel auf (A-F kommt B-G raus)

FUNCTION AWL GLOBAL _itoa
VAR_INPUT
	udNumber		: UDINT;
	pString			: ^CHAR;
END_VAR

	l.edi		pString	
	l.ebx		udNumber
	o.ebx		ebx
	jnz			_itoa_nz
	l.b			(edi) ,'0'
	l.b			(edi+1),0
	jmp			_itoa_ret
	
_itoa_nz
	l.ecx		8
_itoa_loop
	rol.ebx		4
	l.al		bl
	and.al		16#0f
	add.al		'0'
	cmp.al		'9'
	jbe			_itoa_09
	add.al		'A'-'0'-16#0A
			
_itoa_09
	stosb
	loop		_itoa_loop
	clr			al
	stosb

_itoa_ret	 
	

END_FUNCTION  



#ifndef __C
FUNCTION STRUPR
VAR_INPUT
	STR			: ^CHAR;
END_VAR
VAR
	c				:  CHAR;
END_VAR

	WHILE TRUE DO

		c := str^;
		IF c = 0 THEN
			EXIT;
		END_IF;
	
		IF (c >= 'a' & c <= 'z') THEN
			str^ := c-'a'+'A';
		END_IF;
		str += 1;

	END_WHILE;

END_FUNCTION
#endif