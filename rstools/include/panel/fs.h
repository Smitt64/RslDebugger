/*-----------------------------------------------------------------------*-
 File Name   : fs.h

 Sybsystem   : FS - библиотека
 Description : Поддержка эффективного экранного доступа для DOS,DPMI16,WIN32

 Source      :

 Library(s)  : fsl.lib fs16.lib fs32.lib

 Copyright (c) 1991 - 1994 by R-Style JSC
 All Rights Reserved.

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/panel/h/fs.h $
                  $Revision: 62 $
                  $Date: 4.07.00 18:57 $
                  $Author: Kubrin $
                  $Modtime: 4.07.00 18:57 $
-*- History -------------------------------------------------------------*-
 October 8,1992   Sergei Kubrin (K78) - Create file
 January 6,1995   Serg Kubrin (K78)
            Добавил стандартное макро __FS_H
            Коды клавиш вынес в файл keys.h
 March 13,1995   Serg Kubrin (K78)
            Поддержка WIN32
 Добавлены новые функции:

    fs_toScreen    - эти функции должны использоваться вместо устаревших
    fs_fromScreen    vram и vptr

    fs_initsize    - функция производит переинизиализацию пакета после
                     изменения параметров консоли. Может вызываться при
                     обработке сообщения K_RESIZE

    fs_suspend     - может вызываться перед запуском внешних программ
    fs_resume      - восстанавливает работу пакета после fs_suupend
    fs_openEx      - альтернативная функция инициализации пакета


    TestScroll     - поддержка скролинга областей экрана
    fs_getevent    - обработка клавиатуры и мыши
    fs_getkbd      - обработка только клавиатуры
    fs_getMouseEvent - обработка только мыши
    KbdIdle        - указатель на процедуру, вызываемую когда нет
                     нет событий от мыши или клавиатуры

 22 января 1997  Kubrin Serg
    Добавлены функции fs_setattr  и fs_curattr. Раньше были define

 February 24,1998 Kubrin Sergey
    Add:
    1) fs_getXlatFlag, fs_setXlatFlag, fs_setXlanNotify
    2) Support for alt xlate table
    3) Change fs_setWin32Xlat return value


-*-----------------------------------------------------------------------*/

#if !defined(__FS_H)
#define __FS_H

#include "keys.h"

#define FSH   // Оставляем для совместимости

#include <stddef.h>

#include "platform.h"
#include "rstypes.h"
#include "bdate.h"
#include "tooldef.h"

#ifndef RSL_MD_FLAT
    #define  FSFAR  _far
#else
    #define  FSFAR
#endif


// Default socket number for debugging fs-terminal-enabled applications
#define  RSL_FS_DBG_PORT  78

// Standard FS terminal codepage
#define  RSL_FS_CODEPAGE  866

#include "packpshn.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Main (startup) procedure for fs-terminal-enabled application
//
typedef int (DLMAPIC *fsapp_mainproc_t)(int, char **);

//
// Cleanup procedure for fs-terminal-enabled application
//
typedef void (DLMAPIC *fsapp_cleanup_t)(int stat);

//
// Idle keyboard procedure
//
typedef int (DLMAPIC *fs_idle_proc)(void);


// January 21,2000
enum
   {
    FSCALLBACK_EXIT,
    FSCALLBACK_MSG,
    FSCALLBACK_BREAK,
    FSCALLBACK_IDLE,
    FSCALLBACK_CTRLB,
    FSCALLBACK_SHOWMES,
    FSCALLBACK_USERACTIVE,
    FSCALLBACK_BEGINIDLE,
    FSCALLBACK_RESIZE,
    FSCALLBACK_PUTINDIC,
    FSCALLBACK_REMINDIC,
    FSCALLBACK_NEWINDIC
   };

typedef int (DLMAPIC *fs_callback_t)(int, void *);

_TOOLEXP void  fs_setcallback(fs_callback_t);

//
// External (exported) variables
//
extern _TOOLEXP   int  fs_numrows;   /* Количество строк на экране        */
extern _TOOLEXP   int  fs_numcols;   /* Количество символов в строке      */
extern _TOOLEXP   int  fs_mode;      /* Текущий видеорежим                */


extern   char  fs_mono,      /* Монохромный режим                 */
               fs_snow;      /* CGA                               */

extern   char  fs_init,      /* Флаг иницилизации fs_пакета       */
               fs_ega,       /* Флаг EGA/VGA_адаптера             */
               attr;         /* Текущий атрибут                   */

extern   char  fs_actpage,   /* Текущая активная страница         */
               fs_vizpage;   /* Текущая видимая  страница         */

extern  unsigned fs_base;    /* Сегментный адрес видеопамяти      */


#define  FONT8x8   0x0200    /* Может применяься с CT8025 и MT8025 для   */
#define  FONT8x14  0x0100    /* загрузки шрифта 8x8 или 8x14             */

typedef enum VMODE
      {
       CT4025C,
       CT4025,
       CT8025C,
       CT8025,
       CGCGA,
       CGCGAC,
       MGCGA,
       MT8025,

       RSGUIMOD     = -10,
       RSGUIMOD_DBG = -11

      } VMODE;

#if  !defined(__COLORS)
#define __COLORS
    typedef enum COLORS
          {
           BLACK,        /* dark colors */
           BLUE,
           GREEN,
           CYAN,
           RED,
           MAGENTA,
           BROWN,
           LIGHTGRAY,
           DARKGRAY,     /* light colors */
           LIGHTBLUE,
           LIGHTGREEN,
           LIGHTCYAN,
           LIGHTRED,
           LIGHTMAGENTA,
           YELLOW,
           WHITE
          } COLORS;
#endif

#define  fs_setsnow(sn)  (fs_snow = (char)(sn))
//    fs_setsnow( 1 );  Устанавливает контроль на обратный ход луча
//    fs_setsnow( 0 );  Снимает контроль

                /* Возможно многократное открыте с разными режимами */
_TOOLEXP int  fs_open   /* Открывает работу с fs_пакетом,             */
(    /* возвращает установленный режим          */
 int mode       /* Номер устанавливаемого видеорежима          */
);              /* Устанавливает активной и видимой 0-ю страницу    */

_TOOLEXP void  fs_close(void);   /* Закрывает работу с fs_пакетом    */
        /* Восстанавливает видео режим и страницу       */

_TOOLEXP void  fs_setactpage(int page); /* Устанавливает активную страницу        */
_TOOLEXP void  fs_setvizpage(int page); /* Устанавливает  видимую страницу        */


_TOOLEXP void  fs_setcrtmode    /* Устанавливает видеорежим          */
(                         /* Для загрузки шрифта 8х8 :                  */
 int mode                 /*     fs_setcrtmode( CT8025 | SMALLFONT );   */
);

/* Возвращает текущий видеорежим */
_TOOLEXP int  fs_getcrtmode(void);

/* Возвращает указатель на видеопамять позиции x, y   */
void FSFAR *vptr(int x, int y);

/* Назначает строке длиной nomber атрибут attr     */
void TOOLDLMC fs_wrtnatr(int x, int y, int number, int attr);


void TOOLDLMC fs_setattr(unsigned color);      /* Set default attribute        */
unsigned TOOLDLMC fs_curattr(void);             /* Вернуть текущий атрибут      */

#define  makeattr(b, f)  (unsigned)(((b) << 4) | (f))   /* Создать атрибут */
#define  makecha(at, ch) (((at) << 8)|((ch) & 0x00ff))  /* Атрибут+символ  */

/* Выводит на экран стоку *str c текущим атрибутом    */
void TOOLDLMC fs_wrtstr(int x, int y, const char *str);

void TOOLDLMC fs_wrtncell /* Записывает nomber символов в видеопамять  */
(
 int x,
 int y,
 int number,
 int charattr    /* atr*256+ch              */
);

#define  fs_buffsize(a, b, c, d)  (((c) - (a) + 1)*((d) - (b) + 1)*2)

/* Записывает содержимое области экрана в буфер */
void TOOLDLMC fs_gettext(int x1, int y1, int x2, int y2, void *buf);

/* Выводит содержимое буфура на экран     */
void TOOLDLMC fs_puttext(int x1, int y1, int x2, int y2, const void *buf);

/* Перемещает по экрану прямоугольную область   */
void TOOLDLMC fs_movetext(int x1, int y1, int x2, int y2, int dx, int dy);

/* Копирует nomber символов (с атрибутами)   */
void vram(
          int         number,
          void FSFAR *sourse,   /* Адрес откуда копировать       */
          void FSFAR *dest      /* Адрес куда копировать         */
         );

/* Заполняет экран символом ch с атрибутом atr  */
void TOOLDLMC fs_clr(int ch, int atr);

/* Заполняет область символом ch с атрибутом atr   */
void TOOLDLMC fs_bar(int x1, int y1, int x2, int y2, int ch, int atr);

/* Рамка             */
enum BORD
   {
    BORDOFF,     /* Отсутствует             */
    BORDDUBL,    /* Двойная                 */
    BORDSING,    /* Одинарная               */
    BORDSOL,     /*                         */
    BORHDVS,     /*                         */
    BORVSHD      /*                         */
   };

/* Рисует рамку               */
void TOOLDLMC fs_box(int style, int attr, unsigned lcol, unsigned trow, unsigned rcol, unsigned brow);

/************************************************************************/
/*                              */
/*       Работа с курсором            */
/*                              */
/************************************************************************/

#define  NORMCUR  0x0607          /* Обычный курсор       */
#define  BARCUR   0x0106          /* Большой курсор       */
#define  MIDCUR   0x0307          // Средний курсор
#define  NOCUR    0x2000          /* Невидимый курсор        */

/* Устанавливает форму курсора            */
void TOOLDLMC fs_setcurtype(unsigned int cur);

/* Позиционирует курсор          */
void TOOLDLMC fs_setcurpoz(int x, int y);

/* Возвращает форму и координаты курсора */
unsigned TOOLDLMC fs_getcurpoz(int *x, int *y);

/*   March 7,1995 (K78)
    Обработка клавиатуры
*/
extern _TOOLEXP fs_idle_proc  KbdIdle ;

_TOOLEXP int fs_peekEvent(int *shiftState);

/* Возвращает код с ожиданием и инициализирует ряд глобальных переменных */
_TOOLEXP int  fs_getevent        (void);
/* Возвращаит один код при нажатии функц.       */
/* и символьных  клавиш                         */
_TOOLEXP int  fs_getkbd          (void);
_TOOLEXP int  fs_getMouseEvent   (void);
_TOOLEXP int  fs_getKbdNoWait    (int delayTime);
_TOOLEXP int  fs_testForAllEvents(void);
_TOOLEXP void fs_clearEvents     (void);

extern _TOOLEXP int  enhancedBiosFlag;

extern _TOOLEXP int  Xm;
extern _TOOLEXP int  Ym;
extern _TOOLEXP int  Butn;  /* Координаты курсора мыши и статус кнопок  */
extern _TOOLEXP int  LastXm;
extern _TOOLEXP int  LastYm;
extern _TOOLEXP int  kbflags;
extern _TOOLEXP int  scan;
extern _TOOLEXP int  Mouse;
extern _TOOLEXP unsigned long  LastEventTic;


// Функции на замену vram и vptr
_TOOLEXP void fs_toScreen(int x, int y, int number, const unsigned short *buf);
_TOOLEXP void fs_fromScreen(int x, int y,int number, unsigned short *buf);

_TOOLEXP void fs_initsize(void);
_TOOLEXP void fs_suspend(void);
_TOOLEXP void fs_resume(void);
_TOOLEXP int  fs_openEx(int x, int y);

_TOOLEXP int  TestScroll(int x, int y, int width, int nline, int key);


#define  XLATE_DPMI32  1
#define  XLATE_WIN95   2
#define  XLATE_WINNT   4

_TOOLEXP int fs_setWin32XlatEx(int key, int state, int platformMask, const char *homeDir);
_TOOLEXP int fs_setWin32Xlat(int virtKey, int state, int platformMask);
/* virtKey - задает виртуальный код клавиши из windows.h */
/* state - комбинация SH_ флагов                         */


#define  FLT_GET_KBD      1
#define  FLT_GET_MOUSE    2
#define  FLT_GET_ALL      3

typedef struct TFsVersionOld
      {
       short int  number;
       short int  revision;

       short int  min;
       short int  max;
       short int  cmdll;
      } TFsVersionOld;

typedef struct TFsVersion
      {
       short int      number;
       short int      revision;

       unsigned long  buildNumber;

       short int      cmdll;         // true if communication DLL

       unsigned long  flags;
       short int      termNumber;
      } TFsVersion;

// Версия Сервера приложений
typedef struct TAppVersion
      {
       short int  cmdll;

       unsigned short  verHi;
       unsigned short  verLow;
       unsigned short  build;
       unsigned short  subBuild;
      } TAppVersion;

// Параметры консоли
typedef struct TConsoleInfo
      {
       unsigned short  cmd;           // Код команды (пока не используется)

       unsigned short  screen_max_x;  // Размер консольного окна по ширине
       unsigned short  screen_max_y;  // Размер консольного окна по высоте
      } TConsoleInfo;


// -----------------------------------------------
#define  dMAX_HOST_LEN    256 // Должно быть равно TERM_MAX_HOST_LEN из server\hmon\clnt\term.hpp !!!
#define  dMAX_PIPE_LEN     50

// Параметры используемого Сервера приложений
typedef struct TUsedServApp
      {
       char            hostName[dMAX_HOST_LEN];  // Имя компьютера (SERVER из term.ini)
       char            pipeName[dMAX_PIPE_LEN];  // Имя пайпа (PIPENAME из term.ini)
       unsigned short  port;                     // Номер TCP-порта
      } TUsedServApp;

TOOLDLMEXP void TOOLDLMC fs_version(TFsVersion *ver);

// Получить версию используемого Сервера приложений
_TOOLEXP void fs_versionApp(TAppVersion *ver);

// Получить версию используемого Сервера приложений
_TOOLEXP void fs_usedServApp(TUsedServApp *data);

// Получить версию используемого Сервера приложений
_TOOLEXP void fs_usedServApp2(TUsedServApp *data);

// Отправить сообщение на Сервер
_TOOLEXP int  fs_srv_SetClientUserInfo(const char *str);

// Получить параметры консоли
_TOOLEXP void fs_GetConsoleInfo(TConsoleInfo *data);

// Установить код возврата приложения (отправить на терминал)
_TOOLEXP void fs_SetExitCode(int status, int statusEx);

_TOOLEXP int  fs_getCtrlBreak(void);

_TOOLEXP void fs_runInfo(char *prg1, char *prg2);
_TOOLEXP void fs_beep(void);

_TOOLEXP void *fs_getFullHaspInfo(void);
_TOOLEXP void *fs_getFullHaspInfo2(void);
_TOOLEXP void *fs_getFullHaspInfo3(void);

// Получить режим поиска HASP. Возвращает значение вида dHASP_FINDMODE_
_TOOLEXP int   fs_getHaspMode(void);

TOOLDLMEXP void TOOLDLMC fs_padstr(int x, int y, char *str, int outlen);
TOOLDLMEXP void TOOLDLMC fs_wrtItemStr(int x, int y, const char *str, int outlen, int selAtr);

_TOOLEXP char RSL_FAR *MakeItemStr(char RSL_FAR *strn, char RSL_FAR *hstr, char RSL_FAR *buff,
                                   int  outlen, int offs, int atn, int ats);

_TOOLEXP int   itemlen (const char RSL_FAR *strn);

_TOOLEXP void  ChrToBuf(unsigned short RSL_FAR *buff, int character, int atr, int size);
_TOOLEXP void  AtrToBuf(unsigned short RSL_FAR *buff, int atr, int size);
_TOOLEXP void  StrToBuf(unsigned short RSL_FAR *buff, char RSL_FAR *strn, int atr, int size);
_TOOLEXP void  AtrStrToBuf(unsigned short RSL_FAR *buff, unsigned char RSL_FAR *attrStr, int size);

_TOOLEXP int   GetNumLock(void);
_TOOLEXP void  SetNumLock(int on);

_TOOLEXP void  fs_statLine(const char *str, int atr, int selAtr);


typedef struct TMarkPos
      {
       db_uint16 xB;
       db_uint16 xE;
       unsigned char attr;
      } TMarkPos;

TOOLDLMEXP void TOOLDLMC fs_wrtMarkStr(int x, int y, char *str, int outlen, int num, TMarkPos *pos);

_TOOLEXP void fs_wrtMarkStrEx(int x, int y, char *str, int outlen, int num, TMarkPos *pos, unsigned char *attrStr);

void *DLMAPI fs_saveStat(void);
void  DLMAPI fs_restStat(void *);

TOOLDLMEXP void TOOLDLMC fs_saveStatLine(void *buf);

TOOLDLMEXP int  TOOLDLMC fs_getBuffSize(int x1, int y1, int x2, int y2);

_TOOLEXP int fs_isInited(void);

#ifndef TFSEVENT_DEFINED
#define TFSEVENT_DEFINED
    // -------------------------------------------
    // Просьба не обольщаться! Структура передаётся через TEWPutEvent, у которой одноименные поля имеют
    // тип int16, а поле Butn - uchar.
    typedef struct
          {
           int key;
           int kbflags;
           int scan;
           int Xm;
           int Ym;
           int Butn;
          } TFSEvent;
#endif

int   DLMAPI    fs_event(TFSEvent *ev, int waitTime);
void  DLMAPI    fs_screenSize(int *nucols, int *numrows);
void  DLMAPI    fs_makeEvent(TFSEvent *ev);

typedef void (DLMAPIC *RslTimerCheck_f)(void);

_TOOLEXP void fs_setRslTimerPtr(RslTimerCheck_f ptr);
_TOOLEXP RslTimerCheck_f fs_getRslTimerPtr(void);

typedef void (DLMAPIC *fs_XlatNotify_f)(int flag);

_TOOLEXP int  fs_getXlatFlag(int *isAltXlate);
_TOOLEXP void fs_setXlatFlag(int code);
_TOOLEXP void fs_setXlatNotify(fs_XlatNotify_f proc);

TOOLDLMEXP char *TOOLDLMC  fs_getDeferBuff(size_t sz, const char *dllName, int cmd);
TOOLDLMEXP void  TOOLDLMC  fs_queueMessage(char *mes);
TOOLDLMEXP char *TOOLDLMC  fs_getSendBuff(size_t sz, const char *dllName, int cmd);
TOOLDLMEXP void  TOOLDLMC  fs_sendMessage(const char *mes);
TOOLDLMEXP char *TOOLDLMC  fs_transactMessage(char *mes, size_t *sz);

#ifndef EXCLUDE_FS_ASYNC
#ifdef _INC_WINDOWS
    #ifdef DEFFSCOMPLETE
        typedef struct TFsComplete
              {
               HANDLE   ev;
               void    *elem;
               char     reserv[8];
              } TFsComplete;
    #else
        #ifdef __cplusplus
            struct TFsComplete;
        #else
            typedef void *TFsComplete;
        #endif

    #endif
        bool fs_sendAsyncEx(const void *in, size_t sz, const char *tblName, int cmdId, TFsComplete *cmplPtr);
        bool fs_resultAsyncEx(TFsComplete *cmplPtr, DWORD timeOut, void *out, size_t szOut, size_t *szOutPtr);
        void fs_cancelAsync(TFsComplete *cmplPtr);
    #else
        bool fs_sendAsyncEx(const void *in, size_t sz, const char *tblName, int cmdId, void *cmplPtr);
        bool fs_resultAsyncEx(void *cmplPtr, unsigned timeOut, void *out, size_t szOut, size_t *szOutPtr);
        void fs_cancelAsync(void *cmplPtr);
    #endif
#endif // EXCLUDE_FS_ASYNC

_TOOLEXP void fs_sendDeferMessages(void);

//
// fs_startApp
// Function started fs-terminal-enabled application in standard or test mode
// Parameters:
//    rsMain   -  main application entry point
//    rsExit   -  cleanup procedure (can be NULL)
//    argc     -  number of parameters (as in main)
//    argv     -  array of parameters (as in main)
//
_TOOLEXP int fs_startApp(fsapp_mainproc_t rsMain, fsapp_cleanup_t rsExit, int argc, char **argv, int port);

#define  FS_NORM_MODE  0
#define  FS_COMM_MODE  1
#define  FS_NOFS_MODE  2

_TOOLEXP int fs_startDLL(int mode, int numrows, int numcols, int vmode, int mouse, int argc, char **argv);
int fs_initDLM(void);

_TOOLEXP int    fs_argc(void);
_TOOLEXP char **fs_argv(void);

_TOOLEXP unsigned long fs_setMinDelay(unsigned long tm);

#define  FS_AUTO_FLUSH         1
#define  FS_NOFS               2
#define  FS_APPARTMENT         4    // fs32stw returns this flag
#define  FS_SURROGATE          8    // Application runs under rcomcnt.exe
#define  FS_PROHIBITREMOTE    16    // prohibit remote RSCOM under fs32stw
#define  FS_DSBLMSGLOOP       32

_TOOLEXP unsigned  fs_flags(unsigned fl, bool set);

#ifdef RSL_PL_MS
    #define  fs_setflags(fl, set)  fs_flags((fl), (set))
    #define  fs_getflags()         fs_flags(0, 0)
#else
    unsigned long fs_setflags(unsigned long fl, bool set);
    unsigned long fs_getflags();
#endif

#define  RSL_ARGC  (fs_argc())
#define  RSL_ARGV  (fs_argv())

// Spec. for internal init. data in fs32p.lib
void fs_initData(int isCom, int numrows, int numcols, int mode, int mouse);
void fs_doneData(void);

_TOOLEXP int fs_addPrnAlias(const char *alias, const char *prnName);

typedef int (DLMAPIC *fs_asyncProc_t)(int cmd, const void *inMes, void *outMes);

TOOLDLMEXP void TOOLDLM fs_addDispTable(const char *name, fs_asyncProc_t p);
_TOOLEXP   bool         fs_addDispTable2(const char *name, fs_asyncProc_t p);
TOOLDLMEXP void TOOLDLM fs_remDispTable(const char *name);
_TOOLEXP   void         fs_addEvent(int key, int shift, int scan, int Xm, int Ym, int Butn);


// Обработчик фатальных ошибок
//
void fs_setGUImode(void);

void DisableFsCalls(void);
void EnableFsCalls(void);
void DoneCommClient();

int fs_getInitMode(void);

_TOOLEXP void fs_setSleepTime(int tm);

void fs_beginAction(unsigned tm, void *data);
void fs_endAction(unsigned tm);
int  fs_cancelActionIfDelay(void);
void fs_changeAction(const char *mes);

_TOOLEXP void fs_setRSCOMmoduleName(const char *name); // Need for NOFS mode only.
void fs_prepareRSCOM (void); // Need for NOFS mode only.

typedef void (*ctrlbrk_action_proc)(void);
void SetBreakActionProc(ctrlbrk_action_proc proc);
void RaiseBreakActionProc(void);


// -----------------------------------------------------------------------------
#define  FS_APPSYSNAME  128

// -----------------------------------------------------------------------------
// Значения для флага TTermAppInfo::syncType
#define  dSyncType_Off          0x0000  // Синхронизация выключена
#define  dSyncType_On           0x0001  // Синхронизация с проверкой актуальности
#define  dSyncType_Force        0x0002  // Принудительная синхронизация (без проверки актуальности)
#define  dSyncType_Report       0x0004  // Вывод результатов синхронизации
                                        // Само по себе смысла не имеет, только в комбинации с dSyncType_On/dSyncType_Force

// -----------------------------------------------------------------------------
typedef struct
      {
       unsigned  version; // Set before call to fs_getTerminalAppInfo;

       uint16    syncType;
       uint32    syncDataLow;
       uint32    syncDataHi;


       uint16    verHi;
       uint16    verLow;
       uint16    build;
       uint16    subBuild;

       char      systemName[FS_APPSYSNAME];
      } TTermAppInfo;

// -----------------------------------------------------------------------------
_TOOLEXP bool fs_getTerminalAppInfo(TTermAppInfo *data);


// -----------------------------------------------------------------------------
// Дефайны аналогичны одноименным (без префикса 'd') из server\hmon\monmsg.hpp
#define  dNAME_LEN_COMP     31   // Имя компьютера (включая 0-символ)
#define  dNAME_LEN_COMP2   256   // Полное имя компьютера (включая 0-символ)
#define  dNAME_LEN_USER    257   // Имя пользователя (включая 0-символ)

#define  dADDR_LEN_MAC       6   // Разрядность MAC-адреса (байт)
#define  dADDR_LEN_IP4       4   // Разрядность IP4-адреса (байт)
#define  dADDR_LEN_IP6      16   // Разрядность IP6-адреса (байт)


// -----------------------------------------------------------------------------
typedef struct TClientNetInfo
      {
       unsigned char  mac[dADDR_LEN_MAC];        // MAC-адрес сетевой карты терминала
       unsigned char  ip4[dADDR_LEN_IP4];        // IP-адрес терминала (формат IPv4)
       unsigned char  ip6[dADDR_LEN_IP6];        // IP-адрес терминала (формат IPv6) - пока не используется
       char           compName[dNAME_LEN_COMP];  // Имя компьютера
       char           userName[dNAME_LEN_USER];  // Имя пользователя
      } TClientNetInfo;

// -----------------------------------------------------------------------------
typedef struct TClientNetInfo2
      {
       unsigned char  mac[dADDR_LEN_MAC];        // MAC-адрес сетевой карты терминала
       unsigned char  ip4[dADDR_LEN_IP4];        // IP-адрес терминала (формат IPv4)
       unsigned char  ip6[dADDR_LEN_IP6];        // IP-адрес терминала (формат IPv6) - пока не используется
       char           compName[dNAME_LEN_COMP2]; // Имя компьютера
       char           userName[dNAME_LEN_USER];  // Имя пользователя
      } TClientNetInfo2;

// -----------------------------------------------------------------------------
_TOOLEXP bool fs_getTerminalNetInfo(TClientNetInfo *data);
_TOOLEXP bool fs_getTerminalNetInfo2(TClientNetInfo2 *data);

// -----------------------------------------------------------------------------
// Не совсем к месту, но другого пока не нашел
// Получение даты/времени с терминала посредством расширения rsextt.d32
// Если не трёхзвенка, возвращает дату/время с машины приложения
_TOOLEXP bool fs_getRemoteDateTime(bdate *dt, btime *tm);

// Тоже, но всегда возвращает локальную дату/время
_TOOLEXP bool fs_getLocaleDateTime(bdate *dt, btime *tm);

// -----------------------------------------------------------------------------
int fs_openConsole(int *numrows, int *numcols, int *mouse);
int fs_closeConsole(void);

#ifdef __cplusplus
}
#endif

#include "packpop.h"

// -----------------------------------------------
#ifndef RSL_DLM_MES_TABLE
    #define  RSL_DLM_MES_TABLE(name)  int name (int cmd, const void *inMes, void *outMes)   \
                                      { switch (cmd) { default: break;
#endif

// -----------------------------------------------
#ifndef RSL_DLM_HANDLE
    #define  RSL_DLM_HANDLE(msg)      case msg: return mes_##msg (inMes, outMes);
#endif

// -----------------------------------------------
#ifndef RSL_DLM_MES_END
    #define  RSL_DLM_MES_END          } return 0; }
#endif

#endif  // __FS_H

/* <-- EOF --> */