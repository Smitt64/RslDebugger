/*-----------------------------------------------------------------------*-
 File Name   : icatch.h  

 Sybsystem   : Tools  
 Description : TRY/CATCH support

 Source      : icatch.c

 Library(s)  : 

 Copyright (c) 1991 - 1994 by R-Style JSC
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 March 1,1994  Sergei Kubrin (K78) - Create file
 January 20,1995   Serg Kubrin (K78)
            Увеличил вложенность TRY/CATCH до 10
            Ввел JmpErrors
-*-----------------------------------------------------------------------*/

#ifndef __ICATCH_H
#define __ICATCH_H


/*
    Как использовать TRY/CATCH
   Пример:

#define ERCODE 10
void fun3 (void)
{
   if (!myfun ())
      THROW (ERCODE);
}

void fun1 (void)
{
   TRY
      fun1 ();  // Любое количество инструкций ...
      fun2 ();
      fun3 ();
      fun4 ();
   CATCH        // Все THROW передают управление сюда
      {         // jmperror - код ошибки (параметр THROW)
      printf ("Catch in fun1 %d\n",jmperror);
      freefun ();
      }
}

TRY/CATCH может иметь вложенность до MAXERR (5).
THROW передает управление на ближайший CATCH. Если THROW вызывается
не из TRY , то вызывается exit(5).
*/

#include <stdlib.h>
#include <setjmp.h>

#include "pexpdef.h"

#ifdef __cplusplus
extern "C" {
#endif


// Коды для THROW

#define  JMPONERROR  setjmp (errjmp[++usejmp <= MAXJMP ? usejmp-1 : MAXJMP - 1])
#define  CLEARJMP    usejmp--
#define  AGAINJMP    usejmp++

#define  ERRORJMP(er)  if(usejmp)                                               \
                        { usejmp--; longjmp(errjmp[usejmp], jmperror = (er)); } \
                       else exit(5);

#define  TRY        if((jmperror = JMPONERROR) == 0) {
#define  CATCH      CLEARJMP; } else
#define  ENDCATCH   }

#define  THROW(er)  { ERRORJMP(er); } 

#define  MAXJMP   10  // Максимальная вложенность longjmp

extern int     _RSPNLEXP  usejmp;   // Равно количеству вложений TRY
extern int     _RSPNLEXP  jmperror; // Код последней ошибки переданный THROW
extern jmp_buf _RSPNLEXP  errjmp[MAXJMP];  // Буфер контекстов стека

#ifdef __cplusplus
}
#endif

#endif // icatch_h

/* <-- EOF --> */