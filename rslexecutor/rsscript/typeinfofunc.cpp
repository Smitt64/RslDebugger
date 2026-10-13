// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "statvars.h"
#include "rsl/isymbol.h"
#include "playrep.h"
#include "statvars.h"
#include "registerinfobase.h"
#include "typeinfo_p.h"
#include "registerobjlist.hpp"

unsigned short GenObjRelease(TGenObject *o)
{
    QObjectRsl *obj = (QObjectRsl *)o;
    unsigned short refCount = --obj->refCount;

    if(!obj->refCount && obj->owner == RegisterInfoBase::RslOwner)
        delete obj->object;

    return refCount;
}

unsigned short GenObjAddRef(TGenObject *o)
{
    QObjectRsl *obj = (QObjectRsl*)o;
    obj->refCount++;
    return obj->refCount;
}

uintptr_t GenObjTypeId(TGenObject *obj)
{
    return (uintptr_t)obj->vtbl;
}

int GenObjAttach(TGenObject *o, const char *method, SYMPROC *aProc)
{
    return -1;
}

int GenObjNprops(void) { return 0; }
int GenObjNMethods(void) { return 0; }
int GenObjCanInherit(void) { return 1; }
int GenObjCvtToIDisp(TGenObject *pObj) { return 1; }

int GenObjMemberFromID(TGenObject *pObj, long uniqID, long *id)
{
    *id = uniqID;
    /*const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());

        if (*id >= 1 && *id <= info->ArrProp.size())
            return 1;

        if (*id >= OBJ_RSL_METHOD_OFFSET && *id <= info->ArrMethod.size() +
                OBJ_RSL_METHOD_OFFSET)
            return 0;

        if(*id == RSL_DISP_ENUM)
           return 0;*/

    return  1;   // Property
}


int SetObjectGeneration(QObjectRsl *obj, unsigned short gen)
{
    //RegisterInfoBase *info = findInfo(obj->className);

    //int  i;
    int  curGen = obj->generation;

    if(obj->generation == gen) // Already mark this object
        return curGen;

    obj->generation = gen;

    /*for(i = info->ArrProp.size(); i; --i)
    {
        if(info->ArrProp[i - 1].value.v_type == V_GENOBJ)
            RslSetGenerationToObject(P_GOBJ(info->ArrProp[i - 1].value.value.obj), gen);
    }*/

    //if(obj->extObj)
        //RslSetGenerationToObject(obj->extObj, gen);

    return curGen;
}

// -----------------------------------------------------------------------------
int GenTypeGetFuncEx(int ind, TGenInfo *p, TMemberInfo *info)
{
    int              i, j, n;
    unsigned char    kind;

    TDescPropMeth   *pDesc;
    TPropMethTable  *pTbl;
    int              nm = 0, np = 0;

    for(i = 0, pDesc = p->meth; i < p->nMeth; ++i, ++pDesc)
        nm += pDesc->num;

    for(i = 0, pDesc = p->prop; i < p->nProp; ++i, ++pDesc)
        np += pDesc->num;

    if(ind < nm)
    {
        for(i = 0, pDesc = p->meth; i < p->nMeth; ++i, ++pDesc)
        {
            if(ind < pDesc->num)
            {
                info->name = pDesc->tbl[ind].name;
                info->id   = pDesc->tbl[ind].id;

                if(info->id >= 0)
                    info->id += np;

                info->declType = pDesc->tbl[ind].tp;
                info->nParm    = pDesc->tbl[ind].nParm;
                info->parInfo  = pDesc->tbl[ind].parInfo;
                info->flags    = pDesc->tbl[ind].flags;
                info->kind     = RSL_KIND_RUN;

                return 0;
            }

            ind -= pDesc->num;
        }
    }

    ind -= nm;

    n = 0;
    for(i = 0, pDesc = p->prop; i < p->nProp; ++i, ++pDesc)
    {
        for(j = 0, pTbl = pDesc->tbl; j < pDesc->num; ++j, ++pTbl)
        {
            if(n == ind)
            {
                kind = RSL_KIND_GET;

                break;
            }

            ++n;

            if(!(pTbl->flags & VAL_FLAG_RDONLY))
            {
                kind = RSL_KIND_SET;

                if(n == ind)
                    break;

                ++n;
            }

        }

        if(j < pDesc->num)
        {
            info->name     = pTbl->name;
            info->id       = pTbl->id;
            info->declType = pTbl->tp;
            info->nParm    = pTbl->nParm;
            info->parInfo  = pTbl->parInfo;
            info->flags    = pTbl->flags;
            info->kind     = kind;

            return 0;
        }
    }

    return -1;
}

// -----------------------------------------------------------------------------
int GenTypeGetMemberEx(long id, TGenInfo *p, TMemberInfo *info)
{
    int  i, j, np = 0;
    TDescPropMeth   *pDesc;
    TPropMethTable  *pTbl;

    for(i = 0, pDesc = p->prop; i < p->nProp; ++i, ++pDesc)
        np += pDesc->num;

    for(i = 0, pDesc = p->prop; i < p->nProp; ++i, ++pDesc)
        for(j = 0, pTbl = pDesc->tbl; j < pDesc->num; ++j, ++pTbl)
            if(pTbl->id == id)
            {
                info->name     = pTbl->name;
                info->id       = pTbl->id;
                info->declType = pTbl->tp;
                info->nParm    = pTbl->nParm;
                info->parInfo  = pTbl->parInfo;
                info->flags    = pTbl->flags;
                info->kind     = RSL_KIND_GET;

                if(!(info->flags & VAL_FLAG_RDONLY))
                    info->kind |= RSL_KIND_SET;

                return 0;
            }

    if(id >= 0)
        id -= np;

    for(i = 0, pDesc = p->meth; i < p->nMeth; ++i, ++pDesc)
        for(j = 0, pTbl = pDesc->tbl; j < pDesc->num; ++j, ++pTbl)
            if(pTbl->id == id)
            {
                info->name = pTbl->name;
                info->id   = pTbl->id;

                if(info->id >= 0)
                    info->id += np;

                info->declType = pTbl->tp;
                info->nParm    = pTbl->nParm;
                info->parInfo  = pTbl->parInfo;
                info->flags    = pTbl->flags;
                info->kind     = RSL_KIND_RUN;

                return 0;
            }

    return -1;
}

/*static int GetTypeInfoProc(const QMetaObject &meta, TGenInfo *p, int cmd, void *data)
{
    TMemberInfo  *info = (TMemberInfo *)data;
    RegisterInfoBase *clsinfo = RegisterObjList::inst()->info(meta.className());

    if(cmd == GOBJ_GETTYPEINFO)
        return GenTypeCalcEx(clsinfo, p, (TClassInfo *)data);
    else if(cmd == GOBJ_GETPROPBYIND)
        return GenTypeGetPropEx(clsinfo, info->ind, p, info);
    else if(cmd == GOBJ_GETFUNCBYIND)
        return GenTypeGetFuncEx(info->ind, p, info);
    else if(cmd == GOBJ_MEMBERBYID)
        return GenTypeGetMemberEx(info->id, p, info);

    return -1;
}
*/
