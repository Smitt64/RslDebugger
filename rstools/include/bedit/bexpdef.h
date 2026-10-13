/*******************************************************************************
 FILE         :   BEXPDEF.H

 COPYRIGHT    :   R-Style, 1996

 DESCRIPTION  :   Macro definitions for RSBTRV library exports/imports.

 PROGRAMMED BY:   Yakov E. Markovitch (JNS)

 CREATION DATE:   27/11/1996

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/BEDIT/H/bexpdef.h $
                  $Revision: 5 $
                  $Date: 8.09.98 18:12 $
                  $Author: Markovitch $
                  $Modtime: 8.09.98 0:22 $
*******************************************************************************/

#ifndef __BEXPDEF_H
#define __BEXPDEF_H

#include "platform.h"

#if defined(__RSBTRV_BUILD) && !defined(RSL_MD_DLL)
    #error You must build .DLL if __RSBTRV_BUILD were defined.
#endif

#if defined(__RSBTRV_BUILD)
    #define _BTEXP  __EXPORT2
    #define _BTFUNC __EXPORT2
    #define _BTDATA __EXPORT2
#else
    #if defined(__RSRTL_DLL) && !defined(__RSBTRV_DLL)
        #define __RSBTRV_DLL
    #endif

    #ifdef __RSBTRV_DLL
        #define _BTEXP  __IMPORT2
        #define _BTFUNC __IMPORT2
        #define _BTDATA __IMPORT2
    #else   // Use Static lib or use WIN16 DLL
        #define _BTEXP
        #define _BTFUNC
        #define _BTDATA
    #endif
#endif

#endif // __BEXPDEF_H

/* <-- EOF --> */