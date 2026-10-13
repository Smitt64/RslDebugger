/*************************************************************************
        File Name               : hook.hpp                      05.06.2001
        Programmer              : E.Lebedeva

        Description             : Класс HookList предназначен для реализации списка 
                    перехватчиков калвиатуры

        Copyright(C) 2001 by R-Style Software Lab.
        All Rights Reserved.

*************************************************************************/


#ifndef __HOOK_HPP
#define __HOOK_HPP

#include "menu.h"
#include "listobj.h"

// Прототип функции hook'a
typedef int (*HookProc)(PANEL *p, int key, void *data);

#ifdef __cplusplus
    class HookList
        {
         public :

           HookList()
           {
            InitCntList(&h_list, LST_DEFAULT, offsetof(HookObject, list_elem));
           };

          ~HookList()
           {
            Clear();
           };
            
           void Add(const char *PName, int k, HookProc f, void *dt = 0,
                    const char *MIName = 0, const char *status = 0, int h = 0);

           void Clear();

           int  Nitems()
           {
            return LobjNitems(&h_list);
           };

           int  FindAndExecute(PANEL *p, int key);

           void CreateMenuItems(PANEL *p);

         private :

           struct HookObject
                {
                 HookObject(const char *PName, int k, HookProc f, void *dt = 0, 
                            const char *MIName = 0, const char *status = 0, int h = 0);

                ~HookObject();

                 const char *PanelName;
                 int         key;
                 HookProc    proc;
                 void       *data;
                 char       *MenuItemName;
                 const char *par;
                 int         help;
                 LOBJ        list_elem;
                };

           CNTLIST  h_list;
        };
#endif


#ifdef __cplusplus
extern "C" {
#endif

// Установить перехватчик (hook)
_RSPNLEXP void SetHook( 
                       const char *ResPanName,       // Имя ресурса панели
                       int         key,              // Клавиша, на кот. устанавливается hook
                       HookProc    f_hook,           // Функция-hook
                       void       *data,             // Для передачи данных (м.б.0)
                       const char *MenItemName,      // Имя добавляемого пункта в меню"Разное"(м.б.0)
                       const char *status,           // Сообщение в статусную строку (м.б.0)
                       int         hlp               // Номер страницы помощи (м.б.0)
                      );

_RSPNLEXP void ClearHookList(void);  // Очистить список перехватчиков

// Добавить к подменю "Разное" пункты из списка перехватчиков 
// (используется только в инструменте)
extern void CreateHookMenuItems(PANEL *p);

extern int IsHook(void); // (используется только в инструменте)

extern int UseHookList(PANEL *p, int key);

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */