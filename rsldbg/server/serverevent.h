#ifndef SERVEREVENT_H
#define SERVEREVENT_H

// Базовый класс внутренних сообщений отладочного сервера (замена QEvent).
// Доставка - собственная очередь DbgServerBase (mutex + dequeue), Qt event
// loop здесь не участвовал и раньше.
class DbgServerEvent
{
public:
    explicit DbgServerEvent(int type) : m_Type(type) {}
    virtual ~DbgServerEvent() {}

    int type() const { return m_Type; }

private:
    int m_Type;
};

#endif // SERVEREVENT_H
