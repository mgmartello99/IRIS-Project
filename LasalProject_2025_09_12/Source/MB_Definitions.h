
//#########################################################
//# G E N E R A L   D E F I N E S                         #
//#########################################################
#define TURN_BYTES


//#########################################################
//# M O D B U S   F U N C T I O N S                       #
//#########################################################
#define MB_FUNC_RD_MULTIPLE_REG             0x03
#define MB_FUNC_RD_MULTIPLE_INPUT_REG       0x04
#define MB_FUNC_WR_MULTIPLE_REG             0x10
#define MB_FUNC_DIAG                        0x08
#define MB_FUNC_RD_WR_MULTIPLE_REG          0x17


//#########################################################
//# M O D B U S   S U B   F U N C T I O N S               #
//#########################################################
#define SUB_DIAGNOSTIC  0x00


//#########################################################
//# M O D B U S   E R R O R   C O D E S                   #
//#########################################################
#define INVALID_FUNC    0x01
#define ERROR_ADDRESS   0x02
#define ERROR_ACCESS    0x02
#define ERROR_DATA      0x03
#define NO_ERROR        0x00


//#########################################################
//# R E T C O DE S                                        #
//#########################################################
#define PARA_NOT_FOUND   -1000
#define MB_ERR_ADD_PARA    -1
#define MB_PARA_OK          1


//#########################################################
//# P A R A M E T E R   A C C E S S   R I G H T S         #
//#########################################################
#define MB_READ_ONLY   0x00
#define MB_WRITE_ONLY  0x01
#define MB_READ_WRITE  0x02


//#########################################################
//# P A R A M E T E R   T Y P E S                         #
//#########################################################
#define MB_TYPE_SERVER   1
#define MB_TYPE_VARIABLE 2


//#########################################################
//# P A R A M E T E R   D A T A   T Y P E S               #
//#########################################################
#define MB_DAT_VAL      1
#define MB_DAT_STRING   2

//#########################################################
//# B A U D                                               #
//#########################################################
#define BAUD_300      300
#define BAUD_600      600
#define BAUD_1200     1200
#define BAUD_2400     2400
#define BAUD_4800     4800
#define BAUD_9600     9600
#define BAUD_14400    14400
#define BAUD_19200    19200
#define BAUD_38400    38400
#define BAUD_56000    56000
#define BAUD_115200   115200


//#########################################################
//# C H A R A C T E R T I M E S                           #
//#########################################################
#define ONE_AHALF_CHARACTER     16.5
#define THREE_AHALF_CHARACTER   38.5


//#########################################################
//# OFFSET                                                #
//#########################################################
#define MB_OFFSET_CHECK 20000
