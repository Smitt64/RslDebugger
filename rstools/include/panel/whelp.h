/*-----------------------------------------------------------------------*-

 File Name   : whelp.h   

 Sybsystem   :   
 Description : 

 Source      : 

 Library(s)  : 

 VERSION INFO: $Archive: /RS-Tools/Old_tools/tools/h/whelp.h $
               $Revision: 1 $
               $Date: 7.09.00 10:21 $
               $Author: Kubrin $
               $Modtime: 6.09.00 15:09 $

 Copyright (c) 1991 - 1998 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 September 6,2000  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __WHELP_H
#define __WHELP_H

#include "pexpdef.h"
 
#ifndef __TERMEXT_H
    #include "rstypes.h"
#endif

#include <packpshn.h>

#define  RSLENGHELP  "rsleng.hlp"

enum
   {
    CMD_RSL_HLP = 50,
    CMD_RSL_HLP2
   };

#define  RSL_HLP_KEY     0
#define  RSL_HLP_CTX     1
#define  RSL_HLP_EXT     2
#define  RSL_HLP_ID      3
#define  RSL_HLP_NAME    4
#define  RSL_HLP_NAMEID  5

typedef struct
      {
       db_int32   cmd;
       db_uint32  szFlName;
       db_uint32  szKeyLen;
       char       buff[2];
      } TCmdCallHelp;

typedef struct
      {
       db_int32   cmd;
       db_uint32  szFlName;
       db_uint32  szKeyLen;
       db_int32   id;
       char       buff[2];
      } TCmdCallHelp2;

typedef struct
      {
       db_int32  stat;
      } TCmdResultHelp;

#include <packpop.h>


#ifdef __cplusplus
extern "C" {
#endif

_RSPNLEXP extern bool whelpShown;

_RSPNLEXP bool WHelpShowPage      (const char *flName, int num);
_RSPNLEXP bool WHelpShowPageName  (const char *flName, const char *name);
_RSPNLEXP bool WHelpShowPageNameId(const char *flName, int id);
_RSPNLEXP bool WHelpShowKey       (const char *flName, const char *key);
_RSPNLEXP bool WHelpShowContents  (const char *flName);
_RSPNLEXP void WHelpQuit          (void);

#ifdef __cplusplus
}
#endif

#endif // __WHELP_H

/* <-- EOF --> */