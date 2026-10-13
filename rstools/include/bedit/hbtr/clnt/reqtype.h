/*-----------------------------------------------------------------------*-

 File Name   : reqtype.h 

 Sybsystem   :   
 Description : 

 Source      : 

 Library(s)  : 


 Copyright (c) 1991 - 1994 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 July 17,1996  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __REQTYPE_H
#define __REQTYPE_H

// -----------------------------------------------
enum TRslReqType
   {
    RSL_REQ_USESTD,
    RSL_REQ_TCP,
    RSL_REQ_SPX,
    RSL_REQ_PIPE,

    RSL_REQ_SQLEMU
   };

// -----------------------------------------------
enum TRslMapType
   {
    RSL_REQ_USEMAP,
    RSL_REQ_SERVPATH,
    RSL_REQ_TRANCPATH
   };

#endif

/* <-- EOF --> */