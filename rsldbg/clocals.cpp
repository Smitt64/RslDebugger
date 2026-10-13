#include "clocals.h"
#include "cdebug.h"

CLocals::CLocals(CDebug* pParent) :
    CWatchV(pParent)
{

}

CLocals::~CLocals()
{

}

bool CLocals::CollectLocals (const RSLSTACK _st, RSLSTACK* prevSt, bool instChanged)
{
    int is_object = 0;
    char str_name[MAX_NAME] = {0};
    char str_type[MAX_TYPENAME] = {0};
    char str_value[MAX_VALUE] = {0};
    RSLVINFO info = NULL;
    RSLVALUE	val  = NULL;

    std::vector<SpVarPtrType> newLocals;

    CLocals::iterator l_lastInserted = newLocals.end ();

    m_parent->do_GetFirstLocalInfo ( _st, &val, &is_object, str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value, MAX_VALUE, &info);
    if (!info)
        return false;

    do
    {
        DisplayVar* var = new DisplayVar((CDebugRoot*)m_parent);
        var->depth			= 1;
        var->info			= info;
        var->is_expanded	= false;
        var->is_object		= 0!=is_object;//??
        var->str_name		= str_name;
        var->str_type		= str_type;
        var->str_value		= str_value;
        var->val			   = val;

        l_lastInserted = newLocals.insert (l_lastInserted, SpVarPtrType (var));
    }
    while (m_parent->do_GetNextInfo ((*l_lastInserted)->info, &val, &is_object, str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value, MAX_VALUE, &info) && info);

    int l_curDepth = 1;
    bool l_foundOnCurDepth = false;
    do
    {
        l_foundOnCurDepth = false;
        std::vector<std::string> l_namesStack;
        CLocals::iterator it = begin ();
        for (; it != end (); ++it)
        {
            int remFromStack = 0;
            if (it != begin ())
            {
                remFromStack = ((*it)->depth - (*(it-1))->depth) - 1;
                remFromStack *= - 1;
            }

            if (remFromStack > 0)
                l_namesStack.erase (l_namesStack.end () - remFromStack, l_namesStack.end ());

            l_namesStack.push_back ((*it)->str_name.toStdString());

            if ((*it)->depth == l_curDepth && (*it)->is_expanded)
            {
                std::vector<std::string> l_namesTmpStack;

                for (std::vector<SpVarPtrType>::iterator localsIter = newLocals.begin (); localsIter!= newLocals.end(); ++localsIter)
                {
                    int remFromStack = 0;
                    if (localsIter != newLocals.begin ())
                    {
                        remFromStack = ((*localsIter)->depth - (*(localsIter-1))->depth) - 1;
                        remFromStack *= - 1;
                    }

                    if (remFromStack > 0)
                        l_namesTmpStack.erase (l_namesTmpStack.end () - remFromStack, l_namesTmpStack.end ());

                    l_namesTmpStack.push_back ((*localsIter)->str_name.toStdString());

                    if (l_namesTmpStack == l_namesStack)
                    {
                        if (! (*localsIter)->is_object)
                        {
                            break;
                        }
                        else
                        {
                            (*localsIter)->is_expanded = true;
                            l_foundOnCurDepth = true;

                            bool isFake = (*localsIter)->isFakeChildrensItem;
                            if (!isFake)
                            {
                                DisplayVar* childrens = CreateFakeChildrensItem((*localsIter)->val, l_curDepth + 1);
                                if (childrens)
                                    localsIter = newLocals.insert (++localsIter, SpVarPtrType (childrens));
                            }

                            if (isFake)
                            {
                                info = m_parent->do_GetFirstArrayInfo ((*localsIter)->val, &val,
                                                                      &is_object, str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value,
                                                                      MAX_VALUE);
                            }
                            else
                            {
                                m_parent->do_GetFirstPropInfo ((*localsIter)->val, &val, &is_object,
                                                              str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value,
                                                              MAX_VALUE, &info);
                            }

                            do
                            {
                                DisplayVar* var = new DisplayVar((CDebugRoot* )m_parent);
                                var->depth			= l_curDepth + 1;
                                var->info			= info;
                                var->is_expanded	= false;
                                var->is_object		= 0!=is_object;//??
                                var->str_name		= str_name;
                                var->str_type		= str_type;
                                var->str_value		= str_value;
                                var->val			   = val;

                                localsIter = newLocals.insert (++localsIter, SpVarPtrType (var));
                            }
                            while (m_parent->do_GetNextInfo ((*localsIter)->info, &val, &is_object, str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value, MAX_VALUE, &info) && info);
                            break;
                        }
                    }
                }
            }
        }
        ++l_curDepth;
    }
    while (l_foundOnCurDepth);

    swap (newLocals);

    return true;
}
