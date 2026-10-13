// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "rsllistwrapper.h"
#include <malloc.h>

#define  TARRAY_SIZE_ID           -100
#define  TARRAY_SIZE_ITEMS        -102
//#define  TARRAY_VALUE_ID          -101

TGenObject *MakeListContainer(TRslListWrapper *interface);

static TPropMethTable ArrProp[] = {
    { "Size",  TARRAY_SIZE_ID },
    { "children",  TARRAY_SIZE_ITEMS }
};

static TPropMethTable  ArrMethod[] = {
    { "createEnum", RSL_DISP_ENUM }
};

/*TRslListWrapper::TRslListWrapper(TRslListWrapperInteface *item)
{
    refCount = 0;
    owner = RegisterInfoBase::CppOwner;
    memset(&derived, 0, sizeof(TGenObject));
    interface = item;
}

TRslListWrapper::~TRslListWrapper()
{
    //delete interface;
}*/

// -----------------------------------------------------------------------------
static TDescPropMeth  Props[]   = { { NMP(ArrProp), ArrProp} };
static TDescPropMeth  Methods[] = { { NMP(ArrMethod), ArrMethod} };
static TGenInfo       GenInfo   = { NMP(Props), Props, NMP(Methods), Methods };

static long __IBtoI(VALUE *v)
{
    long  ret = 0;

    if(v->v_type == V_INTEGER)
        ret = v->value.intval;
    else if(v->v_type == V_BIGINT)
        ret = (long)v->value.bigint;

    return ret;
}

static int GenObjFindMember(const char *name, long *id)
{
    int  i;

    /*if(stricmpR("def", name) == 0)
    {
        *id = TARRAY_VALUE_ID;

        return 1;
    }*/

    for(i = 0; i < NMP(ArrMethod); i++ )
    {
        if(stricmpR(name, ArrMethod[i].name) == 0)
        {
            *id = ArrMethod[i].id;

            return 0;
        }
    }

    for(i = 0; i < NMP(ArrProp); i++)
    {
        if(stricmpR (name, ArrProp[i].name) == 0)
        {
            *id = ArrProp[i].id;

            return 1;
        }
    }

    *id = -1;

    return -1;
}

static int GenObjNprops(void)
{
 return 10;
}

static int GenObjNMethods(void)
{
    return 0;
}

static int GenObjCanInherit(void)
{
    return 1;
}

static RSL_MAKE_TYPE_TABLE(TypeTable, GenObj);

void DoneRslList(TGenObject *o)
{
    TRslListWrapper *obj = (TRslListWrapper *)o;

    DoneListItemContainer(obj->items);

    if (obj->owner == RegisterInfoBase::RslOwner)
        obj->interface->deinit();

    delete obj->interface;
    free(obj);
}

static unsigned short GenObjRelease(TGenObject *o)
{
    TRslListWrapper  *obj = (TRslListWrapper *)o;
    unsigned short refCount = --obj->refCount;

    if (!refCount)
        DoneRslList(o);

    return refCount;
}

static unsigned short GenObjAddRef(TGenObject *o)
{
    TRslListWrapper  *obj = (TRslListWrapper *)o;

    obj->refCount++;

    return obj->refCount;
}

static int GenObjSetId(TGenObject *obj, long id, VALUE *val)
{
    VALUE *par;
    TRslListWrapper *wrp = (TRslListWrapper *)obj;

    if(id >= 0)
    {
        QVariant value = SetFromRslValue(val);
        return wrp->interface->set(id, value);
    }

    if(id == TARRAY_SIZE_ID)
    {
        if((val->v_type == V_INTEGER && val->value.intval == 0) || (val->v_type == V_BIGINT && val->value.bigint == 0))
            wrp->interface->clear();

        return 0;
    }
}

static int GenObjSet(TGenObject *obj, const char *parm, VALUE *val, long *id)
{
    int tp = GenObjFindMember(parm, id);

    if(tp == 1) // property
        return GenObjSetId(obj, *id, val);

    if(tp == 0)
        return 1;

    return -1;
}

static int GenObjGetId(TGenObject *obj, long id, VALUE *val)
{
    VALUE *par;
    TRslListWrapper *wrp = (TRslListWrapper *)obj;

    if(id >= 0)
    {
        VALUE v;
        ValueMake(&v);
        QVariant value;

        auto SetterFunc = [&v](int type, void *ptr) -> void
        {
            ValueSet(&v, type, ptr);
        };

        int stat = wrp->interface->get(id, &value);
        SetValueFromVariant(SetterFunc, value);

        ValueCopy(&v, val);

        return 0;
    }

    if(id == TARRAY_SIZE_ID)
    {
        ValueClear(val);

        int size = wrp->interface->size();
        ValueSet(val, V_INTEGER, &size);

        return 0;
    }
    else if(id == TARRAY_SIZE_ITEMS)
    {
        ValueClear(val);
        ValueSet(val, V_GENOBJ, wrp->items);
        ADDREF(wrp->items);

        return 0;
    }

    return -1;
}

static int GenObjGet(TGenObject *obj, const char *parm, VALUE *val, long *id)
{
    int  tp = GenObjFindMember(parm, id);

    if(tp == 1) // property
        return GenObjGetId(obj, *id, val);

    if(tp == 0)
        return 1;

    return -1;
}

static int GenObjRunId(TGenObject *obj, long id)
{
    if(id == RSL_DISP_CTRL) // Constructor is called from RslGenObject
    {
        //InitObj ((TArrayObj *)obj, 1);

        return 0;
    }
    /*else if(id == RSL_DISP_ENUM)
    {
        TGenObject  *pEnum = GenObjCreateEx("$TEnumArray", NULL);

        if(pEnum)
        {
            MRUNID(pEnum, RSL_DISP_CTRL);

            ReturnVal(V_GENOBJ, pEnum);

            return 0;
        }
    }*/

    return -1;
}

static int GenObjRun(TGenObject *obj, const char *method, long *id)
{
    if(GenObjFindMember(method, id) == 0)
        return GenObjRunId(obj, *id);

    return -1;
}

static const char *GenObjTypeName(TGenObject *obj)
{
    return "List";
}

static const char *GenObjTypeNameCS(TGenObject *obj)
{
    return "List";
}

static uintptr_t GenObjTypeId(TGenObject *obj)
{
 return (uintptr_t)obj->vtbl;
}

// -----------------------------------------------------------------------------
static int GenObjAttach(TGenObject *o, const char *method, SYMPROC *aProc)
{
 return -1;
}

static long GenObjGetUniqID(TGenObject *pObj, const char *name)
{
    long  id = -1;

    GenObjFindMember(name, &id);

    return id;
}

static int GenObjMemberFromID(TGenObject *pObj, long uniqID, long *id)
{
    *id = uniqID;

    if(*id == RSL_DISP_ENUM)
        return 0;

    return  1;   // Property
}

static int GenObjCvtToIDisp(TGenObject *pObj)
{
    return 1;
}

int ObjGenTypeCalcEx(TGenInfo *p, TClassInfo *info)
{
    int  i, j;
    TDescPropMeth   *pDesc;
    TPropMethTable  *pTbl;

    memset(info, 0, sizeof(TClassInfo));

    for(i = 0, pDesc = p->prop; i < p->nProp; ++i, ++pDesc)
    {
        info->nProps     += pDesc->num;
        info->nMethProps += pDesc->num;
    }

    for (i = 0, pDesc = p->meth; i < p->nMeth; ++i, ++pDesc)
    {
        info->nMeth += pDesc->num;
        info->nFunc += pDesc->num;
    }

    for(i = 0, pDesc = p->prop; i < p->nProp; ++i, ++pDesc)
        for(j = 0, pTbl = pDesc->tbl; j < pDesc->num; ++j, ++pTbl)
        {
            if(pTbl->flags & VAL_FLAG_RDONLY)
                info->nFunc += 1;
            else
                info->nFunc += 2;
        }

    return 0;
}

int ObjGenTypeGetPropEx(int ind, TGenInfo *p, TMemberInfo *info)
{
    int  i /*,np = 0*/;
    TDescPropMeth  *pDesc;


    for(i = 0, pDesc = p->prop; i < p->nProp; ++i, ++pDesc)
    {
        if(ind < pDesc->num)
        {
            info->name     = pDesc->tbl[ind].name;
            info->id       = pDesc->tbl[ind].id;
            info->declType = pDesc->tbl[ind].tp;
            info->nParm    = pDesc->tbl[ind].nParm;
            info->parInfo  = pDesc->tbl[ind].parInfo;
            info->flags    = pDesc->tbl[ind].flags;
            info->kind     = RSL_KIND_VAR | RSL_KIND_GET;

            if(!(info->flags & VAL_FLAG_RDONLY))
                info->kind |= RSL_KIND_SET;

            return 0;
        }

        ind -= pDesc->num;
    }

    return -1;
}

static int ObjGetTypeInfoProc(TGenInfo *p, int cmd, void *data)
{
    TMemberInfo  *info = (TMemberInfo *)data;

    if(cmd == GOBJ_GETTYPEINFO)
        return ObjGenTypeCalcEx(p, (TClassInfo *)data);
    else if(cmd == GOBJ_GETPROPBYIND)
        return ObjGenTypeGetPropEx(info->ind, p, info);
    else if(cmd == GOBJ_GETFUNCBYIND)
        return GenTypeGetFuncEx(info->ind, p, info);
    else if(cmd == GOBJ_MEMBERBYID)
        return GenTypeGetMemberEx(info->id, p, info);

    return -1;
}

static int setArrGeneration(TRslListWrapper *obj, unsigned short gen)
{
    long    i;
    VALUE  *v;
    int     curGen = obj->generation;

    if(obj->generation == gen) // Already mark this object
        return curGen;

    obj->generation = gen;

    for(i = 0; i < obj->interface->size(); ++i)
    {
        /*v = getAtId(obj, i);

        if(v && (v->v_type == V_GENOBJ))
            RslSetGenerationToObject(P_GOBJ(v->value.obj), gen);*/
    }

    return curGen;
}

static int GenObjEnumProps(TGenObject *obj, int cmd, void *data)
{
    if(cmd == GOBJ_SETGENERATION)
        return setArrGeneration((TRslListWrapper *)obj, *((unsigned short *)data));

    if(cmd == GOBJ_MARK_OBJECT)
    {
        int  old = ((TRslListWrapper *)obj)->marknumber;

        ((TRslListWrapper *)obj)->marknumber = *((unsigned short *)data);

        return old;
    }
    /*#ifndef NO_GC
    if(cmd == GOBJ_SETGENERATION)
      return setArrGeneration((TRslListWrapper *)obj, *((unsigned short *)data));

    if(cmd == GOBJ_MARK_OBJECT)
     {
      int  old = ((TRslListWrapper *)obj)->marknumber;

      ((TRslListWrapper *)obj)->marknumber = *((unsigned short *)data);

      return old;
     }
#endif*/

    return ObjGetTypeInfoProc(&GenInfo, cmd, data);
}

static RSL_MAKE_OBJ_TABLE(table, GenObj);

template<class Type>
TRslListWrapper *MakeObj(QList<Type> *lst = nullptr, RegisterInfoBase::QObjectRslOwner owner = RegisterInfoBase::RslOwner)
{
    TRslListWrapper *obj = (TRslListWrapper*)malloc(sizeof(TRslListWrapper));

    obj->refCount = 0;
    obj->owner = RegisterInfoBase::CppOwner;
    memset(&obj->derived, 0, sizeof(TGenObject));
    obj->interface = new TRslTemplateListWrapper<Type>();

    if (!lst)
    {
        obj->owner = RegisterInfoBase::RslOwner;
        obj->interface->init();
    }
    else
    {
        obj->owner = owner;
        obj->interface->setFromPtr((void*)lst);
    }

    obj->items = MakeListContainer(obj);

    P_GOBJ(obj)->vtbl = &table;
    ADDREF(obj);

    return obj;
}

static int InitProvider(CLNT_PRVD_HANDLE *clntId)
{
    *clntId = NULL;

    table.getUniqID         = GenObjGetUniqID;
    table.memberFromID      = GenObjMemberFromID;
    table.canCvtToIDispatch = GenObjCvtToIDisp;

    table.genProc           = GenObjEnumProps;

    return 0;
}

static void DoneProvider(CLNT_PRVD_HANDLE clntId)
{
//    /DoneCntList(&RslInst->ArrayObjList, (LCLRFUNC)DoneObj);
}

static TGenObject *CreateStringListObject(CLNT_PRVD_HANDLE clntId, const char *typeName)
{
    if(qstricmp(typeName, "StringList") != 0)
        return NULL;

    return P_GOBJ(MakeObj<QString>());
}

static IRslTypeInfo *GetTypeInfo(CLNT_PRVD_HANDLE clntId, const char *typeName)
{
    if(qstricmp(typeName, "StringList") == 0)
        return &TypeTable;

    return NULL;
}

void RegisterStringList(void)
{
    AddObjectProviderModEx(InitProvider, DoneProvider, CreateStringListObject, GetTypeInfo);
}

void RslStringList()
{
    TRslListWrapper *obj = MakeObj<QString>(nullptr);

    //InitObj(obj, 0);

    ReturnVal(V_GENOBJ, obj);
}

void *MakeStringList(QStringList *lst, RegisterInfoBase::QObjectRslOwner owner)
{
    TRslListWrapper *obj = MakeObj<QString>(lst, owner);
    return obj;
}
