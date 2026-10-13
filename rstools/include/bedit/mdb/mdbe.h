/*
**  Эмулятор Btrieve для менеджера записей в оперативной памяти (МЗП
**  
**  Предоставляет средства для эмуляции функций bfOp() и bfGP()
**  
*/

#ifndef __MDBE_H
#define __MDBE_H

#include "mdb/mdb.h"
#include "nwbtrv.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Константы
enum
   {
    MDBEE_CANTOPEN = -201    // Ошибка "открытия" при помощи bfOpenMdb()
   };

// -----------------------------------------------------------------------------
// Эти операции поддерживаюся МЗП
#define  mdbeOpIsSupported(btrvOp)                                    \
                          ((btrvOp) == BgetF  || (btrvOp) == BgetL    \
                        || (btrvOp) == BgetN  || (btrvOp) == BgetP    \
                        || (btrvOp) == BgetGT || (btrvOp) == BgetGE   \
                        || (btrvOp) == BgetLT || (btrvOp) == BgetLE   \
                        || (btrvOp) == BgetE                          \
                        || (btrvOp) == BgkF    || (btrvOp) == BgkL    \
                        || (btrvOp) == BgkN    || (btrvOp) == BgkP    \
                        || (btrvOp) == BgkGT   || (btrvOp) == BgkGE   \
                        || (btrvOp) == BgkLT   || (btrvOp) == BgkLE   \
                        || (btrvOp) == BgkE                           \
                        || (btrvOp) == Bgetpos || (btrvOp) == BgetD   \
                        || (btrvOp) == Binsert || (btrvOp) == Bupdate \
                        || (btrvOp) == Bdelete)

// -----------------------------------------------------------------------------
// Функция для поддержки bfOp для МЗП
int mdbeOp(int btrvOp, MDB_CURSOR *c, void *keyBuf, unsigned keyNum, int *recSize);

// Функция для поддержки bfGP() для МЗП
int mdbeGetPos(MDB_CURSOR *c, recpos_t *pos);

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */