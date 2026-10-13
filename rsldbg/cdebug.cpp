#include "cdebug.h"
#include "cqsurvey.h"
#include "csurvey.h"
#include "clocals.h"
#include "cwatchv.h"
#include "saver.h"

const QString CDebug::expanding = "Expanding...\0";
const QString CDebug::na        = "n/a...\0";

CDebug::CDebug(HRD inst, TDbgIntf *dbgftable, QObject *parent)
    : CDebugRoot{parent},
    inInteractiveExp(false),
    m_survey(new CSurvey(this)),
    m_qsurvey(new CQSurvey(this)),
    m_locals(new CLocals(this)),
    m_dataSaver(new Saver(this)),
    m_isDebugging(false)
{
    m_inst            = inst;
    m_dbgftable       = dbgftable;
    m_dbgintfMajorVer = m_dbgftable->GetVersion (inst, 0);
    m_index           = 0;

    m_nVerHi = 0;
    m_nVerLo = 0;

    bpKey = 0;
}

CDebug::~CDebug()
{

}

void CDebug::GetRemoteVersion()
{
    if(m_nVerHi == 0)
    {
        do_GetVersion(&m_nVerLo, &m_nVerHi);

        if(m_dbgintfMajorVer > m_nVerHi)
            m_dbgintfMajorVer = m_nVerHi;
    }
}

iter_bpdata CDebug::FindBP(TBpData *bpdata)
{
    iter_bpdata  i;

    for(i = m_bpdata.begin(); i != m_bpdata.end(); ++i)
    {
        if((bpdata->bp_type == (*i)->bp_type) && (bpdata->len == (*i)->len) && (bpdata->offs == (*i)->offs))
            return i;
    }

    return m_bpdata.end();
}

bool CDebug::AddBp(TBpData *bpdata)
{
    iter_bpdata i = FindBP(bpdata);

    if(m_bpdata.end() != i)
        return false;
    else
    {
        bpdata->key = ++bpKey;

        elem_bpdata  bpnew(bpdata);
        m_bpdata.push_back(bpnew);
    }

    return true;
}

bool CDebug::DelBp(TBpData *bpdata)
{
    iter_bpdata  i = FindBP(bpdata);

    if(m_bpdata.end() == i)
        return false;
    else
        m_bpdata.erase(i);

    return true;
}

bool CDebug::DisBp(TBpData *bpdata)
{
    iter_bpdata  i = FindBP(bpdata);

    if(m_bpdata.end() == i)
        return false;
    else
        (*i)->bp_type = BP_DISABLED;

    return true;
}

CBPData* CDebug::GetBP()
{
    return &m_bpdata;
}

RSLPROC CDebug::GetCurProc(int index)
{
    int _index = index;

    if(index == -1)
        _index = m_index;

    return m_stackinfo[_index]->proc;
}

RSLSTACK CDebug::GetCurStack (int index)
{
    int  _index = index;

    if(index == -1)
        _index = m_index;

    return m_stackinfo[_index]->st;
}

CStackInfo* CDebug::GetStackInfo (void)
{
    return &m_stackinfo;
}

CSurvey* CDebug::GetSurvey(void)
{
    return m_survey.data();
}

int CDebug::GetStackCount()
{
    return (int)m_stackinfo.size();
}

int CDebug::FindStackIndex(RSLSTACK st)
{
    int num = GetStackCount();

    for(int i = 0; i < num; i++)
    {
        if(m_stackinfo[i]->st == st)
            return i;
    }

    return -1;
}

iter_watch CDebug::FindSWatch (const QString &str, RSLSTACK st, RSLPROC proc)
{
    for(iter_watch  i = m_survey->begin(); i != m_survey->end(); ++i)
    {
        if((*i)->str_name == str && (*i)->st == st && (*i)->proc == proc)
            return i;
    }

    return m_survey->end();
}

iter_watch CDebug::FindQWatch (const QString &str, RSLSTACK st, RSLPROC proc)
{
    for(iter_watch  i = m_qsurvey->begin(); i != m_qsurvey->end(); ++i)
    {
        if((*i)->str_name == str && (*i)->st == st && (*i)->proc == proc)
            return i;
    }

    return m_qsurvey->end();
}

iter_watch CDebug::FindLocal(const QString &str, RSLSTACK st, RSLPROC proc)
{
    for(iter_watch  i = m_locals->begin(); i != m_locals->end(); ++i)
    {
        if((*i)->str_name == str && (*i)->st == st && (*i)->proc == proc)
            return i;
    }

    return m_locals->end();
}

DisplayVar* CDebug::FindRootItem(DisplayVar* var, BOOL surv)
{
    bool fl = true;
    iter_watch i;

    if(surv)
    {
        i = FindSWatch(var->str_name, var->st, var->proc);

        if(m_survey->end() == i)
            return NULL;
    }
    else
    {
        i = FindLocal(var->str_name, var->st, var->proc);

        if(m_locals->end() == i)
            return NULL;
    }

    for(;;)
    {
        if((*i)->depth == 1)
            return GetImpl(*i);

        if(surv)
        {
            if(m_survey->begin() == i)
                return NULL;
        }
        else
        {
            if(m_locals->begin() == i)
                return NULL;
        }

        --i;
    }

    return NULL;
}

RSLMODULE CDebug::GetCurModule (int index)
{
    if(index != -1)
        return m_stackinfo[index]->mod;

    if(m_curmod == (RSLMODULE)-1)
        return m_stackinfo[m_index]->mod;
    else
        return m_curmod;
}

RSLSTMT CDebug::GetCurStatement(int* offs, int* len, int index)
{
    int _index = index;

    if(index == -1)
        _index = m_index;

    *offs = m_stackinfo[_index]->offs;
    *len  = m_stackinfo[_index]->len;

    return m_stackinfo[_index]->stmt;
}

int CDebug::GetStackSize()
{
    return (int)m_stackinfo.size();
}

int CDebug::GetWatchVarPos(DisplayVar* var)
{
    int  k = 0;

    for(iter_watch  i = m_survey->begin(); i != m_survey->end(); ++i)
    {
        if((*i)->str_name == var->str_name && (*i)->st == var->st && (*i)->proc == var->proc)
            return k;

        if((*i)->str_name != CDebug::na || ((*i)->is_object && !(*i)->is_expanded))
            ++k;
    }

    return -1;
}

int CDebug::GetLocalVarPos(DisplayVar* var)
{
    int  k = 0;

    for(iter_watch  i = m_locals->begin(); i != m_locals->end(); ++i)
    {
        if((*i)->str_name == var->str_name && (*i)->st == var->st && (*i)->proc == var->proc)
            return k;

        if((*i)->str_name != CDebug::na || ((*i)->is_object && !(*i)->is_expanded))
            ++k;
    }

    return -1;
}

QString CDebug::GetProcName (RSLSTACK st)
{
    QString name;
    int index = FindStackIndex(st);

    if(index != -1)
        name = m_stackinfo[index]->procname;

    return name;
}

QString CDebug::GetCurModuleName(int index)
{
    if(GetStackCount() > 0)
    {
        if(index > 0)
        {
            int _index = index;

            if(index == -1)
                _index = m_index;

            return QString(m_stackinfo[_index]->modname);
        }

        if(m_curmod == (RSLMODULE)NULL || m_curmod == (RSLMODULE)-1)
            return QString(m_stackinfo[m_index]->modname);
        else
        {
            RSLMODULE  curhmod = NULL;
            RSLMODULE  hmod    = NULL;
            bool  fl = true;
            char  modname[_MAX_FNAME];
            int   type;

            while(fl)
            {
                memset(modname, 0, _MAX_FNAME);

                type = -1;

                if(!do_EnumModule(curhmod, modname, _MAX_FNAME, &type, &curhmod) || !curhmod)
                    fl = false;
                else
                {
                    if(curhmod == m_curmod)
                        fl = false;
                }
            }

            return QString(modname);
        }
    }

    return QString("Unknown module");
}

RSLSTACK CDebug::GetStackAt(int index)
{
    if((index < 0) || ((size_t)index >= m_stackinfo.size()))
        return NULL;

    return m_stackinfo[index]->st;
}

int CDebug::GetIndex()
{
    return m_index;
}

void CDebug::SetIndex(int index)
{
    m_index = index;
}

void CDebug::UpdateDbgInfo()
{
    m_stackinfo.clear();

    RSLSTACK     st_prev = NULL;
    RSLSTACK     st_cur  = NULL;
    bool         fl      = true;
    RSLPROC      proc;
    RSLSTMT      stmt;
    RSLMODULE    mod;
    int          offs, len;
    char         procname[100];
    char         modname[100];
    TStackInfo  *stack_info;

    while(fl)
    {
        proc = NULL;
        stmt = NULL;
        mod  = NULL;
        offs = len = 0;

        memset(procname, 0, 100);
        memset(modname,  0, 100);

        do_EnumStack(st_prev, &proc, &mod, &stmt, &offs, &len, procname, 100, modname, 100, &st_cur);

        if(!st_cur)
            fl = false;
        else
        {
            stack_info = new TStackInfo;

            if(!st_prev)
                stack_info->st = st_cur; //first time, when null
            else
                stack_info->st = st_cur;

            stack_info->proc = proc;
            stack_info->stmt = stmt;
            stack_info->offs = offs;
            stack_info->len  = len;
            stack_info->mod  = mod;

            stack_info->procname = procname;//toUnicode(procname);
            stack_info->modname = modname;//toUnicode(modname);

            elem_stackinfo  stack_info_aptr(stack_info);
            m_stackinfo.push_back(stack_info_aptr);

            st_prev = st_cur;
            st_cur  = NULL;
        }
    }
}

CLocals* CDebug::GetLocals()
{
    return m_locals.data();
}

CQSurvey* CDebug::GetQSurvey()
{
    return m_qsurvey.data();
}

Saver *CDebug::GetSaver()
{
    return m_dataSaver.data();
}

void CDebug::UpdateVariables()
{
    char      str_name[MAX_NAME];
    char      str_type[MAX_TYPENAME];
    char      str_value[MAX_VALUE];
    int       is_object;
    RSLVALUE  val;

    SpIter i;

    for(i = m_locals->begin(); i != m_locals->end(); ++i)
    {
        val = NULL;
        is_object = 0;

        memset(str_name,  0, MAX_NAME);
        memset(str_type,  0, MAX_TYPENAME);
        memset(str_value, 0, MAX_VALUE);

        do_RefreshInfo((*i)->info, &val, &is_object, str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value, MAX_VALUE);

        (*i)->str_name    = str_name;
        (*i)->str_value   = str_value;
        (*i)->str_type    = str_type;
        (*i)->val         = val;
        (*i)->is_object   = is_object != 0;
        (*i)->is_expanded = false;
    }
}

void CDebug::UpdateSurvey()
{
    m_survey->Update(m_index);
}

void CDebug::UpdateQSurvey()
{
    SpIter     i;
    int        is_object;
    int        is_lvalue;
    char       type[MAX_TYPENAME];
    char       value[MAX_VALUE];
    RSLEXPCTX  exp;

    QByteArray str_unknown;
    QByteArray str_error;
    QByteArray str_cannoteval;

    str_unknown = "unknown";
    str_error = "error";
    str_cannoteval = "cannot evaluate";

    for(i = m_qsurvey->begin(); i != m_qsurvey->end(); ++i)
    {
        if((*i)->exp == NULL)
        {
            exp       = NULL;
            is_lvalue = -1;

            do_ParseExp((*i)->proc, (*i)->str_name.data(), &is_lvalue, &exp);

            (*i)->exp       = exp;
            (*i)->is_lvalue = is_lvalue != 0;
        }

        if((*i)->exp)
        {
            is_object = -1;
            is_lvalue = -1;

            memset(type,  0, MAX_TYPENAME);
            memset(value, 0, MAX_VALUE);

            do_ExecExpAt((*i)->exp, (*i)->st, &is_object, type, MAX_TYPENAME, value, MAX_VALUE, &(*i)->val);

            if((*i)->val)
            {
                (*i)->str_value = value;
                (*i)->str_type  = type;
                (*i)->is_object = is_object != 0;
                (*i)->debug     = (CDebugRoot *)this;
            }
            else
            {
                GetLastErrorText(this, &(*i)->str_value, str_unknown);

                (*i)->str_type  = str_error;
                (*i)->is_object = false;
                (*i)->debug     = (CDebugRoot *)this;
            }
        }
        else
        {
            GetLastErrorText(this, &(*i)->str_value, str_cannoteval);

            (*i)->str_type  = str_error;
            (*i)->is_object = 0;
            (*i)->debug     = (CDebugRoot *)this;
            (*i)->exp       = NULL;
            (*i)->val       = NULL;
            (*i)->info      = NULL;
        }
    }
}

void CDebug::ClearSurvey()
{
    SpIter i;

    for(i = m_survey->begin(); i != m_survey->end();)
    {
        if((*i)->info != NULL)
            i = m_survey->erase(i);
        else
        {
            (*i)->val         = NULL;
            (*i)->is_expanded = false;
            (*i)->is_object   = false;

            (*i)->str_type.clear();
            (*i)->str_value.clear();

            ++i;
        }
    }
}

void CDebug::EraseQSurvey()
{
    m_qsurvey->clear();
}

void CDebug::ClearQSurvey()
{
    SpIter i;

    for(i = m_qsurvey->begin(); i != m_qsurvey->end();)
    {
        if((*i)->info != NULL)
            i = m_qsurvey->erase(i);
        else
        {
            (*i)->val         = NULL;
            (*i)->is_expanded = false;
            (*i)->is_object   = false;

            (*i)->str_type.clear();
            (*i)->str_value.clear();

            ++i;
        }
    }
}

void CDebug::SetCurModule(RSLMODULE mod)
{
    m_curmod = mod;
}

RSLMODULE CDebug::GetActiveModule(void)
{
    return m_stackinfo[0]->mod;
}

bool CDebug::GetDebugState()
{
    return m_isDebugging;
}

void CDebug::SetDebugState(bool state)
{
    m_isDebugging = state;
}

TBpData* CDebug::FindBPbyKey (unsigned long key)
{
    iter_bpdata i;

    for(i = m_bpdata.begin(); i != m_bpdata.end(); ++i)
    {
        if((*i)->key == key)
            return Loki::GetImpl(*i);
    }

    return NULL;
}

bool CDebug::AddSWatch(QByteArray str, RSLSTACK st, RSLPROC proc, QByteArray curprocname, RSLMODULE curmodule, bool isLvalue)
{
    iter_watch i = FindSWatch(str, st, proc);

    if(m_survey->end() != i)
        return false;

    DisplayVar  *var = new DisplayVar((CDebugRoot *)NULL);

    var->str_name  = str;
    var->st        = st;
    var->proc      = proc;
    var->str_proc  = curprocname;
    var->mod       = curmodule;
    var->is_lvalue = isLvalue;

    SpVarPtrType  var_aptr(var);
    m_survey->push_back(var_aptr);

    return true;
}

bool CDebug::DelSwatch(DisplayVar *var)
{
    return true;
}

bool CDebug::AddQWatch(QByteArray str, RSLSTACK st, RSLPROC proc, QByteArray curprocname, RSLMODULE curmodule)
{
    iter_watch  i = FindQWatch(str , st, proc);

    if(m_qsurvey->end() != i)
        return false;

    DisplayVar  *var = new DisplayVar((CDebugRoot *)NULL);

    var->str_name = str;
    var->st       = st;
    var->proc     = proc;
    var->str_proc = curprocname;
    var->mod      = curmodule;

    SpVarPtrType  var_aptr(var);
    m_qsurvey->push_back(var_aptr);

    return true;
}

bool CDebug::DelQWatch (QString str)
{
    return true;
}

BP_TYPE CDebug::GetBPType(int index)
{
    return m_bpdata[index]->bp_type;
}

int CDebug::GetBPOffs(int index)
{
    return m_bpdata[index]->offs;
}

int CDebug::GetBPLen(int index)
{
    return m_bpdata[index]->len;
}

int CDebug::GetBPCount()
{
    return (int)m_bpdata.size();
}

TBpData* CDebug::GetBP(int i)
{
    return GetImpl(m_bpdata[i]);
}

iter_bpdata CDebug::GetBPEnd()
{
    return m_bpdata.end();
}

void CDebug::SetInst(HRD inst)
{
    m_inst = inst;
}

TDbgIntf* CDebug::GetDbgFTable()
{
    return m_dbgftable;
}

HRD CDebug::GetInst()
{
    return m_inst;
}

QString CDebug::GetError()
{
    return m_error;
}

int CDebug::GetStatementLine(int offs, int len, RSLMODULE mod)
{
    int      line = -1;
    int      realoffs, reallen;
    RSLSTMT  stmt;

    do_GetStatementOfPos(mod, offs, len, &realoffs, &reallen, &stmt, &line);

    return line;
}
