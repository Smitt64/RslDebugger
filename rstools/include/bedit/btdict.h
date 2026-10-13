/*************************************************************************
   File Name      : btdict.h
   Programmer     : Alexey Lebedev

   Creation       : 29 Aug 2002
   Description    : Доступ к словарной системе

   Copyright(C) 2002 by R-Style Software Lab.
   All Rights Reserved.

*************************************************************************/

#ifndef __BTDICT_H
#define __BTDICT_H

typedef struct tagBTKeyInfo   BTKeyInfo;
typedef struct tagBTFileInfo  BTFileInfo;
typedef struct tagBTFieldInfo BTFieldInfo;

#include "btrv.h"

#include "packpshn.h"

// Описание файла базы данных в словарной системе
struct tagBTFileInfo
     {
      char          *FileName;
      char          *FileOwner;
      char          *FileDesc;
      unsigned short FileFlags;
      unsigned short OwnerType;
      unsigned short MaxVarLen;
      unsigned short RecordLen;
      unsigned char  BLOBFlag;
      unsigned char  Reserve;

      char          *Dictionary; // имя словаря БД

      unsigned short nFields;    // число полей (размер массива Fields)
      unsigned short nKeys;      // число сегментов ключей (размер массива Keys)

      BTFieldInfo   *Fields;
      BTKeyInfo     *Keys;
      unsigned short PageSize;
     };

// Описание поля в словарной системе
struct tagBTFieldInfo
     {
      char          *FldName;    // название поля
      char          *FldDesc;    // описание поля
      unsigned short FldType;    // тип поля FT_ (enum TFVT)
      unsigned short FldSize;    // размер поля в байтах
      unsigned short FldOffset;  // смещение поля в байтах от начала записи
      unsigned short FldLength;  // длина поля для вывода
      short          FldPoint;   // позиция точки при выводе поля
      unsigned char  Hidden;     // не показывать поле в скроллингах
      unsigned char  Reserve;
     };

// Описание ключа в словарной системе
struct tagBTKeyInfo
     {
      unsigned short FldNum;     // номер поля (индекс в массиве Fields)
      unsigned short KeyNum;     // номер ключа, все сегменты в ключе имеют
                                 // одинаковый номер
      unsigned short KeyType;    // тип ключа (nwbtrv.h - extendet key type)
      unsigned short KeySize;    // размер ключа в байтах (мб меньше FldSize)
      unsigned short KeyFlags;   // флаги ключа (nwbtrv.h - enum Key_Flags)
      char           KeyNull;    // нулевое значение
      unsigned short Reserve;
     };

#include "packpop.h"

/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

_BTEXP void InitBtDictionary(void);
_BTEXP void DoneBtDictionary(void);

// Задать список словарей БД
// Параметры: szDicList - список словарей, разделенный ';'
_BTEXP void BtSetDictionary(const char *szDicList);

// Задать список каталогов для поиска словарей
_BTEXP void BtSetDatabasePath(const char *szDataPath);

// Получаем список словарей, разделенный символами ‘;’ и возвращаем длину результирующей строки.
// Если параметр szDicList равен NULL, то функция только возвращает длину результирующей строки.
_BTEXP int BtGetDictionary(char *szDicList);

// Добавляем словарь в список. В случае успеха возвращает 0.
// Если такой словарь уже имеется в списке, функция возвращает 1.
// В случае ошибки возвращается -1.
// В szDic должен быть указан только один словарь.
_BTEXP int BtAddDictionary(const char *szDic);

// Удаляем словарь из списка. В случае успеха возвращает 0.
// Если словарь отсутствует в списке, функция возвращает 1. Если удаляемый словарь открыт,
// будет выполнено его закрытие.
// В случае ошибки возвращается -1.
// В szDic должен быть указан только один словарь.
_BTEXP int BtRemoveDictionary(const char *szDic);

// Проверяем наличие словаря в списке. Если словарь присутствует, возвращаем 0. Иначе 1.
// В случае ошибки возвращается -1.
// В szDic должен быть указан только один словарь.
_BTEXP int BtCheckDictionary(const char *szDic);


// Флаги описания файла
enum BtFileInfoFlags
   {
    OnlyFileInfo = 1,
   };

// Получить описание файла
_BTEXP BTFileInfo *BtGetFileInfo(const char *szFileName, int Flags);
_BTEXP BTFileInfo *BtGetFileInfo2(BTRVFILE *bf);

// Освободить описание файла
_BTEXP void BtFreeFileInfo (BTFileInfo**);
_BTEXP void BtFreeFileInfo2 (BTRVFILE* bf, BTFileInfo**);

// Очистить кэш с описаниями
_BTEXP void BtClearCache();

// Получить статическое описание словаря
_BTEXP BTFileInfo *BtGetDictFileInfo(const void *rbuf, size_t rlen);

// Проверить, является ли переданное описание словарем БД
_BTEXP int IsBtDictFileInfo(const BTFileInfo *fileInfo);

// Получить значение подкачиваемого поля
_BTEXP int BtGetDictFldVal(const char *dict, const void *rec, size_t offset, char *buf, size_t sz);

// Заполнить буфер в соответствии с описанием файла в словаре
_BTEXP int BtSetDataBuffer(BTFileInfo *fileInfo, int key, int mode, void *buf, size_t *len);

#ifdef __cplusplus
}
#endif

#endif // __BTDICT_H

/* <-- EOF --> */