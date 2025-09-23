//#include <RTOS_Private.h>
//#include <RTOS_Variables.h>
#include "RTOS_IprIntern.h"

(*

Abarbeitung des Interpretercodes
================================

Mit dem Aufruf der GetResu Funktion wird Interpretercode ausgeführt und das Ergebnis 
zurückgegeben.


Klammern im Interpreter
-----------------------
Damit Klammern im Interpretercode richtig funktionieren, muss man folgendes beachten:
- Eine öffnende Klammer (P_POPEN) fasst nur die nächsten beiden Befehle zusammen
- Eine schliessende Klammer (P_CLOSE) ist ein leerer Befehl der ausser dem 
  Instruktion-Pointer nichts verändert

Ein mathematischer Ausdruck mit Klammern muss daher in manchen Fällen angepasst werden,
damit ihn der Interpreter richtig auswertet. Eine Anpassung ist notwendig, wenn entweder 
mehr als 2 Befehle in einem Klammerausdruck vorkommen, oder wenn mehrere Klammerebenen 
verwendet werden.

Die Anpassung wird folgendermaßen gemacht:
- Wenn in einer Klammer mehr als 2 Befehle stehen, dann werden die letzten beiden 
  Befehle in Klammern gesetzt. Das wird so lange wiederholt, bis jede öffnende 
  Klammer nur mehr 2 Befehle enthält.
- Schliessende Klammern müssen entweder alle entfernt werden, oder man ersetzt jede Klammer 
  durch 2 Klammern gleichen Typs.
  
Beispiele:

Ausdruck:
  222 / (((3 * (4 * 5)) + (6 + 7)))
Interpretercode:
  222 / ((((((3 * ((4 * 5)))) + ((6 + 7))))))
  oder
  222 / (((3 * (4 * 5 + (6 + 7
  
Ausdruck:
  0 - (1 + 1 + 1 + 1) - 1
Interpretercode:
  (0 - ((1 + ((1 + ((1 + 1)))))) - 1)
  oder
  (0 - (1 + (1 + (1 + 1 - 1

Vorrangregeln
-------------
Vorrangregeln gibt es keine, d.h. es gibt keine Punkt-Vor-Strich-Regelung.
Die Befehle werden einfach der Reihe nach abgearbeitet. Wenn die Auswertereihenfolge eine 
Rolle spielt, dann muss man Klammern setzen unter Beachtung der oben angegebenen Regeln.

*)

#ifdef _LSL_TARGETARCH_ARM
FUNCTION EXPR
VAR_INPUT
  pin  : ^pvoid;
  result : dint;
  psysdata : ^void;
END_VAR
VAR_OUTPUT
	retcode : DINT;
END_VAR
  VAR
    src : ^void;
    ptmp : ^dint;
    error : bool;
  END_VAR

  error := false;
  src   := pin^;

  case src$^Prefix^ of
  
    P_COMMA  ,
    P_EOL    : 
  
    P_IMMED  : src += 1;
	             result := (src$^dint)^;
               src += 4;
               
    P_VARIA  : src += 1;
               result := src^$^VirtualBase^.Read();
               src += 4;
    
    P_EQ     : src += 1;
               if(result = EXPR(#src, result, psysdata)) then
                 result := 1;
               else
                 result := 0;
               end_if;
               
    P_NEQ    : src += 1;
               if(result <> EXPR(#src, result, psysdata)) then
                 result := 1;
               else
                 result := 0;
               end_if;
               
    P_GT     : src += 1;
               if(result > EXPR(#src, result, psysdata)) then
                 result := 1;
               else
                 result := 0;
               end_if;
    
    P_GEQ    : src += 1;
               if(result >= EXPR(#src, result, psysdata)) then
                 result := 1;
               else
                 result := 0;
               end_if;
    
    P_LT     : src += 1;
               if(result < EXPR(#src, result, psysdata)) then
                 result := 1;
               else
                 result := 0;
               end_if;
               
    P_LEQ    : src += 1;
               if(result <= EXPR(#src, result, psysdata)) then
                 result := 1;
               else
                 result := 0;
               end_if;

    P_NOT    : src += 1;
               result := EXPR(#src, result, psysdata);
               if(result and 1) then
                 result := 0;
               else
                 result := 1;
               end_if;

    P_AND    : src += 1;
               result := result and EXPR(#src, result, psysdata);

    P_OR     : src += 1;
               result := result or EXPR(#src, result, psysdata);

    P_XOR	   : src += 1;
               result := result xor EXPR(#src, result, psysdata);

    P_ADD    : src += 1;
               result := result + EXPR(#src, result, psysdata);
    
    P_SUB    : src += 1;
               result := result - EXPR(#src, result, psysdata);

    P_MUL    : src += 1;
               // cast auf UDINT, damit unsigned gerechnet wird wird
               // (weil bei der Intel Version MUL - unsigned multiply - verwendet wird)
               result$UDINT := result$UDINT * EXPR(#src, result, psysdata)$UDINT; 
               
    P_DIV    : src += 1;
               // cast auf UDINT, damit unsigned gerechnet wird
               // (weil bei der Intel Version DIV - unsigned divide - verwendet wird)
               result$UDINT := result$UDINT / EXPR(#src, result, psysdata)$UDINT; 

    P_POPEN  : src += 1;
               result := EXPR(#src, result, psysdata);
               result := EXPR(#src, result, psysdata);
               
    P_PCLOSE : src += 1;
               
    P_SYS    : src += 1;
               case (src$^sysFunct^) of
                 RD_ABSTIME : src += 2;
                              result$udint := ops.tabsolute;
                 SYS_RESULT : src += 2;
                              ptmp   := psysdata + src$^uint^;
                              result := ptmp^;
                              src    += 2;
                 else         error  := true;
               end_case;
    
    else       src += 1;
               error := true;
  end_case;

  retcode := result;
  pin^    := src;

END_FUNCTION

FUNCTION STR GLOBAL GetResu
VAR_INPUT
	ppRead	: ^VOID;
	pResu	: ^RESULTS;
END_VAR
VAR_OUTPUT
	retcode		: DINT;
END_VAR
  var
    src : ^void;
  end_var

  retcode := 0;
  src     := ppRead^$^void;

  while true do
    case (src$^Prefix)^ of
      P_COMMA ,
      P_EOL   : exit;
      else      retcode := EXPR(#src, retcode, #pResu^.aData[0]);
    end_case;
  end_while;

  ppRead^$^void := src;

END_FUNCTION

#else

FUNCTION AWL EXPR
VAR_INPUT
	ECX		: DINT;
	EDX		: DINT;
END_VAR
VAR_OUTPUT
	ECX		: DINT;
END_VAR;

    


//    
//  The function evaluates the next complex expression.
//
//    -> ppRead	: points to pointer to expression
//    <- EAX		: Data
//       pointer to expression points to delimiter, either;
//                               P_EOL 	(end of line)
//                               P_COMMA 	(separator)

FUNCTION STR GLOBAL GetResu
VAR_INPUT
	ppRead	: ^VOID;
	pResu	: ^RESULTS;
END_VAR
VAR_OUTPUT
	new		: DINT;
END_VAR
VAR
	ip		: ^Prefix;
END_VAR

	ip		:= ppRead^$^Prefix;
	new		:= 0;
	ESI		:= ip$UDINT;

	WHILE TRUE DO

		// separators comma and end of line
		CASE ip^ OF
		P_EOL:		EXIT;	// end of line
		P_COMMA:	EXIT;	// comma
		ELSE		new := EXPR(new, (#pResu^.aData[0])$DINT);
					ip$UDINT	:= ESI;
		END_CASE;

	END_WHILE;

	// we are ready
	ppRead^$^Prefix	:= ip;
	// store pointer to next instruction

END_FUNCTION



//--------------------------------------------------------------------
// get an expression
//--------------------------------------------------------------------
// -> ECX		: result of evaluated expressions
// -> ESI		: pointer to this instruction
// <- ECX		: result of this expression
// <- ESI		: pointer to next instruction
FUNCTION AWL EXPR
VAR_INPUT
	ECX		: DINT;
	EDX		: DINT;
END_VAR
VAR_OUTPUT
	ECX		: DINT;
END_VAR

			L			5
			L.AH		(ESI)               // rd prefix

// separators comma and end of line
			CMP.AH		P_EOL
			JE			GR_R_END
			CMP.AH		P_COMMA
			JE			GR_R_END

// select prefix
			CMP.AH		P_ILLEGAL
			JAE			GR_ERR
			MUL			AH
			AND.EAX		16#FFFF			// loWord

			ADD.EAX		#GESEL()
			JMP			EAX

GR_ERR		INC			ESI
			JMP			GR_R_END

GESEL		JMP			P_IMMEDIATE
			JMP			P_VARIABLE
			JMP			P_SYSTEM
			JMP			P_P_OPEN
			JMP			P_P_CLOSE
			JMP			P_OP_ADD
			JMP			P_OP_SUB
			JMP			P_P_COMMA
			JMP			P_P_LE
			JMP			P_P_EQ
			JMP			P_P_NEQ
			JMP			P_P_GT
			JMP			P_P_GEQ
			JMP			P_P_LT
			JMP			P_P_LEQ
			JMP			P_P_NOT
			JMP			P_P_AND
			JMP			P_P_OR
			JMP			P_P_XOR	
			JMP			GR_ERR		// stream
			JMP			P_OP_MUL
			JMP			P_OP_DIV

P_P_NOT			// NOT <expr>				-> 0/1
			ADD.ESI		1
			FC			EXPR
			NOT			ECX
			A.ECX		0000000001
			JMP			P_P_LE


P_P_EQ			//  <expr1 -> ECX> EQ <expr2>
			ADD.ESI		1
			PSH			ECX
			FC			EXPR
			POP			EAX
			CMP.EAX		ECX
			JE			P_TRU			// set ECX and return
			CLR			ECX
			JMP 		P_P_LE

P_P_NEQ			// <expr1 -> ECX> NEQ <expr2>
			ADD.ESI		1
			PSH			ECX
			FC			EXPR
			POP			EAX
			CMP.EAX		ECX
			JNE			P_TRU			// set ECX and return
			CLR			ECX
			JMP			P_P_LE


P_P_GT			// <expr1 -> ECX> GT <expr2>
			ADD.ESI		1
			PSH			ECX
			FC			EXPR
			POP			EAX
			CMP.EAX		ECX
			JG			P_TRU			// set ECX and return
			CLR			ECX
			JMP			P_P_LE


P_P_GEQ			// <expr1 -> ECX> GEQ <expr2>
			ADD.ESI		1
			PSH			ECX
			FC			EXPR
			POP			EAX
			CMP.EAX		ECX
			JGE			P_TRU			// set ECX and return
			CLR			ECX
			JMP 		P_P_LE

P_P_LT			// <expr1 -> ECX> LT <expr2>
         	ADD.ESI     1
         	PSH			ECX
         	FC			EXPR
         	POP			EAX
         	CMP.EAX		ECX
         	JL			P_TRU			// set ECX and return
         	CLR			ECX
         	JMP			P_P_LE

P_P_LEQ			// <expr1 -> ECX> LEQ <expr2>
         	ADD.ESI     1
         	PSH			ECX
         	FC        EXPR
         	POP       EAX
         	CMP.EAX   ECX
         	JLE       P_TRU			// set ECX and return
         	CLR       ECX
         	JMP 		P_P_LE


P_TRU			 // TRUE
         	L.ECX		1
         	JMP 		P_P_LE


P_P_AND			// <expr1 -> ECX> AND <expr2>
         	ADD.ESI		1
         	PSH       	ECX
         	FC        EXPR
         	POP       EAX
         	AND.ECX   EAX
         	JMP 		  P_P_LE

P_P_OR			// <expr1 -> ECX> OR <expr2>
         	ADD.ESI     1
         	PSH       	ECX
         	FC        EXPR
         	POP       EAX
         	OR.ECX    EAX
         	JMP 		P_P_LE

P_P_XOR			// <expr1 -> ECX> XOR <expr2>
         	ADD.ESI     1
         	PSH       	ECX
         	FC        EXPR
         	POP       EAX
         	XOR.ECX   EAX
         	JMP 		P_P_LE

//--------------------------------------------------------------------
// constant value: <P_IM>, <4 byte data>
//--------------------------------------------------------------------

P_IMMEDIATE

         	L.ECX		(ESI+001)
         	ADD.ESI		5
         	JMP 		P_P_LE


//--------------------------------------------------------------------
// Variable:		<P_VA>,	<4 byte addr>			-> read value from channel
//--------------------------------------------------------------------

P_VARIABLE

			PUSH.D		ESI	// ip
			L.ESI		(ESI+001)
			L.EDI		(ESI+SvrCh.pMeth)               // pt method
			CALL		(EDI+ChMeth.pRd)
			L.ECX		EAX
			POP.D		ESI	// ip
			ADD.ESI		5
			JMP 		P_P_LE

//--------------------------------------------------------------------
// Systemdata
//--------------------------------------------------------------------


P_SYSTEM
			L.AX		(ESI+001)
			CMP.AX		SYS_ILLEGAL
			JAE			GR_ERR
			L.AH		5
			MUL			AH
			AND.EAX		16#FFFF
			ADD.EAX		#P_SYSEL()
			JMP			EAX

P_SYSEL		JMP			P_SY_ABSTI
			JMP			P_SY_RESU

P_SY_ABSTI
			L.ECX		OPS.tAbsolute
			ADD.ESI		3
			JMP			P_P_LE
P_SY_RESU	
			LZXW.EBX	(ESI+003)
			L.ECX		(EDX+EBX)
			ADD.ESI		5		
			JMP			P_P_LE

//--------------------------------------------------------------------
// ( <exp>
//--------------------------------------------------------------------

P_P_OPEN
			ADD.ESI		1
			FC			EXPR
			FC			EXPR
			JMP			P_P_LE

//--------------------------------------------------------------------
//  <exp>  )
//--------------------------------------------------------------------


P_P_CLOSE
			ADD.ESI		1
			JMP 		P_P_LE

//--------------------------------------------------------------------
// ADD
//--------------------------------------------------------------------

P_OP_ADD
			ADD.ESI		1
			PSH			ECX
			FC			EXPR
			POP			EAX
			ADD.ECX		EAX
			JMP 		P_P_LE


//--------------------------------------------------------------------
// SUB
//--------------------------------------------------------------------

P_OP_SUB
			ADD.ESI		1
			PSH			ECX
			FC			EXPR
			POP			EAX
			SUB.EAX		ECX
			L.ECX		EAX
			JMP 		P_P_LE


//--------------------------------------------------------------------
// MUL
//--------------------------------------------------------------------

P_OP_MUL
			ADD.ESI		1
			PSH			ECX
			FC			EXPR
			POP			EAX
			MUL			ECX
			XCH.EAX		ECX
			JMP 		P_P_LE


//--------------------------------------------------------------------
// DIV
//--------------------------------------------------------------------

P_OP_DIV
			ADD.ESI		1
			PSH			ECX
			FC			EXPR
			POP			EAX
      PSH     EDX
      CLR     EDX
			DIV			ECX
      POP     EDX
			XCH.EAX		ECX
			JMP 		P_P_LE


//--------------------------------------------------------------------
// comma  = data separator
//--------------------------------------------------------------------
// --> we never come here


P_P_COMMA


//--------------------------------------------------------------------
// linened  = ENDOF LINE
//--------------------------------------------------------------------
// --> we never come here

P_P_LE

GR_R_END

         
END_FUNCTION
#endif

