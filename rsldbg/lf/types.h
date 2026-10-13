#ifndef TYPES_H
#define TYPES_H

#include <QSharedPointer>
#include "bp_data.h"
#include "rsl/dbgintf.h"
#include "tracemsg.h"
#include <loki/SmartPtr.h>
#include <vector>
//#include <string>

typedef struct tagTStackInfo
{
    RSLSTACK	st;
    RSLPROC		proc;
    RSLSTMT		stmt;
    RSLMODULE	mod;
    QByteArray		procname;
    QByteArray		modname;
    int			offs, len;
} TStackInfo;

typedef struct tagTModuleInfo
{
    QByteArray		name;
    int			type;
    RSLMODULE	hmod;
} TModuleInfo;

typedef	Loki::SmartPtr<TBpData, Loki::RefLinked> elem_bpdata;
typedef Loki::SmartPtr<TStackInfo, Loki::RefLinked> elem_stackinfo;
typedef Loki::SmartPtr<TModuleInfo, Loki::RefLinked> elem_moduleinfo;
typedef Loki::SmartPtr<TTraceMsg, Loki::RefLinked> elem_tracemsg;

typedef	QByteArray elem_qwatch;

typedef	std::vector<elem_bpdata>    CBPData;
typedef	std::vector<elem_stackinfo> CStackInfo;
typedef std::vector<elem_tracemsg>  CTraceMsg;

typedef CTraceMsg::iterator iter_tracemsg;
typedef	CBPData::iterator iter_bpdata;
typedef CStackInfo::iterator iter_stackinfo;

typedef std::vector<elem_moduleinfo> CModuleInfo;
typedef CModuleInfo::iterator iter_moduleinfo;

#endif // TYPES_H
