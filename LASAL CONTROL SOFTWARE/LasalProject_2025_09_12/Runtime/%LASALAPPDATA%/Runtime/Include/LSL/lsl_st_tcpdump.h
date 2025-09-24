#pragma once

#define INTERFACE_TCPDUMP "TCPDUMP"

(* Fehlercodes der TCPDUMP Funkionen *)
#define TCPDUMP_ERROR_OS				      -1
#define TCPDUMP_ERROR_BUSY				    -2
#define TCPDUMP_ERROR_ARGS_TOO_LONG		-3
#define TCPDUMP_ERROR_INV_IFACE			  -4
#define TCPDUMP_ERROR_INV_PARAM			  -5
#define TCPDUMP_ERROR_INV_FILENAME		-6

(* Events für Start- und Stopcondition *)
#define TCPDUMP_EVENT_RUN        0x00000001 ///< trigger on RUN
#define TCPDUMP_EVENT_RESET      0x00000002 ///< trigger on reset
#define TCPDUMP_EVENT_ERROR      0x00000004 ///< trigger on error
#define TCPDUMP_EVENT_CDIASWD    0x00000008 ///< CDias Watchdog
#define TCPDUMP_EVENT_RUNTIME    0x00000010 ///< Runtime Error
#define TCPDUMP_EVENT_EVENT      0x00000020 ///< Tastatur od. Maus Event
#define TCPDUMP_EVENT_LSE0       0x00000040 ///< LSE Visu Event
#define TCPDUMP_EVENT_LSE1       0x00000080 ///< LSE Visu Event
#define TCPDUMP_EVENT_USER0      0x00000100 ///< Application User Event
#define TCPDUMP_EVENT_USER1      0x00000200 ///< Application User Event
#define TCPDUMP_EVENT_IMMED      0x40000000 ///< start now


TYPE
	LSL_TCPDUMP :STRUCT
		version             : UDINT;
    // version 1
    start               : pVoid;
    stop                : pVoid;
    status              : pVoid;
    // version 2
    start2              : pVoid;
	END_STRUCT;
END_TYPE

(**
 * Starten einer tcpdump Netzwerkaufzeichnung
 *
 * Beispiel:
 * 	 	TCPDUMP_start(
 * 	 		TCPDUMP_EVENT_IMMED, 	// startCond: start sofort
 * 	 		TCPDUMP_EVENT_ERROR, 	// stopCond: stop bei einem Fehler
 * 	 		1,						        // iface: IP 1
 * 	 		0,						        // sizeMB: default 4 MB
 * 	 		0,						        // count: 0 (=kein Limit bei der Anzahl der Pakete)
 * 	 		"c:\tcpdump.test",		// outfileAppl: c:\tcpdump.test[0..3]
 * 	 		"", 					        // exprfileAppl: kein Expressionfile
 * 	 		);
 *
 * @param startCond Oder-Verknüpfung von Events, bei denen tcpdump gestartet wird.
 * 		siehe defines TCPDUMP_EVENT_...
 * @param stopCond Oder-Verknüpfung von Events, bei denen tcpdump gestoppt wird.
 * 		siehe defines TCPDUMP_EVENT_...
 * @param iface Nummer des Interfaces bei dem die Aufzeichnung durchgeführt wird.
 * 		Der Wert 0 bezeichnet das erste vorhandene Interface, ansonsten wird die im Lasal
 * 		verwendete IP NUmmer verwendet (IP 1, ...)
 * @param sizeMB Größe der Ausgabefile in MB. Es werden 4 Ausgabefiles verwendet, die
 * 		in einer Art Ringbuffer beschrieben werden. Ein einzelnes Ausgabefile kann die
 * 		Größe eines vielfachen von 1 MB haben. D.h. die kleinste Größe ist 4 MB.
 * 		Der Parameter sizeMB wird auf ein vielfaches von 4 MB aufgerundet.
 * @param count Wenn >0, dann wird tcpdump nach der Aufzeichnung von count Paketen beendet.
 * @param outfileAppl Name des Ausgabefiles (ohne die angehängte Nummer am Ende).
 * 		Bei einem Leerstring wird das File C:\sysmsg\tcpdump verwendet.
 * 		Da 4 Ausgabefiles verwendet werden, wird am Ende die Zahl 0..3 angehängt.
 * @param exprfileAppl Name des Expressionfiles mit Filteranweisungen.
 * 		Bei einem Leerstring wird kein Expressionfile verwendet.
 * 		Mit Filteranweisungen kann gesteuert werden, welche Pakete aufgezeichnet
 * 		werden. Die Syntax der Filteranweisung ist in der tcpdump Dokumentation
 * 		beschrieben. Die Filteranweisungen können auch im Parameter argline
 * 		angegeben werden. Wenn ein Expressionfile verwendet wird, werden die
 * 		Filteranweisungen in argline ignoriert.
 * @return 0 wenn ok, sonst ein negativer Fehlercode
 *)
FUNCTION GLOBAL __cdecl P_TCPDUMP_Start
VAR_INPUT
  startCond : UDINT;
  stopCond : UDINT;
  iface : UDINT;
  sizeMB : UDINT;
  count : UDINT;
  outfile : ^USINT;
  exprfile : ^USINT;
END_VAR
VAR_OUTPUT
	retval      : DINT;
END_VAR;

(**
 * Starten einer tcpdump Netzwerkaufzeichnung, Version 2
 *
 * Beispiel:
 * 	 	TCPDUMP_start2(
 * 	 		TCPDUMP_EVENT_IMMED, 	// startCond: start sofort
 * 	 		TCPDUMP_EVENT_ERROR, 	// stopCond: stop bei einem Fehler
 * 	 		1,						        // iface: IP 1
 * 	 		0,						        // sizeMB: default 4 MB
 * 	 		0,						        // count: 0 (=kein Limit bei der Anzahl der Pakete)
 * 	 		"c:\tcpdump.test",		// outfileAppl: c:\tcpdump.test[0..3]
 * 	 		"", 					        // exprfileAppl: kein Expressionfile
 *      1954,                 // Filtern auf Port 1954
 * 	 		);
 *
 * @param startCond Oder-Verknüpfung von Events, bei denen tcpdump gestartet wird.
 * 		siehe defines TCPDUMP_EVENT_...
 * @param stopCond Oder-Verknüpfung von Events, bei denen tcpdump gestoppt wird.
 * 		siehe defines TCPDUMP_EVENT_...
 * @param iface Nummer des Interfaces bei dem die Aufzeichnung durchgeführt wird.
 * 		Der Wert 0 bezeichnet das erste vorhandene Interface, ansonsten wird die im Lasal
 * 		verwendete IP NUmmer verwendet (IP 1, ...)
 * @param sizeMB Größe der Ausgabefile in MB. Es werden 4 Ausgabefiles verwendet, die
 * 		in einer Art Ringbuffer beschrieben werden. Ein einzelnes Ausgabefile kann die
 * 		Größe eines vielfachen von 1 MB haben. D.h. die kleinste Größe ist 4 MB.
 * 		Der Parameter sizeMB wird auf ein vielfaches von 4 MB aufgerundet.
 * @param count Wenn >0, dann wird tcpdump nach der Aufzeichnung von count Paketen beendet.
 * @param outfileAppl Name des Ausgabefiles (ohne die angehängte Nummer am Ende).
 * 		Bei einem Leerstring wird das File C:\sysmsg\tcpdump verwendet.
 * 		Da 4 Ausgabefiles verwendet werden, wird am Ende die Zahl 0..3 angehängt.
 * @param exprfileAppl Name des Expressionfiles mit Filteranweisungen.
 * 		Bei einem Leerstring wird kein Expressionfile verwendet.
 * 		Mit Filteranweisungen kann gesteuert werden, welche Pakete aufgezeichnet
 * 		werden. Die Syntax der Filteranweisung ist in der tcpdump Dokumentation
 * 		beschrieben. Die Filteranweisungen können auch im Parameter argline
 * 		angegeben werden. Wenn ein Expressionfile verwendet wird, werden die
 * 		Filteranweisungen in argline ignoriert.
 *    Dieser Parameter kann nur verwendet werden, wenn der Parameter port 0 ist.
 * @param port Wenn hier ein Wert <> 0 angegeben wird, dann wird aus dieser Nummer 
 *    ein Filteranweisung erstellt, mit der auf den Port mit dieser Nummer gefiltert wird.
 *    Wenn ein Expressionfile angegeben ist, dann muss dieser Parameter 0 sein.
 * @return 0 wenn ok, sonst ein negativer Fehlercode
 *)
FUNCTION GLOBAL __cdecl P_TCPDUMP_Start2
VAR_INPUT
  startCond : UDINT;
  stopCond : UDINT;
  iface : UDINT;
  sizeMB : UDINT;
  count : UDINT;
  outfile : ^USINT;
  exprfile : ^USINT;
  port : UDINT;
END_VAR
VAR_OUTPUT
	retval      : DINT;
END_VAR;

(**
 * Stoppen einer tcpdump Netzwerkaufzeichnung
 *
 * Es wird eine gesetzte Startbedingung auf 0 zurückgesetzt und falls ein tcpdump
 * Prozess existiert, wird dieser beendet.
 *
 * @return 0 wenn ok, sonst ein negativer Fehlercode
 *)
FUNCTION GLOBAL __cdecl P_TCPDUMP_Stop
VAR_OUTPUT
	retval      : DINT;
END_VAR;

(**
 * Abfrage ob ein tcpdump Prozess existiert
 *
 * @return 1 wenn ein tcpdump Prozess existiert, sonst 0
 *)
FUNCTION GLOBAL __cdecl P_TCPDUMP_Status
VAR_OUTPUT
	retval      : DINT;
END_VAR;


// Version 1
#define OS_TCPDUMP_START(pTCPDUMP,p1,p2,p3,p4,p5,p6,p7) pTCPDUMP^.Start $ P_TCPDUMP_Start(p1,p2,p3,p4,p5,p6,p7)
#define OS_TCPDUMP_STOP(pTCPDUMP)                 pTCPDUMP^.Stop      $ P_TCPDUMP_Stop()
#define OS_TCPDUMP_STATUS(pTCPDUMP)               pTCPDUMP^.Status    $ P_TCPDUMP_Status()
// Version 2
#define OS_TCPDUMP_START2(pTCPDUMP,p1,p2,p3,p4,p5,p6,p7,p8) pTCPDUMP^.Start2 $ P_TCPDUMP_Start2(p1,p2,p3,p4,p5,p6,p7,p8)
