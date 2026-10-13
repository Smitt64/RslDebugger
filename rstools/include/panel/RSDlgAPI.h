/********************************************************************
   Created: 2002/12/24
   Created: 24:12:2002   12:04
   Filename: D:\Work\RSDialogsApi\RSDlgAPI.h
   Project: API for RS-Dialogs
   Author: Andrew Ushakov
*********************************************************************/

#ifndef __RSDLGAPI_H
#define __RSDLGAPI_H

#include "pexpdef.h"
#include "RSDgCnst.h"

#ifdef __cplusplus
extern "C"
{
#endif

///////////////////////////////////////////////////////
// Message Box
///////////////////////////////////////////////////////
_RSPNLEXP int RsMsgBox(void *pParent, // IFrameWindow
                       const char *lpszText, const char *lpszCaption, unsigned long ulStyle, bool bUseAnsi);

_RSPNLEXP int RsMsgBoxMinMaxSize(void *pParent, const char *lpszText, const char *lpszCaption,
                                 unsigned long ulStyle, bool bUseAnsi, int iMinWidth, int iMinHeight,
                                 int iMaxWidth, int iMaxHeight);

_RSPNLEXP int RsErrorBox(void *pParent, char **lplpszText, int iStrCount, const char *lpszCaption,
                         unsigned long ulStyle, bool bUseAnsi);

_RSPNLEXP int RsErrorBoxMinMaxSize(void *pParent, char **lplpszText, int iStrCount, const char *lpszCaption,
                                   unsigned long ulStyle, bool bUseAnsi, int iMinWidth, int iMinHeight,
                                   int iMaxWidth, int iMaxHeight);

///////////////////////////////////////////////////////
// Indicators
///////////////////////////////////////////////////////
typedef int (*pfTestBrk)();

_RSPNLEXP bool RsInitIndicator(void *pParent, pfTestBrk pBrkFunc, // Not implemented (SendCtrlBrk always)
                               const char *lpszCaption, unsigned long ulStyle, char **lplpszText,
                               long *pOperCount, int iElementCount, bool bUseAnsi);

_RSPNLEXP void RsDoneIndicator();

_RSPNLEXP bool RsUseIndicator(unsigned long ulOperCount, int iIndicNum);

_RSPNLEXP bool RsAddIndicator(const char *lpszText, unsigned long ulOperCount, bool bUseAnsi);

_RSPNLEXP bool RsRemIndicator(bool bRemFromPanel);

_RSPNLEXP bool RsReinitIndicator(const char *lpszText, unsigned long ulOperCount, int iIndicNum, bool bUseAnsi);


///////////////////////////////////////////////////////
// Input Box
///////////////////////////////////////////////////////
_RSPNLEXP int RsInputBox(void *pParent, const char *lpszTitle, const char *lpszText, void *pValue, int iType,
                         unsigned int uiEditWidth, unsigned int uiStringWidth, int iPointCount, bool bUseAnsi);

_RSPNLEXP int RsPasswordInputBox(void *pParent, const char *lpszTitle, const char *lpszText, void *pValue, int iType,
                                 unsigned int uiEditWidth, unsigned int uiStringWidth, int iPointCount,
                                 bool bUsePswd, bool bUseAnsi);

_RSPNLEXP int RsFormatInputBox(void *pParent, const char *lpszTitle, const char *lpszText, void *pValue, int iType,
                               unsigned int uiEditWidth, bool bUsePswd, const char *lpszFormat, bool bUseAnsi);

///////////////////////////////////////////////////////
// Open file dialog
///////////////////////////////////////////////////////
_RSPNLEXP int RsSelectFile(void *pParent, char *lpszOutBuff, size_t uiOutBuffLength, const char *lpszFileMask,
                           const char *lpszInitDir, bool bOpenDlg, const char *lpszTitle, bool bUseMultySel,
                           const char *lpszFilesGroupName, bool bIsLocal, bool bUseAnsi);

///////////////////////////////////////////////////////
// Select folder dialog
///////////////////////////////////////////////////////
_RSPNLEXP int RsSelectFolder(void *pParent, char *lpszOutBuff, size_t uiOutBuffLength, const char *lpszTitle,
                             const char *lpszInitDir, unsigned long uFlags, bool bIsLocal, bool bUseAnsi);

///////////////////////////////////////////////////////
// Choose font dialog
///////////////////////////////////////////////////////
_RSPNLEXP int RsChooseFont(void *pParent, void *pInitFont, unsigned long initRgbColor, unsigned long uFlags,
                           void **ppSelectedFont, unsigned long *pRgbColor, bool bIsLocal);

///////////////////////////////////////////////////////
// Choose color dialog
///////////////////////////////////////////////////////
_RSPNLEXP int RsChooseColor(void *pParent, unsigned long initRgbColor, unsigned long uFlags,
                            unsigned long *pRgbColor, bool bIsLocal);


#ifdef __cplusplus
}
#endif

#endif // __RSDLGAPI_H

/* <-- EOF --> */