// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 28.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#include "lsl_st_tcp_user.h"
#include "DS_Lib.h"
#include <Lsl_st_ifssr.h>
#include <LSL_ST_IFLINKER.H>
//#include <rtos_variables.h>
#include <LSL_ST_SYSENV.H>

#pragma using StringInternal

var_external
	pMapper : ^CRCMap;		  // to find faster the CRC and the objects
  mt_api  : ^LSL_MT_TYPE; // multitask interface
end_var
var_private
  lsl_tcp_user : ^LSL_TCP_USER;
end_var

FUNCTION GLOBAL LDR_LockCRCMap;
FUNCTION GLOBAL LDR_UnlockCRCMap;

// ************************************************************************************************
// ************************************************************************************************
// CTor 
// ************************************************************************************************
// ************************************************************************************************
function global __cdecl dslib_CTor

  OS_CILGET("TCP_USER", #lsl_tcp_user);
  OS_CILGet("LSL_MULTITASK", #mt_api$void);

end_function

function global __cdecl dslib_system_message var_input txt:^char; end_var

  TRACE(txt);

end_function


// ************************************************************************************************
// ************************************************************************************************
// Atomic ARM
// ************************************************************************************************
// ************************************************************************************************
#ifdef _LSL_TARGETARCH_ARM
FUNCTION awl global __cdecl dslib_atomic_incU32 var_input pValue:^udint; end_var var_output retcode:udint; end_var
  
AtomicUIncRetry
  LDREX r1, [pValue]
  ADDS r3, r1, #1
  STREX r2, r3, [pValue]
  CMP r2, #0
  BNE AtomicUIncRetry
  MOV r0, r1

END_FUNCTION

FUNCTION awl global __cdecl dslib_atomic_decU32 var_input pValue:^udint; end_var var_output retcode:udint; end_var
  
AtomicUDecRetry
  LDREX r1, [pValue]
  SUBS r3, r1, #1
  STREX r2, r3, [pValue]
  CMP r2, #0
  BNE AtomicUDecRetry
  MOV r0, r1

END_FUNCTION
#endif



// ************************************************************************************************
// ************************************************************************************************
// systime und sysdate 
// ************************************************************************************************
// ************************************************************************************************
function global __cdecl dslib_get_systime var_output retcode : udint; end_var
  var
    xtime : SYSTIME;
  end_var
  
  OS_SSR_GetSysTime(#xtime);
  retcode := xtime.wHour;
  retcode := (retcode shl 8) or xtime.wMinute;
  retcode := (retcode shl 8) or xtime.wSecond;
  retcode := (retcode shl 8); 
  
end_function

function global __cdecl dslib_set_systime var_input ltime : udint; end_var var_output retcode : udint; end_var
  var
    xtime : SYSTIME;
  end_var
  
  retcode := 0;
  
  ltime := (ltime shr 8);
  xtime.wSecond := to_uint(ltime and 16#FF);
  ltime := (ltime shr 8);
  xtime.wMinute := to_uint(ltime and 16#FF);
  ltime := (ltime shr 8);
  xtime.wHour := to_uint(ltime and 16#FF);
  
  if((xtime.wSecond < 60) & (xtime.wMinute < 60) & (xtime.wHour < 24)) then
    OS_SSR_SetSysTime(#xtime);
    retcode := 1;
  end_if;
  
end_function

function global __cdecl dslib_get_sysdate var_output retcode : udint; end_var
  var
    xdate : SYSDATE;
  end_var
  
  OS_SSR_GetSysDate(#xdate);
  retcode := xdate.wYear;
  retcode := (retcode shl 4) or xdate.wMonth;
  retcode := (retcode shl 8) or xdate.wDay;
  retcode := (retcode shl 4) or xdate.wDayOfWeek;
  
end_function

function global __cdecl dslib_set_sysdate var_input ldate : udint; end_var var_output retcode : udint; end_var
  var
    xdate : SYSDATE;
  end_var

  ldate := ldate shr 4;
  xdate.wDay := to_uint(ldate and 16#FF);
  ldate := ldate shr 8;
  xdate.wMonth := to_uint(ldate and 16#0F);
  ldate := ldate shr 4;
  xdate.wYear := to_uint(ldate and 16#FFFF);

  OS_SSR_SetSysDate(#xdate);
  retcode := 1;

end_function

function global __cdecl dslib_thread_delay_us var_input duration:udint; end_var

  if(mt_api <> NIL) then
    if(mt_api^.version >= 16#00010004) then
      OS_MT_DELAYUS(duration);
    else
      duration := (duration + 500) / 1000;
      if(duration = 0) then
        duration := 1;
      end_if;
      OS_MT_DELAY(duration);
    end_if;
  end_if;
  
end_function

// ************************************************************************************************
// ************************************************************************************************
// Name of SERVER by lasalid 
// ************************************************************************************************
// ************************************************************************************************
function dslib_get_server_name var_input pcls:^ClsHdr; pserverno : ^udint; end_var var_output retcode : ^char; end_var
  var 
    pchc : ^ClsHdrConst;
    ppcd : ^ChDsc;
    svno : udint;
  end_var

  retcode := NIL;

  if(pcls <> NIL) then
    if(pcls^.pBaseCls <> NIL) then // suche in basisklasse
      retcode := dslib_get_server_name(pcls^.pBaseCls, pserverno);
    end_if;
  
    if(retcode = NIL) then
      pchc := pcls^.pDsc;
#if 1
      (*
        Anm. praerw:
        Diese Implementierung geht davon aus, dass im Objekt immer zuerst die Server und dann 
        die Clients angeordnet sind. Bei einer Ableitung zuerst die Server + Clients der Basisklasse 
        und dann die der Subklasse.
        Ich gehe davon aus, dass sich das nicht mehr ändern wird. 
        Wenn man auf Nummer sicher gehen will, dann muss man den Offset im Objekt als Parameter 
        übergeben und in einer Schleife alle Channel Deskriptoren durchgehen bis man den Channel 
        mit dem richtigen Offset gefunden hat. Dieser Code ist im #else Zweig zu finden.
      *)
      svno := pserverno^;
      
      if(svno < pchc^.nSvrChs) then
        ppcd := #pcls^.aDscChs[svno];
        if(ppcd <> NIL) then
          retcode := #ppcd^.pDsc^.Name.Text[0];
        end_if;
      else
        pserverno^ := svno - pchc^.nSvrChs - pchc^.nCltChs;
      end_if;
#else
      (* Das ist die Variante mit dem Suchen nach dem richtigen Offset in den Channel Deskriptoren *)
      for svno := 0 to pchc^.nSvrChs - 1 do
        ppcd := #pcls^.aDscChs[svno];
        if ppcd^.pDsc^.uiOff = uiOff then
          retcode := #ppcd^.pDsc^.Name.Text[0];
          exit;
        end_if;
      end_for;

#endif
    end_if;
  end_if;

end_function 

function global __cdecl dslib_get_serverlabel var_input dst:^char; lasalid : udint; end_var var_output retcode : udint; end_var
  VAR
  	ph : ^ONEOBJ; 
    nox, diff, diffmax : udint;
    pobj : ^Obj;
    pa, pah : ^char;
  END_VAR

  retcode := 0;
  dst^    := 0;

  if((pmapper <> NIL) & (lasalid <> 0)) then
  
    // Beim Zugriff auf pmapper (CRC Map Datenstruktur) muss ein Mutex verwendet werden, 
    // sonst ist die Funktion nicht thread-safe.
    LDR_LockCRCMap();
  
    pobj    := NIL;
    diffmax := 16#FFFFFFFF;
    ph      := #pMapper^.OBJs[0];
    nox     := pmapper^.nr;
    
    while(nox > 0) do
      nox -= 1;
      
      if(lasalid >= ph^.pOBJ$udint) then
        diff := lasalid - ph^.pOBJ$udint;
        if(diff < diffmax) then
          diffmax := diff;
          pobj := ph^.pOBJ;
          if(diff = sizeof(Obj)) then
            nox := 0;
          end_if;
        end_if;
      end_if;
      
      ph += sizeof(ONEOBJ);
      
    end_while;

    LDR_UnlockCRCMap();
    
    if(pobj <> NIL) then
      // objekt gefunden, suche jetzt den zugehörigen server
      nox := (lasalid - (pobj$udint + sizeof(Obj))) / 12; 
      pa  := dslib_get_server_name(pobj^.pClsHdr, #nox);
      
      if(pa <> NIL) then
        pah  := #pobj^.pObjDsc^.SymName.Text[0];
        nox  := _strlen(pah);
        _memcpy(dst, pah, nox);
        dst  += nox;
        dst^ := '.';
        _strcpy(dst+1, pa);
        retcode := 1;
      end_if;
      
    end_if;
  end_if;

end_function

// ************************************************************************************************
// ************************************************************************************************
// NUM-SERVER access 
// ************************************************************************************************
// ************************************************************************************************
function global __cdecl dslib_call_write_method var_input lasalid : udint; value : udint; end_var var_output retcode : udint; end_var

  retcode := ACCESS_DENIED;
  
  if(lasalid <> 0) then
    retcode$udint := WR(lasalid$^void, value$dint);
  end_if;
  
end_function

function global __cdecl dslib_call_read_method var_input lasalid : udint; end_var var_output retcode : udint; end_var

  retcode := 0;
  
  if(lasalid <> 0) then
    retcode$dint := RD(lasalid$^void);
  end_if;
  
end_function


// ************************************************************************************************
// ************************************************************************************************
// NewInst aufruf 
// ************************************************************************************************
// ************************************************************************************************
#ifndef P_IMMED
#define P_IMMED 0
#endif
#ifndef P_VARIA
#define P_VARIA 1
#endif
#ifndef P_COMMA
#define P_COMMA 7
#endif
#ifndef P_USER_STREAM 
#define P_USER_STREAM  19
#endif
#ifndef P_EOL 
#define P_EOL 8
#endif

function dsparse_icmd_stream var_input psrc:^usint; length:dint; end_var var_output retcode : bool; end_var
VAR
	pdst : ^usint;
  cmd : usint;
END_VAR

  retcode := true;
  pdst    := psrc;
  
  while(length > 0) do
    cmd    := psrc^;
    psrc   += 1;
    length -= 1;
    
    case cmd of 
      P_USER_STREAM:
        _memmove(pdst, psrc, length$udint);
        length := 0;
      P_IMMED:
        pdst$^udint^ := psrc$^udint^;
        length -= 4;
        psrc += 4;
        pdst += 4;
      P_VARIA:
        pdst$^udint^ := dslib_call_read_method(psrc$^udint^);
        length -= 4;
        psrc += 4;
        pdst += 4;
      P_COMMA:
      P_EOL:
        length := 0;
      else
        length := 0;
        retcode := false;
    end_case;
  end_while;
  
end_function

function global __cdecl dslib_call_new_inst_direct var_input lasalid:udint; ppara:^void; pResult:^results; end_var var_output retcode : udint; end_var
  VAR
	  psvr : ^VirtualBase;
    state : iprStates;
  END_VAR

  retcode := ERROR; // iprStates
  
  if(lasalid <> 0) then
    psvr    := lasalid$^VirtualBase;
    state   := psvr^.NewInst(ppara$^CmdStruct, pResult);
    retcode := to_udint(state);
  end_if;
  
end_function

function global __cdecl dslib_call_new_inst var_input pdata:^usint; datalength:udint; pResult:^results; end_var var_output retcode : udint; end_var
  VAR
//    psvr : ^VirtualBase;
//    state : iprStates;
//    ppara : ^CmdStruct;
    lasalid : udint;
  END_VAR

  lasalid := pdata$^udint^;
  retcode := ERROR; // iprStates
  
  if(dsparse_icmd_stream(pdata+6, datalength$dint) = true) then
    retcode := dslib_call_new_inst_direct(lasalid, pdata+4, pResult);
  
//    if(lasalid <> 0) then
//      psvr    := lasalid$^VirtualBase;
//      ppara   := pdata + 4;
//      state   := psvr^.NewInst(ppara, pResult);
//      retcode := to_udint(state);
//    end_if;
  end_if;
 
end_function

// ************************************************************************************************
// ************************************************************************************************
// CLIENT access 
// ************************************************************************************************
// ************************************************************************************************
function global __cdecl dslib_read_client_data var_input lasalid : udint; end_var var_output retcode : udint; end_var
  VAR
    pu32 : ^udint;
  END_VAR;  

  retcode := 0;

  if(lasalid <> 0) then
    pu32 := lasalid$^udint;
    pu32 += 4;
    retcode := pu32^; // von client.data lesen 
  end_if;
  
end_function

function global __cdecl dslib_write_client_data var_input lasalid : udint; value : udint; end_var var_output retcode : udint; end_var
  VAR
    pu32 : ^udint;
  END_VAR;  

  retcode := 0;

  if(lasalid <> 0) then
    pu32  := lasalid$^udint;
    pu32  += 4;
    pu32^ := value; // auf client.data schreiben 
    retcode := value;
  end_if;
  
end_function


// ************************************************************************************************
// ************************************************************************************************
// STRING-SERVER access 
// ************************************************************************************************
// ************************************************************************************************
function global __cdecl dslib_read_string var_input pdst:^void; dstsize:udint; pchrsize:^udint; lasalid:udint; end_var var_output retcode:^void; end_var
  VAR
  	pstr : ^StringInternal;
    len, chrsize : udint;
  END_VAR

  retcode := pdst;
  
  if(lasalid <> 0) then
    pstr    := lasalid$^StringInternal;
    chrsize := pstr^.TxtGetCharSize();
    len     := pstr^.TxtLen() + 1;
  
    if((len * chrsize) > dstsize) then
      retcode := dslib_malloc(len * chrsize);
      if(retcode <> NIL) then
        dstsize := len * chrsize;
      end_if;
    end_if;
  
    pstr^.TxtGet(retcode, dstsize);
  
    pchrsize^ := chrsize;
  else
    retcode$^usint^ := 0;
    pchrsize^       := 1;
  end_if;
  
end_function

function global __cdecl dslib_write_string var_input lasalid:udint; ptxt:^void; chrsize:udint; end_var var_output retcode:udint; end_var
  VAR
  	pstr : ^StringInternal;
  END_VAR

  retcode := 0;
  
  if(lasalid <> 0) then
    pstr := lasalid$^StringInternal;
    if(pstr^.TxtSet(chrsize, ptxt, chrsize) = true) then
      retcode := 1;
    end_if;
  end_if;
  
end_function

function global __cdecl dslib_get_string_crc var_input lasalid:udint; end_var var_output retcode:udint; end_var
  VAR
  	pstr : ^StringInternal;
  END_VAR

  retcode := 0;
  
  if(lasalid <> 0) then
    pstr    := lasalid$^StringInternal;
    retcode := pstr^.GetCRC();
  end_if;
  
end_function

// ************************************************************************************************
// ************************************************************************************************
// DATA-AT access 
// ************************************************************************************************
// ************************************************************************************************
function global __cdecl dslib_read_dataat var_input dst:^void; lasalid:udint; datasize:udint; end_var var_output retcode:udint; end_var
VAR
	p8 : ^usint;
	p16 : ^uint;
	p32 : ^udint;
END_VAR

  retcode := 0;
  
  if(lasalid <> 0) then
    retcode := 1;
    if(datasize = 1) then
      p8          := lasalid$^usint;
      dst$^usint^ := p8^;
    elsif(datasize = 2) then
      p16         := lasalid$^uint;
      dst$^uint^  := p16^;
    elsif(datasize = 4) then
      p32         := lasalid$^udint;
      dst$^udint^ := p32^;
    else
      _memcpy(dst, lasalid$^void, datasize);
    end_if;
  end_if;
  
end_function

function global __cdecl dslib_write_dataat var_input lasalid:udint; src:^void; datasize:udint; end_var var_output retcode:udint; end_var
VAR
	p8 : ^usint;
	p16 : ^uint;
	p32 : ^udint;
END_VAR

  retcode := 0;

  if(lasalid <> 0) then
    if(datasize = 1) then
      p8   := lasalid$^usint;
      p8^  := src$^usint^;
    elsif(datasize = 2) then
      p16  := lasalid$^uint;
      p16^ := src$^uint^;
    elsif(datasize = 4) then
      p32  := lasalid$^udint;
      p32^ := src$^udint^;
    else
      _memcpy(lasalid$^void, src, datasize);
    end_if;
  
    retcode := 1;
  end_if;
  
end_function

function global __cdecl dslib_write_dataat_part var_input pdata:^udint; lasalid:udint; value:udint; datasize:udint; end_var var_output retcode:udint; end_var
VAR
	p8 : ^usint;
	p16 : ^uint;
	p32 : ^udint;
  mask : udint;
END_VAR

  retcode := 0;
  pdata^  := 0;

  if(lasalid <> 0) then

    mask := value and 16#7FFFFFFF; 
    if(mask = 0) then
      mask := 16#80000000;
    end_if;  

    if((value and 16#80000000) = 0) then
      mask := (16#FFFFFFFF xor mask);
    end_if;
  
    if(datasize = 1) then
      p8 := lasalid$^usint;
      if(value and 16#80000000) then
        p8^ := p8^ or mask$usint;
      else
        p8^ := p8^ and mask$usint;
      end_if;
      pdata^ := p8^;
      retcode := 1;
    elsif(datasize = 2) then
      p16 := lasalid$^uint;
      if(value and 16#80000000) then
        p16^ := p16^ or mask$uint;
      else
        p16^ := p16^ and mask$uint;
      end_if;
      pdata^ := p16^;
      retcode := 1;
    elsif(datasize = 4) then
      p32 := lasalid$^udint;
      if(value and 16#80000000) then
        p32^ := p32^ or mask;
      else
        p32^ := p32^ and mask;
      end_if;
      pdata^ := p32^;
      retcode := 1;
    end_if;
  end_if;
  
end_function

function global __cdecl dslib_get_dataat_crc var_input lasalid:udint; datasize:udint; end_var var_output retcode:udint; end_var
VAR
	p8 : ^usint;
	p16 : ^uint;
	p32 : ^udint;
END_VAR
  
  retcode := 0;
  
  if(lasalid <> 0) then
    if(datasize = 1) then
      p8      := lasalid$^usint;
      retcode := p8^;
    elsif(datasize = 2) then
      p16     := lasalid$^uint;
      retcode := p16^;
    elsif(datasize = 4) then
      p32     := lasalid$^udint;
      retcode := p32^;
    else
      retcode := CRC32_Buffer(lasalid$^void, datasize);
    end_if;
  end_if;
  
end_function

// ************************************************************************************************
// ************************************************************************************************
// SystemVar access 
// ************************************************************************************************
// ************************************************************************************************
function global __cdecl dslib_get_dsccrc var_output retcode:udint; end_var

  retcode := ops.udDescCRC;

end_function

function global __cdecl dslib_get_loaderversion var_output retcode:udint; end_var

  retcode := ops.uiLoaderVersion$udint;

end_function

FUNCTION global __cdecl dslib_get_opsysversion var_output retcode : udint; end_var

  retcode := _RTOSVERSION; 

END_FUNCTION

function global __cdecl dslib_get_cpustate var_output retcode : udint; end_var

  retcode := _runstatus;

end_function

function global __cdecl dslib_get_cpuload var_output retcode : udint; end_var

  retcode := _cpuload;

end_function

function global __cdecl dslib_tabsolute var_output retcode:udint; end_var

  retcode := ops.tAbsolute;
  
end_function 

function global __cdecl dslib_is_x86 var_output retcode:udint; end_var

 #ifdef _LSL_TARGETARCH_X86
  retcode := 1;
 #else  
  retcode := 0;
 #endif

end_function 

#ifndef LSL_EQUIPMENT_VALID
 #define LSL_EQUIPMENT_VALID 0x80000000
#endif
#ifndef LSL_OS_LINUX
 #define LSL_OS_LINUX        0x00040000
#endif
#ifndef LSL_PLATFORMMASK    
 #define LSL_PLATFORMMASK    0x0000FF00
#endif
#ifndef LSL_OS_SALAMANDER
 #define LSL_OS_SALAMANDER   0x00000200
#endif
#ifndef LSL_GECKO
 #define LSL_GECKO           0x00000300
#endif

FUNCTION global __cdecl dslib_is_lars var_output retcode:udint; end_var
 #ifdef _LSL_TARGETARCH_X86
  VAR
    val : udint;
  END_VAR
 #endif
  
  retcode := 0;
  
 #ifdef _LSL_TARGETARCH_X86
  val := _WHOAMI and 16#0FFFFFFF;
  if((val = 16#1B) | (val = 16#1C) | (val = 16#1D) | (val = 16#1E)) then
    retcode := 1;
  end_if;
 #endif
 
END_FUNCTION

FUNCTION global __cdecl dslib_is_gecko var_output retcode:udint; end_var
  VAR
  	ptr : ^udint;
    val : udint;
  END_VAR

  retcode := 0;
  ptr     := #_RealMaximumTime;
  ptr     += 16#F44;
  val     := ptr^;
  
  if((val and (LSL_OS_LINUX or LSL_EQUIPMENT_VALID)) = (LSL_OS_LINUX or LSL_EQUIPMENT_VALID)) then
    val := val and LSL_PLATFORMMASK;
    if(val = LSL_GECKO) then
      retcode := 1;
    end_if;
  end_if;

END_FUNCTION

FUNCTION global __cdecl dslib_is_salamander var_output retcode:udint; end_var
  VAR
  	ptr : ^udint;
    val : udint;
  END_VAR
  
  retcode := 0;
  ptr := #_RealMaximumTime;
  ptr += 16#F44;
  val := ptr^;

  if((val and (LSL_OS_LINUX or LSL_EQUIPMENT_VALID)) = (LSL_OS_LINUX or LSL_EQUIPMENT_VALID))  then
    val := val and LSL_PLATFORMMASK;
    if(val = LSL_OS_SALAMANDER) | (val = 0) then
      retcode := 1;
    end_if;
  end_if;

END_FUNCTION

FUNCTION global __cdecl dslib_is_linux var_output retcode:udint; end_var
  VAR
  	ptr : ^udint;
  END_VAR
  
  ptr := #_RealMaximumTime;
  ptr += 16#F44;
  retcode := ((ptr^) and (LSL_OS_LINUX or LSL_EQUIPMENT_VALID)) = (LSL_OS_LINUX or LSL_EQUIPMENT_VALID);

END_FUNCTION

FUNCTION global __cdecl dslib_get_sysinfo var_output retcode:udint; end_var
  // Auf den unteren 16 bit wird Information abgelegt
  // Die oberen 16 Bit geben Auskunft welche der unteren 16 Bits aktuell gültig sind (zukünftige Erweiterung möglich)

  // Bit 0 ..... 1 wenn x86 Architektur
  // Bit 1 ..... 1 wenn LARS
  // Bit 2 ..... 1 wenn LINUX
  // Bit 3 ..... 1 wenn Salamander
  // Bit 4 ..... 1 wenn Gecko

  retcode := 16#001F0000; // aktuelle Gültigkeitsmaske
  if(dslib_is_x86() <> 0)        then retcode := retcode or 16#0001; end_if;
  if(dslib_is_lars() <> 0)       then retcode := retcode or 16#0002; end_if;
  if(dslib_is_linux() <> 0)      then retcode := retcode or 16#0004; end_if;
  if(dslib_is_salamander() <> 0) then retcode := retcode or 16#0008; end_if;
  if(dslib_is_gecko() <> 0)      then retcode := retcode or 16#0010; end_if;
  
END_FUNCTION


// ************************************************************************************************
// ************************************************************************************************
// LasalId 
// ************************************************************************************************
// ************************************************************************************************
function global __cdecl dslib_get_lasal_id var_input pchmod : ^usint; label : ^char; end_var var_output retcode : udint; end_var
VAR
	obj_ch : ^void; //^Virtual_base;
  obj : ^virtualbase;
  gid : CHMODE;
END_VAR
  
  retcode  := 0;
  
  gid := _LookUpEmbed_C(label, #obj_ch, #obj);
  case(gid) of
    _CH_OBJ, _CH_CMD, _CH_SVR, _CH_SVR_OBJ:
      retcode := obj_ch$udint;
      pchmod^ := 1;
    _CH_CLT_CMD, _CH_CLT_DATA, _CH_CLT_OBJ: // access to client
      retcode := obj_ch$udint;
      pchmod^ := 2;
    else
      retcode := OS_LNK_GetAddress_Variable(label)$UDINT;
      pchmod^ := 3;
  end_case;
  
  if(retcode = 0) then
    pchmod^ := 0;
  end_if;
  
end_function



// ************************************************************************************************
// ************************************************************************************************
// TcpIp 
// ************************************************************************************************
// ************************************************************************************************

function global __cdecl dslib_tcp_create_socket var_output retcode:dint; end_var
  VAR
  	onoff : udint; // 8D-3385
  END_VAR

  retcode := OS_TCP_USER_SOCKET();

  if(retcode >= 0) then  
    onoff := 0;
    dslib_tcp_set_socket_option(retcode, SOL_SOCKET, SO_NAGLE, #onoff, sizeof(onoff));
    dslib_tcp_set_socket_option(retcode, SOL_SOCKET, SO_DELAYED_ACK, #onoff, sizeof(onoff));
  end_if;

end_function

function global __cdecl dslib_tcp_recv var_input sock_clt:dint; pdst:^void; dstsize:udint; end_var var_output retcode:dint; end_var

  retcode := OS_TCP_USER_RECV(sock_clt, pdst$^char, dstsize, 0, 0);

end_function

function global __cdecl dslib_tcp_send var_input sock_clt:dint; pdata:^void; datasize:udint; end_var var_output retcode:dint; end_var
  
  retcode := OS_TCP_USER_SEND(sock_clt, pdata$^char, datasize, 0, 0);
  
end_function

function global __cdecl dslib_close_socket var_input sock:dint; end_var

  OS_TCP_USER_CLOSESOCKET(sock, 0);

end_function

function global __cdecl dslib_tcp_set_socket_option var_input socket:dint; level:dint; option_name:dint; option_value:^void; optionlen:dint; end_var var_output retcode:dint; end_var
    
  retcode :=  OS_TCP_USER_SETSOCKOPT(socket, level, option_name, option_value$^char, optionlen);

end_function

function global __cdecl dslib_tcp_user_listen var_input socket:dint; localport:udint; backlogsize:udint; end_var var_output retcode:dint; end_var

  retcode := OS_TCP_USER_LISTEN(socket, localport, backlogsize);

end_function

function global __cdecl dslib_tcp_user_accept var_input socket:dint; timeout_ms:udint; end_var var_output retcode:dint; end_var

  retcode := OS_TCP_USER_ACCEPT(socket, timeout_ms);
  
end_function


function global __cdecl dslib_tcp_user_connect var_input socket:dint; localport:udint; ipAddress:^char; port:udint; timeout_ms:udint; end_var var_output retcode:dint; end_var
  
  retcode := OS_TCP_USER_CONNECT(socket, localport, ipAddress, port, timeout_ms);
  
end_function

function global __cdecl dslib_getenvvar VAR_INPUT name:^char; value:^char; sizeof_value:udint; END_VAR VAR_OUTPUT retcode:dint; END_VAR
  VAR
  	pSysenv : ^OS_SYSENV;
  END_VAR

  retcode := 0;
  
  if((sizeof_value > 0) & (value <> nil) & (name <> nil)) then
    value^  := 0; // ensure 0-termination
    pSysenv := nil;
    OS_CILGet("SYSENV", #pSysenv$void);
    if pSysenv <> nil then
      retcode := OS_GETENVVAR(pSysenv, name, value, sizeof_value);
    end_if;
  end_if;
  
end_function
