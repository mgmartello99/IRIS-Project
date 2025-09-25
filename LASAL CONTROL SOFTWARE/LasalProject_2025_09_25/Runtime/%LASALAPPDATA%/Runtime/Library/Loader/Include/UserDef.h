#ifndef __LSLPRJ_UNITS_H__
  #include <Units.h>
#endif
#define COMLINK_LASAL
#define DATASERVICE_LASAL
#define DATASERVICE_VERSION  4
#define COMLINK_TCP_SERVER
#define COMLINK_TCP_CLIENT
#define LOADER_4_LASAL2
//#define COMLINK_PG

// Max. number of CAN Comlink Channels, Range: 1..10
// !!! IMPORTANT: This value must be the same in all Comlink Nodes of a Comlink network !!!
#define COMLINK_CAN_COMCHS	5

// display on-screen trace messages
#define LDR_USETRACE
//#define LDR_TRACE_SUCCESS

// use retentive servers
#define LDR_RETSVR

// this define is used to activate cyclic check of SRAM
//#define CYCLIC_SRAM_CHECK
// reset sram when the chksum of a format-1 sram cell is invalid
//#define SRAM_REPORT_INVALID_CELL1
// The Write-Method of the following Server is called (if available), when the SRAM is invalid
#define SRAMALARM_SVR_NAME	"ProcTimer\Alarm1"

// This define is used to reduce the cpu-cycles of the tcp/ip processing in the loader, but the 
// downside of this define is an increased response time of the tcp/ip loader communication (comlink).
//#define LOADER_TCP_MODERATE

#define LSL_TEST
//#define FAR_OBJECT
//#define OTTOSPEED
FUNCTION GLOBAL __cdecl CriticalLoaderSectionStart;
FUNCTION GLOBAL __cdecl CriticalLoaderSectionStop;

#ifdef _LSL_TARGETARCH_ARM
	#undef _LSL_USECLISTI
#endif

// The following include file ist used to override the previous default settings.
#include "UserDefOV.h"

#define LOADER_REV_HI		16#22
#define LOADER_REV_LO		249
#define LOADER_REV			((LOADER_REV_HI * 256) + LOADER_REV_LO)
#define LOADERINFO      LSL_LDR_LoaderInfo_V22F9

// 02.02.249
//    - 8D-9332: Unberechtigte Fehlermeldung 'Name specifies channel with wrong type, expecting object channel!' 
//      bei der Abfrage eines Servers eines Advaned IO Elements entfernt.

// 02.02.248
//    - Fehlerbehebung: wenn nach dem Löschen des Srams und der sernum.dat das Projekt gestartet 
//      wurde, dann wurde die Datei sernum.dat nicht neu erstellt. Wenn dann die Speicherkarte in 
//      eine andere Steuerung gegeben wird, dann wird der Wechsel des Speichermediums nicht erkannt 
//      und der Sraminhalt könnte inkonsistent sein weil ramfile.dat nicht mit dem Sram Inhalt 
//      übereinstimmt.
//    - 8D-8632 Abfrage der Version der MMserverString Klasse korrigiert
//    - 8D-9735 Die Debugger Trace Meldung '..Lookup.lob: Name specifies channel with..' wurde 
//      so erweitert, dass auch der Name des Objekts, welcher den Fehler verursacht, ausgegeben wird.
//    - 8D-9818 Fehlerbehebung: die Stringübertragung via Multimaster war bei Stringlängen größer 
//      als 252 Bytes fehlerhaft. Es wurden an den 256 Byte Grenzen 4 Zeichen rausgeschnitten.
//    - 8D-10761 Erweiterung: Neue Funktionen LDR_RegisterAdvIoElWithNewinst, LDR_IsHWConnected
//      und LDR_IsSimConnected mit denen geprüft werden kann, ob bei einem Client eine Verbindung 
//      zu einem Hardware- oder Simulations-Server hinter einem Advanced IO Element besteht.

// 02.02.247
//    - P04323 Alarm-Kommunikation mit Dataservice wurde um Userdata (max.10kByte) erweitert

// 02.02.246
//    - 8D-8123 Fehlerbehebung: Wenn die Adresse eines Objekts bzw. Kanals mittels 
//      LookupEmbedded/I_GET_OBJ abgefragt wurde, kam es zu einer Exception wenn 
//      der Name ein Element (z.B. dData) eines Clients angibt, welcher mit einer 
//      OSInterface Klasse verbunden ist.
//    - 8D-7469 neue Funktionen um die Abarbeitung von Interpreterprogrammen zu steuern 
//      bzw. zu limitieren:
//      - IprMgr_SetMaxNbrIprCmds
//      - ProgIp_SetMaxNbrIprCmds

// 02.02.245
//    - 8D-6870 Wenn im Projekt mehrere Objekte mit gleicher CRC existieren, dann wurde bisher 
//      eine Warnung ausgegeben. Jetzt wird eine Fehlermeldung ausgegeben.

// 02.02.244
//    - P21056 Unterstützung von advanced IO Elementen
//    - P21056 Neue Loaderfunktion LDR_InstallPostInitCB_Add, mit der eine Callback-Methode installiert 
//      werden kann, die nach den Initmethoden als NewInst aufgerufen wird. Im Unterschied zu 
//      LDR_InstallPostInitCB wird hier die Callback-Methode in einer Liste eingetraten, anstatt den 
//      alten Wert zu überschreiben. Der Aufruf der erfolgt vor der mit LDR_InstallPostInitCB 
//      installierten Callback-Methode.


// 02.02.243
//    - Korrigiertes Binärformat von Linker-Dateien.

// 02.02.242
//    - 8D-7008 MathBSD-Funktionen, die REAL-Datentypen zurückgeben, haben eine spezielle ABI, 
//      die mit einem bekannten Compiler-Fehler kompatibel ist, der aber in LC2 C67 behoben 
//      wurde. Eine neue Vorwärtsdeklaration ist erforderlich, um die Binärkompatibilität 
//      sicherzustellen, wenn neuer LC2 Compiler die Library inkludiert.

// 02.02.241
//    - 8D-5363 Eine C Funktion mit MOD Operator "x % y" führt zu einem Code-Freeze falls 
//      "x > 2.147.483.647" und "y = 1".

// 02.02.240
//    - 8D-5834 Neue Funktion LDR_SetConfigFlags, mit der ein bestimmtes Verhalten des Loaders 
//      festgelegt werden kann. z.B. LDR_DO_NOT_USE_RAMEX_FOR_IPRPROG bewirkt, dass 
//      Interpreterprogramme nicht in Ramex- sondern in MerkerEx Objekten gespeichert werden.
//    - 8D-6072 Fehlerbehebung RamEx über Multimaster: 
//      Änderungen an einem über Multimaster angebundenen RamEx Objekt wurde wurden falsch 
//      übertragen, wenn der Offset der geänderten Daten größer als 0 ist. Die Daten wurden 
//      im Zielsystem auf einen falschen Offset geschrieben.

// 02.02.239
//    - 8D-5889 C Compiler stellt nicht alle Mathematikfunktionen aus ANSI C <math.h> zur Verfügung

// 02.02.238
//    - 8D-5356 Fehlerbehebung bei der Debuginterpreter Kommunikation.
//      Ein Debuginterpreterbefehl konnte unter ungünstigen Bedingungen 
//      (vor allem auf Multicore CPUs und bei hoher Realtime Auslastung) 
//      falsche Ergebnisse liefern.

// 02.02.237
//    - 8D-5014 Fehlerbehebung bei der MOD Operation.

// 02.02.236
//    - 8D-4379 Bei einem fehlenden ramfile.dat kam es zu einer Access Exception.
//      Dieser Fehler ist mit 02.02.235 (8D-4161) reingekommen.

// 02.02.235
//    - 8D-4161 Beim Sram Format 2 wurde das Limit von max. 49152 Ram Objekten nicht 
//      geprüft, und es kam zu einer Exception, wenn mehr als die max. Anzahl von Ram 
//      Objekten verwendet wurde.
//      Weiters wurde das durch das Ramfile.dat vorgegebene Limit aufgehoben. 
//      Es können im Format 2 jetzt mehr als 49152 Ram Objekte angelegt werden.

// 02.02.234
//    - 8D-3999 Die maximal mögliche Anzahl von Comlink Logins kann jetzt über die 
//      Umgebungsvariable COMLINK_MAX_LOGINS eingestellt werden (Wertebereich 1 bis 64). 
//      Bisher war dieser Wert mit define MAX_LOGINS auf 16 festgelegt. Ohne die 
//      Umgebungsvariable bleibt der Wert 16 bestehen.
//    - 8D-3199 Include-Guard __LSLPRJ_UNITS_H__ in UserDef.h hinugefügt, zur Vermeidung 
//      der Compilerwarnung [WARN] W 0145 ... Identical redefinition of '...'
//    - 8D-3971 In der Funktion Add_sRam_I (x86 Variante) wurden die Befehle CLI/STI zum 
//      Sperren/Freigeben der Interrupts entfernt, da diese Befehle auf Salamander CPUs 
//      nicht mehr möglich sind. Da Add_sRam_I nur in der Initphase aufgerufen wird, 
//      kann der Aufruf nur aus einem Thread erfolgen und es kann CLI/STI ersatzlos 
//      gestrichen werden.
//    - Der Workaround von SA26191,DS2087 (STI in FlushSramToDisk) wird auf RTK CPUs 
//      eingeschränkt, da ein STI bei Salamander nicht erlaubt ist und in diesem Fall 
//      auch nicht notwendig

// 02.02.233
//    - 8D-2885 Fehlerbehebung im Multimaster
//      Beim Schreiben von RamEx Daten mit einem Offset > 0 wurden auch die Daten 
//      von 0 bis zum Offset beschrieben.
//    - 8D-2756 Fehlerbehebung DataService, Funktion dslib_get_lasal_id:
//      LasalId Ermittlung für hinausgezogene erste Server funktioniert in der Loader 
//      Schnittstelle für den Dataservice nicht immer

// 02.02.232
//    - 8D-1732: TcpPort von Dataservice kann mittels Umgebungsvariable geändert werden
//      Bsp.: Autoexec.lsl SETENV DataServiceTcpPort 1305
//    - 8D-3385: Korrektur in Aufruf von dslib_set_socket_option()

// 02.02.231
//    - PRJ05920, Erweiterung Comlink:
//      Die max. Anzahl von serverseitigen Comlink Connections kann jetzt mit der Umgebungsvariable 
//      COMLINK_MAX_SERVER_CONNECTIONS eingestellt werden (z.B setenv COMLINK_MAX_SERVER_CONNECTIONS 40). 
//      Wenn die Umgebungsvariable nicht gesetzt ist, ist der Wert auf 33 eingestellt.
//    - PRJ05920, Fehlerbehebung Comlink:
//      In der Funktion SendNUnbuffered wurde in bestimmten Fällen der Timeoutwert nicht berücksichtigt, 
//      wodurch die Funktion nicht mehr verlassen wurde. Weiters wurde im Falle des OS_TCP_USER_SEND 
//      Rückgabewertes TCP_NOT_READY ein Delay von 1 Tick eingefügt, um die CPU Belastung zu reduzieren.

// 02.02.230
//    - 8D-1814 Fehlerbehebung in der dslib_get_serverlabel Funktion:
//      Wenn in der Basisklasse Clients existieren, dann wurde der Name eines Servers in 
//      der Subklasse nicht richtig ermittelt.
//    - 8D-1849 Fehlerbehebung Sram:
//      Bei x86 CPUs mit einer Sramgröße kleiner als 8k wurde, wenn das Format 1 verwendet 
//      wurde, auf den Speicher nach dem Sram geschrieben. Es wird jetzt generell bei einer 
//      Sramgröße kleiner als 8k standardmäßig das Format 2 verwendet.
//      ARM CPUs sind davon nicht betroffen weil hier bereits standardmäßig das Format 2 
//      verwendet wird, unabhängig von der Sramgröße.

// 02.02.229
//    - 8D-1406, SA59480 Fehlerbehebung im Multimaster+Comlink: 
//      Wenn eine Multimaster Verbindung geschlossen wurde, dann konnte durch den dabei 
//      verwendeten Alive-Checker Mechanismus ein bereits freigebener Speicher noch einmal 
//      beschrieben werden.
//    - 8D-918 Fehlerbehebung in der Refreshliste: Eine Änderung an einer 3 Byte globalen 
//      Datenstruktur in der Refreshliste wurde nicht erkannt
//    - 8D-1810: UserText + DataLink Kommunikation:
//      UserText bei Anmeldung via DataLink wird verarbeitet und kann von Applikation abgefragt werden.
//      nur additive Erweiterung, kein Risiko
//    - 8D-1866: neue LibraryFunktionen hinzugefügt: dslib_get_opsysversion(), dslib_is_x86(), dslib_is_lars(), dslib_is_gecko(), dslib_is_salamander(), dslib_is_linux()
//      DataLink um 2 neue Systemvariablen erweitern: 'ComVarNo_SysOpsysVersion' und 'ComVarNo_SysInfo'

// 02.02.228
//    - 8D-1052 Fehlerbehebung im Comlink: Es konnte zu einer Exception kommen, wenn 
//      bei einer Comlink TCPIP Verbindung ein LOGOUT gemacht wurde, während in einem 
//      anderen Thread noch eine Übertragung stattfand (z.B. TXCOMMAND)

// 02.02.227
//    - SWLSE2-2909 Bei der Kommunikation mit dem DataService ist das Empfangen von fragmentierten
//      Paketen fehlgeschlagen. Wurde behoben. Ebenfalls wird nun an den DataService, nach 
//      Verbindungsaufbau ein AlarmRecordEX-Commando abgesetzt welches alle aktiven Alarme an den
//      DataService sendet.

// 02.02.226
//    - DEVOS-1156 [SA 57156] Neue Funktion LDR_MM_StatNrConnConfigStr, mit der Multimaster 
//      IP Verbindungseinstellungen aus dem File c:\lsldata\stations.txt überschrieben werden 
//      können, ohne das File stations.txt zu ändern. Wenn ein mit dieser Funktion geänderter 
//      Wert entfernt wird, dann ist wieder die ursprüngliche Einstellung aktiv.
//      Die überschriebenen Werte werden im File c:\lsldata\station2.txt gespeichert.

// 02.02.225
//    - DEVOS-1212 Loader ist inkompatibel mit dem OP-Mode Alarmsystem.
//      In Kombination wird immer nur der erste Alarm korrekt in der Visu angezeigt, wenn mehrere 
//      Alarme in einem Zyklus auftreten.
//    - DEVOS-1077 [SA 54662] Fehlerbehebung: Wenn bei der Comlink Kommunikation kein Userheap 
//      mehr verfügbar war, dann gab es eine Access Exception im Loader.
//    - DEVOS-888 [SA 52349] Programminterpreter konnten in bestimmten Konstellationen nicht 
//      geladen werden, wenn Referenzen von einem Interpreterprogram in einer anderes existierten.

// 02.02.224
//	  - DEVOS-993 [SA 53783] Berechnung der Descriptor-CRC verbessert: Der Zeiger auf 
//      ClsHdrConst wird nicht mehr in die CRC mit eingerechnet. Stattdessen der Inhalt 
//      von ClsHdrConst. Dadurch hat die Codeadresse der LOBs keine Auswirkung mehr auf die CRC.

// 02.02.223
//	  - DEVOS-967 [SA 52460] Bei der Umsetzung dieses Datensatzes in der Version 02.02.221 
//      war ein Fehler enthalten, der hier korrigiert wurde.

// 02.02.222
//    - DEVOS-786, DEVOS-1000 die Änderung in 02.02.213 zur Verbesserung der Antwortzeiten bei 
//      Multimaster Verbindungen war fehlerhaft, da ein Mutex nicht initialisiert war und dadurch 
//      Fehler wegen fehlender Threadsicherheit entstehen konnten.

// 02.02.221
//	  - DEVOS-967 [SA 52460] Beim Aufruf eines unverbundenen Objektkanals wird eine Exception 
//      ausgelöst und eine Tracemeldung ausgegeben.
//    - DEVOS-1044 Beim Schreiben der Daten für retentive Server vom Typ File wurde das File 
//      c:\retsvr.dat nicht mit dem Flag ATT_COMMITTED geöffnet, was bei einem Salamander 
//      System dazu führen kann, dass sich geänderte Serverdaten nicht am Datenträger befinden, 
//      auch wenn LDR_IsAsnycFileOperationInProgress signalisiert, dass alle Retentive Server 
//      Fileoperationen abgeschlossen wurden. 

// 02.02.220
//	  - DS2352, SA20733/DS1777: Neue Umgebungsvariable SRAM_DISABLE_REORG_FORCE, mit der das Reorganisieren 
//      des Srams in jedem Fall verhindert wird, auch wenn das Sram ungültig ist. Anm.: Mit der bestehenden 
//      Umgebungsvariable SRAM_DISABLE_REORG wird das Reorganisieren nur bei einem gültigen Sram verhindert.

// 02.02.219
//	  - DEVOS-1032 DataService API Erweiterung implementiert ,sodass mehrere DataService-Instanzen verbunden sein können und
//	  - die Kommunikation seperat funktioniert.

// 02.02.218
//    - DEVOS-979 CRC Berechnung für Interpreterprogramme funktioniert jetzt auch für Programme größer als 64k.
//    - DEVOS-786 (Multimaster Antwortzeiten) Diese Änderung in 02.02.213 ist im Loader 02.02.215 irrtümlich nicht 
//      enthalten, sie wurde in dieser Version wieder übernommen

// 02.02.217 - Versionsnummer ist für eine mögliche Bugfixrelease einer früheren Version reserviert

// 02.02.216 - Versionsnummer wurde für eine Bugfixrelease einer früheren Version verwendet

// 02.02.215
//    - SWLSE2 - 2491 Implementierung in der Version 212 war fehlerhaft. Fehler sind behoben.
//
//
// 02.02.214
//    - DEVOS-928 Wert für die max. Anzahl von dyn. Refreshlisteneinträgen von 5000 auf 15000 
//      erhöht (#define MAX_DYNLIST_ENTRIES). Damit wird auch das Limit für die max. Anzahl 
//      vom Multimaster Servern von 5000 auf 15000 erhöht.

// 02.02.213
//    - DEVOS-786 Antwortzeiten bei Multimaster Verbindungen verbessert, wenn Verbindungen 
//      zu mehreren Station definiert sind und nicht alle Stationen erreichbar sind.

// 02.02.212
//    - SWLSE2-2190 Senden und Empfangen von fragmentierten Packeten des DataServices.
//    - SWLSE2-2183 Added Sendefunktion welche auf eine Response wartet.

// 02.02.211
//    - DEVOS-832 Loader Warnung W 0174 mit neuer Compilerversion eliminiert
//    - DEVOS-836 Neue Systemvariablen zur Überwachung des Resourcenverbrauchs im OS

// 02.02.210
//    - DEVOS-785 [SA 50206] Die in 02.02.197 durchgeführte Änderung DEVOS-418 (Wiederverwendung von 
//      Comlink Semaphoren) war fehlerhaft was dazu führte das Übertragungsfehler beim TXCOMMAND 
//      Befehl auftreten konnten.

// 02.02.209
//    - DEVOS-611 [SA 40071]: Beim Single-Steppen im Interpreter werden Kommentare jetzt übersprungen
//    - DEVOS-772 [SA 44093]: Neue globale Funktion ProgIp_GetSubStack, mit der der Interpreter Callstack 
//        zur Verfügung gestellt wird.

// 02.02.208
//    - DEVOS-762 Fehlerbehebung beim Multimaster: beim Aufruf der NewInst Methode über eine Multimaster 
//      Verbindung wurde in manchen Fällen immer ERROR als Returnwert zurückgegeben.

// 02.02.207
//    - DEVOS-705 Adressen von manchen Objekten wurden im Loader anhand des Namens nicht gefunden.
//      (z.B. beim I_GET_OBJ Kommando oder bei der _LookupEmbedded Funktion).

// 02.02.206
//    - SWLSE2-1696 DataService: Anpassung des Headers im RT-DS Protokoll
//    - SWLSE2-1835 DataService: Funktionen zum Senden vom Daten welche größer als 1024 byte sind, hinzugefügt

// 02.02.205
//    - Diese Version ging als Testversion zum Kunden raus.

// 02.02.204
//    - DEVOS-593 globale Funktion ProgIp_SetNextInstruction hinzugefügt

// 02.02.203
//    - DEVOS-587 Bei Verwendung eines Retentive-Server-Sram und fehlendem SRAM wurde die Fehlermeldung verbessert.

// 02.02.202
//    - DEVOS-549 [SA 42288]: Fehlerbehebung: NewInst communikation über Loader schlägt sporradisch fehl
//    - DEVOS-568: Fehlerbehebung: Wenn man ein Multimasterprojekt hat mit einem Multimaster MerkerEx 
//      Server, dann ist das Projekt nicht lauffähig, wenn eine neuere Version der MerkerEx (1.22) verwendet 
//      wird, als die, mit der der Loader compiliert wurde (1.20).
//    - Alle Globalen Funktionen aus Klasse DataService durch Präfix 'LDR_' erweitert

// 02.02.201
//    - SWLSE2-1464: DataService wurde in den Loader integriert
//    - DEVOS-446 [SA 41265]: Unterstützung eines verschlüsselten Connectionfiles hinzugefügt (prjcfg.lcf)

// 02.02.200
//    - DEVSW-63 [SA 37342]: Die Einstellung "SET FPUROUNDCONTROL" ändert die Berechnungen von einigen 
//		Funktionen. Code wurde hinzugefügt, um diese Nebenwirkungen zu verhindern.

// 02.02.199
//    - DEVOS-442: Fehlerbehebung Debuginterpreter: Es konnte der Fall eintreten, dass ein Kommunikationskanal 
//      des Debuginterpreters im Loader ungültig wird, während das Betriebssystem diesen Kanal 
//      weiterhin verwendet. Es funktionierten dann keine Debuginterpreterkommandos mehr auf diesem 
//      Kommunikationskanal. Diese Fehlersituation konnte dann eintreten, wenn 2 aufeinanderfolgende 
//      Debuginterpreterkommandos im Abstand von 60 Sekunden ausgeführt werden. 
//    - DEVSW-63 [SA 37342]: Funktion sin() liefert Falschen Wert. Die BSD Math Library wurde von 
//      https://svn.freebsd.org/base/head/lib/msun portiert und angepasst. Durch bestimmte Compiler Einstellung 
//      kann Lasal2 die BSD Funktionen benützen und bessere Ergebnisse bekommen. Fehler an der BSD Library ist 
//      typischerweise <1 ulp (Units in the Last Place) und Geschwindigkeit konnte durch optimierte 
//      Compilereinstellung verbessert werden.
//      Hinzugefügten Funktionen: bsd_acos, bsd_acosf, bsd_acosh, bsd_acoshf, bsd_asin, bsd_asinf, bsd_asinh, 
//      bsd_asinhf, bsd_atan, bsd_atan2, bsd_atan2f, bsd_atanf, bsd_atanh, bsd_atanhf, bsd_ceil, bsd_ceilf, 
//      bsd_cos, bsd_cosf, bsd_cosh, bsd_coshf, bsd_exp, bsd_exp2, bsd_exp2f, bsd_expf, bsd_expm1, bsd_expm1f, 
//      bsd_fabs, bsd_fabsf, bsd_floor, bsd_floorf, bsd_fmod, bsd_fmodf, bsd_frexp, bsd_frexpf, bsd_hypot, 
//      bsd_hypotf, bsd_log, bsd_log10, bsd_log10f, bsd_log1p, bsd_log1pf, bsd_logf, bsd_modf, bsd_modff, 
//      bsd_pow, bsd_powf, bsd_remainder, bsd_remainderf, bsd_remquo, bsd_remquof, bsd_round, bsd_roundf, 
//      bsd_scalbn, bsd_scalbnf, bsd_sin, bsd_sincos, bsd_sincosf, bsd_sinf, bsd_sinh, bsd_sinhf, bsd_sqrt, 
//      bsd_sqrtf, bsd_tan, bsd_tanf, bsd_tanh, bsd_tanhf, bsd_trunc und bsd_truncf.

// 02.02.198
//  Diese Versionsnummer ist reserviert für die Fehlerbehebung DEVOS-442 im Loader 2.2.193

// 02.02.197
//    - DEVOS-385 Unterstützung mehrerer Cores für Realtime und Cyclic
//    - DEVOS-418 Workaround für Salamander Fehler beim Löschen von Semaphoren:
//      Die bei einer Comlink Verbindung verwendeten Semaphoren werden beim Beenden 
//      der Verbindung nicht gelöscht, sondern behalten und dann beim nächsten Start einer 
//      Verbindung wiederverwendet.

// 02.02.196
//    - DEVOS-252[SA 39053] Ein Multimaster Objekt wurde nicht gefunden wenn ein Leerzeichen im 
//      Projektnamen der Zielstation enthalten war.
//    - DEVOS-346, SA 39886 Die in 02.02.194 durchgeführte Multimaster Änderung (DEVOS-299) konnte dazu 
//      führen, dass ein Runtime Fehler ausgelöst wird.

// 02.02.195
//    - DEVOS-315 - Die in SA30586, RM19 durchgeführte Änderung des Messung der verstrichenen Zeit 
//      in IprMgr::ProgSequences wurde optimiert. Es wurden die OS_GetMicroSec Aufrufe minimiert, 
//      um CPU Zeit einzusparen.
//    - DEVOS-316 - Sram: Bei den Loaderversionen 2.2.103 bis 2.2.194 wurde nicht reorganiaisert, wenn die 
//      Applikation nach dem Einschalten schon einmal gelaufen ist und sich der Projektname nicht geändert 
//      hat. Das hat aber den ungewünschten Effekt, dass die Descriptor-CRC im Reorg und im nicht-Reorg 
//      Fall unterschiedlich ist. Es wird deshalb jetzt wieder immer reorganisiert, damit die Descriptor-CRC 
//      in diesen beiden Fällen gleich ist.

// 02.02.194
//    - DEVOS-299 Multimaster:
//      - Die Threadsicherheit im Multimaster wurde verbessert (Codeabschnitte mit ComdefMutex geschützt).
//      - Fehlerbehebung beim Aufruf der Newinst-Methode eines Multimalster-Servers: Wenn der Aufruf 
//        erfolgte, während keine Onlineverbindung zum Server vorhanden war, dann lieferte die 
//        Newinst-Methode keinen Fehler und darauffolgende GetState Aufrufe lieferten dann immer BUSY 
//        zurück, d.h. der Befehl wurde nie fertig.
//      - Fehlerbehebung bei der Änderung des Verbindungsstrings: Wenn mit LDR_MM_StatNrConnSetStr 
//        eine Änderung des Verbindungsstrings angefordert wurde, dann wurde in der Zeit vom Aufruf 
//        der Funktion bis zur Änderung des Verbindungsstrings ein alter Status (möglicherweise > 0) 
//        zurückgegeben. Jetzt wird in dieser Zeit von LDR_MM_StatNrIsConnected der Wert 0 zurückgegeben.

// 02.02.193
//    - DEVOS-181: neue Funktion LDR_CallProgsequencesInCyclic, mit der der Aufruf des Programminterpreters 
//      vom Loader-Task in den Cyclic-Task verlagert werden kann ()
//    - DEVOS-182: Delayzeiten der Multimasterthreads wurden verringert, um die CPU Last zu reduzieren

// 02.02.192
//    - DEVOS-141, SA 38066: Multimaster: Fehlerbehebung und Trennung zwischen 4- und n-byte Server Implementation
//    - DEVOS-165: globale Systemvariable _onlineDiagFlags (AT % M 0120) zur Diagnose von 
//      Online-Kommunikationsproblemen hinzugefügt. Bit 1 wird vom Loader gesetzt, wenn die 
//      Connection der Refreshliste aufgrund eines Problems geschlossen wird.
//    - DEVOS-169: Communication: Die TCPIP Socketoptionen Delayed-Ack und Nagle werden jetzt auch 
//      explizit bei Accept-Sockets gesetzt, da diese beim Salamander nicht vom Listen-Socket 
//      übernommen werden.

// 02.02.191
//    - DS2088,SA26192: Ret.Server File: Die in 02.02.190 durchgeführte Änderung (beim Reboot warten, 
//      bis Schreiben ins File fertig), war beim Salamander Betriebssysteme nicht wirksam, da eine 
//      Versionsabfrage (09.03.080) falsch durchgeführt wurde. Weiters wurde ein Mutex verwendet, 
//      um einen kritischen Abschnitt in diesem Softwareteil zu schützen.
//    - SA30586, RM19 - Steuerungen mit einem Salamander Betriebssystem konnten beim Debuggen 
//      (Breakpoint im Realtime) einfrieren, wenn Programminterpreter in Verwendung sind. Der Grund 
//      fürs Einfrieren ist eine Warteschleife im Programminterpreter mit einer Zeitüberwachung mittels 
//      der Variablen ops.tAbsolute, die nicht aktualisisert wird, wenn der Realtime angehalten wird. 
//    - DS2680 - Comlink: Wenn ein Socket, der für das Senden der Änderungen in der Refreshliste 
//      verwendet wird, aufgrund eines zu grossen Füllstands des Sendebuffers geschlossen wird, 
//      dann wird dieses Ereignis jetzt im Logfile event19.log protokolliert.
//    - DEVOS-107:globale Systemvariable _sramDiagFlags (AT % M 011C) hinzugefügt. In dieser Variable werden 
//      mit einzelnen Bits aufgetretene Sram- und SD Kartenprobleme angezeigt.

// 02.02.190
//    - DS2690,SA37321 - Multimaster: es können jetzt 5000 statt bisher 1000 Werte übertragen werden
//    - DS2088,SA26192: Ret.Server File: Die in 02.02.186 durchgeführte Änderung (beim Reboot warten, 
//      bis Schreiben ins File fertig), ist jetzt auch für Salamander Betriebssysteme ab Version 
//      09.03.080 wirksam.

// 02.02.189
//    - DS2675,SA36539 - Fehlerbehebung: ab Loader Version 02.02.170 kam es mit einer LasalOS 
//      Version kleiner 1.2.124 zu einer Access Exception.
//    - DS2673: Multimaster: Neue Funktion LDR_MM_SetWaitForReqTimeout, mit der die Zeit angegeben 
//      werden kann, wie lange man bei einer Multimaster Verbindung vor dem Aufruf der letzten 
//      Init-Methode wartet, bis die Werte von required Clients vorhanden sind. 
//      Der Defaultwert ist 0, d.h. es wird nicht gewartet.
//      Der Wert kann auch über das File c:\lsldata\stations.txt bei der Connection eingestellt werden:
//      z.B. Wert 5000ms: "Station1","TCPIP:10.10.116.7",5000
//      Neue Funktion LDR_MM_AreAllReqInitialised mit der abgefragt werden kann, ob alle requred 
//      Server initialisiert sind.
//      Die Funktion LDR_MM_StatNrIsConnected und LDR_MM_AreAllReqInitialised wurden so geändert, 
//      dass während dem Aufruf der letzten Init-Methoden (_FirstScan TRUE) der Status, der vor 
//      dem Aufruf der letzten Init-Methode vorlag, zurückgegeben wird.
//    - DS2681: Optimierung: Mutex Aufrufe beim Comlink und bei asynchronen Fileoperationen verringert

// 02.02.188
//    - DS5389(LSL) Multimaster NewInst: Neue Funktion LDR_MM_SvrGetState 
//                                       Acces Fehler beim Allokieren der internen Paramter für die NewInst
//                                       Kommandos behoben.

// 02.02.187
//    - DS5322(LSL), SA35771 Multimaster MerkerEx: CRC erhöhen, wenn Daten empfangen wurden
//    - DS5389(LSL) Multimaster NewInst: Rückgabewert der NewInst wird nun korrekt befüllt
//                                       Verbesserung der Statusrückmeldungen
//    - DS5390(LSL), SA36798 Multimaster Login auf Hoststation ändert nicht mehr die Descriptor CRC. 

// 02.02.186
//    - DS2088,SA26192: Ret.Server File: Wenn nicht geschriebene Werte vorhanden sind, dann wird 
//      das Betriebssystem darüber informiert, damit das Betriebssystem bei ein Reboot aus der 
//      Applikation so lange verzögern kann, bis die Werte geschrieben sind.
//    - DS2640,SA33607: Zugriff auf eingebettete String Objekte vereinfacht. Der Zugriff auf einen 
//      eingebetteten String ist jetzt auch mit dem Namen des äusseren Servers, der auf den embedded 
//      String verbunden ist, möglich.
//    - DS5303 - double/float cast to long long causing linker errors in C source code.
//    - DS5376 - Modulo operator for long long data types causes linker errors in C source code.
//    - DS5377 - Implementation for modulo operator using long long data type computes wrong values.
//    - DS5379 - Using float/double library function isnan() causes linker error.
//    - DS5380 - Missing clz() built-in functions for ARM target.


// 02.02.185
//    - DS5340(LSL): Bei schreibgeschützen Multimasterservern wird bei einem Schreibvorgang nun ACCESS_DENIED 
 //                  statt -1 zurückgeliefert
//    - DS2618, SA34829: Geschwindigkeitsoptimierung beim Laden eines Projekts mit vielen retentive File-Servern 
//    - DS2636: Beim Interpreter wurde beim Singlestep die aktuelle Zeile im Interpreterprogramm (uiActLine) 
//      nicht korrekt hochgezählt.

// 02.02.184
//    - DS5048: Wird die Länge bei einem Multimaster MerkerEx verkleinert, wird diese nun auch beim Lesen korrekt gesetzt.
//	  - SA34164: ARM-Plattform: Divisionen erzeugen jetzt einen Division-Error, wenn ein Überlauf passiert.

// 02.02.183
//    - SA34666: Trigonometrie-Funktionen, welche aus dem OS aufgerufen werden,
//      haben auf ARM-Plattform teilweise sehr lange Laufzeiten, um die Genauigkeit sicherzustellen.
//      Die Änderungen zu SA32567 wurden daher wieder rückgängig gemacht.

// 02.02.182
//    - DS2589 Fehlerbehebung SRAM: Wenn die Umgebungsvariable SRAM_CONTINUE_ON_ERROR 
//      gesetzt war und im Sram ein Fehler erkannt wurde, dann wurde das Sram nicht mit 
//      den Init-Werten, sondern mit den Werten der Sram-Kopie des zuletzt ausgeführten 
//      Reorganisierungsvorgang geladen.

// 02.02.181
//    - SA33674,DS2548 Erweiterung: Neue Funktion ProgIp_StartProgramOfs mit der ein Interpreterprogramm 
//      ab einem bestimmten Offset gestartet werden kann. Bisher konnte man ein Interpreterprogramm 
//      nur ab einem bestimmten Label starten (ProgIp_StartProgram)
//    - DS2574 Fehlerbehebung: Es gab ein Speicherloch im Comlink-Server, wenn sich ein Comlink-Client 
//      wiederholt bei einem Comlink-Server nach einem Neustart angemeldet hat, wobei das Speicherloch 
//      nach 32 Anmeldungen nicht mehr weiter gewachsen ist.
//    - SA33434,DS2537 Fehlerbehebung: Der Machine Manager hat den Wert 16#8000_0001 nicht übertragen.


// 02.02.180
//    - SA33778, DS 5079 (LSL) Fehlerbehebung: Stoppen und Starten einer Multimaster Host Station
//      führt nicht mehr zu einem Speicherloch von 32 Byte auf der Multimaster Consumer Station.

// 02.02.179
//    - SA33172,DS2528 Bei einem permanenten Fehler auf der SD Karte hat der Versuch, 
//      die Daten von Retentive-Server Objekten wegzuschreiben, zu einer sehr hohen CPU-Belastung 
//      geführt.
//	  - DS 5012(LSL) Fehlerbehebung in 64 Bit Divisionsfunktion für C code.

// 02.02.178
//    - DS4998(LSL) Fehlerbehebung: Beim Lesen von Multimaster String bzw MerkerEx Servern, die 
//      noch keinen Wert enthalten, wird der Status korrekt gesetzt.
//	  - DS4957(LSL) Optimierung: Bei den Multimaster Server wird die CRC nur bei Strings 
//		berechnet. Aus Perfomancegründen wurde die CRC Berechnung für MerkerEx entfernt 
//		und liefert bei GetCRC() immer 0.
//	  - SA 32567 Für Mathematikfunktionen in LREAL(SIN,COS,TAN,...)werden jetzt 
//		OS-Funktionen verwendet, daes zu rundungs unterscheiden zwischen ARM und Intel kam.
//    - SA31591,DS2507 Unterstützung von IO-Elementen für Clients (bisher waren IO-Elemente 
//    auf Server beschränkt)
//    - DS2385 - Optimierung der Refreshliste über TCP/IP

// 02.02.177
//    - SA33093,DS2513: Bei CPUs mit Readonly Filesystem (z.B. DTC161) werden jetzt die 
//      Deskriptor-Bereiche nicht mehr ins File geschrieben und beim nächsten Hochlauf 
//      wiederverwendet, da dieser Mechanismus bei Readonly Filesystemen nicht funktioniert.

// 02.02.176
//	  - DS2432 Fehlerbehebung: Wenn die _LookupEmbedded Funktion mit bestimmten falschen 
//      Objektnamen (z.B. 'Obj1. Server0') aufgerufen wurde, dann kam es zu einem Runtime-Error.
//    - DS2491 Verbesserung: Wenn externe Verbindungen zu Clients von nicht existierenden 
//      Objekten vorhanden sind, dann wird nicht nur für die erste fehlerhafte Verbindung sondern 
//      für alle eine Fehlermeldung im Debugger-Trace ausgegeben.
//    - DS2493 Verbesserung: Zeitliche Optimierung der _LookupEmbedded Funktion: Die Durchlaufzeit
//      der Funktion war seit der Berücksichtigung der IO-Elemente (2.2.166) bei Anfragen von nicht 
//      existierenden Objekten relativ hoch.
//	  - DS4943(LSL): Einige Optimierungen und Sicherheitsabfragen im Bereich Multimaster

// 02.02.175
//	  - DS4937(LSL) Fehlerbehebung: Es konnte vorkommen, dass bei der gleichzeitigen Verwendung der Multimaster-Variablen 
//		und Visualisierung die Werte nicht mehr aktualisiert wurden. 
//	  - DS4934(LSL) Fehlerbehebung: Access Fehler bei den Loaderfunktion LDR_MM_StatNrConnStop, LDR_MM_StatNrConnStart
//      und LDR_MM_StatNrConnSetStr behoben

// 02.02.174
//	  - DS4933(LSL) Fehlerbehebung: Im LookUp der Tabelle für IO- und Multimaster-Elemente
//		wurde die Tabelle immer wieder gelesen und somit Speicher allokiert
//    - SA31989,DS2477 Fehlerbehebung bei der Berechnung der Projekt-CRC (Funktion _CalcDescCRC):
//      Die Tabelle mit den IO Elementen (ab 02.02.166 verfügbar) wurde nicht in die Projekt-CRC 
//      mit eingerechnet.
//    - SA32263,DS2475 Fehlerbehebung: Beim Sram Format 2 wird der Wert von _s_ram_hptr.Datalength 
//      so korrigiert, dass damit die allokierbare Sramgröße angezeigt wird. Bisher konnte diese Variable 
//      einen um bis zu 31 Byte zu großen Wert aufweisen (wg. der MAT Blocksize). Dadurch wurde im 
//      Lasal-Class im Project-Info Fenster ein zu großer Wert von 'Free Mem' angezeigt.
//      Beim Sram Format 1 wird jetzt zusätzlich überprüft, ob das Sram große genug ist für den ersten 
//      xRamHdr Block (dzt. gibt es aber keine CPUs, bei denen Sram Format 1 bei weniger als 8k Sram 
//      verwendet wird)
//    - SA32055,DS2490,RM152 Fehlerbehebung: Die Funktion _stricmp (case insensitiver String Vergleich) 
//      war bei ARM Plattformen falsch implementiert: Wenn bei den zu vergleichenden Strings bei einem 
//      ein großes und beim anderen ein kleines z enthalten war, die Strings ansonsten aber gleich waren, 
//      dann lieferte die Funktion den Rückgabewert 'ungleich'.

// 02.02.173
//		- Prj09715 Multimaster Variablen: 
//			Lesen von n-Byte Daten in einem eigenen Thread
//			Fehler bei MerkerEx Übertragung behoben

// 02.02.172
//		- Prj09715 Multimaster Variablen: 
//			Fehler bei StatusFlag behoben
//			Mutex bei den Comdef Funktionen 
//			Perfomance Verbesserunge bei LDR_LookupEmbedded3

// 02.02.171
//    - SA32171 Beim Laden eines Interpreterprogramms (ProgLoad) wird die Checksumme 
//        jetzt erst nach dem Linken berechnet.

// 02.02.170
//   - Prj09715 Multimaster Variablen
//        Neue Tabellen am Ende der ConfigObjects einlesen
//        Neue Tabllen am Ende der ON einlesen
//        Stations.txt einlesen
//        Neue Klassen für Multimaster
//        Neue TXCOMMAND I_INIT_MERKEREX und I_MERKEREX
//			  Neues Flag für CRC - Prüfung für MerkerEx in der Refreshliste
//        Update der MerkerEx Klasse
//        StringInternal eingebunden
// 	- SA 23461 Schnellere ASIN Funktion implementiert.
//			  Dadurch sind auch ASIN_GR, ACOS_GR und ACOS 
//			  betroffen da diese Funktionen die ASIN Funktion verwenden.


// 02.02.169
//        - Prj09114,DS24413 - Timeslots für Realtime und Cyclic
//          In den Netzwerktabellen gibt es hinter den external - und magic internal connections
//          jetzt zusätzliche Tabellen (rt und cy) für Timeslots. Diese müssen zusätzlich nach der
//          Abarbeitung der Netzwerktabellen ausgewertet werden und beim Setzen vom Task (Objekte
//          müssen asynchron gesetzt werden).

// 02.02.168
//        - SA29877,DS2338 - Comlink,CAN: Die in 02.02.166 durchgeführte Änderung 
//          (Freigabe des Kommunikationskanals nach einem Login) konnte beim Abstecken 
//          der CAN Verbindung zu einer Access Exception führen.

// 02.02.167
//        - SA30524,DS2377: Die in 02.02.166 durchgeführte Änderung des Verhaltens der 
//          _strncpy Funktion wurde wieder rückgängig gemacht. Bei der Verwendung von 
//          _strncpy muss jetzt wieder darauf geachtet werden, dass es zu keinem 
//          Speicherüberschreiber kommt, da in den Zielbuffer um 1 Byte mehr geschrieben 
//          werden kann, als im Parameter 'max' angegeben.
//          Beispiel:
//            _strncpy(buf, "AB", 2) -> buf: 'A','B','\0'
//        - DS2316,SA29496: Wenn die Umgebungsvariable NO_SRAM_POWERDOWN_CHECK existiert 
//          und einen Wert ungleich "0" aufweist, dann wird die Prüfung, ob das Sram beim 
//          Powerdown geschrieben wurde, übersprungen. Diese Prüfung gibt es ab der 
//          Loaderversion 02.02.158.

// 02.02.166
//        - SA29877,DS2338 - Comlink,CAN: Wenn beim LOGIN ein Fehler auftritt, dann wird 
//          jetzt der Kommunikationskanal freigegeben.
//        - SA30524,DS2377 - Änderung des Verhaltens der _strncpy Funktion:
//          _strncpy kopiert jetzt nie mehr Bytes als im Parameter 'max' angegeben. 
//          Bisher wurden maximal 'max' Zeichen plus ein abschließendes 0-Zeichen kopiert.
//          Beispiel:
//            alt: _strncpy(buf, "AB", 2) -> buf: 'A','B','\0'
//            neu: _strncpy(buf, "AB", 2) -> buf: 'B','\0'
//        - SA29771,DS2330 - CAN Verbindungsproblem:
//          Beim LOGOUT wurden beim CAN nicht alle Resourcen freigegeben was dazu führte, dass nach 
//          mehrmaligem LOGIN/LOGOUT über CAN kein weiteres CAN-LOGIN mehr möglich war.	
//        - DS2286(LasalOS),DS3579(Lasal),SA22084: Adressen von IO Elementen können jetzt über 
//          I_GET_OBJ/LookupEmbedded abgefragt werden. Damit können IO Elemente im LSE und 
//          im Data Analyzer visualisiert werden.

// 02.02.165
//        - DS2382: Fehlerbehebung beim Aufruf von Sysmain. Dieser Fehler führte bei einer 
//          ARM CPU und einer neuen Compilerversion zu einer Endlosschleife und das Projekt 
//          startete nicht (CPU Status blieb im Status 'Loader OK').
//        - Der Datentyp der Systemvariablen für die Heap-Statistik (% M 10E4 - % M 10FC) 
//          wurde von UINT auf UDINT korrigiert.

// 02.02.160
//        - Code korrigiert, der ab Version 02.02.158 zu einer Compiler-Warnung führte

// 02.02.159
//        - DS2352: Wenn nach einem Leerprojekt (ohne Klassen und Objekte) ein Klassenprojekt 
//          gestartet wurde, und dabei die Umgebungsvariable SRAM_DISABLE_REORG gesetzt war, 
//          dann wurde fälschlicherweise ein Sram-Fehler erkannt. 

// 02.02.158
//        - DS2316,SA29496:Auf Plattformen mit einer Sramdisk (ETVEdge) wird jetzt geprüft, ob beim 
//          Abschalten das Sram weggeschrieben wurde. Wenn nicht wird ein Sram-Fehler ausgelöst.
//        - DS811(Salamander): neue Sysvars für Memory-Leak Diagnose (nur bei Salamander vorhanden)

// 02.02.157
//        - DS2146,SA26708: Fehlerbehebung beim Download-on-the-fly: Fehlerbehebung in 02.02.156 korrigiert
//        - Fehler in Hilfsfunktion Sqrt64 behoben

// 02.02.156
//        - DS678(Salamander),SA28483: Die Variable _s_ram_hprt.UsedData, in der die Sram-Belegung 
//          gespeichert ist, wurde falsch (zu groß) berechnet. Weiters wurde die Belegung des Srams 
//          nicht optimal ausgenutzt.
//        - DS2146,SA26708: Fehlerbehebung beim Download-on-the-fly: Wenn eine Klasse einen Server 
//          enthielt, bei dem das Retentive-Flag gesetzt ist, dann ist nur beim ersten Objekt dieser 
//          Klasse der Code, der mit Download-on-the-fly runtergespielt wurde, wirksam geworden.

// 02.02.155
//        - DS2246,SA28286: Bei einer externen not-required Client Verbindung zu einem 
//          Server eines nicht existierenden Objekts ist keine Fehlermeldung erstellt worden.

// 02.02.154
//        - Code korrigiert, der ab Compiler V17 zu einer Compilerwarnung führt 

// 02.02.153
//        - DS2235,SA28580: Verbesserung bei der Allokierung von Heap-Speicher, damit 
//          der Heap weniger start fragmentiert wird.
//        - Commando I_WR_SVRLIST kann jetzt auch mit falschen LasalIDs(NIL) umgehen.

// 02.02.152
//        - DS2203: Bei der ARM-Version wurde der Interpreterbefehl P_DIV von vorzeichenbehaftet 
//          auf vorzeichenlos umgestellt, damit sich der Befehl gleich wie bei der 
//          Intel-Version verhält
//        - DS2197: Refactorings bei den Funktionen Find_sRamFile und Find_sRamFile: 
//          Pointer-Pointer Typ eines Parameters korrigiert (pXFileCell_t)

// 02.02.151
//        - SA26381: Comlink + StringObjekte via CAN-Bus funktioniert auch auf ARM-Plattform

// 02.02.150
//        - DS2170: Loader fährt jetzt auch hoch wenn kein Sram oder Sramgröße 0 vorhanden ist
//        - DS2197: Fehlerbehebung in einer Loaderfunktion (Add_sRamFile_I, nur ARM-Version).
//                  Es kam zu einem Stacküberschreiber, wenn ein Ramex Objekt die Verbindung 
//                  zur Sram-Zelle hergestellt hat.

// 02.02.149
//        - SA26381: Comlink via CAN-Bus funktioniert auch auf ARM-Plattform

// 02.02.148
//        - DS2145: Es sind jetzt auch SRAM Größen kleiner als 8kB möglich

// 02.02.147
//        - SA 27392: Fehlerbehebung im Interpreter code für ARM.
//                    Umstellung von RamEx auf MerkerEx beim Anlegen von Interpreter Objekten auf ARM Plattformen(Unaligned Access in SRAM).
//        -      ARM: Fehlerbehebungen in Hilfsfunktionen für Division/Modulo.

// 02.02.146
//        -SA27194 - Fehler bei Division mit negativem Divisor im ARM-Funktion __divdi6432()

// 02.02.145 
//        - DS2115,SA25903 - Fehlerbehebung SRAM:
//          Unter bestimmten Umständen konnte die Belegung der RAM- und RAM-Ex Zellen 
//          im Sram mehr fragmentiert sein als notwendig. Das hat die Auswirkung, dass 
//          die Ausnutzung des Srams nicht optimal ist und die Anzeige der Größe des 
//          benutzten Srams einen größeren Wert aufweist.
//          Der Fehler tritt dann auf, wenn in der Sram Datenstruktur ein Fehler festgestellt 
//          wird und gleichzeitig das Format des Srams (Umgebungsvariable SRAMFORMAT) 
//          geändert wird.
//          Nach dem aus- und wieder einschalten der Steuerung wird das Sram reorganisert 
//          und die Auswirkungen des Fehlers sind dann behoben.
//        - ARM: CRC16 Wird wieder richtig berechnet.

// 02.02.144 11.12.2013
//        - ARM: Exeptionhandling für Divisionen durch 0 in C Files

// 02.02.144 03.07.2014 Kunden-KM änderung von 143 auf 144 am 03.07.2014
//        - Commando I_WR_SVRLIST kann jetzt auch mit falschen LasalIDs(NIL) umgehen.   

// 02.02.143 7.11.2013 
//        - ARM Ready
//			  - CLI/STI removed 
//			  - CriticalSection usage

// 02.02.142 6.11.2013
//        - SA26191,DS2087
//          Workaround für einen Betriebssystemfehler: Nach dem Aufruf der OS-Funktion SRAMDISK_COMMIT 
//          sind die Interrupts gesperrt, wenn auf ETVEdge-Plattformen das Flag fullCopyAtPowerdown in 
//          der config.lsl gesetzt ist. 
//          Mit diesem Workaround werden im Loader nach dem Aufruf dieser Funktion die Interrupts wieder 
//          freigegeben.

// 02.02.140 24.10.2013 
//            - DS2085,SA26176 - Fehlerbehebung Sram: bei der Hardware BDF2000-20-T2 kam es nach 
//              jedem Neustart (stromlos) zu einem 'Loader out of near' Fehler.

// 02.02.139 
//            - DS2078 - RamEx Convert.

// 02.02.138 02.09.2013
//            - DS1747
//              Projekt-CRC war nach einem Reboot anders als nach einem Reset+Run
//            - DS2053,SA18179
//              Versionsnummer und name des Lasal-Projekts kann mit der neuen Funktion LDR_GetPrjInfo 
//              abgefragt werden.
//            - DS2047,SA25361: Sram
//              Bei der Plattform CCL081CET trat ab der Loader Version 2.2.134 nach dem RUN ein Sram-Fehler auf.
//              Fehlerursache: Es wurde in HasRetentiveSram nicht berücksichtigt, dass bei dieser Plattform 
//              kein nullspannungssicheres Sram vorhanden ist.

// 02.02.137 17.6.2013
//            - DS2027
//              Neue Loaderfunktion LDR_InstallPostInitCB, mit der eine Callback-Methode installiert 
//              werden kann, die nach den Initmethoden als NewInst aufgerufen wird
//            - Vorwärtsdeklaration für Mathematikfunktionen

// 02.02.136 7.5.2013
//            - DS2013: Comlink
//              Beim Beenden einer Comlink-Verbindung wurden nicht alle zu dieser Verbindung gehörenden 
//              Resourcen freigegeben.
//              Änderung. Speicherleck in der Funktion StoreCmdEx behoben.
//              Dieser Fehler ist ab Version 2.2.102 vorhanden.

// 02.02.135 2.5.2013
//            - DS2009: Comlink
//              Mehrere LARS Instanzen auf dem gleichen PC konnten nicht gleichzeitig eine Comlink 
//              Verbindung zu ein und derselben Steuerung herstellen. Damit das funktioniert, muss 
//              nämlich jede Lars Instanz beim Aufbau der Verbindung eine eindeutige Application-ID 
//              mitschicken.
//              Es wurde jetzt eine Möglichkeit geschaffen, eine Application-ID für eine Comlink-Verbindung 
//              zu definieren, und zwar mit dem Setzen der Umgebungsvariablen COMLINK_APPLID. Der Wert 
//              dieser Variablen muss eine Zahl größer als 0 sein und muss in jeder LARS Instanz unterschiedlich 
//              sein.
//              Der Befehl zum Setzen der Umgebungsvariablen wird üblicherweise in die autoexec.lsl eingetragen.
//              Bsp.: SETENV COMLINK_APPLID 1234

// 02.02.134 30.4.2013
//            - Bei Aufruf von I_WRITE über DebugIP wurde die entsprechende RefreshlistZelle nicht auf Invalid gesetzt.
//              Dadurch konnte es zu Update-Probleme bei Kommunikation über Lasal32.dll samt Verwendung von RefreshList kommen.
//              neue Funktion: SyncRefListsTcpAll() 

// 02.02.133 21.02.2013
//            - DB1729: SRAM
//              Beim Laden der Datei sram.cpy bzw. sram.ini wurde am Heap zuwenig Speicher allokiert, 
//              wenn die sram.cpy/ini mehr Sramdaten enthält, als die derzeitige Größe des Srams.
//              Es kam infolge zu einem Absturz. Sramdaten gingen keine verloren.
//            - Initialisierung
//              Die Versionsprüfung der RAM- und RAMEx- Klassen war fehlerhaft: Der Loader benötigt 
//              für die RAM-Klasse eine min.Version 1.1 und für die RAMEx- Klasse eine min.Version 1.2. 
//              Hier wurde fälschlicherweise nur die LO-Revision geprüft, was daszu führte, dass 
//              bei RAM-Klassen die Versionsnummern x.0 und bei RAMEx-Klassen die Versionsnummern x.0 und x.1 
//              nicht funktionieren. Der Loader bleibt in diesem Fall mit dem Fehlercode C_INCOMPATIBLE stehen.
//            - DB1753: Debug-Interpreter
//              Bei den Debug-Interpreter Befehlen I_WRITE, I_WRITE_TO_CLNT, I_READ, I_READ_CLT werden
//              die Read- und Write- Methoden nur mehr dann aufgerufen, wenn die Applikation auch läuft.
//            - DB2007: SRAM
//              Wenn die Initmethoden nicht fertig durchlaufen wurden (z.B. wegen einer Exception 
//              oder eines Resets in der Init-Methode) und wenn zusätzlich ein Reorganisieren des Srams 
//              durchgeführt wurde, dann sind Änderungen an Ram- oder Ramex- Objekten nicht übernommen worden.
//            - DB1875: SRAM
//              Wenn der verwendete SRAM-Speicher kleiner als der verfügbare war, dann konnte es zu 
//              einem Absturz kommen.

// 02.02.132 22.10.2012
//            - DB1944, SA23101: SRAM
//              Nach einer Änderung der ramfile.dat Datei wird bei Edge CPUs das Sram auf 
//              die SD-Karte geschrieben, damit für den Fall wo beim Abschalten keine Sram 
//              Sicherung auf Disk gemacht wird, die ramfile.dat Datei mit dem Sram-Inhalt 
//              zusammenpasst.
//            - DB1975: SRAM
//              Fehlerbehebung: Wenn bei 2 Ramex Objekte mit dem NewInst Befehl EXCHANGE_DATA_PTR 
//              die Daten ausgetauscht wurden, dann haben beim nächsten Hochlauf die Datenzeiger 
//              der beiden Ramex Objekte auf die gleichen Daten gezeigt.
//            - DB1950, SA23231: SRAM
//              Fehlerbehebung SRAM Konvertierung: Auf Plattformen mit einem Read-Only-Filesystem 
//              kam zu einer Access Exception (ab 02.02.123)
//            - Fehlerbehebung bei der strncpy Funktion:
//              Bei gleichen Strings kürzer als die angegebene max.Vergleichslänge wurde über das 
//              Stringende hinaus verglichen. Der Fehler hat sich bisher nicht ausgewirkt, da es 
//              sich um keine globale Funktion handelt und keine internen Aufrufe existieren, 
//              die zum Fehler führen.
//			  - DS1909, SA22211
//				Wurde im Lasal Class die TRACE() Funktion mit mehr als 255 Zeichen verwendet, ist
//				meistens die CPU abgestürzt. Dies wurde jetzt in der OutMessage Funktion korrigiert.

// 02.02.131 13.07.2012
//            - Neue Trace Makros zum Ausgeben von Fehlermeldungen oder Warnungen in Lasal Class2.
//              Bestehende Trace Ausgaben je nach Kontext auf die neuen Makros umgestellt um
//              Fehler für den User schneller sichtbar zu machen.

// 02.02.130 13.07.2012
//            - Im Zuge von vorhergehenden Änderungen wurde eine Debug Meldung nicht entfernt,
//              dadurch brauchte der Loader sehr lange zum Starten, wenn viele Ram Zellen im
//              Projekt verwendet wurden.
//            - Wenn ein Retentive Server von Ram auf File geändert wurde und SRAMFORMAT 2 verwendet wird
//              und auch noch das SRAM reorganisiert (reboot) wurde, wurde der alte Wert nicht übernommen.

// 02.02.129 28.06.2012
//            - Neue Hilfsfunktionen für LReal
//            - DB1679: Bei der Auswertung des Lasal-Connection-Files (prjcfg.lcf) kam es 
//              bei CPUs ohne Filesystem zu einem Absturz. Das Connection-File wird ab 
//              Loader Version 02.02.120 ausgewertet.
//            - Fehlerbehebung bei der in 2.2.123 hinzugefügten Sram Konvertierung 
//              (Konvertierung Retsvr-File -> Retsvr-Sram und wieder zurück funktionierte nicht)

// 02.02.128 21.06.2012
//            - Fehlerbehebung bei der in 2.2.123 hinzugefügten Sram Konvertierung 
//              (Exception durch einen falschen Formatstring bei der Debug-Ausgabe)

// 02.02.127 14.06.2012
//            - DB1848: Erweiterung des prjcfg.lcf Files (Angabe von Initwerten möglich)

// 02.02.126 22.05.2012
//            - Neue Funktion "LDR_IsAsnycFileOperationInProgress".
//              Sind Retentive File Operationen aktiv ?
//            - Update von RamEx und MerkerEx.

// 02.02.125 14.05.2012
//            - Nicht mehr benötigte RamEx Files löschen.

// 02.02.124 7.5.2012
//            - DB1822: SRAM
//              Wenn die Initmethoden nicht fertig durchlaufen wurden (z.B. wegen einer Exception 
//              oder eines Resets in der Init-Methode), dann ist ein zuvor im Konstruktor neu 
//              angelegtes RAM-Objekt verloren gegangen. Der Fehler trat nur beim Sram-Format 2 auf.

// 02.02.123 11.04.2012
//            - DB1795, SA20924: SRAM
//              Wird ein RetentiveRam auf einen RetentiveFile (oder umgekehrt) Server geändert, wird
//              der alte Wert übernommen.
//            - DB1794, SA20925: SRAM
//              Laden von den Files "SRAM.INI" bzw. "RAMFILE.INI".
//              Diese Files werden beim Reorganisieren (nach reboot) des SRAM geladen, wenn diese im
//              Verzeichnis LSLDATA vorhanden sind und das SRAM gültig ist. (Nicht CPU Status "Ldr out of near")
//              Bei einer neuen CPU (die Datei "isnotnew.inf" ist NICHT vorhanden) werden die Files auch bei
//              ungültigem SRAM geladen. (Nur bei erstem RUN nach PowerON).
//            - SRAM - Konvertierung von Format 1 <--> 2.
//            - Bei Edge CPU's werden jetzt die SRAM Daten (Format 2) geladen, wenn die SD Karte
//              in einer anderen CPU verwendet wird.
//            - DB1821:
//              Es können jetzt mehr als 65530 Objekte erzeugt werden.

// 02.02.122 29.2.2012
//            - In der Version 2.2.120 wurde irrtümlich im File mem.st eine Variablendeklaration 
//              von VAR_PRIVATE auf VAR_GLOBAL geändert, was zu Namenskonflikten führte. 
//              Das wird in dieser Version wieder rückgängig gemacht.
//            - Globale Variablen für die Disk Schreibstatistik hinzugefügt (_diskOperatingSeconds etc.)

// 02.02.121 7.2.2012
//            - DB1739, SA19953: Sram
//              Die Loader-Funktion SRAM_MarkMemory kann jetzt so aufgerufen werden, dass 
//              der Speicherbereich nur dann als belegt markiert wird, wenn dieser vor dem 
//              Aufruf zur Gänze frei war. Wenn beim Aufruf von SRAM_MarkMemory beim Parameter 
//              length0 das oberste Bit gesetzt ist, dann wird vor dem Markieren geprüft ob 
//              alles frei ist. Ohne gesetztes oberstes Bit verhält sich die Funktion unverändert.
//              Diese Funktionalität wird von der RamEx Klasse ab Version 1.5 benötigt, damit 
//              das Sram weniger fragmentiert wird.
//            - DB1784:
//              Wenn für die im Projekt verwendeten RAM- und RAMEx- Zellen keine SRAM Speicher 
//              mehr zur Verfügung steht, dann fährt das Projekt nicht mehr hoch. Es wird der Fehler 
//              C_OUTOF_NEAR und die Tracemeldung 'Out of SRAM memory'

// 02.02.120 7.12.2011
//            - DB1679: Auswertung des Lasal-Connection-Files (prjcfg.lcf)

// 02.02.119 7.12.2011
//            - Beim Sram Format 1 gab es einen kurzen Zeitraum während der Reorgphase, in der 
//              der Sraminhalt verlorengehen konnte, wenn in diesem Zeitraum abgeschaltet wurde.

// 02.02.118 7.11.2011
//            - SA19434, DB1720:
//              Die Prüfung, ob es sich um ein jungfräuliches System handelt, hat nicht funktioniert, 
//              wenn das Verzeichnis c:\lsldata nicht vorhanden war.

// 02.02.117 18.10.2011
//            Neue globale Systemvariable _RtOSPatchVersion. Enthält die LasalOS Patch Version ('A', 'B', 'C', usw.).

// 02.02.116 10.10.2011
//            - SA19809
//              Neue Loaderfunktion LDR_ForceSramReorgOnNextRun, mit der man das Reorganisieren 
//              des Srams beim nächsten Run erzwingen kann (ab 2.2.103 wird nämlich das Sram 
//              aus Geschwindigkeitsgründen nur mehr beim ersten Run nach dem Einschalten reorganisiert).
//            - SA19809
//              Wenn im Sram aufgrund eines jungfräulichen Systems oder eines SRAMCLEAR Cli Befehls 
//              eine ungültige Datenstruktur entdeckt wird, dann wird dieser Fall jetzt erkannt 
//              und nicht mehr als Fehler gewertet.

// 02.02.115 10.10.2011
//            - DB1709: Wenn in der Hochlaufphase ein Sram Fehler erkannt wird, dann wird die Applikation 
//              nicht angehalten, wenn die Umgebungsvariable SRAM_CONTINUE_ON_ERROR 
//              vorhanden ist und einen Wert ungleich 0 aufweist (die Änderung dass die Applikation 
//              nach der Erkennung eines Sram Fehlers nicht mehr hochfährt, ist in 2.2.97 gemacht worden).

// 02.02.114 03.10.2011
//            - Bei der zyklischen SRAM Prüfung, die mit dem define CYCLIC_SRAM_CHECK aktiviert wird, 
//              wurde der Alarm für den SRAM-Fehler nur im C_RUNRAM aufgerufen..

// 02.02.113 30.9.2011
//            - Bei der zyklischen SRAM Prüfung, die mit dem define CYCLIC_SRAM_CHECK aktiviert wird, 
//              kann jetzt auch ein Sram Format 1 geprüft werden.

// 02.02.112 29.9.2011
//            - SA19568, DB1698 - Comlink/TCPIP Fehlerbehebung 
//              Wenn während eines LOGIN Aufrufs ein Taskswitch stattfand, dann konnte der Fall 
//              eintreten, dass ein Element in der LOGIN-Datenstruktur (pComdefT.pLogin) 
//              fälschlicherweise auf 0 gesetzt wurde, was dann zu einer Exception (in TXCOMMAND) führte.

// 02.02.111 14.9.2011
//            - Bei den Plattformen CCL081 und HZS511 wird jetzt berücksichtigt, dass kein 
//              nullspannungssicheres Sram vorhanden ist. Es kommt jetzt hier zu keinen falschen 
//              Sram-Fehlermeldungen mehr.

// 02.02.110 8.8.2011
//            - Die Fehlerbehebung aus 02.02.102 ist beim Zusammenführen 2er Loaderversionen 
//              irrtümlich nicht mitkopiert worden und ist jetzt wieder enthalten.

// 02.02.109 7.7.2011
//            - SA18794: ungültige Sramdaten (z.B. wegen einer leeren Batterie) konnten zu einer 
//              Access-Exception führen

// 02.02.108 27.6.2011
//            - Fehler in 2.2.107 behoben (wenn das Sram Reorg unterbrochen wurde, kam es zu einem Sram Fehler)

// 02.02.107 27.6.2011
//            - SA18905: Fehlerbehebung der Änderung in 2.2.104 (Erstellung der Files):
//              Files werden jetzt nur mehr mit einer maximalen Blockgröße beschrieben um einen 
//              Fehler -150 (Sram zu klein) zu vermeiden.
//            - SA17754: Wenn das Sram aufgrund einer leeren Batterie ungültig war, wurde der 
//              Sram-Inhalt von der Kopie geladen und keine Meldung ausgegeben.

// 02.02.106 21.6.2011
//            - Fehlerbehebung der Änderung in 2.2.103 (Hochlaufzeit verringert):
//              Die verkettete Liste der DebugIp Objekte war defekt.

// 02.02.105 20.6.2011
//            - Ab der Änderung in 2.2.103 (Hochlaufzeit verringert) konnte der Fall eintreten, dass 
//              der Sram-Inhalt verloren geht, wenn man das Projekt während des Hochlaufs anhält und 
//              dann mit Run wieder startet. Der Fehlerfall tritt nur dann ein, wenn das Projekt mitten 
//              in der Sram-Reorg-Phase angehalten wird. Zu dieser Zeit ist der Sram Inhalt nämlich 
//              nicht gültig, es existiert nur eine Sicherung in der Sram-Kopie-Datei. Wenn in diesem 
//              Fall beim nächsten Run die Kopie nicht geladen wird (und das war bei der Version der 
//              Hochlaufzeit-Optimierung der Fall), dann geht der Sram-Inhalt verloren.

// 02.02.104 27.5.2011
//            - The files DSCBLKS.BIN, SRAM.CPY and RAMFILE.CPY are now not created always, but 
//              existent files are reused to avoid FAT changes.

// 02.02.103 9.5.2011
//            - Hochlaufzeit verringert
//            - DebugIp Befehle I_RD_SVRLIST und I_WR_SVRLIST hinzugefügt
//            - SA16973 Fehler in _format behoben

// 02.02.102 9.5.2011
//            - Sram: SA17043 - When sram format 1 is used, then a retentive server, that ist backed up in 
//              Sram, destroyed the sram-cell when the init-value of the server was written into the sram-cell.

// 02.02.097 5.5.2011
//            - Comlink/TCPIP: beim Freigeben der Verbindungen konnte es zu einem ungültigen Speicherzugriff kommen
//            - Sram: After an error in the sram, its content will be resetted and a project restart is necessary
//            - Sram: Platform AUW now uses hardcoded sram format 1

// 02.02.096 14.10.2010
//            - Speicherleck und Resourceleck (Sockets) beim Comlink/TCPIP behoben

// 02.02.095 06.10.2010
//            - Der Vergleich von Gleitkommazahlen erfolgte nicht korrekt, da z.B. NaNs nicht 
//              berücksichtigt wurden.
//              Die Funktion DIAS_R_CMP wurde durch DIAS_R_CMP_E, DIAS_R_CMP_L und DIAS_R_CMP_G
//              ersetzt.
//            - Es wurden neue Funktionen hinzugefügt, um den Inhalt einer Gleitkommazahl beurteilen
//              zu können (FloatLib), wie z.B. IsNaN, IsFinite usw.

// 02.02.094 01.06.2010
//            - RTDB: ConnectCltSvr and _ReadConnect(with parameter bOnePair set to FALSE) did not 
//              return FALSE when a wrong or non-existent client was specified.
//            - Comlink: Beim Logout wurde versucht nicht allokierten Speicher freizugeben.

// 02.02.093 12.05.2010
//            - Retentive Server: A retentive server configured for a backup to a file now writes 
//              to the file in an asynchronous manner. This reduces the duration of the write-method 
//              of the server.

// 02.02.092 19.04.2010
//            - DebugIP: new Command I_GET_OBJ_LIST and I_VISU_RDY, for faster "GetID" of Kernel
//            - Comlink: added corrections when define COMLINK_PG is active
//            - Comlink: added a define (LOADER_TCP_MODERATE) that reduces cpu-cycles when tcp/ip 
//              connections are in use (but tcp/ip response time is increased in this case).
//              The default of this setting is off.

// 02.02.091 16.03.2010
//            - Verschlüsselung entfernt

// 02.02.090 12.03.2010
//            - LogBuf0 für Event19.log vergrössert (wegen Änderung der Logbufferstruktur
//              im Betriebsystem ab v01.02.055)      

// 02.02.089 30.11.2009
//            - Include names and addresses of variables in descriptor-CRC calculation

// 02.02.088 19.10.2009
//            - _PrepareLink Funktion in Rtdb_lib.st korrigiert (benötigt bei DownloadOnTheFly).
//              Es werden virtuelle Methoden der Klasse ausgetauscht (vorher hat das nicht funktioniert
//              wenn die betroffene Klasse eine Basisklasse hatte), außerdem werden die Deskriptoren der
//              Klasse und der Kanäle (Zeiger innerhalb _CT) richtiggestellt.
//            - In Funktion ApplInit die Funktion SystemInit aufrufen, falls das noch nicht geschehen ist.
//              Ansonsten kann es passieren dass eine andere Desc_CRC berechnet wird.

// 02.02.087 14.07.2009
//            - Fehlerbehebung bei funktion SQRT64

// 02.02.086 3.2.2009
//            - Fehlerbehebung bei Comlink über TCP/IP: es war bisher nicht möglich auf einem 
//              einzigen PC mit mehreren Programme gleichzeitig eine Comlink-Verbindung aufzubauen.

// 02.02.085 30.1.2009
//            - Comlink über TCP/IP: die Updateliste konnte nicht auf die vorgesehene Maximalgröße wachsen.
//              Die Obergrenze liegt jetzt bei 1000(stat.)/5000(dyn.), bisher: 900(stat.)/900(dyn.)

// 02.02.084 27.1.2009
//            - Comlink über TCP/IP: das Protokoll wurde so erweitert, dass in die Refresh-Liste 
//              Einträge mit globalen Variablen mit einer Grösse ungleich 4 eingetragen werden können.
//              (bisher war bei globalen Variablen die Grösse immer 4 Bytes)

// 02.02.083 11.12.2008
//            - Comlink über TCP/IP: es konnte der Fall eintreten, dass die Befehle am Refresh-Kanal 
//              nie bearbeitet wurden, wenn zu viele Änderungen in der Refresh-Liste waren 
//              (mit dem refrMsgListenInhibitCnt wird diese Situation jetzt vermieden)

// 02.02.082 3.12.2008
//            - Loader.lob um die Funktionen OS_ISARRAYOUTOFBOUND und OS_RAISEEXCEPTION erweitert

// 02.02.081 23.9.2008
//            - globale Funktion ProgIp_SetInstructionToProgStart

// 02.02.080 15.8.2008
//            - In der Funktion retentiveWriteSplitter wurd der Rückgabewert von M_WR_DIRECT 
//              nicht auf die Rückgabevariable geschrieben.

// 02.02.079 7.8.2008
//            - Das File zur Speicherung der Retentiveserver wird nun shared geöffnet.

// 02.02.078 31.7.2008
//            - in RetSvrFile::WriteNew wurde dData nicht aktualisiert, was zur Folge hatte, daß die 
//              Werte nicht in allen Fällen ins File geschrieben wurden.

// 02.02.076 8.5.2008
//            - Die Grösse des Speichers für die SRAM-Kopie ist jetzt immer ein Vielfaches
//              einer bestimmten Blockgrösse. Damit bewirken kleine Änderungen in der SRAM-Belegung 
//              nicht gleich eine Änderung der Objektadressen (=LasalIDs), die nach dem Reservieren 
//              der SRAM-Kopie angelegt werden.

// 02.02.075 xx.xx.xxxx
//            - Retentive Server for ETV-Platform

// 02.02.074 13.02.2008
//            - Bugfix in Mem.st: interpretation of return value of ISYSSERNUM_SERNUMGETPLC
//            - Bugfix in DebugIP (GetState, Kill)

// 02.02.073 19.12.2007
//            - Bugfix in Comlink TCP/IP
//            - Increased max. nbr of incoming tcp/ip comlink connections from 16 to 32

// 02.02.072 7.12.2007
//            - Bugfix in ObjectReader (retentive server)

// 02.02.071 3.12.2007
//            - Support for more CAN Interfaces
//            - The smallest update interval in the refresh list was reduced from 20 to 10 ms
//            - Corrected state machine in DebugIp

// 02.02.070 26.11.2007
//            - In some situations the format of the sram was calculated wrong. This error 
//              was introduced in version 2.2.65 with the environment variable SRAMFORMAT.

// 02.02.069 8.11.2007
///           - TxCommandEx is now thread-safe

// 02.02.068 22.10.2007
//            - Corrected errors in floating point functions (x**y was not correctly calculated 
//              under certain conditions)

// 02.02.067 15.10.2007
//            - GetState and Kill are now available via the debug interpreter (lasal32.dll).

// 02.02.066 10.10.2007
//            - Corrected error in memory allocation. A block was freed that was never allocated.
//            - The optimization introduced in 02.02.057 (re-using descriptor blocks) has now 
//              a requirement for LasalOS >= 01.01.217 because there was a bug in OS_SSR_MarkAllocV1

// 02.02.065 4.10.2007
//            - The environment variable SRAMFORMAT is used to specify to SRAM format.
//              A value of '1' means sram format 1 (without file sram.dat), the value '2' 
//              means sram format 2 (with file sram.dat).
//              An environment variable can be set with the CLI-command 'SETENV <name> <value>' 
//              e.g. SETENV SRAMFORMAT 2
//              *IMPORTANT*:
//              Whenever the sram format is changed, the content of the sram is lost, so it 
//              is necessary to save the sram content before setting a new format and restore 
//              it after the new format is set.
//            - Reduced Comlink TCP/IP Timeouts (Login: 10->3 secs, Requests: 5->3 secs)

// 02.02.064 21.08.2007
//            - Loader considers now the prioritiy of a lasal objects task function.
//              These priorities can be specified in lasal in the properties 
//              RealIndex, CyclicIndex, BackgroundIndex of a lasal object.

// 02.02.063 17.08.2007
//            - LDR_RETSVR define to switch on (default)/of the retentive servers
//              classes RETSVRFILE and RAM (aka RetSvrRam) added again

// 02.02.062 09.08.2007
//            - LDR_RETSVR define to switch off (default)/on the retentive servers
//              classes RAM and RETSVRFILE also removed until further notice

// 02.02.061 30.7.07
//            - Corrected error in the optimization introduced in 02.02.057

// 02.02.060 11.7.07
//            - The optimization introduced in 02.02.057 has now a requirement for LasalOS >= 
//              01.01.196

// 02.02.059 9.7.07
//            - Support for global variables in the refresh list: Corrected Error
//            - DebugIP: new Command I_GET_STACKINFO to get Callstack Info from a program interpreter

// 02.02.058 6.7.07
//            - retentive servers: no need to place RAM or RETSVRFILE classes in the project anymore

// 02.02.057 3.7.07
//            - Optimization to reduce project startup time: 
//              After the project is loaded, parts of the project structure is written to a 
//              file. This information is re-used on subsequent project starts when the project 
//              has not changed.
//            - Support for global variables in the refresh list:
//              The meaning of parameter LslCommRegData.uiTIME in function TXUPD has been 
//              extended: Bit 14 = 1 means that the value specified by LslCommRegData.LASALID 
//              is not a channel, but it is the address of a global variable instead.
//            - Corrected error in calculation of descriptor-CRC:
//              A change of the arrangement of server- or client- channels did not change the 
//              descriptor-CRC,

// 02.02.056 27.6.07
//            - retentive servers are supported

// 02.02.055 3.4.07
//            - bugfix in logout function

// 02.02.054 7.3.07
//            - On platform AUW, the SRAM will be resetted on startup (AUW has no battery buffered sram)

// 02.02.053 7.3.07
//            - ProgMgr_LoadIprProg2: Problem when calling this function from a low priority
//              task -> interpreter starts unmeant. An additional parameter controlls the
//              behaviour.
//            - Trace messages will now also be displayed beneath "Running LOADER..." 
//              (max. 5 lines)
//            - Trace message "Ext.connected from object ..." expanded by the respective 
//              client number
//            - Set the maximum nuber of interpreter programs to 64
//            - return value of LinkPrograms may now be also FALSE
//            - LOGOUT function implemented

// 02.02.052 19.2.07
//            - Corrected error at sram reorg:
//            When the sram was invalid, the previous valid copy of the sram could 
//            not be loaded when the sram usage was high.
//            - Comlink TCP/IP:
//            Changed socket options NAGLE and DELAYED_ACK back from on to off.
//            Remark: These options were changed in version 2.2.50 from off to on, but 
//            this change had the drawback that the response times increased dramatically 
//            when more than 4 cpus were involved in communication.

// 02.02.051 19.1.07
//            - DebugIp: 
//            When the application is stopped, incomplete debug-interpreters requests 
//            are canceled to avoid re-execution of the request when the loader is restarted.

// 02.02.050 2.1.07
//            - Comlink TCP/IP: 
//            Removed unecessary TCP/IP calls to reduce CPU-load
//            Changed socket options NAGLE and DELAYED_ACK from off to on
//            - Program interpreter
//            The command P_DIV (unsigned division) incorrect, because register EDX 
//            was not cleared.
//            - Comlink RS232:
//            Corrected error with COM5-COM10 - theses interfaces did not work.

// 02.02.048 1.12.06
//            - Comlink TCP/IP: 
//            Corrected wrong read of ops.tAbsolute

// 02.02.047 6.11.06
//            - DebugIp:
//            When the application stopped at a breakpoint in a write method that was 
//            called from the DebugIp (debug interpreter), then the write method 
//            continued to run after a restart of the application.
//            - SRAM:
//            When the file RAMFILE.DAT (contains SRAM control structures) did not have 
//            the correct length, but was otherwise o.k, then the loader could not 
//            recover from this situation and generated a SRAM error on every start of 
//            the application.
//            - SRAM:
//            The cyclic SRAM check is disabled (it can be re-enabled by activating the 
//            #define CYCLIC_SRAM_CHECK). The reason for disabling this feature is 
//            because the check is not thread-safe. It makes no sense to make it the 
//            check thread-safe because this would be too much overhead.
//            - Program Interpreter
//            When an interpreter programm is stopped programmatically (method 
//            BaseIp::StopProgram), then the last error code is set to PROGIP_ERR_PROGSTOPPED.

// 02.02.046 7.9.06
//            - Comlink: Corrected errors with interface type RS232

// 02.02.045 7.9.06
//            - Comlink: The interface type RS232 is now supported by comlink

// 02.02.044 18.8.06
//            - Comlink TCP/IP
//            It was possible that messages in the TCP/IP comlink protocol were 
//            not transferred correct. The effect was that the values in the refresh-list 
//            were not updated.  
//            - Comlink TCP/IP
//            Disabled NAGLE and DELAYED_ACK socket options to avoid performance degradation.
//            - DebugIp
//            Added missing function call of DebugIp_Init. Without this call it was possible 
//            that a LslCmd connection did not work

// 02.02.043 27.7.06
//            - SRAM:
//            When the SRAM was cleared, the dataValid Flag in the _s_ram_hptr structure 
//            was never set to 1 when no sram reallocations were in the project. 
//            The result was that ram or ramex objects could not store their data in 
//            the sram.
//            A sram reallocation occurs when the size of a ramex object is changed.
//            For example the presence of a StringRam object with an init-value changes 
//            the size of its embedded Ramex Object when the project is started after 
//            an empty sram.
//            - Interpreter programs
//            It ist now possible to create 32 instead of 31 interpreter programs

// 02.02.042 23.6.06
//            - Corrected error at sram reorg:
//            When the project was interrupted during sram reorg before ramfile.dat was 
//            written and the sram layout has changed in the init methods, then it was 
//            possible that the sram data is lost.

// 02.02.041 21.6.06
//            - Comlink: the changes in 2.2.40 (COMLINK_PG) contained an error. This error 
//              resulted in a comlink malfunction, when more than 2 CAN stations were involved 
//              in a comlink communication.

// 02.02.040 12.6.06
//            - Comlink: When the define COMLINK_PG ist set, then comlink uses a protocol 
//              to talk with PG cpu's.

// 02.02.039 6.6.06
//            Interpreter:
//            - Since the loader runs in state reset and lasal can call DebugIp commands, 
//              some commands that access the pointer ops.pProg without checking for a NIL value 
//              can cause an access exception (CMD_REMOVE_ALL_BP).
//            - The interpreter callback function has a new eventType IPRCB_ERROR. The 
//              callback function is called with this eventType when the status (funkSt) of 
//              an interpreter changes to error.

// 02.02.038 15.5.06
//            - Corrected error in project initialization: When a project contained a class 
//              with a class-name shorter than 3 characters, the the project could not be loaded.
//            - The empty dummy lob file Befipr.lob is again included in the project for historical 
//              reasons. The reason for this file is a hardcoded list of lobs in the lasal2 
//              program. This list is used to determine differences in loader lob files.
//              This change has no effect on program behavior.
//
//            Interpreter:
//            - When the application ist not running, the state of all interpreterprograms 
//              is changed to ERROR. This is necessary because interpreterprograms are not 
//              processed in reset state, but the status can be queried from lasal.
//            - The interpreter command StopProgram has now no effect when the target interpreter 
//              is already in funkSt ERROR. In previous versions it was possible that code 
//              from man an already stopped interpreter was executed (e.g. a Kill routine).
//            - A sequential call to another interpreter is now possible when the target 
//              interpreter executes I_RUNPROG and the target interpreter is in the 
//              calling hierarchy of the caller.
//
//            Sram:
//            - Corrected error in sram check: when an error was detected in a ramex cell,
//              a copy of the sram image was nevertheless created for reorganizing the sram. 
//              Now the sram is resetted in the case of an error in a ramex cell.
//            - SRAM: On systems where the ramfile.dat is used, the serial number of the PLC 
//              is stored in file C:\LSLDATA\SERNUM.DAT in order to detect a removeable media 
//              that belongs to another PLC. When a media change is detected, the ramfile.dat 
//              will not be used. This has the effect, that the sram will be resetted or when 
//              a copy from a previous reorg process is found, then the sram will be recovered 
//              from this copy.
//
//            API:
//            - new API function:
//              - ProgMgr_LoadIprProg2: same function as ProgMgr_LoadIprProg, but with an additional 
//                parameter bDoNotLink (default: FALSE). Setting this parameter to TRUE implies 
//                that the caller has already linked the program and the loader does not link it 
//                again.
//            - New include file UserDefOV.h. This is an anpty include file which can be used to 
//              override the default values of the defines in UserDef.h.

// 02.02.037 10.5.06
//            - New API function LDR_InstallIprCallback
//            - New DebugIp command for writing on global variables: I_SET_DATA
//			      - I_GET_DATA command: changed len field from UDINT to USINT
//            - New API function LDR_SetCanWaitRemote

// 02.02.036 4.4.06
//            - Zusätzlicher Parameter bCheck bei der ProgMgr_LoadIprProg Funktion.
//              Wenn dieser Parameter auf TRUE steht, dann wird das Ipr Programm 
//              ungeprüft in den Programmspeicher kopiert. Es werden dann z.B. beim 
//              Laden keine Linkerfehler erkannt.

// 02.02.035 3.4.06
//            - neue DebugIp Befehle zum Auslesen von globalen Variablen:
//              I_GET_GLOBAL_ADDR und I_GET_DATA
//            - neuer DebugIp Befehle (Sammelbefehl zur Statusabfrage von Interpreterprogrammen)
//              I_GETPROGSTATE_ALL
//            - Wenn die Applikation während der SRAM Reorg Phase unterbrochen wird, dann 
//              kann es den Zustand geben, daß das SRAM ungültig ist, aber eine gültige 
//              Kopie im Filesystem existiert. Wenn dann die Applikation wieder gestartet 
//              wird, dann wurde bisher ein SRAM Error gemeldet.
//              Ab dieser Version wird in so einem Fall kein SRAM Error mehr gemeldet, denn 
//              es gehen ja keine SRAM Daten verloren (sie werden von der Filekopie 
//              wiederhergestellt).
//            - Interpreterprogramme:
//              CheckForLoad-Aufruf wird jetzt zyklisch durchgeführt (Aufruf erfolgt in der 
//              ProgSequences Methode)
//            - Wenn über den DebugIp eine NewInst Methode aufgerufen wurde und diese BUSY 
//              zurückgegeben hat, dann wurde die NewInst Methode zyklisch aus der CyWork 
//              Methode des DebugIp Objekts aufgerufen. Das wurde jetzt so geändert, daß 
//              nur mehr 1 Aufruf erfolgt, unabhängig vom Rückgabewert.

// 02.02.034 22.3.06
//            - Fehlerbehebung bei 'Loaderaufruf während !RUN':
//              Es waren RTDB-DebugIp Befehle bei einer nicht initialisierten RTDB möglich.
//            - Fehlerbehebung in ProgMgr_CreateIprProg
//            - Bei einem mit NewObj erzeugten ProgIp (Interpreterprogramm-Objekt) fehlte 
//              das embedded Object 'm_Breakpoints'. Es konnten deswegen keine Breakpoints 
//              in Interpreterprogramme gesetzt werden.

// 02.02.033 6.3.06
//            - Bei OS Versionen < 1.1.96 wurde der SystemInit(FALSE) nur beim 1.Start aufgerufen, 
//              da LOADER_ISINITIALIZED nicht zurückgesetzt wurde.
//            - Interpreteränderungen:
//              - neue Funktionen zum Anlegen und Laden von Interpreterprogrammen:
//                ProgMgr_CreateIprProg, 
//                ProgMgr_LoadIprProg
//              - neue Funktionen um die Abarbeitung von Interpreterprogrammen zu steuern 
//                bzw. zu limitieren:
//                ProgIp_SetMaxProgSeqDuration_ms
//                IprMgr_SetMaxProgSeqDurationAll_ms
//                IprMgr_SetProgSeqCycleTime_ms
//              - neuer Befehl I_LOAD_PROG2 (I_LOAD_PROG hat einen Fehler bei der Checksummenberechnung)
//              - neuer Befehl I_CMD_DEBUGIP. Mit diesem Befehl können die NewInst Methoden 
//                des DebupIp Objekts ohne Kenntnis der DebugIp Objektadresse aufgerufen werden.
//              - neuer DebugIp NewInst Befehl: GET_NBR_OF_IP_PROGS

// 02.02.032 20.2.06
//            - Verbereitung für Loaderaufruf während !RUN

// 02.02.031 9.2.06
//            - MakeSramKopie hat das Makro OS_FILE_READV1 verwendet. Die dahinterliegende 
//              Betriebssystemfunktion gibt es aber erst ab 1.1.28, d.h. bei OS Versionen 
//              kleiner 1.1.28 kam es zu einem Absturz.
//              -> OS_FILE_READV1 durch OS_FILE_READ ersetzt.
//            - Anzahl der max. möglichen Comlink Kanäle(Summe aus CAN und INTERN) auf 15 erhöht.
//              Die Beschränkung durch die COMLINK_CAN_COMCHS Einstellung bleibt aber bestehen.
//              D.h.wenn der COMLINK_CAN_COMCHS Defaultwert von 5 beibehalten wird, dann sind 
//              noch 10 interne Kanäle möglich.
//              Der Sinn dieser Erweiterung besteht darin, daß auch bei mehreren CAN Verbindungen 
//              interne Comlink Kanäle übrig bleiben um z.b. das Befipr Interface über einen 
//              internen Comlink Kanal benutzen zu können.

// 02.02.030 27.1.06
//            - Bei einer falschen Verbindung zu einem Server in einem externen Netzwerk 
//              wird jetzt auch eine Tracemeldung generiert in welcher der Objektname des 
//              Objekts mit dem fehlerhaften Client-Eintag zu sehen ist.
//            - Bei CPUs mit Filesystem (ausgenommen ein Readonly Filesystem) wird das SRAM 
//              beim Loader Start reorganisiert. 
//              Das hat zur Folge, daß beim CIPC das Löschen der Files C:\RAMFILE.* nicht 
//              ausreicht, um das SRAM zu löschen. Es müssen zusätzlich die Files 
//              C:\LSLDATA\SRAM.CPY und C:\LSLDATA\RAMFILE.CPY gelöscht werden.
//            - Neue globale Funktion: SRAM_UnmarkMemory
//              Mit dieser Funktion kann z.B. eine RAMEX Klasse belegten SRAM Speicher als 
//              frei kennzeichnen.

// 02.02.029 15.12.05
//            - neuer DebugIp Befehl I_LSLCMD

// 02.02.028 6.12.05
//						- Teile im Sourcode mit '#ifndef DUMMY_LOADER' geklammert, damit beim 
//              kompilieren des Dummy-Loaders keine Klassen referenziert werden
//              (das würde beim Linken einer Appl mit dem Dummyloader zu einem 
//              Linkerfehler führen)
//            - neues ST-File Loader.st:
//              Wenn in diesem File das '#define DUMMY_LOADER' aktiviert wird, dann kann 
//              ein Loader.lob (Dummy-Loader) kompiliert werden.
//            - Fehlerbehebung in der Funktion _GetObjNameFromAddr:
//              Die Funktion lieferte immer einen Fehler, da geprüft wurde ob der 
//              Obj.Descriptor im UserData Bereich liegt. Seit Lasal2 kann der 
//              Obj.Descriptor aber auch im UserProg Bereich liegen.
// 02.02.027 15.11.05
//						- Globale Variable LSL_LDR_LOADERINFO... in der im Variablennamen 
//              Dinge wie z.B. die Loaderversion codiert sind.
//            - I_READ_CHANNEL schickt ChMode jetzt richtigerweise als UDINT anstatt UINT
//
// 02.02.026 06.10.05
//						- Download on the Fly: Pointer auf ClsHdrConst korrigiert (//ws004)
//
// 02.02.025 28.09.05
//            - Die in 2.2.21 eingebaute Erkennung von verlorengegangenen CAN Packerln
//              war fehlerhaft, wenn die Länge der Antwort 0 war.
//
// 02.02.024 27.09.05
//            - Fehlerbehebung Comlink Verbindungsüberwachung:
//              Wenn unmittelbar nach dem Aktivieren der Überwachung der
//              Status abgefragt wurde, dann wurde fälschlicherweise eine 
//              Unterbrechung angezeigt.
//
// 02.02.023 20.09.05
//            - Erkennung der Rekursion in OpCyclicDebugIp war fehlerhaft
//            - neue Sysvar: _SysTaskMeasEnable
//            - Parameterprüfung bei Befipr-Befehlen (I_READ, I_WRITE etc.)
//            - neues SRAM Format bei der Plattform DESTPLC_TEACHBOX
//
// 02.02.022 19.09.05
//            - neue globale Funktionen für Zugriffe auf Loaderklassen-Elemente:
//              - IprMgr_KillProgs
//              - ProgIp_GetActLine
//             - main() Funktion kann mit dem Parameter C_OK+1 aufgerufen werden. Damit 
//               ist es für das Betriebssystem möglich, die DebugIp Kommunikation am Leben 
//               zu halten, auch wenn die main() - Funktion durch einen Breakpoint im Comlink 
//               angehalten worden ist (z.B.während des Aufrufs einer Init-Methode). 
//
// 02.02.021 08.09.05
//            - Comlink CAN Kommunikation:
//              Wenn beim TXCOMMAND Can Packerl verlorengehen, dann wird das jetzt erkannt 
//              und ein Fehler zurückgegeben.
//            - Fehler in der Kommunikationsüberwachung behoben
//
// 02.02.020 25.08.05
//            - Rückgabewerte von CanTxObj werden berücksichtigt, da eventuell ohne die 
//				      CAN-Bremse ein Bufferüberlauf auftreten kann (Änderungsmarkierung: //ws003)
//
// 02.02.019 11.08.05
//            - I_GET_OBJ schickt ChMode jetzt richtigerweise als UDINT anstatt UINT
//
// 02.02.018 10.08.05
//            - I-am-Alive Mechanismus beim Comlink
//            - globale Funktion ProgIp_SetNextInstructionLabelNo
//
// 02.02.017 04.08.05
//            - neue globale Funktion LDR_SetCanWait( usint ): ermöglicht das Setzen der 
//              bislang fixen Waitstates bei der CAN-Kommunikation.
//              Der Parameter gibt die Wartezeit in ms an. (Änderungsmarkierung: //ws001)
//			      - Kompatibilität mit Lasal Class Loader (Änderungsmarkierung: //ws002)
//
// 02.02.015 12.07.05	
//            - Wenn beim Laden der RAMFILE.DAT ein CRC Fehler festgestellt wird, dann werden 
//						  die Diagnosefiles C:\RAMFILE.ER1 - C:\RAMFILE.ER4 erstellt.
//            - neue Systemvariable: _CDIASconfig
//            - Methode SetNextInstructionLabelNo der ProgIp Klasse von Privat auf Global geändert
//
// 02.02.014 30.06.05
//						- DebugIp: 
//						  Beim Freigeben eines DebugIp Objekts wurde die die mit EXCLUSIVE allokierte 
//						  CommandTable nicht freigegeben, was zu einem Memory Leak führte (bei jedem 
//						  Online + Offline auf dem Lasal).
// 02.02.013 14.06.05
//            - Die Anzahl der möglichen CAN Comlink Channels ist über das
//              #define COMLINK_CAN_COMCHS einstellbar (Standardwert: 5)
//            - Die Elementnamen _aDynList und _aStatList der Struktur comData wurden wieder
//              auf den ursprünglichen Namen aDynList und aStatList zurückgeändert, da
//              Anwendungen existieren, die direkt auf dieses Array zugreifen.
//            - neue Sysvar:
//                _ClockTicks
//            - TXCOMMAND initialisiert jetzt vor dem Aufruf der Befipr Methode das
//              Längenfeld der Antwort mit -1 (Com._m.resu.uilng)
//            - LOGIN hat eine Exception verursacht, wenn versucht wurde mehr als die
//              max. mögliche Anzahl von Verbindungen aufzubauen.
//            - Das Mutex gegen Wiedereintritt in der Funktion ComlinkT_Login entfernt, 
//              da dieser Code Thread-Safe ist.
// 02.02.012 01.06.05
//            - Bei einem Objektkanal, dessen referenzierte Klasse nicht vorhanden ist,
//              kam es zu einem Absturz. Dieser ist jetzt abgefangen.
//              In diesem Fall ist der Anwender dafür verantwortlich, keinen Aufruf
//              über den Objektkanal zu machen.
// 02.02.011 28.4.05
//            - Wrapperfunktionen für globale Methoden angelegt:
//              ProgMgr_GetProgram -> ProgMgr::GetProgram
//              IprMgr_GetInterpreter -> IprMgr::GetInterpreter
//              ClassReader_NewCls -> ClassReader::NewCls
//            - Wrapperfunktionen für Methoden der Klasse ProgIp angelegt:
//              <ProgIpPtr>^.<MemberFunction>(<paras>) -> ProgIp_<MemberFunction>(ProgIpPtr,<paras>)
//
// 02.02.010 20.4.05
//            - neue Systemvariablen: _rebootOnError, _isRebootCodeAvailable, _rebootCode
//
// 02.02.009 15.02.05	
//            - Wenn von der Anwendung Speicher über eine Loaderfunktion angefordert wird, dann 
//						  wird jetzt auch einen Kennung PRG (13) und nicht LDR (15) vergeben
//						  (z.B. Merker, MerkerEx od. ein direkter Aufruf von _alloc_resize).
//
//
// 02.02.008 11.02.05	
//            - Bei einem leeren RAMFILE.DAT (udentries = 0) wird jetzt kein SRAM Error mehr generiert.
//						- Bei einem neuen CIPC (kein RAMFILE.DAT und keine RAM-Zellen) wird ein leeres RAMFILE.DAT angelegt.
//						- Beim Reservieren von Speicherblöcken (malloc) wird die Kennung 15 mitgegeben.
//            - Wenn der Loader eine fehlerhafte RAMEX Zelle gefunden hat, dann wurde 
//						  sie zwar im SRAM und in der Filestruktur für ungültig erklärt, die Änderungen 
//						  wurden aber nicht ins File (RAMFILE.DAT) geschrieben.
//						  Das hatte zur Folge, daß bei jedem Start eine Tracemeldung 'sram error ...'
//						  gekommen ist. 
//						- SRAM: wenn udEntries = 0 ist, dann wird kein SRAM Fehler mehr gemeldet
//						- Neue globale OS-Variable: _heapAllocCnt. Gibt die Anzahl der angeforderten 
//						  Speicherblöcke an.
//						- DebugIp: 
//						  Beim Freigeben eines DebugIp Objekts wurde die die mit EXCLUSIVE allokierte 
//						  CommandTable nicht freigegeben, was zu einem Memory Leak führte (bei jedem 
//						  Online + Offline auf dem Lasal).
//						- Comlink:
//						  TxUpd wurde um einen Returnwert erweitert. Die Funktion scheitert, wenn 
//						  der Index der einzutragenden Zelle größer ist als die Refreshlistengröße 
//						  des Servers. Wenn die Refreshlistengröße des Servers nicht bekannt ist 
//						  (z.B. bei einem alter Loader) dann wird die Größe der eigenen Liste zur 
//						  Überprüfung verwendet.
// 02.02.007  2.2.05
//            - ProjectReader::CreateClass hat die Basisklasse einer Klasse 
//              angelegt, auch wenn diese schon existierte.
//              Das hat zu einem Fehler geführt, wenn aus ClassReader::CheckClsSyntax 
//              eine Klasse neu angelegt wurde, weil bereits initialisierte Werte im ClsHdr 
//              durch erneutes Einlesen der CT-Tabelle überschrieben wurden.
// 02.02.006  1.2.05
//            - ScanLexem hat bei einem Token-Typ ungleich SN_ID (z.B. ein Leerstring)
//              ein undefiniertes Ergebnis in der Ouput-Variablen Lexem hinterlassen.
// 02.02.005  24.1.05
//            - #defines COMLINK_TCP_SERVER + COMLINK_TCP_CLIENT haben gefehlt
// 02.02.004  20.1.05
//            - _LookUpObjLex war fehlerhaft (Fehler hat sich nur bei 386er CPUs ausgewirkt)
// 02.02.003  19.1.05
//            - Änderungen im alten Loader seit Okt.2004 übernommen:
//              - neue OS Variablen in RTOS_Variables.st
//              - SRAM Prüfung
//              - SRAM Fehlerbehebung
//              - Comlink Refreshlisten-Problem behoben
// 02.02.002  12.1.05
//            - ONs ohne Objekte führen zu keinem Fehler mehr
//            - deutsche TRACE Meldungen auf englisch umgestellt
//            - Tabellennamen der IPR-Conf Tabelle von IPR_CONF_ auf IPR_CONF geändert
//              (IPR_CONF_ war ein Test)
// 02.02      Magic Clients neu behandelt (eigener Connect-Abschnitt)
// 02.xx      LASAL 2 Loader haben highest revision >= 2
// 01.01      LASAL 1 Loader haben highest revision <  2
//
// 01.01.001 28.10.04	- Umstellung auf neues Versionsnummernschema:
//						    xx.yy.zzz
//						    !  !  \___ 0-255
//						    !  \______ 1-15
//						    \_________ 1-15
//						  xx.yy ist im bisherigen REV_HI-Byte codiert - die oberen 4 Bit sind xx, 
//						  die unteren 4 Bit sind yy.



