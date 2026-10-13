#ifndef __RSCOMERR_H
#define __RSCOMERR_H

#include "pexpdef.h"

#ifdef __cplusplus
extern "C" {
#endif

_RSPNLEXP void HandleRsComError  (TAppObj *app, TRsStat stat);
_RSPNLEXP int  HandleRsComErrorEx(TAppObj *app, TRsStat stat, const char *caption, unsigned long flags, bool isAnsi);

#ifdef __cplusplus
}
#endif

#endif // __RSCOMERR_H

/* <-- EOF --> */