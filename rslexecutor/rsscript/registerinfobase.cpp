// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "rslexecutor.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "playrep.h"
#include "statvars.h"
#include "typeinfo_p.h"
#include "registerinfobase.h"
#include <memory.h>
#include <QDebug>
#include <QMetaProperty>
#include <QMetaMethod>

extern unsigned short GenObjRelease(TGenObject *o);
extern unsigned short GenObjAddRef(TGenObject *o);

extern uintptr_t GenObjTypeId(TGenObject *obj);
extern int GenObjAttach(TGenObject *o, const char *method, SYMPROC *aProc);
extern int GenObjNprops(void);
extern int GenObjNMethods(void);
extern int GenObjCanInherit(void);
extern int GenObjCvtToIDisp(TGenObject *pObj);
extern int GenObjMemberFromID(TGenObject *pObj, long uniqID, long *id);
extern int GenObjSetId(TGenObject *obj, long id, VALUE *val);
extern int GenObjSet(TGenObject *obj, const char *parm, VALUE *val, long *id);
extern int GenObjGetId(TGenObject *obj, long id, VALUE *val);
extern int GenObjGet(TGenObject *obj, const char *parm, VALUE *val, long *id);
extern int SetObjectGeneration(QObjectRsl *obj, unsigned short gen);
extern int GenObjRunId(TGenObject *obj, long id);
extern int GenObjRun(TGenObject *obj, const char *methodname, long *id);

class RegisterInfoBasePrivate
{
    Q_DECLARE_PUBLIC(RegisterInfoBase)
public:
    RegisterInfoBasePrivate(RegisterInfoBase *obj)
    {
        q_ptr = obj;

        memset(&Table, 0, sizeof(IRslObject));
        Table.size = sizeof(IRslObject);
    }

    int findMember(const char *name, long *id)
    {
        for(int i = 0; i < ArrMethod.size(); i++ )
        {
            if(qstricmp(name, ArrMethod[i].name) == 0)
            {
                *id = ArrMethod[i].id;

                return 0;
            }
        }

        for(int i = 0; i < ArrProp.size(); i++)
        {
            if(qstricmp (name, ArrProp[i].name) == 0)
            {
                *id = ArrProp[i].id;

                return 1;
            }
        }

        *id = -1;

        return -1;
    }

    QObjectRsl *MakeObj(void)
    {
        const QMetaObject &meta = MetaObject;

        QObjectRsl *obj = new QObjectRsl();
                //(QObjectRsl *)_alloca(sizeof(QObjectRsl));
        qstrcpy(obj->className, meta.className());
        P_GOBJ(obj)->vtbl = &Table;

        ADDREF(obj);
        return obj;
    }

    void FillFromMetaObject(const qint32 &flags,
                            const QMetaObject &meta,
                            ObjTypeName ObjTypeNameFunc,
                            ObjFindMember ObjFindMemberFunc)
    {
        TypeName = meta.className();

        Table.AddRef = GenObjAddRef;
        Table.Release = GenObjRelease;
        Table.set = GenObjSet;
        Table.setId = GenObjGetId;
        Table.get = GenObjGet;
        Table.getId = GenObjGetId;
        Table.run = GenObjRun;
        Table.runId = GenObjRunId;
        Table.typeID = GenObjTypeId;
        Table.attach = GenObjAttach;
        Table.typeName = (const char*(*)(TGenObject *))ObjTypeNameFunc;
        Table.typeNameCS = (const char*(*)(TGenObject *))ObjTypeNameFunc;

        int propertyOffset = flags & GenInfoUseParentProps ? 0 : meta.propertyOffset();
        for (int i = propertyOffset; i < meta.propertyCount(); i++)
        {
            QMetaProperty property = meta.property(i);

            TPropMethTable prop = {property.name(), i + 1};
            prop.flags = RSL_KIND_VAR | RSL_KIND_GET;

            if (property.isWritable())
                prop.flags |= RSL_KIND_SET;

            ArrProp.append(prop);
        }

        for (int i = meta.enumeratorOffset(); i < meta.enumeratorCount(); i++)
        {
            QMetaEnum property = meta.enumerator(i);

            for (int j = 0; j < property.keyCount(); j++)
            {
                TPropMethTable prop = {property.key(j), OBJ_RSL_ENUM_OFFSET + j};
                prop.flags = RSL_KIND_RUN;
                EnumValue.insert(OBJ_RSL_ENUM_OFFSET + j, property.value(j));

                ArrProp.append(prop);
            }
        }

        QStringList AddedMethods;
        int methodOffset = flags & GenInfoUseParentProps ? 0 : meta.methodOffset();
        for (int i = methodOffset; i < meta.methodCount(); i++)
        {
            QMetaMethod method = meta.method(i);

            if (method.methodType() == QMetaMethod::Method || method.methodType() == QMetaMethod::Slot)
            {
                if (!AddedMethods.contains(method.name(), Qt::CaseInsensitive))
                {
                    TPropMethTable prop = { method.name(), OBJ_RSL_METHOD_OFFSET + i };
                    prop.flags = RSL_KIND_RUN;
                    ArrMethod.append(prop);
                    AddedMethods.append(method.name());
                }
            }
            /*else if (method.methodType() == QMetaMethod::Signal)
            {
                qDebug() << method.methodSignature();
            }*/
        }

        GenInfo.nMeth = ArrMethod.size();
        GenInfo.nProp = ArrProp.size();

        Props = {GenInfo.nProp, ArrProp.data()};
        Methods = {GenInfo.nMeth, ArrMethod.data()};

        GenInfo.prop = &Props;
        GenInfo.meth = &Methods;

        memset(&TypeTable, 0, sizeof(IRslTypeInfo));
        TypeTable.size = sizeof(IRslTypeInfo);
        TypeTable.isProp = ObjFindMemberFunc;
        TypeTable.getNProps = GenObjNprops;
        TypeTable.getNMethods = GenObjNMethods;
        TypeTable.canInherit = GenObjCanInherit;

        MetaObject = meta;
    }

    int GenTypeCalcEx(TGenInfo *p, TClassInfo *ptr)
    {
        ptr->nVars  += ArrProp.size();
        ptr->nProps += ArrProp.size();

        ptr->nFunc  += ArrMethod.size();
        ptr->nMeth  += ArrMethod.size();

        return 0;
    }

    int GenTypeGetPropEx(int ind, TGenInfo *p, TMemberInfo *info)
    {
        if (info->ind >= ArrProp.size())
            return -1;

        info->name     = ArrProp[info->ind].name;
        info->name_origin = ArrProp[info->ind].name;
        info->id       = ArrProp[info->ind].id;
        info->nParm    = 0;
        info->parInfo  = NULL;
        info->flags    = ArrProp[info->ind].flags;
        info->kind     = RSL_KIND_VAR;

        return 0;
    }

    int GetTypeInfoProc(TGenInfo *p, int cmd, void *data)
    {
        TMemberInfo  *info = (TMemberInfo *)data;

        if(cmd == GOBJ_GETTYPEINFO)
            return GenTypeCalcEx(p, (TClassInfo *)data);
        else if(cmd == GOBJ_GETPROPBYIND)
            return GenTypeGetPropEx(info->ind, p, info);
        else if(cmd == GOBJ_GETFUNCBYIND)
            return GenTypeGetFuncEx(info->ind, p, info);
        else if(cmd == GOBJ_MEMBERBYID)
            return GenTypeGetMemberEx(info->id, p, info);

        return -1;
    }


    int InitProvider(CLNT_PRVD_HANDLE *clntId)
    {
        *clntId = NULL;
        Table.getUniqID         = (long(*)(TGenObject*, const char*))_ObjGetUniqIDFunc;
        Table.memberFromID      = GenObjMemberFromID;
        Table.canCvtToIDispatch = GenObjCvtToIDisp;
        Table.genProc = (int(*)(TGenObject*,int,void *))_ObjEnumProps;

        return 0;
    }

    RegisterInfoBase *q_ptr;

    QString TypeName;

    IRslObject Table;
    IRslTypeInfo TypeTable;

    TGenInfo GenInfo;
    QVector<TPropMethTable> ArrProp;
    QVector<TPropMethTable> ArrMethod;
    QMap<int, int> EnumValue;

    TDescPropMeth Props;
    TDescPropMeth Methods;

    // InitProvider
    ObjGetUniqID _ObjGetUniqIDFunc;
    ObjEnumProps _ObjEnumProps;

    ObjInitProvider _ObjInitProvider;
    ObjDoneProvider _ObjDoneProvider;
    ObjCreateObject _ObjCreateObject;
    ObjClassProc _ObjClassProc;
    ObjConstructorProc _ObjConstructorProc;

    QMetaObject MetaObject;
};

RegisterInfoBase::RegisterInfoBase() :
    d_ptr(new RegisterInfoBasePrivate(this))
{

}

RegisterInfoBase::~RegisterInfoBase()
{

}

void RegisterInfoBase::FillFromMetaObject(const qint32 &flags,
                                          const QMetaObject &meta,
                                          ObjTypeName ObjTypeNameFunc,
                                          ObjFindMember ObjFindMemberFunc)
{
    Q_D(RegisterInfoBase);
    d->FillFromMetaObject(flags, meta, ObjTypeNameFunc, ObjFindMemberFunc);
}

void RegisterInfoBase::SetFunctions(ObjGetUniqID ObjGetUniqIDFunc, ObjEnumProps ObjEnumProps)
{
    Q_D(RegisterInfoBase);
    d->_ObjGetUniqIDFunc = ObjGetUniqIDFunc;
    d->_ObjEnumProps = ObjEnumProps;
}

void RegisterInfoBase::AddObjectFuncs(ObjInitProvider _InitProvider,
                                      ObjDoneProvider _DoneProvider,
                                      ObjCreateObject _CreateObject,
                                      ObjClassProc _ClassProc,
                                      ObjConstructorProc _ObjConstructorProc)
{
    Q_D(RegisterInfoBase);
    d->_ObjInitProvider = _InitProvider;
    d->_ObjDoneProvider = _DoneProvider;
    d->_ObjCreateObject = _CreateObject;
    d->_ObjClassProc = _ClassProc;
    d->_ObjConstructorProc = _ObjConstructorProc;
}

void RegisterInfoBase::Create(void **GenObject, QObject *cls, const QObjectRslOwner &owner)
{
    Q_D(RegisterInfoBase);
    QObjectRsl *obj = d->MakeObj();

    if (cls)
    {
        obj->object = cls;
        obj->owner = owner;
    }
    else
    {
        int NumParam = GetNumParm();
        const QMetaObject &meta = d->MetaObject;

        int id = FindMethod(&meta, meta.className(), NumParam, true);

        if (id >= 0)
        {
            QMetaMethod method = meta.constructor(id);
            obj->object = (QObject*)CallMethod(&meta, method, QMetaObject::CreateInstance, id, nullptr);
        }
        else
            ThrowParamTypeError(1);
    }

    *GenObject = P_GOBJ(obj);
}

int RegisterInfoBase::findMember(const char *name, long *id)
{
    Q_D(RegisterInfoBase);
    return d->findMember(name, id);
}

void *RegisterInfoBase::GetTypeInfo(void *clntId, const char *typeName)
{
    Q_D(RegisterInfoBase);
    const QMetaObject &meta = d->MetaObject;

    if(qstricmp(typeName, meta.className()) == 0)
        return &d->TypeTable;

    return NULL;
}

int RegisterInfoBase::EnumProps(void *obj, int cmd, void *data)
{
    Q_D(RegisterInfoBase);

    if(cmd == GOBJ_SETGENERATION)
        return SetObjectGeneration((QObjectRsl*)obj, *((unsigned short *)data));

    if(cmd == GOBJ_MARK_OBJECT)
    {
        int old = ((QObjectRsl*)obj)->marknumber;
        ((QObjectRsl*)obj)->marknumber = *((unsigned short *)data);
        return old;
    }

    return d->GetTypeInfoProc(&d->GenInfo, cmd, data);
}

void RegisterInfoBase::DoneProvider(void *clntId)
{

}

int RegisterInfoBase::InitProvider(void *clntId)
{
    Q_D(RegisterInfoBase);
    return d->InitProvider((CLNT_PRVD_HANDLE*)clntId);
}

void RegisterInfoBase::CallCansrtuctor()
{
    TGenObject *obj = 0;
    Create((void**)&obj, nullptr);

    //InitObj(obj, 0);

    ReturnVal(V_GENOBJ, P_GOBJ(obj));
}

Qt::HANDLE RegisterInfoBase::rslID() const
{
    Q_D(const RegisterInfoBase);
    return (Qt::HANDLE)&d->Table;
}

Qt::HANDLE RegisterInfoBase::object(Qt::HANDLE GenObject)
{
    TGenObject *obj = (TGenObject*)GenObject;
    if (rslID() != (Qt::HANDLE)RSCLSID(obj))
        return nullptr;

    return ((QObjectRsl*)obj)->object;
}

int RegisterInfoBase::enumValue(int id) const
{
    Q_D(const RegisterInfoBase);
    return d->EnumValue[id];
}

void RegisterInfoBase::importObject(const bool &canCreate)
{
    Q_D(RegisterInfoBase);

    AddObjectProviderModEx(
                (TInitRslProvider)d->_ObjInitProvider,
                (TDoneRslProvider)d->_ObjDoneProvider,
                (TCreateRslObject)d->_ObjCreateObject,
                (TRslTypeProc)d->_ObjClassProc);

    if (canCreate)
        AddStdProc(V_GENOBJ, d->MetaObject.className(), d->_ObjConstructorProc, 0);
}

void ConsrtuctorCaller(RegisterInfoBase *info)
{
    info->CallCansrtuctor();
}

int EnumPropsCaller(RegisterInfoBase *info, void *obj, int cmd, void *data)
{
    return info->EnumProps(obj, cmd, data);
}

int InitProviderCaller(RegisterInfoBase *info, void *clntId)
{
    return info->InitProvider(clntId);
}

void DoneProviderCaller(RegisterInfoBase *info, void *clntId)
{
    info->DoneProvider(clntId);
}

void *GetTypeInfoCaller(RegisterInfoBase *info, void *clntId, const char *typeName)
{
    return info->GetTypeInfo(clntId, typeName);
}
