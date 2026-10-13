/*******************************************************************************
 FILE         :   RSMAIN.H

 COPYRIGHT    :   R-Style Software Lab, 1999

 DESCRIPTION  :   RsMain description

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   6 May 1999

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/PANEL/H/rsmain.h $
                  $Revision: 3 $
                  $Date: 6.05.99 18:57 $
                  $Author: Markovitch $
                  $Modtime: 6.05.99 18:57 $
*******************************************************************************/

#ifndef __RSMAIN_H
#define __RSMAIN_H

#include <stdlib.h>

#include "fs.h"
#include "rsatexit.h"
#include "pexpdef.h"


// -----------------------------------------------------------------------------
#include <packpshn.h>

typedef struct CheckArgsParm
      {
       int     argc;    // аргументы командной строки
       char  **argv;
       int     remote;  // 1 - трехзвенка, 0 - двухзвенка
       int     isGUI;   // 1 - графическое приложение (нельзя использовать printf - у него
                        //     нет консоли - только MessageBox), 0 - консольное
      } CheckArgsParm;

#include <packpop.h>

// -----------------------------------------------------------------------------
#ifdef __cplusplus

    int RsFmMain(int argc, char **argv);  // This function is only used in C++!

extern "C" {
#endif


_RSPNLEXP extern int   isDebug;
_RSPNLEXP extern int   isRemote;
_RSPNLEXP extern int   isConsole;
_RSPNLEXP extern int   NormalTerminateFlag;
_RSPNLEXP extern bool  isSql;
          
_RSPNLEXP extern bool  isRceMode;

typedef int (CheckArgs_t)(const CheckArgsParm *);

_RSPNLEXP void DisableEasyWin(void);
_RSPNLEXP void SetCheckArgs(CheckArgs_t *);

// Функция вызывается до fs_startApp. В прикладном коде эту функцию можно переопределить.
// Если функция возвращает 0, программа завершается.
// Код возврата: 1 - разрешить работу программы, 0 - запретить.
int CheckArgs(const CheckArgsParm *);


// -----------------------------------------------------------------------------
#define  RSL_NO_EASYWIN    int CheckArgs(const CheckArgsParm *parm) { DisableEasyWin(); return 1; }
#define  RSFM_APPLICATION  int CheckArgs(const CheckArgsParm *parm) { DisableEasyWin(); SetRsFmMode (1); return 1; }


_RSPNLEXP int RslStartApp(int argc, char **argv, fsapp_mainproc_t rsMain, int noFS);
_RSPNLEXP int RslWinMain(char *szCmdLine, fsapp_mainproc_t rsMain, int noFS);

// Завершить работу программы.
// Параметр cexit позволяет задать режим завершения cexit:
//    1 - вызвать все функции зарегистрированые через RslAddExitProc, завершить работу программы функцией exit();
//    0 - завершить работу программы функцией exit();
//   -1 - аварийно покинуть программу вызвав abort().    
// exitCode - код возврата приложения
_RSPNLEXP void RslStopApp(int cexit, int exitCode);


// -----------------------------------------------------------------------------
// Установить код возврата из приложения
// Возвращаемое значение не используется
_RSPNLEXP int RslSetExitCode(int code);

// Разбор аргументов командной строке. Учитывая аргументы, заключенные в
// двойные кавычки - "large argument". Для таких аргументов дополнительо
// поддерживаются esc-символы: \xHH \oOOO \DDD и стандартные \a\b\f\n\r\t.

_RSPNLEXP size_t cmds2argl(char *cmds, char **argv, size_t size);


// -----------------------------------------------------------------------------
int RsMain(int argc, char **argv);

_RSPNLEXP void SetRsFmMode (int mod);
_RSPNLEXP int  IsRsFmMode  (void);
_RSPNLEXP void DoneRsForms (void);
          bool UseLocalRsFm();


_RSPNLEXP int RsFmAppInit  (int argc, char **argv);
_RSPNLEXP int RsFmAppInitEx(int argc, char **argv, const char *appName);

void SyncWithTerminal(const char *appName);


#ifdef __cplusplus
} ;
#endif


// -----------------------------------------------------------------------------
#define  RSMAIN(argc, argv)  int RsMain(int argc, char **argv)

#endif // __RSMAIN_H

/* <-- EOF --> */