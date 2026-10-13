//-*--------------------------------------------------------------------*-
//
// File Name   : RSHostExt.h
// Programmer  : Leonid S. Pavlov
//
// Copyright (c) 1991 - 2000 by R-Style Software Lab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------*-

#ifndef __RSHOSTEXT_H__
#define __RSHOSTEXT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//-*--------------------------------------------------------------------*-

MIDL_INTERFACE("8D4A6430-B0A7-4b4b-AED1-AA813E8172AD")
IOverlappedWnd : public IUnknown
{
 virtual void STDMETHODCALLTYPE activate() = 0;
 virtual bool STDMETHODCALLTYPE enable(bool bEnable) = 0; // if (::IsWindow(>m_hWnd))
 virtual bool STDMETHODCALLTYPE canClose(bool bAppTerm) = 0;
 virtual void STDMETHODCALLTYPE close() = 0; // post
 virtual void STDMETHODCALLTYPE destroy() = 0;
};

//-*--------------------------------------------------------------------*-

MIDL_INTERFACE("F9E77C3D-849A-4c88-8520-E56715D09D6C")
IWndManager : public IUnknown
{
 virtual void STDMETHODCALLTYPE addWnd(IOverlappedWnd *pWnd) = 0;
 virtual void STDMETHODCALLTYPE removeWnd(IOverlappedWnd *pWnd) = 0;  // вызвать при разрушении
 virtual IOverlappedWnd *STDMETHODCALLTYPE getActiveWnd() = 0;

 virtual void STDMETHODCALLTYPE activateCurrent() = 0;
 virtual void STDMETHODCALLTYPE activateNext() = 0;
 virtual void STDMETHODCALLTYPE enableAllWnd(bool bEnable) = 0;
 
 virtual void STDMETHODCALLTYPE onActivate(IOverlappedWnd *pWnd) = 0; // theApp.SetActiveFrame

 virtual void STDMETHODCALLTYPE setExtManager(IWndManager *pMgr) = 0;

 virtual bool STDMETHODCALLTYPE canCloseApplication() = 0;
 virtual void STDMETHODCALLTYPE destroyAllWnd() = 0;
 
 // onShow-onHide -- перенесение в хвост списка и обратно?
};

//-*--------------------------------------------------------------------*-

class CMainFrame;

MIDL_INTERFACE("505E9C9F-9F02-4b77-9F07-C895297D646E")
IHostWnd : public IUnknown
{
 virtual CMainFrame *STDMETHODCALLTYPE getThis() = 0;
};

//-*--------------------------------------------------------------------*-

#endif // __RSHOSTEXT_H__

/* <-- EOF --> */