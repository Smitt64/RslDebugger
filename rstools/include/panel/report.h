//***********************************************************************
//             Полноэкранный текстовый интерфейс для Borland C.
//          Поддержка отчетов.
//           VECsoft 31.08.92
//
// VERSION INFO :   $Archive: /RS-Tools/Old_tools/panel/h/report.h $
//                  $Revision: 9 $
//                  $Date: 18.02.00 13:35 $
//                  $Author: Lebedev $
//                  $Modtime: 17.02.00 15:14 $
//
//***********************************************************************

#ifndef __REPORT_H
#define __REPORT_H

#include <stdio.h>

#include "menu.h"
#include "rstypes.h"

typedef unsigned char  r_coord_rpt;

_RSPNLEXP extern FILE *prnfile;           /* Файл для отчета       */

#define  setpfile(prf) prnfile = prf; /* Установить файл для отчета   */

#define  HEADON   1   /* состояние строки : печатать заголовок  */
#define  HEADOFF  2   /* состояние строки : не печатать заголовок  */

#include <packpshn.h>

/* Описатель поля отчета        */
typedef struct
      {
       unsigned char  RVt;        /* Тип значения              */
       void          *RValue;     /* Значение в поле            */
       unsigned int   RVp;        /* Знаков после точки,признак wrap для строк   */
       coord          x, y, l, h; /* Координаты поля           */
       char         **wlist;      /* Список для wrap           */
      } RFIELD;

/* Строка отчета        */
typedef struct
      {
       RFIELD         *RFList;  /* Массив полей строки отчета    */
       unsigned char   RFnum;   /* Количество полей строки отчета   */
       unsigned char   Lspace;  /* Признак пропуска строки    */
       unsigned char   Lcentr;  /* Признак центрирования      */
       coord           h;       /* Количество линий в строке     */
       int             stat;    /* состояние выводимой строки    */
       int             ext;     /* Номер дополнительной линии строки   */
      } RLINE;

#define  NR(r)  sizeof(r)/sizeof(RFIELD)

/* Описатель отчета        */
typedef struct
      {
       int      wide;     /* Ширина отчета        */
       int      pagesize; /* Длина страницы       */
       RLINE   *titul;    /* Заголовок            */
       int      Page;     /* Номер страницы       */
       int      Line;     /* Номер линии на странице    */
       char    *buff;     /* Буфер          */
       char    *wsave;    /* Область сохранения статуса    */
       FILE    *fr;       // Файл отчета
       int      nl;       // Число строк отчета.
       RLINE  **rl;       // Строки отчета.
      } REPRT;

#include <packpop.h>

#ifdef __cplusplus
extern "C" {
#endif

// Вывести буфер в файл отчета.
_RSPNLEXP int PfB(
                  FILE *f,                      // Файл.
                  char *buff                    // Буфер.
                 );

/* Получить буфер для печати     */
/* возвращает указатель на буфер        */
_RSPNLEXP char *makeB(
                      int lenb                       /* Длина строки            */
                     );

/* Вывод строки с усечением в буфер    */
_RSPNLEXP int toB(
                  char *buff,     /* Буфер               */
                  int   offs,     /* Смещение в буфере            */
                  char *stro,     /* Строка для вывода          */
                  int   outlen    /* Длина поля для вывода         */
                 );

/* Вывести значение поля в буфер     */
_RSPNLEXP void FtoB(
                    char *buff,       /* Буфер               */
                    int   offs,       /* Смещение в буфере            */
                    void *val,        /* Значение             */
                    int   lf,         /* Ширина поля                                */
                    unsigned int vp,  /* Знаков после точки           */
                    unsigned int tp   /* Тип значения              */
                   );

/* Центрировать буфер            */
_RSPNLEXP void centrB(
                      char *b     /* Буфер          */
                     );

/* Начать новую страницу         */
_RSPNLEXP int newpage(
                      REPRT *r    /* Описатель отчета          */
                     );

/* Вывести строку отчета         */
_RSPNLEXP int Prline(
                     REPRT *r,    /* Описатель отчета          */
                     RLINE *rl    /* Описатель строки           */
                    );

/* Установить статус вывода строки     */
_RSPNLEXP void SetPrnStat(
                          RLINE *rl,      /* Описатель строки           */
                          int   stat      /* Статус строки           */
                         );

/* Определить статус вывода строки     */
_RSPNLEXP int IsPrnStat(
                        RLINE *rl    /* Описатель строки           */
                       );

/* Вывести строки отчета         */
_RSPNLEXP int Prlines(
                      REPRT  *r,              /* Описатель отчета          */
                      RLINE **ln,     /* Список описателей строк       */
                      int     Numln     /* Количество описателей строк         */
                     );

/* Вывести строку отчета потоком    */
_RSPNLEXP int PrlineFlow(
                         REPRT *r,       /* Описатель отчета          */
                         RLINE *ln       /* Список описателей строк       */
                        );

/* Начать отчет            */
_RSPNLEXP int brep(REPRT *r,
                   FILE  *f,
                   int    wd,         /* Ширина отчета        */
                   int    ps,         /* Длина страницы       */
                   RLINE *tt,         /* Заголовок            */
                   int    np       /* Индекс поля с номером страницы   */
                  );

/* Начать отчет            */
_RSPNLEXP REPRT *begrep(
                        int    wd,      /* Ширина отчета        */
                        int    ps,      /* Длина страницы       */
                        RLINE *tt,      /* Заголовок            */
                        int    np       /* Индекс поля с номером страницы   */
                       );

/* Кончить отчет        */
_RSPNLEXP void endrep(
                      REPRT *r        /* Описатель отчета        */
                     );

#include <packpsh1.h>

typedef struct
      {
       uchar        RFnum, Lspace, Lcentr;
       r_coord_rpt  h;
       db_int16     len;
      } RlineR;

typedef struct
      {
       uchar        RVt;
       db_uint16    RVp;
       r_coord_rpt  x, y, l, h, vfl, lens;
      } RfieldR;

typedef struct
      {
       db_int16  wd, ps, np, nline;
      } ReprtR;

#include <packpop.h>

// Конструктор RLINE.
_RSPNLEXP RLINE * CrRline(
                          uchar RFnum,   // Количество полей строки отчета.
                          uchar Lspace,  // Признак пропуска строки.
                          uchar Lcentr,  // Признак центрирования.
                          coord h        // Количество линий в строке.
                         );

// Конструктор RFIELD.
_RSPNLEXP void InsRfield(
                         RLINE *rl,
                         uchar  RVt,     // Тип значения.
                         unsigned int RVp,     // Знаков после точки,признак wrap для строк.
                         coord  x,       // Относительные координаты поля.
                         coord  y,
                         coord  l,
                         coord  h
                        );

// Полный конструктор RFIELD.
_RSPNLEXP void FInsRfield(
                          RLINE *rl,
                          uchar  RVt,        // Тип значения.
                          void  *RValue,     // Значение в поле.
                          unsigned int  RVp, // Знаков после точки,признак wrap для строк.
                          coord  x,          // Относительные координаты поля.
                          coord  y,
                          coord  l,
                          coord  h
                         );

// Считывание RLINE.
_RSPNLEXP RLINE *RdRline(
                         ResFile *rt,    // Открытый ресурс.
                         char    *member // Имя раздела.
                        );

#define  DelRline(rl)  free(rl)  // Деструктор RLINE.

// Запись адресов переменных в поля отчета.
_RSPNLEXP void SetRFlds(RLINE *rl, ...);

// Полный конструктор RFIELD.
_RSPNLEXP void FInsRfield(
                          RLINE *rl,
                          uchar  RVt,        // Тип значения.
                          void  *RValue,     // Значение в поле.
                          unsigned int  RVp, // Знаков после точки,признак wrap для строк.
                          coord  x,          // Относительные координаты поля.
                          coord  y,
                          coord  l,
                          coord  h
                         );

// Конструктор REPRT.
_RSPNLEXP REPRT *CrRep(
                       ResFile    *rt,       // Открытый ресурс.
                       const char *member,   // Имя раздела.
                       FILE       *fr        // Файл отчета.
                      );

_RSPNLEXP void DelRep(REPRT *r);

// Конструктор REPRT.
_RSPNLEXP REPRT *CrRepF(
                        ResFile   *rt,       // Открытый ресурс.
                        const char *member,  // Имя раздела.
                        const char *fn       // Файл отчета.
                       );

_RSPNLEXP void DelRepF(REPRT *r);

#ifdef __cplusplus
}
#endif

#endif // __REPORT_H

/* <-- EOF --> */