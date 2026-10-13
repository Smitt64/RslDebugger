// -------------------------------------------------------------------------------------------------

#ifndef __MSEL_H
#define __MSEL_H

#include "listobj.h"

// -----------------------------------------------------------------------------
// как проверять конкурентные изменения
#define  MSEL_CHECK_NO                0  // никак
#define  MSEL_CHECK_CRC32             1  // crc32
#define  MSEL_CHECK_SAVEB             2  // буфер записи

// уровень проверки по умолчанию
#define  MSEL_DEFAULT_CHECK_LEVEL     MSEL_CHECK_SAVEB

// сообщения, передаваемые в польз. функцию прохода списка
#define  MSEL_ACTION_STARTWALK        1  // начало обхода
#define  MSEL_ACTION_PROCESSRECORD    2  // обработка записи
#define  MSEL_ACTION_ENDWALK          3  // конец обхода


// что делать после обработки записи
#define  MSEL_WALK_CONTINUE_KEEPSEL   1  // идти дальше, сохранить выделение
#define  MSEL_WALK_CONTINUE_CLEARSEL  2  // идти дальше, снять выделение
#define  MSEL_WALK_STOP_KEEPSEL       3  // прервать, сохранить выделение
#define  MSEL_WALK_STOP_CLEARSEL      4  // прервать, снять выделение
#define  MSEL_WALK_STOP_CLEARALLSEL   5  // прервать, снять выделение со всех

// параметры выбора (битовая маска - long)
#define  MSEL_OPT_NOINVERSION         1  // запрет выбора с инверсией
#define  MSEL_OPT_NODELETE            2  // запрет системного массового удаления
#define  MSEL_OPT_FILEKEYWALK         4  // проход списка по ключу всегда идет по файлу
#define  MSEL_OPT_PROCESSERR          8  // вызов действия в случае ошибки чтения записи

// параметры по умолчанию
#define  MSEL_DEFAULT_OPTIONS         0

// -----------------------------------------------
typedef struct tagMSEL_COUNT_PARM
      {
       db_int32   num;
       db_int32   err;
       db_int32   partly;
       db_int32   all;  
       PANEL     *Pn;
       char      *Pstatus;
       char      *Phead;
      } MSEL_COUNT_PARM;

// -----------------------------------------------
// Структура параметров массового выбора
typedef struct tagMULTISELECT
      {
       int      Disabled;         // массовый выбор отключен
       int      SaveBufCheck;     // уровень проверки конкурентных изменений
       int      AutoDestruct;     // автоматически прибить при выходе из скроллинга
       long     Options;          // набор параметров (MSEL_OPT_*)

       int      Invert;           // инверсия выборки
       int      Running;          // флаг выполнения массовой процедуры
       char    *log;              // строка с набором сообщений о ходе массовой процедуры

       CNTLIST  RecList;          // список выбранных записей
       CNTLIST  FunList;          // список зарегистрированных действий

       int               BlockAction;  // массовая обработка заблокирована
       const char       *BlockMessage; // текст сообщения, запрещающего массовую обработку

       int               LockSection;  // счетчик блокировок для предотвращения частого обновления секции

       MSEL_COUNT_PARM  *counter;
      } MULTISELECT;

// -----------------------------------------------
// Структура системных параметров, передаваемая в пользовательскую функцию
// массовой обработки запсей
typedef struct tagMSEL_KEYACTIONPARM
      {
       int  error;   // ошибка при проходе списка; реально пока только 0 или 70
                     // (запись конкурентно изменена). Что делать с этой ошибкой
                     // (игнорировать, прервать работу) - решает программист
       int  key;     // собственно клавиша, которую обрабатываем.
      } MSEL_KEYACTIONPARM;

#endif

/* <-- EOF --> */