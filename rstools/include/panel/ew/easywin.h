//-*--------------------------------------------------------------------------*-
//
// File Name   : easywin.h
//
// Copyright (c) 1991 - 2004 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// February 22,2001  Alexey Lebedev - Create file
//-*--------------------------------------------------------------------------*-

#ifndef __EASYWIN_H
#define __EASYWIN_H

#include <string>

#ifndef _INC_WINDOWS
#define  STRICT
#include <windows.h>
#endif

#include <commctrl.h>
#pragma hdrstop

#include <vector>

#include "menu.h"
#include "listobj.h"

#ifdef UNICODE
    #define  rsmemset(p, ch, count)  wmemset(p, ch, count)
#else
    #define  rsmemset(p, ch, count)  memset(p, ch, count)
#endif


#ifndef WC_EDIT
    #define  WC_EDIT            "EDIT"
#endif

#ifndef WC_BUTTON
    #define  WC_BUTTON          "BUTTON"
#endif

#ifndef WC_STATIC
    #define  WC_STATIC          "STATIC"
#endif

#ifndef ICON_SMALL
    #define  ICON_SMALL         0
#endif

#ifndef ICON_BIG
    #define  ICON_BIG           1
#endif

#ifndef WM_MOUSEWHEEL
    #define  WM_MOUSEWHEEL      0x020A
#endif

#ifndef WM_GETOBJECT
    #define  WM_GETOBJECT      0x003D
#endif

#ifndef SB_SIMPLEID
    #define  SB_SIMPLEID        0x00ff   // refers to the data saved for simple mode
#endif

#ifndef SBARS_TOOLTIPS
    #define  SBARS_TOOLTIPS     0x0800
#endif

#ifndef CX_PANE_BORDER
    #define  CX_PANE_BORDER     6        // 3 pixels on each side of each pane
#endif

#ifndef TTS_BALLOON
    #define  TTS_BALLOON        0x40
#endif

#ifndef TTM_SETTITLE
    #define  TTM_SETTITLE      (WM_USER + 32)
#endif

#ifndef GET_X_LPARAM
    #define  GET_X_LPARAM(lParam)  ((int)(short)LOWORD(lParam))
#endif

#ifndef GET_Y_LPARAM
    #define  GET_Y_LPARAM(lParam)  ((int)(short)HIWORD(lParam))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Модуль расширения терминала
#define  EW_TERMEXT         "EWEXTT"

// Таблица сообщений от расширения терминала
#define  EW_MES_TABLE       "EWTBL"

// Размер строки с названием таблицы сообщений
#define  EW_MES_TABLE_SIZE  9

// Индикаторы в статусной строке
#define  EW_FILTR_TEXT      TEXT("FILTR")
#define  EW_INS_TEXT        TEXT("INS")
#define  EW_OVR_TEXT        TEXT("OVR")
#define  EW_NUMLOCK_TEXT    TEXT("NUM")
#define  EW_CAPSLOCK_TEXT   TEXT("CAPS")
#define  EW_SCROLLLOCK_TEXT TEXT("SCRL")

// Размер временного буфера
#define  EW_BUFSIZE              512

// Число цветов в палитре
#define  EW_MAXCOLOR               7

// Число секций в статусной строке
#define  EW_MAXPANE               64

// Символ троетичия
#define  EW_ELLIPSIS_CHAR_A      133
#define  EW_ELLIPSIS_CHAR_W   0x2026

// Максимальная длина названия пункта меню и комментария к нему
#define  EW_MAXEXTEDITOR_NAME     31
#define  EW_MAXEXTEDITOR_COMMENT  81

// Максимальная длина статус-строки (в символах)
#define  EW_MAX_STATUS_LEN        80

// Максимальный размер заголовка и текста уведомления
#define  EW_MAX_BALLOON_HEAD      81
#define  EW_MAX_BALLOON_TEXT     301

// Максимальная длина текста кнопки
#define  EW_MAX_BTTEXT_SIZE      101

// Размер иконки
#define  EW_SMALLICONSIZE         16
#define  EW_LARGEICONSIZE         24

// Размер стандартной кнопки
#define  EW_BUTTON_API_H          23  // Высота

// Флаги для элементов статус-строки (для ew_setStatusText, параметр flags)
#define  EW_SBT_NO                 0  // Используется ширина секции, устаноленная через ew_addStatusPane
#define  EW_SBT_TEXT               1  // Ширина секции вычисляется по ширине текста
#define  EW_SBT_TEXTTIPS           2  // Вывести текст и подсказку к нему
#define  EW_SBT_ICON               4  // Вывести иконку (и подсказку к ней при наличии)
#define  EW_SBT_ALL                (EW_SBT_TEXT | EW_SBT_TEXTTIPS | EW_SBT_ICON)


#ifdef UNICODE
    #define  EW_ELLIPSIS_CHAR  EW_ELLIPSIS_CHAR_W
#else
    #define  EW_ELLIPSIS_CHAR  EW_ELLIPSIS_CHAR_A
#endif


// Уменьшение компоненты цвета (для пометки выделенных записей в скроллинге)
#define  EW_COLOR_DECREASE        50

// Сдвиг индекса цвета
#define  EW_COLOR_SHIFT          100

// Значения цветов по умолчанию
#define  EW_YELLOW          RGB(255, 255, 200)
#define  EW_ROSE            RGB(255, 200, 240)
#define  EW_GREEN           RGB(200, 255, 200)
#define  EW_GRAY            RGB(220, 220, 220)
#define  EW_BLUE            RGB(100, 255, 255)
#define  EW_PURPLE          RGB(220, 200, 255)
#define  EW_DOTLINE         RGB( 90,  90,  90)
#define  EW_RED             RGB(255,   0,   0)


// Дефолтные значения цветов (палитра CGA) для текстовых полей (массив EWGlobalData::staticTextColor)
#define  EW_TEXT_CGA_BLACK            RGB(  0,   0,   0)
#define  EW_TEXT_CGA_BLUE             RGB(  0,   0, 170)
#define  EW_TEXT_CGA_GREEN            RGB(  0, 170,   0)
#define  EW_TEXT_CGA_CYAN             RGB(  0, 170, 170)
#define  EW_TEXT_CGA_RED              RGB(170,   0,   0)
#define  EW_TEXT_CGA_MAGENTA          RGB(170,   0, 170)
#define  EW_TEXT_CGA_BROWN            RGB(170,  85,   0)
#define  EW_TEXT_CGA_LTGRAY           RGB(170, 170, 170)
#define  EW_TEXT_CGA_DKGRAY           RGB( 85,  85,  85)
#define  EW_TEXT_CGA_LTBLUE           RGB( 85,  85, 255)
#define  EW_TEXT_CGA_LTGREEN          RGB( 85, 255,  85)
#define  EW_TEXT_CGA_LTCYAN           RGB( 85, 255, 255)
#define  EW_TEXT_CGA_LTRED            RGB(255,  85,  85)
#define  EW_TEXT_CGA_LTMAGENTA        RGB(255,  85, 255)
#define  EW_TEXT_CGA_YELLOW           RGB(255, 255,  85)
#define  EW_TEXT_CGA_WHITE            RGB(255, 255, 255)


// Дефолтные значения цветов (палитра WEB) для текстовых полей
#define  EW_TEXT_WEB_BLACK            RGB(  0,   0,   0)
#define  EW_TEXT_WEB_BLUE             RGB(  0,   0, 255)
#define  EW_TEXT_WEB_GREEN            RGB(  0, 128,   0)
#define  EW_TEXT_WEB_AQUA             RGB(  0, 255, 255)
#define  EW_TEXT_WEB_RED              RGB(255,   0,   0)
#define  EW_TEXT_WEB_FUCHSIA          RGB(255,   0, 255)
#define  EW_TEXT_WEB_MAROON           RGB(128,   0,   0)
#define  EW_TEXT_WEB_SILVER           RGB(192, 192, 192)
#define  EW_TEXT_WEB_GRAY             RGB(128, 128, 128)
#define  EW_TEXT_WEB_NAVI             RGB(  0,   0, 128)
#define  EW_TEXT_WEB_LIME             RGB(  0, 255,   0)
#define  EW_TEXT_WEB_TEAL             RGB(  0, 128, 128)
#define  EW_TEXT_WEB_OLIVE            RGB(128, 128,   0)
#define  EW_TEXT_WEB_PURPLE           RGB(128,   0, 128)
#define  EW_TEXT_WEB_YELLOW           RGB(255, 255,   0)
#define  EW_TEXT_WEB_WHITE            RGB(255, 255, 255)


// Дефолтные значения цветов (палитра WEB) для фона текстовых полей
#define  EW_BACK_WEB_WHITE            RGB(255, 255, 255)
#define  EW_BACK_WEB_BLUE             RGB(  0, 191, 255)
#define  EW_BACK_WEB_GREEN            RGB(128, 128,   0)
#define  EW_BACK_WEB_CYAN             RGB(  0, 255, 255)
#define  EW_BACK_WEB_RED              RGB(250, 128, 114)
#define  EW_BACK_WEB_MAGENTA          RGB(238, 130, 238)
#define  EW_BACK_WEB_BROWN            RGB(160,  82,  45)
#define  EW_BACK_WEB_LTGRAY           RGB(211, 211, 211)
#define  EW_BACK_WEB_DKGRAY           RGB(128, 128, 128)
#define  EW_BACK_WEB_LTBLUE           RGB(176, 196, 222)
#define  EW_BACK_WEB_LTGREEN          RGB(173, 255,  47)
#define  EW_BACK_WEB_LTCYAN           RGB(224, 255, 255)
#define  EW_BACK_WEB_LTRED            RGB(255, 192, 203)
#define  EW_BACK_WEB_LTMAGENTA        RGB(230, 230, 250)
#define  EW_BACK_WEB_YELLOW           RGB(255, 255, 224)
#define  EW_BACK_WEB_BLACK            RGB(  0,   0,   0)


// Текущая палитра (CGA)
// Цвета символов
#define  EW_TEXT_STYLE_BLACK          EW_TEXT_CGA_BLACK
#define  EW_TEXT_STYLE_BLUE           EW_TEXT_CGA_BLUE
#define  EW_TEXT_STYLE_GREEN          EW_TEXT_CGA_GREEN
#define  EW_TEXT_STYLE_CYAN           EW_TEXT_CGA_CYAN
#define  EW_TEXT_STYLE_RED            EW_TEXT_CGA_RED
#define  EW_TEXT_STYLE_MAGENTA        EW_TEXT_CGA_MAGENTA
#define  EW_TEXT_STYLE_BROWN          EW_TEXT_CGA_BROWN
#define  EW_TEXT_STYLE_LTGRAY         EW_TEXT_CGA_LTGRAY
#define  EW_TEXT_STYLE_DKGRAY         EW_TEXT_CGA_DKGRAY
#define  EW_TEXT_STYLE_LTBLUE         EW_TEXT_CGA_LTBLUE
#define  EW_TEXT_STYLE_LTGREEN        EW_TEXT_CGA_LTGREEN
#define  EW_TEXT_STYLE_LTCYAN         EW_TEXT_CGA_LTCYAN
#define  EW_TEXT_STYLE_LTRED          EW_TEXT_CGA_LTRED
#define  EW_TEXT_STYLE_LTMAGENTA      EW_TEXT_CGA_LTMAGENTA
#define  EW_TEXT_STYLE_YELLOW         EW_TEXT_CGA_YELLOW
#define  EW_TEXT_STYLE_WHITE          EW_TEXT_CGA_WHITE

// Цвета фона
#define  EW_BACK_STYLE_WHITE          EW_BACK_WEB_WHITE
#define  EW_BACK_STYLE_BLUE           EW_BACK_WEB_BLUE
#define  EW_BACK_STYLE_GREEN          EW_BACK_WEB_GREEN
#define  EW_BACK_STYLE_CYAN           EW_BACK_WEB_CYAN
#define  EW_BACK_STYLE_RED            EW_BACK_WEB_RED
#define  EW_BACK_STYLE_MAGENTA        EW_BACK_WEB_MAGENTA
#define  EW_BACK_STYLE_BROWN          EW_BACK_WEB_BROWN
#define  EW_BACK_STYLE_LTGRAY         EW_BACK_WEB_LTGRAY
#define  EW_BACK_STYLE_DKGRAY         EW_BACK_WEB_DKGRAY
#define  EW_BACK_STYLE_LTBLUE         EW_BACK_WEB_LTBLUE
#define  EW_BACK_STYLE_LTGREEN        EW_BACK_WEB_LTGREEN
#define  EW_BACK_STYLE_LTCYAN         EW_BACK_WEB_LTCYAN
#define  EW_BACK_STYLE_LTRED          EW_BACK_WEB_LTRED
#define  EW_BACK_STYLE_LTMAGENTA      EW_BACK_WEB_LTMAGENTA
#define  EW_BACK_STYLE_YELLOW         EW_BACK_WEB_YELLOW
#define  EW_BACK_STYLE_BLACK          EW_BACK_WEB_BLACK

// -----------------------------------------------------------------------------
// Типы шрифтов
#define  EW_TYPE_FONT_TEXT            1
#define  EW_TYPE_FONT_FIELD           2
#define  EW_TYPE_FONT_DISABLE         3
#define  EW_TYPE_FONT_LIST            4
#define  EW_TYPE_FONT_HEADER          5
#define  EW_TYPE_FONT_EDITOR          6
#define  EW_TYPE_FONT_MENU            7

// -----------------------------------------------------------------------------
// Минимальный размер окна редактора
#define  EW_EDITOR_MINSIZE_X   (2 * ew_data->editorCX)
#define  EW_EDITOR_MINSIZE_Y   (2 * ew_data->editorCY)

// -----------------------------------------------------------------------------
// Минимальный размер окна вьювера графических файлов
#define  EW_VIEWER_MINSIZE_W   400
#define  EW_VIEWER_MINSIZE_H   300

// Стили диалога
#define  EW_CHILD           0x00000001
#define  EW_POPUP           0x00000002
#define  EW_NOTITLE         0x00000004
#define  EW_NOCLOSE         0x00000008
#define  EW_BORDER          0x00000010
#define  EW_NOTITLETC       0x00000100  // Без заголовка для TabControl (совместно с EW_NOTITLE)
#define  EW_CENTERED        0x00001000  // Диалог должен быть отцентрован по главному окну приложения
#define  EW_FIXED           0x00010000

// Стили скроллинга
#define  EW_AUTOHEAD        0x00000001
#define  EW_HORLINES        0x00000002
#define  EW_VERLINES        0x00000004
#define  EW_DOTLINES        0x00000008
#define  EW_AUTOFILL        0x00000010
#define  EW_MAXIMIZED       0x00000020
#define  EW_ELLIPSIS        0x10000000
#define  EW_MULTIHEAD       0x00000040
//#define EW_

// Стили выравнивания
#define  EW_LEFTJUST        0x00000000
#define  EW_RIGHTJUST       0x00000001
#define  EW_CENTERJUST      0x00000002

#define  EW_JUSTSTYLE       (EW_LEFTJUST | EW_RIGHTJUST | EW_CENTERJUST)

// Places text on the left side of the radio button or check box.
#define  EW_LEFTTEXT        0x00000004
// Positions a radio button's circle or a check box's square
// on the right side of the button rectangle.
#define  EW_RIGHTBUTTON     EW_LEFTTEXT

#define  EW_SCROL_FIELD     0x00000010

// Типы обрамления
#define  EW_DOUBLE_EDGE     2
#define  EW_SINGLE_EDGE     1
#define  EW_EMPTY_EDGE      0

//
#define  EW_APPDATA_MAGIC      0x0A12B9E3
#define  EW_APPDATA_NAME       "EasyWin Application Data File."
#define  EW_APPDATA_COPYRIGHT  "Copyright (c) 2001-2019 R-Style Softlab."

// Данные для фрэйма
#define  EW_FRAME_DATA      0x00000001

// ew_addEvent target:
// 0 - send to the server
// 1 - leave on terminal if capture is on
// 2 - leave on terminal
#define  LOCAL_TARGET       2
#define  REMOTE_TARGET      0
#define  CAPTURE_TARGET     1

// Пустое (не загруженное меню)
#define  EW_EMPTY_MENU      ((void *)-1)

// -----------------------------------------------------------------------------
// Флаги для раздельной обработки правых и левых shift-клавиш
#define  EW_SPLIT_LEFT_RIGHT_ALT     1
#define  EW_SPLIT_LEFT_RIGHT_SHIFT   2
#define  EW_SPLIT_LEFT_RIGHT_CTRL    4

// -----------------------------------------------------------------------------
// Команды контекстного меню секции уведомлений
#define  dCMD_TT_SHOW_HISTORY     100
#define  dCMD_TT_CLEAR_HISTORY    101
#define  dCMD_TT_USE_HISTORY      102
#define  dCMD_TT_CLEAR_STATE      110
#define  dCMD_TT_SILENT_MODE      120

// Подкоманды для секции уведомлений

// Для dCMD_TT_
#define  dCMD_TT_VALUE_ON           1
#define  dCMD_TT_VALUE_OFF          2   // Блокируем двойной клик и контекстное меню (ибо рекурсия)


// -----------------------------------------------
// Параметры для идентификаторов иконок
#define  dEW_ToolRangeBegin     3000   // Начало диапазона инструментальных иконок
#define  dEW_ToolRangeEnd       4999   // Окончание диапазона инструментальных иконок

#define  dEW_UserRangeBegin     7000   // Начало диапазона пользовательских иконок
#define  dEW_UserRangeEnd       8999   // Окончание диапазона пользовательских иконок

#define  dEW_IconShift24        2000   // Сдвиг идентификатора для иконок размера 24х24

// -----------------------------------------------------------------------------
// Значения для параметра NOGRIDSCROL
#define  dEW_HideScrolLineH        1   // Не рисовать горизонтальную сетку
#define  dEW_HideScrolLineV        2   // Не рисовать вертикальную сетку


// -----------------------------------------------------------------------------
// Значения для параметра ENABLEBUTTONICON
#define  dEW_ButtonIconNone        0   // Не отображать картинки на кнопках
#define  dEW_ButtonIconSmall       1   // Всегда отображать мальнькие картинки
#define  dEW_ButtonIconAuto        2   // Отображать для мальньких кнопок маленькие картинки,
                                       // а для больших - большие


// -----------------------------------------------------------------------------
// Идентификаторы типов ообъектов (поле otype)

#define  EW_TYPEOBJ_MENU         100
#define  EW_TYPEOBJ_FRAME        101
#define  EW_TYPEOBJ_LINE         102
#define  EW_TYPEOBJ_TEXT         103
#define  EW_TYPEOBJ_BOX          104
#define  EW_TYPEOBJ_FIELD        105
#define  EW_TYPEOBJ_COLUMN       106
#define  EW_TYPEOBJ_CEIL         107
#define  EW_TYPEOBJ_SCROL        108
#define  EW_TYPEOBJ_EDITOR       109
#define  EW_TYPEOBJ_TAB          110
#define  EW_TYPEOBJ_POPUP        111
#define  EW_TYPEOBJ_DLG          112
#define  EW_TYPEOBJ_WND          113
#define  EW_TYPEOBJ_BALLOON      114
#define  EW_TYPEOBJ_STATUSBAR    115


// -----------------------------------------------------------------------------
// Указатель на функцию-шлюз отправки сообщений
typedef  int (*TFMsgGate)(int msg, void *data, void *add);

// -----------------------------------------------
// Идентикаторы объектов в списке окон
enum
   {
    DLGOBJECT_ID,
    POPUPOBJECT_ID,
    WINDOWOBJECT_ID
   };

// -----------------------------------------------
// Идентификаторы контролов в диалогах
enum
   {
    // Control base-identifiers
    ID_BOX      = 0x0100,
    ID_TEXT     = 0x0200,
    ID_FIELD    = 0x0300,

    ID_DLGBTN   = 0x0400,
    ID_DLGTEXT  = 0x0500,

    ID_FRAMEWND = 0x0600,
    ID_REBARWND,
    ID_CLIENTWND,
    ID_MENUBAR,
    ID_STATUSBAR,
    ID_TOOLBAR,

    ID_POPUPWND = 0x0700,

    ID_PANEL    = 0x0800,
    ID_SCROLL   = 0x0900,
    ID_HEADER   = 0x0A00,
    ID_EDITOR   = 0x0B00,
    ID_TABCTRL  = 0x0C00,
    ID_TOOLTIP  = 0x0D00,
    ID_BALLOON  = 0x0E00,

    // Timer identifier
    ID_INDICTIMER = 0x0100,
    ID_MOUSETIMER,
    ID_FOCUSTIMER,
   };

// -----------------------------------------------
// Идентификаторы цветов в палитре
enum
   {
    MES_COLOR = 0,
    RMES_COLOR,
    HLP_COLOR,
    MENU_COLOR,
    COM_COLOR,
    SCRL_COLOR,
    BACK_COLOR,

    USER_COLOR
   };

// -----------------------------------------------
enum ColorTypes
   {
    NormalColor   = 0x11,
    SelectedColor = 0x12,
    ErrorColor    = 0x13,

    KeyColor      = 0xF1,
    TypeColor     = 0xF2,
    StdFuncColor  = 0xF3,
    NumericColor  = 0xF4,
    OperatorColor = 0xF5,
    StringColor   = 0xF6,
    SpecVarColor  = 0xF7,
    CommentColor  = 0xF8
   };

#include "packpsh1.h"

// -----------------------------------------------
class EwAccessibleObj;

// -----------------------------------------------
typedef struct
      {
       int32          type;
       uint32         size;
      } APPDATA_ITEM;

// -----------------------------------------------
typedef struct
      {
       int32          type;
       uint32         size;

       UINT           flags;
       UINT           showCmd;
       RECT           rcNormal;

       char           reserv[480];
      } FRAMEDATA_ITEM;

// -----------------------------------------------
typedef struct
      {
       int32          magic;
       char           name[31];
       char           copyright[41];

       char           reserv[180];
      } APPDATA_HEADER;

#include "packpop.h"


// -----------------------------------------------
typedef struct MENUOBJ
      {
       int            otype;            // EW_TYPEOBJ_MENU

       MENU          *pMenu;
       int            nMenu;
       bool           popup;
       int            x, y;

       HMENU          hMenu;
       HMENU          hPreviousMenu;
       struct MENUOBJ *previousMenu;

       BOOL           fUseMenu;
       BOOL           fLoopMenu;
       BOOL           fEnableExit;
       BOOL           fExitMenuLoop;

       PANEL         *pnl;
       void          *psave;

       void          *mapExt;           // Контейнер типа map для расширений редактора (CDataItem)
       BOOL           fHasExt;          // Признак наличия расширений редактора
       int            itemId;           // Идентификатор пункта меню
      } MENUOBJ;

    
// -----------------------------------------------
typedef struct BALLOONOBJ
      {
       int            otype;            // EW_TYPEOBJ_BALLOON

       TEXT           head[EW_MAX_BALLOON_HEAD];  // Заголовок уведомления
       TEXT           text[EW_MAX_BALLOON_TEXT];  // Текст уведомления

       int            timeOut;          // Время (мс) отображения уведомления

       HWND           hWnd;             // Дескриптор окна уведомления
       HWND           hStatusBar;       // Дескриптор окна статус-строки

       HIMAGELIST     hImageList;       // Список картинок
       CNTLIST        lstIcons;         // Список идентификаторов иконок
       int            iIcon;            // Номер текущей иконки в списке hImageList (м.б -1 если без иконки)

       bool           flSilentMode;     // Не показывать уведомления
       bool           flHistory;        // Использовать/не использовать историю
       bool           flBlock;          // Блокировка двойного клика и контекстного меню
      } BALLOONOBJ;
      

// -----------------------------------------------
// Указатель на оконную процедуру
typedef LRESULT (CALLBACK *TFWndProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// -----------------------------------------------------------------------------
// Структура для ручной отрисовки ячеек StatusBar'а
typedef struct
      {
       TCHAR    text[EW_MAX_STATUS_LEN],  // Текст элемента
                tips[EW_MAX_STATUS_LEN];  // Текст всплывающей подсказки
       int      id;                       // Идентификатор ячейки
       int      pane;                     // Индекс ячейки
       int      flags;                    // Флаги для ячейки (EW_SBT_*)
       HBITMAP  hIcon;                    // Хендл выводимой картинки
      } EWStatusItem;

// -----------------------------------------------
typedef struct STATUSBAROBJ
      {
       int           otype;               // EW_TYPEOBJ_STATUSBAR

       bool          bSizeGrip;           // Признак наличия у статус-бара "уголка" для изменения размера
       bool          bHasFilter;          // Наличие признака фильтрации

       int           npanes;              // Колическтво секций в статус-баре (размеры массивов id/cx/item)
       short         id[EW_MAXPANE];      // Массив идентификаторов секций
       short         cx[EW_MAXPANE];      // Массив ширин секций
       EWStatusItem  item[EW_MAXPANE];    // Массив данных по секциям

       CNTLIST       lstIcons;            // Список иконок для статус-бара

       HWND          hToolTips;           // Контрол для всплывающих подсказок

       TFWndProc     pWndProc;            // Указатель на "родную" оконную процедуру статус-бара

       BALLOONOBJ   *balloon;             // Данные уведомлений
      } STATUSBAROBJ;


// -----------------------------------------------
class CCustomizeToolBar;

// -----------------------------------------------
typedef struct
      {
       int            otype;              // EW_TYPEOBJ_FRAME

       int            numcols;
       int            numrows;

       bool           showFrame;
       bool           blockOpen;
       bool           lostForeground;
       bool           restMaximized;

       HWND           hFrameWnd;
       HWND           hClientWnd;
       HWND           hParentWnd;

       HWND           hMenuBar;
       HWND           hRebarWnd;
       HWND           hStatusBar;
       HWND           hToolBar;           // Дескриптор ToolBar'а
       HIMAGELIST     hImageListTB;       // Список картинок для ToolBar'а
CCustomizeToolBar*    pCustomizeTB;

       HWND           hOldMainWnd;

       MENUOBJ       *menu;
       void          *psave;

       BOOL           fPostResize;
       BOOL           fParentDisabled;

       void          *overlappedWnd;

       BOOL           prevToolBarBusy;   // Предыдущее состояние ToolBar'а
      } FRAMEOBJ;

// -----------------------------------------------
typedef struct
      {
       char     fldType;
       int      fldTypeF;                // аналог DLGFIELD::typeF
       unsigned fldLength;
       int      fldPoint;

       bool     oemFont;

       char    *minVal;
       char    *maxVal;

       int    (*userProc)(const char *, void *);
       void    *userParm;
      } VLDOBJ;

// -----------------------------------------------
typedef struct
      {
       void  *fmtedit;
      } FMTOBJ;

// -----------------------------------------------
typedef struct
      {
       int            x;
       int            y;
       int            cx;
       int            cy;
      } DLGCOORD;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_LINE

       int            x1;
       int            y1;
       int            x2;
       int            y2;

       DLGCOORD       dlg;
      } DLGLINE;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_TEXT

       int            x;
       int            y;
       char          *value;
       int            align;
       int            format;
       int            color;

       DLGCOORD       dlg;
      } DLGTEXT;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_BOX

       int            x;
       int            y;
       int            l;
       int            h;
       char          *text;

       DLGCOORD       dlg;
      } DLGBOX;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_FIELD

       FIELD         *fld;

       unsigned char  Ftype;            // Тип поля
       unsigned int   St;               // Стиль изображения поля
       unsigned char  FVt;              // Тип значения

       int            x;
       int            l;

       unsigned char  typeF;
       unsigned long  FFlags;

       bool           enable;
       bool           blocked;
       bool           oemFont;

       char          *text;
       int            style;            // Выравнивание и т.п.
       int            color,            // Стиль поля (влияет на фон)
                      colorText,        // Цвет текста поля (точнее индекс цвета)
                      colorBack;        // Цвет фона поля
       int            format;           // Формат (B/I/U). Пока не используется, но чтобы "два раза не вставать"
       int            focusState;

       HWND           hWnd;
       WNDPROC        wndProc;

       VLDOBJ        *vldobj;
       FMTOBJ        *fmtobj;

       DLGCOORD       dlg;
       int            IndOffset;

       EwAccessibleObj *acc;

       unsigned int   St2;              // Логическое продолжение St

       TCHAR         *tooltipStr;       // Всплывающая подсказка к полю (динамический буфер)

       int            iconID;           // Идентификатор иконки, выводимой в поле
       int            iconFlag;         // Флаги к выводу иконки (dICON_ALIGN_)

       int            iconIndex;        // Индекс иконки в списке картинок (DLGOBJ::hImageListFld)
      } DLGFIELD;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_COLUMN

       FIELD         *fld;

       unsigned int   St;
       unsigned char  Ftype;
       int            x;
       int            l;

       unsigned char  typeF;
       unsigned long  FFlags;
       unsigned char  fieldNum;

       bool           selected;
       int            style;

       int            xColumn;          // Правая граница колонки
       int            xColumnSrc;       // Исходное значение xColumn (для восстановления)
       int            xTreePos;

       int            lMargin;
       int            rMargin;
       int            nMinCX;           // Минимальная ширина колонки
       int            nWidth;           // Оригинальная ширина колонки (используется при восстановлении
                                        // вида по-умолчанию.
       DLGCOORD       dlg;

       unsigned int   St2;              // Логическое продолжение St
      } DLGCOLUMN;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_CEIL

       int            x;
       int            l;

       unsigned char  typeF;
       unsigned long  FFlags;

       short          needOffs;

       int            align;
       int            color,            // Стиль поля (влияет на фон)
                      colorText,        // Цвет текста поля (точнее индекс цвета)
                      colorBack;        // Цвет фона поля
       int            format;           // Формат (B/I/U). Пока не используется, но чтобы "два раза не вставать"

       char          *text;
       char          *tree;

       int            mark1;
       int            mark2;

       bool           IsTrunc;

       int            iconID;           // Идентификатор иконки, выводимой в поле
       int            iconFlag;         // Флаги к выводу иконки (пока не используется)

       bool           IsMark;           // Признак отмеченной записи в скроллинге
      } DLGCEIL;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_SCROL

       int               x;
       int               y;
       int               l;
       int               h;             // высота одной строки в скроллинге

       int               ncolumns;
       int               nrows;

       DLGCOLUMN        *columns;
       DLGCEIL         **ceilPtr;

       int               xColumn;
       int               focusState;

       int               curItem;
       int               curCeil;

       int               numRows;
       int               numCols;
       int               numRec;

       const int        *firstRecPtr;   // уазатель на номер первой записи в скроллинге
       const int        *numRecPtr;     // уазатель на общее число записей в скроллинге

       int               style;
       bool              bRunState;

       HWND              hEditWnd;
       HWND              hScrollWnd;
       HWND              hHeaderWnd;
       HWND              hToolTips;
       HWND              hHeaderToolTips;
       TCHAR            *lpTip;
       int               TTLastHitRow;
       int               TTLastHitCol;

       HFONT             hScrollFont;
       HFONT             hHeaderFont;

       DLGCOORD          dlg;

       size_t            parSize;
       void             *parBuf;

       bool              isPartRow;
       bool              isViewMode;

       EwAccessibleObj  *acc;

       void             *ewHeader;  // Указатель на объект типа std:vector

       int               old_y;     // BSCROL::old_y
       int               old_n;     // BSCROL::old_Mn (DLGSCROL::nrows)
      } DLGSCROL;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_EDITOR

       int            x;
       int            y;
       int            l;
       int            h;
       int            flag;

       TCHAR        **buffer;
       void          *markAreas;

       int            caretX;
       int            caretY;
       int            caretType;
       int            focusState;

       bool           bRunState;

       HWND           hWnd;
       HFONT          hFont;
       HBITMAP        hBitmap;
       HRGN           hInvalidRgn;

       DLGCOORD       dlg;

       unsigned char  **attrArray;

       bool           isImager;         // Признак того, что редактор используется для просмотра картинок
       void          *imgData;          // Указатель на структуру EWPictureData
      } DLGEDITOR;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_TAB

       int            x;
       int            y;
       int            l;
       int            h;
       char          *tabs;

       HWND           hWnd;
       HFONT          hFont;

       DLGCOORD       dlg;

       // #3359
       bool           canResize;        // TabCtrl может менять размер

       int            deltaX,           // Последнее изменение TabCtrl'а в пикселях
                      deltaY;

       RECT           rcDialogOld,      // Старый размер страницы (диалога)
                      rcElemOld;        // Старый размер элемента страницы (скроллинга)

       // #3463
       int            style;
      } DLGTAB;

// -----------------------------------------------
typedef struct POPUPOBJ
      {
       int            otype;            // EW_TYPEOBJ_POPUP

       char          *text;
       char          *btext;
       char          *status;
       char          *caption;

       int            type;
       int            style;
       int            curfld;
       int            length;

       int            help;
       bool           enableClosing;

       int            ntexts;
       int            nbuttons;

       void          *psave;

       HWND           hPopupWnd;
       HWND           hToolTip;

       HWND           hFrameWnd;
       HWND           hParentWnd;

       HFONT          hFont;
       POINT          pt;

       DLGCOORD       dlg;
       DLGCOORD       textDlg;
       DLGCOORD      *buttonDlg;

       void          *webCrtl;

       EwAccessibleObj  *acc;
       long              errCode;       // ercode  cwinEx
       int               infoType;      // message, warning, error, fatal.

       BOOL           prevToolBarState;  // Предыдущий статус ToolBar'а заблокирован/разблокирован (для корректного восстановления)
      } POPUPOBJ;

// -----------------------------------------------
typedef  POPUPOBJ DLGPOPUP;

// -----------------------------------------------
typedef struct DLGOBJ
      {
       int            otype;            // EW_TYPEOBJ_DLG

       PANEL         *pnl;

       struct DLGOBJ *parent;
       struct DLGOBJ *oldUseObject;

       LOBJ           _lobj;            // элемент списка дочерних панелей
       CNTLIST        child_list;       // список дочерних панелей

       int            nfields;
       int            nlines;
       int            ntexts;
       int            nboxs;

       DLGTAB        *tab;
       DLGSCROL      *scrol;
       DLGEDITOR     *editor;

       DLGFIELD      *fields;
       DLGLINE       *lines;
       DLGTEXT       *texts;
       DLGBOX        *boxs;

       char          *values;
       int            style;

       POPUPOBJ      *popup;

       HWND           hWnd;
       HWND           hParentWnd;
       HWND           hStatusBar;
       HWND           hToolTips;

       HWND           hFocusWnd;

       HFONT          hTextFont;
       HFONT          hFieldFont;
       HFONT          hDisableFont;

       DLGCOORD       dlg;

       EwAccessibleObj  *acc;

       BOOL           prevToolBarState; // Предыдущий статус ToolBar'а заблокирован/разблокирован (для корректного восстановления)
       BOOL           hasMenu;          // #3392

       HIMAGELIST     hImageListFld;    // Список картинок к полям

       BOOL           fSizeCmd;         // Нажата кнопка MAXIMIZE/RESTORE
      } DLGOBJ;

// -----------------------------------------------
typedef struct
      {
       int            otype;            // EW_TYPEOBJ_WND

       HWND           hWnd;
       LOBJ           _lobj;

       void          *object;
       int            objectID;

       void          *overlappedWnd;
       bool           externalObject;
      } WNDOBJ;

// -----------------------------------------------
typedef struct
      {
       DLGOBJ        *dlgobj;
      } ew_ExtPan;

// -----------------------------------------------
typedef struct
      {
       FRAMEOBJ      *frame;

       WNDOBJ        *topWindow;

       DLGOBJ        *useObject;

       BOOL           fAutoDelete;
       BOOL           fRemoteCapture;

       LOBJ           _lobj;
       CNTLIST        wnd_list;
      } EWContext;

// -----------------------------------------------
typedef struct
      {
       int            version;          // Версия интерфейса

       TFMsgGate      SendMessageGate;  // Указатель на функцию-шлюз для отправки сообщений
      } TSMessageGate;

// -----------------------------------------------------------------------------
// Структура для хранения параметров шрифта для полей (для стилей B/I/U)
typedef struct
      {
       LOBJ           lobj;             // Техническая структура для списка

       int            type;             // Тип шрифта (EW_TYPE_FONT_*)
       int            format;           // Ключ для поиска (флаги B/I/U)

       LOGFONT        font;             // Данные шрифта
       int            cx, cy;           // Размеры максимального знакоместа данного шрифта
       HFONT          hFont;            // Дескриптор шрифта
      } TSFontData;

// -----------------------------------------------
typedef struct
      {
       void       *appIntf;

       char       *appid;
       char       *system;
       char       *version;
       char       *copyright;

       HINSTANCE   hInstance;
       int         nShowCmd;
       LPTSTR      szCmdLine;

       char        szFrameIcon[_MAX_PATH];
       HICON       hFrameIcon;
       HICON       hSmallFrameIcon;

       char       *szRegClass;

       int         edgeType;
       bool        edgeForDisable;
       bool        edgeForReadOnly;
       int         editorThumbMode;
       int         menuLoadingMode;

       BOOL        bOldWindows;

       BOOL        fSizeMoveLoop;
       BOOL        fScrollBarLoop;
       BOOL        fOverWriteMode;
       BOOL        fEditorOvrMode;

       LOGFONT     lfTextFont;           // Шрифт для текстовых полей
       CNTLIST     lstFormatFont;        // Список шрифтов для стилей B/I/U/S


       LOGFONT     lfFieldFont;
       LOGFONT     lfDisableFont;

       LOGFONT     lfScrollFont;
       LOGFONT     lfHeaderFont;

       LOGFONT     lfEditorFont;

       LOGFONT     lfMenuFont;

       bool        fOemTextFont;
       bool        fOemFieldFont;
       bool        fOemDisableFont;

       bool        fOemScrollFont;
       bool        fOemHeaderFont;

       bool        fOemEditorFont;

       int         textCX, textCY;
       int         fieldCX, fieldCY;
       int         editorCX, editorCY;
       int         listCX;

       int         coordX, coordY;              // Размеры знакоместа (STEPX, STEPY)

       int         virtualScreenW,              // Размер виртуального экрана (для диалогов)
                   virtualScreenH;

       HPALETTE    hPalette;
       COLORREF    nColorRef[EW_MAXCOLOR],
                   nColorSel[EW_MAXCOLOR];      // Цвета для пометки выделенных записей

       HBRUSH      hBkBrush[EW_MAXCOLOR];

       COLORREF    editTextClr[256];
       COLORREF    editBkClr[256];

       COLORREF    textColor[dTEXT_MAX_COLOR];  // Палитра для полей (цвет текста)
       COLORREF    backColor[dBACK_MAX_COLOR];  // Палитра для полей (цвет фона)

       COLORREF    nColorSel2[dBACK_MAX_COLOR];
       HBRUSH      hBkBrush2[dBACK_MAX_COLOR];

       CNTLIST     context_list;

       PANEL      *pInd;
       void       *pLock;                       // Указатель на объект класса CLockUserInputWnd

       HIMAGELIST  himl;

       HHOOK       hCallWndProcHook;
       HHOOK       hCallWndProcRetHook;

       void       *appInfo;
       void       *appData;
       size_t      appSize;

       // ----------------------------------------
       // Параметры успользуемого СП
       int         hostPort;
       char       *hostName,
                  *pipeName;
       // ----------------------------------------

       void       *wndMgr;
       void       *extWndMgr;
       bool        ownWndMgr;

       bool        beepMode;

       bool        noRecreateCtrl;

       TSMessageGate  msgGate;           // Шлюз для сообщений

       // AV 17.03.2011 picure for background main window
       char        szBackPicture[_MAX_PATH];

       int         splitLeftRightKeys;   // Разделять левые и правые shift-клавиши

       // #3061 - Перекраска признака фильтрации по желанию пользователя
       int         selectStatusFilter;   // Активация режима перекраски
       COLORREF    colorStatusFilterBG,  // Цвет фона
                   colorStatusFilterFG;  // Цвет символа

       BOOL        flStatusUpdate;       // Признак изменения статусов INS/OVR, CAPS и т.п.
       BOOL        flForceStatusUpdate;  // Признак принудительного обновления статусов INS/OVR, CAPS и т.п.
       BOOL        flIsBusy;             // Признак занятости системы (устанавливается/сбрасывается CMD_EW_BUSY)

       int         toolBarMode;          // Режим работы ToolBar'а (пока 0 или 1)
       int         toolBarCustom;        // Режим настройки ToolBar'a (0 или 1)
       int         toolBarImageSize;     // Размер картинок на ToolBar'е (0 - 16х16, 1 - 24х24)
       int         toolBarShowErrIcons;  // Режим отображения проблемных картинок на ToolBar'е.

       COLORREF    crTransparentBmp,     // "Прозрачный" цвет в картинках
                   crGrayedText,         // Цвет неактивного текста в меню
                   crSelText,            // Цвет текста в меню под курсором
                   crMenuBkgnd,          // Цвет фона в меню
                   crSelBkgnd;           // Цвет фона в меню под курсором

       char        szDirResFile[_MAX_PATH],  // Каталог ресурсов
                   szToolResDll[_MAX_PATH],  // Инструментальная библиотека ресурсов
                   szUserResDll[_MAX_PATH];  // Пользовательская библиотека ресурсов
                   

       HMODULE     hToolResDll,          // Хендл инструментальной библиотеки ресурсов
                   hUserResDll;          // Хендл пользовательской библиотеки ресурсов

       HGDIOBJ     hMenuFont;            // Хендл системного шрифта для меню
       bool        flUseUserMenuFont;    // Использовать для меню пользовательский шрифт (см. lfMenuFont)

       int         cmdll;                    // Режим работы (локаль/трехзвенка)
       char        szEditorExt[_MAX_PATH];   // Каталог расширений редактора
       char        szTxtFile[_MAX_PATH];     // Каталог TxtFile

       int         newFeatureMode;       // Режим нового интерфейса
       int         noGridScrol;          // Не рисовать сетку в скроллингах (см. dEW_HideScrolLine*)
       int         flInputAsPic;         // Вместо символа 'R' в статус-строке выводить картинку
       int         flWorkModeAsPic;      // Для отображения в статус-строке режима работы выводить картинку
       int         enableScrolIcon;      // Разрешить отображение иконок в скроллингах
       int         enableTextFormat;     // Включить поддержку цвета/формата для текстовых полей
       int         enableTTDialog;       // Включить всплывающие подсказки к полям диалога

       int         treeIconClose,        // Идентификаторы иконок (вместо плюсиков) для
                   treeIconOpen;         // иерархического скроллинга.

       int         lockInputMode;        // Режим блокировки окна приложения

       int         enableButtonIcon;     // Разрешить отображение иконок на кнопках
       int         useAPIButtonH;        // Использовать стандартную высоту кнопок

       int         enableSysButton;      // Разрешить использовать кнопку максимизации для скроллингов

       int         workGraphCharMode;    // Режим обработки псевдографики
      } EWGlobalData;

// -----------------------------------------------------------------------------
typedef struct
      {
       int   selB;
       int   selE;
      } EWMarkArea;


// -----------------------------------------------------------------------------
// Флаги вывода наименования меню
#define  dEWMD_None     -1   // Используется в качестве инициализирующего значения для EWMenuData::flagsTextSave
#define  dEWMD_OutAll    0   // Вывести полное наименование
#define  dEWMD_OutLeft   1   // Вывести левую часть
#define  dEWMD_OutRight  2   // Вывести правую часть

// -----------------------------------------------------------------------------
// Данные для MENU::ewdata. Служит для оптимизации алгоритмов отрисовки меню
// с иконками и тулбаров.
typedef struct
      {
       char  *Text;             // Полное название пункта меню в обработанном для GUI формате
       char  *TextL,            // Левая часть названия меню (наименование)
             *TextR;            // Правая часть названия меню (клавиатурная комбинация)
       int    flagsText,        // Какая часть меню дожна выводится (см. dEWMD_)
              flagsTextSave;    // Предыдущее значение flagsText. По-умолчанию инициализируется dEWMD_None.
      } EWMenuData;


// -----------------------------------------------------------------------------
// Данные для поля с картинкой
typedef struct
      {
       int       size;              // Размер файла с картинкой
       int       classID;           // Идентификатор класса объекта
       int       countList;         // Количество объектов в списке
       int       countFrame;        // Количество кадров в файле
       int       countMax;          // Максимальное число изображений в списке
       int       sizeMax;           // Максимальный размер изображения
       int       currPos;           // Номер текущего объекта
       bool      readOnly;          // Режим "Только для чтения"
       TCHAR     name[_MAX_PATH];   // Исходное название файла (с расширением)
       HGLOBAL   hMem;              // Дескриптор памяти, связанной с картинкой
       void     *oImage;            // Указатель на объект Gdiplus::Image
       bool      externalViewer;    // Просмотр внешней программой
       // ----- Данные из клипбоарда
       void     *cbData;            // Указатель на данные
       int       cbSize;            // Размер данных
      } EWPictureData;

// -----------------------------------------------------------------------------
extern EWGlobalData  *ew_data;

// -----------------------------------------------------------------------------
// Global data
bool  ew_initData(void *appIntf, HMODULE hModule);
void  ew_cleanupData(void);

// -----------------------------------------------------------------------------
// ATL module
void  ew_initAtlModule(HINSTANCE hInstance);
void  ew_doneAtlModule(void);

// -----------------------------------------------------------------------------
// Context
bool  ew_initContextList(void);
void  ew_doneContextList(void);

EWContext *ew_createContext(void);
void       ew_deleteContext(EWContext *context);

void       ew_setActiveContext(EWContext *context);
EWContext *ew_findContextForMenu(MENUOBJ *menu);
EWContext *ew_findContextForFrame(FRAMEOBJ *frame);
EWContext *ew_findContextForFrameWnd(void *overlappedWnd);
EWContext *ew_findContextWithFrame(void);
EWContext *ew_findContextForObject(void *object, int objectID);

void  ew_addToContext(EWContext *context, WNDOBJ *wndobj);
void  ew_dropFromContext(EWContext *context, WNDOBJ *wndobj);

void *ew_getTopObject(EWContext *context, int objectID);

void  ew_activateContext(EWContext *context, bool activate, bool focus);
void  ew_sendToAllWindow(EWContext *context, int iMessage, WPARAM wParam, LPARAM lParam);
void  ew_updateContextLayout(EWContext *context);

// -----------------------------------------------------------------------------
// WndObj
WNDOBJ *ew_addWndObj(HWND hWnd, void *object, int objectID);
WNDOBJ *ew_findWndObj(void *object, int objectID, EWContext **pContext);
bool    ew_dropWndObj(void *object, int objectID);

WNDOBJ *ew_addExternalWndObj(HWND hWnd, void *object, int objectID, void *overlappedWnd);
bool    ew_dropExternalWndObj(EWContext *context, WNDOBJ *wndobj);

WNDOBJ *ew_findOverlappedWnd(void *overlappedWnd, EWContext **pContext);

void  ew_activateWndObj(WNDOBJ *wndobj);
void  ew_deactivateWndObj(WNDOBJ *wndobj);

// -----------------------------------------------------------------------------
// Window manager
bool  ew_initWndManager(void);
void  ew_doneWndManager(void);

bool  ew_canCloseExtWndManager(void);
void  ew_destroyExtWndManager(void);

void  ew_addToWndManager(WNDOBJ *wndobj, bool disableAllFlag);
void  ew_dropFromWndManager(WNDOBJ *wndobj, bool enableAllFlag);

void  ew_addFrameToWndManager(FRAMEOBJ *frame);
void  ew_dropFrameFromWndManager(FRAMEOBJ *frame);

void  ew_addContextToWndManager(EWContext *context);
void  ew_dropContextFromWndManager(EWContext *context);

void  ew_activateOverlapped(void *overlapped);
void  ew_onActivateOverlapped(void *overlapped);

// -----------------------------------------------------------------------------
// Windows hook
void  ew_initWindowsHook(void);
void  ew_doneWindowsHook(void);
HWND  ew_getSysLoopWnd(void);

// -----------------------------------------------------------------------------
// Window class
int   ew_regWndClass(WNDCLASS *);
int   ew_regWndClassEx(WNDCLASSEX *);
void  ew_doneWndClass(void);

// Icon
HICON ew_loadIcon(const char *szIconName, int cx, int cy, unsigned opt);

// Font support
bool  ew_initFont (void);
void  ew_initFontMetrics (void);
int   ew_parseFontString (LOGFONT *lf, char *str);
HFONT ew_createFont (LOGFONT *);

// -----------------------------------------------------------------------------
// Получить параметры шрифта для поля требуемого формата
// Если таких параметров нет, они будут созданы
// [in] type   - Тип шрифта (EW_TYPE_FONT_*)
// [in] format - Формат шрифта (dTEXT_FORMAT_*)
// [in] parent - Указатель на структуру для заимствования шрифта
TSFontData *ew_getFont(int type, int format, const LOGFONT *parent);

// -----------------------------------------------------------------------------
// Color support
bool  ew_initColor (void);
void  ew_doneColor (void);
int   ew_parseColorString (COLORREF *cr, char *str);
void  ew_initColorObjects (void);

// -----------------------------------------------------------------------------
// Events, session
int   ew_addEvent(TFSEvent *ev, int target);
int   ew_getEvent(TFSEvent *ev, int waitTime);
void  ew_clearAllEvents(void);
void  ew_stopSession(bool doAbort);


// -----------------------------------------------------------------------------
#define  ew_canSetFocus()       (!ew_context->frame || ::GetActiveWindow())
#define  ew_context             ((EWContext *)((char*)ew_data->context_list.last->next - ew_data->context_list.delta))
#define  ew_getTopDlgObject()   ((DLGOBJ *)(ew_getTopObject(ew_context, DLGOBJECT_ID)))


HWND  ew_getTopWnd(void);
HWND  ew_getTopFrameWnd(void);

DLGOBJ *ew_getObjectFromWnd(HWND);

bool  ew_isTopObject(void *object, int objectID);

int   ew_messageBox(const char *text, const char *caption, unsigned style);
void  ew_errorBox(DWORD dwErrorCode, const char *fmt, ...);

// -----------------------------------------------------------------------------
// Отцентрировать окно
bool ew_centerWindow(HWND hWnd, HWND hWndCenter, bool centrX, bool centerY, bool asText);

// Активизировать окно
void ew_setActiveWindow(HWND hWnd, bool bActivate);

// Установить фокус
bool ew_setFocus(HWND hWnd);

// Обертки для API-функций работы с фокусом (больше для удобства трассировки)
HWND ew_SetFocus(HWND hWnd);
HWND ew_GetFocus(void);

// Обновить стили окна
void ew_changeWindowStyleEx(HWND hWnd, DWORD dwStyle, DWORD dwExStyle);

// Вычислить размеры окна по размерам его клиентской области
bool ew_setWindowSize(HWND, RECT *, int flags);

// Изменить позицию окна
bool ew_changeWindowPos(HWND, int deltaX, int deltaY, int flags);

// Изменить размеры окна
bool ew_changeWindowSize(HWND, int deltaX, int deltaY, int flags);

// Добавить в строку многоточие
TCHAR *ew_makeEndEllipsis    (HDC hDC, TCHAR *text, int cx, int *plen, TCHAR *psave, bool *isTrunc);
TCHAR *ew_makeBeginEllipsisEx(HDC hDC, TCHAR *text, int cx, int needOffs, bool *isTrunc);

// Конвертация координат
void ew_cvtNum2Size(int, int, long *, long *);
void ew_cvtSize2Num(long, long, int *, int *);

// Рассчитать размер клиенсткой области в размер окна
void ew_adjustDlgCoord(DLGCOORD *dlg, DWORD dwStyle, DWORD dwExStyle);
void ew_alignDlgCoord(DLGOBJ *, int, int, int, int, DLGCOORD *);

// Рассчитать координаты диалога и его элементов
void ew_calcDlgCoord(DLGOBJ * dlgobj);

// Конвертация строк
void ew_convMesStr(char *);
void ew_convMenuStr(char *);
void ew_convStatusStr(LPTSTR str);
void ew_convButtonStr(char *);
void ew_convFieldStr(char *);
char  *ew_convToMultiLine(const char *, char *, size_t);
size_t ew_convFromMultiLine(LPCTSTR src, LPTSTR dst, size_t size);

// -----------------------------------------------------------------------------
// FrameObj
FRAMEOBJ *ew_createFrame(const char *title, int x, int y, int cx, int cy, HWND hParentWnd);
void ew_destroyFrame(FRAMEOBJ *);

bool ew_showFrame(FRAMEOBJ *, int show);
void ew_openFrame(FRAMEOBJ *frame);
void ew_closeFrame(FRAMEOBJ *frame);

bool ew_initFrameSize(FRAMEOBJ *frame);
void ew_initFrameStatus(FRAMEOBJ *frame);

bool ew_calcFrameSize(int numcols, int numrows, int *cx, int *cy);
bool ew_restoreFrameData(int *x, int *y, int *cx, int *cy, int *show);
void ew_saveFrameData(FRAMEOBJ *);

void ew_setFrameMenu(MENUOBJ *menu);
void ew_restoreFrameMenu(MENUOBJ *menu);
void ew_updateFrameMenu(MENUOBJ *menu);

void ew_scrollFrameToRect(FRAMEOBJ *frame, long left, long top, long right, long bottom, int mode);
void ew_updateFrameScrollBars(FRAMEOBJ *frame);

// -----------------------------------------------------------------------------
// StatusBar
HWND ew_createStatusBar(HWND hParentWnd, bool bToolTips, bool bSizeGrip, bool bBalloon, SECTIONS *sect);
void ew_destroyStatusBar(HWND hStatusBar);
void ew_layoutStatusBar(HWND hStatusBar);
void ew_processIdleStatus(HWND hStatusBar);

void ew_layoutDialogStatus(HWND hStatusBar);
void ew_setDialogStatusText(HWND hStatusBar, int, const char *);
int  ew_getStatusPane(HWND hStatusBar, int id);

// -----------------------------------------------------------------------------
// Notify (Balloon)
BALLOONOBJ *ew_createBalloonObject(HWND hStatusBar, int timeOut);
void ew_deleteBalloonObject(BALLOONOBJ *obj);
bool ew_createBalloon(BALLOONOBJ *obj, const char *head, const char *text, int secID, int iconID, int timeOut, int colorText, int colorBack);
void ew_destroyBalloon(BALLOONOBJ *obj);
BOOL ew_processCMStatus(HWND hwnd, WPARAM wParam, LPARAM lParam, BALLOONOBJ *obj, bool isCM);
void ew_cmdNotify(int cmd, int sub);

// -----------------------------------------------------------------------------
// Menu
BOOL ew_isMenuEnabled(HWND hWnd);
BOOL ew_initDropDownMenu(MENUOBJ *menu, HMENU hMenu, UINT uItem);
BOOL ew_processMenuHelp(MENUOBJ *menu, HELPINFO *helpInfo);
BOOL ew_processMenuSelect(HWND hStatusBar, HMENU hMenu, UINT uFlags, UINT uItem);
BOOL ew_processMenuCommand(MENUOBJ *menu, UINT uItem, BOOL fromToolBar = FALSE);
void ew_processIdleMenu(MENUOBJ *);

#ifdef __EWEXTT_BUILD
    bool ew_downloadMenu(PANEL *pnl, MENU *pMenu);
#endif //__EWEXTT_BUILD

// -----------------------------------------------------------------------------
// DlgObj
DLGOBJ *ew_createDlgObject(PANEL *, DLGOBJ *);
void    ew_deleteDlgObject(DLGOBJ *);

// -----------------------------------------------------------------------------
// DlgScroll
DLGSCROL *ew_createDlgScrol(int, int, int, int, int, int, void *, size_t);
void ew_deleteDlgScrol(DLGSCROL *);

// -----------------------------------------------------------------------------
// DlgColumn
void ew_initDlgColumn(const DLGOBJ *);

// -----------------------------------------------------------------------------
// DlgItems
void ew_initDlgItems(DLGOBJ *);

// -----------------------------------------------------------------------------
// DlgCeil
DLGCEIL **ew_allocDlgCeil(DLGSCROL *);
void ew_deleteDlgCeil(DLGSCROL *);
void ew_moveDlgCeil(DLGSCROL *scrol, int dest, int src, int count);

// -----------------------------------------------------------------------------
// DlgEditor
DLGEDITOR *ew_createDlgEditor(int, int, int, int, bool isImager);
bool       ew_reallocDlgEditor(DLGEDITOR *, int dx, int dy, int flag);
void       ew_deleteDlgEditor(DLGEDITOR *);


// -----------------------------------------------------------------------------
// TxtBuffer
TCHAR **ew_allocTxtBuffer(int dx, int dy);
TCHAR **ew_reallocTxtBuffer(TCHAR **, int dx, int dy, int new_dx, int new_dy);
void ew_freeTxtBuffer (TCHAR **);

// -----------------------------------------------------------------------------
// DlgTab
DLGTAB *ew_createDlgTab(int, int, int, int, const char *);
void    ew_deleteDlgTab(DLGTAB *);

// -----------------------------------------------------------------------------
// PopupObj
POPUPOBJ  *ew_createPopupObject(const char *text, const char *btext, const char *status,
                                const char *caption, bool isWeb, const char* result);
void       ew_deletePopupObject(POPUPOBJ *);

// -----------------------------------------------------------------------------
// Dialog
bool ew_createDialog(DLGOBJ *);
void ew_destroyDialog(DLGOBJ *);
bool ew_isActiveDlg(DLGOBJ *dlgobj);
RECT ew_getDlgItemRect(const DLGOBJ *, int);

// -----------------------------------------------------------------------------
// Scroll
bool ew_createScroll(DLGOBJ *);
void ew_runScroll(const DLGOBJ *, bool);
RECT ew_getScrollRect(const DLGOBJ *, int, int);
RECT ew_getScrollEditRect(const DLGOBJ *);
void ew_invalidScrollRect(const DLGOBJ *, int, int, BOOL);
void ew_checkNumRec(DLGOBJ *dlgobj, int numRec, int op);
UINT ew_getColumnWidth(const DLGOBJ *, int nColumn);
void ew_setColumnWidth(const DLGOBJ *, int nColumn, UINT nWidth);
void ew_setColumnWidthMH(const DLGOBJ *, int nColumn, UINT nWidth);
void ew_moveScrollColumn(const DLGOBJ *, int deltaX);
void ew_moveScrollRow(const DLGOBJ *, int row, int num, int delta);
BOOL ew_hittestScroll(const DLGOBJ *, POINT *, int *, int *);
void ew_scrollGoToCol(const DLGOBJ * dlgobj, int col);
int  ew_scrolFldNToColN(const DLGOBJ *blgobj, int fld);
void ew_updateHeaderMH(const DLGOBJ *dlgobj, int col, int new_cx, int right);
void ew_storeHeaderMH(const DLGOBJ *dlgobj);
void ew_restoreHeaderMH(const DLGOBJ *dlgobj);
void ew_cutHeaderMH(const DLGOBJ *dlgobj, int dx);

// -----------------------------------------------------------------------------
// Field
int  ew_getFieldAlign(const FIELD* fld, int* needOffs);
void ew_addFieldStyle(const DLGFIELD *, DWORD *, DWORD *, DWORD_PTR *);
void ew_addFieldBorder(const DLGFIELD *, DWORD *pStyle, DWORD *pExStyle);
void ew_remFieldBorder(const DLGFIELD *, DWORD *pStyle, DWORD *pExStyle);

void ew_setFieldFont(const DLGOBJ *, int field);
void ew_setFieldValidator(DLGFIELD *);

void ew_enableField(HWND hFieldWnd, DLGFIELD *dlgfield, bool enable);
int  ew_processField(DLGOBJ *dlgobj, DLGFIELD *dlgfield, HWND hWnd);

void ew_initDlgField(DLGOBJ *dlgobj, DLGFIELD *dlgfield);
void ew_initListField(DLGOBJ *dlgobj, DLGFIELD *dlgfield);

void ew_putDlgFields(DLGOBJ *, int, int);
void ew_putListFields(DLGOBJ *, int, int, int);

int  ew_processDlgField(DLGOBJ *);
int  ew_processListField(DLGOBJ *, PANEL * pnl);

void ew_checkDlgFields(DLGOBJ *dlgobj);
void ew_checkListFields(DLGOBJ *dlgobj, int);

char * ew_setLViewItemText (DLGOBJ *, int, int, char *);
void ew_setLViewTreeItem (DLGOBJ *, int, int, char *);
void ew_setLViewFindData(DLGOBJ *, int, int, int, char *, unsigned char);

char * ew_field2str(FIELD *fld, char *buf, size_t sizebuf, bool fEdit, bool fOemFont);
char * ew_field2str2(FIELD *fld, char *buf, size_t sizebuf, bool fEdit, bool fOemFont);

// -----------------------------------------------------------------------------
// Header
bool ew_createHeader(DLGOBJ *dlgobj);
void ew_initHeader(DLGOBJ *dlgobj);
void ew_setHeaderText(const DLGOBJ *dlgobj);
void ew_layoutHeader(const DLGOBJ *dlgobj, int flags);
BOOL ew_notifyHeader(DLGOBJ *dlgobj, WPARAM wParam, LPARAM lParam);
void ew_updateHeaderToolTip(const DLGOBJ *dlgobj);
int  ew_headerIndToOrd(HWND hWnd, int Ind);
void ew_setColumnData(const DLGOBJ *dlgobj, int col);
BOOL ew_getHeaderRect(const DLGOBJ *dlgobj, RECT &rch);

// -----------------------------------------------------------------------------
// Editor
bool ew_createEditor(DLGOBJ *dlgobj);
void ew_runEditor(DLGEDITOR *, bool);
bool ew_resizeEditor(DLGEDITOR *editor, int deltaX, int deltaY);
BOOL ew_hittestEditor(const DLGEDITOR *, POINT *);

// -----------------------------------------------------------------------------
// Tab
bool ew_createTab(DLGTAB *, HWND);
void ew_destroyTab(DLGTAB *tab);

void ew_adjustTabCtrl(DLGTAB *);
void ew_adjustTabItem(DLGTAB *, const DLGOBJ *);
BOOL ew_notifyTab(DLGTAB *, WPARAM, LPARAM);

// -----------------------------------------------------------------------------
// Popup
bool ew_createPopup(DLGPOPUP *popup);
void ew_destroyPopup(DLGPOPUP *popup);
void ew_closePopup(DLGPOPUP *popup);

// -----------------------------------------------------------------------------
// ToolTip
bool ew_createToolTip(POPUPOBJ *popup, bool);
void ew_trackToolTip(DLGPOPUP *popup);

// Обобщенная функция создания всплывающих подсказок
HWND ew_createToolTips(DWORD dwStyle, DWORD dwExStyle, HWND hParentWnd, HINSTANCE hInstance);

// -----------------------------------------------------------------------------
// VldObj
VLDOBJ *ew_createVldObj(char, ...);
void ew_deleteVldObj(VLDOBJ *);
BOOL ew_processVldObj(VLDOBJ *, HWND, UINT, WPARAM, LPARAM);

// -----------------------------------------------------------------------------
// FmtObj
FMTOBJ *ew_createFmtObj(DLGFIELD *dlgfield);
void  ew_deleteFmtObj(FMTOBJ *);
BOOL  ew_processFmtObj(FMTOBJ *, HWND, UINT, WPARAM, LPARAM);
void  ew_drawFmtObj(FMTOBJ *fmtobj, HDC, HBRUSH, int isScrol);
bool  ew_isFmtEditMode(void *p);

// -----------------------------------------------------------------------------
// Format
int  ew_getFormatAlign (void *fmt, int *needOffs);
const char *ew_getFormatResult(void *fmt);
bool ew_getFormatModify(void *fmt);
bool ew_getFormatValue(void *fmt, FIELD *fld);
void ew_getFormatMode(void *fmt, BOOL *fOverWriteMode);

void ew_setFromatSelection(void *fmt, bool);
void ew_setFormatValue(void *fmt, FIELD *fld);
void ew_setFormatMode(void *fmt, BOOL fOverWriteMode);

// -----------------------------------------------------------------------------
// Clipboard
BOOL ew_processClipboard(HWND hWnd, int wID, UINT iMessage, WPARAM wParam, LPARAM lParam, int keystat);
BOOL ew_processClipboardKey(HWND hWnd, int wID, int virtkey, int kbflags);

// -----------------------------------------------------------------------------
// Application Data
void ew_initAppData(const char *appid, const char *system, const char *version, const char *copyright, bool beepMode);
void ew_cleanupAppData(void);
bool ew_readAppData(const char *key, void **pbuf, size_t *psize);
bool ew_writeAppData(const char *key, void *buf, size_t size);
void ew_loadAppData(void);
void ew_saveAppData(void);
APPDATA_ITEM *ew_addAppData(int type, size_t size);
APPDATA_ITEM *ew_findAppData(int type);

void ew_initAppServData(int cmdll, const char *hostName, const char *pipeName, int port);
void ew_cleanupAppServData(void);

RSL_CFUNC void ew_arrangeDlgItems (const DLGOBJ *);
RSL_CFUNC void ew_blinkDlgItems (const DLGOBJ *);
RSL_CFUNC void ew_movedDlgItems (const DLGOBJ *);

// -----------------------------------------------------------------------------
// Color support
void  ew_setFldColor (DLGOBJ *, int);
void  ew_setCeilColor (DLGOBJ *, int, int);
LRESULT  ew_processColor (HWND, WPARAM, LPARAM);
LRESULT  ew_processColorInLView (HWND, WPARAM, LPARAM);

// -----------------------------------------------------------------------------
// Process palette messages
LRESULT ew_processPaletteChanged(HWND hWnd, WPARAM wParam);
LRESULT ew_processQueryNewPalette(HWND hWnd);

// -----------------------------------------------------------------------------
// Window title
void *ew_saveTitle(HWND);
void  ew_restTitle(HWND, void *);

// -----------------------------------------------------------------------------
// Mouse and keyboard support
void  ew_setMessagePos(DWORD);
DWORD ew_getMessagePos(void);

void ew_putKey(int key, int target);
void ew_putChar(int ch, int target);
void ew_putCharEx(int ch, int target, bool from_WM_CHAR);
void ew_putMouseEvent(int key, int x, int y, int item, int target);
void ew_putMenuEvent(int menu);
void ew_putResizeEvent(int numcols, int numrows);
void ew_putScrolEvent(int key, int pos, int item, int target);
void ew_putWheelEvent(int key, int target);
void ew_putHeaderEvent(int key, int item, int butn, int target);

BOOL ew_processKeyboard(HWND hWnd, int virtkey, int kbflags, MSG *pMsg);
BOOL ew_processMenuKeyboard(HWND hWnd, int virtkey, int kbflags, MSG *pMsg);
BOOL ew_processPopupKeyboard(HWND hWnd, int virtkey, int kbflags, MSG *pMsg);

BOOL ew_processMouseInField(DLGFIELD *, HWND, UINT, WPARAM, LPARAM);
BOOL ew_processMouseInLView(DLGOBJ *, HWND, UINT, WPARAM, LPARAM);
BOOL ew_processMouseInEditor(DLGEDITOR *, UINT, WPARAM, LPARAM);
BOOL ew_processMouseInMenu(HWND, UINT, WPARAM, LPARAM);

BOOL ew_processScrollMessage(HWND, UINT, WPARAM, LPARAM, int);

// -----------------------------------------------------------------------------
// Message filter and idle routines
BOOL ew_filterProc(MSG *,void *);
void ew_idleProc(void *,unsigned level);

// -----------------------------------------------------------------------------
// Local message queue
void ew_addMessage(MSG *, int kbflags);
bool ew_getMessage(MSG *, int *kbflags);
void ew_pumpMessage(HWND hFocusWnd);
void ew_clearMessage(void);

int  ew_moveColumn(DLGOBJ *dlgobj, int oldOrder, int iOrder);

BOOL ew_restOrderHead(DLGOBJ *dlgobj);

void ew_setFieldAlign(const DLGFIELD *dlgfield, DWORD *pStyle);

void ew_entryUnlock(int entry);
bool ew_isLockMessage(MSG *pMsg);
void ew_destroyLockWnd(void);

// -----------------------------------------------------------------------------
// Далаем из наименования пункта меню строку в формате GDI
// [in]  buf  - буфер под результирующую строку
// [in]  name - исходное имя пункта меню
// [in]  pos  - позиция "горячего" символа
// [ret] возвращает указатель на buf или NULL
char *ew_MakeMenuItemString(char *buf, const char *name, int pos);

// -----------------------------------------------------------------------------
// Разбиваем строку меню на две части. Разделителем служат символы табуляции.
// Символы табуляции в результирующие строки не попадают.
// [in]  name  - исходная строка
// [out] buffL - левая часть строки
// [out] buffR - правая часть строки
void ew_SplitMenuItemString(const char *name, char *buffL, char *buffR);

// -----------------------------------------------------------------------------
// ToolBar
void ew_clearToolBar(FRAMEOBJ *frame);

// -----------------------------------------------------------------------------
// Настройка ToolBar
class CCustomizeToolBar 
{
 private:
   HWND      m_hToolBar;
   int       m_nResetCount; 
   TBBUTTON* m_pResetButtons; 
   std::vector<TBBUTTON> m_SavedButtons;
 public: 
   CCustomizeToolBar() {} 
   CCustomizeToolBar::CCustomizeToolBar(HWND hToolBar) 
     : m_hToolBar(hToolBar), m_nResetCount(0), m_pResetButtons(NULL){}

   ~CCustomizeToolBar(){}  

   void AddSavedButtons(TBBUTTON* tb);
   std::string NeedText(NMHDR* pNMHDR, bool bFromToolBar = true);
   void GetButtons(int& nSavedCount, TBBUTTON*& pSavedButtons);   
   void SetButtons(int nSavedCount, TBBUTTON* pSavedButtons);   

   BOOL OnBeginAdjust(NMHDR* pNMHDR, LRESULT* pResult);   
   BOOL OnEndAdjust(NMHDR* pNMHDR, LRESULT* pResult);   
   BOOL OnGetButtonInfo(NMHDR* pNMHDR, LRESULT* pResult);   
   BOOL OnQueryDelete(NMHDR* pNMHDR, LRESULT* pResult);   
   BOOL OnQueryInsert(NMHDR* pNMHDR, LRESULT* pResult);   
   BOOL OnReset(NMHDR* pNMHDR, LRESULT* pResult);   
   BOOL OnToolBarChange(NMHDR* pNMHDR, LRESULT* pResult);  
   BOOL OnNeedText(NMHDR* pNMHDR, LRESULT* lResult);   
};

// -----------------------------------------------------------------------------
// Загрузить картинку из ресурса с учётом значения bmpId
HBITMAP ew_LoadBitmap(int bmpId);
HICON   ew_LoadIcon(int iconId);

void* CreateWebFormData(const char *attr, const char *resultField);
void DeleteWebFormData(void *ptr);
HWND CreateWebControl(void *ptr, HWND parent, RECT *rect, const char *url);
void MoveWebControl(void *ptr, DWORD x, DWORD y, DWORD cx, DWORD cy, BOOL repaint);
void OnDestroyWebControl(void *ptr);
BOOL PretranslateWebControl(void *ptr, LPMSG msg);
const char* GetWebControlResult(void *ptr, size_t *sz);
void FreeWebControlResult(const char *buff);
void SetFocusWebControl(void *ptr);
bool GetDispatchWebControl(void *ptr, IDispatch **outPtr);

bool GetIntProp(const char *attr, const char *key, long *result, bool *isPerSent);


void ewInitAccessibility();
void ewDoneAccessibility(bool force);

void DeleteAccessibleObj(EwAccessibleObj *ptr);
bool CreateAccessibleProxyFor(EwAccessibleObj *ptr, WPARAM wParam, LPARAM lParam, LRESULT *pResult);

EwAccessibleObj *CreateAccessibleObjForDLGFIELD(DLGFIELD *pObj);
EwAccessibleObj *CreateAccessibleObjForDLGOBJ(DLGOBJ *pObj);
EwAccessibleObj *CreateAccessibleObjForPOPUPOBJ(POPUPOBJ *pObj);
EwAccessibleObj *CreateAccessibleObjForDLGSCROL(DLGOBJ *pObj);

EwAccessibleObj *CreateAccessibleObjForMenu(HWND hWnd, HMENU hMenu);
void RepairAccessibleObjForMenu(EwAccessibleObj *pAcc, HMENU hMenu);

void SetSatusPaneTo(const char *str);

// Получаем данные MENU, ассоциированные с пунктом меню
MENU *GetMenuItemData(HMENU hMenu, int item, MENUITEMINFO *mii);

// Добавляем меню hSrcMenu к меню hDstMenu
void AppendContextMenu(HMENU hDstMenu, HMENU hSrcMenu, bool addSeparator = false);

// Восстанавливаем данные меню (в блоке EWMenuData)
void RestoreMenu(HMENU hMenu);

// Обработка сообщения WM_MEASUREITEM для меню
BOOL Menu_ON_MEASUREITEM(HWND hWnd, LPMEASUREITEMSTRUCT lpMI);

// Обработка сообщения WM_DRAWITEM для меню
BOOL Menu_ON_DRAWITEM(LPDRAWITEMSTRUCT lpDIS);

// Обработка сообщения WM_CONTEXTMENU
BOOL Menu_ON_CONTEXTMENU(DLGFIELD *dlgfield, DLGOBJ *dlgobj, HWND hWnd, WPARAM wParam, LPARAM lParam);

int  ew_resizeDialog2(DLGOBJ *dlgobj, int deltaX, int deltaY);

void ew_DateToStr(TCHAR *str, size_t size, int year, int month, int day);

// Ручная отрисовка кнопки в WM_DRAWITEM
void Button_ON_DRAWITEM(DLGOBJ *dlgobj, int fldID, DRAWITEMSTRUCT *pDIS);

// -----------------------------------------------------------------------------
// Загрузка картинки из буфера
void *LoadImageFromBuff(EWPictureData *data, const char *ptr, int size);

// -----------------------------------------------------------------------------
// Конвертация картинки для корректного вписывания в окно с сохранением пропорций.
RECT IMG_ConvertRect(RECT &rcScreen, RECT &sizePicture, bool bCenter);

// -----------------------------------------------------------------------------
// Выполнить конвертацию названия кнопки. А именно:
//   - Удалить начальные пробелы
//   - Удалить символы '~'
//   - Удалить конечные пробелы
void ew_CvtButtonName(char *str);

#ifdef __cplusplus
}
#endif

#endif //__EASYWIN_H

/* <-- EOF --> */