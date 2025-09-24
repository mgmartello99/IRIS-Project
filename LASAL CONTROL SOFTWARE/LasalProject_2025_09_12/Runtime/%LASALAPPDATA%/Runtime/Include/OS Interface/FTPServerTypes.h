#ifndef __FTP_SERVER_TYPES_H_
#define __FTP_SERVER_TYPES_H_

// error values
#define FTPSVR_ERR_VF                -1000  // error initializing internal virtual file system
#define FTPSVR_ERR_SEMA              -1001  // error creating semaphore
#define FTPSVR_ERR_DEAMON            -1002  // error creating FTP Server Main Thread
#define FTPSVR_ERR_POINTER           -1003  // error invalid pointer, NIL
#define FTPSVR_ERR_PARAM             -1004  // error invalid parameter (username too long, ...)
#define FTPSVR_ERR_NOMEM             -1005  // not enough memory
#define FTPSVR_ERR_NOT_FOUND         -1006  // entry not found (user, access, ...)
#define FTPSVR_ERR_EXISTS            -1007  // entry already exists (user, access, ...)
#define FTPSVR_ERR_INI_OPEN          -1008  // error creating/opening ftps.ini file
#define FTPSVR_ERR_INI_WRITE         -1009  // error writing to ftps.ini file
#define FTPSVR_ERR_INI_READ          -1010  // error reading from ftps.ini file
#define FTPSVR_ERR_UPDATETASK        -1011  // error update task could not be startet
#define FTPSVR_ERR_UPD_BUSY          -1012  // update task busy
#define FTPSVR_ERR_UPD_NO_TASK       -1013  // update task not started
#define FTPSVR_ERR_LOGGED_IN         -1014  // user logged in

// reserved return value - this error value will be returned from every method of the FTP-Server
// class, if no FTP-Server interface is available.
// Starting the application without the FTP-Server DLL is possilble
#define FTPSVR_ERR_NO_INTERFACE      -1015  // error, no FTP-Server interface available

// flags for edit user
#define FTPSVR_CHANGE_PASSWORD        0x00000001
#define FTPSVR_CHANGE_GENERALACCESS   0x00000002
#define FTPSVR_CHANGE_ROOTDIR         0x00000004

// access
#define FTP_ACCESS_NONE               0x0000
#define FTP_ACCESS_READ               0x0001
#define FTP_ACCESS_WRITE              0x0002

// max. len for username/password
#define FTPSVR_USERNAME_LEN           63
#define FTPSVR_PASSWORD_LEN           63

// infinite value for option FTPS_OPT_..._TMO
#define FTPSVR_INF                    0xffff

// UserInfo, GetUserExtendedInfo
#define FTPS_USR_INFO_REFERENCE       1
#define FTPS_USR_INFO_STATUS          2
#define FTPS_USR_INFO_LOGGEDIN        3

// Reference
#define FTPS_USR_REF_OS               1
#define FTPS_USR_REF_APPL             3

// Status
#define FTPS_USR_STAT_ADDED           0x00000001
#define FTPS_USR_STAT_CHANGED         0x00000002
#define FTPS_USR_STAT_STORED          0x00000004

// server config. options
#define FTPS_OPT_LISTEN_BACKLOG       1
#define FTPS_OPT_READ_TMO             2
#define FTPS_OPT_WRITE_TMO            3
#define FTPS_OPT_CMD_TMO              4
#define FTPS_OPT_NO_DRIVE             5
#define FTPS_OPT_BACKSLASH            6
#define FTPS_OPT_FORWARDSLASH         7
#define FTPS_OPT_MAX_SESSIONS         8
#define FTPS_OPT_DEF_ROOT_DIR         9
#define FTPS_OPT_PASV_PORT            10
#define FTPS_OPT_ANOMYMOUS            11
#define FTPS_OPT_ANONYMACCESS         12
#define FTPS_OPT_NO_FTPSINI           13
#define FTPS_OPT_SVR_D_PRIO           14
#define FTPS_OPT_SVR_T_PRIO           15

#endif // __FTP_SERVER_TYPES_H_
