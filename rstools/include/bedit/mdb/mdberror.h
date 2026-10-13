/*
**  Менеджер записей в оперативной памяти (МЗП)
**  Извлечение текста ошибки по ее коду
**  
*/

#ifndef __MDBERROR_H
#define __MDBERROR_H

#include "bexpdef.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Получить текст ошибки по ее коду
_BTEXP char RSL_FAR *mdbErrorText(int code);

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */