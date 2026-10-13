#ifndef __GETS_H
#define __GETS_H

#include "pexpdef.h"
#include "tooldef.h"

#ifdef __cplusplus
extern "C" {
#endif

_RSPNLEXP char *getline(char *str, int maxlen, FILE *fp);
_RSPNLEXP int   putline(char *str, FILE *fp );

_RSPNLEXP int   tfputc(int c, FILE *fp);

_RSPNLEXP int   PrnPutLine(char *str, FILE *fp);
_RSPNLEXP int   PrnFputc(int c, FILE *fp);

_RSPNLEXP FILE *PrnOpen(char *name);
_RSPNLEXP int   PrnClose(FILE *fp);

_TOOLEXP void   SetDefPrinterName(void);

#ifdef __cplusplus
} ;
#endif

#endif // __GETS_H

/* <-- EOF --> */