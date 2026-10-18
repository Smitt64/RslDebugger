#ifndef DBGSERVERPROTO_H
#define DBGSERVERPROTO_H

#ifndef IGNORE_INCLUDE
#include <QObject>
#include <QEvent>
#endif

#define RSLDBG_MAGIC "RSLDBG"

// v1: legacy, handshake без payload, все сообщения - бинарные упакованные структуры
// v2: handshake несёт DBG_HANDSHAKE, все сообщения - JSON (UTF-8),
//     хендлы (RSLVALUE/RSLSTACK/...) передаются строками во избежание double
#define RSLDBG_PROTOCOL_VERSION 2

#define DBG_REQUEST_HANDSHAKE 100
#define DBG_REQUEST_UPDATELOCALS_JSON 109
#define DBG_REQUEST_EVENT_JSON 110
#define DBG_REQUEST_UPDATETEXT_JSON 111
#define DBG_REQUEST_EXECCONTNUE_JSON 112
#define DBG_REQUEST_UPDATEBP_JSON 113
#define DBG_REQUEST_UPDATESTACK_JSON 114
#define DBG_REQUEST_EXPANDVARIABLE_JSON 115
#define DBG_REQUEST_GETVALUENFO_JSON 116

#define	MSG_BREAKPOINT   (QEvent::User + 1)
#define MSG_FINISH       (QEvent::User + 2)
#define MSG_TRACE        (QEvent::User + 3)
#define MSG_EDITWATCH    (QEvent::User + 4)
#define	MSG_LOG          (QEvent::User + 5)

#define	MSG_SERVER_EVENT        (QEvent::User + 6)
#define	MSG_UPDATEDBGINFO_EVENT (QEvent::User + 7)

#define  EXECCONTNUE_STEP_OVER           1
#define  EXECCONTNUE_STEP_INTO           2
#define  EXECCONTNUE_DBG_RUN             3
#define  EXECCONTNUE_DBG_ABORT           4
#define  EXECCONTNUE_STEP_OUT            5
#define  EXECCONTNUE_RUN_NEXTMOD         6
#define  EXECCONTNUE_RUN_THISMOD         7
#define  EXECCONTNUE_DBG_ABORT_DEFAULT   8
#define  EXECCONTNUE_EXEC_INTEREXP       9

enum DBGBP_TYPE
{
    DBGBP_ENABLED,
    DBGBP_DISABLED,
    DBGBP_INVISIBLE,
    DBGBP_DELETED
};

#include <packpsh1.h>
typedef struct
{
    char magic[6]; // RSLDBG
    qint32 size;
    qint16 action;
}DBGHEADER;

typedef struct
{
    qint32 version; // RSLDBG_PROTOCOL_VERSION
}DBG_HANDSHAKE;
#include <packpop.h>

inline bool DbgCheckHeader(DBGHEADER *hdr)
{
    return !memcmp(hdr->magic, RSLDBG_MAGIC, sizeof(hdr->magic));
}

// NB: datasize передаём ПО ЗНАЧЕНИЮ! При передаче по ссылке вызов вида
// DbgMakeHeader(&hdr, hdr.size, action) приводил к алиасингу: memset ниже
// обнуляет *hdr (включая hdr.size), и обнулённое значение записывалось
// обратно - на провод уходил заголовок с size == 0.
inline void DbgMakeHeader(DBGHEADER *hdr, const int datasize,  const qint16 &action)
{
    memset(hdr, 0, sizeof(DBGHEADER));
    memcpy(hdr->magic, RSLDBG_MAGIC, sizeof(hdr->magic));

    hdr->size = datasize;
    hdr->action = action;
}

#endif // DBGSERVERPROTO_H
