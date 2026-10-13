/**********************************************************************
*        File Name:   ierrfpu.h          March 1,1994            
*       Programmer:   S. Kubrin              
*      Description:   Обработка фатальных ошибок
*  Public routines:  
* Private routines:  
***********************************************************************/

#ifndef __IERRFPU_H
#define __IERRFPU_H

#include "pexpdef.h"

#ifdef __cplusplus
extern "C" {
#endif

// Указатель на функцию вывода сообщений об ошибках
// из обработчика FPU-ошибок
typedef void (*FpeShowMes_f)(const char *);

extern _RSPNLEXP FpeShowMes_f  FpeShowMes;

// Устанавливает обработчики ошибок FPU
void FpeInstall(FpeShowMes_f ShowMes);

#ifdef __cplusplus
}
#endif

#endif  // __IERRFPU_H

/* <-- EOF --> */