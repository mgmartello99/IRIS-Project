#ifndef _T_GRC_H
#pragma once
 #define _T_GRC_H

//#define HIRES      // 800*600/1024*768 pixel

                                       // XXXX XXXX --XX XXXX XXXX XXXX XXXX XXXX
#define T_COPY                    0UL  // ---- ---- ---- ---- ---- ---- ---- --00
#define T_XOR                     1UL  // ---- ---- ---- ---- ---- ---- ---- --01
#define T_OR                      2UL  // ---- ---- ---- ---- ---- ---- ---- --10
#define T_COMBI                   3UL  // (T_XOR|T_OR|T_COPY)

// T_3D is also used in circles for arcmode
#define T_3D                      4UL  // ---- ---- ---- ---- ---- ---- ---- -1--
#define T_FILL                    8UL  // ---- ---- ---- ---- ---- ---- ---- 1---
#define T_INVRS                  16UL  // ---- ---- ---- ---- ---- ---- ---1 ----
#define T_BLINK                  32UL  // ---- ---- ---- ---- ---- ---- --1- ----
#define T_ROUND                  64UL  // ---- ---- ---- ---- ---- ---- -1-- ----
#define T_PROP                  128UL  // ---- ---- ---- ---- ---- ---- 1--- ----

#define T_BOLD                 8192UL  // ---- ---- ---- ---- --1- ---- ---- ----
#define T_ITALIC              16384UL  // ---- ---- ---- ---- -1-- ---- ---- ----
#define T_UNDERLINE           32768UL  // ---- ---- ---- ---- 1--- ---- ---- ----
#define T_STRIKEOUT          262144UL  // ---- ---- ---- -1-- ---- ---- ---- ----
#define T_OUTLINE            524288UL  // ---- ---- ---- 1--- ---- ---- ---- ----

#define T_DUPX            268435456UL  // 0001 ---- ---- ---- ---- ---- ---- ----
#define T_DUPY             16777216UL  // ---- 0001 ---- ---- ---- ---- ---- ----
#define T_XSIZE          4026531840UL  // 1111 ---- ---- ---- ---- ---- ---- ----
#define T_YSIZE           251658240UL  // ---- 1111 ---- ---- ---- ---- ---- ----
#define T_SIZE           4278190080UL  // (T_XSIZE|T_YSIZE)

#define T_LEFTBOUND               0UL  // ---- ---- ---- --00 ---- ---- ---- ----
#define T_RIGHTBOUND          65536UL  // ---- ---- ---- --01 ---- ---- ---- ----
#define T_CENTERBOUND        131072UL  // ---- ---- ---- --10 ---- ---- ---- ----
#define T_UPBOUND                 0UL  // ---- ---- --00 ---- ---- ---- ---- ----
#define T_DOWNBOUND         1048576UL  // ---- ---- --01 ---- ---- ---- ---- ----
#define T_MIDBOUND          2097152UL  // ---- ---- --10 ---- ---- ---- ---- ----
#define T_HORIZBOUND         196608UL  // (T_CENTERBOUND|T_RIGHTBOUND|T_LEFTBOUND)
#define T_VERTBOUND         3145728UL  // (T_UPBOUND|T_DOWNBOUND|T_MIDBOUND)
#define T_BOUND             3342336UL  // (T_VERTBOUND|T_HORIZBOUND)

#define T_PAT00                   0UL  // ---- ---- ---- ---- ---0 0000 ---- ----
#define T_PAT01                 256UL  // ---- ---- ---- ---- ---0 0001 ---- ----
#define T_PAT02                 512UL  // ---- ---- ---- ---- ---0 0010 ---- ----
#define T_PAT03                 768UL  // ---- ---- ---- ---- ---0 0011 ---- ----
#define T_PAT04                1024UL  // ---- ---- ---- ---- ---0 0100 ---- ----
#define T_PAT05                1280UL  // ---- ---- ---- ---- ---0 0101 ---- ----
#define T_PAT06                1536UL  // ---- ---- ---- ---- ---0 0110 ---- ----
#define T_PAT07                1792UL  // ---- ---- ---- ---- ---0 0111 ---- ----
#define T_PAT08                2048UL  // ---- ---- ---- ---- ---0 1000 ---- ----
#define T_PAT09                2304UL  // ---- ---- ---- ---- ---0 1001 ---- ----
#define T_PAT10                2560UL  // ---- ---- ---- ---- ---0 1010 ---- ----
#define T_PAT11                2816UL  // ---- ---- ---- ---- ---0 1011 ---- ----
#define T_PAT12                3072UL  // ---- ---- ---- ---- ---0 1100 ---- ----
#define T_PAT13                3328UL  // ---- ---- ---- ---- ---0 1101 ---- ----
#define T_PAT14                3584UL  // ---- ---- ---- ---- ---0 1110 ---- ----
#define T_PAT15                3840UL  // ---- ---- ---- ---- ---0 1111 ---- ----
#define T_PAT16                4096UL  // ---- ---- ---- ---- ---1 0000 ---- ----
#define T_PAT17                4352UL  // ---- ---- ---- ---- ---1 0001 ---- ----
#define T_PAT18                4608UL  // ---- ---- ---- ---- ---1 0010 ---- ----
#define T_PAT19                4864UL  // ---- ---- ---- ---- ---1 0011 ---- ----
#define T_PAT20                5120UL  // ---- ---- ---- ---- ---1 0100 ---- ----
#define T_PAT21                5376UL  // ---- ---- ---- ---- ---1 0101 ---- ----
#define T_PAT22                5632UL  // ---- ---- ---- ---- ---1 0110 ---- ----
#define T_PAT23                5888UL  // ---- ---- ---- ---- ---1 0111 ---- ----
#define T_PAT24                6144UL  // ---- ---- ---- ---- ---1 1000 ---- ----
#define T_PAT25                6400UL  // ---- ---- ---- ---- ---1 1001 ---- ----
#define T_PAT26                6656UL  // ---- ---- ---- ---- ---1 1010 ---- ----
#define T_PAT27                6912UL  // ---- ---- ---- ---- ---1 1011 ---- ----
#define T_PAT28                7168UL  // ---- ---- ---- ---- ---1 1100 ---- ----
#define T_PAT29                7424UL  // ---- ---- ---- ---- ---1 1101 ---- ----
#define T_PAT30                7680UL  // ---- ---- ---- ---- ---1 1110 ---- ----
#define T_PAT31                7936UL  // ---- ---- ---- ---- ---1 1111 ---- ----

#define T_PATTERN              7936UL
#define T_SOLID             T_PAT01
#define T_DOTTED            T_PAT03
#define T_DASHED            T_PAT14
#define T_CENTER            T_PAT08

#define DEFFONT                 255UL                                // font init
#define DEFCOLOR         4294901758UL // 0xFFFEFFFE        // color (_COLOR) init
#define INVISIBLE             65534UL // 0xFFFE
#define DEFATTRIB        4294967295UL // 0xFFFFFFFF   // attribute (_ATTRIB) init
#define DEFKOORD              32000UL                               // koord init

#ifdef BLACK
 #undef BLACK
#endif
#ifdef BLUE
 #undef BLUE
#endif
#ifdef GREEN
 #undef GREEN
#endif
#ifdef CYAN
 #undef CYAN
#endif
#ifdef RED
 #undef RED
#endif
#ifdef MAGENTA
 #undef MAGENTA
#endif
#ifdef BROWN
 #undef BROWN
#endif
#ifdef LIGHTGRAY
 #undef LIGHTGRAY
#endif
#ifdef DARKGRAY
 #undef DARKGRAY
#endif
#ifdef LIGHTBLUE
 #undef LIGHTBLUE
#endif
#ifdef LIGHTGREEN
 #undef LIGHTGREEN
#endif
#ifdef LIGHTCYAN
 #undef LIGHTCYAN
#endif
#ifdef LIGHTRED
 #undef LIGHTRED
#endif
#ifdef LIGHTMAGENTA
 #undef LIGHTMAGENTA
#endif
#ifdef YELLOW
 #undef YELLOW
#endif
#ifdef WHITE
 #undef WHITE
#endif

#ifdef UC_HICOLOR
 #define BLACK              0 //0x0000 //MakeTrueColor(  0,   0,   0)
 #define BLUE              21 //0x0015 //MakeTrueColor(  0,   0, 168)
 #define GREEN           1344 //0x0540 //MakeTrueColor(  0, 168,   0)
 #define CYAN            1365 //0x0555 //MakeTrueColor(  0, 168, 168)
 #define RED            43008 //0xA800 //MakeTrueColor(168,   0,   0)
 #define MAGENTA        43029 //0xA815 //MakeTrueColor(168,   0, 168)
 #define BROWN          43680 //0xAAA0 //MakeTrueColor(168,  84,   0)
 #define LIGHTGRAY      44373 //0xAD55 //MakeTrueColor(168, 168, 168)
 #define DARKGRAY       21162 //0x52AA //MakeTrueColor( 84,  84,  84)
 #define LIGHTBLUE      21183 //0x52BF //MakeTrueColor( 84,  84, 255)
 #define LIGHTGREEN     26602 //0x67EA //MakeTrueColor( 84, 255,  84)
 #define LIGHTCYAN      22527 //0x57FF //MakeTrueColor( 84, 255, 255)
 #define LIGHTRED       64170 //0xFAAA //MakeTrueColor(255,  84,  84)
 #define LIGHTMAGENTA   64191 //0xFABF //MakeTrueColor(255,  84, 255)
 #define YELLOW         65514 //0xFFEA //MakeTrueColor(255, 255,  84)
 #define WHITE          65535 //0xFFFF //MakeTrueColor(255, 255, 255)
#else
 #define BLACK              0 //0x0000 //MakeTrueColor(  0,   0,   0)
 #define BLUE               1 //0x0015 //MakeTrueColor(  0,   0, 168)
 #define GREEN              2 //0x0540 //MakeTrueColor(  0, 168,   0)
 #define CYAN               3 //0x0555 //MakeTrueColor(  0, 168, 168)
 #define RED                4 //0xA800 //MakeTrueColor(168,   0,   0)
 #define MAGENTA            5 //0xA815 //MakeTrueColor(168,   0, 168)
 #define BROWN              6 //0xAAA0 //MakeTrueColor(168,  84,   0)
 #define LIGHTGRAY          7 //0xAD55 //MakeTrueColor(168, 168, 168)
 #define DARKGRAY           8 //0x52AA //MakeTrueColor( 84,  84,  84)
 #define LIGHTBLUE          9 //0x52BF //MakeTrueColor( 84,  84, 255)
 #define LIGHTGREEN        10 //0x575A //MakeTrueColor( 84, 255,  84)
 #define LIGHTCYAN         11 //0x57FF //MakeTrueColor( 84, 255, 255)
 #define LIGHTRED          12 //0xFAAA //MakeTrueColor(255,  84,  84)
 #define LIGHTMAGENTA      13 //0xFABF //MakeTrueColor(255,  84, 255)
 #define YELLOW            14 //0xFFEA //MakeTrueColor(255, 255,  84)
 #define WHITE             15 //0xFFFF //MakeTrueColor(255, 255, 255)
#endif

//#define BLACK          MakeColorFromIndex(0)
//#define BLUE           MakeColorFromIndex(1)
//#define GREEN          MakeColorFromIndex(2)
//#define CYAN           MakeColorFromIndex(3)
//#define RED            MakeColorFromIndex(4)
//#define MAGENTA        MakeColorFromIndex(5)
//#define BROWN          MakeColorFromIndex(6)
//#define LIGHTGRAY      MakeColorFromIndex(7)
//#define DARKGRAY       MakeColorFromIndex(8)
//#define LIGHTBLUE      MakeColorFromIndex(9)
//#define LIGHTGREEN     MakeColorFromIndex(10)
//#define LIGHTCYAN      MakeColorFromIndex(11)
//#define LIGHTRED       MakeColorFromIndex(12)
//#define LIGHTMAGENTA   MakeColorFromIndex(13)
//#define YELLOW         MakeColorFromIndex(14)
//#define WHITE          MakeColorFromIndex(15)

#define LCD_SETVALUE 4
#define LCD_STANDARD 3
#define LCD_INCREASE 2
#define LCD_DECREASE 1
#define LCD_GETVALUE 0

#define GRC_SET_PIXELCORRECTION     1  // include right bottom pixel
#define GRC_RESET_PIXELCORRECTION   2  // exclude right bottom pixel
#define GRC_SET_ORIGINCOLOR         4  // color: normal mode
#define GRC_SET_NAVYCOLOR           5  // color: navy mode (red)
#define GRC_XFREEZE_SCREEN          7  // Screnn refresh start/stop
#define GRC_USE_SFB                 8  // Use Shadow Frame Buffer



#endif // _T_GRC_H

#ifndef LSE_COLOR256
#pragma once

#define LSE_COLOR1     MakeColorFromIndex(0)
#define LSE_COLOR2     MakeColorFromIndex(1)
#define LSE_COLOR3     MakeColorFromIndex(2)
#define LSE_COLOR4     MakeColorFromIndex(3)
#define LSE_COLOR5     MakeColorFromIndex(4)
#define LSE_COLOR6     MakeColorFromIndex(5)
#define LSE_COLOR7     MakeColorFromIndex(6)
#define LSE_COLOR8     MakeColorFromIndex(7)
#define LSE_COLOR9     MakeColorFromIndex(8)
#define LSE_COLOR10    MakeColorFromIndex(9)
#define LSE_COLOR11    MakeColorFromIndex(10)
#define LSE_COLOR12    MakeColorFromIndex(11)
#define LSE_COLOR13    MakeColorFromIndex(12)
#define LSE_COLOR14    MakeColorFromIndex(13)
#define LSE_COLOR15    MakeColorFromIndex(14)
#define LSE_COLOR16    MakeColorFromIndex(15)
#define LSE_COLOR17    MakeColorFromIndex(16)
#define LSE_COLOR18    MakeColorFromIndex(17)
#define LSE_COLOR19    MakeColorFromIndex(18)
#define LSE_COLOR20    MakeColorFromIndex(19)
#define LSE_COLOR21    MakeColorFromIndex(20)
#define LSE_COLOR22    MakeColorFromIndex(21)
#define LSE_COLOR23    MakeColorFromIndex(22)
#define LSE_COLOR24    MakeColorFromIndex(23)
#define LSE_COLOR25    MakeColorFromIndex(24)
#define LSE_COLOR26    MakeColorFromIndex(25)
#define LSE_COLOR27    MakeColorFromIndex(26)
#define LSE_COLOR28    MakeColorFromIndex(27)
#define LSE_COLOR29    MakeColorFromIndex(28)
#define LSE_COLOR30    MakeColorFromIndex(29)
#define LSE_COLOR31    MakeColorFromIndex(30)
#define LSE_COLOR32    MakeColorFromIndex(31)
#define LSE_COLOR33    MakeColorFromIndex(32)
#define LSE_COLOR34    MakeColorFromIndex(33)
#define LSE_COLOR35    MakeColorFromIndex(34)
#define LSE_COLOR36    MakeColorFromIndex(35)
#define LSE_COLOR37    MakeColorFromIndex(36)
#define LSE_COLOR38    MakeColorFromIndex(37)
#define LSE_COLOR39    MakeColorFromIndex(38)
#define LSE_COLOR40    MakeColorFromIndex(39)
#define LSE_COLOR41    MakeColorFromIndex(40)
#define LSE_COLOR42    MakeColorFromIndex(41)
#define LSE_COLOR43    MakeColorFromIndex(42)
#define LSE_COLOR44    MakeColorFromIndex(43)
#define LSE_COLOR45    MakeColorFromIndex(44)
#define LSE_COLOR46    MakeColorFromIndex(45)
#define LSE_COLOR47    MakeColorFromIndex(46)
#define LSE_COLOR48    MakeColorFromIndex(47)
#define LSE_COLOR49    MakeColorFromIndex(48)
#define LSE_COLOR50    MakeColorFromIndex(49)
#define LSE_COLOR51    MakeColorFromIndex(50)
#define LSE_COLOR52    MakeColorFromIndex(51)
#define LSE_COLOR53    MakeColorFromIndex(52)
#define LSE_COLOR54    MakeColorFromIndex(53)
#define LSE_COLOR55    MakeColorFromIndex(54)
#define LSE_COLOR56    MakeColorFromIndex(55)
#define LSE_COLOR57    MakeColorFromIndex(56)
#define LSE_COLOR58    MakeColorFromIndex(57)
#define LSE_COLOR59    MakeColorFromIndex(58)
#define LSE_COLOR60    MakeColorFromIndex(59)
#define LSE_COLOR61    MakeColorFromIndex(60)
#define LSE_COLOR62    MakeColorFromIndex(61)
#define LSE_COLOR63    MakeColorFromIndex(62)
#define LSE_COLOR64    MakeColorFromIndex(63)
#define LSE_COLOR65    MakeColorFromIndex(64)
#define LSE_COLOR66    MakeColorFromIndex(65)
#define LSE_COLOR67    MakeColorFromIndex(66)
#define LSE_COLOR68    MakeColorFromIndex(67)
#define LSE_COLOR69    MakeColorFromIndex(68)
#define LSE_COLOR70    MakeColorFromIndex(69)
#define LSE_COLOR71    MakeColorFromIndex(70)
#define LSE_COLOR72    MakeColorFromIndex(71)
#define LSE_COLOR73    MakeColorFromIndex(72)
#define LSE_COLOR74    MakeColorFromIndex(73)
#define LSE_COLOR75    MakeColorFromIndex(74)
#define LSE_COLOR76    MakeColorFromIndex(75)
#define LSE_COLOR77    MakeColorFromIndex(76)
#define LSE_COLOR78    MakeColorFromIndex(77)
#define LSE_COLOR79    MakeColorFromIndex(78)
#define LSE_COLOR80    MakeColorFromIndex(79)
#define LSE_COLOR81    MakeColorFromIndex(80)
#define LSE_COLOR82    MakeColorFromIndex(81)
#define LSE_COLOR83    MakeColorFromIndex(82)
#define LSE_COLOR84    MakeColorFromIndex(83)
#define LSE_COLOR85    MakeColorFromIndex(84)
#define LSE_COLOR86    MakeColorFromIndex(85)
#define LSE_COLOR87    MakeColorFromIndex(86)
#define LSE_COLOR88    MakeColorFromIndex(87)
#define LSE_COLOR89    MakeColorFromIndex(88)
#define LSE_COLOR90    MakeColorFromIndex(89)
#define LSE_COLOR91    MakeColorFromIndex(90)
#define LSE_COLOR92    MakeColorFromIndex(91)
#define LSE_COLOR93    MakeColorFromIndex(92)
#define LSE_COLOR94    MakeColorFromIndex(93)
#define LSE_COLOR95    MakeColorFromIndex(94)
#define LSE_COLOR96    MakeColorFromIndex(95)
#define LSE_COLOR97    MakeColorFromIndex(96)
#define LSE_COLOR98    MakeColorFromIndex(97)
#define LSE_COLOR99    MakeColorFromIndex(98)
#define LSE_COLOR100   MakeColorFromIndex(99)
#define LSE_COLOR101   MakeColorFromIndex(100)
#define LSE_COLOR102   MakeColorFromIndex(101)
#define LSE_COLOR103   MakeColorFromIndex(102)
#define LSE_COLOR104   MakeColorFromIndex(103)
#define LSE_COLOR105   MakeColorFromIndex(104)
#define LSE_COLOR106   MakeColorFromIndex(105)
#define LSE_COLOR107   MakeColorFromIndex(106)
#define LSE_COLOR108   MakeColorFromIndex(107)
#define LSE_COLOR109   MakeColorFromIndex(108)
#define LSE_COLOR110   MakeColorFromIndex(109)
#define LSE_COLOR111   MakeColorFromIndex(110)
#define LSE_COLOR112   MakeColorFromIndex(111)
#define LSE_COLOR113   MakeColorFromIndex(112)
#define LSE_COLOR114   MakeColorFromIndex(113)
#define LSE_COLOR115   MakeColorFromIndex(114)
#define LSE_COLOR116   MakeColorFromIndex(115)
#define LSE_COLOR117   MakeColorFromIndex(116)
#define LSE_COLOR118   MakeColorFromIndex(117)
#define LSE_COLOR119   MakeColorFromIndex(118)
#define LSE_COLOR120   MakeColorFromIndex(119)
#define LSE_COLOR121   MakeColorFromIndex(120)
#define LSE_COLOR122   MakeColorFromIndex(121)
#define LSE_COLOR123   MakeColorFromIndex(122)
#define LSE_COLOR124   MakeColorFromIndex(123)
#define LSE_COLOR125   MakeColorFromIndex(124)
#define LSE_COLOR126   MakeColorFromIndex(125)
#define LSE_COLOR127   MakeColorFromIndex(126)
#define LSE_COLOR128   MakeColorFromIndex(127)
#define LSE_COLOR129   MakeColorFromIndex(128)
#define LSE_COLOR130   MakeColorFromIndex(129)
#define LSE_COLOR131   MakeColorFromIndex(130)
#define LSE_COLOR132   MakeColorFromIndex(131)
#define LSE_COLOR133   MakeColorFromIndex(132)
#define LSE_COLOR134   MakeColorFromIndex(133)
#define LSE_COLOR135   MakeColorFromIndex(134)
#define LSE_COLOR136   MakeColorFromIndex(135)
#define LSE_COLOR137   MakeColorFromIndex(136)
#define LSE_COLOR138   MakeColorFromIndex(137)
#define LSE_COLOR139   MakeColorFromIndex(138)
#define LSE_COLOR140   MakeColorFromIndex(139)
#define LSE_COLOR141   MakeColorFromIndex(140)
#define LSE_COLOR142   MakeColorFromIndex(141)
#define LSE_COLOR143   MakeColorFromIndex(142)
#define LSE_COLOR144   MakeColorFromIndex(143)
#define LSE_COLOR145   MakeColorFromIndex(144)
#define LSE_COLOR146   MakeColorFromIndex(145)
#define LSE_COLOR147   MakeColorFromIndex(146)
#define LSE_COLOR148   MakeColorFromIndex(147)
#define LSE_COLOR149   MakeColorFromIndex(148)
#define LSE_COLOR150   MakeColorFromIndex(149)
#define LSE_COLOR151   MakeColorFromIndex(150)
#define LSE_COLOR152   MakeColorFromIndex(151)
#define LSE_COLOR153   MakeColorFromIndex(152)
#define LSE_COLOR154   MakeColorFromIndex(153)
#define LSE_COLOR155   MakeColorFromIndex(154)
#define LSE_COLOR156   MakeColorFromIndex(155)
#define LSE_COLOR157   MakeColorFromIndex(156)
#define LSE_COLOR158   MakeColorFromIndex(157)
#define LSE_COLOR159   MakeColorFromIndex(158)
#define LSE_COLOR160   MakeColorFromIndex(159)
#define LSE_COLOR161   MakeColorFromIndex(160)
#define LSE_COLOR162   MakeColorFromIndex(161)
#define LSE_COLOR163   MakeColorFromIndex(162)
#define LSE_COLOR164   MakeColorFromIndex(163)
#define LSE_COLOR165   MakeColorFromIndex(164)
#define LSE_COLOR166   MakeColorFromIndex(165)
#define LSE_COLOR167   MakeColorFromIndex(166)
#define LSE_COLOR168   MakeColorFromIndex(167)
#define LSE_COLOR169   MakeColorFromIndex(168)
#define LSE_COLOR170   MakeColorFromIndex(169)
#define LSE_COLOR171   MakeColorFromIndex(170)
#define LSE_COLOR172   MakeColorFromIndex(171)
#define LSE_COLOR173   MakeColorFromIndex(172)
#define LSE_COLOR174   MakeColorFromIndex(173)
#define LSE_COLOR175   MakeColorFromIndex(174)
#define LSE_COLOR176   MakeColorFromIndex(175)
#define LSE_COLOR177   MakeColorFromIndex(176)
#define LSE_COLOR178   MakeColorFromIndex(177)
#define LSE_COLOR179   MakeColorFromIndex(178)
#define LSE_COLOR180   MakeColorFromIndex(179)
#define LSE_COLOR181   MakeColorFromIndex(180)
#define LSE_COLOR182   MakeColorFromIndex(181)
#define LSE_COLOR183   MakeColorFromIndex(182)
#define LSE_COLOR184   MakeColorFromIndex(183)
#define LSE_COLOR185   MakeColorFromIndex(184)
#define LSE_COLOR186   MakeColorFromIndex(185)
#define LSE_COLOR187   MakeColorFromIndex(186)
#define LSE_COLOR188   MakeColorFromIndex(187)
#define LSE_COLOR189   MakeColorFromIndex(188)
#define LSE_COLOR190   MakeColorFromIndex(189)
#define LSE_COLOR191   MakeColorFromIndex(190)
#define LSE_COLOR192   MakeColorFromIndex(191)
#define LSE_COLOR193   MakeColorFromIndex(192)
#define LSE_COLOR194   MakeColorFromIndex(193)
#define LSE_COLOR195   MakeColorFromIndex(194)
#define LSE_COLOR196   MakeColorFromIndex(195)
#define LSE_COLOR197   MakeColorFromIndex(196)
#define LSE_COLOR198   MakeColorFromIndex(197)
#define LSE_COLOR199   MakeColorFromIndex(198)
#define LSE_COLOR200   MakeColorFromIndex(199)
#define LSE_COLOR201   MakeColorFromIndex(200)
#define LSE_COLOR202   MakeColorFromIndex(201)
#define LSE_COLOR203   MakeColorFromIndex(202)
#define LSE_COLOR204   MakeColorFromIndex(203)
#define LSE_COLOR205   MakeColorFromIndex(204)
#define LSE_COLOR206   MakeColorFromIndex(205)
#define LSE_COLOR207   MakeColorFromIndex(206)
#define LSE_COLOR208   MakeColorFromIndex(207)
#define LSE_COLOR209   MakeColorFromIndex(208)
#define LSE_COLOR210   MakeColorFromIndex(209)
#define LSE_COLOR211   MakeColorFromIndex(210)
#define LSE_COLOR212   MakeColorFromIndex(211)
#define LSE_COLOR213   MakeColorFromIndex(212)
#define LSE_COLOR214   MakeColorFromIndex(213)
#define LSE_COLOR215   MakeColorFromIndex(214)
#define LSE_COLOR216   MakeColorFromIndex(215)
#define LSE_COLOR217   MakeColorFromIndex(216)
#define LSE_COLOR218   MakeColorFromIndex(217)
#define LSE_COLOR219   MakeColorFromIndex(218)
#define LSE_COLOR220   MakeColorFromIndex(219)
#define LSE_COLOR221   MakeColorFromIndex(220)
#define LSE_COLOR222   MakeColorFromIndex(221)
#define LSE_COLOR223   MakeColorFromIndex(222)
#define LSE_COLOR224   MakeColorFromIndex(223)
#define LSE_COLOR225   MakeColorFromIndex(224)
#define LSE_COLOR226   MakeColorFromIndex(225)
#define LSE_COLOR227   MakeColorFromIndex(226)
#define LSE_COLOR228   MakeColorFromIndex(227)
#define LSE_COLOR229   MakeColorFromIndex(228)
#define LSE_COLOR230   MakeColorFromIndex(229)
#define LSE_COLOR231   MakeColorFromIndex(230)
#define LSE_COLOR232   MakeColorFromIndex(231)
#define LSE_COLOR233   MakeColorFromIndex(232)
#define LSE_COLOR234   MakeColorFromIndex(233)
#define LSE_COLOR235   MakeColorFromIndex(234)
#define LSE_COLOR236   MakeColorFromIndex(235)
#define LSE_COLOR237   MakeColorFromIndex(236)
#define LSE_COLOR238   MakeColorFromIndex(237)
#define LSE_COLOR239   MakeColorFromIndex(238)
#define LSE_COLOR240   MakeColorFromIndex(239)
#define LSE_COLOR241   MakeColorFromIndex(240)
#define LSE_COLOR242   MakeColorFromIndex(241)
#define LSE_COLOR243   MakeColorFromIndex(242)
#define LSE_COLOR244   MakeColorFromIndex(243)
#define LSE_COLOR245   MakeColorFromIndex(244)
#define LSE_COLOR246   MakeColorFromIndex(245)
#define LSE_COLOR247   MakeColorFromIndex(246)
#define LSE_COLOR248   MakeColorFromIndex(247)
#define LSE_COLOR249   MakeColorFromIndex(248)
#define LSE_COLOR250   MakeColorFromIndex(249)
#define LSE_COLOR251   MakeColorFromIndex(250)
#define LSE_COLOR252   MakeColorFromIndex(251)
#define LSE_COLOR253   MakeColorFromIndex(252)
#define LSE_COLOR254   MakeColorFromIndex(253)
#define LSE_COLOR255   MakeColorFromIndex(254)
#define LSE_COLOR256   MakeColorFromIndex(255)

#endif
