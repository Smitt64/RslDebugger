//-*--------------------------------------------------------------------------*-
//
// File Name   : FmtEditImpl.h
//
// Copyright (c) 1991 - 2004 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// April 29,2004  Nechaev - Create file
//-*--------------------------------------------------------------------------*-


// FmtEditImpl.h : Declaration of Format Edit Implementation

#ifndef __FMTEDITIMPL_H__
#define __FMTEDITIMPL_H__

#include <vector>
#include "RSFmtIntfC.h"

#ifndef RWSTD_NO_NAMESPACE
    using std::vector;
#endif

/////////////////////////////////////////////////////////////////////////////

BOOL RSIsClipboardDataAvailable(UINT *p_fmts, int p_count = 1);

/////////////////////////////////////////////////////////////////////////////
// CRSFmtHandle

#define  RSFMT_CALL(hnd, method)            hnd->method(hnd)
#define  RSFMT_CALL1(hnd, method, a1)       hnd->method(hnd, a1)
#define  RSFMT_CALL2(hnd, method, a1, a2)   hnd->method(hnd, a1, a2)

class CRSFmtHandle
    {
     public:
       CRSFmtHandle();
      ~CRSFmtHandle();

       void           Init(PFORMAT p_pFmt = NULL);
       bool           IsInited() const;
       bool           IsReal() const;
       PFORMAT        Format();

     public:     // API
       bool           Create(const char *p_fmtstr);

       void           SetVarType(int p_type, int p_prm2 = -1);
       int            GetVarType(int *p_prm2 = NULL);

       unsigned char  SetAlign(unsigned char p_align);
       unsigned char  GetAlign();

       bool           ImportStr(const char *p_str);
       bool           ImportVal(const fmt_value *p_val);
       bool           Export(fmt_value *p_val);

       const char    *Result();
       void           RawResult(char *p_buf, int p_size);

       bool           SetLimit(unsigned long p_len);

       bool           BackSpace();
       bool           Insert(const char *p_str, int p_len = -1);
       bool           Del();
       bool           ResetString();

       bool           Move(int p_method /* FT_... */);
       bool           MoveTo(unsigned long p_pos);
       unsigned long  Current();

       bool           Select(int p_method /* FT_... */);
       bool           SelectTo(long p_pos);
       void           Unselect();
       bool           IsSelected();
       unsigned long  LeftSelected();
       unsigned long  RightSelected();

       unsigned char  ModeSet(unsigned char p_mode);
       unsigned char  ModeGet();

       unsigned char  CodeSet(unsigned char p_code);
       unsigned char  CodeGet();

       bool           IsAllSel();
       bool           IsValid() const;
       bool           IsDirty() const;

       void           CursorAtLastChar(int p_enable);

       void           SetSpaceSymbol(char p_symbol);
       void           SetDefSelection();
       void           SetDefSpaceSymbol();
       void           SetDirty(bool dirty);

     protected:
       PFORMAT        m_pFmt;

     private:
       int            _GetNextPos(int p_pos, int p_method /* FT_... */, bool p_abs = false);
       void           _Select(int p_method /* FT_... */, bool p_abs = false);

       #if !defined RSL_MIN_CRT
           struct STRE;
           STRE      *m_pStrE;
       #endif //#if !defined RSL_MIN_CRT
    };

/////////////////////////////////////////////////////////////////////////////
// CRSFmtEditImpl

struct RSFMTEDIT_DRAWATTR
     {
      COLORREF s_clrBack;
      COLORREF s_clrFore;
      COLORREF s_clrSelBack;
      COLORREF s_clrSelFore;

      HBRUSH   s_brBack;
     };

enum RSFMTEDIT_HA
   {
    RSFMTEDIT_HA_LEFT    = 1,
    RSFMTEDIT_HA_CENTER  = 2,
    RSFMTEDIT_HA_RIGHT   = 3
   };

enum RSFMTEDIT_VA
   {
    RSFMTEDIT_VA_TOP     = 1,
    RSFMTEDIT_VA_CENTER  = 2,
    RSFMTEDIT_VA_BOTTOM  = 3
   };

class CRSFmtEditImpl
    {
     private:
       void PutCursorForInsertMode();

     public:
       CRSFmtEditImpl();

       void  SetViewRect( const RECT *p_rc, const RECT *p_margins = NULL);
       const RECT *GetViewRect(RECT *p_margins = NULL) const
       {
        if(p_margins)
          *p_margins = m_margins;

        return &m_rc;
       }

       void  SelectAll();
       void  SetSel(int p_start, int p_end, bool p_redraw = true);
       void  GetSel(int *p_start, int *p_end );
       void  ReplaceSel(const char *p_text);
       void  SetDefSelection();

       void  SetInsertMode(bool bOverMode);
       void  ChangeInsertMode();

       HANDLE GetNativeData();
       HANDLE GetTextData();

       void  SetNativeData(HANDLE hData);

       void  Copy();
       void  Paste();
       void  Cut();
       void  Clear();
       BOOL  IsClipbrdData();

       LRESULT  ProcessMsg(const MSG &p_stMsg, BOOL *p_bHandled);
       void  Draw(HDC p_hDc, const RSFMTEDIT_DRAWATTR *p_stAttrs);
       void  DrawEx(HDC p_hDc, const RECT &p_rc, const RSFMTEDIT_DRAWATTR *p_stAttrs);

     public:  // Events
       virtual  int FmtEditImpl_GetXOffset () { return 0; }
       virtual  CRSFmtHandle &FmtEditImpl_OnGetFmt () = 0;
       virtual  HFONT FmtEditImpl_OnGetFont() = 0;
       virtual  HWND  FmtEditImpl_OnGetHwnd() { return 0; }
       virtual  HDC   FmtEditImpl_OnDcGet() = 0;
       virtual  void  FmtEditImpl_OnDcRelease(HDC p_dc) = 0;
       virtual  void  FmtEditImpl_OnInvalidate() = 0;

       virtual  void  FmtEditImpl_OnCaretCreate(SIZE &p_sz) = 0;
       virtual  void  FmtEditImpl_OnCaretSetPos(POINT &p_pos, POINT &textOffs) = 0;
       virtual  void  FmtEditImpl_OnCaretDestroy() = 0;

       virtual  void  FmtEditImpl_OnCaptureCreate() = 0;
       virtual  BOOL  FmtEditImpl_OnIsCapture() = 0;
       virtual  void  FmtEditImpl_OnCaptureRelease() = 0;

       virtual  void  FmtEditImpl_OnChanged();
       virtual  void  FmtEditImpl_OnDataChanged();
       virtual  void  FmtEditImpl_OnModeChanged();

       virtual  void  FmtEditImpl_OnSetFocus()  {}
       virtual  void  FmtEditImpl_OnKillFocus() {}

       virtual  const char *FmtEditImpl_OnGetText();
       virtual  BOOL  FmtEditImpl_OnIsPasswd();
       virtual  BOOL  FmtEditImpl_OnIsReadOnly();
       virtual  BOOL  FmtEditImpl_OnCopyFormat();

       virtual  RSFMTEDIT_HA FmtEditImpl_OnGetHorAligh();
       virtual  RSFMTEDIT_VA FmtEditImpl_OnGetVerAligh();

       virtual  void  FmtEditImpl_GetViewRect(RECT &rect) { rect = m_rc; }

     protected:
       void CheckCaretWidth();

     private:
       struct _DRAW
            {
             HDC         s_dc;
             RECT        s_rc;
             HRGN        s_clip;

             const RSFMTEDIT_DRAWATTR  *s_stAttrs;

             int         s_offs;
             int         s_iCaretWidth;
             int         s_iFontHeight;
            };

       int CalculateCaretWidth();

     protected:  // Handler prototypes:
       LRESULT  OnPaint    (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnCreate   (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnDestryoy (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       

       LRESULT  OnSetFocus  (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnKillFocus (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

       LRESULT  OnChar    (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnKeyDown (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

       LRESULT  OnSetCursor      (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnCaptureChanged (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

       LRESULT  OnLButtonDown     (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnMouseMove       (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnMouseLeave      (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnLButtonUp       (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnLButtonDblClick (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

       LRESULT  OnCut   (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnCopy  (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnPaste (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
       LRESULT  OnClear (UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

     protected:  // Operations
       virtual bool OnCreateCore();
       virtual bool OnDestroyCore();
       virtual bool OnLButtonDownCore(int xPos, int yPos, bool shiftP);
       virtual bool OnLButtonUpCore(int xPos, int yPos);
       virtual bool OnLButtonDblClickCore (int xPos, int yPos);
       virtual bool OnMouseMoveCore(int xPos, int yPos, bool shiftP);
       virtual void OnMouseLeaveCore();

     private:
       virtual bool IsUsingXpStyles();
       virtual int CalculatePosWithXpStyles(int p_offs, int *alignedOffset);

     protected:
       int CalculatePosDefault(int p_offs, int *alignedOffset);

       void  InitDraw(_DRAW &p_stDraw);
       void  Track(int p_offs, bool f_sel);

       int CalculatePos(int p_offs, int *alignedOffset = 0);
       int CalculateTextOffset(_DRAW &p_stDraw, int string_size, int field_size);

       void  Txt_Draw(_DRAW &p_stDraw);
       int   Txt_GetOffs(HDC p_dc, int p_pos);
       int   Txt_GetWidth(HDC p_dc, int p_pos);
       int   Txt_GetHeight(HDC p_dc);
       int   Txt_GetPos(HDC p_dc, int p_offs, int *p_offschar = NULL);
       int   Txt_AdjustOffs(HDC p_dc, int p_offs);
       void  Txt_GetRect(_DRAW &p_stDraw, RECT &p_rc);
       bool  Txt_GetSelRect(HDC p_dc, RECT &p_rc, int *p_from, int *p_to);

       bool  IsFocus();

       unsigned char _GetFontCode();
       void  _UpdateDisplayString();
       void  _UpdateButtonType();
       void  _UpdateButtonWidth(bool p_update = true);
       void  _UpdateAutosize();
       void  _OnDataChanged();

       void  _ClipReg();
       void  _ClipPasteAnsi(HANDLE &p_data);
       void  _ClipPasteOem(HANDLE &p_data);

     protected:
       RECT           m_rc;
       RECT           m_margins;
       int            m_iCaretWidth;
       int            m_iFontHeight;

       vector<char>   m_strDisp;

       unsigned       m_fTracking : 1;
       unsigned       m_fFocused : 1;

       int            m_offs;
       int            m_shift;
       const char    *m_str;
       unsigned       m_len;
      };

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#endif //__FMTEDITIMPL_H__

/* <-- EOF --> */