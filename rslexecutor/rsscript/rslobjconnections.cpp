#include <QSignalSpy>
#include "rsl/dlmintf.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "rslibdynamicfuncs.h"
#include "rslobjconnections.h"
#include "rslexecutor.h"

RslObjConnections::RslObjConnections(QObject *parent)
    : QObject{parent}
{

}

RslObjConnections::~RslObjConnections()
{

}

void RslObjConnections::setMacroFunc(const QString &macro)
{
    m_MacroName = macro;
}

void RslObjConnections::setMacroFunc(const R2M &R2m)
{
    m_R2m = R2m;
}

int RslObjConnections::execIndex() const
{
    const QMetaObject *meta = metaObject();
    return meta->indexOfMethod("ExecuteMacroFunc()");
}

void RslObjConnections::initSignalSpy(const QObject *obj, const char *signal)
{
    if (signal[0] == '2')
        m_pSpy.reset(new QSignalSpy(obj, signal));
    else
    {
        QString method = QString("2%1").arg(signal);
        m_pSpy.reset(new QSignalSpy(obj, method.toLocal8Bit().data()));
    }
}

void RslObjConnections::ExecuteMacroFunc()
{
    RslExecutor *excutor = qobject_cast<RslExecutor*>(parent());
    QList<QVariant> arguments = m_pSpy->takeFirst();

    if (!m_MacroName.isEmpty())
        excutor->call(m_MacroName, arguments);
    else
    {
        if (m_R2m.obj)
        {
            VALUE retVal;
            ValueMake(&retVal);

            int nPar = arguments.size();
            VALUE *prm = new VALUE[nPar];
            for (int i = 0; i < nPar; i++)
            {
                ValueMake(&prm[i]);

                SetValueFromVariant(std::bind(StdValueSetFunc, &prm[i],
                                              std::placeholders::_1,
                                              std::placeholders::_2),
                                    arguments[i]);
            }

            _LibRslObjInvoke((TGenObject*)m_R2m.obj, m_R2m.id, RSL_DISP_RUN, nPar, prm, &retVal);
        }
    }
}
