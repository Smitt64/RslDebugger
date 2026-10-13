#ifndef __RSTRNMGR_H
#define __RSTRNMGR_H

#include "conapp.h"
#include "nwbtrv.h"
#include "btrv.h"

// -----------------------------------------------------------------------------
class CRscMgr : public CRsResourceMgr
    {
     bclient_h  id;

     public:
       CRscMgr() : id(BTRVEM_DEF_HANDLE) {}
       CRscMgr(bclient_h id_) : id(id_) {}
       
       // ----------------------------------------
       virtual TrnResult CanUseMSDTC()
       {
        return TRN_FAIL;
       }

       // ----------------------------------------
       virtual TrnResult EnlistMSDTC(ITransaction *)
       {
        return TRN_FAIL;
       }
       
       // ----------------------------------------
       int Begin(int attr)  
       { 
        int       res;
        btrvem_h  oldDb = SetCurrentDb(id);


        if(!bfBTEx(attr, id))
          res = TRN_OK;
        else
          res = TRN_FAIL;

        SetCurrentDb(oldDb);

        return res;
       }

       // ----------------------------------------
       int Commit()         
       { 
        int       res;
        btrvem_h  oldDb = SetCurrentDb(id);


        if(!bfETEx(id))
          res = TRN_OK;
        else
          res = TRN_FAIL;

        SetCurrentDb(oldDb);

        return res;
       }

       // ----------------------------------------
       int Abort()          
       { 
        int res;
        btrvem_h  oldDb = SetCurrentDb(id);


        if(!bfATEx(id))
          res = TRN_OK;
        else
          res = TRN_FAIL;

        SetCurrentDb(oldDb);

        return res;
       }

       // ----------------------------------------
       virtual TrnResult GetModuleName(char *name, int len) 
       { 
        RslGetRTLFileName(name, len);

        return TRN_OK;
       }

       // ----------------------------------------
       virtual TrnResult Init()  
       { 
        if(!RSRTLInit())
          return TRN_OK;
        else
          return TRN_FAIL;
       }

       // ----------------------------------------
       virtual TrnResult Done()  
       { 
        RSRTLDoneDll(0);

        return TRN_OK;
       }

       // ----------------------------------------
       bclient_h GetClientId()
       {
        return id;
       }
    };

#endif //__RSTRNMGR_H

/* <-- EOF --> */