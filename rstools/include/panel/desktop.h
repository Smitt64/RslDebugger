/*******************************************************************************
  RS-Bank 4.31,5.0                                       R-Style Software Lab

  Полноэкранный текстовый интерфейс для Borland C. DESCTOP

FILENAME: DESKTOP.H
CREATED: 02.11.92
VERSION INFO :  $Archive: /RS-Tools/Old_tools/panel/h/desktop.h $
                $Revision: 16 $
                $Date: 18.02.00 13:35 $
                $Author: Lebedev $
                $Modtime: 17.02.00 15:10 $

MODIFICATIONS:
  26.12.96 Kubrin Sergey
    Ликвидировал включение windows.h
    Добавлена функция JDELAY (jdelay.c)
  02.03.99 Мол.
    В описатель меню (MENU) добавлен для хранения значения параметра
    пункта меню
    Добавлены функции GetSelectedMenuItem() и SetSelectedMenuItem()
*******************************************************************************/

#ifndef DESKTOP
#define DESKTOP
#define __DESKTOP_H

#include <stdlib.h>

#include "menu.h"
#include "rsmain.h"
#include "jdelay.h"

#include "traceinit.h"


// -----------------------------------------------------------------------------
extern  HTRACE  hPanelTrace;


#include <packpshn.h>

// -----------------------------------------------------------------------------
#define  NM(m)  (sizeof(m)/sizeof(MENU))

// Для конструирования popup-меню
#define  POPUP_TAG  UCHAR_MAX

// Сконструировать id-пункта меню
#define  MAKEMENUITEM(lo, hi)  ((lo & 0xFFFF) | ((hi & 0xFFFF) << 16))

// Сконструировать позицию ползунка
#define  MAKETRACKPOS(lo, hi)  ((lo & 0xFFFF) | ((hi & 0xFFFF) << 16))

// указатель на программу для вызова из меню
typedef int (*FPTR)(PANEL *p, MENU *v);

#include <packpop.h>

#ifdef __cplusplus
extern "C" {
#endif

_RSPNLEXP extern ResFile AppRes;

_RSPNLEXP PANEL *CreateVM(MENU *v, int n, int x, int y, int fl);

_RSPNLEXP PANEL *CreateVMEx(MENU *v, int n, int x, int y, int fl, MenuVar *VarMenu);

// Создать панель горизонтального меню.
// Возвращает указатель на распределенную панель.
_RSPNLEXP PANEL *CreateHM(MENU  *horm,           // Список горизонтального меню.
                          int    n,              // Его размер.
                          int    y               // Номер строки для меню.
                         );

// Создать панель горизонтального меню.
// Возвращает указатель на распределенную панель.
_RSPNLEXP PANEL *CreateHMEx(MENU     *horm,           // Список горизонтального меню.
                            int       n,              // Его размер.
                            int       y,              // Номер строки для меню.
                            MenuVar  *VarMenu         // Указатель на структуру, содержащую меню "Разное".
                           );

// Вывести и обрабатывать меню.
// Возвращает код команды меню.
_RSPNLEXP int LoopM(PANEL *p,              // Панель меню.
                    uchar  clf             // YES разрешает закрываться по ESC.
                   );

// Опрос горизонтального меню.
// Возвращает код команды меню или 0.
_RSPNLEXP int TestHM(PANEL *p,             // Панель меню.
                     uchar  ffl
                    );

// Возвратить ссылку на выбранный пункт меню (после работы функции TestHM
_RSPNLEXP MENU *GetSelectedMenuItem(void);

// Установить новую ссылку на текущий пункт меню (может быть 0)
// Возвращает предыдущее значение
_RSPNLEXP MENU *SetSelectedMenuItem(MENU *);

// Возвратить идентификатор текущего пункта меню (вне UseMenuPanel невалидна!)
int GetCurrentMenuItemId(void);

// Установить идентификатор текущего пункта меню (вне UseMenuPanel невалидна!)
void SetCurrentMenuItemId(int id);

// Опрос горизонтального меню. Возвращает код команды меню или event.
_RSPNLEXP int TestMenu(PANEL *p,              // Панель меню.
                       int    event
                      );

// Создать панель горизонтального меню.
// Возвращает указатель на распределенную панель.
_RSPNLEXP PANEL *CrHM(MENU  *horm,           // Список горизонтального меню.
                      int    n,              // Его размер.
                      int    y               // Номер строки для меню.
                     );

// Создать панель горизонтального меню.
// Возвращает указатель на распределенную панель.
_RSPNLEXP PANEL *CrHMEx(MENU     *horm,            // Список горизонтального меню.
                        int       n,               // Его размер.
                        int       y,               // Номер строки для меню.
                        MenuVar  *VarMenu
                       );

// -----------------------------------------------------------------------------
// Проверить, не находимся ли мы в меню
_RSPNLEXP bool InMenuProgress(void);

// Выбросить строку статуса.
_RSPNLEXP void PutStatLine(void);

// Обновить статус.
_RSPNLEXP void UseStatLine(const char *s);

// Принудительно обновить статус.
_RSPNLEXP void UseStatLine2(const char *s);

_RSPNLEXP int  SizeStatLine(void);
_RSPNLEXP void SaveStatLine(void *psave);
_RSPNLEXP void RestStatLine(void *psave);

_RSPNLEXP void appdesk(const char *Systema, const char *Desk, const char *Copr, const char *Resurs, const char *Helpfile);

_RSPNLEXP void SetNoDesk(int value);


int RSNotifyInit(void);
void RSNotifyDone(void);

// Обработчик команд статусной строки
int HandlerStatusCmd(int secID, int cmd, int val);

// Установить пользовательский список поддерживаемых ключей
// Длина сроки s не может превышать 1000 символов (б\у ноль-символа)
_RSPNLEXP void SetAppCmdKey(const char *s);

// -----------------
enum
   {
    eAS_NORMAL = 0,      // Нормальный размер
    eAS_MAX,             // Распахнуть окно на весь экран
    eAS_MIN,             // Минимизировать окно
    eAS_RESTORE,         // Восстановить предыдущее состояние окна
    eAS_ACTIVATE = 100,  // Активация окна (миганием кнопки в таскбаре)
    eAS_ACTIVATE2        // Активация окна (по возможности настоящая)
   };

// Максимизировать/минимизировать окно приложения.
// Работает только в EasyWin-режиме.
// [in]  cmdSize - команда операции (см. eAS_)
// [ret]         - не используется
_RSPNLEXP int SetAppSize(int cmdSize);

void PanelKbdIndic(int flag);

_RSPNLEXP extern int NoteBook;  // Программа запущена на note book
_RSPNLEXP extern int MonoMode;
_RSPNLEXP extern int RslDefVideoMode;

_RSPNLEXP void SetAppTitle(const char *title);
_RSPNLEXP int  GetAppTitle(char *buf, int size);

_RSPNLEXP int  PanelInit(void);
_RSPNLEXP void PanelDone(void);

_RSPNLEXP int  SetAutoSend(int tick);
_RSPNLEXP void SendToTerminal(void);
_RSPNLEXP void ForceSendToTerminal(void);


// Запуск внешних программа в 2-х и 3-х звенке.
//
//    StartProgram - запускает на выполнение внешнюю программу.
//       Возвращает идентификатор процесса или его хэндл, а если
//       произошла ошибка, то -1.
//       Если запуск без ожидания невозможен (DOS, Win16, Dpmi32),
//       вызывает Run_prog и, если нет ошибок, возвращает 0.
//       Если fDetached равен false, возвращает хэндл процесса,
//       который должен быть закрыт с помощью StopProgram или WaitProgram.
//       Если fDetached равен truе, возвращает идентификатор процесса.
//
//    StopProgram - прерывает выполнение запущенной программы. Параметр
//       exitCode задает код завершения процесса (для Win32) и способ
//       завершения. Если он равен -1, процесс будет завершен аварийно.
//       Возвращает 0 или -1 в случае ошибки.
//
//    WaitProgram - ожидает завершения запущенной программы в течении
//       заданного времени интервала времени (в миллисекундах). Если
//       параметр timeOut равен -1, ожидание выполняется неограниченно
//       долго. Возвращает 0 или -1 в случае ошибки.
//
//    ExecuteProgram - запускает внешнюю программу и ждет ее завершения
//       Возвращает:
//          0, если программа выполнена без ощибок
//         -1, если произошла ошибка
//         -2, если выполнение программы было прервано пользователем
//

#define  RSL_STILL_ACTIVE  0x0103

_RSPNLEXP intptr_t StartProgram(
                                const char *ProgName,   // имя программы
                                const char *CmdArgs,    // строка со списком аргументов
                                bool        fRemote,    // в 3х звенке заускать на терминале
                                bool        fDetached   // закрыть хэндл на запущенный процесс
                               );

intptr_t StartProgramEx(
                        const char *ProgName,   // имя программы
                        const char *CmdArgs,    // строка со списком аргументов
                        bool        fRemote,    // в 3х звенке запускать на терминале
                        bool        fDetached,  // закрыть хэндл на запущенный процесс

                        intptr_t    inHandle,   // стандартный ввод
                        intptr_t    outHandle,  // стандартный
                                                // если заданы, процесс наследует хендлы

                        int         console     // 0 - detached, 1 - current console, 2 - new console
                       );

_RSPNLEXP int StopProgram(
                          intptr_t    hProcess,   // хэндл процесса, полученный от StartProgram
                          int         exitCode,   // код завершения процесса
                          bool        fRemote     // в 3х звенке заускать на терминале
                         );

_RSPNLEXP int WaitProgram(
                          intptr_t    hProcess,   // хэндл процесса, полученный от StartProgram
                          int         timeOut,    // время ожидания в милисекундах
                          bool        fRemote     // в 3х звенке заускать на терминале
                         );

_RSPNLEXP int ExecuteProgram(
                             const char *ProgName,   // имя программы
                             const char *CmdArgs,    // строка со списком аргументов
                             bool        fRemote     // в 3х звенке заускать на терминале
                            );

bool ActivateProcessWindow (uint32 ptocId, const char *header, bool fRemote);
void OpenDesignerResource (uint32 pId, const char *head, const char *resId, bool remote);


// Вывести на экран панель с сообщением
_RSPNLEXP PANEL * PutMessage(const char *MsgStr, const char *Status);

// Убрать панель с сообщением с экрана
_RSPNLEXP void RemMessage(PANEL *pnl);

bool IsKbdFiltered(void);
void SetKbdFiltered();

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */