#include "serveractionevent.h"
#include "dbgserverproto.h"

ServerActionEvent::ServerActionEvent(const qint16 &action, const QByteArray &data) :
    QEvent((QEvent::Type)MSG_SERVER_EVENT)
{
    m_Buffer = data;
    m_Action = action;
}

ServerActionEvent::~ServerActionEvent()
{

}

const qint16 &ServerActionEvent::action() const
{
    return m_Action;
}

QByteArray &ServerActionEvent::buffer()
{
    return m_Buffer;
}
