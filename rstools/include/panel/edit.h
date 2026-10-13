/*******************************************************************************
 FILE         :   EDIT.H

 COPYRIGHT    :   R-Style Software Lab, 1994, 1998

 DESCRIPTION  :

 PROGRAMMED BY:   Serge Kubrin

 CREATION DATE:   2 Aug 1994

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/panel/h/edit.h $
                  $Revision: 14 $
                  $Date: 18.02.00 19:50 $
                  $Author: Lebedev $
                  $Modtime: 18.02.00 18:02 $
*******************************************************************************/
/*
 December 29,1994   Serg Kubrin (K78)
            Поддержка двух окон в редакторе

*/

#ifndef __EDIT_H
#define __EDIT_H

#ifndef edit_h
#define edit_h

#include "rstypes.h"
#include "prindef.h"
#include "edit/viewed.h"
#include "menu.h"

#include "apptldef.h"


typedef struct ViewData
      {
       db_int16   percent;
       db_int16   position;
       db_int32   line;
       PANEL     *pv;

       View      *v, *v1, *v2;
       int        x1, y1, x2, y2;
       int        wId;
       unsigned char useHighlight;
       unsigned char partLineVisible;

       #ifdef RSL_EASYWIN
           void  *statbuf;
           size_t statsize;
       #endif
      } ViewData;


/*************************************************************************
   Интерфейс TEditIntf
*************************************************************************/

enum TEditIntfFlags
   {
    TEditIntf_noWait = 0x0001,
    TEditIntf_readOnly,
   };

typedef struct
      {
       bool (*runExtEditor)(uint32 flags, const char *editor, const char *file);
       bool (*runGUIEditor)(uint32 flags, const char *file,   const char *title);
      } TEditIntf;

#ifdef __cplusplus
 extern "C" {
#endif

_RSPNLEXP long _StdPanelDialog(View *v, int fun, va_list ap);
_RSPNLEXP int  StdPanelHandle(View *v, int event);
_RSPNLEXP void PrintViewMessage(char *mes);
_RSPNLEXP void ClrViewMes(void);
_RSPNLEXP int  InitViewPanel(ViewData *d, char *title, int ll, int mode);
_RSPNLEXP void DoneViewPanel(ViewData *d);
_RSPNLEXP int  SetDsblEdit(int dsbl);
_RSPNLEXP int  HandleMouseEvents(View *v, int event);
_RSPNLEXP int  HandleScrollEvents(View *v, int event);
_RSPNLEXP int  TranslateEvent(int kbkod );
_RSPNLEXP int  NewViewName(View *v);
_RSPNLEXP void PutDecor(ViewData *d,int);
_RSPNLEXP int  IsMouseInView(View *v);
_RSPNLEXP void SetCoord(ViewData *d);

_RSPNLEXP int  ResizeView (View *v, int dx, int dy, int flag);
_RSPNLEXP void PrintFile(const char *fileName, bool useAltPrint);

_RSPNLEXP const char *SetExtViewer(const char *newViewer);
_RSPNLEXP const char *SetExtEditor(const char *newEditor);
_RSPNLEXP int         SetViewerMode(int wait);       // 0 - nowait
_RSPNLEXP TEditIntf  *SetEditIntf(TEditIntf *newIntf);
_RSPNLEXP long        SetEditFileSize(long newSize);
_RSPNLEXP int         StartExtEditor(const char *Fname, const char *Title, int mode);

_APPTLFUNC int  ShowFV(const char *Fname, const char *Title);
_APPTLFUNC int  EditFV(const char *Fname, const char *Title, int mode);


typedef void (*ESignProcType) (int code, const char *fileName);
               
_APPTLFUNC ESignProcType SetSignProc(ESignProcType proc);

#ifdef __cplusplus
 };
#endif

#endif // edit_h

#endif // __EDIT_H

/* <-- EOF --> */