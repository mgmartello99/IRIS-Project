//============= Parametri Generali =============
#define MotorMaxCurrent   700 

#define M_V_MAX           150000 //15 [grad/s]
#define M_V_MIN           50000 //5 [grad/s]

#define M7_V_MAX          1500000 //15 [grad/s]
#define M7_V_MIN          1000000 //5 [grad/s]

#define M_A_MAX           1500000
#define M_J_MAX           15000000

#define MINIMUM_ANGLE     10000 //1 DEGREE
#define V_mov             100000



//================ Motore M1 ===================

#define P_M1            3600000 //posizione per homing
#define P_M1_C          1900000 //Posizione accoppiamento slider
#define V_M1            150000 
#define A_M1            1000000  
#define J_M1            10000000 

#define P_NoStuck       230000 //10 gradi


//================ Motore M2 ===================

#define P_M2_Homing     3600000  //da verificare il numero di 0; voglio 270°
#define P_M2_Charge     3600000  //da verificare il numero di 0; voglio 270°
#define V_M2            120000   //Velocità || da verificare
#define A_M2            1000000  //Accelerazione || da verificare
#define J_M2            10000000 //Jerk|| da verificare


//================ Motore M3 ===================

#define P_M3            3600000 //def velocità prova
#define P_M3_C          1910000 //Valore iniziale 1930000
#define V_M3            150000 //def velocità prova
#define A_M3            1000000  //Accelerazione || da verificare
#define J_M3            10000000 //Jerk|| da verificare

//================ Motore M4 ===================
#define V_M4            80000   //Velocità || da verificare
#define A_M4            1000000  //Accelerazione || da verificare
#define J_M4            10000000 //Jerk|| da verificare

#define P_M4_H          3600000 //def posizione prova

#define P1_M4           3400000   //da verificare il numero di 0; voglio 45°
#define P2_M4           950000   //da verificare il numero di 0; voglio 45°
#define P3_M4           1400000   //da verificare il numero di 0; voglio 45°

//================ Motore M5 ===================

#define V_M5            150000   //Velocità || da verificare
#define A_M5            1000000  //Accelerazione || da verificare
#define J_M5            10000000 //Jerk|| da verificare

#define P_M5_H          3600000 //

//-Charging 
#define P1_M5           -2370000   //da verificare il numero di 0; voglio 45° //-24200
#define P2_M5           -4800000   //da verificare il numero di 0; voglio 45° //-48200
#define P3_M5           -7120000   //da verificare il numero di 0; voglio 45° //-72200

//-Irradiation
#define P1_M5_I         450000   //da verificare 
#define P2_M5_I         2750000   //da verificare 
#define P3_M5_I         5050000   //da verificare 

//-Dischargin
#define P1_M5_D         1100000   //da verificare il numero di 0; voglio 45°
#define P2_M5_D         920000   //da verificare il numero di 0; voglio 45°
#define P3_M5_D         1430000   //da verificare il numero di 0; voglio 45°

#define P_M5_D          2500000
#define OffsetDischarge 2400000


//================ Motore M6 ===================

#define P_M6            36000000 //def velocità prova - 10 giri
#define V_M6            250000 //def velocità prova
#define A_M6            1000000  //Accelerazione || da verificare
#define J_M6            10000000 //Jerk|| da verificare
//================ Motore M8 ===================

#define P_M8            36000000 //def velocità prova - 10 giri
#define V_M8            1250000 //def velocità prova
#define A_M8            1000000  //Accelerazione || da verificare
#define J_M8            10000000 //Jerk|| da verificare