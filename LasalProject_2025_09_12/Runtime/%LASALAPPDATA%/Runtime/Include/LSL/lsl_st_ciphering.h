#ifndef  __CIPHERING_H
#pragma once
#define  __CIPHERING_H

#define LSL_CIPHERING_ERR_BUFFER_UNDERFLOW     -1     // input buffer is too small
#define LSL_CIPHERING_ERR_WRONG_FILETYPE       -2     // wrong file type
#define LSL_CIPHERING_ERR_INVALID_CONTENT      -3     // invalid file content
#define LSL_CIPHERING_ERR_WRONG_VERSION        -4     // wrong file version
#define LSL_CIPHERING_ERR_DECRYPT_ERROR1       -5     // error during decrypt of header with user password
#define LSL_CIPHERING_ERR_DECRYPT_ERROR2       -6     // error during decrypt of data
#define LSL_CIPHERING_ERR_OUT_OF_MEM           -7     // out of memory
#define LSL_CIPHERING_ERR_INVALID_PASSWORD1    -8     // invalid sigmatek password
#define LSL_CIPHERING_ERR_INVALID_PASSWORD2    -9     // invalid user password
#define LSL_CIPHERING_ERR_BUF_TOO_SMALL        -10    // output buffer is too small

TYPE
  #pragma pack (push, 1)
  LSL_CIPHERING_TYPE : STRUCT
    size : UDINT;
    version : UDINT;
    Lsl_Decrypt	: pVoid;
  END_STRUCT;
  #pragma pack (pop)
END_TYPE

(*
  Decrypt a encrypted sigmatek file.

  \param in Input data (file content)
  \param inSize Size of input data
  \param outBuf Output buffer, where the decrypted data is written to
  \param outBufSize Size of output data
  \param key Passwort
  \param useSigmatekPwd
      0 = Password is a user password
      1 = Password is the sigmatek password which was used die enrypt the user password 
          in the header of the file.
  \return
      >=0 length of the encrypted data
      <0 error code
*)
FUNCTION __CDECL GLOBAL P_CIPHERING_Lsl_Decrypt
  VAR_INPUT
    in : ^CHAR;
    inSize : UDINT;
    outBuf : ^CHAR;
    outBufSize : UDINT;
    key : ^CHAR;
    useSigmatekPwd : UDINT;
  END_VAR
  VAR_OUTPUT
    retcode : DINT;
  END_VAR;

#define OS_CIPHERING_LSL_DECRYPT(pCIPHERING,p1,p2,p3,p4,p5,p6)		pCIPHERING^.Lsl_Decrypt $ P_CIPHERING_Lsl_Decrypt(p1,p2,p3,p4,p5,p6)

#endif
