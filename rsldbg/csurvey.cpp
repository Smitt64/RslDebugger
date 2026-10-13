#include "csurvey.h"
#include "cdebug.h"

CSurvey::CSurvey(CDebug *pParent) :
    CWatchV(pParent)
{
    unknownStr = "unknown";
    errorStr = "error";
    cannotevalStr = "cannot evaluate";
}

CSurvey::~CSurvey()
{

}

bool CSurvey::ExecuteExpression (RSLEXPCTX expr)
{
    bool ret = true;
    bool executed = m_parent->do_ExecExpAt (expr, m_parent->GetCurStack(), &isObject, typeStr, MAX_TYPENAME, valueStr, MAX_VALUE, &val);

    if (!executed || !strcmp (valueStr, "UNLOADED") || !val)
        ret = false;

    return ret;
}

bool CSurvey::ParseExpression (SpVarPtrType value)
{
    int isLval = 0;
    RSLEXPCTX expr = NULL;
    bool parsed = m_parent->do_ParseExp (m_parent->GetCurProc(), value->str_name.data(), &isLval, &expr);

    if (expr)
    {
        if (value->exp)
            m_parent->do_RemExp (value->exp);
        value->exp = expr;
    }
    return value->exp && parsed;
}

void CSurvey::ApplyCurInfoToElem (SpVarPtrType value)
{
    value->is_expanded = false;
    value->is_object   = !!isObject;
    value->str_value   = valueStr;
    value->str_type    = typeStr;
    value->info        = info;
    value->val         = val;
}

bool CSurvey::GetFirstChildItem (SpVarPtrType expElem)
{
    bool ret = false;

    if (expElem->isFakeChildrensItem)
    {
        info = m_parent->do_GetFirstArrayInfo (expElem->val, &val, &isObject,
                                              nameStr, MAX_NAME, typeStr, MAX_TYPENAME, valueStr,
                                              MAX_VALUE);

        ret = info != 0;
    }
    else
    {
        ret = m_parent->do_GetFirstPropInfo (expElem->val, &val, &isObject,
                                            nameStr, MAX_NAME, typeStr, MAX_TYPENAME, valueStr,
                                            MAX_VALUE, &info) && info;
    }

    return ret;
}

bool CSurvey::GetNextChildItem (SpVarPtrType expElem)
{
    return m_parent->do_GetNextInfo (expElem->info, &val, &isObject, nameStr, MAX_NAME, typeStr, MAX_TYPENAME, valueStr, MAX_VALUE, &info) && info;
}

void CSurvey::UpdateExpandInfo ()
{
    std::vector<std::string> tmpNameStack;
    for (iterator it = begin (); it != end (); ++it)
    {
        int remElems = 0;

        if (it != begin ())
            remElems = (*(it-1))->depth - (*it)->depth + 1;

        if (remElems > 0)
            tmpNameStack.erase (tmpNameStack.end () - remElems, tmpNameStack.end());

        tmpNameStack.push_back ((*it)->str_name.toStdString());

        if ((*it)->is_expanded)
            ExpandInfo.insert (tmpNameStack);
    }
}

CSurvey::iterator CSurvey::ExpandElem (iterator it)
{
    int childsCount = 0;

    bool isFake = (*it)->isFakeChildrensItem;
    int childsDepth = (*it)->depth + 1;

    if (!isFake)
    {
        SpVarPtrType dv = CreateFakeChildrensItem ((*it)->val, childsDepth);
        if (dv)
        {
            it = insert (it + 1, dv);
            ++childsCount;
        }
    }

    if (GetFirstChildItem (*(it - childsCount)))
    {
        (*(it - childsCount))->is_expanded = true;

        do
        {
            SpVarPtrType dv = new DisplayVar (m_parent);
            ApplyCurInfoToElem (dv);
            dv->exp = NULL;
            dv->proc     = m_parent->GetCurProc ();
            dv->st       = m_parent->GetCurStack ();
            dv->str_name = nameStr;
            dv->depth    = childsDepth;
            ++childsCount;
            it = insert (it + 1, dv);
        }
        while (GetNextChildItem (*it));
    }
    return it - childsCount;
}

void CSurvey::Expand()
{
    std::vector<std::string> tmpNameStack;
    for (iterator it = begin (); it != end (); ++it)
    {
        int remElems = 0;
        if (it != begin ())
            remElems = (*(it-1))->depth - (*it)->depth + 1;

        tmpNameStack.erase (tmpNameStack.end () - remElems, tmpNameStack.end ());
        tmpNameStack.push_back ((*it)->str_name.toStdString());

        if (!(*it)->is_expanded && (*it)->is_object && ExpandInfo.find (tmpNameStack) != ExpandInfo.end ())
            it = ExpandElem (it);
    }
}

void CSurvey::RefreshValue (SpVarPtrType value)
{
    bool executed     = false;
    bool parseExpired = false;
    if (value->exp)
    {
        executed = ExecuteExpression (value->exp);
        if (!executed)
            parseExpired = true;
    }

    if (!executed)
    {
        bool parsed = ParseExpression (value);

        if (parsed)
            executed = ExecuteExpression (value->exp);

        if (!executed)
        {
            isObject = 0;
            if (parseExpired)
            {
                QByteArray errStr;
                GetLastErrorText (m_parent, &errStr, errorStr);

                strcpy (valueStr, errStr.data());
                strcpy (typeStr,  errorStr.data());
            }
            else
            {
                QByteArray errStr;
                GetLastErrorText (m_parent, &errStr, errorStr);

                strcpy (valueStr, errStr.data());
                strcpy (typeStr,  errorStr.data());
            }
        }
    }

    if (executed)
    {
        value->proc     = m_parent->GetCurProc ();
        value->st       = m_parent->GetCurStack ();
        int index       = m_parent->FindStackIndex (value->st);
        value->str_proc = (*m_parent->GetStackInfo ())[index]->procname;
    }

    info = NULL;
    ApplyCurInfoToElem (value);
}

void CSurvey::BuildNameList (iterator it, std::vector<std::string>* dstNameList)
{
    while (1)
    {
        dstNameList->insert (dstNameList->begin (), (*it)->str_name.toStdString());
        int dpth = (*it)->depth;
        if (dpth > 1)
            while ((*(--it))->depth == dpth);
        else
            break;
    }
}

void CSurvey::NotifyCollapsing (iterator it)
{
    std::vector<std::string> nameList;
    BuildNameList (it, &nameList);
    for (std::set<std::vector<std::string> >::iterator setIt = ExpandInfo.begin ();
         setIt != ExpandInfo.end ();)
    {
        if (nameList.size () <= setIt->size () &&
            std::mismatch (nameList.begin (), nameList.end (), setIt->begin ()).first == nameList.end ())
            setIt = ExpandInfo.erase (setIt);
        else
            ++setIt;
    }
}

void CSurvey::Update(int m_index)
{
    class IsDepthIsOne : public std::unary_function<SpVarPtrType, bool>
    {
    public:
        bool operator ()(SpVarPtrType value) const
        {
            return value->depth == 1;
        }
    };

    UpdateExpandInfo ();

    erase(std::remove_if(begin(), end(), std::not1(IsDepthIsOne())), end());

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
    std::for_each(begin(), end(), std::bind1st(std::mem_fun(&CSurvey::RefreshValue), this));
#else
    std::for_each(begin(), end(), std::bind1st(std::mem_fun1(&CSurvey::RefreshValue), this));
#endif

    Expand();
}
