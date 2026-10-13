#ifndef LOGEVENT_H
#define LOGEVENT_H

#include <QEvent>
#include <QDateTime>

class QMessageLogContext;
class LogEvent : public QEvent
{
public:
    LogEvent(const QDateTime &DateTime, const QtMsgType &type, const QString &context, const QString &msg);
    virtual ~LogEvent();

    const QDateTime &dateTime() const;
    const QtMsgType &type() const;
    const QString &context() const;
    const QString &msg() const;

private:
    QDateTime m_DateTime;
    QtMsgType m_MsgType;
    //QMessageLogContext *m_pContext;
    QString m_Msg, m_Context;
};

#endif // LOGEVENT_H
