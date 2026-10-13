/*-----------------------------------------------------------------------*-

 File Name   : fsmes.h   

 Sybsystem   :   
 Description : Terminal extantion support

 !!!!!!!!!!!!  To create DLM use DLM SDK !!!!!!

 Source      : 

 Library(s)  : 


 Copyright (c) 1991 - 1994 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 October 6,1997  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __FSMES_H
#define __FSMES_H

#include "platform.h"
#include "pexpdef.h"

#ifndef __EXT_H
#include "dlm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RSL_MD_FLAT
    #ifdef RSAPPSERV
        #define  APPEXP  DLMAPIC
        #define  EXP32   __declspec(dllimport)
    #else
        #define  APPEXP  DLMAPIC
        #define  EXP32   __declspec(dllexport)
    #endif
#else
    #ifdef RSAPPSERV
        #define  APPEXP  DLMAPIC IMP
        #define  EXP32
    #else
        #define  APPEXP  DLMAPIC EXP
        #define  EXP32
    #endif
#endif

//=========================================================================
//   Функции экспортируемые из term.exe 

void MsgBox(const char **str, int nStr);
void ShowMessage(const char **str, int nStr);
int  ReadIniFile(char *name, int (DLMAPIC *fun)(char *key, char *name, void *data), void *data);
int  RunProg(
             char *program,   /*  Запускаемая программа                       */
             char *com,       /*  Командная строка                            */
             char *initmes,   /*  Стартовое сообщение  или  NULL              */
             char *pausemes,  /*  Заключительное сообщение и пауза или NULL   */
             int   swap_size  /*  Размер в KB необходимый для программы       */
           );

// Функции аналогичные стандартным, но для работы с русскими буквами          
int   toupc     (int ch);
int   stricmpR  (char *str1, char *str2);
int   strnicmpR (char *str1, char *str2, int len);
int   toDownc   (int ch);
char *struprR   (char *str);
char *strnuprR  (char *str, int len);
char *strlwrR   (char *str);
char *strnlwrR  (char *str, int len);

char *MakeFirstPath(char *Buff, char *file, char *IncDir, char *ext);
int   FindIncFile(char *Buff, char *file, char *IncPath, char *ext, int fCurDir);
char *RunPath(char *name);  /*  name должна вмещать полный путь */

void  RslSplitFile(const char *fname, char *dir, char *name, char *ext);
char *RslMergeFile(char *buff, const char *dir, const char *name, const char *ext);
int   TestExistFile(const char *file);

EXP32 int APPEXP RslExtMessageProc(int cmd, char *inMes, char *outMes);

#define  RSL_EXT_MES_TABLE    EXP32 int APPEXP RslExtMessageProc(int cmd, char *inMes, char *outMes)   \
                                               { RslGetInstCount(); switch (cmd) { default: return 0;

#define  RSL_EXT_HANDLE(msg)  case msg: return mes_##msg (inMes, outMes);

#define  RSL_EXT_MES_LOAD     case -1:  mes_Load (); return 0; 

#define  RSL_EXT_MES_UNLOAD   case -2:  mes_Unload (); return 0; 


#ifdef RSL_MD_FLAT
    #define  RSL_EXT_MES_END   }}
#else
    #define  RSL_EXT_MES_END   }}                                                 \
             ENTRY  cmdTable[] = {                                                \
              { "_RslExtMessageProc", RslExtMessageProc },                        \
              { "RslSetCallBacks"   , RslSetCallBacks   },                        \
              { NULL, NULL } };                                                   \
              void *malloc(size_t sz) { return _EXT_GetMem (sz); }                \
              void huge *farmalloc(unsigned long sz) { return _EXT_GetMem (sz); } \
              void free(void *ptr) { _EXT_FreeMem(ptr); }                         \
              void farfree(void huge *ptr) {_EXT_FreeMem(ptr); }                  \
              int DLMAPI EXP ExitLib(void) { return 0; }                          \
              int DLMAPI EXP InitLib(void) { return 0; }                          

#endif

#ifdef __cplusplus
}
#endif

#endif // __FSMES_H

/* <-- EOF --> */