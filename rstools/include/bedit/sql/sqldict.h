#ifndef __SQLDICT_H
#define __SQLDICT_H

#include "btdict.h"
#include "sql/btrvem.h"

/*************************************************************************
                        class SqlDicFile
*************************************************************************/
class SqlDicFile
{
 // Construction
 public:
   SqlDicFile();
  ~SqlDicFile();

   // Operations

   // Загрузить описание файла
   BTFileInfo *GetFileInfo(const char *szFileName, int Flags);
   BTFileInfo *GetFileInfo(BTRVFILE *bf);
   void FreeFileInfo(BTFileInfo *info);

   void reset();
   void calcSizes(RSDICT_FMT *fmt, bool bPrfxName);

   // Implementation
 protected:
   char *FillFileInfo (BTFileInfo *ptr, char *txtPtr, const char *szFileName, RSDICT_FMT *fmt);
   void  FillFieldInfo(BTFileInfo *ptr, char *txtPtr, RSDICT_FMT *fmt, bool bPrfxName);
   void  FillKeyInfo  (BTFileInfo *ptr, RSDICT_FMT *fmt);

   inline char *CopyText(char *dst, const char *src, const char **next);

 private:
   BTFileInfo *_getFileInfo(RSDICT_FMT *fmt, const char *fname, int Flags, bool bPrfxName);

   int  m_TextLen;
   int  m_nSeg;
   int  m_nKeys;
   int  m_nFields;
   int  m_recLen;

   bool m_onlyFI;
};

#endif // __SQLDICT_H

/* <-- EOF --> */