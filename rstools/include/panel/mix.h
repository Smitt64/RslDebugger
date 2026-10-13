/*******************************************************************************
 FILE         :   MIX.H

 COPYRIGHT    :   R-Style Software Lab, 1992,1998

 DESCRIPTION  :   Multi-Panel

 PROGRAMMED BY:   Emil Dovidovitch

 CREATION DATE:   26 Sep 1992

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/PANEL/H/mix.h $
                  $Revision: 3 $
                  $Date: 13.10.98 23:35 $
                  $Author: Markovitch $
                  $Modtime: 13.10.98 23:29 $
*******************************************************************************/

#ifndef __MIX_H
#define __MIX_H

#include "pexpdef.h"

#include <packpshn.h>

typedef struct Mix
      {
       void  *bp;              // Объект
       int    (*constr)();     // Конструктор
       void   (*destr)();      // Деструктор
       int    (*play)();       // Проигрыватель
       coord  kt, ke;          // Навигация по TAB и ENTER
       coord  x1, y1, x2, y2;  // Глобальные координаты
       void  *Parm;            // Для передачи параметров

       // 18.02.1997 (K78)
       coord  kst;             // Навигация по Shift-Tab

       // Azartsov Valery 06.05.2010
       bool   IsBSCROL;        // Признак объекта с типом BSCROL
       void  *AddParm;         // Дополнительные пользовательские параметры (для AddScroll)
      } Mix;

// Платформа
typedef struct Platform
      {
       PANEL *mp;           // Панель
       int    (*constr)();  // Конструктор
       void   (*destr)();   // Деструктор
       Mix   *MixList;      // Список объектов
       int    NMixList;     // Количество объектов
       Mix   *CurMix;       // Текущий объект
       int    (*pr)();      // Обработчик
       void  *Parm;         // Для передачи параметров
       int    SwitchKey;    // Код клавиши переключения между панелью и накладными скроллингами
      } Platform;

#include <packpop.h>

#ifdef __cplusplus
extern "C" {
#endif

// Вывести платформу
_RSPNLEXP int PutPlatform(
                          Platform *pf           // Описатель платформы
                         );

// Убрать платформу
_RSPNLEXP void RemPlatform(
                           Platform *pf           // Описатель платформы
                          );

_RSPNLEXP int UsePlatform(Platform *pf, int mff);

// Отработать с платформой.
// Возвращает -key или 0 при нехватке памяти.
_RSPNLEXP int LoopPlatform(
                           Platform *pf,      // Описатель платформы.
                           int       mff
                          );

_RSPNLEXP int MouseInPanel(PANEL *p);

#ifdef __cplusplus
} ;
#endif

#endif // __MIX_H

/* <-- EOF --> */