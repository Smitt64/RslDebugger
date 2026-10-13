/*           Обработка ошибок для Turbo C++ 1.01         */
/*        VECsoft 23.04.91          */

#include "pexpdef.h"

enum DOSERAS
   {
    DER_IGNORE,  /* Вернуть управление программе        */
    DER_RETRY,   /* Повторить операцию            */
    DER_ABORT    /* Вызвать обработчик Ctrl Break                */
   };

extern int _RSPNLEXP  HandDevErr;  /* Признак игнорированной ошибки устройства */
extern int _RSPNLEXP  HandErrLev;  /* Признак допустимости ABORT         */

#define  HardHand(Level)  { harderr(handler); HandErrLev = Level; }

#ifdef __cplusplus
extern "C" {
#endif

/* Обработчик критических ошибок    */
int handler(
            int err,   /* Номер ошибки         */
            int ax,    /* Регистры программы   */
            int bp,
            int si
           );

int crewin(char *msg);

#ifdef __cplusplus
}
#endif

/* <-- EOF --> */