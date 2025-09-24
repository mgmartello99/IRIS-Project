#pragma once
#define __LSL_C_IFFTP


typedef struct
{
    long (* pftp_Connect)(unsigned long host, char *username, char *password);
    long (* pftp_XConnect)(unsigned long host, unsigned short port, char *username, char *password, 
    void (*response_func)(void* user_arg, char* wasned, int wasnix), void* user_arg);
    long (* pftp_CWD)(long ftpsocket, char* dir);
    long (* pftp_PWD)(long ftpsocket, char *buf);
    long (* pftp_MKDIR)(long ftpsocket, char *dirname);
    long (* pftp_RMDIR)(long ftpsocket, char *dirname);
    long (* pftp_DELE)(long ftpsocket,  char *filename);
    long (* pftp_BYE)(long ftpsocket);
    long (* pftp_LIST)(long ftpsocket, char* path, int (*user_function)(char*, int));
    long (* pftp_NLIST)(long ftpsocket, char * path, int (*user_function)( char *, int));
    long (* pftp_PUT)(long ftpsocket, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
    long (* pftp_APPEND)(long ftpsocket, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
    long (* pftp_GET)(long ftpsocket, char *filename, int (*user_function)(void *, char*, int), void *user_pointer);
    long (* pftp_TRANSFTYPE)(long ftpsocket, char type);

    void (* pftp_CloseAllSessions)(void);

    void* (* pGetNextFTPSocket)(void *pre0, void *sock0, unsigned short *ftpport0);

    // ab LasalOS version 1.1.15 gibt es eine Versionsnummer und eine Länge der FTP Struktur
    unsigned long size;
#define LSL_FTP_API_VERSION 2
    unsigned long version;

    // ab version 1
    long (* pftp_REN)(long ftpsocket, char *from0, char *to0);

    // ab version 2
    long (* pftp_ClientCfg)(unsigned long ulCfgOption, void * pValue, void * ppValue);

} LSL_FTP_API;

