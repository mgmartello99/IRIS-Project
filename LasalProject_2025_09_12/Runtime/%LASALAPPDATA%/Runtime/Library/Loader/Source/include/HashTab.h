#pragma once

#ifndef __HASHTAB_HEADER__
#define __HASHTAB_HEADER__

TYPE
  ContainerBlock : STRUCT
    allocatedSize : UDINT;  // aktuell allokierte Grösse 
    usedSize      : UDINT;  // benutzte Grösse
    pNext         : ^ContainerBlock;
    dummyData     : USINT;  // Dummy Variable wg. variabler Länge der Daten
  END_STRUCT;  
  ContainerBlockPtr : ^ContainerBlock;
  
  (* List TheLists will be an array of lists, allocated later *)
  HashTbl : STRUCT
    TableSize : UDINT;
    TheLists : ^PListNode;
    fnHash : ^P_Hash;
    fnCmp : ^P_Cmp;
    containerBlks : ContainerBlockPtr;
  END_STRUCT;
  HashTable : ^HashTbl;
END_TYPE

FUNCTION GLOBAL LDR_alloc_in_container
VAR_INPUT
  ppContainerBlockList : ^ContainerBlockPtr;  // verkettete Liste von Container-Blöcken
	udSize		: UDINT;
END_VAR
VAR_OUTPUT
	pMem		: ^void;
END_VAR;

FUNCTION GLOBAL LDR_free_all_container
VAR_INPUT
  ppContainerBlockList : ^ContainerBlockPtr;  // verkettete Liste von Container-Blöcken
END_VAR;

FUNCTION GLOBAL HashTable_Initialize
VAR_INPUT
  TableSize : UDINT;
  fnHash : pVoid;
  fnCmp : pVoid;
END_VAR
VAR_OUTPUT
  H : HashTable;
END_VAR;

FUNCTION GLOBAL HashTable_Destroy
VAR_INPUT
  pH : ^HashTable;
END_VAR;

FUNCTION GLOBAL HashTable_Find
VAR_INPUT
  Key : pVoid;
  H : HashTable;
  cookie : pVoid;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR;

FUNCTION GLOBAL HashTable_Insert
VAR_INPUT
  Key : pVoid;
  Value : pVoid;
  H : HashTable;
  cookie : pVoid;
END_VAR
VAR_OUTPUT
  result : DINT;
END_VAR;

FUNCTION GLOBAL HashTable_Iter
VAR_INPUT
  H : HashTable;
  pIdx : ^DINT;
	pNode : ^pVoid;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR;

FUNCTION GLOBAL HashTable_RetrieveEl
VAR_INPUT
  P : pVoid;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR;

FUNCTION GLOBAL HashTable_RetrieveVal
VAR_INPUT
  P : pVoid;
END_VAR
VAR_OUTPUT
  result : pVoid;
END_VAR;

#endif // __HASHTAB_HEADER__