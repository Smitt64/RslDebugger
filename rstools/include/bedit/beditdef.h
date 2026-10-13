/*******************************************************************************
 FILE         :   BEDITDEF.H

 COPYRIGHT    :   R-Style, 1996

 DESCRIPTION  :   Macro definitions for BEDIT library exports/imports

 PROGRAMMED BY:   Yakov E. Markovitch (JNS)

 CREATION DATE:   27/11/1996

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/BEDIT/H/beditdef.h $
                  $Revision: 1 $
                  $Date: 20.10.98 12:12 $
                  $Author: Markovitch $
                  $Modtime: 17.10.98 0:03 $
*******************************************************************************/

#ifndef __BEDITDEF_H
#define __BEDITDEF_H

#ifndef __PLATFORM_H
    #include "platform.h"
#endif // PLATFORM.H


#if defined(__BEDIT_BUILD)
    #if defined(RSL_MD_DLL)
        #define _BEDITEXP  __EXPORT2
        #define _BEDITFUNC __EXPORT2
        #define _BEDITDATA __EXPORT2
    #else
        #define _BEDITEXP  __IMPORT2
        #define _BEDITFUNC __IMPORT2
        #define _BEDITDATA __IMPORT2
    #endif
#else
    #if defined(__RSRTL_DLL) && !defined(__BEDIT_DLL)
        #define __BEDIT_DLL
    #endif

    #ifdef __BEDIT_DLL
        #define _BEDITEXP  __IMPORT2
        #define _BEDITFUNC __IMPORT2
        #define _BEDITDATA __IMPORT2
    #else   // Use Static lib or use WIN16 DLL
        #define _BEDITEXP
        #define _BEDITFUNC
        #define _BEDITDATA
    #endif
#endif

#endif // __BEDITDEF_H

/* <-- EOF --> */