#ifndef TRACEMSG_H
#define TRACEMSG_H

#include <QObject>

enum MSGLEVEL
{
    MSGLEVEL_NORMAL = 0,
    MSGLEVEL_WARNING = 1,
    MSGLEVEL_ERROR = 2,
    MSGLEVEL_CRYTICAL = 3,

    MSGLEVEL_USERNORMAL = 4,
    MSGLEVEL_USERWARNING = 5,
    MSGLEVEL_USERERROR = 6,
    MSGLEVEL_USERCRYTICAL = 7
};

typedef struct tagTTraceMsg{
    QString msg;
    MSGLEVEL level;
} TTraceMsg;

#endif // TRACEMSG_H
