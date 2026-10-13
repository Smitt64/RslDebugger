//-*--------------------------------------------------------------------------*-
//
// File Name   : marklist.h
//
// Copyright (c) 1991 - 2004 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// Augest 03, 2005 Elena Lebedeva - Create file
//-*--------------------------------------------------------------------------*-

#ifndef __MARKLIST_H
#define __MARKLIST_H

#include "listobj.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagMARK 
      {
       LOBJ   list_elem;
       void  *mark;
      } MARKELEM;

typedef struct tagMARKLIST
      {
       int      offset;
       int      size;
       int      nitems;
       CNTLIST  m_list;
      } MARKLIST;


MARKLIST *CreateMarkList(int offset, int size);

bool  AddRecToMarkList     (void *rec, MARKLIST *MList);
bool  RemoveRecFromMarkList(void *rec, MARKLIST *MList);
void  RemoveAddRecMarkList (void *rec, MARKLIST *MList);
void  DestroyMarkList      (MARKLIST *MList);
bool  IsRecInMarkList      (void *rec, MARKLIST *MList);
void  ClearMarkList        (MARKLIST *MList);
void *GetFirslMark         (MARKLIST *MList);
void *GetNextMark          (MARKLIST *MList, void *item);
int   GetNumMark           (MARKLIST *MList);

#ifdef __cplusplus
}
#endif

#endif   // __MARKLIST_H

/* <-- EOF --> */