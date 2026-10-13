/*-----------------------------------------------------------------------*-

 File Name   : btserver.h

 Sybsystem   :
 Description :

 Source      :

 Library(s)  :


 Copyright (c) 1991 - 1994 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 August 14,1996  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __BTSERVER_H
#define __BTSERVER_H

#include "bexpdef.h"

#ifndef _BTREQDLL
    // -------------------------------------------
    enum TRslReqType
       {
        RSL_REQ_USESTD = 0,
        RSL_REQ_TCP,
        RSL_REQ_SPX,
        RSL_REQ_PIPE,

        RSL_REQ_SQLEMU
       };

    // -------------------------------------------
    enum TRslMapType
       {
        RSL_REQ_USEMAP = 0,
        RSL_REQ_SERVPATH,
        RSL_REQ_TRANCPATH
       };
#endif

// -----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

struct TClientConfigData
     {
      // Transactin control
      int            repCount;
      int            sleepTime;
      int            randomTime;
      int            ConcurrentProhib;

      // requester data
      int            port;
      int            reqType;        // One of RSL_REQ_...
      int            mapType;
      unsigned long  bufSize;
      char           host[25];

      int            lockType;

      int            isWait;
      int            opRepCount;
      int            opSleepTime;
      char           logPath[80];

      int            optLock;        // Use opimized locks in bfGet

      char           connString[256];

      int            joinOptMode;    // Join   Optimization mode (for BfAddLink)
      int            resetOnSwitch;  // Reset cache when switching
      int            cacheSize;      // Recordset default cache size

      int            useTrnMgr;      // Use Transaction Manager
                                     // USETRNMGR = (0-NONE,1-RSMGR,2-MSDTC)
      int            maxBlobLen;     // Max BLOB length (0 - use temporary file)
      int            sqlBreak;       // Switch on CancelSqlQuery
      int            getRecCount;    // if 0 then bfNRec returns -1
      int            useNulls;       // NULL values in emulator

      char           nlsLang[80];
      int            xmlLog;

      int            miniDump;       // AV 17.03.2011 generate minidumps for exceptions
      bool           set_miniDump;

      // RSL (за неимением rsscript.ini)
      int            testmode;       // AV 06.05.2011 workmode only for complete autotest, in RSL date() time() return constant values
      int            dbgRmt;
      int            dbgBrk;
      int            rslStackSize;
      int            rslTraceMode;   // #3203
     };

// -----------------------------------------------
enum TxmlLogType
   {
    XML_LOG_NO   = 0,  // xml-лог не ведется
    XML_LOG_DUP  = 1,  // xml-лог ведется дополнительно, дублируя основной лог
    XML_LOG_ONLY = 2   // ведется только xml-лог
   };

// If cnfFile == NULL, used rsreq.cnf in startup directory
_BTEXP int BtServerConnect(const char *cnfFile);
_BTEXP int BtServerDisconnect(void);

_BTEXP int BtServerExistFile(const char *file);
_BTEXP int BtTestExistTable(const char *tableName);

#ifdef SQLBUILD
    int SqlServerExistFile(const char *file);
    int SqlServerExistFmt(const char *file);
#endif

_BTEXP struct TClientConfigData *BtServerGetConfig(void);

_BTEXP void BtServerReadClientData(const char *cnfFile, struct TClientConfigData *data);

_BTEXP int  BtTestExistFile(const char *file);

// Проверка наличия dbt-файла через попытку его открытия. Если файл существует,
// функция возвращает 1, в противном случае - 0.
_BTEXP int  BtTestExistDbtFile(const char *file);

#define  TestExistFile(file)  BtTestExistFile(file)

// Transaction executor function
typedef int (*bedit_trnfn_t)(void *prm);

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */