/**********************************************************************
*        File Name:   bmsearch.h         October 5,1994            
*       Programmer:   S. Kubrin              
*      Description:   
*  Public routines:  
* Private routines:  
***********************************************************************/

#ifndef __BMSEARCH_H
#define __BMSEARCH_H

#include "pexpdef.h"

#define _BMSEARCH_

typedef struct
      {
       unsigned char    sch[256];        // Искомая строка in used codePage
       unsigned char ardisp[256];
       short int            flag;
       unsigned char    codePage;
      } SchData;

#define  S_CASE     1
// Used in KED
#define  S_WORD     2

#ifdef __cplusplus
extern "C" {
#endif

_RSPNLEXP void InitSearch(SchData *sch );
_RSPNLEXP void InitSearchEx(SchData *sch, unsigned char codePage);

_RSPNLEXP int  BMsearch(SchData *sch, unsigned char *s, int N, int *delta);
//  return 1 - found and delta == offset in s, 0 - notfound
//  s - where to find
//  N - lenghth of string to find

#ifdef __cplusplus
}
#endif

#endif

/* <-- EOF --> */