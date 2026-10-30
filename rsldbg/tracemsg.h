#ifndef TRACEMSG_H
#define TRACEMSG_H

#include <string>

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

// msg - сырая строка рантайма (ANSI/OEM байты, не UTF-8).
typedef struct tagTTraceMsg{
    std::string msg;
    MSGLEVEL level;
} TTraceMsg;

#endif // TRACEMSG_H
