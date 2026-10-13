/*-----------------------------------------------------------------------*-

 File Name   : trnlog.h  

 Sybsystem   :   
 Description : 

 Source      : 

 Library(s)  : 

 VERSION INFO: $Archive: /RS-Tools/Old_tools/tools/h/trnlog.h $
               $Revision: 3 $
               $Date: 16.05.00 18:01 $
               $Author: Kubrin $
               $Modtime: 15.05.00 15:48 $

 Copyright (c) 1991 - 1998 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 May 5,2000  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __TRNLOG_H
#define __TRNLOG_H

#include "tooldef.h"

#define BTRIEVE_TRACE "BtrieveSource"
#define MDB_TRACE     "MdbSource"

#include <packpshn.h>

// -----------------------------------------------------------------------------
typedef struct tagTrnLogData
      {
       int          oper;
       const char  *fileName;
       int          ercode;
       
       int          repCounter;
       
       int          inTrn;

       char        *m_lpszLogDataFile;
       int          m_iLineNumber;
       int          m_iUserAbort;

       int          Mode;  // Режим открытия файла

       tagTrnLogData() : oper(0), ercode(0), fileName(""), repCounter(0), inTrn(0), m_lpszLogDataFile(""), m_iLineNumber(0), m_iUserAbort(0), Mode(0) {}
       
      } TTrnLogData;

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
_TOOLEXP void InitTrnLog    (const char *path);
_TOOLEXP void InitTrnLogEx  (const char *path, const char *mdbPath);
_TOOLEXP void CloseTrnLog   (void);
_TOOLEXP void WriteTrnLog   (TTrnLogData *data);
_TOOLEXP void WriteTrnLogEx (TTrnLogData *data, bool bMdbFile);
_TOOLEXP void BeginTrnLog   (void);
_TOOLEXP void EndTrnLog     (int isAbort);

#ifdef __cplusplus
}
#endif

#include <packpop.h>

#endif // __TRNLOG_H

/* <-- EOF --> */