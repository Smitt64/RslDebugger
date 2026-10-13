#include "typeinfo_p.h"
#include "registerobjlist.hpp"
#include "rslexecutor.h"
#include "toolsruntime.h"
#include "rslobjconnections.h"
#include <QDebug>
#include <QTextCodec>
#include <QDate>
#include <QTime>
#include <QMetaProperty>
#include <QLibrary>
#include <QMetaType>

void toolConnect()
{
    enum
    {
        prm_obj = 0,
        prm_signal,
        prm_function
    };

    R2M r2m;
    QString signal, function;
    int type = GetFuncParamType(prm_obj);

    if (type != QVariant::UserType && type != QMetaType::QObjectStar)
        ThrowParamTypeError<QObject>(prm_obj);

    type = GetFuncParamType(prm_signal);
    if (type == QVariant::String)
        signal = GetFuncParam(prm_signal).toString();

    type = GetFuncParamType(prm_function);
    if (type == QVariant::String)
        function = GetFuncParam(prm_function).toString();
    else
    {
        if (type == QVariantR2M)
            r2m = GetFuncParam(prm_function).value<R2M>();
        else
            ThrowParamTypeError<R2M>(prm_function);
    }

    QObject *obj = GetFuncParam<QObject*>(prm_obj);

    const QMetaObject *meta = obj->metaObject();
    QMetaMethod signalmethod;
    for (int i = 0; i < meta->methodCount(); i++)
    {
        QMetaMethod method = meta->method(i);

        if (method.methodType() == QMetaMethod::Signal && method.access() == QMetaMethod::Public)
        {
            if (signal == method.methodSignature())
            {
                signalmethod = method;
                break;
            }
        }
    }

    if (!signalmethod.isValid())
    {
        qInfo(logRsl()) << QString("toolConnect: Object %1 does not contain signal %2")
                               .arg(meta->className(), signal);

        SetReturnVal(false);
        return;
    }

    HRSLINST hInst = RslGetCurrentInst();
    RslExecutor *executor = rslExecutorForRslInstance(hInst);

    if (!executor)
    {
        qCritical(logRsl()) << "toolConnect: Executor not found for instance" << hInst
                            << " when try connect to signal" << signalmethod.methodSignature();
        SetReturnVal(false);
        return;
    }

    //qDebug() << meta->indexOfSignal(signalmethod.methodSignature()) - meta->methodOffset();
    RslObjConnections *connector = new RslObjConnections(executor);

    if (!function.isEmpty())
        connector->setMacroFunc(function);
    else
        connector->setMacroFunc(r2m);

    connector->initSignalSpy(obj, signalmethod.methodSignature());

    QObject::connect(executor, &RslExecutor::destroyed, connector, &RslObjConnections::deleteLater);
    QObject::connect(executor, &RslExecutor::PlayRepFinished, connector, &RslObjConnections::deleteLater);
    QObject::connect(obj, &QObject::destroyed, connector, &RslObjConnections::deleteLater);

    int execMethod = connector->execIndex();
    QMetaObject::connect(obj, signalmethod.methodIndex(), connector, execMethod);
}
