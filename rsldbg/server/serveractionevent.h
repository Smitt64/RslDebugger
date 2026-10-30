#ifndef SERVERACTIONEVENT_H
#define SERVERACTIONEVENT_H

#include "serverevent.h"
#include "../dbgserverproto.h"
#include <string>

// Команда от клиента (action + JSON-payload в UTF-8).
class ServerActionEvent : public DbgServerEvent
{
public:
    ServerActionEvent(const qint16 &action, const std::string &data)
        : DbgServerEvent(MSG_SERVER_EVENT), m_Buffer(data), m_Action(action) {}
    virtual ~ServerActionEvent() {}

    const qint16 &action() const { return m_Action; }
    std::string &buffer() { return m_Buffer; }

private:
    std::string m_Buffer;
    qint16 m_Action;
};

#endif // SERVERACTIONEVENT_H
