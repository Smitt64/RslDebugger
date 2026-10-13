//-*--------------------------------------------------------------------------*-
//
// File Name   : WndManager.h
//
// Copyright (c) 1991 - 2005 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// May 05, 2005 Alexey Lebedev - Create file
//-*--------------------------------------------------------------------------*-

//
// EasyWin window mamanger
//

#ifndef __WNDMANAGER_H__
#define __WNDMANAGER_H__

#define _ATL_SINGLE_THREAD
#include <atlbase.h>

extern CComModule _Module;

#include <atlcom.h>

#include "RSHostExt.h"


//---------------------------------------------------------------------------
// CWndManager
class CWndManager : public IWndManager, public CComObjectRoot
    {
     DECLARE_NO_REGISTRY()
     DECLARE_NOT_AGGREGATABLE(CWndManager)

     BEGIN_COM_MAP(CWndManager)
        COM_INTERFACE_ENTRY(IWndManager)
     END_COM_MAP()

     // IWndManager
     public:
       STDMETHOD_(void, addWnd)(IOverlappedWnd *pWnd);
       STDMETHOD_(void, removeWnd)(IOverlappedWnd *pWnd);
       STDMETHOD_(IOverlappedWnd *, getActiveWnd)();
       STDMETHOD_(void, activateCurrent)();
       STDMETHOD_(void, activateNext)();
       STDMETHOD_(void, enableAllWnd)(bool bEnable);
       STDMETHOD_(void, onActivate)(IOverlappedWnd *pWnd);
       STDMETHOD_(void, setExtManager)(IWndManager *pMgr);
       STDMETHOD_(bool, canCloseApplication)();
       STDMETHOD_(void, destroyAllWnd)();
    };


//---------------------------------------------------------------------------
// COverlappedWnd
class COverlappedWnd : public IOverlappedWnd, public CComObjectRoot
    {
     DECLARE_NO_REGISTRY()
     DECLARE_NOT_AGGREGATABLE(COverlappedWnd)

     BEGIN_COM_MAP(COverlappedWnd)
        COM_INTERFACE_ENTRY(IOverlappedWnd)
     END_COM_MAP()

     // IOverlappedWnd
     public:
       STDMETHOD_(void, activate)();
       STDMETHOD_(bool, enable)(bool bEnable);
       STDMETHOD_(bool, canClose)(bool bAppTerm);
       STDMETHOD_(void, close)();
       STDMETHOD_(void, destroy)();

     // Ctor
     public:
       COverlappedWnd();

     // Methods
     public:
       void InitFrame();

     // Attributes
     private:
       bool     m_bFrameWnd;
    };

#endif //__WNDMANAGER_H__

/* <-- EOF --> */