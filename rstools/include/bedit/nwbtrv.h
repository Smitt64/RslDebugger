/*-----------------------------------------------------------------------*-
 File Name   : nwbtrv.h

 Sybsystem   : Btrieve
 Description : Содержит интерфес с Btrieve для DOS, DPMI16, DPMI32,
               Windows

               Для DPMI16 и DPMI32 нельзя использовать команду
               Btrieve Bstop !!!

 Source      : nwbtrv.c

 Library(s)  :

 Copyright (c) 1991 - 1994 by R-Style JSC
 All Rights Reserved.

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/bedit/h/nwbtrv.h $
                  $Revision: 34 $
                  $Date: 8.06.00 16:37 $
                  $Author: Kubrin $
                  $Modtime: 8.06.00 16:37 $

-*- History -------------------------------------------------------------*-
    January 6,1995   Sergei Kubrin (K78) - Create file
    February 13,1995   Serg Kubrin (K78)
      Поддержка Windows NT

   09/01/1997 Yakov E. Markovitch (JNS)
    Multi-client support added.
   13/02/1997 Yakov E. Markovitch (JNS)
    btrvNuke & bfStop added.
   20/06/1997 Yakov E. Markovitch (JNS)
    Codes of extended operations added to Btrieve_Operations.

-*-----------------------------------------------------------------------*/

#include "platform.h"

#ifndef __NWBTRV_H
#define __NWBTRV_H

#include "bexpdef.h"

#include <bdate.h>
#include <rstypes.h>

// -----------------------------------------------------------------------------
#if defined(SQLBUILD) || (defined(__RSRTL_DLL) && defined (_MSC_VER) && !defined (NO_BKMK_CLASS))
    #ifdef __cplusplus
        #include "sql/bkmrk.h"

        typedef Bookmark  recpos_t;
    #else
        typedef long      recpos_t;
        #define  RECPOS_T_ZERO      (0L)
        #define  RECPOS_T_NOT_ZERO  (1L)
    #endif // __cplusplus
#else //!SQLBUILD
    #define  RECPOS_T_ZERO      (0L)
    #define  RECPOS_T_NOT_ZERO  (1L)
    #define  BKMHANDLE          long

    typedef  long  recpos_t;
#endif //SQLBUILD

#include <packpsh1.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Операции для bfOp, bfGet и т.п.
typedef enum Btrieve_Operations
      {
       Bopenposblk   = -2,  // Фиктивная операция специально для использования в
                            // перехватчике операций для сохранения реального posblk
       Bnop          = -1,

       Bopen         = 0,   // Открыть существующий файл
       Bclose        = 1,   // Закрыть файл

       Binsert       = 2,   // Вставить запись
       Bupdate       = 3,   // Обновить запись
       Bdelete       = 4,   // Удалить запись

       // Операции BgetE - BgetL выполняют получение записей с учетом ключевого фильтра
       BgetE         = 5,   // Получить запись по ключу (точное соответствие)
       BgetN         = 6,   // Получить следующую запись по ключу
       BgetP         = 7,   // Получить предыдущую запись по ключу
       BgetGT        = 8,   // Получить запись строго больше значения ключа
       BgetGE        = 9,   // Получить запись больше или равно значению ключа
       BgetLT        = 10,  // Получить запись строго меньше значения ключа
       BgetLE        = 11,  // Получить запись меньше или равно значению ключа
       BgetF         = 12,  // Получить первую запись по ключу
       BgetL         = 13,  // Получить последнюю запись по ключу

       Bcreate       = 14,  // Создать файл
       BcreateX      = 14,  // Создать файл
       Bstat         = 15,  // Получить информацию о файле
       Bextend       = 16,  // Разделить файл данных на два логических диска (резерв)
       Bsetdir       = 17,  // Установить текущий каталог
       Bgetdir       = 18,  // Получить текущий каталог для требуемого диска

       // Работы с транзакциями
       BBT           = 19,  // Начать транзакцию
       BET           = 20,  // Завершить транзакцию
       BAT           = 21,  // Прервать транзакцию

       Bgetpos       = 22,  // Получить позицию текущей записи
       BgetD         = 23,  // Получить запись по ее позиции
       BstepN        = 24,  // Получить следующую запись в физической последовательности
       Bstop         = 25,  // Остановить работу текущего клиента СУБД
       Bver          = 26,  // Получить версии компонент СУБД
       Bunlock       = 27,  // Разблокировать запись(си)
       Breset        = 28,  // Освободить ресурсы, занятые клиентом
       Bsetown       = 29,  // Установить владельца для файла
       Bclearown     = 30,  // Снять владельца с файла
       BcreateSI     = 31,  // Добавить ключ в существующий файл
       BdropSI       = 32,  // Удалить ключ из существующего файла

       // Работа с записями в физической последовательности (без ключевого фильтра)
       BstepF        = 33,  // Получить первую запись в физической последовательности
       BstepL        = 34,  // Получить последнюю запись в физической последовательности
       BstepP        = 35,  // Получить предыдущую запись в физической последовательности

       // Операции с пакетной поддержкой
       BgetNX        = 36,  // Получить одну или несколько следующих записей по ключу
       BgetPX        = 37,  // Получить одну или несколько предыдущих записей по ключу
       BstepNX       = 38,  // Получить одну или несколько следующих записей в физической последовательности
       BstepPX       = 39,  // Получить одну или несколько предыдущих записей в физической последовательности
       BinsertX      = 40,  // Добавить одну или несколько записей в файл

       // Операции с процентным позиционированием (для скроллингов)
       BgetPcnt      = 44,  // Получить запись по заданному процентному размещению с учетом ключа
       BfindPcnt     = 45,  // Получить процентное размещение текущей записи

       // BLOB и т.п.
       BupdateChunk  = 53,  // Обновить часть записи

       // Аналоги операций BgetE - BgetL, но работают только с ключами (сама запись на загружается).
       // В ряде случаев позволяет увеличить скорость работы с СУБД
       BgkE          = 55,  // Получить ключ записи (точное соответствие)
       BgkN          = 56,  // Получить ключ следующей записи
       BgkP          = 57,  // Получить ключ предыдущей записи
       BgkGT         = 58,  // Получить ключ записи строго больше заданному значению ключа
       BgkGE         = 59,  // Получить ключ записи больше или равно заданному значению ключа
       BgkLT         = 60,  // Получить ключ записи строго меньше заданному значению ключа
       BgkLE         = 61,  // Получить ключ записи меньше или равно заданному значению ключа
       BgkF          = 62,  // Получить ключ первой записи
       BgkL          = 63,  // Получить ключ последней записи

       // Новые операции добавлять только перед BmaxOp !!!
       BmaxOp
      } Btrieve_Operations;


// -----------------------------------------------------------------------------
// Блокировки (и не только)
enum Locks
   {
    BIAS          =  50,    // Для получения операций 55 - 63 из соответствующих 5 - 13
    SWL           = 100,    // Блокировка чтения одной записи с ожиданием
    SNL           = 200,    // Блокировка чтения одной записи без ожидания
    MWL           = 300,    // Блокировка чтения нескольких записей с ожиданием
    MNL           = 400,    // Блокировка чтения нескольких записей без ожидания
    NOWAITMODIFY  = 500,    // Блокировка записи страницы без ожидания (для конкурентных транзакций)
    TRNCONCURENT  = 1000,   // Конкурентные транзакции (для V.6 ?)
    TRNCONCURRENT = TRNCONCURENT
   };

// -----------------------------------------------------------------------------
// Режимы открытия файлов

enum OpenModes
   {
    Accelerated  =  -1,     // Ускоренный режим
    ReadOnly     =  -2,     // Режим "только для чтения"
    Verify       =  -3,     // Верификация данных (не поддерживается)
    Exclusive    =  -4,     // Эксклюзивный режим

#ifdef SQLBUILD
    Share        =  -5,     // Разделяемый режим
    ExclusiveEx  =  -6,     // Расширенный эксклюзивный режим (для SQL)
#endif

    Normal       =   0,     // Нормальный режим (чтение/запись)

    // ----- Аналоги для принудительной локальной работы с СУБД
    NormalL      =   6,
    AcceleratedL =   7,
    ReadOnlyL    =   8,
    ExclusiveL   =  10,

    // ----- Аналоги для принудительной удаленной работы с СУБД
    NormalR      =  99,
    AcceleratedR = 100,
    ReadOnlyR    = 101,
    ExclusiveR   = 103
   };

#ifndef  SQLBUILD
    #define  Share        Normal
    #define  ExclusiveEx  Exclusive
#endif


// -----------------------------------------------------------------------------
// Поддержка операций создания файлов и т.п.

// Специальные номера ключей (для операции Bcreate)
#define  KNrename    -127           // Переименовать файл
#define  KNdelete    -128           // Удалить файл

// -----------------------------------------------
// Флаги файла
enum File_Flags
   {
    VarLen            =      1,     // Запись содержит переменную часть
    TruncB            =      2,     // Усечение конечных пробелов в записи
    PreAll            =      4,     // Предвыделение страниц
    DataC             =      8,     // Сжатие данных
    KeyOnly           =     16,     // Файл содержащий только ключи
    BalancedIndex     =     32, // Балансировка индексов
    Free10            =     64, // Зарезервировать 10% свободного места
    Free20            =    128, // --//-- 20%
    Free30            =    192, // --//-- 30%
    IncludeSystemData =    512, // Добавлять системные данные
    SPECIFY_KEY_NUMS  =   1024,     // Специфический номер ключа
    VATS_SUPPORT      =   2048, //
    IUO_KeyOnly       = 0x4000, // Внутренний флаг Первазива
    NotSystemData     =   4608, // Не добавлять системные данные
    IUO_PendingKey    = 0x8000, // Внутренний флаг Первазива
    PageC             =   8192  // Сжатие страниц
   };

// -----------------------------------------------
typedef enum File_Flags  File_Flas;

// -----------------------------------------------
// Флаги ключа
enum Key_Flags
   {
    Kdup  =   1,                    // Дублируемый
    Kmod  =   2,                    // Модифицируемый
    Kbin  =   4,                    // Двоичный (старый)
    Knull =   8,                    // Нулевой ключ
    Ksegm =  16,                    // Сегментируемый
    Kalt  =  32,                    // Альтернативный
    Kdes  =  64,                    // Обратная сортировка
    Ksup  = 128,                    // Повторяющийся дублируемый ()
    Kext  = 256,                    // Ключ содержит данные расширенного типа
    Kman  = 512                     // Ручной
   };

// -----------------------------------------------
// Максимальное число сегментов ключа
// Зависит от версии файла/размера страницы и может быть до 420.
#define  MaxKeySegm  119

// -----------------------------------------------
// Расширенные типы ключей (Extended_Key_Type)
#define  Estring    '\x00'          // STRING
#define  Einteger   '\x01'          // INTEGER
#define  Efloat     '\x02'          // FLOAT
#define  Edate      '\x03'          // DATE
#define  Etime      '\x04'          // TIME
#define  Edecimal   '\x05'          // DECIMAL
#define  Emoney     '\x06'          // MONEY
#define  Elogical   '\x07'          // LOGICAL
#define  Enumeric   '\x08'          // NUMERIC
#define  Ebfloat    '\x09'          // BFLOAT (Microsoft basic standard)
#define  Elstring   '\x0A'          // LSTRING (Pascal compatible string)
#define  Ezstring   '\x0B'          // ZSTRING (C compatible string)
#define  Eunbin     '\x0E'          // UNSIGNED BINARY
#define  Eautoinc   '\x0F'          // AUTOINC
#define  Ecurrency  '\x13'          // CURRENCY

// -----------------------------------------------
#if defined (USE_FDECIMAL)
    #define  EMONEY   Einteger
#else
    #define  EMONEY   Efloat
#endif

// -----------------------------------------------
typedef struct posblk_t
      {
       char  posblk[128];
      } posblk_t;

// -----------------------------------------------
typedef posblk_t  *posblk_h;


// -----------------------------------------------
// Спецификация файла
typedef struct FileSpecs
      {
       db_baseint16   reclen;       // Размер фиксированной части записи
       db_baseint16   pagesize;     // Размер страницы (кратен 512, но зависит от версии СУБД)
       db_baseint16   nindex;       // Число ключей (индексов) - младший байт, в старшем - версия файла
                                    // (по-умолчанию 0х00)
       db_ubaseint32  nrecords;     // При создании файла это резерв, а для операции Stat здесь возвращается
                                    // количество записей в файле.
       db_baseint16   fileflag;     // Флаги файла (File_Flags)
       char           reserved[2];  // Резерв
       db_baseint16   prealloc;     // Число предварительно выделяемых страниц (для создания файла,
                                    // для операции Stat здесь возвращается число неиспользуемых страниц
      } FileSpecs;

// -----------------------------------------------
#define  BTRV_FILE_SPECS(reclen, pagesize, nindex, fileflag, prealloc) \
{                             \
   (db_int16)(reclen),        \
   (db_int16)(pagesize),      \
   (db_int16)(nindex),        \
   (db_uint32)0,              \
   (db_int16)(fileflag),      \
   { '\0', '\0' },            \
   (db_int16)(prealloc)       \
}

// -----------------------------------------------
// Спецификация ключа
typedef struct KeySpecs
      {
       db_baseint16   keypos;       // Позиция ключа (первого байта) внутри записи (нумерация байт в записи с 1-цы)
       db_baseint16   keylen;       // Длина ключа (байт)
       db_baseint16   keyflag;      // Флаги ключа (Key_Flags)
       db_ubaseint32  nkeys;        // Резерв (для операции Stat здесь возвращается число уникальных ключей)
       char           keytype;      // Расширенный тип ключа (Extended_Key_Type)
       char           nullvalue;    // Нулевое значение (Null Value)
       unsigned char  reserved[4];  // Резерв
      } KeySpecs;

// -----------------------------------------------
#define  BTRV_KEY_SPECS(keypos, keylen, keyflag, keytype, nullvalue) \
{                             \
   (db_int16)(keypos),        \
   (db_int16)(keylen),        \
   (db_int16)(keyflag),       \
   (db_uint32)0,              \
   (keytype),                 \
   (nullvalue),               \
   { '\0', '\0', '\0', '\0' } \
}

// -----------------------------------------------
#define  BTRV_KEY_SPECS2(keypos, keylen, keyflag, keytype, nullvalue, keyNum) \
{                             \
   (db_int16)(keypos),        \
   (db_int16)(keylen),        \
   (db_int16)(keyflag),       \
   (db_uint32)0,              \
   (keytype),                 \
   (nullvalue),               \
   { '\0', '\0', (keyNum), '\0' } \
}

// -----------------------------------------------
// Альтернативная последовательность для сортировки
#ifndef __BTREXP_H
   typedef struct AltCol
         {
          char  sign;       // Сигнатура (0xAC)
          char  name[8];    // Уникальное имя последовательности
          char  tab[256];   // Таблица перекодировки
         } AltCol;
#endif


// -----------------------------------------------
// Режим создания файла
enum
   {
    NotOverFile = -1,       // Не перезаписывать существующий файл
    OverFile    = 0         // Перезаписывать существующий файл
   };


/********************************* Extend operation support *************/
// -----------------------------------------------
// Set key number parametr
enum
   {
    UseImmed  = -1,  // Use extension immediately
    UseNormal =  0   // Use extension until drive is full
   };


// -----------------------------------------------
// Тип пароля для установки владельца
enum
   {
    SetORW  = 0,  // Запрос владельца для всех режимов открытия файла, данные не шифруются
    SetOW   = 1,  // Запрос владельца для всех режимов открытия файла, кроме ReadOnly, данные не шифруются
    SetORWE = 2,  // Запрос владельца для всех режимов открытия файла, данные шифруются
    SetOWE  = 3   // Запрос владельца для всех режимов открытия файла, кроме ReadOnly, данные шифруются
   };

/********************************* Unlock operation support *************/
// -----------------------------------------------
// Set key number parametr
enum
   {
    UnlockSingle =  0,  // Unlock single record lock
    UnlockOne    = -1,  // Unlock one record multiply locked
    UnlockAll    = -2   // Unlock all records multiply locked
   };

/********************************* Version operation support ************/
// -----------------------------------------------
typedef struct
      {
       db_int16   version;    // Version number
       db_int16   revision;   // Revision number
       char       netware;    // 'N' for NetWare Btrive
      } VersBuf;


// -----------------------------------------------------------------------------
// Коды ошибок Pervasive
// В список не вошли устаревшие и совсем уж экзотические

#ifndef __BTREXP_H
    enum Berors
       {
        BEOK          =   0,        // Успешно
        BEinoper      =   1,        // Некорректная операция
        BEioerr       =   2,        // Ошибка ввода/вывода (чтения/записи)
        BEnoopen      =   3,        // Файл не открыт
        BEnorec       =   4,        // Запись не найдена (по заданному ключу)
        BEdupkey      =   5,        // Дублирование ключа (запись с таким ключом уже существует)
        BEinvkey      =   6,        // Некорректный номер ключа
        BEdifkey      =   7,        // Изменение ключа (между последовательными GET-операциями)
        BEinpos       =   8,        // Некорректная позиция записи (перед операциями Update/Delete)
        BEeof         =   9,        // Конец файла
        BEmodkey      =  10,        // Обновление неизменяемого ключа
        BEfilenm      =  11,        // Некорректное имя файла
        BEnofile      =  12,        // Файл не найден
        BEexfile      =  13,        // Ошибка открытия расширения файла (вида .^)
        BEclose       =  17,        // Ошибка закрытия файла (хотя в документации такой нет)
        BEfulld       =  18,        // Нет места на диске
        BEinactive    =  20,        // Ядро СУБД не активно
        BEsmallkey    =  21,        // Недостаточный размер буфера ключа
        BEldata       =  22,        // Недостаточный размер буфера данных
        BEpagesize    =  24,        // Некорректный размер страницы или буфера данных
        BEcreate      =  25,        // Ошибка создания файла
        BEkeynum      =  26,        // Некорректный номер ключа
        BEkeypos      =  27,        // Некорректная позиция ключа
        BEreclen      =  28,        // Некорректная длина записи
        BEkeylen      =  29,        // Некорректная длина ключа
        BEnobfile     =  30,        // Файл не является корректным файлом СУБД
        BEnoexpand    =  32,        // Файл не может быть расширен
        BEdirerror    =  35,        // Ошибка каталога
        BEduptrn      =  37,        // Другая транзакция уже активна
        BEnobegtrn    =  39,        // Транзакция не была начата
        BEbadoper     =  41,        // Операция не может быть выполнена
        BEnumrec      =  43,        // Некорректный адрес записи
        BEkeypath     =  44,        // Некорректный путь ключа
        BEkeyflag     =  45,        // Некорректный флаг ключа
        BEfileacc     =  46,        // Нет доступа к файлу
        BEmaxop       =  47,        // Превышен лимит на число открытых файлов (устарел)
        BEbadasc      =  48,        // Некорректное определение альтернативной последовательности сортировки
        BEexkeytype   =  49,        // Некорректный расширенный тип ключа
        BEowner       =  50,        // Владелец для файла уже установлен
        BEbadowner    =  51,        // Некорректное имя владельца
        BEbadvarlen   =  54,        // Часть переменной части записи повреждена
        BEautoinc     =  55,        // Некорректный атрибут автоинкрементного ключа
        BEbadindex    =  56,        // Индекс поврежден
        BEefile       =  59,        // Файл уже существует
        BEbadrec      =  60,        // Операция была выполнена не для всех записей (для операций 36 - 39)
        BEworkspace   =  61,        // Недостаточный размер рабочего пространства (для операций 36 - 39)
        BEdes         =  62,        // Некорректный дескриптор
        BEbatchbuf    =  63,        // Некорректный буфер данных для операции пакетной вставки записей
        BEfiltrlimit  =  64,        // Достигнут предел фильтра (для операций 36 - 39)
        BEfldoffset   =  65,        // Некорректное смещение поля
        BEbadddf      =  67,        // Ошибка открытия словаря SQL (DDF-файлы)
        BEconcurent =  70,   // Это инструментальная ошибка "Запись конкурентно изменена"
        BEdeadLock    =  78,        // Взаимоблокировка
        BEconflict    =  80,        // Конфликт на уровне записей (нечто вроде конкурентного изменения записей)
        BEunlocrec    =  81,        // Ошибка разблокировки записи
        BElostpos     =  82,        // Потеря позиции записи
        BErecloc      =  84,        // Запись или страница заблокирована
        BEfileloc     =  85,        // Файл заблокирован
        BEtableful    =  86,        // Переполнение таблицы файлов (> 65535)
        BEincmode     =  88,        // Несовместимый режим
        BEserver      =  91,        // Ошибка сервера
        BElocktype    =  93,        // Несовместимые типы блокировок записи
        BEpermis      =  94,        // Ошибка прав доступа
        BEsession     =  95,        // Обрыв сессии
        BEenv         =  96,        // Ошибка параметров окружения
        BEsmallbuff   =  97,        // Недостаточный размер буфера данных
        BEsmallcache  = 100,        // Недостаточный размер кеша
        BEsmallram    = 101,        // Недостаточно оперативной памяти
        BEsmallstack  = 102,        // Недостаточный размер стека
        BEchunkOffs   = 103,        // Слишком большое смещение части записи
        BElocale      = 104,        // Некорректная локаль
        BEnextchunk   = 106,        // Невозможно получить следующую часть записи
        BEsemaphore   = 109,        // Ошибка создания семафора или доступа к нему
        BEbusy        = 112,        // Файл используется другим клиентом
        BElimkeynum   = 120,        // Превышен лимит ключей для данного размера страницы
        BElimfilesize = 132,        // Превышен лимит на размер файла
        BEnoasc       = 136,        // Не найдена альтернативная последовательность для сортировки
        BEkeynumtrn   = 139,        // Некорректный номер ключа
        BEdupsyskey   = 146,        // Дублирование системного ключа
        BEsmalloffset = 151,        // Смещение части записи слишком мало
        BElimlic      = 161,        // Превышение лицензионных ограничений

        BEmaxerr,

        // ----- Расширенные ошибки
        BEminexterr   = -18,
        BEconstraint_parent_notfound = BEminexterr, // сработал constraint - ORA-02291 при добавлении \изменении записи, не найдена родительская запись
        BEconstraint_child_found     = -17,         // сработал constraint - ORA-02292 при удалении\изменении записи, найдены дочерние записи
        BEnomoreclnts = -16,                  // There are no more free client slots
        BEillclntid   = -15,                  // Illegal client id
        BEillclnth    = -14,                  // Illegal client handle
        BEinitReq     = -13,                  // Can not init requester (standart or ours)
        BEnodesc      = -12,                  // Description not found
        BEintegrviol  = -11,                  // Любые необработанные ошибки Oracle
        BEusrbrk      = -10,                  // Break SQL query
        BEinvrlen     = -9,                   // Dictionary record length mismatch
        BEdone        = -8,         // Операция выполнена прикладным обработчиком самостоятельно
        BEFilterAbort = -7,                   // User filter will abort the transaction

        BEmaxexterr
       };
#endif

// -----------------------------------------------
typedef struct TBtID
      {
       char           reserv[12];
       char           id1[2];
       unsigned short id2;
      } BTClientID;

// -----------------------------------------------
typedef short  btrvem_h;

// -----------------------------------------------------------------------------
#define  BTRVEM_DEF_HANDLE  ((btrvem_h)0)
#define  BTRVEM_BAD_HANDLE  ((btrvem_h)-1)
#define  BTRVEM_NULL_HANDLE ((btrvem_h)-2)

// -----------------------------------------------------------------------------
typedef unsigned long   bclient_t; // Хендл клиента для работы с СУБД
typedef short           bclient_h; // Хендл внутреннего клиента для работы с СУБД

// -----------------------------------------------
#define  BCLIENT_BAD_HANDLE  ((bclient_h)-1)
#define  BCLIENT_DEF_HANDLE  ((bclient_h)0)
#define  BCLIENT_EXT_HANDLE  ((bclient_h)-2)
#define  BCLIENT_BAD_CLIENT  ((bclient_t)0xFFFFFFFEUL)

_BTEXP bclient_h bGetClient(const char *name, int *stat);
_BTEXP int       bReleaseClient(bclient_h client);

_BTEXP bclient_h bSetDefClient(bclient_h client, int *stat);
_BTEXP bclient_h btrvGetClient(const char *name);
_BTEXP bclient_h btrvSetDefClient(bclient_h client);
_BTEXP bclient_t btrvGetDefClient(void);
_BTEXP bclient_t btrvHandle2Id(bclient_h handle);
_BTEXP bclient_h btrvId2Handle(bclient_t id);
_BTEXP int       btrvReleaseClient(bclient_h client);

// Close ALL CLIENTS IMMEDIATELY!
_BTEXP void btrvNuke(void);

_BTEXP void InitBtrClients(void);
_BTEXP void DoneBtrClients(void);

// -----------------------------------------------
#define  NWcall(OP, POS_BLK, DATA_BUF, DATA_LEN, KEY_BUF, KEY_LEN, KEY_NUM) \
               NWcallEx((OP), (POS_BLK), (DATA_BUF), (DATA_LEN), (KEY_BUF), (KEY_LEN), (KEY_NUM), 0)

// -----------------------------------------------------------------------------
// Обобщенная функция для работы с СУБД Pervasive (транзакционный режим)
// [in]     OP        - Номер операции
// [in]     POS_BLK   - Блок описания dbt-файла
// [in]     DATA_BUF  - Буфер данных
// [in,out] DATA_LEN  - Размер буфера данных
// [in]     KEY_BUF   - Буфер ключа
// [in]     KEY_LEN   - Размер буфера ключа
// [in]     KEY_NUM   - Номер ключа
// [in]     CLIENT_ID - Дескриптор внутреннего клиента
// [ret]    Возвращает статус операции (0 - успех, в остальных случаях код ошибки)
_BTEXP int NWcallEx(int OP, void *POS_BLK, void *DATA_BUF, int *DATA_LEN, void *KEY_BUF, int KEY_LEN,
                    signed char KEY_NUM, bclient_h CLIENT_ID);

// -----------------------------------------------------------------------------
// Аналог NWcallEx, но с возможностью работы под заданным внешним клиентом.
// Для этого в качестве CLIENT_ID нужно передать значение BCLIENT_EXT_HANDLE.
// В противном случае параметр BtrID будет проигнорирован.
_BTEXP int NWcallExClntID(int OP, void *POS_BLK, void *DATA_BUF, int *DATA_LEN, void *KEY_BUF,
                          int KEY_LEN, signed char KEY_NUM, bclient_h CLIENT_ID, BTClientID *BtrID);

// -----------------------------------------------------------------------------
// Указатель на прикладную функцию-обработчик операций с СУБД.
// Параметры функции полностью аналогичны NWcallExClntID.
// Обработчик вызывается непосредственно перед обращением к СУБД.
// Алгоритм обработки значений, возвращаемых обработчиком:
//      BEOK   - Выполнить требуемую операцию к СУБД
//      BEdone - Операция выполнена обработчиком самостоятельно, обращение к СУБД не выполняется
//      Остальные возвращаемые значения трактуются как ошибки, обращение к СУБД не выполняется.
typedef int (*BtrUserFilterType)(int OP, void *POS_BLK, void *DATA_BUF, int *DATA_LEN, void *KEY_BUF,
                                 int KEY_LEN, signed char KEY_NUM, BTClientID *clntId);

// -----------------------------------------------------------------------------
// Указатель на прикладную функцию-обработчик статусов операций с СУБД.
// Параметры функции полностью аналогичны NWcallExClntID за исключением stat, который
// содержит статус операции с СУБД.
// Обработчик вызывается непосредственно после обращения к СУБД.
// Значения, возвращаемые обработчиком, не используются.
// Примечание: если был установлен обработчик BtrUserFilterType и он вернул статус,
//             отличный от BEOK, обработчик статусов операции вызван не будет.
typedef int (*BtrUserFilterStat)(int stat, int OP, void *POS_BLK, void *DATA_BUF, int *DATA_LEN, void *KEY_BUF,
                                 int KEY_LEN, signed char KEY_NUM, BTClientID *clntId);


// -----------------------------------------------------------------------------
// Установить пользовательский обработчик операций с СУБД.
// Возвращает указатель на предыдщий обработчик.
BtrUserFilterType BtrSetUserCallBack(BtrUserFilterType newFilter);

// -----------------------------------------------------------------------------
// Установить пользовательский обработчик статусов операций с СУБД.
// Возвращает указатель на предыдщий обработчик.
BtrUserFilterStat BtrSetUserCallBackStat(BtrUserFilterStat newFilter);

// Btrieve error message encription
_BTEXP char *BErrorText(int ErrorCode);
_BTEXP char *BErrorText2(int errorCode, int sql);


// -----------------------------------------------------------------------------
// Поддержка операций с частями записи (переменные части, BLOB)

// -----------------------------------------------
struct tagTChunkDes
     {
      unsigned long   ChunkOffset;
      unsigned long   ChunkLen;
      void RSL_FAR   *UserData;
     };

// -----------------------------------------------
typedef struct tagTChunkDes  TChunkDes;

// -----------------------------------------------
typedef struct
      {
       unsigned long  Subfunction;
       unsigned long  NumChunks;
      } TChunkHeader;

// -----------------------------------------------
typedef struct
      {
       TChunkHeader   head;
       TChunkDes      des[1];
      } TChunk;

// -----------------------------------------------
typedef struct
      {
       unsigned long  addr;
       TChunkHeader   head;
       TChunkDes      des[1];
      } TReadChunk;

// -----------------------------------------------
typedef struct
      {
       TChunkHeader   head;
       TChunkDes      des[1];
      } TWriteChunk;

// -----------------------------------------------
typedef struct
      {
       unsigned long  Subfunction;
       unsigned long  ChunkOffset;
      } TTruncateChunk;


// -----------------------------------------------------------------------------
#define  BT_DIRECT_RANDOM    0x80000000L
#define  BT_INDIRECT_RANDOM  0x80000001L

#define  BT_DIRECT_RECT      0x80000002L
#define  BT_INDIRECT_RECT    0x80000003L

#define  BT_TRUNCATE         0x80000004L

#define  BT_NEXT_BIAS        0x40000000L
#define  BT_APPEND_BIAS      0x20000000L

#define  BT_INDIRECT_BIT     0x00000001L

// -----------------------------------------------------------------------------

int getFullClientID(BTClientID *id, bclient_h client);

#ifdef __cplusplus
}
#endif

#include <packpop.h>

// -----------------------------------------------
#define  ErrorText(ErrorCode)  BErrorText(ErrorCode)

#endif  /* __NWBTRV_H */

/* <-- EOF --> */