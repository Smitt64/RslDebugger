//-*--------------------------------------------------------------------------*-
//
// File Name   : ewpanel.h
//
// Copyright (c) 1991 - 2004 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// February 22,2001  Alexey Lebedev - Create file
//-*--------------------------------------------------------------------------*-

#ifndef __EWPANEL_H
#define __EWPANEL_H

#include "fs.h"
#include "bedit.h"
#include "rsmain.h"


// -----------------------------------------------------------------------------
// Для каждой функции имеется два варианта: локальный и удаленный.
// Суффикс _impl указывает на локальную реализацию функции (или просто реализацию).
// Суффикс _remote означает вызов удаленной реализации.
//
// Нижеследующий define вводит определение локальной и удаленной реализации для
// некоторой функции.
// Параметры:
//    type - возвращаемое значение
//    name - имя функции без суффиксов
//    arglist - спецификация аргументов функции

#define  EW_DECL_METHOD(type, name, arglist) _RSPNLEXP type name##_impl   arglist; \
                                             _RSPNLEXP type name##_remote arglist;


// -----------------------------------------------------------------------------
// Нижеследующий define используется для формирования имени функции, которая
// содержит локальную реализацию некторой функции.
#define  EW_IMPL_METHOD(type, name, arglist)    type name##_impl   arglist
#define  EW_REMOTE_METHOD(type, name, arglist)  type name##_remote arglist

// -----------------------------------------------------------------------------
// Непосредственный вызов локальной или удаленной реализации метода
#ifdef RSL_EASYWIN_LOCAL                          // локальная реализация
    #define  EW_CALL_IMPL(name)    name##_impl
    #define  EW_CALL_REMOTE(name)
#elif defined RSL_EASYWIN_REMOTE                  // удалённая реализация
    #define  EW_CALL_IMPL(name)
    #define  EW_CALL_REMOTE(name)  name##_remote
#else                                             // в 2-х звенке локальная, в 3-х удаленная
    #define  EW_CALL_IMPL(name)    name##_impl
    #define  EW_CALL_REMOTE(name)  name##_remote
#endif

// -----------------------------------------------------------------------------
// С помощью нижеследующих defin'ов происходит настройка на локальный, удаленный или
// комбинированный интерфейс со всеми экспортируемыми функциями. В зависимости от того,
// какой из define'ов определен из прикладного кода будет вызываться:
//    - локальная реализация,
//    - удалённая реализация,
//    - в 2-х звенке локальная, в 3-х удаленная.

#ifdef RSL_EASYWIN_LOCAL                                  // локальная реализация
    #define  EW_CALL_METHOD(name)  EW_CALL_IMPL(name)
#elif defined RSL_EASYWIN_REMOTE                          // удалённая реализация
    #define  EW_CALL_METHOD(name)  EW_CALL_REMOTE(name)
#else                                                     // в 2-х звенке локальная, в 3-х удаленная
    #define  EW_CALL_METHOD(name)  (isRemote ? EW_CALL_REMOTE(name) : EW_CALL_IMPL(name))
#endif

// -----------------------------------------------------------------------------
// Типы сообщений
enum
   {
    EW_NONE_MES        = 0,
    EW_ASYNC_MES       = 100,
    EW_NOBLOCK_MES     = 200,

    EW_INFORMATION_MES = 1,
    EW_WARNING_MES,
    EW_ERROR_MES,
    EW_ONTOP_MES
   };

// -----------------------------------------------------------------------------
// Идентификаторы секций в статусной строке
enum
   {
    EW_TEXT_PANE       = 1,

    EW_FILTR_PANE      = 10,

    EW_NUMLOCK_PANE    = 30,
    EW_CAPSLOCK_PANE,
    EW_SCROLLLOCK_PANE,
    EW_INSOVR_PANE,
    // -------------------------------------------
    // Иконки, выводимые без бордюра
    EW_TRACE_FLAG,
    EW_MODE_FLAG,
    EW_NOTIFY_FLAG,
    EW_INPUT_FLAG,
    // -------------------------------------------

    EW_DATE_PANE       = 40,
    EW_TIME_PANE,

    EW_USER_PANE       = 100,
   };

// Границы диапазона иконок без бордюра
#define  EW_NOBORDER_BEGIN   EW_TRACE_FLAG
#define  EW_NOBORDER_END     EW_INPUT_FLAG

// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
   Прототипы функций с локальной и удаленной реализациями
*************************************************************************/

//////////////////////////////////
// STARTUP AND CLEANUP
//////////////////////////////////
EW_DECL_METHOD(int,  ew_initInstance,  (int, void *, uintptr_t));
EW_DECL_METHOD(void, ew_exitInstance,  (void));

//////////////////////////////////
// Инициализация/деинициализация шлюза
//////////////////////////////////
EW_DECL_METHOD(int,  ew_initMessageGate,  (int, void *));
EW_DECL_METHOD(void, ew_doneMessageGate,  (void));

//////////////////////////////////
// Отправка сообщения через шлюз
//////////////////////////////////
EW_DECL_METHOD(int,  ew_SendMessageGate,  (int, void *, void *));

//////////////////////////////////
// VERSION
//////////////////////////////////
EW_DECL_METHOD(int,  ew_getVersion,    (void));

//////////////////////////////////
// APP
//////////////////////////////////
EW_DECL_METHOD(void, ew_setAppInfo,    (const char *, const char *, const char *, const char *, bool));
EW_DECL_METHOD(void, ew_setAppServInfo,(int, const char *, const char *, int));
EW_DECL_METHOD(void, ew_setOverMode,   (bool));

EW_DECL_METHOD(int,  ew_SetAppSize,    (int cmdSize));

//////////////////////////////////
// CAPTURE
//////////////////////////////////
EW_DECL_METHOD(void, ew_setCapture,    (int));

//////////////////////////////////
// EVENT
//////////////////////////////////
EW_DECL_METHOD(int,  ew_getKey,        (void));
EW_DECL_METHOD(int,  ew_testKey,       (void));
EW_DECL_METHOD(void, ew_sendEvent,     (TFSEvent*));

//////////////////////////////////
// WINDOW
//////////////////////////////////
EW_DECL_METHOD(void, ew_createWindow,  (const char *, int, int));
EW_DECL_METHOD(void, ew_destroyWindow, (void));
EW_DECL_METHOD(void, ew_openWindow,    (void));
EW_DECL_METHOD(void, ew_closeWindow,   (void));

//////////////////////////////////
// TITLE
//////////////////////////////////
EW_DECL_METHOD(void, ew_setTitle,      (const PANEL *, const char *));
EW_DECL_METHOD(int,  ew_getTitle,      (const PANEL *, char *, int));

//////////////////////////////////
// MESWIN, FLDWIN, CONFWIN
//////////////////////////////////
EW_DECL_METHOD(int,  ew_meswin,        (const char *text, int type, int help,
                                        const char *caption, const char *status, int infoType, long errCode));

EW_DECL_METHOD(int,  ew_fldwin,        (PANEL *, const char *text, int type, int help,
                                        const char *caption, const char *status));

EW_DECL_METHOD(int,  ew_confwin,       (const char *text, const char *btext,
                                        int curfld, int length, int type, int help,
                                        const char *caption, const char *status, int infoType, long errCode))


EW_DECL_METHOD(const char*, ew_webwin, (const char *url, const char *attr,
                                        const char *caption, const char *status, const char *resultField, size_t *outSz))

EW_DECL_METHOD(void, ew_freeWebResult, (const char *ptr))


//////////////////////////////////
// MENU
//////////////////////////////////
EW_DECL_METHOD(int,  ew_createMenu,    (PANEL *p, MENU *, int));
EW_DECL_METHOD(int,  ew_createPopupMenu,(PANEL *p, MENU *, int, int, int));
EW_DECL_METHOD(int,  ew_appendMenu,    (PANEL *, MENU *, int));
EW_DECL_METHOD(void, ew_destroyMenu,   (PANEL *));
EW_DECL_METHOD(int,  ew_enableMenuItem,(PANEL *, MENU *, bool enable));
EW_DECL_METHOD(int,  ew_deleteMenuItem,(PANEL *, MENU *));
EW_DECL_METHOD(int,  ew_insertMenuItem,(PANEL *, MENU *, MENU *, int, int, bool));
EW_DECL_METHOD(int,  ew_loopMenu,      (PANEL *, bool));

EW_DECL_METHOD(int,  ew_iconMenuItem,  (PANEL *p, MENU *m, int iconID, int flags));

//////////////////////////////////
// PANEL
//////////////////////////////////
EW_DECL_METHOD(int,  ew_putPanel,      (PANEL *, void *));
EW_DECL_METHOD(void, ew_putFields,     (PANEL *));
EW_DECL_METHOD(void, ew_putTexts,      (PANEL *));
EW_DECL_METHOD(int,  ew_procfld,       (PANEL *));
EW_DECL_METHOD(int,  ew_unknownfld,    (PANEL *));
EW_DECL_METHOD(void, ew_putField,      (PANEL *, int));

EW_DECL_METHOD(void, ew_runPanel,      (PANEL *, int));
EW_DECL_METHOD(void, ew_remPanel,      (PANEL *));
EW_DECL_METHOD(void, ew_switchPanel,   (const PANEL *));
EW_DECL_METHOD(int,  ew_setFldParm,    (void));
EW_DECL_METHOD(char*,ew_getFldText,    (PANEL *, int));

EW_DECL_METHOD(void, ew_setScrollBar,  (PANEL *pnl, char, int, int, int, unsigned));

EW_DECL_METHOD(void, ew_setHelpContext, (PANEL *pnl, const char *context));

EW_DECL_METHOD(void, ew_updateFieldTooltip, (PANEL *pnl, int cf, const char *text));

EW_DECL_METHOD(int,  ew_getFieldData,  (PANEL *pnl, int cf, int type, size_t *outSize, void **outData));
EW_DECL_METHOD(int,  ew_putFieldData,  (PANEL *pnl, int cf, int type, size_t imgSize, void *imgData));


//////////////////////////////////
// BSCROL, SCROL
//////////////////////////////////
EW_DECL_METHOD(int,  ew_putScrol,      (SCROL *, void *, size_t, int *));
EW_DECL_METHOD(int,  ew_putBScrol,     (BSCROL *, void *, size_t, int *));
EW_DECL_METHOD(void, ew_moveScroll,    (const PANEL *, int, int, int));
EW_DECL_METHOD(void, ew_clearItem,     (const PANEL *, int));
EW_DECL_METHOD(void, ew_scrollTo,      (const PANEL *, int));
EW_DECL_METHOD(void, ew_setBScrolEditMode,   (const PANEL *, bool));
EW_DECL_METHOD(int,  ew_resizeScroll,  (PANEL *, int, int, void *));
EW_DECL_METHOD(void*,ew_saveScrolColWidth,  (PANEL *, size_t*));
EW_DECL_METHOD(void, ew_showColumn,    (PANEL *, int, int));
EW_DECL_METHOD(void, ew_resetColumns,  (PANEL *));
EW_DECL_METHOD(void, ew_drawHead,      (PANEL *pnl));
EW_DECL_METHOD(void, ew_selectColumn,  (const PANEL *, int, int, bool));
EW_DECL_METHOD(int,  ew_getColOrdWidth,(const PANEL *, void *, size_t));
EW_DECL_METHOD(void, ew_startVScroll,  (PANEL *));
EW_DECL_METHOD(void, ew_setColumnText, (PANEL *pnl, int col, const char *head, const char *tips));

//////////////////////////////////
// TEXT EDITOR
//////////////////////////////////
EW_DECL_METHOD(int,  ew_putEditor,     (PANEL *, int, int, int *, int *, int *));
EW_DECL_METHOD(void, ew_showCaret,     (PANEL *, int line, int pos, int caret));
EW_DECL_METHOD(void, ew_outputLine,    (PANEL *, int line, int len, int selBegin, int selEnd, const char *str, unsigned char *attr));
EW_DECL_METHOD(void, ew_updateEditor,  (PANEL *));
EW_DECL_METHOD(void, ew_setEditorOvrMode,(int overMode));
EW_DECL_METHOD(void, ew_sendEditor,    (PANEL *, int, const char *, const char *));
EW_DECL_METHOD(void, ew_initEditor,    (int cmdll, const char *ext, const char *txt));

//////////////////////////////////
// TAB CONTROL
//////////////////////////////////
EW_DECL_METHOD(int,  ew_putTabCtrl,    (PANEL *, int, int, int, int));
EW_DECL_METHOD(void, ew_setActiveTab,  (PANEL *, int));

//////////////////////////////////
// STATUS LINE
//////////////////////////////////
EW_DECL_METHOD(void, ew_putStatLine,   (const char *));
EW_DECL_METHOD(void, ew_saveStatLine,  (void *));
EW_DECL_METHOD(void, ew_restStatLine,  (void *));
EW_DECL_METHOD(int,  ew_sendNotify,    (const char *head, const char *text, int secID, int iconID, int timeOut, int colorText, int colorBack));

//////////////////////////////////
// STATUS LINE PANE
//////////////////////////////////
EW_DECL_METHOD(void, ew_addStatusPane, (int id, int cx, int before));
EW_DECL_METHOD(void, ew_delStatusPane, (int id));
EW_DECL_METHOD(void, ew_setStatusText, (int id, const char *text, const char *tips, int flags));
EW_DECL_METHOD(void, ew_setStatusIcon, (int id, int icon, const char *tips));
EW_DECL_METHOD(void, ew_saveStatusPane,(int id, void **, size_t *));
EW_DECL_METHOD(void, ew_restStatusPane,(int id, void *, size_t));

EW_DECL_METHOD(void, ew_putStatusText, (const PANEL *, int, const char *));
EW_DECL_METHOD(void, ew_putStatusPane, (const PANEL *, int, int, const char *));
EW_DECL_METHOD(void, ew_remStatusPane, (const PANEL *, int));

//////////////////////////////////
// ACTIVE INDICATOR
//////////////////////////////////
EW_DECL_METHOD(void, ew_putIndAct,     (const char *msg, unsigned flags));
EW_DECL_METHOD(void, ew_remIndAct,     (void));
EW_DECL_METHOD(void, ew_changeIndAct,  (const char *));


//////////////////////////////////
// LOCK INPUT
//////////////////////////////////
EW_DECL_METHOD(void *, ew_lockUserInput, (int flags, UnlockInputProc unlockProc, void *unlockData));
EW_DECL_METHOD(void, ew_unlockUserInput, (void *));

EW_DECL_METHOD(int,  ew_setBusy,       (int version, int busy));

//////////////////////////////////
// MULTI MESSAGE
//////////////////////////////////
EW_DECL_METHOD(void, ew_sendDeferMessages, (void));

/*************************************************************************
   Макросы для вызова функций из прикладного кода
*************************************************************************/

//////////////////////////////////
// STARTUP AND CLEANUP
//////////////////////////////////
#define  ew_initInstance    EW_CALL_METHOD(ew_initInstance)
#define  ew_exitInstance    EW_CALL_METHOD(ew_exitInstance)

//////////////////////////////////
// Инициализация/деинициализация шлюза
//////////////////////////////////
#define  ew_initMessageGate EW_CALL_METHOD(ew_initMessageGate)
#define  ew_doneMessageGate EW_CALL_METHOD(ew_doneMessageGate)

//////////////////////////////////
// Отправка сообщения через шлюз
//////////////////////////////////
#define  ew_SendMessageGate EW_CALL_METHOD(ew_SendMessageGate)

//////////////////////////////////
// VERSION
//////////////////////////////////
#define  ew_getVersion      EW_CALL_METHOD(ew_getVersion)

//////////////////////////////////
// APP
//////////////////////////////////
#define  ew_setAppInfo      EW_CALL_METHOD(ew_setAppInfo)
#define  ew_setAppServInfo  EW_CALL_METHOD(ew_setAppServInfo)
#define  ew_setOverMode     EW_CALL_METHOD(ew_setOverMode)

#define  ew_SetAppSize      EW_CALL_METHOD(ew_SetAppSize)

//////////////////////////////////
// CAPTURE
//////////////////////////////////
#define  ew_setCapture      EW_CALL_METHOD(ew_setCapture)

//////////////////////////////////
// EVENT
//////////////////////////////////
#define  ew_getKey          EW_CALL_METHOD(ew_getKey)
#define  ew_testKey         EW_CALL_METHOD(ew_testKey)
#define  ew_sendEvent       EW_CALL_METHOD(ew_sendEvent)

//////////////////////////////////
// WINDOW
//////////////////////////////////
#define  ew_createWindow    EW_CALL_METHOD(ew_createWindow)
#define  ew_destroyWindow   EW_CALL_METHOD(ew_destroyWindow)
#define  ew_openWindow      EW_CALL_METHOD(ew_openWindow)
#define  ew_closeWindow     EW_CALL_METHOD(ew_closeWindow)

//////////////////////////////////
// TITLE
//////////////////////////////////
#define  ew_setTitle        EW_CALL_METHOD(ew_setTitle)
#define  ew_getTitle        EW_CALL_METHOD(ew_getTitle)

//////////////////////////////////
// MESWIN, FLDWIN, CONFWIN
//////////////////////////////////
#define  ew_meswin          EW_CALL_METHOD(ew_meswin)
#define  ew_fldwin          EW_CALL_METHOD(ew_fldwin)
#define  ew_confwin         EW_CALL_METHOD(ew_confwin)
#define  ew_webwin          EW_CALL_METHOD(ew_webwin)
#define  ew_freeWebResult   EW_CALL_METHOD(ew_freeWebResult)

//////////////////////////////////
// MENU
//////////////////////////////////
#define  ew_createMenu      EW_CALL_METHOD(ew_createMenu)
#define  ew_createPopupMenu EW_CALL_METHOD(ew_createPopupMenu)
#define  ew_appendMenu      EW_CALL_METHOD(ew_appendMenu)
#define  ew_destroyMenu     EW_CALL_METHOD(ew_destroyMenu)
#define  ew_enableMenuItem  EW_CALL_METHOD(ew_enableMenuItem)
#define  ew_deleteMenuItem  EW_CALL_METHOD(ew_deleteMenuItem)
#define  ew_insertMenuItem  EW_CALL_METHOD(ew_insertMenuItem)
#define  ew_loopMenu        EW_CALL_METHOD(ew_loopMenu)
#define  ew_iconMenuItem    EW_CALL_METHOD(ew_iconMenuItem)

//////////////////////////////////
// PANEL
//////////////////////////////////
#define  ew_putPanel        EW_CALL_METHOD(ew_putPanel)
#define  ew_putFields       EW_CALL_METHOD(ew_putFields)
#define  ew_putTexts        EW_CALL_METHOD(ew_putTexts)
#define  ew_procfld         EW_CALL_METHOD(ew_procfld)
#define  ew_unknownfld      EW_CALL_METHOD(ew_unknownfld)
#define  ew_putField        EW_CALL_METHOD(ew_putField)

#define  ew_runPanel        EW_CALL_METHOD(ew_runPanel)
#define  ew_remPanel        EW_CALL_METHOD(ew_remPanel)
#define  ew_switchPanel     EW_CALL_METHOD(ew_switchPanel)
#define  ew_setFldParm      EW_CALL_REMOTE(ew_setFldParm)
#define  ew_getFldText      EW_CALL_METHOD(ew_getFldText)

#define  ew_setScrollBar    EW_CALL_METHOD(ew_setScrollBar)

#define  ew_setHelpContext  EW_CALL_METHOD(ew_setHelpContext)

#define  ew_updateFieldTooltip  EW_CALL_METHOD(ew_updateFieldTooltip)

#define  ew_getFieldData    EW_CALL_METHOD(ew_getFieldData)
#define  ew_putFieldData    EW_CALL_METHOD(ew_putFieldData)

//////////////////////////////////
// BSCROL, SCROL
//////////////////////////////////
#define  ew_putScrol        EW_CALL_METHOD(ew_putScrol)
#define  ew_putBScrol       EW_CALL_METHOD(ew_putBScrol)
#define  ew_moveScroll      EW_CALL_METHOD(ew_moveScroll)
#define  ew_clearItem       EW_CALL_METHOD(ew_clearItem)
#define  ew_scrollTo        EW_CALL_METHOD(ew_scrollTo)
#define  ew_setBScrolEditMode  EW_CALL_METHOD(ew_setBScrolEditMode)
#define  ew_resizeScroll    EW_CALL_METHOD(ew_resizeScroll)
#define  ew_restScrolColWidth  EW_CALL_METHOD(ew_restScrolColWidth)
#define  ew_saveScrolColWidth  EW_CALL_METHOD(ew_saveScrolColWidth)
#define  ew_showColumn      EW_CALL_METHOD(ew_showColumn)
#define  ew_resetColumns    EW_CALL_METHOD(ew_resetColumns)
#define  ew_drawHead        EW_CALL_METHOD(ew_drawHead)
#define  ew_selectColumn    EW_CALL_METHOD(ew_selectColumn)
#define  ew_getColOrdWidth  EW_CALL_METHOD(ew_getColOrdWidth)
#define  ew_startVScroll    EW_CALL_METHOD(ew_startVScroll)
#define  ew_setColumnText   EW_CALL_METHOD(ew_setColumnText)

//////////////////////////////////
// TEXT EDITOR
//////////////////////////////////
#define  ew_putEditor          EW_CALL_METHOD(ew_putEditor)
#define  ew_showCaret          EW_CALL_METHOD(ew_showCaret)
#define  ew_outputLine         EW_CALL_METHOD(ew_outputLine)
#define  ew_updateEditor       EW_CALL_METHOD(ew_updateEditor)
#define  ew_setEditorOvrMode   EW_CALL_METHOD(ew_setEditorOvrMode)
#define  ew_sendEditor         EW_CALL_METHOD(ew_sendEditor)
#define  ew_initEditor         EW_CALL_METHOD(ew_initEditor)

//////////////////////////////////
// TAB CONTROL
//////////////////////////////////
#define  ew_putTabCtrl      EW_CALL_METHOD(ew_putTabCtrl)
#define  ew_setActiveTab    EW_CALL_METHOD(ew_setActiveTab)

//////////////////////////////////
// STATUS LINE
//////////////////////////////////
#define  ew_putStatLine     EW_CALL_METHOD(ew_putStatLine)
#define  ew_saveStatLine    EW_CALL_METHOD(ew_saveStatLine)
#define  ew_restStatLine    EW_CALL_METHOD(ew_restStatLine)
#define  ew_sendNotify      EW_CALL_METHOD(ew_sendNotify)

//////////////////////////////////
// STATUS LINE PANE
//////////////////////////////////
#define  ew_addStatusPane   EW_CALL_METHOD(ew_addStatusPane)
#define  ew_delStatusPane   EW_CALL_METHOD(ew_delStatusPane)
#define  ew_setStatusText   EW_CALL_METHOD(ew_setStatusText)
#define  ew_setStatusIcon   EW_CALL_METHOD(ew_setStatusIcon)
#define  ew_saveStatusPane  EW_CALL_METHOD(ew_saveStatusPane)
#define  ew_restStatusPane  EW_CALL_METHOD(ew_restStatusPane)
#define  ew_putStatusText   EW_CALL_METHOD(ew_putStatusText)
#define  ew_putStatusPane   EW_CALL_METHOD(ew_putStatusPane)
#define  ew_remStatusPane   EW_CALL_METHOD(ew_remStatusPane)

//////////////////////////////////
// ACTIVE INDICATOR
//////////////////////////////////
#define  ew_putIndAct       EW_CALL_METHOD(ew_putIndAct)
#define  ew_remIndAct       EW_CALL_METHOD(ew_remIndAct)
#define  ew_changeIndAct    EW_CALL_METHOD(ew_changeIndAct)

//////////////////////////////////
// LOCK INPUT
//////////////////////////////////
#define  ew_lockUserInput   EW_CALL_METHOD(ew_lockUserInput)
#define  ew_unlockUserInput EW_CALL_METHOD(ew_unlockUserInput)

#define  ew_setBusy         EW_CALL_METHOD(ew_setBusy)

//////////////////////////////////
// MULTI MESSAGE
//////////////////////////////////
#define  ew_sendDeferMessages  EW_CALL_REMOTE(ew_sendDeferMessages)

int ew_showHelp(int page, const char *context = NULL, int key = 0);

#ifdef __cplusplus
}
#endif

#endif //__EWPANEL_H

/* <-- EOF --> */