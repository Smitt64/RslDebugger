/*******************************************************************************
 FILE         :   _OBJPNL.H

 COPYRIGHT    :   R-Style, 1996

 DESCRIPTION  :   Hidden variables for PANEL <-> OODSKTOP interaction

 PROGRAMMED BY:   Yakov E. Markovitch (JNS)

 CREATION DATE:   29/11/1996

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/PANEL/H/_objpnl.h $
                  $Revision: 4 $
                  $Date: 6.05.99 15:49 $
                  $Author: Markovitch $
                  $Modtime: 6.05.99 15:45 $

*******************************************************************************/

#ifndef ___OBJPNL_H
#define ___OBJPNL_H

#include <pexpdef.h>
#include <desktop.h>
#include <rsmain.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*_pnl_accel_tester)(const PANEL *, int);

extern _RSPNLEXP _pnl_accel_tester   panelTestAccel;
extern _RSPNLEXP int                 lastHelpErr;
extern _RSPNLEXP int                 errHelpPage;
extern _RSPNLEXP int                 objectPanelHelp;

_RSPNLEXP int  editfld(PANEL *p, int canEdit);
_RSPNLEXP int  wrapfld(PANEL *p);
_RSPNLEXP int  helpstr(PANEL *p);
_RSPNLEXP void exitor(void);

#ifdef __cplusplus
};
#endif

#endif // ___OBJPNL_H

/* <-- EOF --> */