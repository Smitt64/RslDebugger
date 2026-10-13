#ifndef __TRANMGR_H
#define __TRANMGR_H

#ifdef __cplusplus
extern "C" {
#endif

void initTrnMgr();
void doneTrnMgr();
bool connectTrnMgr();
bool addRscMgr();
bool addRscMgrEx (btrvem_h db);
void removeRscMgr(btrvem_h db);

bool CanUseTrnMgr();

#ifdef __cplusplus
}
#endif

#endif //__TRANMGR_H

/* <-- EOF --> */