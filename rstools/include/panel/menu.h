//-*--------------------------------------------------------------------------*-
//
// File Name   : menu.h
//
// Copyright (c) 1991 - 2004 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
//   November 18, 1992 Emil Dovidovich  - Create file
//-*--------------------------------------------------------------------------*-

//
// Полноэкранный текстовый интерфейс для Borland C.
//

#ifndef __MENU_H
#define __MENU_H

#define  MENUH

#include <stdarg.h>

#include "fs.h"
#include "res.h"
#include "pexpdef.h"
#include "rstring.h"
#include "bdate.h"

#ifdef RSL_PL_FLAT
    #define  MNFAR
#else
    #define  MNFAR  _far
#endif

#include "fieldtp.h"
#include "platform.h"
#include "rstypes.h"
#include "rslmenu.h"

#include <packpshn.h>


#ifndef USE_HANDLE32
    #if defined(RSL_PL_WIN64) || defined(__EWEXTT_BUILD)
        #define  USE_HANDLE32
    #endif
#endif


#define  ON           1
#define  OFF          0
#define  YES          1
#define  NO           0

#define  RslLeftTop   0
#define  RslLeftBot   1
#define  RslRightTop  2
#define  RslRightBot  3

#define  NBAR          0x0000U          // Нет рамки
#define  SBAR          0x0200U          // Одиночная рамка
#define  DBAR          0x0100U          // Двойная рамка
#define  TBARTC        0x7E00U          // Для TabControl'а
#define  TBARTCP       0x8E00U          // Закладка TabControl'а
#define  TBAR          0xFE00U
#define  HBAR          0xFF00U

#define  PNL_BAR_TAG   0xFF

/*******************************************************************************
            Structure initializer macros

 DO NOT MAKE STATIC INITIALIZATION OF THAT STRUCTURES DIRECTLY!
 Use these macros instead
*******************************************************************************/
#define  PNL_TEXT_SINIT(style, value, x, y) { (style), (value), (x), (y) }

/**************************************************************************/

// -----------------------------------------------
// Константы для FIELD::FFlags
#define  RF_REQUIRED     0x00000004
#define  RF_NEGATIVE     0x00000008

#define  RF_ELIPSBTN     0x00001000  // Поле с кнопкой "..." (вызов справочника)
#define  RF_DOWNBTN      0x00002000  // Поле с кнопкой "" (выбор из списка)
#define  RF_EMPTY        0x00004000
#define  RF_TRIMZEROS    0x00008000

#define  RF_3STATE       0x00000100  // Признак CheckBox'а с тремя состояниями (совместно с RF_CHECKBOX)

#define  RF_GROUP        0x00010000
#define  RF_CHECKBOX     0x00020000
#define  RF_RADIO        0x00040000
#define  RF_PASSWORD     0x00080000

#define  RF_PROGRESS     0x00100000
#define  RF_TREEITEM     0x00200000
#define  RF_DYNLEN       0x00400000
#define  RF_INDIC        0x00800000

#define  RF_ASTEXT       0x01000000
#define  RF_NOEDGE       0x02000000
#define  RF_BLOCKHIDE    0x04000000  // Запретить скрывать колонку в автоскроллингах
#define  RF_FLAT         0x08000000

#define  RF_GROUPING     0x10000000
#define  RF_NOTABSTOP    0x20000000
#define  RF_NOCHECK      0x40000000
#define  RF_ANSI_STRING  0x80000000

// -----------------------------------------------
// Константы для FIELD::Ftype
#define  TYPEF            0x0F
#define  DUMMF            0x10
#define  HIDEF            0x20   // Поле спрятано пользователем

#define  DISABLE          0x01   // Пункт меню недоступен
#define  HIDDEN           0x02   // Пункт меню невидим
#define  DISINEMPTYSCROL  0x04   // Пункт меню недоступен в пустом скроллинге

#define  DISABLE_INT      0x08   // Пункт меню недоступен (Используется только в инструменте)
#define  BIGCMDNUMBER1    0x10   // Флаг установлен если код команды меню более MAX_SHORT.  x = Cmd - MAX_SHORT
#define  BIGCMDNUMBER2    0x20   // Флаг установлен если код команды меню более MAX_USHORT. x = Cmd - MAX_USHORT

#define  HIDEINEMPTYSCROL 0x40   // Пункт меню невидим в пустом скроллинге
#define  HIDE_INT         0x80   // Пункт меню невидим (Используется только в инструменте)


#define  isActF(t)         (!( (t) & DUMMF))
#define  setDF(t)          ((t) | DUMMF)
#define  typeF(t)          ((t) & TYPEF)

#define  isHideF(t)        ((t) & HIDEF)
#define  setHideF(t)       ((t) |= HIDEF)
#define  unsetHideF(t)     ((t) &= ~HIDEF)

#define  isDisableF(t)     ((t) & DUMMF)
#define  setDisableF(t)    ((t) |= DUMMF)
#define  unsetDisableF(t)  ((t) &= ~DUMMF)


#define  flagsF(t)         ((t) & ADDFLGF) // Added by (JNS), 12/02/1996

#define  FET   0x01      /* Редактируемое поле                           */
#define  FBT   0x02      /* Кнопка                                       */
#define  FBS   0x03      /* Кнопка с тенью                               */
#define  FWR   0x04      /* Редактируемое поле многострочное             */
#define  FVT   0x05      /* Нередактируемое поле                         */
#define  FSP   0x06      /* Subpanel                                     */
#define  FCL   0x07      /* Cluster (Check box or Radio button           */
#define  FVW   0x08      /* Многострочное нередактируюемое               */

#define  FHP   0x0F      /* Строка помощи                                */
#define  FDM   0x11

#define  D_MENUPANELNAME    "$MENU$"     // Идентификатор панели, являющейся панелью меню

#define  D_MENUBTRLBRNAME   "itemsyst.dbt"   // Символическое имя библиотеки ресурсов для оперменю
#define  D_MENUBTRRESNAME   "OPERMENU"       // Символическое имя ресурса оперменю


#define  D_HELPCONTEXTLEN      255       // Длина строки контекста помощи (с учётом 0-символа)
#define  D_HELPCONTEXT      "_CONTEXT_"  // Контекст
#define  D_HELPCONTEXTFILE  "\\file:\\"  // Метка для задания файла помощи

#include "strtobd.h"

typedef struct tagTEXT      TEXT;
typedef struct tagBAR       BAR;
typedef struct tagFIELD     FIELD;
typedef struct tagFIELD_EX  FIELD_EX;
typedef struct tagPANEL     PANEL;
typedef struct tagPSAVER    PSAVER;
typedef struct tagFDATA     FDATA;
typedef struct tagSCROL     SCROL;

typedef  int  (*JKeyProc)(PANEL *p, int key);  // Обработчик клавиатуры панели
typedef  void (*JRecProc)(SCROL *mn);          // Обработчик записи в скроллере

typedef JKeyProc  panel_keyproc_t;
typedef JRecProc  panel_recproc_t;


// -----------------------------------------------------------------------------
// Размещение новых констант в стиле текстового поля (TEXT::St)
// Размер поля St составляет 4 байта, 2 младших оставлены за существующей технологией.
// Формат старших 2 байт представлен ниже:
//
// ======= Старший байт старшего слова
// 31
// 30  Номер цвета (dBACK_COLOR_) - для EW-режима
// 29
// 28
// ----
// 27
// 26  Номер цвета (dTEXT_COLOR_) - для EW-режима
// 25
// 24
// ======= Младший байт старшего слова
// 23
// 22  Резерв
// ----
// 21
// 20
// 19  Форматирование (dTEXT_FORMAT_)
// 18
// ----
// 17  Выравнивание текстового поля (dTEXT_ALIGN_)
// 16
// ======= Старший байт младшего слова
// 15  Существующие компоненты стиля и сам стиль
// ...
//  0


// -----------------------------------------------------------------------------
// Константы выравнивания для TEXT. Находятся в младшем байте старшего слова поля St
#define  dTEXT_ALIGN_LEFT          0x00
#define  dTEXT_ALIGN_RIGHT         0x01
#define  dTEXT_ALIGN_CENTER        0x02

// Константы форматирования для TEXT. Находятся в младшем байте старшего слова поля St
#define  dTEXT_FORMAT_NORMAL       0x00
#define  dTEXT_FORMAT_BOLD         0x01
#define  dTEXT_FORMAT_ITALIC       0x02
#define  dTEXT_FORMAT_UNDERLINE    0x04
#define  dTEXT_FORMAT_STRIKEOUT    0x08

// Константы цвета для TEXT (палитра CGA/WEB)
#define  dTEXT_STYLE_BLACK            0       //
#define  dTEXT_STYLE_BLUE             1       //
#define  dTEXT_STYLE_GREEN            2       //
#define  dTEXT_STYLE_CYAN             3       //
#define  dTEXT_STYLE_RED              4       //
#define  dTEXT_STYLE_MAGENTA          5       //
#define  dTEXT_STYLE_BROWN            6       //
#define  dTEXT_STYLE_LTGRAY           7       //
#define  dTEXT_STYLE_DKGRAY           8       //
#define  dTEXT_STYLE_LTBLUE           9       //
#define  dTEXT_STYLE_LTGREEN         10       //
#define  dTEXT_STYLE_LTCYAN          11       //
#define  dTEXT_STYLE_LTRED           12       //
#define  dTEXT_STYLE_LTMAGENTA       13       //
#define  dTEXT_STYLE_YELLOW          14       //
#define  dTEXT_STYLE_WHITE           15       //

#define  dTEXT_STYLE_DEFAULT         99       // Цвет по-умолчанию (не во всех режимах поддерживается)

// Максимальное число цветов в палитре
#define  dTEXT_MAX_COLOR             16

// Константы цвета для BACK
#define  dBACK_STYLE_WHITE            0       //
#define  dBACK_STYLE_BLUE             1       //
#define  dBACK_STYLE_GREEN            2       //
#define  dBACK_STYLE_CYAN             3       //
#define  dBACK_STYLE_RED              4       //
#define  dBACK_STYLE_MAGENTA          5       //
#define  dBACK_STYLE_BROWN            6       //
#define  dBACK_STYLE_LTGRAY           7       //
#define  dBACK_STYLE_DKGRAY           8       //
#define  dBACK_STYLE_LTBLUE           9       //
#define  dBACK_STYLE_LTGREEN         10       //
#define  dBACK_STYLE_LTCYAN          11       //
#define  dBACK_STYLE_LTRED           12       //
#define  dBACK_STYLE_LTMAGENTA       13       //
#define  dBACK_STYLE_YELLOW          14       //
#define  dBACK_STYLE_BLACK           15       //

#define  dBACK_STYLE_DEFAULT         99       // Фон по-умолчанию (не во всех режимах поддерживается)

// Максимальное число цветов в палитре
#define  dBACK_MAX_COLOR             16

// -----------------------------------------------------------------------------
// Дефайны для внутриинструментального использования
#define  dTEXT_ALIGN_MASK    0x00030000         // Маска для получения выравнивания из стиля
#define  dTEXT_FORMAT_MASK   0x003C0000         // Маска для получения формата из стиля
#define  dTEXT_COLOR_MASK    0x0F000000         // Маска для получения цвета из стиля
#define  dBACK_COLOR_MASK    0xF0000000         // Маска для получения фона из стиля

#define  dTEXT_STYLE_MASK    0x0000FFFF         // Маска для получения стиля


// -----------------------------------------------------------------------------
// Прикладные дефайны

// Получить выравнивание из стиля
#define  GetAlignSt(St)             (((St) & dTEXT_ALIGN_MASK) >> 16)
#define  GetTextAlignSt(St)         GetAlignSt((St))

// Получить формат из стиля
#define  GetTextFormatSt(St)        (((St) & dTEXT_FORMAT_MASK) >> 18)

// Получить цвет из стиля
#define  GetTextColorSt(St)         (((St) & dTEXT_COLOR_MASK) >> 24)

// Получить фон из стиля
#define  GetBackColorSt(St)         (((St) & dBACK_COLOR_MASK) >> 28)

// Добавить выравнивание в стиль
#define  SetAlignSt(St, fl)         (((fl) << 16) | (St))
#define  SetTextAlignSt(St)         SetAlignSt((St))

// Добавить формат в стиль
#define  SetTextFormatSt(St, fl)    (((fl) << 18) | (St))

// Добавить цвет в стиль
#define  SetTextColorSt(St, fl)     (((fl) << 24) | (St))

// Добавить фон в стиль
#define  SetBackColorSt(St, fl)     (((fl) << 28) | (St))


// -----------------------------------------------------------------------------
// Выделение текстового поля заданными цветом и форматом
// [in,out] p      - указатель на панель
// [in]     n      - номер текстового поля
// [in]     color  - цвет текста (dTEXT_STYLE_*) (если < 0 - компонент не устанавливается)
// [in]     format - формат текста (dTEXT_FORMAT_*) (если < 0 - компонент не устанавливается)
_RSPNLEXP void SetPnTextColor(PANEL *p, int n, int color, int format);

// -----------------------------------------------------------------------------
// Выделение поля заданными цветом и форматом
// [in,out] p      - указатель на панель
// [in]     n      - номер поля
// [in]     color  - цвет текста (dTEXT_STYLE_*) (если < 0 - компонент не устанавливается)
// [in]     format - формат текста (dTEXT_FORMAT_*) (если < 0 - компонент не устанавливается)
_RSPNLEXP void SetPnFieldColor(PANEL *p, int n, int color, int format);

// -----------------------------------------------------------------------------
// Выделение поля заданным цветом, форматом и цветом фона
// [in,out] p      - указатель на панель
// [in]     n      - номер поля (если < 0 - все поля)
// [in]     color  - цвет текста (dTEXT_STYLE_*) (если < 0 - компонент не устанавливается)
// [in]     format - формат текста (dTEXT_FORMAT_*) (если < 0 - компонент не устанавливается)
// [in]     style  - цвет фона поля (dBACK_STYLE_*) (если < 0 - компонент не устанавливается)
_RSPNLEXP void SetPnFieldColorEx(PANEL *p, int n, int color, int format, int style);

// -----------------------------------------------------------------------------
// Выделение поля заданным стилем
// [in,out] p      - указатель на панель
// [in]     n      - номер поля (если < 0 - все поля)
// [in]     style  - стиль поля (-1 - обнуление всего St)
//                   Сам стиль должен представлять собой результат работы макроса
//                   ReS. Например, для установки стиля SMES параметру style нужно передать
//                   значение вида ReS(SMES, 0).
_RSPNLEXP void SetPnFieldStyle(PANEL *p, int n, int style);

// -----------------------------------------------------------------------------
// Выделение поля заданным стилем
// В отличие от SetPnFieldStyle параметру style можно непосредственно задавать значения стиля
// (SMES, SRMS и т.д.).
// [in,out] p      - указатель на панель
// [in]     n      - номер поля (если < 0 - все поля)
// [in]     style  - стиль поля (-1 - обнуление всего St)
_RSPNLEXP void SetPnFieldStyle2(PANEL *p, int n, int style);

// -----------------------------------------------------------------------------
// Выделение поля заданным стилем
// [in,out] p      - указатель на панель
// [in]     n      - номер поля (если < 0 - все поля)
// [in]     style  - стиль поля (-1 - обнуление всего St)
// [in]     color  - цвет фона поля (dBACK_STYLE_*) (используется, если в качестве style
//                   передано значение SUSR)
_RSPNLEXP void SetPnFieldStyleEx2(PANEL *p, int n, int style, int color);


// -----------------------------------------------------------------------------
// Размещение новых констант в стиле поля (FIELD::St2)
// Размер поля St2 составляет 4 байта.
//
// ======= Старший байт старшего слова
// 31
// ... Резерв
// 24
// ======= Младший байт старшего слова
// 23
// ... Резерв
// 16
// ======= Старший байт младшего слова
// 15  Существующие компоненты стиля и сам стиль
// ... Резерв
//  8
// ======= Младший байт младшего слова
//  7
// ... Резерв
//  0  Признак отметки записи в скроллинге (вместо простановки стиля)


// -----------------------------------------------------------------------------
#define  dFIELD_MARK_MASK    0x00000001         // Маска выделения поля в скроллинге

// Манипуляция с признаком выделения поля в скроллинге
#define  SetMarkField(St)           ((St) | dFIELD_MARK_MASK)
#define  UnSetMarkField(St)         ((St) & ~dFIELD_MARK_MASK)
#define  IsMarkField(St)            ((St) & dFIELD_MARK_MASK)

#define  SetFieldSt2(St, fl)        ((St) | ((fl) & dFIELD_MARK_MASK))


// -----------------------------------------------------------------------------
// Размещение новых констант в поле (FIELD::iconFlag)
// Размер поля iconFlag составляет 4 байта.
// Формат младшего байта представлен ниже:
//
// === Младший байт младшего слова
// ----
//  1  Выравнивание текстового поля (dICON_ALIGN_)
//  0


// -----------------------------------------------------------------------------
// Константы выравнивания для icon. Находятся в младшем байте младшего слова поля iconFlag
#define  dICON_ALIGN_LEFT          0x00
#define  dICON_ALIGN_RIGHT         0x01
#define  dICON_ALIGN_CENTER        0x02

// -----------------------------------------------------------------------------
// Дефайны для внутриинструментального использования
#define  dICON_ALIGN_MASK    0x00000003         // Маска для получения выравнивания


// -----------------------------------------------------------------------------
// Прикладные дефайны

// Получить выравнивание из флага
#define  GetIconAlignFlag(fl)         ((fl) & dICON_ALIGN_MASK)

// Добавить выравнивание во флаг
#define  SetIconAlignFlag(fl, align)  ((fl) | (align))


// -----------------------------------------------------------------------------
// Описатель текста
struct tagTEXT
     {
      unsigned int  St;       // Стиль изображения поля (старшее слово используется для выравнивания)
      char         *TValue;   // Значение в поле
      coord         x, y;     // Относительные координаты поля
     };

// Описатель прямоугольника
struct tagBAR
     {
      unsigned int  St;       // Стиль изображения прямоугольника
      uint8         x, y;
      uint8         l, h;     // Относительные координаты прямоугольника
     };

struct tagFIELD_EX
     {
      void        *fmt;            // Шаблон
      const char  *fmtName;        // Строка по которой создан шаблон
      void        *view;           // Редактор многострочного поля
      char        *tree;           // Параметры дерева
      FDATA       *find;           // Параметры поиска
      void        *edit;           // Текущее значение редактируемого поля

    // October 08, 2009
      char        *name2;          // Добавлено по запросу #2753

                                   // Добавлено в рамках запроса #3709 для FIELD::name и FIELD::tooltipStr
                                   // с целью модификации содержимого
      char        *name,           // Имя поля (только динамическая память !!!)
                  *tips;           // Всплывающая подсказка к полю (только динамическая память !!!)
     };

// Описатель поля
struct tagFIELD
     {
      unsigned char  Ftype;          // Тип поля (FET, FBT и т.д.)
      unsigned int   St;             // Стиль изображения поля
      unsigned char  FVt;            // Тип значения
      void          *FValue;         // Значение в поле
      int            FVp;            // Знаков после точки, макс. длина строки
      coord          x, y, l, h;     // Относительные координаты поля
      coord          kl, kr, ku, kd; // Индексы полей для навигации
      int            FHelp;          // Подсказка к полю

      unsigned long  FFlags;         // Flags for object field (JNS)
      char          *name;           // (K78) The name of this field. May the same as FValue
      char          *formatStr;      // (K78)
      short int      group;          // (K78)

      // Feb. 25, 1997
      unsigned int   Xpos;
      FIELD_EX      *Xdata;
      char          *tooltipStr;     // Текст всплывающей подсказки

      int            iconID;         // Идентификатор иконки, выводимой в поле
      int            iconFlag;       // Флаги к выводу иконки (dICON_ALIGN_)

      unsigned int   St2;            // Логическое продолжение St
     };

// -----------------------------------------------------------------------------
// [MENU]
// -----------------------------------------------------------------------------

#define  dMENU_EDITOR_EXTENSION   22334  // Идентификатор п.м. редактора "Внешние расширения"

// -----------------------------------------------
// Битовые флаги для пункта меню
#define  dFMENU_TOOLBAR   0x00000001     // Пункт меню попадает на ToolBar (ТБ)
#define  dFMENU_CONTEXT   0x00000002     // Пункт меню попадает в контекстное меню (КМ)

// -----------------------------------------------
// Сокращенные алиасы
#define  dFMENU_TB        dFMENU_TOOLBAR
#define  dFMENU_CM        dFMENU_CONTEXT

// -----------------------------------------------
// Комбинации флагов

// Пункт меню попадает и на ToolBar, и в контекстное меню
#define  dFMENU_TBCM      (dFMENU_TOOLBAR | dFMENU_CONTEXT)

// -----------------------------------------------
// Вспомогательные макросы для создания меню из прикладного кода

// Создание сепаратора
#define  RSCREATEMENU00(name)                                 \
                     { (name), 0 }

// Создание элемента вертикального меню без подпунктов
#define  RSCREATEMENU30(name, x, par)                         \
                     { (name), 0, 0, (x), (char *)(par), 0 }

// То же самое, но с указанием идентификатора картинки для ToolBar'а
#define  RSCREATEMENU31(name, x, par, iconID)                 \
                     { (name), 0, 0, (x), (char *)(par), 0, 0, 0, 0, 0, (iconID), 0 }

// То же самое, но с указанием флагов
#define  RSCREATEMENU32(name, x, par, iconID, flags)          \
                     { (name), 0, 0, (x), (char *)(par), 0, 0, 0, 0, 0, (iconID), (flags) }

// То же самое, но с включением на ТБ и в КМ
#define  RSCREATEMENU33(name, x, par, iconID)                 \
                     RSCREATEMENU32(name, x, par, iconID, dFMENU_TBCM)

// Создание элемента вертикального меню без подпунктов с указанием флагов (DISABLE/HIDE)
#define  RSCREATEMENU34(name, x, par, flags)                  \
                     { (name), 0, 0, (x), (char *)(par), 0, (flags), 0 }

// Создание элемента вертикального меню без подпунктов
#define  RSCREATEMENU40(name, x, par, help)                   \
                     { (name), 0, 0, (x), (char *)(par), (help) }

// То же самое, но с указанием идентификатора картинки для ToolBar'а
#define  RSCREATEMENU41(name, x, par, help, iconID)           \
                     { (name), 0, 0, (x), (char *)(par), (help), 0, 0, 0, 0, (iconID), 0 }

// То же самое, но с указанием флагов
#define  RSCREATEMENU42(name, x, par, help, iconID, flags)    \
                     { (name), 0, 0, (x), (char *)(par), (help), 0, 0, 0, 0, (iconID), (flags) }

// То же самое, но с включением на ТБ и в КМ
#define  RSCREATEMENU43(name, x, par, help, iconID)    \
                     { (name), 0, 0, (x), (char *)(par), (help), 0, 0, 0, 0, (iconID), dFMENU_TBCM }

// Создание элемента вертикального/горизонтального меню с подпунктами
#define  RSCREATEMENU50(name, v, n, x, par)                   \
                     { (name), (v), (n), (x), (char *)(par), 0 }

// То же самое, но с указанием идентификатора картинки для ToolBar'а
#define  RSCREATEMENU51(name, v, n, x, par, iconID)           \
                     { (name), (v), (n), (x), (char *)(par), 0, 0, 0, 0, 0, (iconID), 0 }

// То же самое, но с указанием флагов
#define  RSCREATEMENU52(name, v, n, x, par, iconID, flags)    \
                     { (name), (v), (n), (x), (char *)(par), 0, 0, 0, 0, 0, (iconID), (flags) }

// То же самое, но с включением на ТБ и в КМ
#define  RSCREATEMENU53(name, v, n, x, par, iconID)    \
                     { (name), (v), (n), (x), (char *)(par), 0, 0, 0, 0, 0, (iconID), dFMENU_TBCM }

// -----------------------------------------------
// Описатель меню
typedef struct tagMENU
      {
       const char    *name;     // Название пункта.
       void          *v;        // Подменю или программа. Иначе NULL.
       unsigned char  n;        // Количество пунктов подменю или 0 для конечного пункта.
       int            x;        // Для горизонтального меню положение пункта.
                                // Для вертикального - код выбора, при условии v == NULL.
       void          *par;      // Статус строка.
       int            Help;     // Номер страницы помощи.
       short          fl;       // Флаг (0/DISABLE/HIDDEN/DISINEMPTSCRL)
       uchar          ChPos;    // Позиция подсвеченного символа
       uchar          Reserv;
       void          *parm;     // Параметры пользователя

       uint32         iconID;   // Идентификатор иконки для ToolBar'а
       uint32         flags;    // Набор битовых флагов dFMENU_

       uint32         cmdEx;    // Расширение команды (набор флагов вида Alt, Shift, Ctrl для x).

       void          *ewdata;   // Данные для EasyWin-режима (см. EWMenuData). Используются только в нем.

       #ifdef USE_HANDLE32
            void     *res0;
            uint32    key;

            uint32    res1;
            uint32    res2;
            uint32    res3;
            uint32    res4;
            uint32    res5;
       #endif
      } MENU;

// -----------------------------------------------
typedef struct
      {
       PANEL          *p;
       unsigned short  n;
       char           *com;
       const char     *name;    // Наименование ресурса
       const char     *lbr;     // Наименование библиотеки ресурсов
       MENU            mn[1];
      } TMenu;

// -----------------------------------------------
typedef struct
      {
       MENU *freeMn;
       char *freeStr;
      } TLoadPtrs;

// -----------------------------------------------
// Описатель меню "Разное"
typedef struct MenuVar
      {
       MENU         *VarM;        // Указатель на пункт "Разное" в гориз. меню
       MENU         *HookM;       // Массив пунктов меню соотв. hook'ам
       uchar         HookN;       // Кол-во пунктов меню, соотв. hook'ам
      } MenuVar;

// -----------------------------------------------
// Описатель горизонтального меню, прикрепл.к панели
typedef struct PanHorMenu
      {
       MENU         *m;           // Массив пунктов гор. меню
       int           n;           // Кол-во пунктов гор. меню
       HRSLMENU      rm;          // Имеет значение, если меню создано из ресурса
       MenuVar       VarMenu;     // Описатель меню "Разное"
      } PanHorMenu;

// -----------------------------------------------
// Структура, передаваемая панели меню через *Parm
typedef struct MenuParm
      {
       MENU          *m;         // Массив пунктов меню
       unsigned char  n;         // Количество пунктов меню
       MenuVar        VarMenu;   // Описатель меню "Разное"

       #ifdef RSL_EASYWIN
           int        popup;
           int        x;
           int        y;
       #endif

       int            ret;       // id выбранного пункта меню
       uchar          clf;       // YES разрешает закрывать меню по ESC
      } MenuParm;

// -----------------------------------------------
// Параметры секции
typedef struct tagSECTPAR
      {
       LOBJ        list_elem;
       int         x;         // относит. координата; отр. значение - от правой границы
       int         cx;        // ширина
       bool        visible;   // текущая видимость видимость
       int         align;     // выравнивание 0 - влево, 1 - вправо
       char       *str;
      } SECTPAR;

// -----------------------------------------------
// Описатель секций
typedef struct tagSECTIONS
      {
       CNTLIST  SectList;               // Список секций
      } SECTIONS;

// -----------------------------------------------
// Дополнительное меню
typedef struct tagADDMENUPAR
      {
       LOBJ        list_elem;
       MENU       *m;          // Массив пунктов гор. меню
       int         n;          // Кол-во пунктов гор. меню
       HRSLMENU    rm;         // Имеет значение, если меню создано из ресурса

       int         key;        // код клавиши, соответсвующей пункту меню (м.б.0)
       char       *name;       // Имя пункта меню (м.б.NULL, при условии key!=0)
      } ADDMENUPAR;

// -----------------------------------------------
// Описатель дополнительных меню
typedef struct tagADDMENU
      {
       CNTLIST  AddMenuList;           // Список дополнительных меню
      } ADDMENU;


// -----------------------------------------------------------------------------
// [PANEL]
// -----------------------------------------------------------------------------
typedef struct tagPANEL_EX
      {
       void           *ew_Ext;         // Расширение панели для EasyWin
       SECTIONS       *Sect;           // Описатель секций
       void           *pblock;         // Область используемая при блокировке панели
       void           *fieldX;
       bool            freeneed;
       ADDMENU        *AdditionMenu;   // Описатель дополнительных меню
       bool            BsEditMode;
       char            PHelpContext[D_HELPCONTEXTLEN]; // Контекст помощи
       PANEL          *ownerTab;       // Панель-подложка для TabControl'а (по #3107)
      } PANEL_EX;

// -----------------------------------------------
// Описатель панели
struct tagPANEL
     {
      unsigned int    St;            // Стиль изображения панели
      FIELD          *PFList;        // Список полей панели
      unsigned char   Pnumf;         // Количество полей панели
      TEXT           *PTList;        // Список текстов
      unsigned char   Pnumt;         // Количество текстов в списке
      coord           x1,y1,x2,y2;   // Координаты панели (абсолютные)
      panel_keyproc_t proc ;         // Обработчик клавиатуры
      int             PHelp;         // Подсказка к панели
      void           *Parm;          // Для передачи параметров
      unsigned char   Pff;           // Номер начального поля
      char           *Pstatus;       // Информация для строки состояния
      char           *Phead;         // Заголовок
      FIELD          *Pcurfld;       // Текущее активное поле
      void           *PSave;         // Область сохранения

      void           *objPtr;
      unsigned long   flags;         // (K78)
      char           *statusRD;      // (K78) Status line for read only mode

      uchar           NeedGoToFldNum;
      uchar           firstf;
      uchar           lastf;
      uchar           reserv1;

      PanHorMenu      HorMenu;       // Описатель гориз.меню,прикрепл.к панели
      PANEL          *HorMenuPan;    // Указатель на панель гор.меню,прикр.к панели
      char           *ResName;       // Имя ресурса панели

      PANEL_EX       *Xdata;

      char           *ResFileName;   // Наименование lbr-файла. Из прикладного кода не трогать !!!

      unsigned long   flags2;        // Продолжение flags
     };


#define  RFP_REJREQ        0x00000004
#define  RFP_UPDREQ        0x00000008
#define  RFP_DELREQ        0x00000010

#define  RFP_CHNGF         0x00000020   // ширина поля скроллинга была изменена
                                        // динамически в EasyWin

#define  RFP_NOCLOSE       0x00000040   // для EasyWin

#define  RFP_RIGHTTEXT     0x00000080   // все тексты в панели имеют правое выравнивание
#define  RFP_TRANSPARENT   0x00000100   // Не используется (см. RFP_CENTERED)
#define  RFP_ANIMATED      0x00000200   // Не используется
#define  RFP_FLOATING      0x00000400
#define  RFP_PINX          0x00000800
#define  RFP_PINY          0x00001000
#define  RFP_NOAUTODIR     0x00002000
#define  RFP_NOAUTONUM     0x00004000
#define  RFP_NOSHADOW      0x00008000

#define  RFP_CENTERED      0x00000100   // Вместо RFP_TRANSPARENT

// This flags is used by BSCROL
#define  RFP_AUTOFILL      0x00010000   // расширять панель и обл. скролинга до экрана
#define  RFP_AUTOFIELDS    0x00020000   // автоматически устанавливать кординать полей в скролинге
#define  RFP_AUTOHEAD      0x00040000   // генерировать заголовки колонок в скролинге
#define  RFP_REVERSORDER   0x00080000   // обратный порядок записей
#define  RFP_DENYSORT      0x01000000   // запретить сортировку скролинга

#define  P_NAME2ALLOCED    0x00100000   // Рзмер поля name включён в общий размер панели. Флаг устанавливается в redit
                                        // при сохранении ресурса

// -----------------------------------------------
// Эти флаги находятся в PANEL::flags2
#define  RFP_MAXIMIZED     0x00200000   // Если размеры позволяют, распахивать скроллинг во весь экран
#define  RFP_MULTIHEAD     0x00400000   // Многострочные заголовки
#define  P_IMAGEEDITOR     0x00800000   // Редактор (просмотровщик) графических файлов
// -----------------------------------------------


#define  P_OUT             0x00100000   // выход из панели (скроллинга)
#define  P_PROC            0x00200000   // переход вызывается из польз. обработчика
#define  P_DELREC          0x00400000   // удаление записи скроллинга
#define  P_FLDFOCUS        0x00800000   // используется только в инструменте,
                                        // означает, что в текущее поле установлен
                                        // фокус, т.е., в обработчик послано ENTFIELD

#define  P_LOOPPANEL       0x00010000   // Панель запущена через LoopPanel (используется только в инструменте)

#define  P_MENU            0x01000000   // панель используется для отображения меню
#define  P_SCROL           0x02000000   // является Mp панелью SCROL'а
#define  P_LSCROL          0x04000000   // является Mp панелью LSCROL'а
#define  P_BSCROL          0x08000000   // является Mp панелью BSCROL'а

#define  P_PANEL_MASK      0x0f000000   // Маска для определения панели

#define  P_DISABLE         0x10000000
#define  P_TEXTEDITOR      0x20000000
#define  P_HELPER          0x40000000
#define  P_TABCTRL         0x80000000


#define  IsCenteredFl(p)      ((p)->flags & RFP_CENTERED)
#define  SetCenteredFl(p)     ((p)->flags |= RFP_CENTERED)
#define  UnsetCenteredFl(p)   ((p)->flags &= ~RFP_CENTERED)

#define  IsRightTextFl(p)      ((p)->flags & RFP_RIGHTTEXT)
#define  SetRightTextFl(p)     ((p)->flags |= RFP_RIGHTTEXT)
#define  UnsetRightTextFl(p)   ((p)->flags &= ~RFP_RIGHTTEXT)

#define  IsMaximizedFl(p)     ((p)->flags2 & RFP_MAXIMIZED)
#define  SetMaximizedFl(p)    ((p)->flags2 |= RFP_MAXIMIZED)
#define  UnsetMaximizedFl(p)  ((p)->flags2 &= ~RFP_MAXIMIZED)

#define  IsMultiheadFl(p)     ((p)->flags2 & RFP_MULTIHEAD)
#define  SetMultiheadFl(p)    ((p)->flags2 |= RFP_MULTIHEAD)
#define  UnsetMultiheadFl(p)  ((p)->flags2 &= ~RFP_MULTIHEAD)

#define  IsAutoFillFl(p)      ((p)->flags & RFP_AUTOFILL)
#define  SetAutoFillFl(p)     ((p)->flags |= RFP_AUTOFILL)
#define  UnsetAutoFillFl(p)   ((p)->flags &= ~RFP_AUTOFILL)

#define  IsAutoFieldsFl(p)    ((p)->flags & RFP_AUTOFIELDS)
#define  SetAutoFieldsFl(p)   ((p)->flags |= RFP_AUTOFIELDS)
#define  UnsetAutoFieldsFl(p) ((p)->flags &= ~RFP_AUTOFIELDS)

#define  IsAutoHeadFl(p)      ((p)->flags & RFP_AUTOHEAD)
#define  SetAutoHeadFl(p)     ((p)->flags |= RFP_AUTOHEAD)
#define  UnsetAutoHeadFl(p)   ((p)->flags &= ~RFP_AUTOHEAD)


#define  IsOutFl(p)           ((p)->flags & P_OUT)
#define  SetOutFl(p)          ((p)->flags |= P_OUT)
#define  UnsetOutFl(p)        ((p)->flags &= ~P_OUT)

#define  IsProcFl(p)          ((p)->flags & P_PROC)
#define  SetProcFl(p)         ((p)->flags |= P_PROC)
#define  UnsetProcFl(p)       ((p)->flags &= ~P_PROC)

#define  IsBsOutFl(bs)        ((bs)->Mp.flags & P_OUT)
#define  SetBsOutFl(bs)       ((bs)->Mp.flags |= P_OUT)
#define  UnsetBsOutFl(bs)     ((bs)->Mp.flags &= ~P_OUT)

#define  IsBsDenySort(bs)     ((bs)->flags & RFP_DENYSORT)
#define  SetBsDenySort(bs)    ((bs)->flags |= RFP_DENYSORT)
#define  UnsetBsDenySort(bs)  ((bs)->flags &= ~RFP_DENYSORT)

#define  IsBsProcFl(bs)       ((bs)->Mp.flags & P_PROC)
#define  SetBsProcFl(bs)      ((bs)->Mp.flags |= P_PROC)
#define  UnsetBsProcFl(bs)    ((bs)->Mp.flags &= ~P_PROC)

#define  IsDelRecFl(bs)       ((bs)->Mp.flags & P_DELREC)
#define  SetDelRecFl(bs)      ((bs)->Mp.flags |= P_DELREC)
#define  UnsetDelRecFl(bs)    ((bs)->Mp.flags &= ~P_DELREC)

#define  IsMenuFl(p)          ((p)->flags & P_MENU)
#define  SetMenuFl(p)         ((p)->flags |= P_MENU)

#define  IsFldFocus(p)        ((p)->flags & P_FLDFOCUS)
#define  SetFldFocus(p)       ((p)->flags |= P_FLDFOCUS)
#define  UnsetFldFocus(p)     ((p)->flags &= ~P_FLDFOCUS)

#ifndef SQLBUILD
    /* Установить режим недоступности для полей панели */
    #define  DisablePanFields(p) ((p)->flags |= P_DISABLE)

    /* Отменить режим недоступности для полей панели */
    #define  EnablePanFields(p)  ((p)->flags &= ~P_DISABLE)

    /* Проверить, что панель доступна */
    #define  IsEnablePanFlds(p)  (!((p)->flags & P_DISABLE))
#else
    #define  DisablePanFields(p)   BlockPanFields((p), BLOCK_READONLY_FIELDS)
    #define  EnablePanFields(p)    UnblockPanFields((p))
    #define  IsEnablePanFlds(p)    !IsBlockedPanFields((p))
#endif

#define  IsBScrlFl(p)     ((p)->flags & P_BSCROL)
#define  IsLScrlFl(p)     ((p)->flags & P_LSCROL)
#define  IsScrlFl(p)      ((p)->flags & P_SCROL)
#define  IsPanelFl(p)     (!((p)->flags & P_PANEL_MASK))

#define  IsChF(p)         ((p)->flags & RFP_CHNGF)
#define  SetChF(p)        ((p)->flags |= RFP_CHNGF)
#define  UnsetChF(p)      ((p)->flags &= ~RFP_CHNGF)

#define  IsNoClose(p)     ((p)->flags & RFP_NOCLOSE)
#define  SetNoClose(p)    ((p)->flags |= RFP_NOCLOSE)
#define  UnsetNoClose(p)  ((p)->flags &= ~RFP_NOCLOSE)


#define  IsChangeFld(p)   ((p)->NeedGoToFldNum != 255 && (p)->NeedGoToFldNum != (p)->Pcurfld - (p)->PFList)

#define  isTabStopFld(f)  (!((f)->FFlags & RF_NOTABSTOP))
#define  isAsTextFld(f)   (((f)->FFlags & RF_ASTEXT))
#define  isActiveFld(f)   (isActF((f)->Ftype) && !isHideF((f)->Ftype) && !isAsTextFld(f))

#define  isEmptyF(f)      (((f)->FFlags & RF_EMPTY))
#define  setEmptyF(f)     (((f)->FFlags |= RF_EMPTY))
#define  unsetEmptyF(f)   (((f)->FFlags &= ~RF_EMPTY))

#define  isBlockHideF(f)      (((f)->FFlags & RF_BLOCKHIDE))
#define  setBlockHideF(f)     (((f)->FFlags |= RF_BLOCKHIDE))
#define  unsetBlockHideF(f)   (((f)->FFlags &= ~RF_BLOCKHIDE))


// Область сохранения
struct tagPSAVER
     {
      int    PSa, PCs;       // атрибута
      coord  PCx, PCy;       // курсора
     };

/* Данные для поиска              */
struct tagFDATA
     {
      PANEL  *Mp;            /* Панель скроллинга                   */
      int     nfield;        /* Номер поля в скроллинге             */
      void   *vfield;        /* Значение записи                     */
      void   *value;         /* Значение поиска                     */
      int     len;           /* Длина значения                      */
      int     Vt;            /* Тип значения                        */
      int     cond;          /* Операция                            */
      int     maker;         /* Перед проверкой вызвать make()      */
     };

enum FindCondition
   {
    EQ = 0,
    NEQ,
    LT,
    GT,
    LE,
    GE,
    SUBSTR = 0,
    REGEXP,
    EQUALSTR,
    NOTEQUALSTR
   };

#define  NF(f)  sizeof(f)/sizeof(FIELD)
#define  NT(t)  sizeof(t)/sizeof(TEXT)

#define  SPartLine    0x00000001

#define  IsSPartLine(s)     (((s)->allocBuf && ((int *)(s)->allocBuf)[0] & SPartLine) ? 1 : 0)
#define  SetSPartLine(s)    ((s)->allocBuf ? (((int *)(s)->allocBuf)[0] |= SPartLine, 1) : 0)
#define  UnsetSPartLine(s)  ((s)->allocBuf ? (((int *)(s)->allocBuf)[0] &= ~SPartLine, 1) : 0)


#define  SFiltMod     0x0001   // Флаг устанавливается, если в SCROL'е используется пользовательский фильтр
#define  SNotQSearch  0x0002   // Флаг устанавливается, если в SCROL'е нужно отключить быстрый поиск

#define  IsSFiltMod(s)     ((s)->flags & SFiltMod)
#define  SetSFiltMod(s)    ((s)->flags |= SFiltMod)
#define  UnsetSFiltMod(s)  ((s)->flags &= ~SFiltMod)

#define  IsSNotQSearch(s)     ((s)->flags & SNotQSearch)
#define  SetSNotQSearch(s)    ((s)->flags |= SNotQSearch)
#define  UnsetSNotQSearch(s)  ((s)->flags &= ~SNotQSearch)

/* Описатель скролинговой области               */
struct tagSCROL
     {
      PANEL      Mp;             // Панель записи
      int        Mn;             // Количество записей в области
      coord      x, y, l, h;     // Координаты области
      panel_recproc_t  rec;      // Программа синхронизации адресов
      void      *Ms;             // Массив записей
      int        Mnumfl;         // Количество записей
      int        currfl;         // Номер текущей активной записи
      int        beginfl;        // Номер записи первой в области
      int        currbar;        // Номер текущего выбора в области
      coord      cursx;          // Координата курсора
      uchar    updtmod;          // Признак обновления скролинга
      PANEL   *Mps;              // Панель скролинга
      panel_keyproc_t  proc;     // Обработчик клавиатуры

      // 28 Mar 2002 A.Lebedev
      PANEL    wrkPan;
      char     isTrunc;          // True if last field in wrkPan is truncated
      uchar    startF;
      short    flags;
      uchar    reserve;
      char    *headBuff;
      void    *allocBuf;         // Указатель на динамически выделяемую память
     };

// 1996 (JNS)
enum NavigationDir
   {
    Left = -2,
    Up,
    None,
    Down,
    Right
   };

// Ф-ция для сопоставления строки с маской.
// Возвращает: 0 - строка соответствует маске,
//            !0 - в противном случае.
//
typedef int (* MaskMatchingProc)(const char *mask, const char *str);

// Ф-ция для преобразования маски в SQL-выражение.
// Возвращает указатель на buf или динамически выделенный буфер (если размера
// переданного буфера недостаточно), в котором сформировано SQL-выражение,
// порождаемое маской, или NULL, если произошла ошибка.
//
// Динамически выделенный буфер должен быть осовобожден с помощью free()
// в вызывающей ф-ции.
//
typedef char * (*ConvertMaskToSqlProc)(const char *mask, const char *fldname, char *buf, size_t sz);


#ifdef __cplusplus
extern "C" {
#endif

typedef int (*JChStrChange)(const char *olds, const char *s, int c);
_RSPNLEXP extern JChStrChange  edstring_callback;

// Lebedeva
typedef int (*SNRChange)(const char *olds,   // Старое значение поля
                         char       *s,      // Новое значение поля
                         int         size,   // Размер выделенной памяти
                         int         c,      // Код нажатой клавиши
                         PANEL      *p);

_RSPNLEXP SNRChange SetSNRCallback(SNRChange);

// Lebedeva

/************************************************************************/
/*                                                                      */
/*                       Работа с клавиатурой                           */
/*                                                                      */
/************************************************************************/

typedef int (* evfunction) (void);

_RSPNLEXP extern evfunction  getkb;
_RSPNLEXP extern evfunction  testkb;

/* Возврат кода клавиатуры в буфер      */
_RSPNLEXP void ungkb(int key);

// Внутриинструментальная версия ungkb (не меняет значение scan и kbflags).
// Возвращает предыдущее значение kbung. Параметр kb может быть NULL;
int ungetkb(int *key);

// Отмена ungkb(), удаляет клавишу из буфер клавиатуры
#define  clrkb()  (ungkb(0))

_RSPNLEXP int _getkb(void);              /* Ожидание и чтение кода с клавиатуры  */
_RSPNLEXP int _testkb(void);             /* Чтение кода с клавиатуры без ожидания */

_RSPNLEXP extern unsigned  dblklick; /* Интервал в тиках ( = 10) для определения DBLKLICK */

int getEvent(int waitTime); /* Чтение кода с клавиатуры или мыши с ожиданием или без */

// Added by Yakov E. Markovitch (JNS)
_RSPNLEXP void putCommand(int key);
_RSPNLEXP int  testCommand();
_RSPNLEXP int  testKey(int key);

_RSPNLEXP void playKString(const int *str);
_RSPNLEXP void playCString(const unsigned char *str);

/************************************************************************/
/*                                                                      */
/*                              Звук                                    */
/*                                                                      */
/************************************************************************/
/* Писк                                 */
_RSPNLEXP void beep(
                    int rep  /* Число повторов */
                   );

bool RslSetBeep(bool on);

/************************************************************************/
/*                                                                      */
/*                          Функции для экрана                          */
/*                                                                      */
/************************************************************************/

/* Вывести подкладку                            */
_RSPNLEXP void BordBar(
                       int x1,                // Координаты
                       int y1,                //
                       int x2,                //
                       int y2,                //
                       unsigned int atr,      // Атрибут
                       unsigned int brd       // Рамка
                      );

/* Вывести подкладку                            */
_RSPNLEXP void DrawBar(
                       int mode,              /* 0 - static, 1 - dynamic */
                       int x1,                /* Координаты                                   */
                       int y1,                /*                                              */
                       int x2,                /*                                              */
                       int y2,                /*                                              */
                       unsigned int atr,      // Атрибут
                       unsigned int brd       // Рамка
                      );

_RSPNLEXP void DrawShadow(PANEL *p) ;

_RSPNLEXP void PutPHead(PANEL *p, const char *Head) ;
_RSPNLEXP void PutPHeadStatic(PANEL *p, const char *Head);

_RSPNLEXP void SetFldRouting(
                             PANEL *p              // Описатель панели
                            );

_RSPNLEXP int SavePanelArea(
                            PANEL *p,              // Описатель панели
                            int sh                 // Shadow yes/no
                           );

// Подготовка прямоугольника панели на экране
_RSPNLEXP int PutPanelBar(
                          PANEL *p,              // Описатель панели
                          int    savef            // Флаг сохранения
                         );

/************************************************************************/
/*                                                                      */
/*              Функции преобразования типов                            */
/*                                                                      */
/************************************************************************/

/* Превратить значение в строку                 */
_RSPNLEXP char * valtostr(
                          void        *val,      /* Значение                                     */
                          int          w,        /* Ширина поля                                  */
                          int          p,        /* Знаков после точки                           */
                          unsigned int type,     /* Тип значения                                 */
                          int         *size,     /* Память выделенная под строку                 */
                          int          ljust     /* Выравнивание влево - ON                      */
                         );

/* Превратить строку в значение                 */
_RSPNLEXP void strtoval(
                        char        *s,        /* Строка                                       */
                        void        *V,        /* Значение                                     */
                        int          p,        /* Знаков после точки или максимальная длина    */
                        unsigned int type,     /* Тип значения                                 */
                        int          size      /* Размер выделенной памяти для строки          */
                       );

// Аналогично valtostr, только с учетом шаблона в FIELD'е
_RSPNLEXP char *valtostrEx(void *val, int w, int p, unsigned int type, int *size, int ljust, FIELD *pFld);

/* ?а?ўа вЁвм - з?-Ё? ў бваRЄг        */
_RSPNLEXP char* valtostrFmt(
                            void            *val,     /* ‡­ зҐ­ЁҐ                            */
                            int              w,       /* ЁаЁ­  Ї®«п                         */
                            int              p,       /* ‡­ Є®ў Ї®б«Ґ в®зЄЁ                  */
                            unsigned int     type,    /* ’ЁЇ §­ зҐ­Ёп                        */
                            int             *size,    /* ђ §¬Ґа ўл¤Ґ«Ґ­­®© Ї ¬пвЁ ¤«п бва®ЄЁ */
                            int              ljust,
                            const char      *fmtStr   /* и Ў«®­ */
                           );

// Аналогично strtoval, только с учетом шаблона в FIELD'е
_RSPNLEXP void strtovalEx(char *s, void *V, int p, unsigned int type, int size, FIELD *pFld);

/************************************************************************/
/*                                                                      */
/*                   Вывод и редактирование строки                      */
/*                                                                      */
/************************************************************************/

_RSPNLEXP void outlenS          /* Вывод строки с усечением             */
                      (
                       int   x,                       /* Позиция                              */
                       int   y,                       /* на экране                            */
                       const char *stro,              /* Строка для вывода                    */
                       int   outlen,                  /* Длина поля для вывода                */
                       int   offs                     /* Смещение первого выводимого символа  */
                      );

_RSPNLEXP void outlenSX         /* Вывод строки с усечением             */
                       (
                        int   x,                       /* Позиция                              */
                        int   y,                       /* на экране                            */
                        const char *stro,              /* Строка для вывода                    */
                        int   outlen,                  /* Длина поля для вывода                */
                        int   offs,                    /* Смещение первого выводимого символа  */
                        unsigned long fflags           /* Field flags                          */
                       );

_RSPNLEXP void outlenS_2        /* Вывод строки с усечением             */
                        (
                         int   x,                       /* Позиция                              */
                         int   y,                       /* на экране                            */
                         const char *stro,              /* Строка для вывода                    */
                         int   outlen,                  /* Длина поля для вывода                */
                         int   offs,                    /* Смещение первого выводимого символа  */
                         unsigned selAtr                /* Атрибут выделения                    */
                        );

_RSPNLEXP void outlenSX_2       /* Вывод строки с усечением             */
                         (
                          int   x,                       /* Позиция                              */
                          int   y,                       /* на экране                            */
                          const char *stro,              /* Строка для вывода                    */
                          int   outlen,                  /* Длина поля для вывода                */
                          int   offs,                    /* Смещение первого выводимого символа  */
                          unsigned long fflags,          /* Field flags                          */
                          unsigned selAtr                /* Атрибут выделения                    */
                         );

_RSPNLEXP extern unsigned InsCur; /* Форма курсора для вставки            */
_RSPNLEXP extern unsigned RepCur; /* Форма курсора для перекрытия         */

_RSPNLEXP extern int Ins;          /* Флаг режима вставки                  */
_RSPNLEXP extern const char *PasswSubst; /* Password substitution string (can be NULL)  */

_RSPNLEXP unsigned int edcs       /* Тип курсора для редактора строки             */
                           (
                            int insf               /* Флаг режима вставки                          */
                           );

_RSPNLEXP int edstring  /* Редактор строки                              */
                      (
                       int   x,               /* Позиция                                      */
                       int   y,               /* на экране                                    */
                       char *s,               /* Редактируемая строка                         */
                       int  *offsp,           /* Смещение начала в строке                     */
                       int  *cursp,           /* Позиция курсора  в строке                    */
                       int   outlen,          /* Длина поля для вывода                        */
                       int   maxlen,          /* Длина строки S                               */
                       int  *firstenter,      /* Если впервые зашел = OFF                     */
                       int  *Insert,          /* Режим вставки = ON                           */
                       void *Pict             /* Шаблон                                       */
                      );

_RSPNLEXP int edstringEx2    /* Редактор строки            */
                         (
                          int   x,     /* Позиция                 */
                          int   y,               /* на экране                                    */
                          char *s,     /* Редактируемая строка          */
                          int  *offsp,         /* Смещение начала в строке          */
                          int  *cursp,       /* Позиция курсора  в строке        */
                          int   outlen,      /* Длина поля для вывода         */
                          int   maxlen,      /* Максимальная длина строки S включая '\0'  */
                          int  *firstenter,  /* Если впервые зашел = OFF         */
                          int  *Insert,      /* Режим вставки = ON            */
                          void *Pict,     /* Шаблон               */
                          int   canEdit,
                          int  *ChangedFlag
                         );

_RSPNLEXP int FmtEdstringEx2     /* Редактор строки - с учетом форматного шаблона */
                            (
                             int   x,              /* Позиция */
                             int   y,              /* на экране  */
                             char *s,              /* Редактируемая строка */
                             int  *offsp,          /* Смещение начала в строке */
                             int  *cursp,          /* Позиция курсора  в строке */
                             int   outlen,         /* Длина поля для вывода */
                             int   maxlen,         /* Максимальная длина строки S включая '\0' */
                             int  *firstenter,     /* Если впервые зашел = OFF */
                             int  *Insert,         /* Режим вставки = ON */
                             void *Pic,            /* Шаблон */
                             int   canEdit,
                             int  *ChangedFlag
                            );

// Проверка шаблоном
_RSPNLEXP int CheckPict(char *s, FIELD *f);

// Получить максимально возможную ширину поля ввода для типа значения type,
// default_len - длина по-умолчанию
_RSPNLEXP int FIELD_GetMaxWidthByType(int type, int default_len);

// Получить максимально возможную ширину поля ввода для поля FIELD с нестроковым типом
// (функция напписана на основе CheckPict)
_RSPNLEXP int FIELD_GetMaxWidth(FIELD *f);

// Получить строковое представление поля
_RSPNLEXP char *GetFldString(PANEL *p, int field);

// Получить/установить наименование поля
_RSPNLEXP const char *GetFldName(FIELD *f);
_RSPNLEXP void        SetFldName(FIELD *f, const char *text);

// Получить/установить текст подсказки к полю
_RSPNLEXP const char *GetFldTips(FIELD *f);
_RSPNLEXP void        SetFldTips(FIELD *f, const char *text);

// Обновить подсказку к полю
_RSPNLEXP void        UpdateFieldTooltip(PANEL *p, int cf, const char *text);


/************************************************************************/
/*                                                                      */
/*                      Вывод элементов панели                          */
/*                                                                      */
/************************************************************************/

_RSPNLEXP void PutTexts /* Вывести текст                                */
                       (
                        PANEL *p               /* Описатель панели                             */
                       );

_RSPNLEXP void PutFET  // Вывести значение поля на экран.
                     (
                      coord     x1,          // Координаты
                      coord     y1,          // глобальные.
                      unsigned  fatr,        // Атрибут.
                      FIELD    *f            // Поле.
                     );

_RSPNLEXP void PutFET2 // Вывести значение поля на экран.
                      (
                       coord     x1,          // Координаты
                       coord     y1,          // глобальные.
                       FIELD    *f,           // Поле.
                       unsigned  ps,          // Палитра.
                       uchar     cmd          // Режим.
                      );

_RSPNLEXP void PutFBT  // Вывести значение кнопки на экран.
                     (
                      coord     x1,          // Координаты
                      coord     y1,          // глобальные.
                      FIELD    *f,           // Поле.
                      unsigned  ps,          // Палитра.
                      uchar     cmd          // Режим.
                     );

_RSPNLEXP void PutFBS  // Вывести кнопку с тенью на экран.
                     (
                      coord     x1,          // Координаты
                      coord     y1,          // глобальные.
                      FIELD    *f,           // Поле.
                      unsigned  ps,          // Палитра.
                      uchar     cmd,         // Режим.
                      uchar     fl           // Флаг нажатости.
                     );

_RSPNLEXP void PutWRP          /* Вывести многострочное поле        */
                     (
                      PANEL    *p,    // Панель.
                      FIELD    *f,    // Поле.
                      int canEdit
                     );

_RSPNLEXP void PutHLP             /* Вывести строку помощи                        */
                     (
                      PANEL *p,              /*                                              */
                      FIELD *f               /* Поле                                         */
                     );

_RSPNLEXP extern int  HiLight;
_RSPNLEXP extern int  BlockNav;
_RSPNLEXP extern int  MousePressFlag;

#define  FPutOff()  HiLight = OFF;

_RSPNLEXP void PutField(PANEL *p, FIELD *f);

/* Вывести поля                                 */
_RSPNLEXP void PutFields(
                         PANEL *p               /* Описатель панели                             */
                        );

_RSPNLEXP char *PrintField(PANEL *p, FIELD *f, char *buf, size_t bufsize);

_RSPNLEXP int IsCharLine(FIELD *f);

/************************************************************************/
/*                                                                      */
/*                     Калькулятор & Отладчик                           */
/*                                                                      */
/************************************************************************/


_RSPNLEXP extern panel_keyproc_t  cl;

#define  initcalc()  cl = calc

// Калькулятор
_RSPNLEXP int calc(PANEL *p, int key);

_RSPNLEXP extern unsigned long  StorLeft;

_RSPNLEXP extern panel_keyproc_t  db;

#define  initdb()  db = debugrep

// Отладчик
_RSPNLEXP int debugrep(PANEL *p, int key);

/************************************************************************/
/*                                                                      */
/*                       Связь с системой помощи                        */
/*                                                                      */
/************************************************************************/

_RSPNLEXP extern panel_keyproc_t  hp;

/* Попытаться получить помощь                   */
_RSPNLEXP int helper(PANEL *p, int key);


typedef int (* helpfunction)(int page, const char *context);
_RSPNLEXP extern helpfunction  bighelp;

/* Открыть окно помощи с нужной страницей */
_RSPNLEXP int _bighelp(int HPage, const char *context);

// То же, что и _bighelp, но с поддержкой нового механизма вызова справки
int _bighelpProxy(int HPage, const char *context);


#define  inithelp(s)  { hp = helper; RslAddHelpRange(0, 0, s); }

_RSPNLEXP bool RslAddHelpRange(int ind1, int ind2, const char *name);

_RSPNLEXP extern int  ContextHelp;
_RSPNLEXP extern int  ContextTemp;

#define  SetContext(page)   (ContextTemp = ContextHelp,              \
                             ContextHelp = page,                     \
                             ContextTemp )

#define  ResetContext(temp)  ContextHelp = temp

// -----------------------------------------------------------------------------
// Получить из контекста имя альтернативного файла и чистый контекст
// [in]  ptr  - полная строка контекста
// [out] file - файл помощи из полного контекста (может отсутствовать)
// [out] ctx  - контекст (может отсутствовать)
// [ret]      - 0 (в любом случае)
_RSPNLEXP int RslSplitHelpContext(const char *ptr, char *file, char *ctx);


/************************************************************************/
/*                                                                      */
/*                       Обработка полей                                */
/*                                                                      */
/************************************************************************/

_RSPNLEXP int procfld   /* Обслуживание поля                            */
                     (
                      PANEL *p               /* Описатель панели                             */
                     );

/* Обслуживание поля неизвестного типа (или панели вообще без поля) */
_RSPNLEXP extern int unknownfld(PANEL *p);


/************************************************************************/
/*                                                                      */
/*                       Работа с панелью                               */
/*                                                                      */
/************************************************************************/

_RSPNLEXP int Direction           // Построение маршрута обхода полей.
                       (                       // Возвращает индекс левого верхнего поля.
                        FIELD *f,              // Массив полей.
                        int    n               // Количество полей в массиве.
                       );

#define  SetDirec(p)    (p)->Pff = (unsigned char)Direction((p)->PFList, (p)->Pnumf)

// 15/02/96 (JNS)
_RSPNLEXP void testNSetFocus(PANEL *p, int disp, int d);

_RSPNLEXP char *psl      // Адрес области сохранения статуса в панели.
                   (
                    PANEL *p
                   );

_RSPNLEXP int PrePanel            // Подготовить панель
                      (
                       PANEL *p,    // Описатель панели
                       int   savef     // Флаг сохранения
                      );

_RSPNLEXP int PutPanel            /* Вывести панель                               */
                      (
                       PANEL *p,              /* Описатель панели                             */
                       int   savef            /* Флаг сохранения                              */
                      );

_RSPNLEXP int UsePanel            /* Цикл опроса панели                           */
                      (
                       PANEL *p               /* Описатель панели                             */
                      );

_RSPNLEXP void ResetField(PANEL *p, int fld);

_RSPNLEXP void RemPanel           /* Убрать  панель                               */
                       (
                        PANEL *p               /* Описатель панели                             */
                       );

_RSPNLEXP void DelPanel(PANEL *p);

_RSPNLEXP void MovePanel(PANEL *p, int x, int y, int from);


_RSPNLEXP int LoopPanel      // Отработать с панелью.
                       (                       // Возвращает -key или 0 при нехватке памяти.
                        PANEL *p     // Описатель панели.
                       );

int  PutMenuPanel(PANEL *p);
void RemMenuPanel(PANEL *p);

int  UseMenuPanel(
                  PANEL *p,              // Панель меню.
                  uchar  clf             // YES разрешает закрываться по ESC.
                 );


// -----------------------------------------------------------------------------
// Задать иконку для поля диалога
// [in,out] p      - указатель на панель
// [in]     n      - номер поля
// [in]     iconID - идентификатор иконки
// [in]     flags  - флаги отображения иконки
_RSPNLEXP void SetFieldIcon(PANEL *p, int n, int iconID, int flags);

// -----------------------------------------------------------------------------
// Функции для работы с контекстами помощи

// Внутренняя функция инициализации механизма контекстов
int InitHelpContext(void);

// Внутренняя функция деинициализации механизма контекстов
void DoneHelpContext(void);

// Установка контекста помощи
_RSPNLEXP void SetPHelpContext(PANEL *p, const char *context);

// Установка контекста помощи для заданного ресурса
// [in]  name    - идентификатор ресурса (регистр не учитывается). Если контекст с заданным name
//                 уже определён в списке, он будет перезаписан новым значением.
// [in]  context - контекст помощи для заданного ресурса. Если равен NULL, то контекст для
//                 данного ресурса будет удалён из списка.
// [ret] Возвращает true в случае успешного добавления контекста в список
_RSPNLEXP bool SetResHelpContext(const char *name, const char *context);

// Получение контекста помощи для заданного ресурса
// [in]  name    - идентификатор ресурса (регистр не учитывается)
// [in]  context - контекст помощи для заданного ресурса. Может быть NULL - в этом случае
//                 функция вернёт размер контекста.
// [ret] Возвращает размер строки контекста (без учёта 0-символа) или -1 в остальных
//       случаях (если контекст для запрашиваемого ресурса отсутствует).
_RSPNLEXP int GetResHelpContext(const char *name, char *context);


// Kireev 29.5.00 Функция для принудительной смены текущего поля
// Предназначена для вызова непосредственно из обработчиков.
_RSPNLEXP int PanGoToFld(PANEL *p, int disp);

//   Функции манипулирующие типами полей из panel.c
_RSPNLEXP void  DisableEditPanel  (PANEL *p);
_RSPNLEXP void  EnableEditPanel   (PANEL *p);
_RSPNLEXP int   SavePanelTypes    (PANEL *p);
_RSPNLEXP int   RestorePanelTypes (PANEL *p, int del);
_RSPNLEXP int   _SavePanelTypes   (PANEL *p, unsigned char **buf);
_RSPNLEXP int   _RestorePanelTypes(PANEL *p, int del, unsigned char **buf);

// Блокировка полей панели
enum
   {
    BLOCK_DISABLE_FIELDS  = 1,
    BLOCK_READONLY_FIELDS = 2
   };

_RSPNLEXP int  BlockPanFields    (PANEL *p, int flags);
_RSPNLEXP void UnblockPanFields  (PANEL *p);
_RSPNLEXP int  IsBlockedPanFields(PANEL *p);

//*************************** Скролинг. **********************************

_RSPNLEXP void UpdateScrol       // Вывести в скролинговую область.
                          (
                           SCROL *mn              // Описатель скролинга.
                          );

_RSPNLEXP int PutScrol           // Вывести область скролинга.
                      (
                       SCROL *mn,             // Описатель панели.
                       int   savef            // Флаг сохранения.
                      );

_RSPNLEXP int UseScrol           // Скролинг записей.
                      (
                       SCROL *mn              // Описатель скролинга.
                      );

_RSPNLEXP void RemScrol          // Убрать скролинг записей
                       (
                        SCROL *mn              // Описатель скролинга.
                       );

_RSPNLEXP int LoopScrol          // Вывести и запустить скроллинг на выполнение
                       (                                // Возвращает -key или 0 при нехватке памяти.
                        SCROL *mn              // Описатель скроллинга.
                       );

_RSPNLEXP void AdjustScrol
                          (
                           SCROL *mn,             // Описатель скролинга.
                           coord x,               // Координаты панели и количество линий
                           coord y,
                           coord l
                          );

_RSPNLEXP int ScrolGoToFld       // Принудительная смена поля
                          (                                // Предназначена для вызова непосредственно из обработчиков.
                           SCROL *mn,             // Описатель скроллинга.
                           int    disp            // Номер поля.
                          );

// Принудительная смена поля
_RSPNLEXP int ScrolGoTo(SCROL *mn, int disp, int dir);

// Поддержка горизонтального скроллинга
_RSPNLEXP void CopyScrolFields(SCROL *mn,int startF);
_RSPNLEXP void DrawScrolHeads(SCROL *mn);

// Изменение размера скроллинга ( последний паоаметр 0 )
_RSPNLEXP int ResizeScrol(PANEL *p, coord x, coord y, int fSizeMoveLoop);

int SResize(PANEL *p, int numRows, int l, bool isPartLine);

//************************** Сообщения. *********************************

_RSPNLEXP char **msgpars          // Разборка сообщения.
                        (
                         const char *msgs,            // Сообщение c разделителями '|'.
                         int  *n                // Количество строк.
                        );

_RSPNLEXP int msgwin              // Окно сообщения
                    (                       // возвращает код ESC.
                     const char *msgs,            // Сообщение c разделителями '|'.
                     int   x,               // Координаты панели,
                     int   y,               // для центрирования -1.
                     int   r                // Краснота.
                    );

_RSPNLEXP int msgwinH              // Окно сообщения
                     (                       // возвращает код ESC.
                      const char *msgs,            // Сообщение c разделителями '|'.
                      const char *Head,     // Заголовок окна
                      int   x,               // Координаты панели,
                      int   y,               // для центрирования -1.
                      int   r                // Краснота.
                     );

_RSPNLEXP int meswin              // Окно сообщения
                    (             // возвращает код ESC.
                     const char *msgs       // Сообщение c разделителями '|'.
                    );

_RSPNLEXP int meswinh             // Окно сообщения
                    (             // возвращает код ESC.
                     const char *msgs,      // Сообщение c разделителями '|'.
                     int         page       // Номер страницы помощи
                    );

_RSPNLEXP int meswinm             // Окно сообщения
                    (             // возвращает код ESC.
                     const char *msgs,      // Сообщение c разделителями '|'.
                     const char  *mname,    // Имя msg-файла               | Для поиска
                     int          mnumber   // Номер сообщения в msg-файле | страницы помощи
                    );

_RSPNLEXP int formwin(int rf, const char *fmt, ...);

#define  freemsg(list) { free(list[0]); free(list); }

/************************************************************************/
/*                                                                      */
/*                         Окна подтверждения                           */
/*                                                                      */
/************************************************************************/

typedef struct
      {
       int                  version;  // Now it's 0
       const char * const  *msg;      /* Сообщение                                    */
       int                  nm;       /* Число текстов сообщений                      */
       const char * const  *btext;    /* Тексты кнопок                                */
       int                  nb;       /* Число кнопок                                 */
       int                  ncf;      /* Текущая кнопока                              */
       const char          *Head;
       const char          *Statl;
       int                  infoType;
       long                 errCode;
      } CwinData;


typedef int (*CwinCallBackType)(const CwinData *p);

_RSPNLEXP CwinCallBackType SetCwinCallBack(CwinCallBackType newVal);


_RSPNLEXP const char *webwin(const char *url, const char *attr, const char *caption, const char *status,
                             const char *resField, size_t *outSz);

_RSPNLEXP void freeWebResult(const char *result);


_RSPNLEXP int confwin             /* Запрос подтверждения                         */
                     (
                      const char * const *msg, /* Сообщение                                    */
                      int   nm,              /* Число строк сообщения                        */
                      const char * const *btext,          /* Тексты кнопок                                */
                      int   lt,              /* Длина текста                                 */
                      int   nb,              /* Число кнопок                                 */
                      int   ncf,             /* Текущая кнопока                              */
                      int   x,               /* Координаты панели,                           */
                      int   y,               /* для центрирования -1                         */
                      int   r                /* Краснота                                     */
                     );

_RSPNLEXP int cwin               /* Запрос подтверждения                         */
                  (
                   const char * const *msg,            /* Сообщение                                    */
                   int   nm,              /* Число текстов сообщений                      */
                   const char * const *btext,          /* Тексты кнопок                                */
                   int   lt,              /* Длина текста                                 */
                   int   nb,              /* Число кнопок                                 */
                   int   ncf,             /* Текущая кнопока                              */
                   int   x,               /* Координаты панели,                           */
                   int   y,               /* для центрирования -1                         */
                   int   r,
                   const char  *Head,
                   const char  *Statl
                  );

_RSPNLEXP int cwinm              /* Запрос подтверждения                         */
                  (
                   const char * const *msg,            /* Сообщение                                    */
                   int   nm,              /* Число текстов сообщений                      */
                   const char * const *btext,          /* Тексты кнопок                                */
                   int   lt,              /* Длина текста                                 */
                   int   nb,              /* Число кнопок                                 */
                   int   ncf,             /* Текущая кнопока                              */
                   int   x,               /* Координаты панели,                           */
                   int   y,               /* для центрирования -1                         */
                   int   r,
                   const char  *Head,
                   const char  *Statl,
                   const char  *mname,    // Имя msg-файла               | Для поиска
                   int          mnumber   // Номер сообщения в msg-файле | страницы помощи
                  );

_RSPNLEXP int cwinEx               /* ‡ Їа®б Ї®¤вўҐа¦¤Ґ­Ёп                         */
                    (
                     const char * const *msg,            /* ‘®®ЎйҐ­ЁҐ                                    */
                     int   nm,              /* —Ёб«® вҐЄбв®ў б®®ЎйҐ­Ё©                      */
                     const char * const *btext,          /* ’ҐЄбвл Є­®Ї®Є                                */
                     int   lt,              /* „«Ё­  вҐЄбв                                  */
                     int   nb,              /* —Ёб«® Є­®Ї®Є                                 */
                     int   ncf,             /* ’ҐЄгй п Є­®Ї®Є                               */
                     int   x,               /* Љ®®а¤Ё­ вл Ї ­Ґ«Ё,                           */
                     int   y,               /* ¤«п жҐ­ваЁа®ў ­Ёп -1                         */
                     int   r,
                     const char *Head,
                     const char *Statl,
                     int   infoType,    // One of RSINFO_...
                     long  errCode
                    );

#define  RSINFO_MESSAGE  0
#define  RSINFO_WARNING  1
#define  RSINFO_ERROR    2
#define  RSINFO_FATAL    3


_RSPNLEXP int fwin              /* Вывод сообщения об ошибке  */
                  (
                   const char  *msgs,    /* Строка сообщения        */
                   PANEL       *p,       /* Текущая панель    */
                   const char  *Head,
                   const char  *Statl
                  );

_RSPNLEXP int fwinm             /* Вывод сообщения об ошибке  */
                  (
                   const char  *msgs,    /* Строка сообщения        */
                   PANEL       *p,       /* Текущая панель    */
                   const char  *Head,
                   const char  *Statl,
                   const char  *mname,   // Имя msg-файла               | Для поиска
                   int          mnumber  // Номер сообщения в msg-файле | страницы помощи
                  );

_RSPNLEXP int yesnowin                /* Запрос: Yes, No ?                            */
                      (
                       const char *msg,       /* Сообщение                                    */
                       int   ncf,             /* Текущая кнопока                              */
                       int   x,               /* Координаты панели,                           */
                       int   y                /* для центрирования -1                         */
                      );

_RSPNLEXP int PanelSetDialogFlag(int newFlag);
_RSPNLEXP int NoUserActions(void);

_RSPNLEXP int YesNo(const char *msg, int ncf, int page);

_RSPNLEXP int fldwin                        // Вывод сообщения об ошибке
                    (
                     const char *msgs,      // Строка сообщения
                     PANEL *p               // Текущая панель
                    );

_RSPNLEXP int fldwinh                       // Вывод сообщения об ошибке
                     (
                      const char *msgs,     // Строка сообщения
                      PANEL      *p,        // Текущая панель
                      int         page      // Номер страницы помощи
                     );

_RSPNLEXP int invwin    /* Ввод одного значения                         */
                    (
                     const char   *mes,     /* Строка приглашения <= 75 символов            */
                     unsigned char FVt,     /* Тип поля                                     */
                     void         *val,     /* Ссылка на вводимое значение                  */
                     int           FVp,     /* Знаков после точки                           */
                     coord         l        /* Ширина поля ввода                            */
                    );

_RSPNLEXP int invwinEx  /* Ввод одного значения                         */
                      (
                       const char   *mes,     /* Строка приглашения <= 75 символов            */
                       unsigned char FVt,     /* Тип поля                                     */
                       void         *val,     /* Ссылка на вводимое значение                  */
                       int           FVp,     /* Знаков после точки                           */
                       coord         l,       /* Ширина поля ввода                            */
                       int      passwd
                      );

_RSPNLEXP int invwinEx2 /* Ввод одного значения                         */
                       (
                        const char   *mes,     /* Строка приглашения <= 75 символов            */
                        unsigned char FVt,     /* Тип поля                                     */
                        void         *val,     /* Ссылка на вводимое значение                  */
                        int           FVp,     /* Знаков после точки                           */
                        coord         l,       /* Ширина поля ввода                            */
                        int           passwd,
                        int           page     /* Страница помощи                              */
                       );

_RSPNLEXP int invwinEx3 // Ввод одного значения
                       (
                        const char   *mes,     // Строка приглашения <= 75 символов
                        unsigned char FVt,     // Тип поля
                        void         *val,     // Ссылка на вводимое значение
                        int           FVp,     // Знаков после точки
                        coord         l,       // Ширина поля ввода
                        int           passwd,  // Выводить звёздочки
                        int           page,    // Страница помощи
                        const char   *head,    // Заголовок окна
                        const char   *stLine   // Статусная строка
                       );

_RSPNLEXP int infldval  /* Ввод одного значения                         */
                      (
                       const char   *Invite,  /* Приглашение к вводу значения поиска          */
                       FIELD        *f,       /* Поле поиска                                  */
                       void         *v,       /* Буфер для значения                           */
                       size_t        sz       /* Размер буфера                                */
                      );

_RSPNLEXP int infldval2 /* Ввод одного значения                         */
                       (
                        const char   *Invite,  /* Приглашение к вводу значения поиска          */
                        FIELD        *f,       /* Поле поиска                                  */
                        void         *v,       /* Буфер для значения                           */
                        size_t        sz,      /* Размер буфера                                */
                        int           page     /* Страница помощи                              */
                       );

// -----------------------------------------------------------------------------
// Работа с уведомлениями

// Отправить уведомление в статус-строку (только EasyWin режим)
// [in] head    - Заголовок уведомления
// [in] text    - Текст уведомления
// [in] secID   - Идентификатор секции статус-строки, для которого выводится уведомление (резерв)
// [in] iconID  - Идентификатор иконки
// [in] timeOut - Тайм-аут (сек)
// [ret]        - Не используется
_RSPNLEXP int RSSendNotify(const char *head, const char *text, int secID, int iconID, int timeOut);

// Отправить уведомление в статус-строку (только EasyWin режим)
// [in] head      - Заголовок уведомления
// [in] text      - Текст уведомления
// [in] secID     - Идентификатор секции статус-строки, для которого выводится уведомление (резерв)
// [in] iconID    - Идентификатор иконки
// [in] timeOut   - Тайм-аут (сек)
// [in] colorText - Цвет текста
// [in] colorBack - Цвет фона
// [ret]          - Не используется
_RSPNLEXP int RSSendNotifyEx(const char *head, const char *text, int secID, int iconID, int timeOut, int colorText, int colorBack);

// -----------------------------------------------------------------------------
//*********************** Индикатор выполнения. *************************

#define  MAX_INDIC  3

// Выбросить индикатор.
// [in] Nzp    - Всего записей.
// [in] Status - Сообщение в статус.
// [in] Head   - Заголовок.
_RSPNLEXP int PutIndic(long Nzp, const char *Status, const char *Head);

// Обновить индикатор.
// [in] Nop - Обработано записей.
_RSPNLEXP void UseIndic(long Nop);

// Убрать индикатор.
_RSPNLEXP void RemIndic(void);

// -----------------------------------------------------------------------------
// Сообщение в статус.
// [in] mes - Текст сообщения.
_RSPNLEXP char *PutWarn(const char *mes);

// Убрать сообщение.
// [out] wsave - Область сохранения статуса.
_RSPNLEXP void RemWarn(char *wsave);


// -----------------------------------------------------------------------------
// Стартовый идентификатор для пользовательских секций статус-бара.
#define  ID_USER_PANE  100


// Вывести текст в статус-бар
// Если секция с таким идентификатором отсутствует, она будет добавлена.
// [in] id   - идентификатор секции статус-бара (для пользовательских секций >= ID_USER_PANE)
// [in] text - текст выводимый в секцию
// [in] tips - текст всплывающей подсказки к тексту
_RSPNLEXP void PutStatusPane   (int id, const char *text, const char *tips);

// Вывести текст в статус-бар
// Если секция с таким идентификатором отсутствует, она будет добавлена.
// [in] id    - идентификатор секции статус-бара (для пользовательских секций >= ID_USER_PANE)
// [in] text  - текст выводимый в секцию
// [in] tips  - текст всплывающей подсказки к тексту
// [in] flags - флаги вида EW_SBT_
_RSPNLEXP void PutStatusPaneEx(int id, const char *text, const char *tips, int flags);

// Обновить текст в статус-баре
// Обновляет текст в секции с заданным идентификатором. Секция с заданным идентификатором
// должна существовать.
// Значение параметров - аналогично PutStatusPane.
_RSPNLEXP void UpdateStatusPane(int id, const char *text, const char *tips);

// Обновить текст в статус-баре
// Обновляет текст в секции с заданным идентификатором. Секция с заданным идентификатором
// должна существовать.
// Значение параметров - аналогично PutStatusPaneEx.
_RSPNLEXP void UpdateStatusPaneEx(int id, const char *text, const char *tips, int flags);

// Удалить секцию с заданным идентификатором из статус-бара
// [in] id   - идентификатор секции статус-бара
_RSPNLEXP void RemStatusPane(int id);

// Вывести иконку в статус-бар
// Если секция с таким идентификатором отсутствует, она будет добавлена.
// [in] id   - идентификатор секции статус-бара (для пользовательских секций >= ID_USER_PANE)
// [in] icon - идентификатор иконки
// [in] tips - текст всплывающей подсказки к иконке (допускается NULL и "")
_RSPNLEXP void PutStatusIcon(int id, int icon, const char *tips);

// Обновить иконку в статус-баре
// Обновляет иконку в секции с заданным идентификатором. Секция с заданным идентификатором
// должна существовать.
// Значение параметров - аналогично PutStatusIcon.
_RSPNLEXP void UpdateStatusIcon(int id, int icon, const char *tips);


_RSPNLEXP int MouseInPanel(PANEL *p);
_RSPNLEXP int MouseInMenuPanel(PANEL *p);

_RSPNLEXP int MouseInScrol(SCROL *mn);

_RSPNLEXP int MouseInFTreeItem(PANEL *p, FIELD *f);

_RSPNLEXP int MouseInFTreeBox(PANEL *p, FIELD *f);

_RSPNLEXP int MouseInField(PANEL *p, FIELD *f);

_RSPNLEXP int IndMouseField(PANEL *p);

_RSPNLEXP int HorNav(PANEL *p, int key);

_RSPNLEXP void NoMem(void);   // Сообщение "Нет памяти нажмите ESC".

// -----------------------------------------------------------------------------
_RSPNLEXP extern int CtrlBreakFlag;

_RSPNLEXP void SetCtrlBrk(void);

_RSPNLEXP void ResetCtrlBrk(void);

_RSPNLEXP int  IsCtrlBrk(int mouse);
_RSPNLEXP int  TestCtrlBrk(void);
_RSPNLEXP void ClearCtrlBrk(void);

_RSPNLEXP int  ForceSetCtrlBrk(int newFlag);
_RSPNLEXP void SendCtrlBrk(void);
_RSPNLEXP void SendKeyEvent(int key);


enum
   {
    RSCONFIRM_LEAVEEDIT = 1,
    RSCONFIRM_PRINTREP  = 2
   };

_RSPNLEXP void          SetConfirmFlags(unsigned long);
_RSPNLEXP unsigned long GetConfirmFlags(void);



/*   January 15,1998 (SD)
    Обработка клавиатуры
*/
typedef int (*ctrlbrk_idle_proc)(void);
_RSPNLEXP extern ctrlbrk_idle_proc CtrlBrkIdle;

// Расширенная CallBack-функция для обработчика CtrlBrk
// [in]   flagUserChoise - значение выбора пользователя (YES/NO) в ответ на запрос
//                         "Вы действительно хотите прервать работу?"
// [ret]  0 - не выполнять никаких действий (остальные значения зарезервированы)
typedef int (*ctrlbrk_idle_proc_ex)(int flagUserChoise);

// Установка новой CallBack-функции (возвращает предыдущее значение)
_RSPNLEXP ctrlbrk_idle_proc_ex SetCtrlBrkEx(ctrlbrk_idle_proc_ex newHandler);

// -----------------------------------------------------------------------------
// Добавляем текстовое поле в панель
_RSPNLEXP TEXT *FInsertT
                        (
                         PANEL *p,
                         int    St,       // Стиль изображения поля
                         char  *TValue,   // Значение в поле
                         coord  x,        // Относительные координаты поля
                         coord  y
                        );

// -----------------------------------------------------------------------------
// Добавляем прямоугольник в панель
_RSPNLEXP BAR *InsertB
                      (
                       PANEL *p,
                       int    St,         // Стиль изображения поля
                       coord  x,          // Относительные координаты прямоугольника
                       coord  y,
                       coord  l,
                       coord  h,
                       coord  fl          // Тип рамки (см. enum BORD в fs.h)
                      );

// -----------------------------------------------------------------------------
// Добавляем поле в панель
_RSPNLEXP FIELD *FInsertF
                         (
                          PANEL         *p,
                          unsigned char  Ftype,          // Тип поля
                          unsigned int   St,             // Стиль изображения поля
                          unsigned char  FVt,            // Тип значения
                          void          *FValue,         // Значение в поле
                          int            FVp,            // Знаков после точки, макс. длина строки
                          coord          x,              // Относительные координаты поля
                          coord          y,
                          coord          l,
                          coord          h,
                          coord          kl,             // Индексы полей для навигации
                          coord          kr,
                          coord          ku,
                          coord          kd,
                          int            FHelp           // Подсказка к полю
                         );

_RSPNLEXP PANEL *CreateP
                        (
                         unsigned int    St,      // Стиль изображения панели
                         unsigned char   Pnumf,   // Количество полей панели
                         unsigned char   Pnumt,   // Количество текстов в списке
                         coord           x1,      // Координаты панели
                         coord           y1,      // Координаты панели
                         coord           x2,      // Координаты панели
                         coord           y2,      // Координаты панели
                         int             PHelp,   // Подсказка к панели
                         unsigned char   Pff,     // Номер начального поля
                         unsigned        adsize
                        );

_RSPNLEXP PANEL *FCreateP
                         (
                          unsigned int    St,      // Стиль изображения панели
                          unsigned char   Pnumf,   // Количество полей панели
                          unsigned char   Pnumt,   // Количество текстов в списке
                          coord           x1,      // Координаты панели
                          coord           y1,      // Координаты панели
                          coord           x2,      // Координаты панели
                          coord           y2,      // Координаты панели
                          panel_keyproc_t proc,    // Обработчик клавиатуры
                          int             PHelp,   // Подсказка к панели
                          void           *Parm,    // Для передачи параметров
                          unsigned char   Pff,     // Номер начального поля
                          const char     *Pstatus, // Информация для строки состояния
                          const char     *Phead    // Заголовок
                         );

_RSPNLEXP SCROL *CreateS
                        (
                         unsigned int    St,      // Стиль изображения панели
                         unsigned char   Pnumf,   // Количество полей панели
                         unsigned char   Pnumt,   // Количество текстов в списке
                         coord           x1,      // Координаты панели
                         coord           y1,      //
                         coord           x2,      //
                         coord           y2,      //
                         int             PHelp,   // Подсказка к панели
                         unsigned char   Pff,     // Номер начального поля
                         int             Mn,      // Количество записей в области
                         coord           x,       // Координаты области
                         coord           y,
                         coord           l,
                         coord           h,
                         unsigned        adsize
                        );

_RSPNLEXP SCROL *FCreateS
                         (
                          unsigned int    St,      // Стиль изображения панели
                          unsigned char   Pnumf,   // Количество полей панели
                          unsigned char   Pnumt,   // Количество текстов в списке
                          coord           x1,      // Координаты панели
                          coord           y1,      // Координаты панели
                          coord           x2,      // Координаты панели
                          coord           y2,      // Координаты панели
                          panel_keyproc_t proc,    // Обработчик клавиатуры
                          int             PHelp,   // Подсказка к панели
                          void           *Parm,    // Для передачи параметров
                          unsigned char   Pff,     // Номер начального поля
                          const char     *Pstatus, // Информация для строки состояния
                          const char     *Phead,   // Заголовок.
                          int             Mn,      // Количество записей в области
                          coord           x,       // Координаты области
                          coord           y,
                          coord           l,
                          coord           h,
                          panel_recproc_t rec,     // Программа синхронизации адресов
                          void           *Ms,      // Массив записей
                          int             Mnumfl   // Количество записей
                         );

// -----------------------------------------------------------------------------

#include <packpsh1.h>

// -----------------------------------------------
typedef struct PanelR_0
      {
       db_uint16    St;
       uchar        Pnumf, Pnumt, Nb, Nt;
       r_coord      x1, y1, x2, y2;
       db_int16     PHelp;
       uchar        Pff;

       db_int16     Mn;
       r_coord      x, y, l, h;

       db_int16     len;
      } PanelR_0;

// -----------------------------------------------
typedef struct PanelR_1
      {
       db_uint16    St;
       uchar        Pnumf, Pnumt, Nb, Nt;
       r_coord      x1, y1, x2, y2;
       db_int16     PHelp;
       uchar        Pff;

       db_int16     Mn;
       r_coord      x, y, l, h;

       db_int16     len;

       db_uint32    flags;  // RFP_... flags
      } PanelR_1;

// -----------------------------------------------
typedef struct PanelR
      {
       db_uint16    St;
       uchar        Pnumf, Pnumt, Nb, Nt;
       r_coord      x1, y1, x2, y2;
       db_int16     PHelp;
       uchar        Pff;

       db_int16     Mn;
       r_coord      x, y, l, h;

       db_int16     len;

       db_uint32    flags;    // RFP_... flags
       db_uint32    flagsEx1, // Резерв
                    flagsEx2; // Резерв
      } PanelR;

// -----------------------------------------------
typedef struct FieldR_0
      {
       uchar          Ftype;
       db_uint16      St;
       uchar          FVt;
       db_int16       FVp;
       r_coord        x, y, l, h;
       r_coord        kl, kr, ku, kd;
       db_int16       FHelp;

       r_coord        vfl, lens;
      } FieldR_0;

// -----------------------------------------------
typedef struct FieldR_1
      {
       uchar          Ftype;
       db_uint16      St;
       uchar          FVt;
       db_int16       FVp;
       r_coord        x, y, l, h;
       r_coord        kl, kr, ku, kd;
       db_int16       FHelp;

       r_coord        vfl, lens;


       r_coord        nameLen, formLen;
       db_uint32      flags;
       db_int16       group;
      } FieldR_1;

// -----------------------------------------------
typedef struct FieldR
      {
       uchar          Ftype;
       db_uint16      St;
       uchar          FVt;
       db_int16       FVp;
       r_coord        x, y, l, h;
       r_coord        kl, kr, ku, kd;
       db_int16       FHelp;

       r_coord        vfl, lens;


       r_coord        nameLen, formLen;
       db_uint32      flags;
       db_int16       group;
       r_coord        tooltipLen;         // Длина строки всплывающей подсказки
      } FieldR;

// -----------------------------------------------
typedef struct TextR
      {
       db_uint16      St;
       r_coord        x, y;

       r_coord        vfl, lens;
      } TextR;

// -----------------------------------------------
typedef struct BordR
      {
       db_uint16  St;
       r_coord    x, y, l, h, fl;
      } BordR;

#include <packpop.h>

// -----------------------------------------------------------------------------

_RSPNLEXP int CrItem(int hd, PANEL *p, PanelR *pp, char *s, int ver, HRSLCVT hcvtRd, bool readName2);

_RSPNLEXP PANEL *CrPan            // Конструктор PANEL.
                      (
                       ResFile    *rt,       // Открытый ресурс.
                       const char *member    // Имя раздела.
                      );

_RSPNLEXP SCROL *CrScrl            // Конструктор SCROL.
                       (
                        ResFile    *rt,       // Открытый ресурс.
                        const char *member    // Имя раздела.
                       );

// Загрузчик SCROL.
_RSPNLEXP int GetScrol(ResFile *rf, void **pv);

_RSPNLEXP int ShowFV(const char *Fname, const char *Title);

_RSPNLEXP void SetFldsVa
                        (
                         PANEL          *p,
                         panel_keyproc_t proc,     // Обработчик клавиатуры
                         void           *Parm,     // Для передачи параметров
                         void           *Pstatus,  // Информация для строки состояния
                         const char     *Phead,    // Заголовок.
                         va_list ap
                        );

_RSPNLEXP void SetFlds
                      (
                       PANEL          *p,
                       panel_keyproc_t proc,       // Обработчик клавиатуры
                       void           *Parm,       // Для передачи параметров
                       void           *Pstatus,    // Информация для строки состояния
                       const char     *Phead,      // Заголовок.
                       ...
                      );

_RSPNLEXP void SSetFlds
                       (
                        SCROL          *m,
                        panel_keyproc_t proc,      // Обработчик клавиатуры
                        void           *Parm,      // Для передачи параметров
                        void           *Pstatus,   // Информация для строки состояния
                        const char     *Phead,     // Заголовок.
                        panel_recproc_t rec,       // Программа синхронизации адресов
                        void           *Ms,        // Массив записей
                        int             Mnumfl     // Количество записей
                       );

_RSPNLEXP int MouseInField(PANEL *p, FIELD *f);

_RSPNLEXP int IndMouseField(PANEL *p);

_RSPNLEXP void setstyle           // Инициализация стилей
                       (
                        int mode,              // Видеорежим
                        int mnt      // Тип монитора
                       );

// -----------------------------------------------------------------------------
// Элементы стиля
enum CLGET
   {
    CLBAR,          // Рамка и альтернативный текст
    CLTXT,          // Текст
    CLSHW,          //
    CLSLB,          //
    CLBTB,          // Подсвеченный символ (младший байт)
    CLBSN,          // Кнопка с тенью
    CLBSR,          // Кнопка с тенью инверсная
    CLBTN,          // Нередактируемое поле
    CLBTR,          // Нередактируемое поле инверсное
    CLSCN,          //
    CLFLN,          // Редактируемое поле
    CLFLR,          // Редактируемое поле инверсное
    // New styles
    CLBDS,          // Закрытая кнопка
    CLDIS,          // Закрытое поле

    // Подсветка синтаксиса в редакторе
    CL_KEYWORD,     // Ключевые слова
    CL_TYPE,        // Типы данных
    CL_STDFUNC,     // Стандартные процедуры
    CL_NUMBER,      // Числовые константы
    CL_OPER,        // Операторы
    CL_STRING,      // Строки
    CL_SPECNAME,    // Специальные имена
    CL_COMMENT      // Комментарии
   };

// -----------------------------------------------------------------------------
// Стили панелей
#define  SMES  0    // Стиль сообщений
#define  SRMS  1    // Стиль аварийных сообщений
#define  SHLP  2    // Стиль помощи
#define  SMEN  3    // Стиль меню
#define  SCOM  4    // Основной стиль
#define  SBCM  5    // Стиль с яркой рамкой
#define  SCRL  6    // Стиль справочных скроллингов

#define  SUSR  7    // Пользовательский стиль

// Вспомогательный дефайн для установки стиля
// В инструменте используется в виде St = ReS(SMEN, 0). В прикладном коде встречаются варианты
// в виде St = ReS(SMEN, St) и St = ReS(0, SHLP), и St = ReS(CLTXT, SHLP).
// Есть подозрение, что st - это стиль, а sp - компонент стиля (CLGET).
#define  ReS(sp, st)  (((sp + 1) << 8) | st)

// Получить из стиля атрибут заданного компонента
// [in] sp  - стиль панели (SMES и т.п.)
// [in] cmd - компонент стиля (CLTXT)
// [in] st  - стиль компонента панели (обычно 0)
_RSPNLEXP unsigned char GetCl   (unsigned sp, int cmd, unsigned st);


_RSPNLEXP unsigned char GetHotCl(unsigned sp, int cmd, unsigned st);
_RSPNLEXP unsigned char GetBord (unsigned sp);

_RSPNLEXP void ErRes(int iErr, const char *szMember);

/*   10.31.94 (K78)
*/
_RSPNLEXP int GetFieldSize   (int FVt, int FVp);
_RSPNLEXP int PanelSetFocus  (PANEL *p, int disp);
_RSPNLEXP int FindFirstActiv (FIELD *f, int n);
_RSPNLEXP int JPanelSetFocus (PANEL *p, int disp, int d);

// 1996 (JNS) You can replace right mouse button function assigning this
//           variable. It's ESC by default
_RSPNLEXP extern int  rightMouseButton;

_RSPNLEXP int ProcSNRTemplate(char *s);

_RSPNLEXP char *SetSNRTemplate(char *templ);
_RSPNLEXP int   SetMaxSNRLen(int len);
_RSPNLEXP int   SetSNRPos(int pos);


// -----------------------------------------------------------------------------
// Состояния для элемента CheckBox
enum
   {
    eCB_ERROR     = -1,     // Ошибка (неверный номер поля или тип элемента)
    eCB_UNCHECKED =  0,     // Не отмечен
    eCB_CHECKED,            // Отмечен
    eCB_UNDEFINE            // Неопределен (для CheckBox с тремя состояниями)
   };


// Обработчик клавиши Пробел для элемента CheckBox
// Работает с текущим полем панели. Выполняет циклическую смену состояний
// UNCHECKED -> CHECKED для обычного CheckBox'а, и UNCHECKED -> CHECKED -> UNDEFINE
// для CheckBox'а с тремя состояниями.
_RSPNLEXP void Press_Space(PANEL *p);

// Установить режим работы CheckBox'а
// [in] p          - указатель на панель
// [in] cf         - номер поля
// [in] flag3state - флаг режима:
//                   false - с двумя состояниями (обычный)
//                   true  - с тремя состояниями
// [ret]           - true  - режим успешно установлен
//                   false - ошибка установки режима
_RSPNLEXP bool SetCheckBoxMode(PANEL *p, int cf, bool flag3state);

// Получить состояние CheckBox'а
// [in] p          - указатель на панель
// [in] cf         - номер поля
// [ret]           - константа вида eCB_
_RSPNLEXP int GetCheckBoxState(PANEL *p, int cf);

// Установить состояние CheckBox'а. Возвращает предыдущее состояние.
// [in] p          - указатель на панель
// [in] cf         - номер поля
// [in] state      - константа вида eCB_
// [ret]           - константа вида eCB_
_RSPNLEXP int SetCheckBoxState(PANEL *p, int cf, int state);


void RslSetAppServSignal(void);

// -----------------------------------------------------------------------------
// Установить пользовательский обработчик сообщений вида FSCALLBACK_.
// Возвращает указатель на предыдущий пользовательский обработчик.
// Пользовательский обработчик вызывается первым. Если он вернул значение -1,
// стандартный обработчик вызван не будет.
_RSPNLEXP fs_callback_t SetUserFSCallback(fs_callback_t proc);

// -----------------------------------------------------------------------------
_RSPNLEXP extern panel_keyproc_t  msgPtr;


// MSEL

// инициализация буфера под список сообщений
_RSPNLEXP void InitMessageList(char **log);

// добавить строку в список сообщений
_RSPNLEXP int  AddMessageList(char *log, char *string);

// уничтожить список сообщений
_RSPNLEXP void FreeMessageList(char **log);

// показать список сообщений
_RSPNLEXP void ShowMessageList(char *head, char *log);


/* Функции установки меню панели */

_RSPNLEXP void SetHorMenu   (PANEL *p, MENU *m, int NumItems);
_RSPNLEXP void SetResHorMenu(PANEL *p, ResFile *rf, const char *member);

_RSPNLEXP int SetAddMenu   (PANEL *p, MENU *m, int NumItems, int key, const char *name);
_RSPNLEXP int SetResAddMenu(PANEL *p, ResFile *rf, const char *member, int key, const char *name);

void ClearAddMenu(PANEL* p);


/* Функция освобождения памяти, выделенной для меню, созданного из рессурса
   при вызове функции SetResHorMenu. Должна быть парной функцией. */
_RSPNLEXP void FreeResHorMenu(PANEL *p);

/* Функция для установки заданного флажка
пункта меню,  прикрепленного к скроллингу,  */
_RSPNLEXP int SetMenuItemFlag
                             (
                              MENU         *m,      // указатель на горизонтальное меню
                              unsigned char n,      // количество пунктов в горизонтальном меню
                              int           key,    // код горячей клавиши, соответствующей пункту меню (м.б.0)
                              const char   *name,   // название пункта меню, (м.б.NULL, при условии key!=0)
                              int           flag,   // Флажок, который нужно установить
                              int           active  // ON / OFF
                             );

_RSPNLEXP int SetPanMenuFlag  // Установить флаг пункта меню, прикр. к панели
                            (
                             PANEL      *p,      // указатель на панель, к которой прикреплено меню, или на панель меню
                             int         key,    // код клавиши, соответсвующей пункту меню (м.б.0)
                             const char *name,   // Имя пункта меню (м.б.NULL, при условии key!=0)
                             int         flag,   // Флажок, который нужно установить
                             int         active  // ON / OFF
                            );

// спрятать пункт меню
_RSPNLEXP int HideMenuItem
                          (
                           PANEL      *p,       // указатель на панель, к которой прикреплено меню, или на панель меню
                           int         key,     // код клавиши, соответсвующей пункту меню (м.б.0)
                           const char *name,    // Имя пункта меню (м.б.NULL, при условии key!=0)
                           int         active   // ON(видим) / OFF(невидим)
                          );

// Установить иконку для пункта меню
// [in]  p      - указатель на панель, к которой прикреплено меню, или на панель меню
// [in]  key    - код клавиши, соответсвующей пункту меню (м.б.0)
// [in]  name   - имя пункта меню (м.б. NULL, при условии key != 0)
// [in]  iconID - идентификатор иконки
// [ret]        - предыдущее значение идентификатора иконки
_RSPNLEXP int IconMenuItem(PANEL *p, int key, const char *name, int iconID);

// Установить флаги включения для пункта меню (ToolBar, контекстное меню и т.п.)
// [in]  p      - указатель на панель, к которой прикреплено меню, или на панель меню
// [in]  key    - код клавиши, соответсвующей пункту меню (м.б.0)
// [in]  name   - имя пункта меню (м.б. NULL, при условии key != 0)
// [in]  flags  - флаги включения пункта меню (см. константы вида dFMENU_)
// [ret]        - предыдущее значение флага
_RSPNLEXP int FlagsMenuItem(PANEL *p, int key, const char *name, int flags);

// Сделать пункт меню недоступным
_RSPNLEXP int DisableMenuItem
                             (
                              PANEL      *p,      // указатель на панель, к которой прикреплено меню, или на панель меню
                              int         key,    // код клавиши, соответсвующей пункту меню (м.б.0)
                              const char *name,   // Имя пункта меню (м.б.NULL, при условии key!=0)
                              int         active  // ON(доступен) / OFF(недоступен)
                             );


// -----------------------------------------------------------------------------
// Установить имя библиотеки ресурсов и название ресурса для панели
// [out] p       - Панель
// [in]  lbrName - Наименование библиотеки ресурсов
// [in]  resName - Наименование ресурса
_RSPNLEXP void SetResNamePanel(PANEL *p, const char *lbrName, const char *resName);

// Установить имя библиотеки ресурсов и название меню
// [out] p       - Панель меню
// [in]  lbrName - Наименование библиотеки ресурсов
// [in]  resName - Наименование ресурса
// [in]  toPanel - false: параметры записываются в p->HorMenu.rm
//                 true:  если структура p->HorMenu.rm отсутствует, параметры будут установлены
//                        для панели
_RSPNLEXP void SetResNameMenu(PANEL *p, const char *lbrName, const char *resName, bool toPanel);

// Установить имя библиотеки ресурсов и название меню на базе itemsyst.dbt/opermenu.dbt
// Фактически использует SetResNameMenu с предопределенными именами D_MENUBTRLBRNAME и D_MENUBTRRESNAME
_RSPNLEXP void SetResNameOperMenu(PANEL *p);

// -----------------------------------------------------------------------------
_RSPNLEXP extern MENU vVarMenuPan[4];

_RSPNLEXP extern MENU vVarMenuScrol[11];
_RSPNLEXP extern MENU vVarMenuScrol_NoSel[10];

_RSPNLEXP extern MENU vVarMenuBs[15];
_RSPNLEXP extern MENU vVarMenuBs_NoSel[10];

#ifdef SQLBUILD
    _RSPNLEXP extern MENU vVarMenuBsA[17];
    _RSPNLEXP extern MENU vVarMenuBs_NoSelA[12];
#else
    _RSPNLEXP extern MENU vVarMenuBsA[16];
    _RSPNLEXP extern MENU vVarMenuBs_NoSelA[11];
#endif

// LэшЎшрышчшЁютрЄ№ ьрёёштv ёЄрЄшўхёъшї ьхэ¦ яхЁхф яхЁтvщ шёяюы№чютрэшхь
// vVarMenuBsA, vVarMenuBs_NoSelA.
_RSPNLEXP void InitVarMenu(void);

// обработчик клавиатуры панели (используется только в инструменте)
int PanProc(PANEL *p, int key );


// Найти пункт вертикального меню (используется только в инструменте)
MENU *FindMenuItem
                  (
                   const MENU   *m,              // Указатель на вертикальное меню
                   unsigned char n,              // Количество пунктов меню
                   int           key,            // Горячая клавиша (м.б. == 0)
                   const char   *name            // Задается если key == 0
                  );

// Найти пункт в меню, прикрепленном к панели
_RSPNLEXP MENU *FindPanMenuItem(PANEL *p, int key, const char *name);

// Сравнить названия пунктов меню (используется только в инструменте)
_RSPNLEXP int CmpMenItemName(const char *name1, const char *name2);

// Установить меню "Разное" (используется только в инструменте)
_RSPNLEXP int SetVarMenu(PANEL *p, MENU *m, int n, MENU *vm, unsigned char vn);

// Отправить сообщение о выходе с поля при выходе из панели
_RSPNLEXP extern int SendFldMesOut(PANEL *p);

int SendFldMesIn(PANEL *p);


int FindMenuKeyAct(MENU *m, unsigned char n, int key);

/* Функция освобождения памяти, выделенной для панели.
   Обязательно должна использоваться, если к панели прикреплено меню, загруженное из ресурса  */
_RSPNLEXP void FreePanel(PANEL *p);

/* Функция освобождения памяти, выделенной для скроллинга */
_RSPNLEXP void FreeScrol(SCROL *s);


/************************ Печать SCROL ***********************************/
typedef int (*ScrollPrint)(SCROL *, const char *);

_RSPNLEXP void SetPrintS(ScrollPrint);         // Установить печать
_RSPNLEXP void SetPrintSXml(ScrollPrint);      // Установить экспорт в XML

_RSPNLEXP void SetPrnDirS(const char *dir);    // установить дир.для файлов с распечатанным скролингом

/************* Автоматическое определение ширины колонки *****************/

#define AUTO_LEN_SIZE  100

_RSPNLEXP int SetAutoWidth(int ind, int w);
_RSPNLEXP coord getAutoLen(FIELD *f);
_RSPNLEXP int SetFmtByW(int, const char *);
_RSPNLEXP int SetFmtByType(unsigned char, const char *);
_RSPNLEXP const char *GetFmtByType(unsigned char chType);
_RSPNLEXP const char *GetFmtByField(FIELD *);

const char *GetFmtByW(FIELD *);
const char *GetFmtByFldType(FIELD *);


void CreateFormatsForPanel(PANEL *);
void DeleteFormatsForPanel(PANEL *);

void RecreateFormatsByType(PANEL *);
_RSPNLEXP void RecreateFormatsForField(PANEL *, FIELD *);

void AllocFieldXData   (PANEL *p);
void FreeFieldXData    (PANEL *p);
void PreAllocFieldXData(PANEL *p, int numf);

/*************** Поддержка горизонтального скроллинга *********************/

int FirstActiv(PANEL *p);
int FirstVisible(PANEL *p);
int LastVisible(PANEL *p);
int FindFirstField(PANEL *p, int lastFld, int len);
int FieldVisible(coord l, FIELD *f, coord startX, coord *len);
coord SetUpFields(PANEL *p);

/****************************************************************************/

int  SIsFromMpiMode(SCROL *s);
void SUnsetFromMpiMode(SCROL *s);

int  SIsFromMpeMode(SCROL *s);
void SUnsetFromMpeMode(SCROL *s);

int ScrStandardProc(PANEL *p, int key);

// -----------------------------------------------------------------------------
_RSPNLEXP void SSetBlkPostRec(int val);  // временная функция для отключения и включения блокировки POSTREC
_RSPNLEXP void SSetDblStrHead(int val);  // включение/отключение переноса названия поля на вторую строку

_RSPNLEXP PANEL *ClonePanel  (PANEL *p, bool usePanelMenu);
_RSPNLEXP int    ResizePanel (PANEL *p, int l, int h, int flag);
_RSPNLEXP void   RemoveBorder(PANEL *p);

_RSPNLEXP MENU *CloneMenu    (const MENU *m, int n);
_RSPNLEXP MENU *CreateDynMenu(const MENU *m, unsigned char *n, ADDMENU *AddMenuInf);

// -----------------------------------------------------------------------------
extern int  SBlockPostRec;

int compleft(FIELD *cf, FIELD *nf, FIELD *f);
int compup  (FIELD *cf, FIELD *nf, FIELD *f);
int compdown(FIELD *cf, FIELD *nf, FIELD *f);

// Сохранение / восстановление параметров (ширины колонок скроллинга)
typedef int (*SaveRestParm)
      (
       PANEL   *mp,               // Указатель на панель записи скроллинга
       void    *userParm,         // Указатель на буфер с параметрами
       size_t  *userParmSize,     // Размер буфера
       int      saveRestOperation // value from SaveRestOperationEnum
      );

enum SaveRestOperationEnum
   {
    restoreColumnWidth =   0,   saveColumnWidth,
    restoreFilterData  = 100,   saveFilterData,
    restoreSortData    = 200,   saveSortData
   };

_RSPNLEXP void SetBsSaveParm  (SaveRestParm);
_RSPNLEXP void SetScrlSaveParm(SaveRestParm);

_RSPNLEXP void setHideField(PANEL *p, int nColumn, unsigned int nWidth);

_RSPNLEXP void setChngField(PANEL *p);

const char * const *WrapStringList(const char * const *msg, int num, int outlen, int height, int *newNum);
void FreeWrapList(const char * const *msg, int num);

_RSPNLEXP void PutIndAct(const char *msg);
_RSPNLEXP void RemIndAct(void);


#define  RS_ACTION_NOCANCEL       1
#define  RS_ACTION_CLEAR_ALWAYS   0
#define  RS_ACTION_CLEAR_WAIT     1

_RSPNLEXP void RsBeginActionEx2(unsigned tm, const char *msg, const char *header, unsigned flags, bool useAnsi);
_RSPNLEXP void RsBeginActionEx (unsigned tm, const char *msg, unsigned flags);
_RSPNLEXP void RsBeginAction   (void);
_RSPNLEXP void RsEndAction     (void);
_RSPNLEXP void RsEndActionEx   (unsigned tm);
_RSPNLEXP void RsClearAction   (int flag);
_RSPNLEXP void RsChangeAction  (const char *msg);

void RsPutActionIndic   (void *data);
void RsRemActionIndic   (void *data);
void RsChangeActionIndic(void *data);


int TransferField(PANEL *Mps, int Index, int toIndex);

int changeFieldNav(PANEL *p, int N, int toN);
int restFieldNav  (PANEL *p, db_int16 *buf);

void InitDelayedUpdate(void);
void DoneDelayedUpdate(void);

bool IsDelayedUpdateTexts (PANEL *);
bool IsDelayedUpdateFields(PANEL *);

void UpdatePanelTexts (PANEL *);
void UpdatePanelFields(PANEL *);

bool UpdateStatLine      (void);
bool UpdateDelayedPanel  (void);
bool UpdateDelayedActions(void);

_RSPNLEXP void EnableDelayedUpdate (PANEL *pnl);
_RSPNLEXP void DisableDelayedUpdate(PANEL *pnl, int update);


_RSPNLEXP PANEL *GetActivePanel(void);
PANEL *SetActivePanel(PANEL *pnl);

void BPutFiltFlag(PANEL *p);
void BDelFiltFlag(PANEL *p);

PANEL_EX *CreatePXdata(PANEL *p);

_RSPNLEXP int  AddPanelSection(PANEL *p, int x, int cx, int align, bool visible);
_RSPNLEXP void SetPanelSection(PANEL *p, int x, const char *str);

_RSPNLEXP void SetPanelSectWd(PANEL *p, int x, int cx);
_RSPNLEXP void ClearAllSect  (PANEL *p);

_RSPNLEXP void PutPanelSection (PANEL *p, int x);
_RSPNLEXP void RemPanelSection (PANEL *p, int x);
_RSPNLEXP void PutPanelTextSect(PANEL *p, int x);

void PutAllSect(PANEL *p);

int   CheckFdataCondition(FDATA *fd);
char *ConvertFdataToSqlString(FDATA *fd, const char *fldname, char *buf, size_t size);

_RSPNLEXP MaskMatchingProc     SetMaskMatchingProc    (MaskMatchingProc     proc);
_RSPNLEXP ConvertMaskToSqlProc SetConvertMaskToSqlProc(ConvertMaskToSqlProc proc);

int UseMaskMatching(void);

// Сообщения о входе/выходе из записи
int  SExitRec   (SCROL *s);
void SEntIntoRec(SCROL *s);

void PutFiltFlag(PANEL *p);
void DelFiltFlag(PANEL *p);

int  SelMake  (SCROL *s);
int  SelProc  (PANEL *p, int key);
void SelMouse (SCROL *s, int oldcr);

// -----------------------------------------------------------------------------
typedef int (*UnlockInputProc)(void *data);

_RSPNLEXP void LockUserInput(int flags, UnlockInputProc unlockProc, void *data);
_RSPNLEXP void UnlockUserInput(void);


_RSPNLEXP bool IsBsEditMode(PANEL *p);
void SetBsEditMode  (PANEL *p);
void UnsetBsEditMode(PANEL *p);

MENU *GetMenuPtrFrom(void *mn, int *num);

// Центровка панели (консольные координаты)
// Допустимо вызывать и для BSCROL::Mp
void CenterPanelConsole(PANEL *p);

// Установка режима занятости для ToolBar'а
// 1 (true)  - установить режим занятости
// 0 (false) - снять режим занятости
_RSPNLEXP int RsSetBusy(int busy);

#ifdef __cplusplus
};
#endif

#include <packpop.h>

#endif

/* <-- EOF --> */