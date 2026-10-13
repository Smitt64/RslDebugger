/*-----------------------------------------------------------------------*-

 File Name   : rsreq.h

 Sybsystem   :   
 Description : Interface to RSREQ.DLL

 Source      : 

 Library(s)  : 


 Copyright (c) 1991 - 1994 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 February 27,1996  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __RSREQ_H
#define __RSREQ_H

#include "platform.h"

#include "hbtr\clnt\reqtype.h"

// -----------------------------------------------
#if defined(_BUILDBTREQDLL) || defined(_BTREQDLL)
    #if defined(RSL_PL_WIN32)
        #define  REQAPI  __stdcall
    #elif defined(RSL_PL_OS2)
        #define  REQAPI  __syscall
    #else
        #define  REQAPI
    #endif
#else
    #define  REQAPI
#endif

// -----------------------------------------------
#if defined(_BUILDBTREQDLL)                   
        #define  BREXPIMP
        #define  MSEXPIMP  __declspec(dllexport)
#elif defined(_BTREQDLL) && defined(RSL_PL_WIN32)
        #define  BREXPIMP
        #define  MSEXPIMP  __declspec(dllimport)
#else   // Use Static lib or use WIN16 DLL
    #define  BREXPIMP 
    #define  MSEXPIMP 
#endif

// -----------------------------------------------
typedef short int  TCONID;

// -----------------------------------------------
#define  WIN32_USER_ERROR_FLAG    0xE0000000UL
#define  WIN32_USER_WARNING_FLAG  0xA0000000UL

// -----------------------------------------------
// This is requester errors, other then Win32
#define  RSL_REQ_ERR_NOTCONNECTED  WIN32_USER_ERROR_FLAG + 11
#define  RSL_REQ_ERR_CLNTLIMIT     WIN32_USER_ERROR_FLAG + 12  // Too many clients are connected
#define  RSL_REQ_ERR_EXISTFILE     WIN32_USER_ERROR_FLAG + 13  // Server can not set new directory
#define  RSL_REQ_ERR_LOGIN         WIN32_USER_ERROR_FLAG + 14  // Server can not login the User
#define  RSL_REQ_ERR_LOGOUT        WIN32_USER_ERROR_FLAG + 15  // Server can not logout the User
#define  RSL_REQ_ERR_BTRIEVE       WIN32_USER_ERROR_FLAG + 16  // Btrieve manager error

#ifdef __cplusplus
extern "C" {
#endif

MSEXPIMP BOOL REQAPI BREXPIMP ReqServDisconnect(TCONID hd);
MSEXPIMP BOOL REQAPI BREXPIMP ReqServConnect(char *host, int mapType, int port, DWORD ReqBuffSize, int reqType, TCONID *hd);

MSEXPIMP BOOL REQAPI BREXPIMP ReqServExistFile(TCONID hd, const char *file, BOOL *exist); 

MSEXPIMP BOOL REQAPI BREXPIMP ReqBtrieveOp(TCONID id, unsigned short op, void *pb, void *data, unsigned long *dataLen,
                                           void *key, unsigned char keyLen, signed char keyPath, short int *btrStat);

#ifdef __cplusplus
}
#endif

#endif // __BTCLIENT_H

/* <-- EOF --> */