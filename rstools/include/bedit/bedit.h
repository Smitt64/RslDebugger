/*******************************************************************************
 FILE         :   BEDIT.H

 COPYRIGHT    :   VECsoft, 1992
                  R-Style Software Lab, 1998

 DESCRIPTION  :   Система работы с В-файлами

 PROGRAMMED BY:

 CREATION DATE:   26.11.92

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/bedit/h/bedit.h $
                  $Revision: 31 $
                  $Date: 28.06.00 16:57 $
                  $Author: Kubrin $
                  $Modtime: 28.06.00 16:52 $

 January 20,1995   Serg Kubrin (K78)
    Поддержка транзакций RSL - RslTrn, RslAbortTrn
 January 25,1995   Serg Kubrin (K78)
           Добавлен TrnFlag
 January 3, 1996 Yakov E. Markovitch (JNS)
   Добавлено #ifdef __cplusplus, несколько typedef'ов
 February 3, 1996 Yakov E. Markovitch (JNS)
   Added nonscrollable area support
 February 20,1997   Serg Kubrin (K78)
   Horizontal scrolling

*******************************************************************************/

#ifndef __BEDIT_H
#define __BEDIT_H

#include "btrv.h"
#include "desktop.h"
#include "ledit.h"
#include "btserver.h"
#include "beditdef.h"
#include "msel.h"

// Параметры работы Bedit
#define  NUMREST       3   // Количество попыток восстановить скролинг
#define  TRANSRETRY    5   // Число автоповторов в транзакции
#define  TRANSDELAY  220   // Время задержки при автоповторе в транзакции

/*******************************************************************************
 JNS
*******************************************************************************/

// Says YES if field scrollable
#define  IsScrollable(p, f)        IsInPanel((p), (f))
// Puts all scrollable fields with offset x, y
#define  PutScrollable(p, x, y)    PutSFields((p), ON, x, y)
// Puts all static fields with offset x, y
#define  PutStatic(p, x, y)        PutSFields((p), OFF, x, y)


/*******************************************************************************/

typedef struct tagBSCROL  BSCROL;


/*******************************************************************************
 Functional typedefs
*******************************************************************************/
typedef int (*JBScrollMaker)(BSCROL *);

// Описание процедуры инициализации записи
typedef int (*JBScrollRecInit)(BSCROL *);

// Принимает указатель на скроллер и значение статуса
typedef int (*JBScrollRecTest)(BSCROL *, int);

typedef JBScrollMaker    bscrol_makerec_t;
typedef JBScrollRecInit  bscrol_initrec_t;
typedef JBScrollRecTest  bscrol_testrec_t;

//
// Logging function
//
typedef int (*bedit_logfn_t)(
                             int       iOperation,      // Операция
                             BTRVFILE *bfile,           // Контролируемый файл
                             void     *saveb,           // Буфер записи до выполнения операции
                             int       iSizeVarLenS     // Длина буфера до выполнения
                            );

/*******************************************************************************/

typedef struct tagTCOLINFO
      {
       const char *colHeader;
       const char *fieldName;
       int          colWidth;
       int           fldType;
       int          decPoint;
      } TCOLINFO;

/*******************************************************************************/

// описатель сколинга файла Btrieve
struct tagBSCROL
     {
      PANEL            Mp;       // панель записи
      BTRVFILE        *bfile;    // ссылка на Btrieve-файл
      int              Mn;       // количество записей области
      int              numrec;   // количество записей
      int              currec;   // номер текущей активной записи
      coord            x,y,l,h;  // координаты области
      PANEL           *Mpe;      // панель редактирования записи
      PANEL           *Mpi;      // панель ввода записи

      JBScrollRecInit  n;        // процедура инициализации записи
      JBScrollRecTest  i;        // процедура проверки при вводе.записи
      JBScrollRecTest  c;        // процедура проверки при модифик.записи
      JBScrollRecTest  d;        // процедура проверки при удалении записи

      JBScrollMaker    make;     // процедура изменения вида скролинга

      unsigned char    mode;     // режимы скролинга
      void            *saveb;    // буфер сохранения записи
      recpos_t        *ListA;    // список физич. адресов записей области
      PANEL           *Mps;      // панель скролинга

     //February 20,1997 (K78)
      PANEL            wrkPan;
      char             isTrunc;  // True if last field in wrkPan is truncated
      int              startF;
      char            *headBuff;
      void            *horSaveb; // буфер сохранения записи  (used while horiz scroll)
     // char      inHorScrol;
      unsigned char    mode2;    // режимы скролинга (которые не поместились в mode)

     // MSEL
      MULTISELECT     *lpSelect; // список массового выбора
      panel_keyproc_t  proc ;    // Обработчик клавиатуры - теперь он хранится тут!

      int              sizeb;    // длина записи в буфере сохранения
      void            *filtr;    // параметры фильтра
      void            *allocBuf; // динамически выделенная память

      long             flags;    // флаги типа FiltChanged и т.п.

      char            *userAskOnDelete;  // Пользовательский запрос на удаление записей

      void            *vHeader;  // Указатель на объект типа std:vector
      coord            old_y;    // Исходная координата начала области скроллинга
      int              old_Mn;   // Исходное число строк в области скроллинга
     };

// -----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

_BEDITFUNC int BsGoTo(BSCROL *Bs, int disp, int dir);
_BEDITFUNC int BsGoToFld(BSCROL *Bs, int disp);
_BEDITFUNC int BsGoToPos(BSCROL *Bs, recpos_t RecPos);

// Проверка, находиться ли мышь в области скроллинга
_BEDITFUNC int MouseInBScrol(BSCROL *Bs);

// Проверяем всю область скроллинга, в том числе и пустую, без записей, в незаполненом до конца скроллинге
_BEDITFUNC int MouseInBScrolTotal(BSCROL *Bs);

int  IsInPanel(const PANEL *p, const FIELD *f);
void PutSFields(PANEL *p, int scr, coord xoffs, coord yoffs);

/************************ примитивные функции ***************************/

// Расширенное сообщение об ошибке менеджера
_BEDITFUNC int bexperr(
                       const char *mes,      // Текст сообщения
                       int   stat            // Статус менеджера
                      );

// Перехватчик сообщений об ошибках при работе с B-trieve
typedef int (*BErrUserFilterType)(int errStat, const char *msg);
_BEDITFUNC BErrUserFilterType ToolsSetBErrFilter(BErrUserFilterType newFilter);

   // Cообщение об ошибке менеджера
_BEDITFUNC int berr( const char *mes, const char *rsd_mes = NULL );

// Получить сообщение berr'а в виде текста (безынтерфейсная функция)
_BEDITFUNC void get_berr_text(const char *mes, char *outmes);

// Установить пользовательский текст запроса на уделние записи
_BEDITFUNC void BsSetUserAsqOnDelete(BSCROL *Bs, const char *msg);

// Инициализировать B-запись
_BEDITFUNC void bsinit(BSCROL *Bs);

// Сравнить буфера записи
_BEDITFUNC int bscompb(BSCROL *Bs);

// Сохранить буфер записи
_BEDITFUNC void bssaveb(BSCROL *Bs);

// Восстановить буфер записи
_BEDITFUNC void bsrestb(BSCROL *Bs);

// Режимы обновления скролинга
typedef enum UPDTMODE
      {
       UPDTCUR = 1,    // Обновить текущую запись на экране
       UPDTREC,        // Обновить текущую запись, загрузив из файла
       UPDTSCRL,       // Обновить изображение всей области скролинга
       UPDTZOOM,       // Обновить страницу и скролинг начиная с RecPos
       UPDTPAGE        // Обновить страницу записей и область скролинга

      } UPDTMODE;

#define  CheckUpdtMod(Bs)  ((Bs)->mode & 0x0f)
#define  SetUpdtNo(Bs)     (Bs)->mode &= 0xf0  // Снять запросы на обновление скролинга

// Установить режим обновления скролинга
_BEDITFUNC void SetUpdtMod(
                           BSCROL  *Bs,    // описатель скролинга
                           int      mod    // режим
                          );

/************************************************************************/
/*         Режим редактирования скролинга         */
#ifndef IsScrlMod
    #define  IsScrlMod(Bs)   ((Bs)->mode & 0x10)               // Определить режим редактирования скроллинга
    #define  SetScrlNo(Bs)   (Bs)->mode &= 0xef                // Выкл. режим редактирования
    #define  SetScrlMod(Bs)  (Bs)->mode |= (unsigned char)0x10 // Вкл. режим редактирования
#endif

#ifndef IsReadOnlyMod
    #define  IsReadOnlyMod(Bs)   ((Bs)->mode & 0x20)               // Определить read-only режим
    #define  SetReadOnlyNo(Bs)   (Bs)->mode &= 0xdf                // Выкл. read-only режим
    #define  SetReadOnlyMod(Bs)  (Bs)->mode |= (unsigned char)0x20 // Вкл. read-only режим
#endif

/************************************************************************/
/*         Режим вставки записи             */
#ifndef IsInsMod
    #define  IsInsMod(Bs)   ((Bs)->mode & 0x40)                    // Определить режим вставки
    #define  SetInsNo(Bs)   (Bs)->mode &= 0xbf                     // Выкл. режим вставки записи
    #define  SetInsMod(Bs)  (Bs)->mode |= (unsigned char)0x40      // Вкл. режим вставки записи
#endif

#define  IsFindMod(Bs)          ((Bs)->mode & 0x80)                // Определить режим поиска
#define  SetFindNo(Bs)          (Bs)->mode &= 0x7f                 // Выкл. режим поиска
#define  SetFindMod(Bs)         (Bs)->mode |= (unsigned char)0x80  // Вкл. режим поиска

#define  IsEmptyScrol(Bs)       (!(Bs)->numrec)                    // Скроллинг пустой? (работает для скроллингов в AllowEmptyMod'е)

#define  IsRecModified(Bs)      ((Bs)->mode2 & 0x04)               // Определить режим скроллинга без записей
#define  SetRecNotModified(Bs)  (Bs)->mode2 &= 0xfb                // Выкл. режим скроллинга без записей
#define  SetRecModified(Bs)     (Bs)->mode2 |= (unsigned char)0x04 // Вкл. режим скроллинга без записей

#define  IsAllowEmptyMod(Bs)    ((Bs)->mode2 & 0x02)               // Определить режим скроллинга без записей
#define  SetAllowEmptyNo(Bs)    (Bs)->mode2 &= 0xfd                // Выкл. режим скроллинга без записей
#define  SetAllowEmptyMod(Bs)   (Bs)->mode2 |= (unsigned char)0x02 // Вкл. режим скроллинга без записей

#define  IsHorScrolMod(Bs)      ((Bs)->mode2 & 0x01)               // Определить режим горизонтального скроллирования
#define  SetHorScrolNo(Bs)      (Bs)->mode2 &= 0xfe                // Выкл. режим горизонтального скроллирования
#define  SetHorScrolMod(Bs)     (Bs)->mode2 |= (unsigned char)0x01 // Вкл. режим горизонтального скроллирования

#define  IsVertScrolMod(Bs)     ((Bs)->mode2 & 0x08)               // Определить режим горизонтального скроллирования
#define  SetVertScrolNo(Bs)     (Bs)->mode2 &= 0xf7                // Выкл. режим горизонтального скроллирования
#define  SetVertScrolMod(Bs)    (Bs)->mode2 |= (unsigned char)0x08 // Вкл. режим горизонтального скроллирования

#define  Internal1  0x10 // Флаг устанавливается в mode2. Используется только в инструменте для определеня
                         //  возврата в скроллинг из панели ввода. Устанавливается при выходе из панели ввода.
                         //  Снимается после передачи в обработчик скроллинга ENTFIELD

#define  Internal2  0x40 // Флаг устанавливается в mode2. Используется только в инструменте для определеня
                         // возврата в скроллинг из панели редактирования

#define  FiltMod    0x20 // Флаг устанавливается, если в BSCROL'е используется пользовательский фильтр

#define  IsFiltMod(Bs)     ((Bs)->mode2 & FiltMod)
#define  SetFiltMod(Bs)    ((Bs)->flags |= FiltChanged, (Bs)->mode2 |= FiltMod)
#define  UnsetFiltMod(Bs)  (Bs)->mode2 &= (unsigned char)~FiltMod

#define  SkipMakeMod  0x80   // Флаг устанавливается, если в make не надо вызывать GetE

#ifdef SQLBUILD
    #define  IsSkipMakeMod(Bs)   ((Bs)->mode2 & SkipMakeMod)
    #define  SetSkipMakeNo(Bs)   (Bs)->mode2 &= (unsigned char)~SkipMakeMod
    #define  SetSkipMakeMod(Bs)  (Bs)->mode2 |= (unsigned char)SkipMakeMod
#else
    #define  IsSkipMakeMod(Bs)   ((Bs)->mode2 & 0)
    #define  SetSkipMakeNo(Bs)   (Bs)->mode2 &= (unsigned char)~SkipMakeMod
    #define  SetSkipMakeMod(Bs)  (Bs)->mode2 &= (unsigned char)~SkipMakeMod
#endif

// -----------------------------------------------------------------------------
#define  BegBfile        0x00000001
#define  EndBfile        0x00000002
#define  FiltChanged     0x00000004
#define  PartLine        0x00000008
#define  PartRec         0x00000010

#define  dBSF_RunMode    0x00010000   // Скроллинг в режиме BUseScrol


#define  IsBsRunMode(Bs)     ((Bs)->flags & dBSF_RunMode)
#define  SetBsRunMode(Bs)    ((Bs)->flags |= dBSF_RunMode)
#define  UnsetBsRunMode(Bs)  ((Bs)->flags &= ~dBSF_RunMode)


#define  IsBegBfile(Bs)      ((Bs)->flags & BegBfile)
#define  SetBegBfile(Bs)     (Bs)->flags |= BegBfile
#define  UnsetBegBfile(Bs)   (Bs)->flags &= ~BegBfile

#define  IsEndBfile(Bs)      ((Bs)->flags & EndBfile)
#define  SetEndBfile(Bs)     (Bs)->flags |= EndBfile
#define  UnsetEndBfile(Bs)   (Bs)->flags &= ~EndBfile

#define  IsFiltCh(Bs)        ((Bs)->flags & FiltChanged)
#define  SetFiltCh(Bs)       (Bs)->flags |= FiltChanged
#define  UnsetFiltCh(Bs)     (Bs)->flags &= ~FiltChanged

#define  IsPartLine(Bs)      (((Bs)->flags & PartLine) ? 1 : 0)
#define  SetPartLine(Bs)     (Bs)->flags |= PartLine
#define  UnsetPartLine(Bs)   (Bs)->flags &= ~PartLine

#define  IsPartRec(Bs)       (((Bs)->flags & PartRec) ? 1 : 0)
#define  SetPartRec(Bs)      (Bs)->flags |= PartRec
#define  UnsetPartRec(Bs)    (Bs)->flags &= ~PartRec

// -----------------------------------------------------------------------------
// Внутриинструментальный аналог IsFindMod
// Возвращает 1, если скроллинг находится в режиме поиска записи и 0 в остальных
// случаях.
int IsBsFindMode(void);

// -----------------------------------------------------------------------------
// Внутриинструментальный аналог SetFindMod
void SetBsFindMode(void);

// -----------------------------------------------------------------------------
// Внутриинструментальный аналог SetFindNo
void SetBsFindNo(void);

/************************************************************************/

// Установить сообщение скролинга
// сообщение
_BEDITFUNC void setBMES(char *mes);


/************************* работа в скролинге ***************************/
// Скролинг записей
_BEDITFUNC int BUseScrol(BSCROL *Bs);

// Вывести скролинг
_BEDITFUNC int BPutScrol(
                         BSCROL  *Bs,    // описатель скролинга
                         int  savef      // флаг состояния
                        );

// Удалить скролинг
_BEDITFUNC void BRemScrol(BSCROL *Bs);

// Использовать скролинг
_BEDITFUNC int BLoopScrol(BSCROL *Bs);

// Редактор записей скролинга
_BEDITFUNC int BEditScrol(BSCROL *Bs);

// Редактор записей скролинга
_BEDITFUNC int BRunScrol(BSCROL *Bs);

/************************* обработчики панелей ***************************/

// Код возврата процедур-обработчиков пользователя
#define  QUITSCRL  -600  // Аварийно покинуть скролинг
#define  NOREC     -601  // Нет записей в файле

// Код возврата процедур-проверок пользователя
#define  ALREXE      -1  // Операция выполнена пользователем

// Стандартная процедура панели ввода записи
// Реакция на клавиши:
// ESC - выйти из панели
// F9  - ввести запись и начать вводить новую
_BEDITFUNC int inrec(
                     PANEL *p,  // Описатель панели ввода
                     int    key // код клавиши
                    );

// Стандартная процедура редактирования записи
// Реакция на клавиши:
// ESC - выйти из панели
// F8  - удалить запись и выйти из панели
// F9  - обновить запись и выйти из панели
_BEDITFUNC int edrec(
                     PANEL *p,  // Описатель панели редактирования
                     int    key // код клавиши
                    );

// Стандартная процедура обслуживания списка зап.
// Реакция на клавиши:
//    ESC   - выйти из скролинга
//    UP, DOWN, PDUP, PGDOWM, +Ctrl - обновить текущую запись (если
//       надо) и перейти на другую запись
//    ENTER - предоставить панель редактирования записи (если задана)
//    F4    - найти запись
//    F8    - удалить запись
//    F9    - ввести новую запись или предоставить панель ввода (если задана)
//    NOREC - ввести новую запись (если Edit)
_BEDITFUNC int scrlrec(
                       PANEL *p,  // Описатель панели скролинга
                       int    key // код клавиши
                      );

// Обновить текущую запись
// возврат: = key - запрос выполнен
// возврат: = 0   - запрос невыполнен
// возврат: =-key - разрушен скролинг
_BEDITFUNC int updtrec(
                       PANEL *p,  // Описатель панели
                       int    key // код клавиши
                      );

/******************************* поиск **********************************/

typedef int (*find_rec_proc)(void *buff, void *value);

// Найти запись перебором
// код возврата:
//    0 - найдено
//  < 0 - не найдено
//  > 0 - ошибка Btrieve при поиске
_BEDITFUNC int find_rec(
                        BTRVFILE *bf,      // Описатель записи
                        void     *value,   // Значение поиска
                        find_rec_proc ic   // Программа проверки записи
                       );

// Найти запись перебором
// код возврата:
//    0 - найдено
//  < 0 - не найдено
//  > 0 - ошибка Btrieve при поиске
_BEDITFUNC int find_rec2(
                         BSCROL   *bs,      // Описатель записи
                         void     *value,   // Значение поиска
                         find_rec_proc ic   // Программа проверки записи
                        );

// Найти запись в скролинге
// код возврата:
//    0 - найдено
//  < 0 - не найдено
//  > 0 - ошибка Btrieve при поиске
_BEDITFUNC int find_srec(
                         BSCROL *Bs,        // Описатель скролинга
                         void   *value,     // Значение поиска
                         int    way,        // Способ поиска: 0-по ключу, !0-перебором
                         find_rec_proc ic   // Программа проверки записи
                        );

// Найти запись в скролинге по полю
// код возврата:
//    0 - найдено
//  < 0 - не найдено
//  > 0 - ошибка Btrieve при поиске
_BEDITFUNC int pick_srec(
                         BSCROL *Bs,          // Описатель скролинга
                         FIELD  *f,           // Поле поиска
                         int    way,          // Способ поиска: =0-по ключу, !0-перебором
                         const char *Invite,  // Приглашение к вводу значения поиска
                         const char *Oops     // Сообщение при неудачном поиске
                        );

// Установить фильтр в скролинге по полю
_BEDITFUNC int filtr_srec(
                          BSCROL *Bs,          // Описатель скролинга
                          FIELD  *f,           // Поле поиска
                          const char *Invite,  // Приглашение к вводу значения фильтра
                          const char *Oops     // Сообщение при неудачном поиске
                         );

// Снять фильтр в скролинге по полю
_BEDITFUNC int cancel_filtr_srec(
                                 BSCROL *Bs    // Описатель скролинга
                                );

// Фильтрация по полям скролинга
_BEDITFUNC int bs_filtr(
                        BSCROL *Bs,            // Описатель скролинга
                        const char *Invite,    // Приглашение к вводу значения фильтра
                        const char *Oops       // Сообщение при неудачном поиске
                       );

// Сортировка по полям скролинга
// Описатель скролинга
_BEDITFUNC int bs_sorting(BSCROL *Bs);

// Печать полей скроллинга
// Описатель скролинга
_BEDITFUNC int bs_print(BSCROL *Bs);

// Печать в форматеXML
// Описатель скролинга
_BEDITFUNC int bs_printXML(BSCROL *Bs);

// -----------------------------------------------------------------------------
// Пользовательская обработка выделенных записей в скроллинге
// Описатель скролинга
_BEDITFUNC int bs_userselected(BSCROL *Bs);

// -----------------------------------------------
// Значения флагов, передаваемые в функцию BsSetUserSelectedFlags
typedef enum TEUserSelectedFlag
      {
       eUSF_DisableHandler  = 1,  // Блокировать обработчик bs_userselected
       eUSF_DisableProgress = 2,  // Не показывать индикатор прогресса
       eUSF_DisableReport   = 4,  // Блокировать вывод отчет после отработки макроса
       eUSF_SkipReturnValue = 8   // Не обрабатывать возвращаемое макросом значение
      } TEUserSelectedFlag;

// -----------------------------------------------
// Функция установки режимов работы обработчика bs_userselected
// [in]  flags - Комбинация (по OR) флагов TEUserSelectedFlag
// [ret]       - Возвращает предыдущую комбинацию флагов
_BEDITFUNC int BsSetUserSelectedFlags(int flags);

// -----------------------------------------------------------------------------

find_rec_proc GetBsFindProc(void);

/********************* работа с переменной частью ***********************/

_BEDITFUNC int BEdit_Var   // Редактор строк переменной части записи
(
 LSCROL   *L,    // описатель редактора строк
 BSCROL   *Bs,      // описатель скролинга
 int     LConst, // длина постоянной части записи
 int     *CurrNum   // текущее количество строк в записи
);

/********************* работа с ресурсами ******************************/

// Создать BSCROL
_BEDITFUNC BSCROL *CreateBS(
                            unsigned int    St,    // Стиль изображения панели
                            unsigned char   Pnumf, // Количество полей панели
                            unsigned char   Pnumt, // Количество текстов в списке
                            coord           x1,    // Координаты панели
                            coord           y1,    //
                            coord           x2,    //
                            coord           y2,    //
                            int             PHelp, // Подсказка к панели
                            unsigned char   Pff,   // Номер начального поля
                            int             Mn,    // Количество записей в области
                            coord           x,     // Координаты области
                            coord           y,
                            coord           l,
                            coord           h,
                            unsigned        adsize
                           );

_BEDITFUNC BSCROL *CreateColInfoBS(
                                   unsigned int    St,            // Стиль изображения панели
                                   coord           x,             // Координаты панели
                                   coord           y,             //
                                   coord           cx,            // Размеры скроллинга
                                   coord           cy,            //
                                   panel_keyproc_t proc,          // Обработчик клавиатуры
                                   void           *Parm,          // Для передачи параметров
                                   const char     *Pstatus,       // Информация для строки состояния
                                   const char     *Phead,         // Заголовок
                                   BTRVFILE       *bfile,
                                   TCOLINFO       *colInfo,
                                   int             numCols,
                                   int             readOnly,
                                   const char     *uniqName,
                                   unsigned        adsize
                                  );

// Конструктор BSCROL.
_BEDITFUNC BSCROL * CrBs(
                         ResFile    *rt,     // Открытый ресурс.
                         const char *member  // Имя раздела.
                        );

// Определение BSCROL из ресурса
_BEDITFUNC void  BSetFlds(
                          BSCROL          *m,
                          panel_keyproc_t  proc,    // Обработчик клавиатуры
                          void            *Parm,    // Для передачи параметров
                          void            *Pstatus, // Информация для строки состояния
                          const char      *Phead,   // Заголовок
                          BTRVFILE        *bfile,   // ссылка на Btrieve-файл
                          PANEL           *Mpe,     // панель редактирования записи
                          PANEL           *Mpi,     // панель ввода записи
                          bscrol_initrec_t n,       // процедура инициализации записи
                          bscrol_testrec_t i,       // процедура проверки при вводе.записи
                          bscrol_testrec_t c,       // процедура проверки при модифик.записи
                          bscrol_testrec_t d,       // процедура проверки при удалении записи
                          bscrol_makerec_t make,    // процедура изменения вида скролинга
                          ...
                         );

_BEDITFUNC int ConfCr(char *nf, void *cfb, int l);

_BEDITFUNC void BeditApp(const char *Systema, const char *Desk, const char *Copr, const char *Res,
                         const char *Helpfile, const char *Bdir);


_BEDITFUNC int TrnEx(bedit_trnfn_t t, void *Parm, const char *warn, int concurrent);

// Аналог TrnEx для заданного клиента
_BEDITFUNC int TrnExClnt(bedit_trnfn_t t, void *Parm, const char *warn, int concurrent, bclient_h client);

#define  Trn(a, b, c)         TrnEx(a, b, c, 1)
#define  TrnClnt(a, b, c, d)  TrnExClnt(a, b, c, 1, d)


_BEDITFUNC int TrnSilentMode(int mode);

/*   January 20,1995 (K78)
    Поддержка транзакций RSL
*/
void RslErrorTrn(void);
int  RslTrn(bedit_trnfn_t proc, void *Parm);

// Установить функцию записи в журнал
_BEDITFUNC bedit_logfn_t SetBLogProc(bedit_logfn_t logproc);

// Снять функцию записи в журнал
_BEDITFUNC void RemBLogProc(void);

// Вызвать функцию записи в журнал
_BEDITFUNC int  _BLogProc(
                          int       iOperation,        // Операция
                          BTRVFILE *bfile,             // Контролируемый файл
                          void     *saveb,             // Буфер записи до выполнения операции
                          int       iSizeVarLenS       // Длина буфера до выполнения
                         );

_BEDITFUNC int  RestCurRec(BSCROL *Bs);
_BEDITFUNC void DrawHeads(BSCROL *Bs);

_BEDITFUNC int  SRinit(BSCROL *Bs, int key);
_BEDITFUNC void CopyWorkFields(BSCROL *Bs, int startF);

// MSEL
_BEDITFUNC int BsStandardProc(PANEL *p, int key);
_BEDITFUNC int BsStandardMake(BSCROL *Bs);

// Считать запись из файла с учетом сортировки в скроллинге
_BEDITFUNC int BsGet(BSCROL *Bs, int bop);
_BEDITFUNC int BsGD(BSCROL *Bs);
_BEDITFUNC int BsGDEx(BSCROL *Bs);
_BEDITFUNC int BsGDp(BSCROL *Bs, recpos_t pos);
_BEDITFUNC int BsGDpEx(BSCROL *Bs, recpos_t pos);

// Обновить скроллинг, используя установленный режим
_BEDITFUNC int BUpdateScrol(BSCROL *Bs);

/************* Автоматическое определение ширины колонки *****************/

#define  BsSetAutoWidth(ind, w)  SetAutoWidth(ind, w)

/************************ Печать BSCROL ***********************************/

typedef int (*BScrollPrint)(BSCROL *, const char *);

_BEDITFUNC void SetPrintBs(BScrollPrint);      // Установить печать
_BEDITFUNC void SetPrintBsXML(BScrollPrint);

_BEDITFUNC void SetPrnDirBs(const char *dir);  // установить дир.для файлов
                                               // с распечатанным скролингом

/****************************************************************************/

// временная функция для отключения и включения блокировки POSTREC
_BEDITFUNC void SetBlkPostRec(int val);

// включение/отключение переноса названия поля на вторую строку
_BEDITFUNC void BSetDblStrHead(int val);

// включение/отключение нового режима редактирования
_BEDITFUNC void BSetNewEditMode(int val);

// Возвращает не 0, если включен новый режим редактирования
_BEDITFUNC int BGetNewEditMode(void);

_BEDITFUNC int  BeditInit(void);
_BEDITFUNC void BeditDone(void);


// Отправить сообщение о выходе с поля при выходе из скроллинга

_BEDITFUNC int SendBsFldMesOut(BSCROL *Bs);

// Установить флаг пункта меню, прикр. к скроллингу
_BEDITFUNC int SetBsMenuFlag(
                             BSCROL     *bs,    // указатель на панель, к которой прикреплено меню
                             int         key,   // код клавиши, соответсвующей пункту меню (м.б.0)
                             const char *name,  // Имя пункта меню (м.б.NULL, при условии key!=0)
                             int         flag,  // флажок
                             int         active // ON / OFF
                            );


// Функция освобождения памяти, выделенной для скроллинга.
//  Обязательно должна вызываться, если к скроллингу прикреплено меню, загруженное из ресурса
_BEDITFUNC void FreeBScrol(BSCROL *bs);

_BEDITFUNC void SetBlockMSel(int val);

// Изменение размера скроллинга (последний параметр 0)
int BSResize(PANEL *p, int numRows, int l, bool isPartLine);

_BEDITFUNC int ResizeBScrol(PANEL *p, coord x, coord y, int fSizeMoveLoop);

// Расположить скроллинг в заданной позиции экрана
_BEDITFUNC void AdjustBScrol(
                             BSCROL *Bs,   // Открытый скролинг выбора
                             coord x,      // Координаты панели и количество линий
                             coord y,
                             coord l
                            );


_BEDITFUNC bool SetBsUserSort(BSCROL *Bs, const char *sortCond);

void RemoveBsBorder(BSCROL *Bs);

void ClearFileMsg(void);

/**************** Пользовательские фильтры и сортировка *********************/

bool InitBsFiltr(BSCROL *Bs);
void DoneBsFiltr(BSCROL *Bs);

void UseBsFiltr(BSCROL *Bs, int on);
void ApplyBsFiltr(BSCROL *Bs);
void RestoreBsFiltr(BSCROL *Bs);

bool AddBsFiltr(BSCROL *Bs, int num, find_rec_proc ic, void *fd);
void *DelBsFiltr(BSCROL *Bs, int num);
void *GetBsFiltrData(BSCROL *Bs, int num);

bool AddBsFldFiltr(BSCROL *Bs, FIELD *f, int num, void *v, int len);
void DelBsFldFiltr(BSCROL *Bs, int num);
void DelBsFldFiltr2(BSCROL *Bs, int beg, int end);

bool IsBsFldFiltr(BSCROL *Bs);
_BEDITFUNC bool IsBsFiltr(BSCROL *Bs);

void SetBsFldFiltr(BSCROL *Bs);
int  ShowBsFldFiltr(BSCROL *Bs, const char *Invite, const char *Oops);
int  ShowBsFldSorting(BSCROL *Bs);
bool CanSortBsField(BSCROL *Bs, int field);
int  SwitchBsFldSorting(BSCROL *Bs, int field, int sort);

void SetBsFiltrSaveParm(SaveRestParm pSaveRest);

// -----------------------------------------------------------------------------
// Управление видимостью колонки скроллинга (вызывается ИЗ контекста
// BUseScrol - т.е. из обработчика скроллинга)
// [in,out]  Bs   - указатель на скроллинг
// [in]      col  - номер колонки
// [in]      hide - скрыть (true)/показать (false) колонку
// [ret]          - не используется
_BEDITFUNC int HideBsColumn(BSCROL *Bs, int col, bool hide);

// -----------------------------------------------------------------------------
// Спрятать колонку в скроллинге (вызывается ДО BUseScrol)
// [in,out]  Bs   - указатель на скроллинг
// [in]      col  - номер колонки
// [ret]          - не используется
_BEDITFUNC int RemoveBsColumn(BSCROL *Bs, int col);

// -----------------------------------------------------------------------------
// Разрешить/запретить прятать колонки автоскроллингов из контекстного меню EasyWin
// [in,out]  Bs    - указатель на скроллинг
// [in]      col   - номер колонки
// [in]      block - запретить скрывать колонку (true)/разрешить скрывать (false) колонку
// [ret]           - не используется
_BEDITFUNC int BlockHideBsColumn(BSCROL *Bs, int col, bool block);

// -----------------------------------------------------------------------------
// Изменение наименовани колонки автоскроллинга и всплывающей подсказки к нему
// [in,out]  Bs    - указатель на скроллинг
// [in]      col   - номер колонки (видимой в текущем представлении)
// [in]      head  - новое название колонки
// [in]      tips  - новый текст всплывающей подсказки
_BEDITFUNC int SetBsColumnText(BSCROL *Bs, int col, const char *head, const char *tips);

// -----------------------------------------------------------------------------
// Выделение поля заданным стилем
// Вызывается из функции Make.
// [in,out] bs     - указатель на скроллинг
// [in]     n      - номер поля (если < 0 - вся строка)
// [in]     style  - стиль поля (-1 - обнуление всего St)
//                   Сам стиль должен представлять собой результат работы макроса
//                   ReS. Например, для установки стиля SMES параметру style нужно передать
//                   значение вида ReS(SMES, 0).
_BEDITFUNC void SetBsRowStyle(BSCROL *bs, int n, int style);

// -----------------------------------------------------------------------------
// Выделение поля заданным стилем
// В отличие от SetBsRowStyle параметру style можно непосредственно задавать значения стиля
// (SMES, SRMS и т.д.).
// Вызывается из функции Make.
// [in,out] bs     - указатель на скроллинг
// [in]     n      - номер поля (если < 0 - вся строка)
// [in]     style  - стиль поля (-1 - обнуление всего St)
_BEDITFUNC void SetBsRowStyle2(BSCROL *bs, int n, int style);

// -----------------------------------------------------------------------------
// Выделение поля заданным стилем
// В отличие от SetBsRowStyle параметру style можно непосредственно задавать значения стиля
// (SMES, SRMS и т.д.).
// Вызывается из функции Make.
// [in,out] bs     - указатель на скроллинг
// [in]     n      - номер поля (если < 0 - вся строка)
// [in]     style  - стиль поля (-1 - обнуление всего St)
// [in]     color  - цвет фона поля (dBACK_STYLE_*) (используется, если в качестве style
//                   передано значение SUSR)
_BEDITFUNC void SetBsRowStyleEx2(BSCROL *bs, int n, int style, int color);

// -----------------------------------------------------------------------------
// Выделение текста поля заданными цветом и форматом
// Вызывается из функции Make.
// [in,out] bs     - указатель на скроллинг
// [in]     n      - номер поля (если < 0 - вся строка)
// [in]     color  - цвет текста поля (dTEXT_STYLE_*) (если < 0 - компонент не устанавливается)
// [in]     format - формат текста поля (dTEXT_FORMAT_*) (если < 0 - компонент не устанавливается)
_BEDITFUNC void SetBsRowColor(BSCROL *bs, int n, int color, int format);

// -----------------------------------------------------------------------------
// Выделение текста поля заданными цветом, форматом и стилем
// Вызывается из функции Make.
// [in,out] bs     - указатель на скроллинг
// [in]     n      - номер поля (если < 0 - вся строка)
// [in]     color  - цвет текста поля (dTEXT_STYLE_*) (если < 0 - компонент не устанавливается)
// [in]     format - формат текста поля (dTEXT_FORMAT_*) (если < 0 - компонент не устанавливается)
// [in]     style  - цвет фона поля (dBACK_STYLE_*) (если < 0 - компонент не устанавливается)
_BEDITFUNC void SetBsRowColorEx(BSCROL *bs, int n, int color, int format, int style);

// -----------------------------------------------------------------------------
// Задать иконку для поля скроллинга
// Вызывается из функции Make.
// [in,out] bs     - указатель на скроллинг
// [in]     n      - номер поля (если < 0 - вся строка)
// [in]     iconID - идентификатор иконки
_BEDITFUNC void SetBsFieldIcon(BSCROL *bs, int n, int iconID);

// -----------------------------------------------------------------------------
// Задать иконку для поля скроллинга
// Вызывается из функции Make.
// [in,out] bs     - указатель на скроллинг
// [in]     n      - номер поля (если < 0 - вся строка)
// [in]     iconID - идентификатор иконки
// [in]     flags  - флаги отображения иконки
_BEDITFUNC void SetBsFieldIconF(BSCROL *bs, int n, int iconID, int flags);

// -----------------------------------------------------------------------------
// Флаги режимов для SetBsSizeMode
#define  dSBSM_No          0    //
#define  dSBSM_Maximized   1    // Вписать скроллинг в границы клиентской области главного
                                // окна приложения

// -----------------------------------------------------------------------------
// Задать режим изменения размера автоскроллинга
// [in,out] bs   - указатель на скроллинг
// [in]     mode - режим (см. константы вида dSBSM_*)
_BEDITFUNC void SetBsSizeMode(BSCROL *bs, int mode);


/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __BEDIT_H

/* <-- EOF --> */