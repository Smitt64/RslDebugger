/********************************************************************
   Created: 2002/03/27
   Created: 27:3:2002   16:04
   Filename: D:\Work\RSFormatsC\RSFmtIntfC.h
   Project: RSFormat
   Author: Andrew Ushakov
*********************************************************************/

#ifndef __RSFMTINTFC_H
#define __RSFMTINTFC_H

#include "fieldtp.h"
#include "platform.h"

#include "packpshn.h"

//////////////////////////////////////////////////////
// Используется для передачи данных
//////////////////////////////////////////////////////
typedef struct _fmt_value
      {
       // Тип передаваемого значения
       int   typeValue;

       // Указатель на данные
       void *pValueBuff;

       // Размер буффера (его необходимо использовать только при typeValue==STRING_T,
       // (для указания размера строки с учетом последнего символа), 
       // для остальных типов размер блока памяти, выделенного в pValueBuff должен 
       // определяться и соответствовать typeValue.
       unsigned short iBuffSize;
      } fmt_value;

#define  SET_FMT_VALUE(val, type, pBuff, size)  {val.typeValue = (type); val.pValueBuff = (pBuff); val.iBuffSize = (size); }

#if defined(RSL_MD_SUN)
    #define  RSFMTAPI
#elif defined(RSL_PL_WIN32)
    #define  RSFMTAPI  __stdcall
#endif

// Значения, используемые для Move и Select
#define  FT_HOME              0
#define  FT_LEFT              1
#define  FT_RIGHT             2
#define  FT_END               3
#define  FT_LEFTWORD          6
#define  FT_RIGHTWORD         7

#define  FT_MODE_INSERT       1
#define  FT_MODE_OVERWRITE    0

#define  FT_FMT_C             1
#define  FT_FMT_RSL           2

#define  FT_FMT_OEM           1
#define  FT_FMT_ANSI          2

#define  FT_ALIGN_RIGHT       0
#define  FT_ALIGN_LEFT        1
#define  FT_ALIGN_CENTER      2

#define  FT_GETVAL          255

typedef struct tagFormat
      {
       // создает формат, должна быть вызвана после конструктора
       bool (RSFMTAPI *Create)(struct tagFormat *, const char *);
       bool (RSFMTAPI *CreateEx)(struct tagFormat *, const char *);

       // установить значение
       bool (RSFMTAPI *ImportStr)(struct tagFormat *, const char *);
       bool (RSFMTAPI *ImportVal)(struct tagFormat *, const fmt_value *);

       // получить значение
       bool (RSFMTAPI *Export)(struct tagFormat *, fmt_value *);

       // получить результирующую строку
       const char* (RSFMTAPI *Result)(struct tagFormat *);

       // Режим редактирования insert(1) / overwrite(0)
       // Возвращает предыдущее значение
       unsigned char (RSFMTAPI *ModeSet)(struct tagFormat *, unsigned char);
       unsigned char (RSFMTAPI *ModeGet)(struct tagFormat *);

       // Ставим лимит
       bool (RSFMTAPI *SetLimit)(struct tagFormat *, unsigned long);

       // Получаем лимит
       unsigned long (RSFMTAPI *GetLimit)(struct tagFormat *);

         // Удалить
       bool (RSFMTAPI *Del)(struct tagFormat *);
       bool (RSFMTAPI *BSpace)(struct tagFormat *);

       // Вставить в текущую позицию
       bool (RSFMTAPI *Insert)(struct tagFormat *, char);

       // Передвинуть курсор
       bool (RSFMTAPI *Move)(struct tagFormat *, int);
       bool (RSFMTAPI *MoveTo)(struct tagFormat *, unsigned long);

       // true, если вообще что-нибудь выделено, иначе - false
       bool (RSFMTAPI *IsSelected)(struct tagFormat *);

       // Левая / правая позиция выделенного
       unsigned long (RSFMTAPI *LeftSelected)(struct tagFormat *);
       unsigned long (RSFMTAPI *RightSelected)(struct tagFormat *);

       // выделить символы, обе функции ориентируются на текущую
       // позицию, затем сдвигая ее
       bool (RSFMTAPI *Select)(struct tagFormat *, int);
       bool (RSFMTAPI *SelectTo)(struct tagFormat *, long);

       // сбросить селекцию
       void (RSFMTAPI *Unselect)(struct tagFormat *);

       // Текущая позиция
       unsigned long (RSFMTAPI *Current)(struct tagFormat *);

       // Ставим символ для незаполненных символов в шаблоне
       void (RSFMTAPI *SetSpaceSymbol)(struct tagFormat *, char);

       // Введен специально для использования в Tools
       void (RSFMTAPI *CursorAtLastChar)(struct tagFormat *, int);

       // Ставим курсор в позицию по умолчанию
       bool (RSFMTAPI *SetDefCursor)(struct tagFormat *);

       // Сбрасываем содержимое строки
       void (RSFMTAPI *ResetString)(struct tagFormat *);

       // Для ряда форматов(дат), проводим некое преобразование уже
       // после ввода информации
       bool (RSFMTAPI *Activate)(struct tagFormat *);

       // Получаем тип форматной библиотеки (1 - С++, 2 - RSL)
       unsigned char (RSFMTAPI *GetFmtVersion)(struct tagFormat *);

       // Получаем набор специальных символов-разделителей
       void (RSFMTAPI *GetDelimSym)(struct tagFormat *, char *, unsigned char);

       // Выставляем кодировку OEM или ANSI (1 - OEM, 2- ANSI)
       unsigned char (RSFMTAPI *SetCode)(struct tagFormat *, unsigned char);

       // Получаем номер версии интерфейса
       unsigned long (RSFMTAPI *GetVersion)(struct tagFormat *);

       // Получаем тип выравнивания
       unsigned char (RSFMTAPI *SetAlign)(struct tagFormat *, unsigned char);

       // Ставим значения для типа переменных с которыми работает формат и как формат был создан
       // (1 - тип переменной, 2 - был ли создан из-за ассоциации с типом). Отрицательные значения игнорируются
       void (RSFMTAPI *SetVarType)(struct tagFormat *, int, int);

       // Получаем значения, устаноыленные в SetVarType
       void (RSFMTAPI *GetVarType)(struct tagFormat *, int *, int *);

       // Ставим для поля выделение по умолчанию
       void (RSFMTAPI *SetDefSelection)(struct tagFormat *);

       // Был ли отредактирован после импорта значения
       bool (RSFMTAPI *IsDirty)(struct tagFormat *);

       // Получить строку без разделителей
       void (RSFMTAPI *RawResult)(struct tagFormat *, char *, int);

       // Выделена ли вся строка
       bool (RSFMTAPI *IsAllSelected)(struct tagFormat *);

       // Установить флажек редактирования
       void (RSFMTAPI *SetDirty)(struct tagFormat *, bool);

       // Получаем тип выравнивания
       unsigned char (RSFMTAPI *SetAlignEx)(struct tagFormat *, unsigned char, int *);

       // 0 - не использовать шаблон для редактирования, 1 - использовать
       unsigned char (RSFMTAPI *SetEditMode)(struct tagFormat *, unsigned char mode);
      } FORMAT, *PFORMAT;

#include "packpop.h"

#endif // __RSFMTINTFC_H

/* <-- EOF --> */