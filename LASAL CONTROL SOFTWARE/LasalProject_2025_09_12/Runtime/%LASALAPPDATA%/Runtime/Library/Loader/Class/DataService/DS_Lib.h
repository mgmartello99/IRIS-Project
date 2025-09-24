// +----------------------------------------------------------------------------------------------+
// +-[   copyright ] Sigmatek GmbH & CoKG                                                         |
// +-[      author ] kolott                                                                       |
// +-[        date ] 28.02.2017                                                                   |
// +-[ description ]------------------------------------------------------------------------------+
// |                                                                                              |
// |                                                                                              |
// +----------------------------------------------------------------------------------------------+

#ifndef _TcpComLibH
  #define _TcpComLibH
  
  #include "DS_Deeds.h"

  #ifdef cCompile

    cExtern void   dslib_free(void* ptr);
    cExtern void  *dslib_malloc(uint32 size);
    cExtern void  *dslib_realloc(void* ptr, uint32 size);
            uint32 dslib_memory(void **pptr, uint32 size);
            void   dslib_strcpy(void *dst, const void *src);
            uint32 dslib_strlen(const ascii *txt);
            uint32 dslib_strlen16(const uint16 *txt);
            uint32 dslib_strsize(const void *str, uint32 chrsize);
            void  *dslib_buffer_create(void *statbuff, uint32 statbuff_size, uint32 expected_size);
            void  *dslib_buffer_destroy(void *statbuff, void *buffer);
            void  *dslib_memcpy(void* dst0, const void* src0, uint32 len0);
            void  *dslib_memmove(void* dst0, const void *src0, uint32 len0);
            void  *dslib_cilget(const char *name);
            void  *dslib_criticalsection_create(const char *name);
            int32  dslib_criticalsection_delete(void* hdl);
            int32  dslib_criticalsection_stop(void* hdl);
            int32  dslib_criticalsection_start(void* hdl);
            uint32 dslib_encryptU32(uint32 value); // verschlüsseln
            uint32 dslib_decryptU32(uint32 value); // entschlüsseln
            uint32 dslib_magicnumber(void);  // magic random number
            uint32 dslib_merge32(uint32 value, uint32 newval, uint32 offset, uint32 width);
            char  *dslib_namemerge(char *dst, const char *src, void *pth);
    cExtern long   dslib_strtol(const char *nptr, char **endptr, int base);

    // in st programmiert

    cExtern void   dslib_CTor(void);
    cExtern uint32 dslib_tabsolute(void);
    cExtern uint32 dslib_get_lasal_id(uint08 *pchmod, const ascii *label);
    cExtern uint32 dslib_call_new_inst(uint08 *pdata, uint32 datalength, void *presu);
    cExtern uint32 dslib_call_new_inst_direct(uint32 lasalid, void *ppara, void *presu);
    cExtern uint32 dslib_call_read_method(uint32 lasalid);
    cExtern uint32 dslib_call_write_method(uint32 lasalid, uint32 value);
    cExtern uint32 dslib_read_client_data(uint32 lasalid);
    cExtern uint32 dslib_write_client_data(uint32 lasalid, uint32 value);
    cExtern void*  dslib_read_string(void *pdst, uint32 dstsize, uint32 *pchrsize, uint32 lasalid);
    cExtern uint32 dslib_write_string(uint32 lasalid, const void *ptxt, uint32 chrsize);
    cExtern uint32 dslib_get_string_crc(uint32 lasalid);
    cExtern uint32 dslib_read_dataat(void *dst, uint32 lasalid, uint32 datasize);
    cExtern uint32 dslib_write_dataat(uint32 lasalid, void *src, uint32 datasize);
    cExtern uint32 dslib_write_dataat_part(uint32 *pdata, uint32 lasalid, uint32 bitmask, uint32 datasize);
    cExtern uint32 dslib_get_dataat_crc(uint32 lasalid, uint32 datasize);
    cExtern uint32 dslib_get_dsccrc(void);
    cExtern uint32 dslib_get_loaderversion(void);
    cExtern uint32 dslib_get_opsysversion(void);    
    cExtern uint32 dslib_get_serverlabel(char *dst, uint32 lasalid);
    cExtern uint32 dslib_get_systime(void);
    cExtern uint32 dslib_get_sysdate(void);
    cExtern uint32 dslib_set_systime(uint32 ltime);
    cExtern uint32 dslib_set_sysdate(uint32 ldate);
    cExtern uint32 dslib_thread_delay_us(uint32 duration);
    cExtern uint32 dslib_get_cpustate(void);
    cExtern uint32 dslib_get_cpuload(void);
    cExtern uint32 dslib_is_x86(void);
    cExtern uint32 dslib_is_lars(void);
    cExtern uint32 dslib_is_gecko(void);
    cExtern uint32 dslib_is_salamander(void);
    cExtern uint32 dslib_is_linux(void);
    cExtern uint32 dslib_get_sysinfo(void);
    cExtern int32  dslib_tcp_create_socket(void);
    cExtern int32  dslib_tcp_set_socket_option(int32 socket, int32 level, int32 option_name, void *option_value, int32 optionlen);
    cExtern int32  dslib_tcp_recv(int32 sock_clt, void *pdst, uint32 dstsize);
    cExtern int32  dslib_tcp_send(int32 sock_clt, void *pdata, uint32 datasize);
    cExtern  void  dslib_close_socket(int32 sock);
    cExtern int32  dslib_tcp_user_listen(int32 socket, uint32 localport, uint32 backlogsize);
    cExtern int32  dslib_tcp_user_accept(int32 socket, uint32 timeout_ms);
    cExtern int32  dslib_tcp_user_connect(int32 socket, uint32 localport, char *ipAddress, uint32 port, uint32 timeout_ms);
    cExtern int32  dslib_getenvvar(const char *name, char *value, uint32 sizeof_value);
    
    cExtern uint32 dslib_atomic_incU32(uint32 *pvalue);
    cExtern uint32 dslib_atomic_decU32(uint32 *pvalue);
    cExtern void   dslib_system_message(const ascii *txt);

  #else
  
    function global __cdecl dslib_malloc var_input size:udint; end_var var_output retcode:^void; end_var;
    function global __cdecl dslib_free var_input ptr:^void; end_var;
    function global __cdecl dslib_realloc var_input ptr:^void; size:udint; end_var var_output retcode:^void; end_var;
    
    function global __cdecl dslib_CTor;
    function global __cdecl dslib_tabsolute var_output retcode:udint; end_var;
    function global __cdecl dslib_get_lasal_id var_input pchmod:^usint; label:^char; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_call_new_inst var_input pdata:^usint; datalength:udint; pResult:^results; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_call_new_inst_direct var_input lasalid:udint; ppara:^void; pResult:^results; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_call_read_method var_input lasalid:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_call_write_method var_input lasalid:udint; value:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_read_client_data var_input lasalid:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_write_client_data var_input lasalid:udint; value:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_read_string var_input pdst:^void; dstsize:udint; pchrsize:^udint; lasalid:udint; end_var var_output retcode:^void; end_var;
    function global __cdecl dslib_write_string var_input lasalid:udint; ptxt:^void; chrsize:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_get_string_crc var_input lasalid:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_read_dataat var_input dst:^void; lasalid:udint; datasize:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_write_dataat var_input lasalid:udint; src:^void; datasize:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_write_dataat_part var_input pdata:^udint; lasalid:udint; value:udint; datasize:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_get_dataat_crc var_input lasalid:udint; datasize:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_get_dsccrc var_output retcode:udint; end_var;
    function global __cdecl dslib_get_loaderversion var_output retcode:udint; end_var;
    function global __cdecl dslib_get_opsysversion var_output retcode : udint; end_var;
    function global __cdecl dslib_get_serverlabel var_input dst:^char; lasalid:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_get_systime var_output retcode:udint; end_var;
    function global __cdecl dslib_get_sysdate var_output retcode:udint; end_var;
    function global __cdecl dslib_set_systime var_input ltime:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_set_sysdate var_input ldate:udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_thread_delay_us var_input duration:udint; end_var;
    function global __cdecl dslib_get_cpustate var_output retcode:udint; end_var;
    function global __cdecl dslib_get_cpuload var_output retcode:udint; end_var;
    function global __cdecl dslib_is_x86 var_output retcode:udint; end_var;
    function global __cdecl dslib_is_lars var_output retcode:udint; end_var;
    function global __cdecl dslib_is_gecko var_output retcode:udint; end_var;
    function global __cdecl dslib_is_salamander var_output retcode:udint; end_var;
    function global __cdecl dslib_is_linux var_output retcode:udint; end_var;
    function global __cdecl dslib_get_sysinfo var_output retcode:udint; end_var;
    function global __cdecl dslib_tcp_create_socket var_output retcode:dint; end_var;
    function global __cdecl dslib_tcp_set_socket_option var_input socket:dint; level:dint; option_name:dint; option_value:^void; optionlen:dint; end_var var_output retcode:dint; end_var;
    function global __cdecl dslib_tcp_recv var_input sock_clt:dint; pdst:^void; dstsize:udint; end_var var_output retcode:dint; end_var;
    function global __cdecl dslib_tcp_send var_input sock_clt:dint; pdata:^void; datasize:udint; end_var var_output retcode:dint; end_var;
    function global __cdecl dslib_close_socket var_input sock:dint; end_var;
    function global __cdecl dslib_tcp_user_listen var_input socket:dint; localport:udint; backlogsize:udint; end_var var_output retcode:dint; end_var;
    function global __cdecl dslib_tcp_user_accept var_input socket:dint; timeout_ms:udint; end_var var_output retcode:dint; end_var;
    function global __cdecl dslib_tcp_user_connect var_input socket:dint; localport:udint; ipAddress:^char; port:udint; timeout_ms:udint; end_var var_output retcode:dint; end_var;
    function global __cdecl dslib_getenvvar VAR_INPUT name:^char; value:^char; sizeof_value:udint; END_VAR VAR_OUTPUT retcode:dint; END_VAR;
    function global __cdecl dslib_strtol var_input nptr:^char; endptr:^pvoid; base:dint; end_var var_output retcode:dint; end_var;
  
    function global __cdecl dslib_atomic_incU32 var_input pValue:^udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_atomic_decU32 var_input pValue:^udint; end_var var_output retcode:udint; end_var;
    function global __cdecl dslib_system_message var_input txt:^char; end_var;
  
  #endif


#endif
