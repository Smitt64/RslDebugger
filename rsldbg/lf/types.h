#ifndef TYPES_H
#define TYPES_H

#include "bp_data.h"
#include "rsl/dbgintf.h"
#include "tracemsg.h"
#include <loki/SmartPtr.h>
#include <vector>
#include <string>

// Строки - сырые байты рантайма (ANSI/OEM), не UTF-8.
typedef struct tagTStackInfo
{
    RSLSTACK	st;
    RSLPROC		proc;
    RSLSTMT		stmt;
    RSLMODULE	mod;
    std::string		procname;
    std::string		modname;
    int			offs, len;
} TStackInfo;

typedef struct tagTModuleInfo
{
    std::string		name;
    int			type;
    RSLMODULE	hmod;
} TModuleInfo;

typedef	Loki::SmartPtr<TBpData, Loki::RefLinked> elem_bpdata;
typedef Loki::SmartPtr<TStackInfo, Loki::RefLinked> elem_stackinfo;
typedef Loki::SmartPtr<TModuleInfo, Loki::RefLinked> elem_moduleinfo;
typedef Loki::SmartPtr<TTraceMsg, Loki::RefLinked> elem_tracemsg;

typedef	std::string elem_qwatch;

typedef	std::vector<elem_bpdata>    CBPData;
typedef	std::vector<elem_stackinfo> CStackInfo;
typedef std::vector<elem_tracemsg>  CTraceMsg;

typedef CTraceMsg::iterator iter_tracemsg;
typedef	CBPData::iterator iter_bpdata;
typedef CStackInfo::iterator iter_stackinfo;

typedef std::vector<elem_moduleinfo> CModuleInfo;
typedef CModuleInfo::iterator iter_moduleinfo;

#endif // TYPES_H
