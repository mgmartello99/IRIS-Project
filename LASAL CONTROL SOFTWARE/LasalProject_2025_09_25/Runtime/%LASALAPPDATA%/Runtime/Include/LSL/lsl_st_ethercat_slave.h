(****************************************************************************************)
(*                                                                                      *)
(* lsl_st_ethercat_slave.h                                                              *)
(*                                                                                      *)
(* enthält die Definitionen für das EtherCAT Slave Interface IETHERCATSLAVE             *)
(*                                                                                      *)
(*                                                                                      *)
(*                                                                                      *)
(*                                                                                      *)
(****************************************************************************************)

//HFILTER:1
#ifndef _LSL_ST_ETHERCAT_SLAVE_H
//HFILTER:
//HFILTER:2
#pragma once
//HFILTER:
#define _LSL_ST_ETHERCAT_SLAVE_H

//
// Defines
//

// Interface
#define INTERFACE_ETHERCATSLAVE                 "IETHERCATSLAVE"

// Ergebniscodes
#define ETHERCATSLAVE_OK                          0
#define ETHERCATSLAVE_NOT_FOUND                  -1
#define ETHERCATSLAVE_CALLBACK_NOT_INSTALLED     -2
#define ETHERCATSLAVE_INVALID_PARAMETER          -3 


TYPE
    LSL_ETHERCAT_SLAVE   :STRUCT
        udVersion        : UDINT;
        // Version 0x00010000
        SlaveInit        : pVoid;
        SlaveInstallCb   : pVoid;
        SlaveStart       : pVoid;
        SlaveStopp       : pVoid;
        // reserved for OS
        OS_reserved0     : pVoid;
    END_STRUCT;
END_TYPE

// Initialisiert den EtherCAT Slave und liefert Startadresse und Größe des Speicherbereichs für den EtherCAT-Slave
FUNCTION GLOBAL __cdecl P_ETHERCAT_SLAVE_INIT
VAR_INPUT
    pudAddress   : ^UDINT;
    pudLen       : ^UDINT;
END_VAR
VAR_OUTPUT
    retval       : DINT;
END_VAR;

// Installiert eine Callback-Funktion, die nach jedem EtherCAT-Slave-Event aufgerufen wird
// Deklaration Callback-Funktion:
// FUNCTION GLOBAL __CDECL UserCallback
// VAR_INPUT
//   pUserPara  : pVoid;
// END_VAR
FUNCTION GLOBAL __cdecl P_ETHERCAT_SLAVE_INSTALL_CB
VAR_INPUT
    pUserFct     : pVoid;
    pUserParam   : pVoid;
END_VAR
VAR_OUTPUT
    retval       : DINT;
END_VAR;

// Startet die EtherCAT-Slave Verarbeitung (Interrupts werden aktiviert)
FUNCTION GLOBAL __cdecl P_ETHERCAT_SLAVE_START
VAR_OUTPUT
    retval       : DINT;
END_VAR;

// Stoppt die EtherCAT-Slave Verarbeitung (Interrupts werden deaktiviert)
FUNCTION GLOBAL __cdecl P_ETHERCAT_SLAVE_STOPP
VAR_OUTPUT
    retval       : DINT;
END_VAR;

#define OS_ETHERCAT_SLAVE_VERSION lsl_ethercat_slave^.udVersion

#define OS_ETHERCAT_SLAVE_INIT(p1,p2) lsl_ethercat_slave^.SlaveInit $ P_ETHERCAT_SLAVE_INIT(p1,p2)
#define OS_ETHERCAT_SLAVE_INSTALL_CB(p1,p2) lsl_ethercat_slave^.SlaveInstallCb $ P_ETHERCAT_SLAVE_INSTALL_CB(p1,p2)
#define OS_ETHERCAT_SLAVE_START() lsl_ethercat_slave^.SlaveStart $ P_ETHERCAT_SLAVE_START()
#define OS_ETHERCAT_SLAVE_STOPP() lsl_ethercat_slave^.SlaveStopp $ P_ETHERCAT_SLAVE_STOPP()

//HFILTER:1
#endif //ifndef _LSL_ST_ETHERCAT_SLAVE_H
//HFILTER:
