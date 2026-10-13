/*-----------------------------------------------------------------------*-
 File Name   : gms.h     

 Sybsystem   : FS  
 Description : Процедуры для работы с "мышью"  

 Source      : 

 Library(s)  : 

 Copyright (c) 1991 - 1994 by R-Style JSC
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 March 10,1995   Serg Kubrin (K78)
            WIN32 support 
-*-----------------------------------------------------------------------*/

#ifndef __GMS_H
#define __GMS_H   

#include "platform.h"

#ifdef RSL_MD_FLAT   
    #define  GMSreset()  2
    #define  GMSenbl()
    #define  GMSdsbl()
    #define  GMSgoto(x, y)
#else  // RSL_MD_FLAT

    #include <dos.h>

    /*
     Для работы макросов необходимо определить следующие переменные:
    union   REGS  GMS33;
    struct  SREGS GMS33x;
    */

    #define  GINT    geninterrupt(0x33)
    #define  I33     int86(0x33, &GMS33, &GMS33)
    #define  I33x    int86x(0x33, &GMS33, &GMS33, &GMS33x)

    #define  GMSreset()    ((_AX=0, GINT, _AX) ? _BX : 0)
    #define  GMSenbl()     (_AX=1, GINT)
    #define  GMSdsbl()     (_AX=2, GINT)

    #define  GMSwhere(t, u, but)   (GMS33.x.ax = 3, I33, t = GMS33.x.cx, u = GMS33.x.dx, but = GMS33.x.bx)

    #define  GMSgoto(a, b)         (GMS33.x.ax = 4, GMS33.x.cx = (a), GMS33.x.dx = (b), I33)

    #define  GMSprs(b, n, dd, ss)  (GMS33.x.ax = 5, GMS33.x.bx = (b), I33, n = GMS33.x.ax, dd = GMS33.x.cx, ss = GMS33.x.dx)

    #define  GMSrls(b, n, dd, ss)  (GMS33.x.ax = 6, GMS33.x.bx = (b), I33, n = GMS33.x.ax, dd = GMS33.x.cx, ss = GMS33.x.dx)

    #define  GMSrngx(min, max)     (GMS33.x.ax = 7, GMS33.x.cx = (min), GMS33.x.dx = (max), I33)
    #define  GMSrngy(min, max)     (GMS33.x.ax = 8, GMS33.x.cx = (min), GMS33.x.dx = (max), I33)

    #define  GMSgrmd(xs, ys, pn)   (GMS33.x.ax = 9, GMS33.x.bx = (xs), GMS33.x.cx = (ys), GMS33.x.dx = (unsigned)(pn),  \
                                   GMS33x.es = (unsigned)((unsigned long)(pn) >> 16), I33x)

    #define  GMStxtmd(sl, sm, cm)  (GMS33.x.ax = 10, GMS33.x.bx = (sl), GMS33.x.cx = (sm), GMS33.x.dx = (cm), I33)

    #define  GMSmotion(nx, ny)     (GMS33.x.ax = 11, I33, nx = GMS33.x.cx, ny = GMS33.x.dx)

    /*  Function must be defined as   void huge fun(void);    */
    /*                                                        */
    #define  GMShand(msk, fun)     (GMS33.x.ax = 12, GMS33.x.cx = (msk), GMS33.x.dx = (unsigned)(fun),  \
                                   GMS33x.es = (unsigned)((unsigned long)(fun) >> 16), I33x)

    #define  GMSdefsens(hor, ver)  (GMS33.x.ax = 15, GMS33.x.cx = (hor), GMS33.x.dx = (ver), I33)

    #define  GMSsprng(a, b, c, d)  (GMS33.x.ax = 16, GMS33.x.cx = (a), GMS33.x.dx = (b), GMS33.x.si = (c),  \
                                   GMS33.x.di = (d), I33)

    #define  GMSdbspd(spd)         (_DX = (spd), _AX = 19, GINT)

    #define  GMShandsw(nmsk, nfun, omsk, ofun)                                                            \
                                   (GMS33.x.ax = 20, GMS33.x.cx = (nmsk), GMS33.x.dx = (unsigned)(nfun),  \
                                    GMS33x.es = (unsigned)((unsigned long)(nfun) >> 16), I33x,            \
                                    ofun = MK_FP(GMS33x.es, GMS33.x.dx), omsk = GMS33.x.cx)

    #define  GMSsavesz()           (_AX = 21, GINT, _BX)

    #define  GMSsave(buf)          (GMS33.x.ax = 22, GMS33.x.dx = (unsigned)(buf),  \
                                   GMS33x.es = (unsigned)((unsigned long)(buf) >> 16), I33x)

    #define  GMSrestore(buf)       (GMS33.x.ax = 23, GMS33.x.dx = (unsigned)(buf),  \
                                   GMS33x.es = (unsigned)((unsigned long)(buf) >> 16), I33x)

    #define  GMSsetcrt(page)       (_BX = (page),_AX = 29, GINT)
    #define  GMSgetcrt()           (_AX = 30, GINT, _BX)
#endif // RSL_MD_FLAT

enum GMSBUTNS
   {
    LEFTBUTN   = 1,
    RIGHTBUTN  = 2,
    MIDDLEBUTN = 3
   };

#endif // __GMS_H

/* <-- EOF --> */