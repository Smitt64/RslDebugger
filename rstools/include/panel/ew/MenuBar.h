/*************************************************************************
   File Name      : MenuBar.h
   Programmer     : Alexey Lebedev

   Creation       : 17 Dec 2003
   Description    : MenuBar declaration

   Copyright(C) 2003 by R-Style Software Lab.
   All Rights Reserved.

*************************************************************************/

#ifndef __MENUBAR_H
#define __MENUBAR_H

#ifndef _INC_WINDOWS
#define  STRICT
#include <windows.h>
#endif
#pragma hdrstop

// Extended styles
#define  MENUBAR_FOCUSMODE     0x00000010

#ifdef __cplusplus
extern "C" {
#endif

HWND     CreateRebarWnd(HWND hParentWnd, UINT nID, DWORD dwStyle);

BOOL     AddRebarBand(HWND hRebarWnd, HWND hBandWnd, int nBandID, LPTSTR lpstrTitle,
                      BOOL bNewRow, BOOL bNoGripper, int cxWidth, BOOL bFullWidthAlways);

BOOL     UpdateRebarBand(HWND hRebarWnd, int nBandID, int cxWidth, BOOL bFullWidthAlways);

void     SizeRebarBands(HWND hRebarWnd);

HWND     CreateMenuBar(HWND hParentWnd, UINT nID, HMENU hMenu);
HMENU    SetMenuBarMenu(HWND hMenuBar, HMENU hMenu);
BOOL     UpdateMenuBar(HWND hMenuBar);
BOOL     BlockMenuBar(HWND hMenuBar, BOOL bBlockTracking);

BOOL     BlockToolBar(HWND hToolBar, BOOL bBlockTracking);

BOOL     UpdateMenuBarBand(HWND hMenuBar);

DWORD    GetMenuBarExtendedStyle(HWND hMenuBar);
DWORD    SetMenuBarExtendedStyle(HWND hMenuBar, DWORD dwExtendedStyle, DWORD dwMask);

BOOL     IsMenuBarMessage(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT  ProcessMenuBarMessage(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
};
#endif

#endif //#ifndef __MENUBAR_H

/* <-- EOF --> */