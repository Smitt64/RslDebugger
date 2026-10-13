/*************************************************************************
   File Name      : hhelp.h
   Programmer     : Alexey Lebedev

   Creation       : 17 May 2002
   Description    : HtmlHelp support

   Copyright(C) 2002 by R-Style Software Lab.
   All Rights Reserved.

*************************************************************************/

#ifndef __HHELP_H
#define __HHELP_H

#ifndef __PEXPDEF_H
#include "pexpdef.h"
#endif // PEXPDEF.H

#ifdef __cplusplus
extern "C" {
#endif

_RSPNLEXP extern bool hhelpShown;

// Displays a help topic
_RSPNLEXP bool HHelpShowTopic(const char *fName, const char *topic);

// Displays a help topic based on a mapped topic ID
_RSPNLEXP bool HHelpShowTopicID(const char *fName, unsigned long topicID);

// Opens a pop-up help topic
_RSPNLEXP bool HHelpShowPopupText(const char *fName, unsigned long topicID);

// Looks up one or more keywords
_RSPNLEXP bool HHelpKeywordLookup(const char *fName,const char *keyword);

// Closes all windows opened directly or indirectly by the calling program
_RSPNLEXP void HHelpQuit(void);


uint32 LocalHtmlHelp(int cmd, const char *fileName, const char *topic, unsigned long topicID);
void DoneHtmlHelp(void);


#include <packpsh1.h>

typedef struct
      {
       db_int32    cmd;
       db_uint32   topicID;
      } THHelpCommand;

typedef struct
      {
       db_uint32   hwnd;
      } THHelpResult;

#include <packpop.h>

#ifdef __cplusplus
}
#endif

#endif // __HHELP_H

/* <-- EOF --> */