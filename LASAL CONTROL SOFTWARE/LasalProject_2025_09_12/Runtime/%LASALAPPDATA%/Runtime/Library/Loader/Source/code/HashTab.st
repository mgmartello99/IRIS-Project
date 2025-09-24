#include ".\Loader\RTOS_MemIntern.h"
#include ".\include\HashTab.h"

#define MinTableSize (10)


FUNCTION P_Hash
VAR_INPUT
	Key : pVoid;
END_VAR  
VAR_OUTPUT
  idx : UDINT;
END_VAR;

FUNCTION P_Cmp
VAR_INPUT
	Element : pVoid;
	Key : pVoid;
  cookie : pVoid;
END_VAR  
VAR_OUTPUT
  result : DINT;
END_VAR;

TYPE
  ListNode : STRUCT
    Element : pVoid;
    Value : pVoid;
    Next : ^ListNode;
  END_STRUCT;
  PListNode : ^ListNode;

END_TYPE

(* Return next prime; assume nbr >= 10 *)
FUNCTION NextPrime
VAR_INPUT
  nbr : UDINT;
END_VAR
VAR_OUTPUT
  result : UDINT;
END_VAR
VAR
	i : UDINT;
END_VAR

  IF nbr MOD 2 = 0 THEN
    nbr += 1;
  END_IF;

  WHILE 1 DO
    i := 3;
    WHILE i * i <= nbr DO 
      IF (nbr MOD i) = 0 THEN
        GOTO ContOuter;
      END_IF;
      i += 2;
    END_WHILE;
    result := nbr;
    RETURN;
    
ContOuter:  
    nbr += 2;
  END_WHILE;
    
END_FUNCTION

(* Hash function *)
FUNCTION Hash
VAR_INPUT
  Key : pVoid;
  TableSize : UDINT;
  H : HashTable;
END_VAR
VAR_OUTPUT
  idx : UDINT;
END_VAR

  idx := H^.fnHash$P_Hash(Key);
  idx := idx MOD TableSize;
  
END_FUNCTION


(* Initialisiert eine Hashtable *)
FUNCTION GLOBAL HashTable_Initialize
VAR_INPUT
  TableSize : UDINT;
  fnHash : pVoid;
  fnCmp : pVoid;
END_VAR
VAR_OUTPUT
  H : HashTable;
END_VAR
VAR
	i : UDINT;
  pList : ^PListNode;
END_VAR

  H := NIL;

  IF TableSize < MinTableSize THEN
    // Table size too small
    RETURN;
  END_IF;

  (* Allocate table *)
  H := _alloc_align(SIZEOF(HashTbl))$HashTable;
  IF H = NIL THEN
    RETURN;
  END_IF;

  H^.TableSize := NextPrime(TableSize);
  H^.containerBlks := NIL;

  (* Allocate array of lists *)
  H^.TheLists := LDR_alloc_in_container(#H^.containerBlks, SIZEOF(PListNode) * H^.TableSize)$^PListNode;
  IF H^.TheLists = NIL THEN
    H := NIL;
    RETURN;
  END_IF;

  H^.fnHash$pVoid := fnHash;
  H^.fnCmp$pVoid  := fnCmp;

  (* Allocate list headers *)
  FOR i := 0 TO H^.TableSize DO 
    pList := H^.TheLists + i * SIZEOF(PListNode);
    pList^ := LDR_alloc_in_container(#H^.containerBlks, SIZEOF(ListNode))$PListNode;
    IF pList^ = NIL THEN
      H := NIL;
      RETURN;
    END_IF;
    pList^^.Next := NIL;
  END_FOR;

END_FUNCTION

(* Zerstört eine Hashtable *)
FUNCTION GLOBAL HashTable_Destroy
VAR_INPUT
  pH : ^HashTable;
END_VAR

  IF pH^ = NIL THEN
    RETURN;
  END_IF;

  LDR_free_all_container(#pH^^.containerBlks);
  _free(pH^);
  pH^ := NIL;

END_FUNCTION

(* Sucht ein Element in der Hashtable anhand eines Schlüssels *)
FUNCTION GLOBAL HashTable_Find
VAR_INPUT
  Key : pVoid;
  H : HashTable;
  cookie : pVoid;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR
VAR
	P : PListNode;
  L : PListNode;
  idx : UDINT;
  pList : ^PListNode;
END_VAR

  result := NIL;
  IF H = NIL THEN
    RETURN;
  END_IF;

  idx := Hash(Key, H^.TableSize, H);
  pList := H^.TheLists + idx * SIZEOF(PListNode);
  
  L := pList^;
  P := L^.Next;
  WHILE (P <> NIL) & (H^.fnCmp$P_Cmp(P^.Element, Key, cookie) = 0) DO
    P := P^.Next;
  END_WHILE;
      
  result := P;

END_FUNCTION

#ifdef OTTOSPEED
FUNCTION HashTable_Find_intern
VAR_INPUT
  Key : pVoid;
  H : HashTable;
  cookie : pVoid;
  idx : UDINT;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR
VAR
	P : PListNode;
  L : PListNode;
  pList : ^PListNode;
END_VAR

  result := NIL;

//  idx := Hash(Key, H^.TableSize, H);
  pList := H^.TheLists + idx * SIZEOF(PListNode);
  
  L := pList^;
  P := L^.Next;
  WHILE (P <> NIL) & (H^.fnCmp$P_Cmp(P^.Element, Key, cookie) = 0) DO
    P := P^.Next;
  END_WHILE;
      
  result := P;

END_FUNCTION
#endif

(* Fügt einen Element in die Hashtable ein

  return 1, wenn das Element eingefügt wurde
  return 0, wenn das Element bereits in der Hashtable war
  return <0 bei einem Fehler
*)
FUNCTION GLOBAL HashTable_Insert
VAR_INPUT
  Key : pVoid;
  Value : pVoid;
  H : HashTable;
  cookie : pVoid;
END_VAR
VAR_OUTPUT
  result : DINT;
END_VAR
VAR
	NewCell : PListNode;
  L : PListNode;
  idx : UDINT;
  pList : ^PListNode;
END_VAR

#ifdef OTTOSPEED
  idx := Hash(Key, H^.TableSize, H);
  IF HashTable_Find_intern(Key, H, cookie, idx) THEN
#else
  IF H = NIL THEN
    result := -1;
    RETURN;
  END_IF;
  
  IF HashTable_Find(Key, H, cookie) THEN
#endif
    result := 0;
    RETURN;
  END_IF;
  
  NewCell := LDR_alloc_in_container(#H^.containerBlks, SIZEOF(ListNode))$PListNode;
  IF NewCell = NIL THEN
    result := -2;
    RETURN;
  END_IF;

#ifndef OTTOSPEED
  idx := Hash(Key, H^.TableSize, H);
#endif
  pList := H^.TheLists + idx * SIZEOF(PListNode);
    
  L := pList^;
  NewCell^.Next := L^.Next;
  NewCell^.Element := Key;
  NewCell^.Value := Value;
  L^.Next := NewCell;

  result := 1;

END_FUNCTION

(* Iteriert über die Einträge in der Hashtabelle 
  Startwerte: 
    pIdx : Zeiger auf einen UDINT mit dem Wert -1, 
    pNode: Zeiger auf einen pVoid mit dem Wert NIL
  Ende:
    Returnwert NIL
*)
FUNCTION GLOBAL HashTable_Iter
VAR_INPUT
  H : HashTable;
  pIdx : ^DINT;
	pNode : ^pVoid;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR
VAR
	P : PListNode;
  L : PListNode;
  idx : UDINT;
  pList : ^PListNode;
END_VAR

  idx := pIdx^$UDINT;
  P := pNode$^PListNode^;

  result := NIL;
  IF H = NIL THEN
    RETURN;
  END_IF;

  WHILE P = NIL DO 
    idx += 1;
    IF idx >= H^.TableSize THEN
      GOTO End;
      RETURN;
    END_IF;
    pList := H^.TheLists + idx * SIZEOF(PListNode);
    L := pList^;
    P := L^.Next;
  END_WHILE;

  result := P;
End:  
  pIdx^ := idx$DINT;
  IF P = NIL THEN
    pNode$^PListNode^ := NIL;
  ELSE
    pNode$^PListNode^ := P^.Next;
  END_IF;

END_FUNCTION

(* Liefert das Element eines Hashtable Eintrags *)
FUNCTION GLOBAL HashTable_RetrieveEl
VAR_INPUT
  P : pVoid;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR
VAR
	pln : ^ListNode;
END_VAR

  pln := P$^ListNode;
  result := pln^.Element;
  
END_FUNCTION

(* Liefert den Wert eines Hashtable Eintrags *)
FUNCTION GLOBAL HashTable_RetrieveVal
VAR_INPUT
  P : pVoid;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR
VAR
	pln : ^ListNode;
END_VAR

  pln := P$^ListNode;
  result := pln^.Value;
  
END_FUNCTION

(*============================================================================*)

//#define HASHTAB_TEST
#ifdef HASHTAB_TEST

FUNCTION MyHash
VAR_INPUT
	Key : pVoid;
END_VAR  
VAR_OUTPUT
  idx : UDINT;
END_VAR
VAR
	c : ^CHAR;
  c_upper : CHAR;
END_VAR

  idx := 5381;
  
  c := Key$^CHAR;
  WHILE c^ DO 
    IF c^ >= 'a' & c^ <= 'z' THEN
      c_upper := c^ - 'a' + 'A';
    ELSE
      c_upper := c^;
    END_IF;
    idx := ((idx ROL 5) + idx) + c_upper;  
    c += 1;
  END_WHILE;
  
  idx := idx AND 0x7FFFFFFF;

END_FUNCTION

FUNCTION MyCmp
VAR_INPUT
	Element : pVoid;
	Key : pVoid;
  cookie : pVoid;
END_VAR  
VAR_OUTPUT
  result : DINT;
END_VAR

  result := _stricmp(Element$^CHAR, Key$^CHAR)$DINT;

END_FUNCTION

FUNCTION GLOBAL HashTable_Test
VAR
	H : HashTable;
  res : pVoid;
  key : ^CHAR;
  val : pVoid;
END_VAR

  H := HashTable_Initialize(11, #MyHash(), #MyCmp());
  
  res := HashTable_Find("hallo", H, NIL);
  
  HashTable_Insert("hallo", NIL, H, NIL);
  res := HashTable_Find("HallO", H, NIL);
  IF res THEN
    key := HashTable_RetrieveEl(res)$^CHAR;
    val := HashTable_RetrieveVal(res);
  END_IF;

END_FUNCTION

#endif // HASHTAB_TEST