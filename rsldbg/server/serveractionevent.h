#ifndef SERVERACTIONEVENT_H
#define SERVERACTIONEVENT_H

#include <QEvent>

class ServerActionEvent : public QEvent
{
public:
    ServerActionEvent(const qint16 &action, const QByteArray &data);
    virtual ~ServerActionEvent();

    const qint16 &action() const;
    QByteArray &buffer();

private:
    QByteArray m_Buffer;
    qint16 m_Action;
};

#endif // SERVERACTIONEVENT_H
