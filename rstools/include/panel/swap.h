/***************************************************************************

   Система свопинга программ      27.02.1992
    swap.asm  - основной модуль, содержащий:
      int   far  run_swap(char far *prog, char far *comstr, int swap_size);
            - запускает дочернюю программу  
      void  far runstat(char far *swap);
            - возвращает код ошибки  
      void  far setswap(char far *swap);
            - устанавливает имя файла свопинга

***************************************************************************/

#ifndef __SWAP_H
#define __SWAP_H

#include "pexpdef.h"


#define  DSBLXMS   1
#define  ENBLXMS   0

#ifdef __cplusplus
extern "C" {
#endif

/*   Запускает дочернюю программу, возвращает код завершения или -1    */
_RSPNLEXP int Run_prog(
                       char *program,   /*  Запускаемая программа                       */
                       char *com,       /*  Командная строка                            */
                       char *initmes,   /*  Стартовое сообщение  или  NULL              */
                       char *pausemes,  /*  Заключительное сообщение и пауза или NULL   */
                       int   swap_size  /*  Размер в KB необходимый для программы       */
                      );

_RSPNLEXP int Run_progEx(
                         char *program,   /*  Запускаемая программа                       */
                         char *com,       /*  Командная строка                            */
                         char *initmes,   /*  Стартовое сообщение  или  NULL              */
                         char *pausemes,  /*  Заключительное сообщение и пауза или NULL   */
                         int   swap_size, /*  Размер в KB необходимый для программы       */
                         int   catureHd
                        );

_RSPNLEXP int  Run_progEx2(
                           char *program,   /*  Запускаемая программа                       */
                           char *com,       /*  Командная строка                            */
                           char *initmes,   /*  Стартовое сообщение  или  NULL              */
                           char *pausemes,  /*  Заключительное сообщение и пауза или NULL   */
                           int   swap_size, /*  Размер в KB необходимый для программы       */
                           int   catureHd,
                           FILE *capFile
                          );

#ifndef RSL_MD_FLAT
    int  far run_swap(char far *prog, char far *comstr, int swap_size);
    /*       Если swap_size == 640 , то максимальный свопинг    */
    void far setswap(char far *swap);
    int  far runstat(void);

    int   XMSswap(int dsbl);
#endif // __FLAT__

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */