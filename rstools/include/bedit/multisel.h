#ifndef __MULTISEL_H
#define __MULTISEL_H

#include "bedit.h"
#include "msel.h"

#ifdef __cplusplus
 extern "C" {
#endif

// -----------------------------------------------------------------------------
// Пользовательский обработчик массовых операций #3044
typedef int (*TSelCallback)(BSCROL *);

// прототип пользовательской функции массовой обработки.
typedef int (*msel_iterator_t)(PANEL *, int, MSEL_KEYACTIONPARM *, void *);

// -----------------------------------------------
// структура, содержащая параметры выбранной записи
typedef struct tagMSEL_RECORD
      {
       LOBJ     node;    // для работы списка

       recpos_t RecPos;  // позиция выбранной записи в файле
       long     CRC;     // CRC выбранной записи (если нужно)
       void    *RecBuf;  // буфер выбранной записи (если нужно)
       int      RecLen;  // длина буфера
       void    *KeyBuf;  // ьуфер ключа выбранной записи
       int      KeyLen;  // длина ключевого буфера
      } MSEL_RECORD;

// -----------------------------------------------
// структура, содержащая параметры зарегистрированного обработчика
typedef struct tagMSEL_ACTION
      {
       LOBJ             node;        // для работы списка

       int              key;         // клавиша, которую обрабатываем
       int              KeyWalk;     // проход по ключу (если !0) или в порядке выбора
                                     // записей (если 0)
       msel_iterator_t  func;        // пользовательская функция массовой обработки
       void            *userparm;    // пользовательские параметры
      } MSEL_ACTION;


// Создание структуры массового выбора. Нужно в том случае, если не устраивает
// то, что создается по умолчанию, либо если нужно хранить список выбора после
// выхода из скроллинга и т.п.
_BEDITFUNC int  MSelCreate(
                           MULTISELECT  **lpSel ,        // указатель на указатель на структуру
                           int            AutoDestruct,  // автоматически убить при выходе из скроллинга
                           int            SaveBufChk ,   // уровень проверки конкурентных изменений
                           long           Options        // набор параметров (MSEL_OPT_*)
                          );

// Создание структуры массового выбора с параметрами по умолчанию
_BEDITFUNC  int  MSelCreateDefault(MULTISELECT **lpSel);

// Отключение массового выбора для скроллинга
_BEDITFUNC int  MSelDisable(BSCROL *Bs);

// Не ноль, если выполняется массовая процедура
_BEDITFUNC int  MSelIsRunning(BSCROL *Bs);

// Очистка структуры массового выбора. Структура не освобождается (на случай,
// если она выделена на стеке или вообще статическая)
_BEDITFUNC int  MSelClear(MULTISELECT **lpSel);

// Уничтожение структуры массового выбора (с освобождением памяти)
_BEDITFUNC int  MSelDestroy(MULTISELECT **lpSel);

// Регистрация массового действия 
_BEDITFUNC int  MSelAddKeyAction(
                                 BSCROL         *Bs,       // Скроллинг
                                 int             key,      // Клавиша, на которую вешаем обработчик
                                 int             keywalk,  // Необходимость прохода списка по ключу
                                 msel_iterator_t func,     // Пользовательская процедура массовой обработки
                                 void           *parm      // Пользовательские параметры
                                );

// Make для массового выбора. Используется только в инструменте.
_BEDITFUNC int  MSelMake(BSCROL *Bs);

// Обработчик клавиатуры массового выбора. Используется только в инструменте.
_BEDITFUNC int  MSelProc(PANEL *p, int key);

// Запихивание сообщения в лог работы процедуры массового выбора
_BEDITFUNC void MSelAddMessageToLog(BSCROL *Bs, char *str);

// Регистрация системных массовых действий по умолчанию. Используется только в инструменте.
_BEDITFUNC void MSelAddDefaultKeys(BSCROL *Bs);


_BEDITFUNC void MSelDeselectAll(BSCROL *Bs);
_BEDITFUNC int  MSelListWalk(PANEL *p, msel_iterator_t func, void *userparm, int key);
_BEDITFUNC int  MSelKeyWalk(PANEL *p, msel_iterator_t func, void *userparm, int key);
_BEDITFUNC int  MSelInvertSelection(BSCROL *Bs);
_BEDITFUNC int  MSelSelectAll(BSCROL *Bs , int strict);
_BEDITFUNC int  MSelAddRecord(BSCROL *Bs);

_BEDITFUNC int  MSelRemoveNode(BSCROL *Bs, MSEL_RECORD *node);   
_BEDITFUNC int  MSelRefreshCRC(BSCROL *Bs, MSEL_RECORD *node);

_BEDITFUNC MSEL_RECORD *MSelIsInList(BSCROL *Bs, recpos_t RecPos);


// проверка, есть ли в скроллинге хотя бы одна выделенная запись
_BEDITFUNC int  MSelAnyRecordsInSelection(BSCROL *Bs);

// Установка пользовательского обработчика. Возвращает указатель на предыдущий обработчик.
_BEDITFUNC TSelCallback MSelSetCallback(TSelCallback SelClbk);

// блокировка массовых действий
void MSelBlockAction(BSCROL *Bs, int BlockAction, const char *BlockMessage);

void MSelMouse(BSCROL *Bs, int oldcr);

void MSelLockSection(BSCROL *Bs);
void MSelUnlockSection(BSCROL *Bs);

void MSelAddCounter(BSCROL *Bs, int num, int err, int partly);

#ifdef __cplusplus
 }
#endif

#endif

/* <-- EOF --> */