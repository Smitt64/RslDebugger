//-*--------------------------------------------------------------------------*-
//
// File Name   : ledit.h
//
// Copyright (c) 1991 - 2004 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// December 29, 1992
//-*--------------------------------------------------------------------------*-

// Редактор строк

#ifndef __LEDIT_H
#define __LEDIT_H

#define LEDIT

#include "desktop.h"
#include "marklist.h"

/************************************************************************/
/*         Режим редактирования скролинга         */
#ifndef IsScrlMod
    #define  IsScrlMod(mn)   ((mn)->mode & 0x10) /* Опеределить режим ред.*/
    #define  SetScrlNo(mn)   (mn)->mode &= 0xef
    #define  SetScrlMod(mn)  (mn)->mode |= (unsigned char)0x10
#endif

/************************************************************************/
/*         Режим вставки записи             */
#ifndef IsInsMod
    #define  IsInsMod(mn)   ((mn)->mode & 0x40)
    #define  SetInsNo(mn)   (mn)->mode &= 0xbf
    #define  SetInsMod(mn)  (mn)->mode |= (unsigned char)0x40
#endif


// Аналог из bedit.h
#define  LIsAllowEmptyMod(L)    ((L)->mode2 & 0x02)               // Определить режим скроллинга без записей
#define  LSetAllowEmptyNo(L)    (L)->mode2 &= 0xfd                // Выкл. режим скроллинга без записей
#define  LSetAllowEmptyMod(L)   (L)->mode2 |= (unsigned char)0x02 // Вкл. режим скроллинга без записей


#define  Internal_1  0x20 // Флаг устанавливается в mode. Используется только в
                          // инструменте для определеня возврата в скроллинг из
                          // панели ввода. Устанавливается при
                          // выходе из панели ввода. Снимается
                          // после передачи в обработчик скроллинга ENTFIELD

#define  Internal_2  0x80 // Флаг устанавливается в mode. Используется только в
                          // инструменте для определеня возврата в скроллинг из
                          // панели редактирования

#ifdef __cplusplus
extern "C" {
#endif

// Установить сообщение скролинга
_RSPNLEXP extern void setLMES(
                              const char *mes    // сообщение
                             );

/* Коды возврата из процедуры инициал. строки   */
enum LRETINP
   {
    INPNO = 0,   /* Запретить ввод строки         */
    INPYES,      /* Разрешить ввод строки         */
    INPTOP,      /* Разрешить ввод строки в начало скролинга  */
    INPBOT       /* Разрешить ввод строки в конец скролинга   */
   };

struct LSCROL;

#ifdef __cplusplus
    typedef int (*lscrol_makerec_t)(LSCROL *);

    // Принимает указатель на скроллер и значение статуса
    typedef int (*lscrol_testrec_t)(LSCROL *);
#else
    typedef int (*lscrol_makerec_t)();

    // Принимает указатель на скроллер и значение статуса
    typedef int (*lscrol_testrec_t)();
#endif

typedef lscrol_makerec_t  lscrol_initrec_t;

// описатель редактора строк скролинга
typedef struct LSCROL
      {
       SCROL             m;          // описатель скролинга
       int               LLine;      // длина строки данных
       int               MaxL;       // максимальное количество строк
       PANEL*            Mpi;        // панель ввода строки
       PANEL*            Mpe;        // панель редактирования строки
       lscrol_initrec_t  n;          // процедура инициализации строки
       lscrol_testrec_t  i;          // процедура проверки при вводе строки
       lscrol_testrec_t  c;          // процедура проверки при изменении строки
       lscrol_testrec_t  d;          // процедура проверки при удалении строки
       unsigned char     mode;       // режимы редактирования скролинга
       void*             saveL;      // ссылка на буфер сохранения строки
       unsigned char     mode2;      // режимы скролинга (которые не поместились в mode)
       MARKLIST         *MList;      // список помеченных записей
      } LSCROL;

/************************ примитивные функции ***************************/

/* сохранить текущую строку в буфере сохранения */
_RSPNLEXP void lsaveb(
                      LSCROL *L    /* описатель редактора           */
                     );

/* восстановить текущую строку из буфера сохран.*/
_RSPNLEXP void lrestb(
                      LSCROL *L    /* описатель редактора           */
                     );

/* сравнить текущую строку с буфером сохранения */
_RSPNLEXP int lcmpb(
                    LSCROL *L    /* описатель редактора           */
                   );

/*********************** редактирование списка ***************************/
/* Стандартная процедура ввода строки     */
_RSPNLEXP int inLine(
                     PANEL  *p,     /* Описатель панели ввода        */
                     int     key    /* код клавиши               */
                    );

/* Стандартная процедура редактирования строки  */
_RSPNLEXP int edline(
                     PANEL  *p,     /* Описатель панели редактирования     */
                     int     key    /* код клавиши               */
                    );

/* Стандартная процедура скролинга строк  */
_RSPNLEXP int scrline(
                      PANEL *p,     /* Описатель панели скролинга       */
                      int    key    /* код клавиши               */
                     );

// Стандартная процедура скролинга строк для SCROL
// Урезанная версия
_RSPNLEXP int scrolline(
                        PANEL *p,     /* Описатель панели скролинга       */
                        int    key    /* код клавиши               */
                       );

/* Вывести область редактора строк     */
_RSPNLEXP int LPutScrol(
                        LSCROL  *L      /* описатель редактора строк        */
                       );

/* Удалить область редактора строк     */
_RSPNLEXP void LRemScrol(
                         LSCROL  *L      /* описатель редактора строк        */
                        );

/* Использовать скролинг для редактирования  */
_RSPNLEXP int LLoopScrol(
                         LSCROL *L    /* описатель редактора строк        */
                        );

/* Редактор строк             */
_RSPNLEXP int LEditScrol(
                         LSCROL *L    /* описатель редактора строк        */
                        );

// Редактор записей скролинга
_RSPNLEXP int LRunScrol(LSCROL *L);

/****************************** поиск ***********************************/
/* Найти строку перебором строк        */
/*    0 - найдено          */
/* != 0 - не найдено          */
_RSPNLEXP int find_line(
                        LSCROL *L,      /* Описатель скролинга           */
                        void   *value,  /* Значение поиска            */
                        int     (*ic)(void *buff, void *value) /* Программа проверки записи   */
                       );

/* Найти строку в скролинге         */
/*    0 - найдено          */
/* != 0 - не найдено          */
_RSPNLEXP int find_sline(
                         LSCROL *L,      /* Описатель скролинга           */
                         void   *value,  /* Значение поиска            */
                         int     (*ic)(void *buff, void *value) /* Программа проверки записи   */
                        );

/* Найти строку в скролинге            */
/*    0 - найдено                      */
/* != 0 - не найдено                   */
_RSPNLEXP int pick_sline(
                         LSCROL *L,                /* Описатель скролинга                 */
                         FIELD  *f,                /* Поле поиска                         */
                         const char *Invite,       /* Приглашение к вводу значения поиска */
                         const char *Oops          /* Сообщение при неудачном поиске      */
                        );

// Найти строку в SCROL
int pick_scrline
                (
                 SCROL  *M,             /* Описатель скролинга                 */
                 FIELD  *f,             /* Поле поиска                         */
                 const char *Invite,    /* Приглашение к вводу значения поиска */
                 const char *Oops       /* Сообщение при неудачном поиске      */
                );

/************************ редактирование массива ************************/
/* Записать массив данных в список     */
/* структур редактора строк         */
_RSPNLEXP int arr_to_list(
                          LSCROL  *L,        /* описатель редактора строк        */
                          void    *Arr,      /* массив данных           */
                          int     *CurrNum   /* текущее количество строк         */
                         );

/* Записать список структур редактора строк в   */
/* массив данных           */
_RSPNLEXP int list_to_arr(
                          LSCROL  *L,        /* описатель редактора строк        */
                          void    *Arr,      /* массив данных           */
                          int     *CurrNum   /* текущее количество строк         */
                         );

/* Освободить память,занятую под массив структур*/
/* редактора строк            */
_RSPNLEXP void free_line(
                         LSCROL  *L      /* описатель редактора строк        */
                        );

/* Редактор массива данных       */
_RSPNLEXP int LEdit_Arr(
                        LSCROL  *L,        /* описатель редактора строк        */
                        void    *Arr,      /* массив данных           */
                        int     *CurrNum   /* текущее количество данных массива      */
                       );

// Конструктор LSCROL.
_RSPNLEXP LSCROL *CrLs(
                       ResFile    *rt,     // Открытый ресурс.
                       const char *member  // Имя раздела.
                      );

/************************** Конец файла *********************************/

_RSPNLEXP void LSetFlds(
                        LSCROL   *l,
                        panel_keyproc_t  proc,  // Обработчик клавиатуры
                        void     *Parm,        // Для передачи параметров
                        void     *Pstatus,     // Информация для строки состояния
                        char     *Phead,       // Заголовок
                        panel_recproc_t  rec,  // Программа синхронизации адресов
                        void     *Ms,          // Массив записей
                        int       Mnumfl,      // Количество записей
                        int       LLine,       // длина строки данных
                        int       MaxL,        // максимальное количество строк
                        PANEL    *Mpi,         // панель ввода строки
                        PANEL    *Mpe,         // панель редактирования строки
                        lscrol_initrec_t  n,   // процедура инициализации строки
                        lscrol_testrec_t  i,   // процедура проверки при вводе строки
                        lscrol_testrec_t  c,   // процедура проверки при изменении строки
                        lscrol_testrec_t  d    // процедура проверки при удалении строки
                       );


/* Функция освобождения памяти, выделенной для скроллинга */

_RSPNLEXP void  FreeLScrol      (LSCROL *ls);
_RSPNLEXP void  LsSetMultiSelect(LSCROL *Ls);
_RSPNLEXP void *LsFirstSelected (LSCROL *Ls);
_RSPNLEXP void *LsNextSelected  (LSCROL *Ls, void *item);
_RSPNLEXP bool  LsIsRecSelected (LSCROL *Ls, int n);
_RSPNLEXP int   LsGetNumSelected(LSCROL *Ls);

#ifdef __cplusplus
}
#endif

#endif // __LEDIT_H

/* <-- EOF --> */