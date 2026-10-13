/*******************************************************************************
 FILE         :   PEXPDEF.H

 COPYRIGHT    :   R-Style, 1996

 DESCRIPTION  :   Macro definitions for PANEL library exports/imports

 PROGRAMMED BY:   Yakov E. Markovitch (JNS)

 CREATION DATE:   27/11/1996

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/PANEL/H/pexpdef.h $
                  $Revision: 5 $
                  $Date: 8.09.98 18:12 $
                  $Author: Markovitch $
                  $Modtime: 7.09.98 19:09 $

*******************************************************************************/

#ifndef __PEXPDEF_H
#define __PEXPDEF_H

#include "platform.h"

#if defined(__RSRTL_DLL) && !defined(__PANEL_DLL) && !defined(__PANEL_BUILD)
    #define  __PANEL_DLL
#endif

#if defined(__PANEL_BUILD) && !defined(RSL_MD_DLL)
    #error You must build .DLL if __PANEL_BUILD were defined.
#endif

#if defined(__PANEL_BUILD)
    #if defined(RSL_MD_DLL)
        #define  _RSPNLEXP  __EXPORT2
        #define  _RSRESEXP  __EXPORT2
    #else
        #define  _RSPNLEXP  __IMPORT2
        #define  _RSRESEXP  __IMPORT2
    #endif
#elif defined(__PANEL_DLL) && defined(RSL_PL_WIN32)
    #define  _RSPNLEXP  __IMPORT2
    #define  _RSRESEXP  __IMPORT2
#else   // Use Static lib or use WIN16 DLL
    #define  _RSPNLEXP
    #define  _RSRESEXP
#endif

#endif // __PEXPDEF_H

/* <-- EOF --> */