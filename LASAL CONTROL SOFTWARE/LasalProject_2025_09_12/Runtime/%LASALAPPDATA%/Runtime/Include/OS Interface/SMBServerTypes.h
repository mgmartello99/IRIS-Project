#ifndef __SMB_SERVER_TYPES_H_
#define __SMB_SERVER_TYPES_H_

// error values
#define SMBSVR_ERR_POINTER           -1003  // error invalid pointer, NIL
#define SMBSVR_ERR_NOT_FOUND         -1006  // entry not found (user, access, ...)
#define SMBSVR_ERR_EXISTS            -1007  // entry already exists (user, access, ...)

// reserved return value - this error value will be returned from every method of the SAMBA-Server
// class, if no SAMBA-Server interface is available.
// Starting the application without the SAMBA-Server interface is possible
#define SMBSVR_ERR_NO_INTERFACE      -1015  // error, no SAMBA-Server interface available

// general access
#define SMB_ACCESS_NONE               0x0000
#define SMB_ACCESS_READ               0x0010
#define SMB_ACCESS_WRITE              0x0020

// max. len for username/password
#define SMBSVR_USERNAME_LEN           15  //excl '\0'
#define SMBSVR_PASSWORD_LEN           15  //excl '\0'


#endif // __SMB_SERVER_TYPES_H_
