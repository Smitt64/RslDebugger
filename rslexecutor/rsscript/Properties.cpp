// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "rsl/dlmintf.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "typeinfo_p.h"
#include "registerobjlist.hpp"
#include "rslexecutor.h"
#include "rslibdynamicfuncs.h"
#include "rslobjconnections.h"
#include <cstring>
#include <QDebug>
#include <QTextCodec>
#include <QDate>
#include <QTime>
#include <QMetaProperty>
#include <QLibrary>
#include <QMetaType>
#include <QRect>

Q_GLOBAL_STATIC_WITH_ARGS(QLibrary, RSScriptLib, ("RSScript"))

LibRslTArrayCreate _LibRslTArrayCreate = nullptr;
LibRslTArrayPut _LibRslTArrayPut = nullptr;
LibRslIsTArray _LibRslIsTArray = nullptr;
LibRslTArraySize _LibRslTArraySize = nullptr;
LibRslTArrayGet _LibRslTArrayGet = nullptr;
LibSetParm _LibSetParm = nullptr;
LibRslObjInvoke _LibRslObjInvoke = nullptr;
LibRslAddUniClass _LibRslAddUniClass = nullptr;
LibRslFindUniClass _LibRslFindUniClass = nullptr;
LibRslGetUniClass _LibRslGetUniClass = nullptr;
LibRslUniCast _LibRslUniCast = nullptr;
LibCreateObjectOfClassEx _LibCreateObjectOfClassEx = nullptr;

void LoadFunctions()
{
    if (_LibRslTArrayCreate)
        return;

    _LibRslTArrayCreate = (LibRslTArrayCreate)RSScriptLib->resolve("RslTArrayCreate");
    _LibRslTArrayPut = (LibRslTArrayPut)RSScriptLib->resolve("RslTArrayPut");
    _LibRslIsTArray = (LibRslIsTArray)RSScriptLib->resolve("RslIsTArray");
    _LibRslTArraySize = (LibRslTArraySize)RSScriptLib->resolve("RslTArraySize");
    _LibRslTArrayGet = (LibRslTArrayGet)RSScriptLib->resolve("RslTArrayGet");
    _LibSetParm = (LibSetParm)RSScriptLib->resolve("SetParm");
    _LibRslObjInvoke = (LibRslObjInvoke)RSScriptLib->resolve("RslObjInvoke");
    _LibRslAddUniClass = (LibRslAddUniClass)RSScriptLib->resolve("RslAddUniClass");
    _LibRslFindUniClass = (LibRslFindUniClass)RSScriptLib->resolve("RslFindUniClass");
    _LibRslGetUniClass = (LibRslGetUniClass)RSScriptLib->resolve("RslGetUniClass");
    _LibRslUniCast = (LibRslUniCast)RSScriptLib->resolve("RslUniCast");
    _LibCreateObjectOfClassEx = (LibCreateObjectOfClassEx)RSScriptLib->resolve("CreateObjectOfClassEx");
}

int GetFuncParamType(const int &id)
{
    VALUE *val;
    GetParm(id, &val);

    int result = QVariant::Invalid;

    switch(val->v_type)
    {
    case V_STRING:
        result = QVariant::String;
        break;

    case V_INTEGER:
        result = QVariant::Int;
        break;

    case V_BIGINT:
        result = QVariant::LongLong;
        break;

    case V_BOOL:
        result = QVariant::Bool;
        break;

    case V_DATE:
        result = QVariant::Date;
    break;

    case V_TIME:
        result = QVariant::Time;
    break;

    case V_R2M:
        result = QVariantR2M;
        break;

    case V_GENOBJ:
    {
        TGenObject *obj = P_GOBJ(val->value.obj);
        TGenObject *TArrayText = (TGenObject*)_LibRslIsTArray(P_GOBJ(val->value.obj));

        if (TArrayText)
        {
            bool isStringList = true;

            int size = _LibRslTArraySize(TArrayText);
            for (int i = 0; i < size; i++)
            {
                VALUE *item = (VALUE*)_LibRslTArrayGet(TArrayText, i);
                if (item->v_type != V_STRING)
                {
                    isStringList = false;
                    break;
                }
            }

            if (isStringList)
                result = QVariant::StringList;
            else
                result = QMetaType::QVariantList;
        }
        else
        {
            if (IsRectRsl(obj))
                result = QVariant::Rect;
            else if (IsSizeRsl(obj))
                result = QVariant::Size;
            else if (IsPointRsl(obj))
                result = QVariant::Point;
            else if (IsByteArrayRsl(obj))
                result = QVariant::ByteArray;
            else
                result = QVariant::UserType;
        }
    }
        break;
    }

    return result;
}

QVariant SetFromRslValue(void *value, bool isStringListProp)
{
    QVariant result;
    VALUE *val = (VALUE*)value;
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    LoadFunctions();

    switch(val->v_type)
    {
    case V_STRING:
        result = QVariant::fromValue(codec->toUnicode(val->value.string));
        break;

    case V_INTEGER:
        result = QVariant::fromValue(val->value.intval);
        break;

    case V_BIGINT:
        result = QVariant::fromValue(val->value.bigint);
        break;

    case V_BOOL:
        result = QVariant::fromValue(val->value.boolval);
        break;

    case V_DATE:
    {
        bdate dt = val->value.date;
        result = QVariant::fromValue(QDate(dt.year, dt.mon, dt.day));
    }
        break;

    case V_TIME:
    {
        btime dt = val->value.time;
        result = QVariant::fromValue(QTime(dt.hour, dt.min, dt.sec));
    }
        break;

    case V_R2M:
    {
        R2M r2m;
        r2m.obj = val->value.r2m.obj;
        r2m.id = val->value.r2m.id;
        result = QVariant::fromValue(r2m);
    }
        break;

    case V_GENOBJ:
    {
        TGenObject *TArrayText = (TGenObject*)_LibRslIsTArray(P_GOBJ(val->value.obj));

        if (TArrayText)
        {
            if (isStringListProp)
            {
                QStringList lst;

                int size = _LibRslTArraySize(TArrayText);
                for (int i = 0; i < size; i++)
                {
                    VALUE *v = (VALUE*)_LibRslTArrayGet(TArrayText,i);
                    QVariant vl = SetFromRslValue(v);
                    lst.append(vl.toString());
                }

                result = QVariant::fromValue(lst);
            }
        }
        else
        {
            Qt::HANDLE handle = (Qt::HANDLE)RSCLSID(val->value.obj);
            RegisterInfoBase *info = RegisterObjList::inst()->info(handle);

            if (info)
                result = QVariant::fromValue<QObject*>((QObject*)info->object(val->value.obj));
            else
            {
                if (IsRectRsl(val->value.obj))
                    result = GetRectRsl(val->value.obj);
                else if (IsSizeRsl(val->value.obj))
                    result = GetSizeRsl(val->value.obj);
                else if (IsPointRsl(val->value.obj))
                    result = GetPointRsl(val->value.obj);
                else if (IsByteArrayRsl(val->value.obj))
                    result = GetByteArrayRsl(val->value.obj);
            }
        }
    }
        break;
    }

    return result;
}

int GenObjSetId(TGenObject *obj, long id, VALUE *val)
{
    QObjectRsl *rsl = (QObjectRsl*)obj;
    const QMetaObject *meta = rsl->object->metaObject();
    QMetaProperty property = meta->property(id - 1);
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    const char *prop = property.name();

    if (!property.isWritable())
        return 1;

    bool isStringList = false;
    if (property.type() == QVariant::StringList)
        isStringList = true;

    QVariant var = SetFromRslValue(val, isStringList);
    if (!var.isValid())
        return -1;

    rsl->object->setProperty(prop, var);

    return 0;
}

int GenObjSet(TGenObject *obj, const char *parm, VALUE *val, long *id)
{
    QObjectRsl *rsl = (QObjectRsl*)obj;
    const QMetaObject *meta = rsl->object->metaObject();

    *id = 0;
    bool found = false;
    for (int i = 0; i < meta->propertyCount(); i++)
    {
        QMetaProperty property = meta->property(i);

        if (!QString(property.name()).compare(parm, Qt::CaseInsensitive))
        {
            found = true;
            *id = i + 1;
            break;
        }
    }

    if(found) // property
        return GenObjSetId(obj, *id, val);

    return -1;
}

#define CHECK_TYPE(NeedType) ((value->v_type == NeedType) || (value->v_type == V_UNDEF && isOutParam))
bool CompareTypes(const int &MetaType, void *val, bool isOutParam)
{
    bool result = false;

    VALUE *value = (VALUE*)val;
    switch(MetaType)
    {
    case QMetaType::QObjectStar:
        if (CHECK_TYPE(V_GENOBJ))
        {
            RegisterInfoBase *info = RegisterObjList::inst()->info((Qt::HANDLE)RSCLSID(value->value.obj));

            if (info)
                result = true;
        }
        break;
    case QMetaType::QVariant:
        if (CHECK_TYPE(V_STRING) || CHECK_TYPE(V_INTEGER) || CHECK_TYPE(V_BIGINT) ||
            CHECK_TYPE(V_DOUBLE) || CHECK_TYPE(V_DATE) || CHECK_TYPE(V_TIME) || CHECK_TYPE(V_BOOL))
        {
            result = true;
        }
        else
        {
            if (((VALUE*)val)->v_type == V_GENOBJ)
            {
                if (_LibRslIsTArray(P_GOBJ(((VALUE*)val)->value.obj)) != 0)
                    result = true;
            }
        }
        break;
    case QVariant::String:
        result = CHECK_TYPE(V_STRING);
        break;
    case QVariant::Int:
    case QVariant::UInt:
    case QMetaType::Short:
        result = CHECK_TYPE(V_INTEGER);
        break;
    case QVariant::LongLong:
    case QVariant::ULongLong:
        if (CHECK_TYPE(V_BIGINT) || CHECK_TYPE(V_INTEGER))
            result = true;
        break;
    case QVariant::Double:
        result = CHECK_TYPE(V_DOUBLE);
        break;
    case QVariant::Date:
        result = CHECK_TYPE(V_DATE);
        break;
    case QVariant::Time:
        result = CHECK_TYPE(V_TIME);
        break;

    case QVariant::Bool:
        result = CHECK_TYPE(V_BOOL);
        break;

    case QMetaType::QStringList:
    case QMetaType::QVariantList:
        if (((VALUE*)val)->v_type == V_GENOBJ)
        {
            if (_LibRslIsTArray(P_GOBJ(((VALUE*)val)->value.obj)) != 0)
                result = true;
        }
        break;

    case QMetaType::QRect:
        if (((VALUE*)val)->v_type == V_GENOBJ)
        {
            if (IsRectRsl(P_GOBJ(((VALUE*)val)->value.obj)))
                result = true;
        }
        break;

    case QMetaType::QSize:
        if (((VALUE*)val)->v_type == V_GENOBJ)
        {
            if (IsSizeRsl(P_GOBJ(((VALUE*)val)->value.obj)))
                result = true;
        }
        break;

    case QMetaType::QPoint:
        if (((VALUE*)val)->v_type == V_GENOBJ)
        {
            if (IsPointRsl(P_GOBJ(((VALUE*)val)->value.obj)))
                result = true;
        }
        break;

    case QMetaType::QByteArray:
        if (((VALUE*)val)->v_type == V_GENOBJ)
        {
            if (IsByteArrayRsl(P_GOBJ(((VALUE*)val)->value.obj)))
                result = true;
        }
        break;

    default:
        result = false;
    }
    return result;
}

int SetValueFromVariant(std::function<void(int,void*)> Setter, const QVariant &value)
{
    int result = 0;
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    LoadFunctions();

    int type = value.type();
    switch(type)
    {
    case QVariant::String:
        Setter(V_STRING, codec->fromUnicode(value.toString()).data());
        break;
    case QVariant::Int:
    case QVariant::UInt:
    case QMetaType::Short:
    case QMetaType::UShort:
    {
        long v = value.toInt();
        Setter(V_INTEGER, &v);
    }
        break;
    case QVariant::LongLong:
    case QVariant::ULongLong:
    {
        qint64 v = value.value<qint64>();
        Setter(V_BIGINT, &v);
    }
        break;

    case QVariant::Double:
    {
        qreal v = value.value<qreal>();
        Setter(V_DOUBLE, &v);
    }
        break;

    case QVariant::Date:
    {
        QDate dt = value.toDate();
        bdate rsldt;
        rsldt.day = dt.day();
        rsldt.mon = dt.month();
        rsldt.year = dt.year();
        Setter(V_DATE, &rsldt);
    }
        break;

    case QVariant::Time:
    {
        QTime dt = value.toTime();
        qDebug() << dt;
        btime rsldt;
        rsldt.hour = dt.hour();
        rsldt.min = dt.minute();
        rsldt.sec = dt.second();
        Setter(V_TIME, &rsldt);
    }
        break;

    case QVariant::Bool:
    {
        bool b = value.toBool();
        Setter(V_BOOL, &b);
    }
        break;

    case QVariant::StringList:
    {
        QStringList lst = value.toStringList();
        TGenObject *ValueArray = (TGenObject*)_LibRslTArrayCreate(lst.size(), lst.size());

        int i = 0;
        for (const QString &item : lst)
        {
            VALUE val;
            ValueSet(&val, V_STRING, codec->fromUnicode(item).data());
            _LibRslTArrayPut(ValueArray, i++, &val);
            ValueClear(&val);
        }
        Setter(V_GENOBJ, ValueArray);
    }
        break;

    case QMetaType::QVariantList:
    {
        QVariantList lst = value.toList();
        TGenObject *ValueArray = (TGenObject*)_LibRslTArrayCreate(lst.size(), lst.size());

        int i = 0;
        for (const QVariant &item : lst)
        {
            VALUE val;
            SetValueFromVariant(std::bind(StdValueSetFunc, &val,
                                          std::placeholders::_1,
                                          std::placeholders::_2),
                                item);

            _LibRslTArrayPut(ValueArray, i++, &val);
            ValueClear(&val);
        }
        Setter(V_GENOBJ, ValueArray);
    }
        break;

    case QMetaType::QRect:
    {
        TGenObject *obj = (TGenObject*)CreateRectRsl(value.toRect());
        Setter(V_GENOBJ, obj);
    }
        break;

    case QMetaType::QSize:
    {
        TGenObject *obj = (TGenObject*)CreateSizeRsl(value.toSize());
        Setter(V_GENOBJ, obj);
    }
    break;

    case QMetaType::QPoint:
    {
        TGenObject *obj = (TGenObject*)CreatePointRsl(value.toPoint());
        Setter(V_GENOBJ, obj);
    }
    break;

    case QMetaType::QByteArray:
    {
        TGenObject *obj = (TGenObject*)CreateByteArrayRsl(value.toByteArray());
        Setter(V_GENOBJ, obj);
    }
        break;

    case QMetaType::QObjectStar:
    {
        QObject *obj = value.value<QObject*>();
        const QMetaObject *meta = nullptr;

        if (obj)
            meta = obj->metaObject();

        if (meta && obj)
        {
            RegisterInfoBase *info = RegisterObjList::inst()->info(meta->className());

            if (info)
            {
                RegisterInfoBase::QObjectRslOwner owner = RegisterInfoBase::CppOwner;
                QVariant owner_prop = obj->property(OBJECT_PROP_OWNER);

                if (owner_prop.isValid())
                    owner = (RegisterInfoBase::QObjectRslOwner)owner_prop.toInt();

                TGenObject *Child = nullptr;
                info->Create((void**)&Child, obj, owner);
                Setter(V_GENOBJ, P_GOBJ(Child));
            }
        }
    }
        break;

    default:
        //qDebug() << value.type() << (int)value.type() << value.userType();
        result =  -1;
    }

    return result;
}

void StdValueSetFunc(void *val, int type, void *ptr)
{
    ValueSet((VALUE*)val, type, ptr);
}

#define ValueVariantSet(type, ptr) SetValueFromVariant(SetterFunc, type, ptr)

int GenObjGetId(TGenObject *obj, long id, VALUE *val)
{
    QObjectRsl *rsl = (QObjectRsl*)obj;

    const QMetaObject *meta = rsl->object->metaObject();
    RegisterInfoBase *info = findInfo(meta->className());

    if (id < OBJ_RSL_ENUM_OFFSET)
    {
        QMetaProperty property = meta->property(id - 1);

        if (!property.isReadable())
            return 1;

        QVariant value = rsl->object->property(property.name());
        auto SetterFunc = [=](int type, void *ptr) -> void
        {
            ValueSet(val, type, ptr);
        };

        return SetValueFromVariant(SetterFunc, value);
    }
    else
    {
        int enumval = info->enumValue(id);
        ValueSet(val, V_INTEGER, &enumval);

        return 0;
    }

    return -1;
}

int GenObjGet(TGenObject *obj, const char *parm, VALUE *val, long *id)
{
    QObjectRsl *rsl = (QObjectRsl*)obj;
    const QMetaObject *meta = rsl->object->metaObject();
    RegisterInfoBase *info = RegisterObjList::inst()->info(meta->className());

    int  tp = info->findMember(parm, id);

    if(tp == 1) // property
        return GenObjGetId(obj, *id, val);

    if(tp == 0)
        return 1;

    return -1;
}
