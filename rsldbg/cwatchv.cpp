#include "cwatchv.h"
#include "cdebug.h"
#include "lf/dbgexception.h"
#include <QMessageBox>
#include <QApplication>

CWatchV::CWatchV(CDebug *pParent)
{
    m_parent = pParent;
}

CWatchV::~CWatchV()
{
    SpIter i;
    for(i = begin(); i != end(); ++i){ //do not call ++i!!! erase(i) will change it!!!
        if((*i)->exp)
            m_parent->do_RemExp((*i)->exp);
        if((*i)->info)
            m_parent->do_DoneInfo((*i)->info);
        //erase(i);
    }
}

void CWatchV::operator=(vector <DisplayVar* >* arg)
{
    vector <DisplayVar*>::iterator j;

    for (j = arg->begin(); j != arg->end(); ++j)
    {
        SpVarPtrType aptr (*j);
        push_back (aptr);
    }
}

SpIter CWatchV::FindWatchV (const QString &str)
{
    SpIter i;
    for(i=begin(); i!=end(); ++i){
        if( str == ((*i) -> str_name )){
            return i;
        }
    }
    return end();
}

DisplayVar* CWatchV::CreateFakeChildrensItem (RSLVALUE value, int curDepth)
{
    DisplayVar* var = 0;

    RSLVALUE	val  = NULL;
    RSLVINFO	info = NULL;
    RSLVINFO	prev_info = NULL;

    char str_value[MAX_VALUE] = {0};
    char str_type[MAX_TYPENAME] = {0};
    char str_name[MAX_NAME] = {0};

    int is_object = 0;

    try
    {
        RSLVINFO childInfo = m_parent->do_GetFirstArrayInfo (value, &val,
                                                            &is_object, str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value,
                                                            MAX_VALUE);

        if (childInfo)
        {
            var = new DisplayVar (m_parent);
            var->isFakeChildrensItem = true;
            var->info			= childInfo;
            var->is_expanded	= false;
            var->is_object		= true;
            var->str_name		= "children";
            var->str_type		= "";
            var->str_value		= "";
            var->val			   = value;
            var->depth			= curDepth;
            var->st				= 0;
        }
    }
    catch (dbgexception& e)
    {
        e;
#ifdef _DEBUG
        QMessageBox::critical(qApp->topLevelWidgets()[0], "Error", e.what());
        //AfxMessageBox (e.what ());
#endif
    }

    return var;
}

bool CWatchV::AddWatchV (const QByteArray &str)
{
    if(end() == FindWatchV(str))
    {
        return false;
    }

    RSLPROC proc = m_parent->GetCurProc();

    if(!proc)
        return false;

    RSLVALUE	val  = NULL;
    RSLEXPCTX	exp  = NULL;

    int isObject = 0;
    char str_value[MAX_VALUE];
    char str_type[MAX_TYPENAME];

    int is_lvalue = -1;
    bool rv = m_parent->do_ParseExp(proc, str.data()/*.data()*/, &is_lvalue, &exp);

    if(!rv || !exp)
        return false;

    memset(str_value, 0, MAX_VALUE);
    memset(str_type, 0, MAX_TYPENAME);
    rv = m_parent->do_ExecExpAt(exp, m_parent->GetCurStack(), &isObject, str_type, MAX_TYPENAME, str_value, MAX_VALUE, &val);
    if(!rv || !val)
        return false;

    DisplayVar* var = new DisplayVar((CDebugRoot* )m_parent);

    var->str_name		=	str;
    var->str_type		=	str_type;
    var->str_value		=	str_value;
    var->depth			=	1;
    var->is_object		=	isObject!=0;//?
    var->is_lvalue		=	is_lvalue!=0;//?
    var->is_expanded	=	false;
    var->val			=	val;
    var->exp			=	exp;
    var->info			=	NULL;

    SpVarPtrType var_aptr (var);

    push_back(var_aptr);

    return true;
}

bool CWatchV::DelWatchV(int index)
{
    if (index < 0 || (size_t)index > size())
        return false;

    SpIter pos = begin()+index;//&at(index);//find(begin(), end(), at(index));

    if(end() == pos)
        return false;

    erase(pos);

    return true;
}

bool CWatchV::ExpandV(int index, RSLSTACK _st)
{
    RSLSTACK st = NULL;
    st = m_parent->GetCurStack();

    if (!st)
        return false;

    if (index < 0 || (size_t)index > size () - 1)
        return false;

    SpIter pos = begin ()+index;

    bool isFake = (*pos)->isFakeChildrensItem;

    if (pos == end ())
        return false;

    if(!(*pos)->is_object)
        return false;

    if((*pos)->is_expanded)
        return true;

    (*pos)->is_expanded = true;

    unsigned char depth = (*pos)->depth;
    ++depth;

    int j = index;

    RSLVALUE	val  = NULL;
    RSLVALUE	parent_val = (*pos)->val;
    RSLVINFO	info = NULL;
    RSLVINFO	prev_info = NULL;
    RSLSTACK	parent_stack = (*pos)->st;


    char str_value[MAX_VALUE] = {0};
    char str_type[MAX_TYPENAME] = {0};
    char str_name[MAX_NAME] = {0};
    bool rv = false;

    int is_object = 0;


    if (!isFake)
    {
        DisplayVar* var = CreateFakeChildrensItem (parent_val, depth);
        if (var)
            insert (begin () + ++j, SpVarPtrType (var));
    }

    if (isFake)
    {
        info = m_parent->do_GetFirstArrayInfo (parent_val, &val,
                                              &is_object, str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value,
                                              MAX_VALUE);

        rv = info != 0;
    }
    else
    {
        rv = m_parent->do_GetFirstPropInfo (parent_val, &val, &is_object,
                                           str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value,
                                           MAX_VALUE, &info);
    }

    if(!rv || !info)
        return false;

    SpIter cur_pos;

    while (info && rv)
    {
        DisplayVar* var = new DisplayVar (m_parent);
        var->info			= info;
        var->is_expanded	= false;
        var->is_object		= !!is_object;
        var->str_name		= str_name;
        var->str_type		= str_type;
        var->str_value		= str_value;
        var->val			   = val;
        var->depth			= depth;
        var->st				= parent_stack;

        ++j;
        cur_pos = begin () + j;

        insert (cur_pos, SpVarPtrType (var));
        prev_info = info;
        memset (str_name, 0, sizeof (MAX_NAME));
        memset (str_type, 0, sizeof (MAX_TYPENAME));
        memset (str_value, 0, sizeof (MAX_VALUE));
        is_object = 0;
        val = NULL;
        info = NULL;
        rv = m_parent->do_GetNextInfo (prev_info, &val,
                                      &is_object, str_name, MAX_NAME, str_type,
                                      MAX_TYPENAME, str_value, MAX_VALUE, &info);
    }

    return true;
}

bool CWatchV::CollapseV(int index)
{
    RSLSTACK st = NULL;
    st = m_parent->GetCurStack();
    if(!st)
        return false;

    if( index < 0 || (size_t)index > size() - 1)
        return false;

    SpIter pos = begin()+index;

    if(end()==pos)
        return false;

    if(!(*pos)->is_object)
        return false;

    if(!(*pos)->is_expanded)
        return true;

    (*pos)->is_expanded = false;
    NotifyCollapsing (pos);
    unsigned char _depth = (*pos)->depth;
    ++pos;
    bool fl = true;
    while(fl)
    {
        if(end() == pos)
        {
            fl = false;
        }
        else
        {
            if ((*pos)->depth <= _depth)
                fl = false;
            else
                pos = erase (pos);
        }
    }

    return true;
}
