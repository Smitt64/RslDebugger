/*-----------------------------------------------------------------------*-

 File Name   : rslmenu.h 

 Component   : Panel  
 Description : Интерфейс для работы с заргужаемыми меню

     LoadResMenu   -  Load and view menu
     LoadResMenuEx -  Load and/or view menu
     TestResMenu   -  return menu item ID or 0
     DoneResMenu   -  remove menu from screen and free memory

 Source      : resmenu.c

 Library(s)  : panell.lib, panel32.lib, panel16.lib


 Copyright (c) 1991 - 1997 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 June 17,1997  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __RSLMENU_H
#define __RSLMENU_H

#include "pexpdef.h"

#ifdef __cplusplus
extern "C" {
#endif

RSL_MAKE_HANDLE(HRSLMENU);

//--------------------  0 level interface ------------------------------
_RSRESEXP HRSLMENU LoadResMenu  (ResFile *rt, const char *member);
_RSRESEXP HRSLMENU LoadResMenuEx(ResFile *rt, const char *member, int onlyLoad);
_RSRESEXP int      TestResMenu  (HRSLMENU mn);
_RSRESEXP void     DoneResMenu  (HRSLMENU mn);

//--------------------  0 level interface ------------------------------
            
_RSRESEXP int WriteResMenu(ResFile *rf, const char *file, const char *member);


#ifdef __cplusplus
}
#endif

#endif // __RSLMENU_H

/* <-- EOF --> */