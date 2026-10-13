/*******************************************************************************
 FILE         :   JCMENU.H

 COPYRIGHT    :   R-Style, 1996

 DESCRIPTION  :

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   //96
*******************************************************************************/

#ifndef __JCMENU_H
#define __JCMENU_H

#include "desktop.h"

#ifdef __cplusplus
extern "C"  {
#endif

_RSPNLEXP PANEL    *createMenuBar  (MENU *menu, int y);
_RSPNLEXP PANEL    *createMenuPopup(MENU *menu, int x, int y);
_RSPNLEXP unsigned  getMenuItems   (MENU *m);

#ifdef __cplusplus
}
#endif

#endif // __JCMENU_H

/* <-- EOF --> */