// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "rsl/common.h"
#pragma hdrstop
#include "rsllistwrapper.h"
#include <stdio.h>
#include <QDebug>

static TRslListWrapperInteface *wrp(void *prm)
{
    return (TRslListWrapperInteface*)((TRslListItemContainer*)prm)->wrp;
}

static int GenObjFindMember(TRslListItemContainer *obj, const char *name, long *id)
{
    int  i;

    TRslListWrapperInteface *intrface = wrp(obj);

    if (intrface->size() == 0)
        return -1;

    if (*id >= 0 && *id < intrface->size())
        return 1;

    *id = -1;

    return -1;
}

static int GenObjNprops(void)
{
    return 0;
}

static int GenObjNMethods(void)
{
    return 0;
}

static int GenObjCanInherit(void)
{
    return 1;
}

//static RSL_MAKE_TYPE_TABLE(TypeTable, GenObj);
char *GetName(TRslListItemContainer *obj, int id)
{
    TRslListWrapperInteface *intrface = wrp(obj);

    if (id >= 0 && id < intrface->size())
    {
        if (id >= 0 && id < obj->m_Names.size())
            return obj->m_Names[id];
        else
        {
            int itemsToAdd = id - obj->m_Names.size() + 1;

            for (int i = 0; i < itemsToAdd; ++i)
            {
                char* newItem = new char[255];
                qsnprintf(newItem, 255, "Parm %d", obj->m_Names.size() + i);
                obj->m_Names.push_back(newItem);
            }

            return obj->m_Names[id];
        }
    }
    /*if (id >= obj->m_Names.size())
    {
        for (int i = obj->m_Names.size() - 1; i < obj->m_Names.size() + id; i++)
        {
            char *name = new char[255];
            qsnprintf(name, 255, "Item %d", i);
            obj->m_Names.append(name);
        }
    }*/
    return NULL;
}

void DoneListItemContainer(TGenObject *o)
{
    TRslListItemContainer *obj = (TRslListItemContainer *)o;

    qDeleteAll(obj->m_Names.begin(), obj->m_Names.end());
    obj->m_Names.clear();

    delete obj;
}

static unsigned short GenObjRelease(TGenObject *o)
{
    TRslListItemContainer *obj = (TRslListItemContainer *)o;
    unsigned short  refCount = --obj->refCount;

    if (!refCount)
        DoneListItemContainer(o);

    return refCount;
}

static unsigned short GenObjAddRef(TGenObject *o)
{
    TRslListItemContainer *obj = (TRslListItemContainer *)o;

    obj->refCount++;

    return obj->refCount;
}

static int GenObjSetId(TGenObject *obj, long id, VALUE *val)
{
    return 1;
}

static int GenObjSet(TGenObject *obj, const char *parm, VALUE *val, long *id)
{
    int tp = GenObjFindMember((TRslListItemContainer*)obj, parm, id);

    if(tp == 1) // property
        return GenObjSetId(obj, *id, val);

    if(tp == 0)
        return 1;

    return -1;
}

static int GenObjGetId(TGenObject *obj, long id, VALUE *val)
{
    VALUE *par;
    TRslListItemContainer *parent = (TRslListItemContainer *)obj;

    if(id >= 0)
    {
        VALUE v;
        ValueMake(&v);
        QVariant value;

        auto SetterFunc = [&v](int type, void *ptr) -> void
        {
            ValueSet(&v, type, ptr);
        };

        int stat = wrp(parent)->get(id, &value);
        SetValueFromVariant(SetterFunc, value);

        ValueCopy(&v, val);

        return stat;
    }

    return -1;
}

static int GenObjGet(TGenObject *obj, const char *parm, VALUE *val, long *id)
{
    int  tp = GenObjFindMember((TRslListItemContainer*)obj, parm, id);

    if(tp == 1) // property
        return GenObjGetId(obj, *id, val);

    if(tp == 0)
        return 1;

    return -1;
}

static int GenObjRunId(TGenObject *obj, long id)
{
    return -1;
}

static int GenObjRun(TGenObject *obj, const char *method, long *id)
{
    if(GenObjFindMember((TRslListItemContainer*)obj, method, id) == 0)
        return GenObjRunId(obj, *id);

    return -1;
}

static const char *GenObjTypeName(TGenObject *obj)
{
    return "ListContainer";
}

static const char *GenObjTypeNameCS(TGenObject *obj)
{
    return "ListContainer";
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
    long  id;

    if(GenObjFindMember((TRslListItemContainer *)pObj, name, &id) != -1)
        return id;

    return -1;
}

static int GenObjMemberFromID(TGenObject *pObj, long uniqID, long *id)
{
    if(uniqID >= 0)
    {
        *id = uniqID;

        return 1;  // Property
    }

    return -1;
}

static int GenObjCvtToIDisp(TGenObject *pObj)
{
    return 1;
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
    TMemberInfo  *info = (TMemberInfo *)data;
    TRslListItemContainer *parent = (TRslListItemContainer *)obj;
    TRslListWrapperInteface *intrface = wrp(parent);

    if(cmd == GOBJ_GETTYPEINFO)
    {
        TClassInfo  *ptr = (TClassInfo *)data;
        memset(ptr, 0, sizeof(TClassInfo));
        ptr->nVars = ptr->nProps = wrp(parent)->size();

        return 0;
    }
    else if(cmd == GOBJ_GETPROPBYIND || cmd == GOBJ_GETVARBYIND)
    {
        if (info->ind < intrface->size())
        {
            info->id       = info->ind;
            info->declType = V_UNDEF;
            info->nParm    = 0;
            info->parInfo  = NULL;
            info->kind     = RSL_KIND_VAR;
            info->flags    = 0;
            info->name     = GetName(parent, info->ind);

            if (info->name)
            {
                qDebug() << info->name;
                return 0;
            }
        }
    }
    else if (cmd == GOBJ_MEMBERBYID)
    {
        if (info->ind < intrface->size())
        {
            info->declType = V_UNDEF;
            info->nParm    = 0;
            info->parInfo  = NULL;
            info->kind     = RSL_KIND_VAR;
            info->flags    = 0;
            info->name     = GetName(parent, info->ind);

            if (info->name)
                return 0;
        }
    }

    return -1;
}

static RSL_MAKE_OBJ_TABLE(table, GenObj);


TGenObject *MakeListContainer(TRslListWrapper *parent)
{
    TRslListItemContainer *obj = new TRslListItemContainer();

    obj->wrp = parent->interface;
    P_GOBJ(obj)->vtbl = &table;
    ADDREF(obj);

    table.getUniqID         = GenObjGetUniqID;
    table.memberFromID      = GenObjMemberFromID;
    table.canCvtToIDispatch = GenObjCvtToIDisp;

    table.genProc           =  GenObjEnumProps;

    return P_GOBJ(obj);
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
