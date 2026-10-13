/*******************************************************************************
 FILE         :   _EDIT.H

 COPYRIGHT    :   R-Style Software Lab, 1994, 1998

 DESCRIPTION  :   Private support for VIEW

 PROGRAMMED BY:   Serge Kubrin

 CREATION DATE:   2 Aug 1994

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/PANEL/H/_edit.h $
                  $Revision: 1 $
                  $Date: 19.10.98 19:30 $
                  $Author: Markovitch $
                  $Modtime: 16.10.98 22:00 $
*******************************************************************************/
/*
 December 29,1994   Serg Kubrin (K78)
            Поддержка двух окон в редакторе

*/

#ifndef ___EDIT_H
#define ___EDIT_H

#include "edit/viewed.h"

#ifdef __cplusplus
extern "C" {
#endif

_RSPNLEXP int    NewViewName  (View *v);
_RSPNLEXP void   PutDecor     (ViewData *d, int);
_RSPNLEXP int    IsMouseInView(View *v);
_RSPNLEXP void   SetCoord     (ViewData *d);

#ifdef __cplusplus
};
#endif

#define  NewName(v)  NewViewName(v)
#define  mousein(v)  IsMouseInView(v)

#endif // ___EDIT_H

/* <-- EOF --> */