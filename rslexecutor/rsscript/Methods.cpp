// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "qvariant.h"
#include "rsl/dlmintf.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "typeinfo_p.h"
#include "registerobjlist.hpp"
#include "rslexecutor.h"
#include "rslibdynamicfuncs.h"
#include "rslexecutor.h"
#include <cstring>
#include <QDebug>
#include <QTextCodec>
#include <QDate>
#include <QTime>
#include <QMetaProperty>
#include <QLibrary>
#include <QMetaType>
#include <QRect>

static bool CompareParamTypes(const QMetaMethod &method, int offset)
{
    bool result = true;

    LoadFunctions();
    QList<QByteArray> paramsTypes = method.parameterTypes();
    for (int i = 0; i < method.parameterCount(); i++)
    {
        int type = method.parameterType(i);
        VALUE *val = nullptr;
        GetParm(i + offset, &val);

        bool isOutParam = false;
        if (paramsTypes[i].contains("&"))
           isOutParam = true;

        QString normalized = QString(paramsTypes[i]).remove("&").remove("*");

        if (!type) 
            type = QMetaType::type(normalized.toLocal8Bit().data());

        if (type)
        {
            if (!CompareTypes(type, (void*)val, isOutParam))
                result = false;
        }
        else
        {
            if (val->v_type != V_GENOBJ)
                result = false;
            else
            {
                RegisterInfoBase *info = RegisterObjList::inst()->info(normalized);

                if (!info)
                    result = false;
                else
                {
                    if (info->rslID() != (Qt::HANDLE)RSCLSID(val->value.obj))
                        result = false;
                }
            }
        }
    }

    return result;
}

int FindMethod(const QMetaObject *meta, const QString &name, int NumParams, bool NeedConstructor)
{
    int id = -1;

    int size = NeedConstructor ? meta->constructorCount() : meta->methodCount();
    for (int i = 0; i < size; i++)
    {
        QMetaMethod tmpmethod;

        if (!NeedConstructor)
            tmpmethod = meta->method(i);
        else
            tmpmethod = meta->constructor(i);

        if (!name.compare(tmpmethod.name()))
        {
            int metaParameterCount = tmpmethod.parameterCount();
            if (metaParameterCount == NumParams)
            {
                if (NumParams == 0)
                {
                    id = i;
                    break;
                }
                else
                {
                    // compare params types
                    if (CompareParamTypes(tmpmethod, NeedConstructor ? 0 : 1))
                    {
                        id = i;
                        break;
                    }
                }
            }
        }
    }

    return id;
}

void *CallMethod(const QMetaObject *meta,
                 const QMetaMethod &method,
                 const QMetaObject::Call &type,
                 const long &id,
                 QObject *Instance)
{
    void *result = nullptr;
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");

    int parmaOffset = 0;
    if (method.returnType() != QMetaType::Void)
        parmaOffset = 1;

    int MethodParams = method.parameterCount();
    QList<QByteArray> paramsTypes = method.parameterTypes();

    auto SetterFunc = [=](int id, int type, void *ptr) -> void
    {
        _LibSetParm(id, type, ptr);
    };


    auto AllocaTeParam = [=](const int &Type, void **param, VALUE *val, const  QString &normalized, bool isPtr = false) -> void
    {
        RegisterInfoBase *info = RegisterObjList::inst()->info(normalized);

        if (Type)
            *param = QMetaType::create(Type);
        else
        {
            if (!info)
                iError(IER_RUNTIME, "Unknown param type");
            else
            {
                *param = new void*[1];
            }
        }

        if (!val)
            return;

        VALUE NewVal;
        ValueMake(&NewVal);
        ValueCopy(val, &NewVal);

        //qDebug() << "AllocaTeParam" << Type;
        if (Type == QMetaType::QString)
        {
            if (!CnvType(&NewVal, V_STRING))
                iError(IER_RUNTIME, "Param type missmatch, required string");

            (*reinterpret_cast<QString*>(*param)) = codec->toUnicode(NewVal.value.string);
        }
        else if (Type == QMetaType::Bool)
        {
            if (!CnvType(&NewVal, V_BOOL))
                iError(IER_RUNTIME, "Param type missmatch, required boolean");

            (*reinterpret_cast<bool*>(*param)) = NewVal.value.boolval;
        }
        else if (Type == QMetaType::Int ||
                 Type == QMetaType::UInt ||
                 Type == QMetaType::Long ||
                 Type == QMetaType::ULong
        )
        {
            if (!CnvType(&NewVal, V_INTEGER))
                iError(IER_RUNTIME, "Param type missmatch, required integer");

            (*reinterpret_cast<int*>(*param)) = NewVal.value.intval;
        }
        else if (Type == QMetaType::Short ||
                 Type == QMetaType::UShort
        )
        {
            if (!CnvType(&NewVal, V_INTEGER))
                iError(IER_RUNTIME, "Param type missmatch, required integer");

            (*reinterpret_cast<short*>(*param)) = (short)NewVal.value.intval;
        }
        else if (Type == QMetaType::LongLong || Type == QMetaType::ULongLong)
        {
            if (!CnvType(&NewVal, V_BIGINT) && !CnvType(&NewVal, V_INTEGER))
                iError(IER_RUNTIME, "Param type missmatch, required integer");

            if (NewVal.v_type == V_BIGINT)
                (*reinterpret_cast<qint64*>(*param)) = NewVal.value.bigint;

            if (NewVal.v_type == V_INTEGER)
                (*reinterpret_cast<qint64*>(*param)) = NewVal.value.intval;
        }
        else if (Type == QMetaType::Double)
        {
            if (!CnvType(&NewVal, V_DOUBLE))
                iError(IER_RUNTIME, "Param type missmatch, required double");

            (*reinterpret_cast<qreal*>(*param)) = NewVal.value.doubval;
        }
        else if (Type == QMetaType::QDate)
        {
            if (!CnvType(&NewVal, V_DATE))
                iError(IER_RUNTIME, "Param type missmatch, required date");

            (*reinterpret_cast<QDate*>(*param)) = QDate(NewVal.value.date.year,
                                                        NewVal.value.date.mon,
                                                        NewVal.value.date.day);
        }
        else if (Type == QMetaType::QTime)
        {
            if (!CnvType(&NewVal, V_TIME))
                iError(IER_RUNTIME, "Param type missmatch, required time");

            (*reinterpret_cast<QTime*>(*param)) = QTime(NewVal.value.time.hour,
                                                        NewVal.value.time.min,
                                                        NewVal.value.time.sec);
        }
        else if (Type == QMetaType::QVariant)
        {
            (*reinterpret_cast<QVariant*>(*param)) = SetFromRslValue(&NewVal);
        }
        else if (Type == QMetaType::QRect)
        {
            (*reinterpret_cast<QRect*>(*param)) = SetFromRslValue(&NewVal).toRect();
        }
        else if (Type == QMetaType::QByteArray)
        {
            (*reinterpret_cast<QByteArray*>(*param)) = SetFromRslValue(&NewVal).toByteArray();
        }
        else if (Type == QMetaType::QVariantList)
        {
            if (!CnvType(&NewVal, V_GENOBJ))
                iError(IER_RUNTIME, "Param type missmatch, required TArray");

            TGenObject *TArrayText = (TGenObject*)_LibRslIsTArray(P_GOBJ(NewVal.value.obj));

            if (!TArrayText)
                iError(IER_RUNTIME, "Param type missmatch, required TArray");

            (*reinterpret_cast<QVariantList*>(*param)) = QVariantList();

            int size = _LibRslTArraySize(TArrayText);
            for (int i = 0; i < size; i++)
            {
                VALUE *item = (VALUE*)_LibRslTArrayGet(TArrayText, i);

                QVariant var = SetFromRslValue(item);
                (*reinterpret_cast<QVariantList*>(*param)).append(var);
            }
        }
        else if (Type == QMetaType::QStringList)
        {
            if (!CnvType(&NewVal, V_GENOBJ))
                iError(IER_RUNTIME, "Param type missmatch, required TArray");

            TGenObject *TArrayText = (TGenObject*)_LibRslIsTArray(P_GOBJ(NewVal.value.obj));

            if (!TArrayText)
                iError(IER_RUNTIME, "Param type missmatch, required TArray");

            (*reinterpret_cast<QStringList*>(*param)) = QStringList();

            int size = _LibRslTArraySize(TArrayText);
            for (int i = 0; i < size; i++)
            {
                VALUE *item = (VALUE*)_LibRslTArrayGet(TArrayText, i);

                QVariant var = SetFromRslValue(item);

                if (var.canConvert(QMetaType::QString))
                    (*reinterpret_cast<QStringList*>(*param)).append(var.toString());
            }
        }
        else if (Type == QMetaType::QObjectStar)
        {
            if (!CnvType(&NewVal, V_GENOBJ))
                iError(IER_RUNTIME, "Param type missmatch, required QObject");

            RegisterInfoBase *info = RegisterObjList::inst()->info((Qt::HANDLE)RSCLSID(val->value.obj));

            if (info)
                *((void**)param[0]) = info->object(val->value.obj);
        }
        else if (!Type)
        {
            if (info->rslID() != (Qt::HANDLE)RSCLSID(val->value.obj))
                iError(IER_RUNTIME, "Unknown param type");
            else
            {
                *((void**)param[0]) = info->object(val->value.obj);
            }
        }
    };

    void **params = new void*[MethodParams + 1];

    if (type != QMetaObject::CreateInstance)
    {
        QString normalized = QString(method.typeName()).remove("&").remove("*");
        AllocaTeParam(method.returnType(), &params[0], nullptr, normalized);
    }
    else
        params[0] = new void*[1];

    int offset = type == QMetaObject::CreateInstance ? 0 : 1;
    for (int i = 0; i < MethodParams; i++)
    {
        VALUE *val = nullptr;
        GetParm(i + offset, &val);

        int RealType = method.parameterType(i);
        QString TypeStr = QString(paramsTypes[i]);
        QString normalized = TypeStr.remove("&").remove("*");

        if (!RealType)
            RealType = QMetaType::type(normalized.toLocal8Bit().data());

        AllocaTeParam(RealType, &params[i + 1], val, normalized);
    }

    if (type == QMetaObject::CreateInstance)
        meta->static_metacall(type, id, params);
    else
        meta->metacall(Instance, type, id, params);

    int returnType = method.returnType();
    if (type == QMetaObject::CreateInstance)
        result = *((void**)params[0]);
    else
    {
        if (returnType != QMetaType::Void)
            result = *((void**)params[0]);
    }

    for (int i = 0; i < MethodParams; i++)
    {
        int RealType = method.parameterType(i);

        if (!RealType)
        {
            QString normalized = QString(paramsTypes[i]).remove("&").remove("*");
            RealType = QMetaType::type(normalized.toLocal8Bit().data());

            if (QString(paramsTypes[i]).contains("&"))
            {
                QVariant var = QVariant(RealType, params[i + 1]);
                SetValueFromVariant(std::bind(SetterFunc, i + 1,
                                              std::placeholders::_1,
                                              std::placeholders::_2),
                                    var);
            }
        }
    }

    if (returnType != QMetaType::Void && type != QMetaObject::CreateInstance)
    {
        VALUE ret;
        ValueMake(&ret);
        switch(returnType)
        {
        case QMetaType::QVariant:
            SetValueFromVariant(std::bind(StdValueSetFunc, &ret,
                                          std::placeholders::_1,
                                          std::placeholders::_2),
                                *reinterpret_cast<QVariant*>(params[0]));
            break;
        case QMetaType::Bool:
            ValueSet(&ret, V_BOOL, reinterpret_cast<bool*>(params[0]));
            break;
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Void:
        case QMetaType::Long:
        case QMetaType::ULong:
            ValueSet(&ret, V_INTEGER, reinterpret_cast<int*>(params[0]));
            break;
        case QMetaType::Short:
        case QMetaType::UShort:
        {
            int v = *reinterpret_cast<short*>(params[0]);
            ValueSet(&ret, V_INTEGER, &v);
        }
            break;
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            ValueSet(&ret, V_BIGINT, reinterpret_cast<int*>(params[0]));
            break;
        case QMetaType::Double:
            ValueSet(&ret, V_DOUBLE, reinterpret_cast<int*>(params[0]));
            break;
        case QMetaType::QString:
            ValueSet(&ret, V_STRING, codec->fromUnicode(*reinterpret_cast<QString*>(params[0])));
            break;
        case QMetaType::QDate:
        {
            QDate qdate = *reinterpret_cast<QDate*>(params[0]);
            bdate rsldate;
            rsldate.day = qdate.day();
            rsldate.mon = qdate.month();
            rsldate.year = qdate.year();
            ValueSet(&ret, V_DATE, &rsldate);
        }
            break;
        case QMetaType::QTime:
        {
            QTime qdate = *reinterpret_cast<QTime*>(params[0]);
            btime rsldate;
            rsldate.hour = qdate.hour();
            rsldate.min = qdate.minute();
            rsldate.sec = qdate.second();
            ValueSet(&ret, V_TIME, &rsldate);
        }
            break;
        case QMetaType::QRect:
        {
            TGenObject *obj = (TGenObject*)CreateRectRsl(*reinterpret_cast<QRect*>(params[0]));
            ValueSet(&ret, V_GENOBJ, obj);
        }
            break;
        case QMetaType::QPoint:
        {
            TGenObject *obj = (TGenObject*)CreatePointRsl(*reinterpret_cast<QPoint*>(params[0]));
            ValueSet(&ret, V_GENOBJ, obj);
        }
        break;
        case QMetaType::QSize:
        {
            TGenObject *obj = (TGenObject*)CreateSizeRsl(*reinterpret_cast<QSize*>(params[0]));
            ValueSet(&ret, V_GENOBJ, obj);
        }
        break;
        case QMetaType::QByteArray:
        {
            TGenObject *obj = (TGenObject*)CreateByteArrayRsl(*reinterpret_cast<QByteArray*>(params[0]));
            ValueSet(&ret, V_GENOBJ, obj);
        }
        break;
        case QMetaType::QStringList:
        {
            QStringList *list = reinterpret_cast<QStringList*>(params[0]);
            TGenObject *ValueArray = (TGenObject*)_LibRslTArrayCreate(list->size(), list->size());

            int i = 0;
            for (const QString &item : *list)
            {
                VALUE val;
                ValueSet(&val, V_STRING, codec->fromUnicode(item).data());
                _LibRslTArrayPut(ValueArray, i++, &val);
                ValueClear(&val);
            }

            ValueSet(&ret, V_GENOBJ, ValueArray);
        }
            break;

        case QMetaType::QVariantList:
        {
            QVariantList *list = reinterpret_cast<QVariantList*>(params[0]);
            TGenObject *ValueArray = (TGenObject*)_LibRslTArrayCreate(list->size(), list->size());

            int i = 0;
            for (const QVariant &item : *list)
            {
                VALUE val;
                SetValueFromVariant(std::bind(StdValueSetFunc, &val,
                                              std::placeholders::_1,
                                              std::placeholders::_2),
                                    item);

                _LibRslTArrayPut(ValueArray, i++, &val);
                ValueClear(&val);
            }
            ValueSet(&ret, V_GENOBJ, ValueArray);
        }
            break;

        case QMetaType::UnknownType:
        {
            QString normalized = QString(method.typeName()).remove("&").remove("*");
            RegisterInfoBase *info = RegisterObjList::inst()->info(normalized);

            if (info)
            {
                TGenObject *obj = nullptr;
                info->Create((void**)&obj, (QObject*)result, RegisterInfoBase::CppOwner);
                ValueSet(&ret, V_GENOBJ, obj);
            }

        }
            break;
        }

        ReturnVal2(&ret);
        ValueClear(&ret);
    }

    for (int i = 0; i < MethodParams; i++)
    {
        int Type = method.parameterType(i);

        if (Type)
            QMetaType::destroy(Type, params[i + 1]);
        else
            delete[] params[i + 1];
    }

    if (type == QMetaObject::CreateInstance)
        delete[] params[0];

    delete[] params;

    return result;
}

int GenObjRunId(TGenObject *obj, long id)
{
    LoadFunctions();

    QObjectRsl *rsl = (QObjectRsl*)obj;
    const QMetaObject *meta = rsl->object->metaObject();
    QMetaMethod method = meta->method(id - OBJ_RSL_METHOD_OFFSET);
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");

    int NumParam = GetNumParm() - 1;
    id = FindMethod(meta, method.name(), NumParam);

    if (id < 0)
    {
        iError(IER_RUNMETHOD, "Param count missmatch");
        return -1;
    }

    method = meta->method(id);

    int parmaOffset = 0;
    if (method.returnType() != QMetaType::Void)
        parmaOffset = 1;

    CallMethod(meta, method, QMetaObject::InvokeMetaMethod, id, rsl->object);

    return 0;
}

int GenObjRun(TGenObject *obj, const char *methodname, long *id)
{
    QObjectRsl *rsl = (QObjectRsl*)obj;
    const QMetaObject *meta = rsl->object->metaObject();
    RegisterInfoBase *info = findInfo(meta->className());

    if (info->findMember(methodname, id) == 0)
        return GenObjRunId(obj, *id);

    return -1;
}
