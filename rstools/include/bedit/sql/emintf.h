//-*--------------------------------------------------------------------------*-
//
// File Name   : emintf.h
//
// Copyright (c) 1991 - 2004 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// November 17,2004  Sergei Kubrin (K78) - Create file
//-*--------------------------------------------------------------------------*-

//
// Declaration of the emulator export interface.
//

#ifndef __EMINTF_H
#define __EMINTF_H

#include "platform.h"
#include "na/naoplog_type.h"

#ifdef __cplusplus
    class CRsdConnection;
#else
    #define  CRsdConnection  void*
#endif

// -----------------------------------------------
typedef struct tagSqlFileGetInfo
      {
       int   recSize;
       int   keySize;
       long  maxvarlen;
       char  reserved[10];
      } TSqlFileGetInfo;

// -----------------------------------------------
typedef struct tagSqlFileSetInfo
      {
       void *recBuf;
       void *keyBuf;
       int   mode;
       char  reserved[10];
      } TSqlFileSetInfo;

// -----------------------------------------------
typedef struct tagSqlSessionInfo
      {
       short  sid;
       long   serial;
       long   audsid;
       char   reserved[8];
      } TSqlSessionInfo;

// -----------------------------------------------
typedef struct tagSqlMenuTreeItem
      {
       short  oper;
       short  program;
       short  point;
       char   reserved[10];
      } TSqlMenuTreeItem;

#ifndef EM_BUILD
    typedef struct tagSqlColumnInfo
          {
           long  type;
           int   size;
          } TSqlColumnInfo;
#else
    typedef  TExprInfo  TSqlColumnInfo;
#endif

#ifndef BKMHDEFINED
#define BKMHDEFINED
    RSL_MAKE_HANDLE(BKMHANDLE);
#endif

RSL_MAKE_HANDLE(SQLFILEHANDLE);
RSL_MAKE_HANDLE(RECORDSETHANDLE);
RSL_MAKE_HANDLE(SORTINFOHANDLE);
RSL_MAKE_HANDLE(LINKINFOHANDLE);
RSL_MAKE_HANDLE(SQLINFOEXHANDLE);

// -----------------------------------------------
typedef struct tagToolsIntf
      {
       char *(*valtostr)(void *val, int w, int p, unsigned int type, int *size, int ljust);

       void (*strtoval)(char *s, void *V, int p, unsigned int type, int size);
      } TToolsIntf;

// -----------------------------------------------
typedef struct tagEmIntf
      {
       int   (*version)(void);

       void  (*sfReset)        (SQLFILEHANDLE sf);

       void  (*sfSetToolsWhere)(SQLFILEHANDLE sf, const char *toolsWhere);
       void  (*sfSetCacheMode) (SQLFILEHANDLE sf, int  fullShift);
       void  (*sfSkipBmSearch) (SQLFILEHANDLE sf, bool bSkip);

       int   (*openEmEx)(const char *connStr);

       void  (*addBtemRef)(btrvem_h handle);

       int   (*releaseBmk)(BKMHANDLE pBmk);
       int   (*addRefBmk)(BKMHANDLE pBmk);
       bool  (*EmCompareBmk)(BKMHANDLE left, BKMHANDLE right);


       char *(*GetRsdTableName)(char *rsdName, const char *btrName);
       char *(*GetRsdFieldName)(char *rsdName, const char *btrName);


       int   (*SqlCallMt)(int op, SQLFILEHANDLE sf, void *data_buf, int *data_len, void *key_buf,
                          int key_len, signed char key_num, btrvem_h ehandle);

       int   (*addLink2Mt)(SQLFILEHANDLE sf, const char *depFields, const char *depFiles,
                           const char *joinCond, const char *whereCond, void **array,
                           int arr_size, btrvem_h ehandle);

       int   (*addLinkParam)(SQLFILEHANDLE sf, long type, void *val, int size, btrvem_h ehandle);

       int   (*dropLinkMt)    (SQLFILEHANDLE sf, btrvem_h ehandle);
       int   (*addUserSortMt) (SQLFILEHANDLE sf, const char *strSort, btrvem_h ehandle);
       int   (*dropUserSortMt)(SQLFILEHANDLE sf, btrvem_h ehandle);

       int   (*sfSetCursorMt)(SQLFILEHANDLE sf, int crsType, bool bSync, btrvem_h ehandle);

       int   (*sfGetCursorType)(SQLFILEHANDLE sf);

       void  (*sfGetLinkInfoMt)(SQLFILEHANDLE sf, LINKINFOHANDLE *info, btrvem_h ehandle);
       void  (*sfFreeLinkInfoMt)(LINKINFOHANDLE *info, btrvem_h ehandle);

       void  (*sfGetSortInfoMt)(SQLFILEHANDLE sf, SORTINFOHANDLE *info, btrvem_h ehandle);
       void  (*sfFreeSortInfoMt)(SORTINFOHANDLE *info, btrvem_h ehandle);

       void  (*sfCancelQueryMt)(btrvem_h ehandle);
       int   (*sfSetCachingMt)(SQLFILEHANDLE sf, bool val, btrvem_h ehandle);

       int   (*sfNewExMt)(const char *tableName, const char *strName, SQLFILEHANDLE *pSf, btrvem_h ehandle);

       int   (*sfInitMt)(const char *tableName, SQLFILEHANDLE sf,  btrvem_h ehandle);
       int   (*sfInitByRsdMt)(RECORDSETHANDLE rs, SQLFILEHANDLE sf, btrvem_h ehandle);

       int   (*sqlOpenMt)(SQLFILEHANDLE sf, long cacheSize, btrvem_h ehandle);
       int   (*sqlCloseMt)(SQLFILEHANDLE sf, btrvem_h ehandle);
       int   (*sqlDeleteMt)(SQLFILEHANDLE *pSf, btrvem_h ehandle);

       btrvem_h (*openEmMtEx)(const char *connStr, const char *name);
       btrvem_h (*findDb)(const char *name);

       int   (*closeEmMt) (btrvem_h ehandle);
       bool  (*isOpenEmMt)(btrvem_h ehandle);
       bool  (*isConnEmMt)(btrvem_h ehandle);

       bool  (*IsFmtExistMt)(const char *name, btrvem_h ehandle);
       bool  (*IsTblExistMt)(const char *name, btrvem_h ehandle);

       int   (*copyDbtMt)(const char *nameIn, const char *nameOut, btrvem_h ehandle);
       int   (*sqlCloneMt)(const char *Nfile, const char *Nclone, btrvem_h ehandle);

       int   (*sqlCloneExMt)(const char *Nfile, const char *Nclone, bool permanent, btrvem_h ehandle);

       int   (*sqlDropMt)(const char *tName, btrvem_h ehandle);

       RSDICT_FMT *(*getFmtMt)(const char *name, btrvem_h ehandle);

       void *(*getDefaultConnectionMt)(btrvem_h ehandle);
       void *(*getConnectionIntfMt)   (btrvem_h ehandle);
       void *(*getConnectionHandleMt) (btrvem_h ehandle);

       void  (*GetRsdErrorInfoMt)(char *errInfo, btrvem_h ehandle);
       void  (*LoadTabInfoMt)(char **tabList, int count, btrvem_h ehandle);

       int   (*SetOptmModeMt)(SQLFILEHANDLE  sf,
                              int            qt,         // тип запросов, для которых устанавливается режим
                              int            om,         // режим оптимизации
                              const char    *hint,       // подсказки SQL
                              btrvem_h       ehandle);

       void  (*setEmPropertyMt)(int prop, int val, btrvem_h ehandle);
       void  (*setSfPropertyMt)(SQLFILEHANDLE sf, int prop, int val);

       void  (*DoneRsdBtEm)(void);

       void  (*GetSqlFileInfo)(SQLFILEHANDLE hSql, TSqlFileGetInfo *info);
       void  (*SetSqlFileInfo)(SQLFILEHANDLE hSql, TSqlFileSetInfo *info);

       char *(*GetAllRecordsSql)(SQLFILEHANDLE hSql, char *sql, int crsType, bool useKf,
                                 int keyNum, btrvem_h ehandle);

       int   (*GetPosByBuffer)(void *hSql, void *pos, btrvem_h ehandle);

       int   (*GetSqlSessionInfo)(void *pConnection, TSqlSessionInfo *info);
       int   (*DeleteInactiveRecord)(void *pConnection);

       int   (*GetRecIdFromOperlog)(void *pConnection, const char *table, int dprt, long *recID);

       int   (*LoadMenuSql)(void *pConnection, void **menu, int *n, int oper, int program);
       void  (*FreeMenuSql)(void *pConnection, void *menu, int n);

       int   (*DeleteMenuSql)(void *pConnection, int oper, int program);
       int   (*BorrowMenuSql)(void *pConnection, int oper, int program, int fromOper);

       int   (*LoadMenuTree)(void *pConnection, int oper, int program, int point, bool upward,
                             int (addFunc)(void *parm, int level, const TSqlMenuTreeItem *item),
                             void *parm);

       CRsdConnection *(*getDefConnection)();

       const char *(*SetHint)(
                              SQLFILEHANDLE  sf,
                              int            crsType,// тип курсора, для которого устанавливается режим
                              int            qt,     // тип запросов, для которых устанавливается режим
                              int            all,    // применить ко всем курсорам в стеке
                              const char    *hint,   // подсказки SQL
                              char          *oldHint,
                              btrvem_h       ehandle
                             );

       void  (*getSfPropertyMt)(SQLFILEHANDLE sf, int prop, int *val);
       void  (*getEmPropertyMt)(int prop, int *val, btrvem_h ehandle);

       int   (*addLinkExMt)(SQLFILEHANDLE sf, const char *depFields, const char *depFiles,
                            const char *joinCond, const char *whereCond, void **array, int arr_size,
                            TSqlColumnInfo *colinfo, int colinfo_size, btrvem_h ehandle);

       void  (*sfGetSqlInfoExMt)(SQLFILEHANDLE sf, SQLINFOEXHANDLE *info, int crsType,
                                 bool useKf, int keyNum, btrvem_h ehandle);

       void  (*sfFreeSqlInfoExMt)(SQLINFOEXHANDLE *info, btrvem_h ehandle);
       void  (*sfSetBlobBuffer)(SQLFILEHANDLE sf, void *buf);

       RECORDSETHANDLE (*sfGetRecordsetFromRSL)(RECORDSETHANDLE rs);

       int   (*SetToolsIntf)(TToolsIntf *intf);

       int   (*OperlogInsert)(SQLFILEHANDLE oplog, TOperLog_type::opType optype, const char *filename, void *buf, int len, btrvem_h ehandle, bool add_dxml, int xml_format);
       RSDICT_FMT *(*getFmtExMt)(const char *name, bool rsdName, btrvem_h ehandle);

       bool  (*Xml2Buffer)(SQLFILEHANDLE oplog, void *RecBuf, btrvem_h ehandle, bool add_dxml, int xml_format);

       int   (*HookJdbcDLL)(CRsdConnection *con);

       int   (*GetPrevRecFromOperlog)(char *table, void *pConnection, int numDprt, int recID, int16 *ret_numDprt, uint32 *ret_crc32, int32 *ret_recID);
       
       bool  (*Xml2String_for_CRC32)(SQLFILEHANDLE oplog, char **strBuf, btrvem_h ehandle, bool add_dxml, int xml_format);

      } TEmInterface;

// -----------------------------------------------
#ifdef EM_BUILD
    #define  RSDEMLINK  __declspec(dllexport)
#else
    #define  RSDEMLINK  __declspec(dllimport)
#endif

// -----------------------------------------------
#ifdef __TOOLS_BUILD
    #define  RSDEM  __declspec(dllexport)
#else
    #if !defined(EM_BUILD) && !defined(RSL_BUILD_DLL)
        #define  RSDEM  __declspec(dllimport)
    #else
        #define  RSDEM
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

RSDEMLINK TEmInterface *EmGetInterface(void);

int RsdInitEmulatorIntf(void);
int RsdDoneEmulatorIntf(void);


RSDEM void sfReset(SQLFILEHANDLE sf);

RSDEM void sfSetToolsWhere(SQLFILEHANDLE sf, const char *toolsWhere);
RSDEM void sfSetBlobBuffer(SQLFILEHANDLE sf, void *buf);
RSDEM void sfSetCacheMode (SQLFILEHANDLE sf, int  fullShift);
RSDEM void sfSkipBmSearch (SQLFILEHANDLE sf, bool bSkip);

RSDEM int  openEmEx(const char *connStr);
RSDEM void addBtemRef(btrvem_h handle);

RSDEM int  releaseBmk  (BKMHANDLE pBmk);
RSDEM int  addRefBmk   (BKMHANDLE pBmk);
RSDEM bool EmCompareBmk(BKMHANDLE left, BKMHANDLE right);


RSDEM char *GetRsdTableName(char *rsdName, const char *btrName);
RSDEM char *GetRsdFieldName(char *rsdName, const char *btrName);

RSDEM int SqlCallMt(int op, SQLFILEHANDLE sf, void *data_buf, int *data_len, void *key_buf, int key_len,
                    signed char key_num, btrvem_h ehandle);

RSDEM int addLink2Mt(SQLFILEHANDLE sf, const char *depFields, const char *depFiles, const char *joinCond,
                     const char *whereCond, void **array, int arr_size, btrvem_h ehandle);

RSDEM int addLinkExMt(SQLFILEHANDLE sf, const char *depFields, const char *depFiles, const char *joinCond,
                      const char *whereCond, void **array, int arr_size, TSqlColumnInfo *colinfo,
                      int colinfo_size, btrvem_h ehandle);

RSDEM int addLinkParam(SQLFILEHANDLE sf, long type, void *val, int size, btrvem_h ehandle);

RSDEM int dropLinkMt    (SQLFILEHANDLE sf, btrvem_h ehandle);
RSDEM int addUserSortMt (SQLFILEHANDLE sf, const char *strSort, btrvem_h ehandle);
RSDEM int dropUserSortMt(SQLFILEHANDLE sf, btrvem_h ehandle);

RSDEM int sfSetCursorMt(SQLFILEHANDLE sf, int crsType, bool bSync, btrvem_h ehandle);

RSDEM int sfGetCursorType(SQLFILEHANDLE sf);

RSDEM void sfGetLinkInfoMt(SQLFILEHANDLE sf, LINKINFOHANDLE *info, btrvem_h ehandle);
RSDEM void sfFreeLinkInfoMt(LINKINFOHANDLE *info, btrvem_h ehandle);

RSDEM void sfGetSqlInfoExMt(SQLFILEHANDLE sf, SQLINFOEXHANDLE *info, int crsType,
                            bool useKf, int keyNum, btrvem_h ehandle);

RSDEM void sfFreeSqlInfoExMt(SQLINFOEXHANDLE *info, btrvem_h ehandle);

RSDEM void sfGetSortInfoMt(SQLFILEHANDLE sf, SORTINFOHANDLE *info, btrvem_h ehandle);
RSDEM void sfFreeSortInfoMt(SORTINFOHANDLE *info, btrvem_h ehandle);

RSDEM void sfCancelQueryMt(btrvem_h ehandle);
RSDEM int  sfSetCachingMt(SQLFILEHANDLE sf, bool val, btrvem_h ehandle);

RSDEM int sfNewExMt(const char *tableName, const char *strName, SQLFILEHANDLE *pSf, btrvem_h ehandle);

RSDEM int sfInitMt(const char *tableName, SQLFILEHANDLE sf, btrvem_h ehandle);
RSDEM int sfInitByRsdMt(RECORDSETHANDLE rs, SQLFILEHANDLE sf, btrvem_h ehandle);
RSDEM RECORDSETHANDLE  sfGetRecordsetFromRSL(RECORDSETHANDLE rs);

RSDEM int  sqlOpenMt(SQLFILEHANDLE sf, long cacheSize, btrvem_h ehandle);
RSDEM int  sqlCloseMt(SQLFILEHANDLE sf, btrvem_h ehandle);
RSDEM int  sqlDeleteMt(SQLFILEHANDLE *pSf, btrvem_h ehandle);

RSDEM btrvem_h openEmMtEx(const char *connStr, const char *name);
RSDEM btrvem_h findDb(const char *name);

RSDEM int  closeEmMt (btrvem_h ehandle);
RSDEM bool isOpenEmMt(btrvem_h ehandle);
RSDEM bool isConnEmMt(btrvem_h ehandle);

RSDEM bool IsFmtExistMt(const char *name, btrvem_h ehandle);
RSDEM bool IsTblExistMt(const char *name, btrvem_h ehandle);

RSDEM int copyDbtMt   (const char *nameIn, const char *nameOut, btrvem_h ehandle);
RSDEM int sqlCloneMt  (const char *Nfile, const char *Nclone, btrvem_h ehandle);
RSDEM int sqlCloneExMt(const char *Nfile, const char *Nclone, bool permanent, btrvem_h ehandle);
RSDEM int sqlDropMt   (const char *tName, btrvem_h ehandle);

RSDEM RSDICT_FMT *getFmtMt(const char *name, btrvem_h ehandle);
RSDEM RSDICT_FMT *getFmtExMt(const char *name, bool isRsdName, btrvem_h ehandle);

RSDEM void *getDefaultConnectionMt(btrvem_h ehandle);
RSDEM void *getConnectionIntfMt   (btrvem_h ehandle);
RSDEM void *getConnectionHandleMt (btrvem_h ehandle);

RSDEM void GetRsdErrorInfoMt(char *errInfo, btrvem_h ehandle);
RSDEM void LoadTabInfoMt(char **tabList, int count, btrvem_h ehandle);

RSDEM int SetOptmModeMt(SQLFILEHANDLE  sf,
                        int            qt,         // тип запросов, для которых устанавливается режим
                        int            om,         // режим оптимизации
                        const char    *hint,       // подсказки SQL
                        btrvem_h       ehandle);

RSDEM const char *SetHint(SQLFILEHANDLE  sf,
                          int            crsType,// тип курсора, для которого устанавливается режим
                          int            qt,     // тип запросов, для которых устанавливается режим
                          int            all,    // применить ко всем курсорам в стеке
                          const char    *hint,   // подсказки SQL
                          char          *oldHint,
                          btrvem_h       ehandle
                         );

RSDEM void setEmPropertyMt(int prop, int val, btrvem_h ehandle);
RSDEM void getEmPropertyMt(int prop, int *val, btrvem_h ehandle);
RSDEM void setSfPropertyMt(SQLFILEHANDLE sf, int prop, int val);
RSDEM void getSfPropertyMt(SQLFILEHANDLE sf, int prop, int *val);

RSDEM void DoneRsdBtEm();

RSDEM void GetSqlFileInfo(SQLFILEHANDLE hSql, TSqlFileGetInfo *info);
RSDEM void SetSqlFileInfo(SQLFILEHANDLE hSql, TSqlFileSetInfo *info);

RSDEM char *GetAllRecordsSql(SQLFILEHANDLE hSql, char *sql, int crsType, bool useKf,
                             int keyNum, btrvem_h ehandle);

RSDEM int GetPosByBuffer(void *hSql, void *pos, btrvem_h ehandle);

RSDEM CRsdConnection *getDefConnection();

RSDEM int   OperlogInsert(SQLFILEHANDLE oplog, TOperLog_type::opType optype, const char *filename, void *buf, int len, btrvem_h ehandle, int xml_format);
RSDEM bool  Xml2Buffer   (SQLFILEHANDLE oplog, void *RecBuf, btrvem_h ehandle, int xml_format);
RSDEM bool  Xml2String_for_CRC32(SQLFILEHANDLE oplog, char **strBuf, btrvem_h ehandle, int xml_format);

int GetSqlSessionInfo(void *pConnection, TSqlSessionInfo *info);
int DeleteInactiveRecord(void *pConnection);

int GetRecIdFromOperlog(void *pConnection, const char *table, int dprt, long *recID);
int GetPrevRecFromOperlog(char *table, void *pConnection, int numDprt, int recID, int16 *ret_numDprt, uint32 *ret_crc32, int32 *ret_recID);

int  LoadMenuSql(void *pConnection, void **menu, int *n, int oper, int program);
void FreeMenuSql(void *pConnection, void *menu, int n);

int DeleteMenuSql(void *pConnection, int oper, int program);
int BorrowMenuSql(void *pConnection, int oper, int program, int fromOper);

int LoadMenuTree(void *pConnection, int oper, int program, int point, bool upward,
int (addFunc)(void *parm, int level, const TSqlMenuTreeItem *item), void *parm);

int SetJdbcHook(void *con);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // __EMINTF_H

/* <-- EOF --> */