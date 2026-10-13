//-*--------------------------------------------------------------------------*-
//
// File Name   : rsverinf.h
//
// Copyright (c) 1991 - 2005 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// June 20, 1997 Sergei Kubrin (K78) - Create file
//-*--------------------------------------------------------------------------*-

// Vesion info structre

#ifndef __RSVERINF_H
#define __RSVERINF_H

#if !defined(RC_INVOKED)
    #include "pexpdef.h"
#endif

#define  RSL_VER_SYSTEM_LEN    60
#define  RSL_VER_COPYRT_LEN    58
#define  RSL_VER_CLIENT_LEN    51

#define  RSL_VER_MAGIC_1       "RSL"
#define  RSL_VER_MAGIC_2       "BUILD"
#define  RSL_VER_MAGIC_3       "VERINFO"

#define  RSL_VER_MAGIC         "RSL_BUILD_VERINFO"

// Для старых пакетов клиент <-> СП. Не менять !!!
#define  RSCPRIGHT_AS           "Copyright (c) 1993-2015 R-Style Softlab."

#define  RC_COMPANY_NAME_WIN   "└╬ ""▌Ё-╤Єрщы ╤юЇЄырс""\0"
#define  RC_COPYRIGHT_WIN      "(c) 1993-2019 └╬ ""▌Ё-╤Єрщы ╤юЇЄырс"""

#define  RSCPRIGHT_OEM          "(c) 1993-2019 АО \"Эр-Стайл Софтлаб\""
#define  RSCPRIGHT_WIN          "(c) 1993-2019 └╬ \"▌Ё-╤Єрщы ╤юЇЄырс\""

#define  RSCPRIGHT              RSCPRIGHT_OEM

#define  RSL_VER_TYPE0          0
#define  RSL_VER_TYPE1          1
#define  RSL_VER_TYPE2          2
#define  RSL_VER_TYPE20         20     // Расширение 2 для инструмента (новая структура, как 4 и 5)
#define  RSL_VER_TYPE3          3      // #2909
#define  RSL_VER_TYPE300        300    // То же, что и RSL_VER_TYPE3, но по формату А.ББ.ВВВ.ГГГ(ДД)
#define  RSL_VER_TYPE301        301    // То же, что и RSL_VER_TYPE3, но по формату А.ББ.ВВВ.ГГГ
#define  RSL_VER_TYPE302        302    // То же, что и RSL_VER_TYPE3, но по формату А.ББ.ВВВ.Г
#define  RSL_VER_TYPE4          4      // #3440 для 5.50 (резерв)
#define  RSL_VER_TYPE5          5      // #3440 для V.6


#define  RSL_VER_TYPE           RSL_VER_TYPE2

// Предполагается использование только в инструменте.
#define  RSL_VER_TOOLS          RSL_VER_TYPE20

#if !defined(RC_INVOKED)
    #include <packpshn.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

    // -------------------------------------------
    // Старая версия структуры
    typedef struct
          {
           char            magic[18];  // !!!! Must be RSL_BUILD_VERINFO !!!!!
           unsigned short  build;
           unsigned short  verHi;
           unsigned short  verLow;

           char            system[RSL_VER_SYSTEM_LEN];
           char            copyRight[RSL_VER_COPYRT_LEN];
           short int       verInfoType;
           unsigned short  subBuild; 
           
           unsigned short  build2;
           unsigned short  verHi2;
           unsigned short  verLow2;
           unsigned short  subBuild2; 

           unsigned short  complect; 
          } TRSLVersionInfo0;

    // -------------------------------------------
    // Новая версия структуры
    typedef struct
          {
           char            magic[18];  // !!!! Must be RSL_BUILD_VERINFO !!!!!
           unsigned short  build;
           unsigned short  verHi;
           unsigned short  verLow;

           char            system[RSL_VER_SYSTEM_LEN];
           char            copyRight[RSL_VER_COPYRT_LEN];
           short int       verInfoType;
           unsigned short  subBuild; 
           
           unsigned short  build2;
           unsigned short  verHi2;
           unsigned short  verLow2;
           unsigned short  subBuild2;

           unsigned short  complect; 

           // #3440  Поля для идентификации клиентских сборок
           char            clientName[RSL_VER_CLIENT_LEN];  // Наименование клиента

           unsigned short  verHiClnt;                       // А
           unsigned short  verLowClnt;                      // ББ
           unsigned short  buildClnt;                       // ВВВ
           unsigned short  subBuildClnt;                    // ГГГ(ДД)
           unsigned short  updateClnt;                      // ДД
          } TRSLVersionInfo;

    // -------------------------------------------
    extern TRSLVersionInfo  RSLVersionInfo;


    // -------------------------------------------
    _RSPNLEXP TRSLVersionInfo *getRSLVerInfo(void);
    _RSPNLEXP void SetRSLVerInfo(TRSLVersionInfo *ptr);

    _RSPNLEXP void RSLVerInfoToStr(char *buff, const TRSLVersionInfo *verinfo);

    // Вывести версионные данные по заданной версии
    _RSPNLEXP void RSLVerInfoToStrEx(char *buff, const TRSLVersionInfo *verinfo, int outByVersion);

    _RSPNLEXP int GetTOOLVersion(void);
         

    // You MUST initialize variable anyware as shown:
    //
    // TRSLVersionInfo RSLVersionInfo =
    // { RSL_VER_MAGIC , 123, 4, 30, "System name",RSCPRIGHT };

    // Version declaration help macros
    #define  RSL_DECLARE_VERINFO_FULL_EX(_system, _verHi, _verLow, _build, _subBuild, _verHi2, _verLow2, _build2, _subBuild2, _verType)  \
                                        TRSLVersionInfo RSLVersionInfo =                                                                 \
                                        { RSL_VER_MAGIC, _build, _verHi, _verLow, _system, RSCPRIGHT, _verType, _subBuild,               \
                                          _build2, _verHi2, _verLow2, _subBuild2, 0 };

    #define  RSL_DECLARE_VERINFO_FULL(_system, _verHi, _verLow, _build, _subBuild, _verHi2, _verLow2, _build2, _subBuild2)  \
                                     RSL_DECLARE_VERINFO_FULL_EX(_system, _verHi, _verLow, _build, _subBuild, _verHi2, _verLow2, _build2, _subBuild2, RSL_VER_TOOLS)

    #define  RSL_DECLARE_VERINFO(_system, _verHi, _verLow, _build, _subBuild) \
                                RSL_DECLARE_VERINFO_FULL(_system, _verHi, _verLow, _build, _subBuild, 0, 0, 0, 0)

    #define  RSL_DECLARE_VERINFO2(_system, _verHi, _verLow, _build, _subBuild) \
                                 RSL_DECLARE_VERINFO_FULL(_system, 0, 0, 0, 0, _verHi, _verLow, _build, _subBuild)

    #define  RSL_DECLARE_VERINFO3(_system, _verHi, _verLow, _build, _subBuild) \
                                 RSL_DECLARE_VERINFO_FULL_EX(_system, 0, 0, 0, 0, _verHi, _verLow, _build, _subBuild, RSL_VER_TYPE3)

    #define  RSL_DECLARE_VERINFO_CLIENT_FULL(_verType, _system, _copyright, _verHi, _verLow, _build, _subBuild, _client, _verHiClnt, _verLowClnt, _buildClnt, _subBuildClnt, _updateClnt)  \
                                        TRSLVersionInfo  RSLVersionInfo =                                                                 \
                                        { RSL_VER_MAGIC, _build, _verHi, _verLow, _system, _copyright, _verType, _subBuild,               \
                                          0, 0, 0, 0, 0, _client, \
                                          _verHiClnt, _verLowClnt, _buildClnt, _subBuildClnt, _updateClnt };

    #define  RSL_DECLARE_VERINFO_CLIENT(_verType, _system, _copyright, _verHi, _verLow, _build, _subBuild, _client, _subBuildClnt, _updateClnt)  \
                                       RSL_DECLARE_VERINFO_CLIENT_FULL(_verType, _system, _copyright, _verHi, _verLow, _build, _subBuild, _client, 0, 0, 0, _subBuildClnt, _updateClnt)

    #ifdef __cplusplus
    }
    #endif

    #include <packpop.h>
#endif   // #if !defined(RC_INVOKED)

#endif  // #ifndef __RSVERINF_H

/* <-- EOF --> */