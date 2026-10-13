
/*
**  Кэширование части файла БД в наборе записей менеджера записей в
**  оперативной памяти (МЗП)
**  Реализует операции:
**  - прекэширование части файла БД в МЗП
**  - синхронизация набора записей МЗП с файлом БД
*/

#ifndef __BMDB_H
#define __BMDB_H

#include "btrv.h"
#include "mdb/mdb.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Константы
enum
   {
    BMDB_TEMPL_CACHE_WATERLINE =   50,  // Ватерлиния LRU-кэша шаблонов

    BMDB_SKIP_REC              =   -3,  // Пропустить запись при прекэшир./синхр.
    BMDB_ABORT                 =   -4,  // Прервать прекэширование
    BMDB_UPDATE_REC            =   -5,  // Буфер записи изменен, требуется обновление

    BMDBE_SYNC_FAILED          = -101,  // Облом синхронизации
    BMDBE_ALREADY_ASSIGNED     = -102,  // С данным набором записей уже ассоциирован некоторый Btrieve-файл
    BMDBE_NO_FILE_ASSIGNED     = -103,  // С данным набором записей не ассоциирован Btrieve-файл
    BMDBE_PRECACHE_FAILED      = -104   // Прекэширование обломилось
   };

// -----------------------------------------------------------------------------
// Расширенные ошибки для сообщения BMDB_SYNC_FAIL.
// Возвращаются функцией mdbGetState.
enum
   {
    BMDBE_SF_OK                =  0,    // Успех
    BMDBE_SF_NO_FILE_ASSIGNED,          // Аналог BMDBE_NO_FILE_ASSIGNED
    BMDBE_SF_SYNC_BEGIN,                // Для ошибки BMDB_SYNC_BEGIN
    BMDBE_SF_MDB_CREATE_CURSOR,         // Ошибка вызова mdbCreateCursor
    BMDBE_SF_MDB_GETPOS,                // Ошибка вызова mdbGetPos
    BMDBE_SF_MDB_NEXT,                  // Ошибка вызова mdbNext
    BMDBE_SF_BF_GDP,                    // Ошибка вызова bfGDp
    BMDBE_SF_CONCURENT,                 // Запись конкурентно изменена
    BMDBE_SF_BEFORE_UPD,                // Ошибка вызова BMDB_SYNC_BEFORE_UPD
    BMDBE_SF_BF_UPDATE,                 // Ошибка вызова bfOp(Bupdate)
    BMDBE_SF_AFTER_UPD,                 // Ошибка вызова BMDB_SYNC_AFTER_UPD
    BMDBE_SF_BEFORE_DEL,                // Ошибка вызова BMDB_SYNC_BEFORE_DEL
    BMDBE_SF_BF_DELETE,                 // Ошибка вызова bfOp(Bdelete)
    BMDBE_SF_AFTER_DEL,                 // Ошибка вызова BMDB_SYNC_AFTER_DEL
    BMDBE_SF_NO_ACTION,                 // Ошибка вызова BMDB_SYNC_NO_ACTION
    BMDBE_SF_BEFORE_INS,                // Ошибка вызова BMDB_SYNC_BEFORE_INS
    BMDBE_SF_BF_INSERT,                 // Ошибка вызова bfOp(Binsert)
    BMDBE_SF_BF_GP,                     // Ошибка вызова bfGP
    BMDBE_SF_AFTER_INS,                 // Ошибка вызова BMDB_SYNC_AFTER_INS
    BMDBE_SF_MDB_UPDATE,                // Ошибка вызова mdbUpdate
    BMDBE_SF_BF_ERROR,                  // Общая ошибка DBT
    BMDBE_SF_MDB_ERROR                  // Общая ошибка MDB
   };

// -----------------------------------------------------------------------------
// Сообщения пользовательских функций прекэширования и синхронизации
enum
   {
    BMDB_PRECACHE_BEGIN  = 1,   // Начало прекэширования
    BMDB_PRECACHE_END,          // Конец прекэширования
    BMDB_PRECACHE_FAIL,         // Облом прекэширования
    BMDB_PRECACHE_BEFORE_INS,   // Перед вставкой записи при прекэшировании
    BMDB_PRECACHE_AFTER_INS,    // После вставки записи при прекэшировании

    BMDB_SYNC_BEGIN      = 1,   // Начало синхронизации
    BMDB_SYNC_END,              // Конец синхронизации
    BMDB_SYNC_FAIL,             // Облом синхронизации
    BMDB_SYNC_BEFORE_INS,       // Перед вставкой записи при синхронизации
    BMDB_SYNC_AFTER_INS,        // После вставки записи при синхронизации
    BMDB_SYNC_BEFORE_UPD,       // Перед обновлением записи при синхронизации
    BMDB_SYNC_AFTER_UPD,        // После обновления записи при синхронизации
    BMDB_SYNC_BEFORE_DEL,       // Перед удалением записи при синхронизации
    BMDB_SYNC_AFTER_DEL,        // После удаления записи при синхронизации
    BMDB_SYNC_NO_ACTION         // С данной записью действий не производилось
   };

// Дополнительные метаданные записи для синхронизации с файлом БД
typedef struct
      {
       recpos_t  recPos;      // Физ. позиция соответствующей записи в Btrieve-файле
      } BMDB_REC_AUX;

// Дополнительные метаданные набора записей для синхронизации с файлом БД
typedef struct
      {
       BTRVFILE  *bFile;      // Указатель на файловую переменную файла БД
      } BMDB_RECORDSET_AUX;

// Дополнительные метаданные шаблона, закачанного из словаря
// размер = sizeof ( BMDB_TEMPLATE_AUX ) + strlen( dicPathname );
typedef struct
      {
       char  name[30];        // Имя структуры
       char  dicPathname[1];  // Имя словаря
      } BMDB_TEMPLATE_AUX;

// -----------------------------------------------------------------------------
// Получить указатель на Btrieve-файл, ассоциированный с данным набором
#define  bmdbAssignedFile(recordset)  ((recordset)->aux ? ((BMDB_RECORDSET_AUX *)((recordset)->aux))->bFile : NULL)

// Найти запись в наборе по физ. позиции первичной Btrieve-записи
MDB_REC *bmdbGetByPosRaw(MDB_RECORDSET *recordset, recpos_t recPos);

// Назначить набору записей ассоциированный Btrieve-файл
int bmdbAssign(MDB_RECORDSET *recordset, BTRVFILE *bf);

// Отсоединить ассоциированный Btrieve-файл от набора записей
int bmdbUnassign(MDB_RECORDSET *recordset);

// Закачать в набор записи из текущей области видимости Btrieve-файла
// Если флаг merge взведен, при вставке записи производится проверка
// на дублирование Btrieve-позиции
// proc - процедура пользователя, params - данные для нее
typedef int (*BMDB_PRECACHE_CALLBACK)(int msg, void *data, void *params);

int bmdbPrecacheEx(MDB_RECORDSET *recordset, bool merge, BMDB_PRECACHE_CALLBACK proc, void *params);

// Закачать в набор записи из текущей области видимости Btrieve-файла
// Если флаг merge взведен, при вставке записи производится проверка
// на дублирование Btrieve-позиции
int bmdbPrecache(MDB_RECORDSET *recordset, bool merge);

// Синхронизировать набор записей с текущей областью видимости Btrieve-файла
// Если noExternalInsertionCheck взведен, не выполнять проверку (I)
// proc - процедура пользователя, params - данные для нее
typedef int (*BMDB_SYNC_CALLBACK)(int msg, void *data, void *params);

int bmdbSyncEx(MDB_RECORDSET *recordset, bool noExternalInsertionCheck, BMDB_SYNC_CALLBACK proc, void *params);

int bmdbSyncEx2(MDB_RECORDSET *recordset, bool noExternalInsertionCheck, BMDB_SYNC_CALLBACK proc, void *params, bool UseFilters);

// Синхронизировать набор записей с текущей областью видимости Btrieve-файла
// Если noExternalInsertionCheck взведен, не выполнять проверку (I)
int bmdbSync(MDB_RECORDSET *recordset, bool noExternalInsertionCheck);

// Подтвердить успешное завершение транзакции,
// в рамках которой проводилась синхронизация
int bmdbConfirmSync(MDB_RECORDSET *recordset);
int bmdbConfirmSyncEx(MDB_RECORDSET *recordset, bool UseFilters);

// Создать шаблон по структуре из словаря
_BTEXP MDB_TEMPLATE *bmdbCreateTemplateFromDic(char *dicPathname, char *name);

// Удалить шаблон, закачанный из словаря
_BTEXP void bmdbDeleteTemplate(MDB_TEMPLATE *templ);

// Очистка набора записей
int bmdbClearRecordset(MDB_RECORDSET *recordset);

// Удаление набора записей
void bmdbDeleteRecordset(MDB_RECORDSET *recordset);

// Инициализировать кэш шаблонов
int  bmdbInitTemplateCache(void);

// Деинициализировать кэш шаблонов
void bmdbDoneTemplateCache(void);

// Сравнить два значения
int mdbCompare(const void *a, const void *b, MDB_FLD_TYPE type, unsigned size);

// Дамп первичного ключа записи
_BTEXP void mdbTraceDumpPrimaryKey(const MDB_TEMPLATE *templ, const void *recData);

// Дамп записи
_BTEXP void mdbTraceDumpRecord(const MDB_TEMPLATE *templ, const void *recData, unsigned recLen,
                               const void *oldRecData, unsigned oldRecLen, bool changedOnly);

// -----------------------------------------------------------------------------
// Получить расширенный статус ошибки для сообщения BMDBE_SYNC_FAILED
// [out] stat - Возвращает статус операции с mdb и dbt-файлами. Необязательный
//              параметр.
// [ret]      - Возвращает расширенный код ошибки из BMDBE_SF_.
_BTEXP int mdbGetState(int *stat);

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */