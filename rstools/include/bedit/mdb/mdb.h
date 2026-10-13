
/*
**  Простейший менеджер записей в оперативной памяти
**
**  Реализует операции:
**  - вставки/удаления/обновления записи
**  - поиска записи по ключу
**  - последовательного перебора записей по индексу
*/

#ifndef __MDB_H
#define __MDB_H

#include "listobj.h"
#include "nwbtrv.h"
#include "bexpdef.h"

#ifdef MDB_USE_INDEX_MAPS
  #include <map>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Константы
// =========
enum
   {
    // Максимальные значения
    MAX_IDX            = 25,         // Максимальное количество индексов в наборе записей
    MAX_SEG            = 15,         // Максимальное количество сегментов в индексе

    // Предопределенные значения для смещений
    MDBO_PUSHBACK      = -1,         // Добавить поле после последнего существующего

    // Ошибки
    MDBE_OK            = 0,          // Все хорошо
    MDBE_NO_REC        = BEnorec,    // Нет записи, удовлетворяющей
                                     // значению ключа
    MDBE_EOF           = BEeof,      // Достигнут конец файла
    MDBE_DUP_KEY       = BEdupkey,   // Дублирование значения уникального ключа
    MDBE_BAD_POS       = BEinpos,    // Неверная текущая позиция
    MDBE_BAD_KEY       = BEinvkey,   // Неверный номер ключа
    MDBE_BAD_REC_SIZE  = BEldata,    // Некорректная длина записи
    MDBE_INDEX_CHANGED = BEdifkey,   // Изменился номер ключа
    MDBE_NO_MEMORY     = -1,         // Нехватка памяти
    MDBE_OVERFLOW      = -2,         // Переполнение при добавлении полей, сегментов etc.
    MDBE_WRONGPARAM    = -3,         // Неверный параметр
    MDBE_MULTIPLEAI    = -4,         // Больше одного автоинкрементного поля
    MDBE_BADAIFLD      = -5          // Попытка сделать автоинкрементным несуществующее
                                     // поле или поле не целого типа
   };

// Типы
// ====

// -----------------------------------------------
// Флаги записи
typedef enum
      {
       MDBRF_NO_FLAGS     = 0,
       MDBRF_WAS_UPDATED  = 0x01,    // Была обновлена с момента вставки
       MDBRF_DELETED      = 0x02,    // Удаленная запись
       MDBRF_KEEP         = 0x04,    // При удалении помечать запись как удаленную, а не удалять физически
                                     // ( для нужд синхронизации )
       MDBRF_SAVE_ORIG    = 0x08,    // Сохранить в поле оrig значение данных на момент вставки
       MDBRF_KEEP_AUTOINC = 0x10,    // Сохранить значение (ненулевое) автоинкрементного поля при синхронизации
       MDBRF_SYNCHRONIZED = 0x20     // Запись прошла синхронизацию с таблицей БД
      } MDB_REC_FLAGS;

// -----------------------------------------------
// Флаги индекса
typedef enum
      {
       MDBIF_UNIQUE = 1         // Уникальный ( недублируемый ) ключ
      } MDB_IDX_FLAGS;

// -----------------------------------------------
// Флаги сегмента ключа
typedef enum
      {
       MDBSF_DESCEND = 1        // Обратный порядок сортировки
      } MDB_SEG_FLAGS;

// -----------------------------------------------
// типы полей
typedef enum
      {
       MDBFT_UNKNOWN = -1,
       MDBFT_INT     = 1,    // Целое
       MDBFT_FLOAT,          // Число с плавающей точкой

       #if !defined(NUMERIC_AS_MONEY)
           MDBFT_MONEY,      // Деньги
       #else
           MDBFT_DUMMY3,
       #endif

       MDBFT_CHAR,           // Символ
       MDBFT_STRING,         // Строка
       MDBFT_DATE,           // Дата в формате bdate
       MDBFT_TIME,           // Время в формате btime
       MDBFT_AUTOINC,        // Автоинкремент (целое)

       #if defined(USE_NUMERIC)
           MDBFT_NUMERIC,    // Число типа Numeric
       #endif

       #if defined(NUMERIC_AS_MONEY)
           MDBFT_MONEY = MDBFT_NUMERIC
       #endif
      } MDB_FLD_TYPE;


// -----------------------------------------------
struct tagMDB_TEMPLATE;

// -----------------------------------------------
// Одна запись
typedef struct
      {
       unsigned       flags;           // Флаги
       LOBJ           iLinks[MAX_IDX]; // iLinks[i] определяет положение
                                      // записи в i-той индекной послед-сти
       void          *data;            // Собственно данные
       void          *orig;            // Значение данных на момент
                                      // вставки записи (для нужд синхр. )
       unsigned long  id;              // Уникальный в пределах набора идентификатор записи
       void          *aux;             // Дополнительные метаданные
       unsigned       recSize;         // Длина записи
       unsigned       origRecSize;     // Изначальная длина записи
      } MDB_REC;

// -----------------------------------------------------------------------------
#ifdef MDB_USE_INDEX_MAPS
    // Элемент индекса
    typedef struct tagMDB_INDEX_ENTRY
          {
           unsigned                 indexNum;   // Номер индекса
           void                    *data;       // Данные записи
           unsigned                 number;     // Номер среди записей с одинаковым значением ключа
           struct tagMDB_TEMPLATE  *templ;      // Шаблон

           tagMDB_INDEX_ENTRY() : indexNum(0), data(0), number(0), templ(0) {}

           tagMDB_INDEX_ENTRY(struct tagMDB_TEMPLATE *t, int idxNum, void *d, int n = 0)
                             : indexNum(idxNum), data(d), number(n), templ(t) {}

           bool operator <  (const tagMDB_INDEX_ENTRY &entry) const;
           bool operator == (const tagMDB_INDEX_ENTRY &entry) const;
          } MDB_INDEX_ENTRY;

    // Индексное дерево

    typedef std::map<MDB_INDEX_ENTRY, MDB_REC *, std::less<MDB_INDEX_ENTRY>>  MDB_INDEX_MAP;
    typedef MDB_INDEX_MAP::iterator                                           MDB_INDEX_MAP_POS;
#else
    typedef int MDB_INDEX_MAP_POS;
#endif

// -----------------------------------------------
// Поле
typedef struct
      {
       MDB_FLD_TYPE  type;         // Тип поля
       unsigned      offset;       // Смещение поля от начала данных ( байт )
       unsigned      size;         // Длина поля ( байт )
      } MDB_FIELD;

// -----------------------------------------------
// Сегмент индекса
typedef struct
      {
       unsigned      flags;        // Флаги
       unsigned      fldNo;        // Номер поля
      } MDB_IDX_SEG;

// -----------------------------------------------
// Индекс
typedef struct
      {
       unsigned      flags;        // Флаги
       unsigned      nSegs;        // Число сегментов в индексе
       MDB_IDX_SEG   segments[MAX_SEG];  // Сегменты
      } MDB_INDEX;

// -----------------------------------------------
// Шаблон набора записей
// Представляет собой "тип" набора - определяет длину записи и индексы
typedef struct tagMDB_TEMPLATE
      {
       unsigned      recSize;          // Размер записи
       unsigned      nFields;          // Количество полей
       MDB_FIELD    *fields;           // Поля
       unsigned      nIndexes;         // Количество индексов
       MDB_INDEX    *indexes[MAX_IDX]; // Индексы
       char         *fldNames;         // Имена полей, разделенные символом '\x00'
       void         *aux;              // Дополнительные метаданные
       unsigned      maxRecSize;       // Максимальная длина записи
       long          autoIncFld;       // Номер автоинкрементного поля (-1 - нет такого поля)
       long          autoIncStep;      // Шаг автоинкремента
       bool          useIndexMaps;     // Использовать деревья (вернее, ассоциативные массивы std::map) для
                                       // индексных последовательностей
       unsigned      primaryKey;       // Информация о первичном ключе в МЗП
      } MDB_TEMPLATE;

// -----------------------------------------------
// Набор записей
typedef struct
      {
       MDB_TEMPLATE      *templ;           // Шаблон
       CNTLIST            iLists[MAX_IDX]; // Индексные последовательности (списки)
       unsigned long      maxRecId;        // Макс. идентификатор записи
       void              *aux;             // Дополнительные метаданные
       long               maxAutoIncVal;   // Максимальное ( по абс. величине ) значение автоинкрементного поля
       bool               deleteTempl;     // Удалять template при закрытии mdb-файла
       void              *pMdbFile;        // MDB file handle

       #ifdef MDB_USE_INDEX_MAPS
           MDB_INDEX_MAP  iMaps[MAX_IDX];  // Индексные последовательности (деревья)
       #endif
      } MDB_RECORDSET;

// -----------------------------------------------
// Курсор
typedef struct
      {
       MDB_RECORDSET  *recordset;        // Набор записей
       unsigned        curIdx;           // Текущий индекс
       MDB_REC        *curPos;           // Текущая позиция
       MDB_REC        *prevPos;          // Предыдущая позиция. Валидна только после удаления текущей записи!!!
       MDB_REC        *nextPos;          // Следующая позиция. Валидна только после удаления текущей записи!!!
       MDB_REC        *prevDistinctPos;  // Предыдущая позиция с отличным значением ключа. Валидна только после
                                         // удаления текущей записи!!!
       MDB_REC        *nextDistinctPos;  // Следующая позиция с отличным значением ключа. Валидна только после
                                         // удаления текущей записи!!!
       void           *recBuf;           // Буфер записи
       unsigned        recSize;          // Длина записи
       bool            ignoreDelFlag;    // Игнорировать флаг удаления

       #ifdef MDB_USE_INDEX_MAPS
           MDB_INDEX_MAP_POS  imCurPos;
           MDB_INDEX_MAP_POS  imPrevPos;          // Предыдущая позиция. Валидна только после удаления текущей записи!!!
           MDB_INDEX_MAP_POS  imNextPos;          // Следующая позиция. Валидна только после удаления текущей записи!!!
           MDB_INDEX_MAP_POS  imPrevDistinctPos;  // Предыдущая позиция с отличным значением ключа. Валидна только
                                                  // после удаления текущей записи!!!
           MDB_INDEX_MAP_POS  imNextDistinctPos;  // Следующая позиция с отличным значением ключа. Валидна только
                                                  // после удаления текущей записи!!!
       #endif
      } MDB_CURSOR;


// -----------------------------------------------------------------------------
typedef void (*MDB_DONE_AUX_CALLBACK)(void *);

// Функции для внутренних нужд
// ===========================

// Получение значения автоинкрементного поля
_BTEXP long mdbGetAutoIncValue(MDB_TEMPLATE *templ, void *rec);

// Установка значения автоинкрементного поля
_BTEXP void mdbSetAutoIncValue(MDB_TEMPLATE *templ, void *rec, long val);

// Сравнение значений ключа в двух буферах записей
int mdbCompareKeys(const void *rec1, const void *rec2, MDB_TEMPLATE *templ, MDB_INDEX *index);

// Обработать явное присвоение автоинкремента
_BTEXP void mdbHandleExplicitAutoIncAssignment(MDB_RECORDSET *recordset, long val);

// Физичкское удаление записи по переданному указателю
void mdbDeleteRaw(MDB_RECORDSET *recordset, MDB_REC *rec, MDB_DONE_AUX_CALLBACK doneRecAux);

// Функции для общего пользования
// ==============================

// Вставить запись ( c флагами и метаданными )
_BTEXP int mdbInsertEx(MDB_CURSOR *c, unsigned flags, void *aux);

// Вставить запись
_BTEXP int mdbInsert(MDB_CURSOR *c);

// Удалить запись с метаданными
_BTEXP int mdbDeleteEx(MDB_CURSOR *c, MDB_DONE_AUX_CALLBACK doneRecAux);

// Удалить запись
_BTEXP int mdbDelete(MDB_CURSOR *c);

// Обновить запись
_BTEXP int mdbUpdate(MDB_CURSOR *c);

// Встать на первую запись по текущему ключу
_BTEXP int mdbFirst(MDB_CURSOR *c);

// Встать на последнюю запись по текущему ключу
_BTEXP int mdbLast(MDB_CURSOR *c);

// Получить текущую позицию
_BTEXP void *mdbGetPos(MDB_CURSOR *c);

// Установить текущую позицию ( pos ДОЛЖНА быть предварительно получена
// при помощи вызова mdbGetPos()
_BTEXP void mdbSetPos(MDB_CURSOR *c, void *pos);

// Взять следующую запись по ключу ( расширенная )
_BTEXP int mdbNextEx(MDB_CURSOR *c, bool distinctKeyVal);

// Взять следующую запись по ключу
_BTEXP int mdbNext(MDB_CURSOR *c);

// Взять предыдущую запись по ключу ( расширенная )
_BTEXP int mdbPrevEx(MDB_CURSOR *c, bool distinctKeyVal);

// Взять предыдущую запись по ключу
_BTEXP int mdbPrev(MDB_CURSOR *c);

// Взять запись, большую по ключу, чем текущий буфер записи
_BTEXP int mdbGetGT(MDB_CURSOR *c);

// Взять запись, большую или равную по ключу, чем текущий буфер записи
_BTEXP int mdbGetGE(MDB_CURSOR *c);

// Взять запись, меньшую по ключу, чем текущий буфер записи
_BTEXP int mdbGetLT(MDB_CURSOR *c);

// Взять запись, меньшую или равную по ключу, чем текущий буфер записи
_BTEXP int mdbGetLE(MDB_CURSOR *c);

// Получить запись, равную по ключу текущему буферу
_BTEXP int mdbGetEQ(MDB_CURSOR *c);

// Получить идентификатор записи
_BTEXP unsigned long mdbGetId(MDB_CURSOR *c);

// Найти запись по идентификатору
_BTEXP int mdbGetById(MDB_CURSOR *c, unsigned long id);

// Получить текущий номер ключа
_BTEXP unsigned mdbGetKeyNum(MDB_CURSOR *c);

// Установить текущий номер ключа
_BTEXP int mdbSetKeyNum(MDB_CURSOR *c, unsigned keyNum);

// Создать шаблон
_BTEXP MDB_TEMPLATE *mdbCreateTemplate(unsigned recSize);

// Создать шаблон с записями переменной длины
_BTEXP MDB_TEMPLATE *mdbCreateTemplateVar(unsigned recSize, unsigned maxRecSize);

// Создать шаблон ( с доп. метаданными )
_BTEXP MDB_TEMPLATE *mdbCreateTemplateEx(unsigned recSize, unsigned maxRecSize, void *aux);

// Удалить шаблон
_BTEXP void mdbDeleteTemplate(MDB_TEMPLATE *templ);

// Удалить шаблон ( с доп. метаданными )
_BTEXP void mdbDeleteTemplateEx(MDB_TEMPLATE *templ, MDB_DONE_AUX_CALLBACK doneAux);

// Добавить поле ( если offset == MDBO_PUSHBACK, добавить поле после последнего существ. )
_BTEXP int mdbAddField(MDB_TEMPLATE *templ, char *name, MDB_FLD_TYPE type, unsigned size, unsigned offset);

// Установить автоинкрементное поле (для генерации структуры из словаря)
_BTEXP int mdbSetAutoIncField(MDB_TEMPLATE *templ, unsigned fldNo);

// Установить шаг автоинкремента
_BTEXP void mdbSetAutoIncStep(MDB_TEMPLATE *templ, long step);

// Установить флажок использования древовидных индексов
_BTEXP void mdbUseIndexMaps(MDB_TEMPLATE *templ, bool b);

// Добавить индекс
_BTEXP MDB_INDEX *mdbAddIndex(MDB_TEMPLATE *templ, unsigned flags);

// Добавить сегмент индекса
_BTEXP int mdbAddSegment(MDB_INDEX *idx, unsigned fldNo, unsigned flags);

// Создать набор записей ( с дополнительными метаданными и процедурой обработки сообщений )
_BTEXP MDB_RECORDSET *mdbCreateRecordsetEx(MDB_TEMPLATE *templ, void *aux);

// Создать набор записей
_BTEXP MDB_RECORDSET *mdbCreateRecordset(MDB_TEMPLATE *templ);

// Очистить набор записей ( с удалением доп. метаданных )
_BTEXP int mdbClearRecordsetEx(MDB_RECORDSET *recordset, MDB_DONE_AUX_CALLBACK doneRecAux);

// Очистить набор записей
_BTEXP int mdbClearRecordset(MDB_RECORDSET *recordset);

// Удалить набор записей ( с удалением доп. метаданных )
_BTEXP void mdbDeleteRecordsetEx(MDB_RECORDSET *recordset, MDB_DONE_AUX_CALLBACK doneRecAux,
                                 MDB_DONE_AUX_CALLBACK doneRecordsetAux);

// Удалить набор записей
_BTEXP void mdbDeleteRecordset(MDB_RECORDSET *recordset);

// Создать курсор
_BTEXP MDB_CURSOR *mdbCreateCursor(MDB_RECORDSET *recordset);

// Удалить курсор
_BTEXP void mdbDeleteCursor(MDB_CURSOR *c);

// Получить ( указатель на ) значение, тип и размер поля по его номеру
void *mdbFieldByNum(MDB_CURSOR *c, unsigned num, MDB_FLD_TYPE *type, unsigned *size);

// Получить ( указатель на ) значение, тип и размер поля по его имени
void *mdbFieldByName(MDB_CURSOR *c, char *name, MDB_FLD_TYPE *type, unsigned *size);

// Установить значение флага игнорирования флага удаления
_BTEXP bool mdbSetIgnoreDelFlag(MDB_CURSOR *c, bool ignoreDelFlag);

// Получить длину текущей записи
_BTEXP unsigned mdbGetRecordSize(MDB_CURSOR *c);

// Установить длину текущей записи
_BTEXP int mdbSetRecordSize(MDB_CURSOR *c, unsigned size);

// Получить длину записи по умолчанию
_BTEXP unsigned mdbGetDefaultRecSize(MDB_CURSOR *c);

// Установить длину записи по умолчанию
_BTEXP void mdbSetDefaultRecSize(MDB_CURSOR *c);

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */