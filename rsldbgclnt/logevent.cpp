#include "logevent.h"
#include "dbgserverproto.h"
#include <QDebug>

LogEvent::LogEvent(const QDateTime &DateTime, const QtMsgType &type, const QString &context, const QString &msg):
    QEvent((QEvent::Type)MSG_LOG),
    m_DateTime(DateTime),
    m_MsgType(type),
    m_Msg(msg),
    m_Context(context)
{

}

LogEvent::~LogEvent()
{
    qDebug() << "~LogEvent()";
}

const QDateTime &LogEvent::dateTime() const
{
    return m_DateTime;
}

const QtMsgType &LogEvent::type() const
{
    return m_MsgType;
}

const QString &LogEvent::context() const
{
    return m_Context;
}

const QString &LogEvent::msg() const
{
    return m_Msg;
}
