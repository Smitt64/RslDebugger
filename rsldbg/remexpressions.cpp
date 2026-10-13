#include "remexpressions.h"
#include "appcfgdata.hpp"
#include "csurvey.h"
#include "lf/bp_data.h"
#include "lf/types.h"
#include "displayvar.h"
#include <sstream>
#include <fstream>
#include <iterator>

RemExpressions::RemExpressions(CDebug* _dbg) :
    dbg (_dbg),
    restored (false)
{

}

void RemExpressions::Restore()
{
    if (!restored)
    {
        RSLMODULE module = 0;
        char modName_c[_MAX_FNAME] = {0};
        int mtype;
        dbg->do_EnumModule (module, modName_c, _MAX_FNAME, &mtype, &module);
        do
        {
            procName = modName_c;
            dbg->do_EnumModule (module, modName_c, _MAX_FNAME, &mtype, &module);
        }
        while (module);

        char fullPath_c[256] = {0};
        GetProductFileName ("RSL Debugger", procName.c_str(), fullPath_c);

        std::string fullFileName =  fullPath_c + std::string (".exp");
        std::ifstream ifile (fullFileName.c_str());
        std::string version;
        std::getline (ifile, version);

        std::vector<ExpInfo> expressions;
        typedef std::vector<ExpInfo>::iterator iterator;

        if (version == "1.0")
            std::copy (std::istream_iterator<ExpInfo>(ifile), std::istream_iterator<ExpInfo>(), std::back_inserter (expressions));

        CSurvey* surv = dbg->GetSurvey ();
        surv->clear();

        CStackInfo* stacki = dbg->GetStackInfo ();
        for (iterator it = expressions.begin (); it != expressions.end (); ++it)
        {
            DisplayVar* dv = new DisplayVar (dbg);

            RSLPROC hProc = 0;
            RSLSTACK hStack = 0;
            for (CStackInfo::iterator stit = stacki->begin (); stit != stacki->end (); ++stit)
            {
                if (it->contextStr == (*stit)->procname.toStdString())
                {
                    hProc = (*stit)->proc;
                    hStack = (*stit)->st;
                    break;
                }
            }
            dv->depth = it->depth;
            dv->proc = hProc;
            dv->str_proc = it->contextStr.c_str();
            dv->str_name = it->expStr.c_str();
            dv->is_object = it->isObject;
            dv->is_expanded = it->isExpanded;
            dv->str_type = it->typeStr.c_str();
            dv->st = hStack;
            surv->push_back (dv);
        }
    }
    restored = true;
}
