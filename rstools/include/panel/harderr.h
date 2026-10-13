/*-----------------------------------------------------------------------*-

 File Name   : harderr.h 

 Sybsystem   : TOOLS  
 Description : Обработка CtrlBreak и Int 24

 Source      : harderr.c

 Library(s)  : 


 Copyright (c) 1991 - 1994 by R-Style JSC
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 September 20,1994  Sergei Kubrin (K78) - Create file
 March 16,1995   Serg Kubrin (K78)
      WIN32  Support            
-*-----------------------------------------------------------------------*/

#ifndef __HARDERR_H
#define __HARDERR_H

#include "pexpdef.h"

#ifdef __cplusplus
extern "C" {
#endif

extern _RSPNLEXP int  CtrlBreakFlag;
extern _RSPNLEXP int  CritFlag;
extern _RSPNLEXP int  CritDrive;
extern _RSPNLEXP char DevName[];

_RSPNLEXP void SysIntResume  (void);
_RSPNLEXP void SysIntSuspend (void);
_RSPNLEXP void SysIntOnOff   (int on) ;
_RSPNLEXP void SysIntInstall (void);
_RSPNLEXP void SysIntRemove  (void);
_RSPNLEXP int  SysIntError   (char *s);

#ifdef __cplusplus
}
#endif

#endif // __HARDERR_H

/* <-- EOF --> */