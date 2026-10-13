#ifndef __LRU_H
#define __LRU_H

#include "platform.h"
#include "listobj.h"
#include "bexpdef.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------
typedef bool  (*LRU_CHOOSE_PROC)(void *obj, void *primaryData);
typedef void *(*LRU_CREATE_PROC)(void *primaryData);
typedef void  (*LRU_DONE_PROC)(void *obj);

// -----------------------------------------------
typedef struct
      {
       CNTLIST          items;
       unsigned         waterline;
       LRU_CHOOSE_PROC  choose;
       LRU_CREATE_PROC  create;
       LRU_DONE_PROC    done; 
      } LRU;

// -----------------------------------------------
typedef struct
      {
       LOBJ      lObj;
       unsigned  refCount;
       void     *obj;
      } LRU_ITEM;


// -----------------------------------------------------------------------------
_BTEXP void lruInit(LRU *lru, unsigned waterline, LRU_CHOOSE_PROC choose, LRU_CREATE_PROC create, LRU_DONE_PROC done);

_BTEXP void  lruDone(LRU *lru);
_BTEXP void *lruGet(LRU *lru, void *primaryData);
_BTEXP void  lruFree(LRU *lru, void *obj);

#ifdef __cplusplus
};
#endif

#endif

/* <-- EOF --> */