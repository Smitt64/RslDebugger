/********************************************************************
   Created: 2002/03/27
   Created: 27:3:2002   18:11
   Filename: D:\Work\RSTools\panel\h\formattempl.h
   Project: RS-Tools
   Author: Andrew Ushakov
*********************************************************************/

#ifndef __FORMATTEMPL_H
#define __FORMATTEMPL_H

#include "pexpdef.h"
#include "RSFmtIntfC.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// uchType - format type (1 - C++, 2 - RSL(not implemented yet))
_RSPNLEXP PFORMAT RSCreateFormat(unsigned char uchType);
_RSPNLEXP void RSReleaseFormat(PFORMAT);
_RSPNLEXP PFORMAT ParseFormatString(const char *);

// Enable/Disable formats
_RSPNLEXP bool EnableFormats(bool);


void RsFormatUseBtrDecimal(bool);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __FORMATTEMPL_H

/* <-- EOF --> */