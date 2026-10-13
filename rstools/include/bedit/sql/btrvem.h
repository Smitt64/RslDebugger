// -------------------------------------------------------------------------------------------------

#ifndef __BTRVEM_H
#define __BTRVEM_H

#include "rsfmt.h"

// -----------------------------------------------
class CRsdRecordset;
class CRsdAccessor;
class AccCollection;
class bm_manager;

struct tagRSDRSNAMEBINDING;

// -----------------------------------------------
enum crttabtype
   {
    tempbyfs   = 1,  // временная по FileStat
    tempbyfmt  = 2,  // временная по RSDDICT_FMT
    tabbyname  = 3,  // постоянная по имени (RSDDICT_FMT ищется по имени)
    tabbynfmt  = 4   // постоянная по имени и RSDDICT_FMT        
   };

// -----------------------------------------------
typedef tagRSDRSNAMEBINDING   *RSDFIELDSDEF;
typedef tagRSDRSNAMEBINDING  **RSDKEYSDEF;

// -----------------------------------------------
enum SFFLAGS
   {
    sfflocked  =  0x01,  // lock флаг
    sffinvcrec =  0x02,  // запись удалена и текущая позиция невалидна
    sfftmptab  =  0x04,  // таблица временная
    sffsingrec =  0x08,  // при поиске выбирать только одну запись.
    sffunused1 =  0x10,  // 
    sffreset   =  0x20,  // сбросить кэш при перемене курсоров    
    sffrealqry =  0x40,  // не использовать кэш
    sffunused2 =  0x80,  // 
    sffusekf   =  0x100, // использовать ключевой фильтр
    sffuseoptc =  0x200, // использовать условия addlink (исп.только в инструменте)
    sffusesrtc =  0x400, // использовать пользовательскую сортировку 
    sffnupdacc =  0x800, // не обновлять буфер записи
    sffrsdtab  = 0x1000  // SQLFILE на основе рекордсета RSD
   };

// -----------------------------------------------
enum SFPROPERTY
   {
    SF_AUTOUPDATE_RECBUF = 1,
    SF_OPEN_MODE         = 2
   };

// -----------------------------------------------
typedef struct tagSQLFILE
      {
       char           *fileName;       // имя таблицы

       tagRSDRSNAMEBINDING  *primaryKey;
       tagRSDRSNAMEBINDING **KeyDef;
       tagRSDRSNAMEBINDING  *fileDef;        

       CRsdRecordset  *pMulti;         // указатель на Multi

       CRsdAccessor   *curAcc;         // аксессор, соответствующий текущему ключу
       CRsdAccessor   *updAcc;         // аксессор, через который проходит обновление и вставка 
                                           // на самом деле это аксессор для буфера записи
       CRsdAccessor   *pkAcc;          // аксессор, которым будет обновляться буфер уникального ключа
       
       void           *recBuf;         // указатель на буфер записи
       void           *keyBuf;         // указатель на буфер ключа
       void           *pkBuf;          // буфер уникального ключа
       void           *topVal;         // верхнее значение ключа
       void           *botVal;         // нижнее  значение ключа

       void           *hints;          // подсказки для оптимизации

       AccCollection  *accessors;      // коллекция для хранения акссессоров.
       bm_manager     *bmk_manager;    // управление bookmark'ами

       void           *status;         // статусы
       
       RSDICT_FMT     *fmt;            // указатель на формат, сейчас используется для быстрого доступа к флагам ключ
       
       void           *mainSf;         // для хранения основного курсора
       void           *optmSf;         // для хранения оптимизированного курсора
       void           *sortSf;         // для хранения ПС-курсора
       void           *nextSf;         // для стека курсоров

       int             crsType;        // тип курсора

       long            tid;            // идентификатор таблицы

       long            maxvarlen;
       int             recSize;
       int             keySize;
       int             pkSize;

       int             nkeys;
       int             nfields;
       int             curKey;         // текущий ключ

       int             mode;           // режим открытия
       int             nAuto;          // автоинкрементное поле, если 0, то таблица без автоинкремента
       long            flags;          // SFFLAGS
       void           *sortInfo;
       void           *lastpSf;        // курсор с последней позициией
       void           *sort_columns;
       void           *varBufPtr;      // переменная часть записи отдельно от recBuf    

       char            reserve[8];    // резерв
      } SQLFILE;

#include "emintf.h"

#endif __BTRVEM_H

/* <-- EOF --> */