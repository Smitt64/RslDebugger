/*
         HELP система
         VECsoft 30.01.91
         Modified:
         April 15,1993  Support for resource file format

         VERSION INFO :   $Archive: /RS-Tools/Old_tools/panel/h/help.h $
                          $Revision: 7 $
                          $Date: 12.09.00 17:39 $
                          $Author: Lebedev $
                          $Modtime: 12.09.00 16:41 $
*/

#ifndef  _HELPH
#define  _HELPH
#define  __HELP_H

#include <res.h>

#include "pexpdef.h"

#define  REF_LIST   1
#define  DEF_LIST   2

#include <packpsh1.h>

/* Информация об одном активном поле      */
typedef struct
      {
       db_int16  beg;         /* Начало активного поля          */
       db_int16  end;         /* Конец активного поля        */
       db_int16  page;        /* Номер страницы помощи -1 если страница не активна      */
      } HPACT;

// Элемент оглавления Help файла
typedef struct tagHelpElem
      {
       Resource  rc;
       db_int16  npage;   /* Номер страницы (имя) в файле          */
      } HelpElem;

#include <packpop.h>

typedef struct
      {
       char *str;       /* Одна строка в окне подсказки      */
       int   numact;    /* Количество активных полей в строке (-1 для оглавления) */
       int   base;      /* База в массиве активных полей или номер страницы для оглавления       */
      } HELP;

/* Страница помощи          */
typedef struct
      {
       HELP  *ptrH;     /* Массив строк помощи        */
       int    numstr;   /* Число строк           */
       HPACT *ptrA;     /* Массив активных полей       */
       int    numact;   /* Число активных полей        */
       HPACT *cA;       /* Текущее активное поле       */
      } PAGE;

#ifdef __cplusplus
extern "C" {
#endif

_RSPNLEXP int _OpenHelp(ResFile *rf, const char *fname, int flags);

/* Освободить страницу помощи       */
_RSPNLEXP void PageFree(
                        PAGE *page     /* Заголовок страницы            */
                       );

/* Чтение страницы помощи        */
/* При успехе возвращает 0       */
_RSPNLEXP int PageLoad(
                       ResFile *rf,
                       int      npage,
                       PAGE    *page      /* Заголовок страницы            */
                      );


_RSPNLEXP bool RslAddHelpRange (int ind1, int ind2, const char *name);

ResFile *RslGetHelpFile  (int page, int *status);
ResFile *RslGetHelpFileEx(int page, int *status, const char **fname, int *realPage /* обычно NULL */);

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */