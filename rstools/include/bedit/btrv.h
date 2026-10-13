/*******************************************************************************
 FILE         :   BTRV.H

 COPYRIGHT    :   VECsoft, 1992
                  R-Style Software Lab, 1994,1998

 DESCRIPTION  :   BTRVFILE interface (Btieve & DBtriever/400)

 PROGRAMMED BY:   Emil Dovidovitch, Serg Kubrin, Yakov Markovitch

 CREATION DATE:   28 Oct 1992

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/bedit/h/btrv.h $
                  $Revision: 45 $
                  $Date: 7.09.00 14:18 $
                  $Author: Kormukhin $
                  $Modtime: 7.09.00 12:33 $
*******************************************************************************/

//              Borland's C low & BTRVFILE levels interface
//                   to the Btrieve Record Manager 5.00
//

/*
   September 22,1994
   Кубрин С.  Поддержка DPMI16
   September 27,1994
   Кубрин С.  Поддержка DPMI32

   27 December 1994
   Ашманов Ю. 1) добавлено 2 поля типа void * в структуру BTRVFILE
             2) некоторые #define заменены на enum
   January 6,1995   Serg Kubrin (K78)
             Вынес интерфейс с Btrieve в файл nwbtrv.h
   November 2,1995   Serg Kubrin (K78)
             Add BLOB field support. Need Btrieve 6.x
   27 December 1995
     Маркович Я. (JNS) Добавил #ifdef __cplusplus,
           исправил определение RecFltr и KeyFltr

   13/02/1996 Yakov E. Markovitch (JNS)
     Added bfSt operation and all corresponding structures
   09/01/1997 Yakov E. Markovitch (JNS)
    Multi-client support added.
*/

#ifndef __BTRV_H
#define __BTRV_H

#define BTRVH

#include <rstypes.h>
#include "nwbtrv.h"
#include "bexpdef.h"
#include "beditdef.h"
#include "mdb/mdb.h"
#include <rscdefs.h>

#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


// -----------------------------------------------------------------------------
#ifdef RSL_PL_DOS
    // Новые программы не должны использовать эту функцию
    int BTRV(int OP, void *POS_BLK, void *DATA_BUF, int *DATA_LEN, void *KEY_BUF, char KEY_NUM);

    #define  BTRV(op, pos, data, dlen, key, num)  NWcall(op, pos, data, dlen, key, 255, num)
#endif

// -----------------------------------------------------------------------------
//  Смещение и длина поля в структуре
#ifdef RSL_MD_FLAT
    #define  koff(s_name, m_name)  (offsetof(s_name, m_name) + 1)
    #define  klen(s_name, m_name)  sizeof(((s_name *)0)->m_name)
#else
    #define  koff(s_name, m_name)  (offsetof(s_name, m_name) + 1)
    #define  klen(s_name, m_name)  sizeof(((s_name _FAR *)0)->m_name)
#endif

//-@H------ BEGIN __ BLOB data_________________________________ --------@@-

#ifndef BLOB_DATA_DEFINED
#define BLOB_DATA_DEFINED
    #include <packpsh1.h>

    // -------------------------------------------
    // This structure is written at the begining of BLOB field.
    typedef struct TBlobHeader
          {
           unsigned char  state;        // TBlobState
           db_uint32      size;         // Общий размер BLOB-поля
           unsigned char  blobType;     // 1 - if RSL values saved in BLOB
           char           reserved[8];
          } TBlobHeader;

    #include <packpop.h>

    // -------------------------------------------
    // Состояния BLOB
    enum TBlobState
       {
        BLOB_OK = 1,
        BLOB_DIRTY
       };

    // -------------------------------------------
    // Режимы BLOB'а
    typedef enum TBlobOpenMode
          {
           BLOB_OPEN_READ,      // Чтение
           BLOB_OPEN_WRITE,     // Запись
           BLOB_OPEN_RDWR,      // Чтение/запись
           BLOB_CLOSED          // BLOB закрыт
          } TBlobOpenMode;

    #include <packpshn.h>

    // -------------------------------------------
    // Блок управления BLOB'ом
    typedef struct tagTBlobControlData
          {
           TBlobOpenMode  openMode;
           TBlobHeader    hd;
           unsigned int   bufSize;      // Размер буфера (было просто unsigned)
           char          *bufStart;     // Начало буфера
           char          *bufLast;      // Последняя используемая позиция в буфере (не факт)
           char          *bufPtr;       // Указатель на буфер
           unsigned long  offsInBlob;   // Смещение в Blob первого байта в буфере
           bool           isDirty;      // True если Blob был изменён
           bool           isEmpty;      // True если буфер пуст
           bool           newPos;       // True если в Blob была установлена новая позиция
          }  TBlobControlData;

    #include <packpop.h>
#endif // BLOB_DATA_DEFINED
//-@@------ END __ BLOB data___________________________________ --------@E-

/************************************************************************/

typedef struct btrvfile BTRVFILE;

// Функия фильтрации записей
typedef int (*JBtrFltr)(BTRVFILE *Br);

// Типы баз данных
typedef enum
      {
       DBT_BTRIEVE = 0,   // Btrieve
       DBT_MDB,           // Менеджер записей в оперативной памяти
       DBT_SQL,           // SQL эмулятор Btrieve'а
       DBT_USER
      } DB_TYPE;

#include <packpshn.h>

// -----------------------------------------------------------------------------
typedef struct
      {
       MDB_CURSOR  *cursor;      // Курсор МЗП
      } BMDB_DATA;

// -----------------------------------------------------------------------------
// Управляющий блок файла
struct btrvfile
     {
      union
          {
           posblk_t   posblk;            // Позиционный блок
           BMDB_DATA  mdb;

           #ifdef SQLBUILD
               void  *sqlf;
           #endif

           #if defined(SQLBUILD) || (defined (__cplusplus) && defined(__RSRTL_DLL) && defined (_MSC_VER) && !defined (NO_BKMK_CLASS))
               IBtUserDataSource *user;
           #endif
          } internal;

      BTRVFILE   *prev;         // Предыдущий элемент списка
      BTRVFILE   *next;         // Следующий элемент списка

      void       *RecBuf;       // Ссылка на буфер данных
      void       *KeyBuf;       // Ссылка на буфер ключа

      void       *KeyTopVal;    // Ссылка на минимальное значение ключа
      void       *KeyBotVal;    // Ссылка на максимальное значение ключа

      void       *bfilePtr;     // Используется в Btrieve C++ wrapper
      void       *userData;     // Личные данные пользователя, связанные с файлом

      char       *FileName;     // Имя файла

      JBtrFltr    RecFltr;      // Фильтр выбора записей
      JBtrFltr    KeyFltr;      // Ключевой фильтр выбора записей

      int         MaxRecLen;    // Максимальная длина записи
      int         KeyNum;       // Номер ключа
      int         RecLen;       // Текущая длина записи
      recpos_t    RecPos;       // Физический адрес записи
      int         MaxKeyLen;    // Максимальная длина буфера ключа

   /*   November 2,1995 (K78)
       BLOB field support.
   */
      int         fixsize;      // Record fixed part lenght
      TBlobControlData  *blob;
   /*   January 9,1997 (JNS)
       Multi-client support.
   */
      bclient_h   client;       // BTrieve Client ID
      DB_TYPE     type;         // Тип базы данных
      int         nCaches;      // Количество МЗП-кэшей у данного файла

   // June 27,2000
      unsigned char openMode;

   // May 23, 2001
      bool        redir;        // Перенаправление вызовов к кэшу
      BTRVFILE   *cache;        // Ссылка на первый кэш (для перенаправления)

      // truncated - разрешает игнорировать код BEldata при чтении записей
      // с переменной частью. При этом fixsize должен быть равен размеру постоянной
      // части.
      bool        truncated;

      // reversOrder - означает подмену операций навигации по файлу
      // (BstepF, BstepN, BstepL, BstepP, BgetF, BgetN, BgetL, BgetP)
      // на уровне функции bfGet.
      bool        reversOrder;

      void       *bs;           // bs - указатель на текущий скроллинг

      char        _reserved[5]; // Reserved for future use. DON'T USE VOLUNTARY!!!
     };

// -----------------------------------------------------------------------------
typedef  void (*FreeUserTLSProc)(void *userData);

// -----------------------------------------------------------------------------
struct RSTLS
     {
      int              version;   // Версия данной структуры. Для текущей версии 0.
      int              m_bfstat;
      int              m_bfop;
      int              m_TrnFlag;
      int              m_TrnLockFlags;
      BTRVFILE        *m_bfcb;
      bool             m_useDistrTrn;
      char             m_szLastName[_MAX_PATH];
      void*            userData;
      FreeUserTLSProc  cleanProc;
      int              m_TrnCount;  // #2972 Счетчик повторов транзакции (от 0)

      #ifdef SQLBUILD
          char         m_szLastRsdError[1024];
          btrvem_h     m_curBtrvEm_notUsedNow;
      #endif
     };


#include <packpop.h>

// JNS, 13/02/1996
/*******************************************************************************
 Key structure for BTRIEVE STAT operation
*******************************************************************************/

#include <packpsh1.h>

// -----------------------------------------------------------------------------
typedef struct JBtKSpec
      {
       db_int16       keyPos;    // Key position (offset)
       db_uint16      keyLen;    // Key length
       db_uint16      keyFlags;  // Key flags
       db_int32       uniqNum;   // Number of unique values
       unsigned char  keyType;   // Extended type
       unsigned char  nullVal;
       short          reserved;
       char           keyNum;
       char           ACSnum;
      } JBtKSpec;

// -----------------------------------------------------------------------------
typedef struct JBtFSpec
      {
       db_uint16      recLen;
       db_uint16      pageSize;
       db_uint16      indNum;
       db_uint32      recNum;
       db_uint16      fFlags;
       unsigned short reserved;
       db_uint16      unusedPg;
      } JBtFSpec;

typedef  JBtFSpec JBtrFileSpec;
typedef  JBtKSpec JBtrKeySpec;

// -----------------------------------------------------------------------------
typedef struct JBFSpec
      {
       JBtrFileSpec fs;
       JBtrKeySpec  ks[1];
      } JBFSpec;

#include <packpop.h>

_BTEXP int bfSt(BTRVFILE *bf, JBFSpec *spec, unsigned nkeys);

// ***************** end of (JNS) ********************

bool ToolsInitTLS(void);
void ToolsDoneTLS(void);

_BTEXP void   ToolsClearTLS(void);
_BTEXP RSTLS *ToolsGetTLS(void);



_BTEXP int  *__bfstat(void);
_BTEXP int  *__bfop(void);
_BTEXP int  *__TrnFlag(void);
_BTEXP int  *__TrnCount(void);
_BTEXP bool *__useDistrTrn(void);
_BTEXP BTRVFILE **__bfcb(void);
_BTEXP char *__szLastName(void);

_BTEXP void setUseDistrTrn(bool bOn);
_BTEXP int  SetOptLock(int optLock);

_BTEXP btrvem_h OpenDb(const char *connStr, const char *name);
_BTEXP btrvem_h SetCurrentDb(btrvem_h db);
_BTEXP btrvem_h GetCurrentDb();
_BTEXP btrvem_h FindDb(const char *name);
_BTEXP void     CloseDb(btrvem_h db);

#ifdef SQLBUILD
    _BTEXP char     *__szLastRsdError(void);
    _BTEXP btrvem_h *__curBtrvEm(void);
#endif


#ifndef __cplusplus
    #define  bfstat       (*__bfstat())
    #define  bfop         (*__bfop())
    #define  bfcb         (*__bfcb())
    #define  szLastName   (__szLastName())
    #define  TrnFlag      (*__TrnFlag())
    #define  TrnCount     (*__TrnCount())
    #define  useDistrTrn  (*__useDistrTrn())

    #ifdef SQLBUILD
        #define  szLastRsdError  (__szLastRsdError())
        #define  curBtrvEm       (*__curBtrvEm())
    #endif
#else
    // In C++ we MUST qualify function calls as global namespace
    #define  bfstat       (*(::__bfstat()))
    #define  bfop         (*(::__bfop()))
    #define  bfcb         (*(::__bfcb()))
    #define  szLastName   (::__szLastName())
    #define  TrnFlag      (*(::__TrnFlag()))
    #define  TrnCount     (*(::__TrnCount()))
    #define  useDistrTrn  (*(::__useDistrTrn()))

    #ifdef SQLBUILD
        #define  szLastRsdError  (::__szLastRsdError())
        #define  curBtrvEm       (*__curBtrvEm())
    #endif
#endif

//-@H------ BEGIN __ BLOB functions ___________________________ --------@@-
_BTEXP int bfOpenBlob         (BTRVFILE *bf, TBlobOpenMode openMode);
_BTEXP int bfCloseBlob        (BTRVFILE *bf);
_BTEXP int bfReadBlob         (BTRVFILE *bf, char *buff, int size);
_BTEXP int bfWriteBlob        (BTRVFILE *bf, char *buff, int size);
_BTEXP int bfSetBlobPos       (BTRVFILE *bf, unsigned long pos);
_BTEXP int bfGetBlobPos       (BTRVFILE *bf, unsigned long *pos);
_BTEXP int bfGetBlobSize      (BTRVFILE *bf, unsigned long *size);
_BTEXP int bfTruncateBlob     (BTRVFILE *bf);
_BTEXP int bfSetBlobType      (BTRVFILE *bf, unsigned char tp);
//-@@------ END __ BLOB functions _____________________________ --------@E-

// Размещение BTRVFILE-блока

_BTEXP BTRVFILE *alloc_bf(int size);          // Разместить блок
_BTEXP void       free_bf(BTRVFILE *curblk);  // Освободить блок
_BTEXP BTRVFILE *alloc_bf_topblk(void);

// -----------------------------------------------------------------------------
// Open BTRVFILE
_BTEXP int bfOpen(BTRVFILE **bf,          // BTRVFILE ptr - returnd
                  const char *Name,       // Имя файла
                  int         Mode,       // Режим открытия файла
                  int         MaxRecLen,  // Максимальная длина записи
                  int         MaxKeyLen,  // Максимальная длина буфера ключа
                  int         KeyNum,     // Номер ключа
                  JBtrFltr    RecFltr,    // Фильтр выбора записей
                  JBtrFltr    KeyFltr,    // Ключевой фильтр выбора записей
                  const char *Own,        // Владелец
                  void       *RecBuf      // Ссылка на буфер данных
                 );

// -----------------------------------------------------------------------------
// Open BTRVFILE
_BTEXP int bfOpen2(BTRVFILE   **bf,         // BTRVFILE ptr - returnd
                   const char  *Name,       // Имя файла
                   int          Mode,       // Режим открытия файла
                   int          MaxRecLen,  // Максимальная длина записи
                   int          MaxKeyLen,  // Максимальная длина буфера ключа
                   int          KeyNum,     // Номер ключа
                   JBtrFltr     RecFltr,    // Фильтр выбора записей
                   JBtrFltr     KeyFltr,    // Ключевой фильтр выбора записей
                   const char  *Own,        // Владелец
                   void        *RecBuf,     // Ссылка на буфер данных
                   int          cacheSize
                  );

// -----------------------------------------------------------------------------
// Open BTRVFILE
_BTEXP int bfOpenEx(BTRVFILE    **bf,          // BTRVFILE ptr - returnd
                    const char   *Name,        // Имя файла
                    int           Mode,        // Режим открытия файла
                    int           KeyNum,      // Номер ключа
                    int           useBlob,     // If true use BLOB field in record. Need Btrieve 6.x
                    int           varLenSize,  // Size of varlen buffer,if == 0, then used 1024
                    JBtrFltr      RecFltr,     // Фильтр выбора записей
                    JBtrFltr      KeyFltr,     // Ключевой фильтр выбора записей
                    const char   *Own          // Владелец
                   );

// -----------------------------------------------------------------------------
// Open BTRVFILE
_BTEXP int bfOpenEx2(BTRVFILE    **bf,          // BTRVFILE ptr - returnd
                     const char   *Name,        // Имя файла
                     int           Mode,        // Режим открытия файла
                     int           KeyNum,      // Номер ключа
                     int           useBlob,     // If true use BLOB field in record. Need Btrieve 6.x
                     int           varLenSize,  // Size of varlen buffer,if == 0, then used 1024
                     JBtrFltr      RecFltr,     // Фильтр выбора записей
                     JBtrFltr      KeyFltr,     // Ключевой фильтр выбора записей
                     const char   *Own,         // Владелец
                     int           cacheSize    // Размер кэша
                    );

// -----------------------------------------------------------------------------
// Open BTRVFILE
_BTEXP int bfOpenRSD(BTRVFILE **bf,  // BTRVFILE ptr - returnd
                     void      *rs   // RSD recordset
                    );

// -----------------------------------------------------------------------------
#if defined(SQLBUILD) || (defined (__cplusplus) && defined(__RSRTL_DLL) && defined (_MSC_VER) && !defined (NO_BKMK_CLASS))
    _BTEXP int bfOpenUser(BTRVFILE          **bf,
                          const char         *name,
                          IBtUserDataSource  *user,
                          int                 MaxRecLen,     // Максимальная длина записи
                          int                 MaxKeyLen,     // Максимальная длина буфера ключа
                          JBtrFltr            RecFltr,       // Фильтр выбора записей
                          JBtrFltr            KeyFltr,       // Ключевой фильтр выбора записей
                          void               *RecBuf         // Ссылка на буфер данных
                          );
#endif

// -----------------------------------------------------------------------------
// Ф-ии работы с SQL курсором и настройки эмулятора
// тип SQL запроса
typedef enum tagTBtEmQueryType
      {
       BTEM_GET_QUERY       = 1,   // запрос , выполняющий операции Get
       BTEM_NEXTPREV_QUERY  = 2,   // запросы, выполняющие Prev  Next
       BTEM_FIRSTLAST_QUERY = 3,   // запросы, выполняющие First last
       BTEM_ALL_QUERIES     = 4    // все запросы
      } TBtEmQueryType;

// -----------------------------------------------
// режим оптимизации
typedef enum tagTBtEmOptmMode
      {
       BTEM_NO_OPTM         = 0,   // без оптимизации
       BTEM_HINT_OPTM       = 3    // установка подсказки
      } TBtEmOptmMode;


_BTEXP int bfAddLink(BTRVFILE *bf, const char *depFields, const char *depFiles, const char *joinCond,
                     const char *whereCond, ...);

_BTEXP int bfAddLink2(BTRVFILE *bf, const char *depFields, const char *depFiles, const char *joinCond,
                      const char *whereCond, void **array, int arr_size);

// -----------------------------------------------------------------------------
typedef struct tagExprInfo
      {
       long  type;
       int   size;
      } TExprInfo;

_BTEXP int bfAddLinkEx(BTRVFILE *bf, const char *depFields, const char *depFiles, const char *joinCond,
                       const char *whereCond, void **array, int arr_size, TExprInfo *exprinfo, int exprinfo_size);

_BTEXP int bfAddLinkParam(BTRVFILE *bf, long type, void *val, int size);

_BTEXP int bfDropLink(BTRVFILE *bf);

_BTEXP bool bfLinked(BTRVFILE *bf);
_BTEXP bool bfOptimized(BTRVFILE *bf);

// -----------------------------------------------------------------------------
typedef struct tagTDepField
      {
       char  *Alias;
       char  *TableName;
       char  *FieldName;
      } TDepField;

// -----------------------------------------------------------------------------
typedef struct tagTLinkInfo
      {
       TDepField  *depFields;
       void      **pData ;
       int         nLinks;
      } TLinkInfo;

// -----------------------------------------------------------------------------
typedef struct tagTSqlInfoEx
      {
       void      **data_array;
       int         length;
       TExprInfo  *column_info;
       char       *sql;
      } TSqlInfoEx;

_BTEXP void bfGetLinkInfo(BTRVFILE *bf, TLinkInfo **info);
_BTEXP void bfFreeLinkInfo(TLinkInfo **info);

// -----------------------------------------------------------------------------
typedef enum tagTBfOperType
      {
       BF_BFOPG_OPER  = 1, 
       BF_BFGET_OPER  = 2, 
       BF_BSGET_OPER  = 3,
       BF_GET_ORDERBY = 100  
      } TBfOperType;

_BTEXP char *bfGetSql(BTRVFILE *bf, char *sql, int operType);

_BTEXP void bfGetSqlInfo(BTRVFILE *bf, int operType, TSqlInfoEx **info);
_BTEXP void bfFreeSqlInfo(TSqlInfoEx **info);

// -----------------------------------------------------------------------------
enum EMPROPERTY
   {
    EM_RESETONSWITCH = 1,
    EM_MAXCACHESIZE  = 2,
    EM_RECORDLOCK    = 3,
    EM_SQLBREAK      = 4,
    EM_MAXBLOBSIZE   = 5,
    EM_USENULLS      = 6,
    EM_TRNFLAG       = 7
   };

void bfSetEmProp(int property, int value, btrvem_h hem);

// -----------------------------------------------------------------------------
typedef struct tagTSortField
      {
       char  *Alias;
       char  *TableName;
       char  *FieldName;
       bool   Descending;
       void  *pData;
      } TSortField;

// -----------------------------------------------------------------------------
typedef struct tagTSortInfo
      {
       TSortField *sortFields;    
       int         nFields;
       bool        bUserSort;
      } TSortInfo;

_BTEXP void bfGetSortInfo(BTRVFILE *bf, TSortInfo **info);
_BTEXP void bfFreeSortInfo(TSortInfo **info, DB_TYPE type);

_BTEXP void bfSetToolsWhere(BTRVFILE *bf,const char *toolsWhere);

// -----------------------------------------------------------------------------
typedef enum tagTBfCursorType
      {
       BF_MAIN_CURSOR       = 1,  // основной курсор
       BF_OPTIMIZED_CURSOR  = 2,  // оптимизированный курсор
       BF_USERSORT_CURSOR   = 3   // курсор с ПС
      } TBfCursorType;

_BTEXP int  bfSetCursor    (BTRVFILE *bf, int type, bool bSync);
_BTEXP int  bfGetCursorType(BTRVFILE *bf);
_BTEXP bool bfTestCursor   (BTRVFILE *bf, int type);

_BTEXP int bfAddUserSort  (BTRVFILE *bf, const char *sortFlds);
_BTEXP int bfDropUserSort (BTRVFILE *bf);

_BTEXP int  bfSetCaching  (BTRVFILE *bf, bool val);
_BTEXP void bfSetCacheMode(BTRVFILE *bf, int fullShift);
_BTEXP void bfResetCache  (BTRVFILE *bf);
_BTEXP void bfSkipBmSearch(BTRVFILE *bf, bool bSkip);

_BTEXP void bfLoadInfo(char **nameArr, int arrSize);

_BTEXP int  bfSetOptmMode(BTRVFILE       *bf, 
                          TBtEmQueryType  qt,   // тип запросов, для которых устанавливается режим
                          TBtEmOptmMode   om,   // режим оптимизации
                          const char     *hint  // подсказки SQL 
                         );

_BTEXP const char *bfSetHint(BTRVFILE      *bf, 
                             int            crsType,// тип курсора, для которого устанавливается режим 
                             int            qt,     // тип запросов, для которых устанавливается режим 
                             int            all,    // применить ко всем курсорам в стеке
                             const char    *hint,   // подсказки SQL 
                             char          *oldHint
                            );

_BTEXP int   bfGetPosByBuffer(BTRVFILE *bf, recpos_t *pos);

_BTEXP void  CancelSqlQuery();
_BTEXP void *GetDefaultConnection();    
_BTEXP void *GetConnectionIntf();    
_BTEXP void *GetConnectionHandle();    
_BTEXP void  BtemDone();

/*********************************************************/ 

// -----------------------------------------------------------------------------
// Open BTRVFILE using specified client
_BTEXP int bfOpenClnt(BTRVFILE   **bf,         // BTRVFILE ptr - returnd
                      bclient_h    client,     // BTRIEVE client's handle
                      const char  *Name,       // Имя файла
                      int          Mode,       // Режим открытия файла
                      int          MaxRecLen,  // Максимальная длина записи
                      int          MaxKeyLen,  // Максимальная длина буфера ключа
                      int          KeyNum,     // Номер ключа
                      JBtrFltr     RecFltr,    // Фильтр выбора записей
                      JBtrFltr     KeyFltr,    // Ключевой фильтр выбора записей
                      const char  *Own,        // Владелец
                      void        *RecBuf      // Ссылка на буфер данных
                     );

// -----------------------------------------------------------------------------
// Open BTRVFILE
_BTEXP int bfOpenClntEx(BTRVFILE   **bf,         // BTRVFILE ptr - returnd
                        bclient_h    client,     // BTRIEVE client's handle
                        const char  *Name,       // Имя файла
                        int          Mode,       // Режим открытия файла
                        int          KeyNum,     // Номер ключа
                        int          useBlob,    // If true use BLOB field in record. Need Btrieve 6.x
                        int          varLenSize, // Size of varlen buffer
                        JBtrFltr     RecFltr,    // Фильтр выбора записей
                        JBtrFltr     KeyFltr,    // Ключевой фильтр выбора записей
                        const char  *Own         // Владелец
                       );

// -----------------------------------------------------------------------------
// Open BTRVFILE using specified client
_BTEXP int btrvOpenClnt(BTRVFILE   **bf,      // BTRVFILE ptr - returnd
                        bclient_h    client,   // BTRIEVE client's handle
                        const char  *Name,   // Имя файла
                        int          Mode,    // Режим открытия файла
                        int          MaxRecLen,    // Максимальная длина записи
                        int          MaxKeyLen,    // Максимальная длина буфера ключа
                        int          KeyNum, // Номер ключа
                        JBtrFltr     RecFltr, // Фильтр выбора записей
                        JBtrFltr     KeyFltr, // Ключевой фильтр выбора записей
                        const char  *Own,        // Владелец
                        void        *RecBuf  // Ссылка на буфер данных
                       );

// -----------------------------------------------------------------------------
// Open BTRVFILE
_BTEXP int btrvOpenClntEx(BTRVFILE   **bf,           // BTRVFILE ptr - returnd
                          bclient_h    client,       // BTRIEVE client's handle
                          const char  *Name,         // Имя файла
                          int          Mode,         // Режим открытия файла
                          int          KeyNum,       // Номер ключа
                          int          useBlob,      // If true use BLOB field in record. Need Btrieve 6.x
                          int          varLenSize,   // Size of varlen buffer
                          JBtrFltr     RecFltr,      // Фильтр выбора записей
                          JBtrFltr     KeyFltr,      // Ключевой фильтр выбора записей
                          const char  *Own           // Владелец
                         );


// -----------------------------------------------------------------------------
// Закрыть файл
// [in] cb - Указатель на файл
_BTEXP int bfClose(BTRVFILE **cb);

// -----------------------------------------------------------------------------
// Закрыть все открытые dbt-файлы
_BTEXP int bfCloseAll(void);

// -----------------------------------------------------------------------------
// Установить режим открытия файла (для SQL-режима)
// [in] bf   - Указатель на файл
// [in] mode - Режим открытия
// [ret]     - Возвращает предыдущий режим открытия
_BTEXP int bfSetOpenMode(BTRVFILE *bf, int mode);

// -----------------------------------------------------------------------------
// Закрыть все соединения с СУБД
#define  bfStop()  (btrvNuke(), BEOK)


// -----------------------------------------------------------------------------
// Начать транзакцию
// [in] lock - Тип блокировки
_BTEXP int bfBT(int lock);

// Начать распределенную транзакцию
// [in] lock - Тип блокировки
_BTEXP int bfBDT(int lock);

// Завершить транзакцию
_BTEXP int bfET(void);

// Завершить распределенную транзакцию
_BTEXP int bfEDT(void);

// Прервать транзакцию
_BTEXP int bfAT(void);

// Прервать распределенную транзакцию
_BTEXP int bfADT(void);

// Начать транзакцию под заданным клиентом
// [in] lock   - Режим блокировки
// [in] client - Дескриптор клиента
_BTEXP int bfBTEx(int lock, bclient_h client);

// Завершить транзакцию под заданным клиентом
// [in] client - Дескриптор клиента
_BTEXP int bfETEx(bclient_h client);

// Прервать транзакцию под заданным клиентом
// [in] client - Дескриптор клиента
_BTEXP int bfATEx(bclient_h client);

// -----------------------------------------------------------------------------
// Получить позицию текущей записи (в bf->RecPos)
// [in]  bf - Указатель на файл
_BTEXP int bfGP(BTRVFILE *bf);

// Выполнить операцию (вида Binsert, Bupdate, Bdelete, Bstep*, Bget*)
// без учета фильтра (KeyFltr и RecFltr)
// [in] bf - Указатель на файл
// [in] op - Операция (допускаются блокировки)
_BTEXP int bfOp(BTRVFILE *bf, int op);

// Аналог bfOp, но в зависимости от значения bf->truncated в качестве длины записи
// будет использоваться либо длина фиксированной части (bf->fixsize), либо максимальная
// длина записи (bf->MaxRecLen).
// bf->truncated равен true для записей, содержащих переменную часть либо BLOB.
_BTEXP int bfOpG   (BTRVFILE *bf, int op);

// ----------------------------------------------------------------------------
// Общее: функции вида bfGD*2 отличаются от bfGD* аналогов тем, что первые в SQL-режиме
//        работы используют SQL-запросы.

// Получить запись из файла по ее позиции (bf->RecPos)
// [in] bf - Указатель на файл
_BTEXP int bfGD    (BTRVFILE *bf);
_BTEXP int bfGD2   (BTRVFILE *bf);

// Получить запись из файла по ее позиции (bf->RecPos) с учетом фильтра (KeyFltr и RecFltr)
_BTEXP int bfGDEx  (BTRVFILE *bf);
_BTEXP int bfGDEx2 (BTRVFILE *bf);

// Получить запись из файла по ее позиции pos
_BTEXP int bfGDp   (BTRVFILE *bf, recpos_t pos);
_BTEXP int bfGDp2  (BTRVFILE *bf, recpos_t pos);

// Получить запись из файла по ее позиции pos с учетом фильтра (KeyFltr и RecFltr)
_BTEXP int bfGDpEx (BTRVFILE *bf, recpos_t pos);
_BTEXP int bfGDpEx2(BTRVFILE *bf, recpos_t pos);

// Получить запись из файла по ее позиции (bf->RecPos) с блокировкой loc
_BTEXP int bfGDl   (BTRVFILE *bf, int loc);
_BTEXP int bfGDl2  (BTRVFILE *bf, int loc);

// Получить запись из файла по ее позиции pos с блокировкой loc
_BTEXP int bfGDpl  (BTRVFILE *bf, recpos_t pos, int loc);
_BTEXP int bfGDpl2 (BTRVFILE *bf, recpos_t pos, int loc);

// Выполнить операцию (вида Bstep*, Bget* и Bgk*) с учетом фильтра (KeyFltr и RecFltr)
// [in] bf    - Указатель на файл
// [in] GetOp - Операция (допускаются блокировки)
_BTEXP int bfGet   (BTRVFILE *Br, int GetOp);

// -----------------------------------------------------------------------------
// Аналог bfGet с возможностью прерывания длительных операций
typedef bool (*TBreakProc) (void);
_BTEXP int bfGetEx (BTRVFILE *Br, int GetOp, TBreakProc ptr);

// -----------------------------------------------------------------------------
// Вернуть число записей в файле по текущему ключу и фильтру.
// Для SQL-таблицы, если в настроечном файле rsreq.ini определен параметр GETRECCOUNT,
// подсчет записей будет производится по ключу -2.
// [in]  bf - Указатель на файл
// [out] nr - Число записей
_BTEXP int bfNRec(BTRVFILE  *bf, unsigned long *nr);

// -----------------------------------------------------------------------------
// Возвращает число записей по текущему ключу и фильтру.
// bfNRec в зависимости от настроек может вернуть -1
// [in]  bf - Указатель на файл
// [out] nr - Число записей
_BTEXP int bfGetNRec(BTRVFILE *bf, unsigned long *nr);

// -----------------------------------------------------------------------------
// Установить владельца
// [in] pb   - Указатель на файл
// [in] OWN  - Пароль
// [in] type - Тип пароля (SetORW и т.п.)
_BTEXP int bfSetOWN(BTRVFILE *pb, const char *OWN, int type);

// -----------------------------------------------------------------------------
// Создать клон файла
// [in] Nfile   - Название файла
// [in] Nclone  - Название клона
// [in] OWNfile - Пароль файла (владелец файла)
_BTEXP int bfClone(const char *Nfile, const char *Nclone, const char *OWNfile);

// -----------------------------------------------------------------------------
// Создать файл
// [in] fname - Название файла
// [in] data  - Описание структуры файла
// [in] len   - Длина описания
// [in] mode  - Режим
//                 OverFile    - перезаписывать существующий файл
//                 NotOverFile - не перезаписывать существующий файл
_BTEXP int bfCreate(const char *fname, void *data, int *len, int mode);

// -----------------------------------------------------------------------------
// Создать файл по описанию с расширенной информацией (дубль bfCreate)
// [in] fname - Название файла
// [in] data  - Описание структуры файла
// [in] len   - Длина описания
// [in] mode  - Режим
//                 OverFile    - перезаписывать существующий файл
//                 NotOverFile - не перезаписывать существующий файл
_BTEXP int bfCreateEx(const char *fname, void *data, int *len, int mode);

// -----------------------------------------------------------------------------
// Создать файл по описанию  словаре
// [in] fmtName - Имя структуры в словаре
// [in] dicList - Список словарей через ';'
// [in] path    - Путь к создаваемой таблице (вместе с именем таблицы)
_BTEXP int bfCreateFromDic(const char *fmtName, const char *dicList, const char *path);

// -----------------------------------------------------------------------------
typedef struct BtrvSpec
      {
       FileSpecs   *fs;                // описание файла
       KeySpecs    *ks;                // описание ключей
       AltCol      *acs;               // таблицы альтернативной сортировки
      } BtrvSpec;

// Разместить информацию из структуры BtrvSpec в блоке памяти.
// Возвращает требуемый размер блока памяти, или размер использованного блока.
_BTEXP size_t PlaceDataFromBtrvSpec(const BtrvSpec *spec, void *data, size_t len);

// Проверить наличие ключей по подстроке. Учитывая fld_size, вставлять
// дополнительные поля. Возвращает необходимое и достаточное количество полей.
_BTEXP int InsSubFieldInBtrvSpec(BtrvSpec *spec, int fld_size);


// Set BTrieve record/key buffer
// Parameters:
//    bf       -  file object
//    buffer   -  data or key buffer to set. If NULL bf->RecBuf or bf->KeyBuf used (depends on key parameter)
//    len      -  buffer length.
//    key      -  -1 for data buffer, key number for key buffer
//    mode     -  mode == 0 - set fields to zero values; mode > 0  - set buffer to max values ; mode < 0  - set buffer to min values ;
_BTEXP int btrvSetBuffer(BTRVFILE *bf, void *buffer, size_t *len, int key, int mode) ;


// Convert 'Panel field_type' to 'Btrieve Extendet_Key_Type'
// Parametrs:
//    FVt   - значение типа 'Panel field_type'
//    ks    - указатель на структуру KeySpecs для записи результата преобразования
_BTEXP void fvtype2btrv(int FVt, KeySpecs *ks);


_BTEXP int BfConcurrentProhib(int tp);

// defaults:
// repCount  - 5
// delayTime - 500
// randomDelay - 0
// isConcurent - FALSE

_BTEXP int BfUnlock(BTRVFILE *bf, int mode, recpos_t addr);


_BTEXP int BtChkRecGD (BTRVFILE *bf, void *savebuf);
_BTEXP int BtChkRecGDl(BTRVFILE *bf, void *savebuf, int loc);

#define  GD(f, b)  BtChkRecGD((f), (b))

#include <packpshn.h>

// -----------------------------------------------------------------------------
typedef struct tagTBfErrorInfo
      {
       int      _bfstat;
       int      _bfop;
       BTRVFILE *_bfcb;
       char      _bfName[_MAX_PATH];
      } TBfErrorInfo;

#include <packpop.h>

_BTEXP void BfSaveError     (TBfErrorInfo *info);
_BTEXP void BfRestoreError  (TBfErrorInfo *info);

_BTEXP void BfSaveErrorEx   (TBfErrorInfo *info);
_BTEXP void BfRestoreErrorEx(TBfErrorInfo *info);

#define  BfSaveError     BfSaveErrorEx
#define  BfRestoreError  BfRestoreErrorEx


#define  NCC_OPERATION   -10

// Работа с псевдо-BTRVFILE, представляющим МЗП-курсор

// Открыть псевдо-BTRVFILE
_BEDITEXP int bfOpenMdb(BTRVFILE **bFile,        // Выход
                        char      *name,         // Имя структуры
                        char      *dicPathname); // Имя словаря

// Открыть псевдо-BTRVFILE
_BEDITEXP int bfOpenMdbEx(BTRVFILE **bFile,          // Выход
                          char      *name,           // Имя структуры
                          char      *dicPathname,    // Имя словаря
                          bool       useIndexMaps);  // Использовать древовидные индексы на базе std::map

// Открыть псевдо-BTRVFILE (по готовой структуре, с именем)
_BEDITEXP int bfOpenMdbTemplEx(BTRVFILE     **bFile,     // Выход
                               MDB_TEMPLATE  *templ, 
                               char          *name);

// Открыть псевдо-BTRVFILE (по готовой структуре)
_BEDITEXP int bfOpenMdbTempl(BTRVFILE     **bFile,     // Выход
                             MDB_TEMPLATE  *templ);

// Присоединение к Btrieve-файлу в качестве кэша
_BEDITEXP int bfAttach(BTRVFILE *bFile, BTRVFILE *target);

// Отсоединение от Btrieve-файла
_BEDITEXP int bfDetach(BTRVFILE *bFile);

// Получить реальный Btrieve-файл для кэша

_BEDITEXP BTRVFILE *bfBtrvForCache(BTRVFILE *Cache );

// Прекэширование
_BEDITEXP int bfPrecache(BTRVFILE *bFile, bool merge);

typedef int bfprecache_proc(int msg, void *data, void *params);

_BEDITEXP int bfPrecacheEx(BTRVFILE *bFile, bool merge, bfprecache_proc proc, void *params);

// Синхронизация
_BEDITEXP int bfSync(BTRVFILE *bFile, bool noExternalInsertionCheck);

// Синхронизация (с процедурой обработки событий)
typedef int bfsync_proc(int msg, void *data, void *params);

_BEDITEXP int bfSyncEx (BTRVFILE *bFile, bool noExternalInsertionCheck, bfsync_proc proc, void *params);
_BEDITEXP int bfSyncEx2(BTRVFILE *bFile, bool noExternalInsertionCheck, bfsync_proc proc, void *params, bool UseFilters);

// Подтвердить успешное завершение транзакции,
// в рамках которой производилась синхронизация
_BEDITEXP int bfConfirmSync  (BTRVFILE *bFile);
_BEDITEXP int bfConfirmSyncEx(BTRVFILE *bFile, bool UseFilters);

// Очистка
_BEDITEXP int bfClear(BTRVFILE *bFile);

// Очистка текущей области видимости
_BEDITEXP int bfClearView(BTRVFILE *bFile);

// Перенаправление вызовов к кэшу
_BEDITEXP void bfSetRedirect(BTRVFILE *bFile, bool redir);

_BTEXP int BfGetLockFlags(BTRVFILE *bf, int op);

// Opening database dictionaryes list.
// Parameters:
//    ProjList - list of names of database dictionaryes separated by semicolon;
//    dbfilePath - path to database;

_BTEXP BTRVFILE **OpenProjFiles(const char *ProjList, const char *dbfilePath);

// Closing database dictionaryes list.
//
_BTEXP void CloseProjFiles(BTRVFILE **ProjFiles);

// Searching Btrv-file description in database dictionaryes.
// Parameters:
//    Name - Btrvfile name;
//    ProjFiles - list of open database dictionaryes;
// Returns:
//    Pointer on database dictionary, in which found the description
//    of Btrv-file.
//
_BTEXP BTRVFILE *FindProjFiles(const char *Name, BTRVFILE **ProjFiles);

_BTEXP void InitBtrFileList(void);
_BTEXP void DoneBtrFileList(void);

// Функция уведомления о транзакциях
typedef void (trnNotifyProc)(int bop, bclient_h clnt, void *procParm);

// Добавить/Удалить функцию уведомления о транзакциях
_BTEXP void bfAddTrnNotify(trnNotifyProc trnNotify, bclient_h clnt, void *procParm);
_BTEXP void bfDelTrnNotify(trnNotifyProc trnNotify, bclient_h clnt, void *procParm);

// Отправить уведомление о транзакции
_BTEXP void bfSendTrnNotify(int bop, bclient_h clnt);

// Получить имя файла для последней операции
_BTEXP const char *bfGetLastName(void);

_BTEXP char *GetRsdTblName(char *rsdName, const char *btrName);
_BTEXP char *GetRsdFldName(char *rsdName, const char *btrName);

// callback для установки свойств соединения
typedef void (*ConnectionCallback)(void *conn);
_BTEXP void SetConnectionCallBack(ConnectionCallback cbConn);
ConnectionCallback GetConnectionCallback();

#ifdef __cplusplus
}
#endif

// -----------------------------------------------------------------------------
#define  FILL_FILE_SPECS(reclen, pagesize, nindex, fileflag)  BTRV_FILE_SPECS(reclen, pagesize, nindex, fileflag, 0)
#define  FILL_KEY_SPECS(keypos, keylen, keyflag, keytype)     BTRV_KEY_SPECS(keypos, keylen, keyflag, keytype, '\0')


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Интерфейс для сохранения/восстановления информации о состоянии файла

// -----------------------------------------------------------------------------
// Структура элемента связного списка для рекурсивного вызова
typedef struct TMkfPtr
      {
       struct TMkfPtr  *prev;        // Указатель на предыдущий элемент в списке
       void            *saveTV;      // Указатель на буфер сохранения верхнего значения ключа
       void            *saveBV;      // Указатель на буфер сохранения нижнего значения ключа
       size_t           size;        // Размер буферов сохранения значения ключа
       int              saveKey;     // Значение номера ключа
       JBtrFltr         saveKF;      // Указатель на функцию фильтрации записей - ключевой фильтр
       JBtrFltr         saveRF;      // Указатель на функцию фильтрации записей - фильтр по записям
       bool             flag;        // Управляющий флаг
       void            *saveRB;      // Указатель на буфер сохранения записи
       recpos_t         saveRP;      // Позиция в файле
      } TMkfPtr;

// Записываем состояние файла
_BTEXP int BtrvFileMKF(TMkfPtr **curr, BTRVFILE *bfile);

// Восстанавливаем состояние файла
_BTEXP void BtrvFileRKF(TMkfPtr **curr, BTRVFILE *bfile);

// Записываем состояние файла
_BTEXP int BtrvFileMKF2(TMkfPtr **curr, BTRVFILE *bfile, bool flag, bool NeedRecPos = false);

// Восстанавливаем состояние файла
_BTEXP void BtrvFileRKF2(TMkfPtr **curr, BTRVFILE *bfile, bool flag);

// -----------------------------------------------------------------------------
// Класс для работы с BLOB
// -----------------------------------------------------------------------------
class _BTEXP CRSBlob
    {
     private:
       BTRVFILE       *m_pv_bf;

       TBlobOpenMode   m_pv_BlobOpenMode;        // Режим открытия BLOB
       bool            m_pv_OpenBlob;            // Признак открытия BLOB

       bool            m_pv_MustClose;           // BLOB нужно будет закрыть

       // ----------------------------------------
     protected:
       void _Init(void);

     public:
       // ----------------------------------------
       CRSBlob(BTRVFILE *bf = NULL);
       CRSBlob(BTRVFILE *bf, TBlobOpenMode openMode);
      ~CRSBlob();

       // ----------------------------------------
       BTRVFILE *Attach(BTRVFILE *bf);

       // ----------------------------------------
       // Получить состояние BLOB
       TBlobOpenMode State(void);

       // ----------------------------------------
       // Открыть BLOB
       int Open(TBlobOpenMode openMode = BLOB_OPEN_RDWR);

       // ----------------------------------------
       // Закрыть BLOB
       int Close(void);

       // ----------------------------------------
       // Прочитать данные из BLOB
       int Read(char *buff, int size);

       // ----------------------------------------
       // Записать данные в BLOB
       int Write(char *buff, int size);

       // ----------------------------------------
       // Установить позицию
       int SetPos(unsigned long pos);

       // ----------------------------------------
       // Получить позиуию
       int GetPos(unsigned long *pos);

       // ----------------------------------------
       // Получить размер BLOB'а
       int GetSize(unsigned long *size);

       // ----------------------------------------
       // Обрезать BLOB
       int Truncate(void);

       // ----------------------------------------
       // Задать тип
       int SetType(unsigned char tp);

       // ----------------------------------------
       // Перемотать на начало
       int Rewind(void);
    };

/************************* End Of File **********************************/

#endif // __BTRV_H

/* <-- EOF --> */