#include "rembreakpoints.h"
#include "appcfgdata.hpp"
#include "lf/bp_data.h"
#include "lf/types.h"
#include <windows.h>

#include <iterator>
#include <fstream>
#include <sstream>

bool operator == (const BPinfo& inf1, const BPinfo& inf2)
{
    return ( inf1.offs == inf2.offs &&
            inf1.reallength == inf2.reallength);
}

std::ostream& operator<< (std::ostream& oc, const BPinfo& inf)
{
    oc << (inf.offs) << " " << (inf.reallength) << " " << (inf.isEnabled) << " ";
    return oc;
}

std::istream& operator >> (std::istream& ic, BPinfo& inf)
{
    ic >> inf.offs >> inf.reallength >> inf.isEnabled;
    return ic;
}

// ------------------------------------------------------------------

bool operator== (const ExpInfo& inf1, const ExpInfo& inf2)
{
    return (inf1.contextStr == inf2.contextStr &&
            inf1.expStr == inf2.expStr &&
            inf1.isObject == inf2.isObject);
}

std::ostream& operator<< (std::ostream& oc, const ExpInfo& inf)
{
    oc << inf.expStr << "\n" << inf.contextStr << "\n" << inf.typeStr << "\n"
       << inf.depth << " " <<inf.isExpanded << " " << inf.isObject << "\n";
    return oc;
}

// ------------------------------------------------------------------

std::istream&  operator>> (std::istream& ic, ExpInfo& inf)
{
    std::getline (ic, inf.expStr);
    std::getline (ic, inf.contextStr);
    std::getline (ic, inf.typeStr);
    std::string ParseStr;
    std::getline (ic, ParseStr);
    std::istringstream istr (ParseStr);
    istr >> inf.depth >> inf.isExpanded >> inf.isObject;
    return ic;
}

RemBreakPoints::RemBreakPoints(CDebug *_dbg) :
    dbg (dbg),
    restored (false)
{
    //extern CWnd* p_wndmain; //declared in extern.cpp
    //CMainFrame* cmf = static_cast<CMainFrame*>(p_wndmain);
    //global = cmf->m_dbgregistry.GetBpVis() == 1;
}

RemBreakPoints::~RemBreakPoints()
{

}

std::string RemBreakPoints::getModuleFileName (const std::string& modulename)
{
    return fullPath + modulename + ".bp";
}

void RemBreakPoints::Restore()
{
    if (!restored)
    {
        std::string dirPath;
        char dirPath_c [_MAX_FNAME] = {0};
        GetProductFileName ("RSL Debugger", 0, dirPath_c);
        dirPath = dirPath_c;

        RSLMODULE module = 0;
        char modName_c[_MAX_FNAME] = {0};
        int mtype;
        std::string procname;
        dbg->do_EnumModule (module, modName_c, _MAX_FNAME, &mtype, &module);
        do
        {
            procname = modName_c;
            dbg->do_EnumModule (module, modName_c, _MAX_FNAME, &mtype, &module);
        }
        while (module);

        module = 0;

        if (global)
            fullPath = dirPath + "\\";
        else
            fullPath = dirPath + "\\" + procname + "\\";

        dbg->do_EnumModule (module, modName_c, _MAX_FNAME, &mtype, &module);
        do
        {
            std::string modulename = modName_c;
            std::vector<BPinfo> bps;
            LoadModuleBP (modulename, &bps);
            AddModuleBPs (modulename, module, bps);
            modHandleToName[module] = modName_c;
            dbg->do_EnumModule (module, modName_c, _MAX_FNAME, &mtype, &module);
        }
        while (module);
    }

    for (std::vector<RSLMODULE>::iterator it = newModules.begin(); it != newModules.end(); ++it)
    {
        RestoreModFromFile (*it);
    }
    newModules.clear();
    restored = true;
}

void RemBreakPoints::SaveModuleBP (const std::string& modulename, const std::vector<BPinfo>& bps)
{
    CreateDirectoryA (fullPath.c_str(), 0);
    std::string filename = getModuleFileName (modulename);
    if (bps.size())
    {
        std::ofstream ofile (filename.c_str(), std::ios_base::trunc);
        ofile << "1.0\n"; // version
        std::copy (bps.begin(), bps.end(), std::ostream_iterator<BPinfo> (ofile));
    }
    else
    {
        DeleteFileA (filename.c_str());
    }
}

void RemBreakPoints::LoadModuleBP (const std::string& modulename, std::vector<BPinfo>* bps)
{
    std::string filename = getModuleFileName (modulename);
    std::ifstream ifile (filename.c_str());
    std::string version;
    std::getline (ifile, version);
    if (version == "1.0")
        std::copy (std::istream_iterator<BPinfo> (ifile), std::istream_iterator<BPinfo>(),
                  std::back_inserter (*bps));
}

void RemBreakPoints::AddModuleBPs (const std::string& modulename, RSLMODULE module, const std::vector<BPinfo>& bps)
{
    for (std::vector<BPinfo>::const_iterator it = bps.begin(); it != bps.end(); ++it)
    {
        TBpData* BPinfo = new TBpData;
        int rOffs;
        int rLen;
        RSLSTMT stmt;
        int rLine;
        bool found = dbg->do_GetStatementOfPos (module, it->offs, it->reallength,
                                               &rOffs, &rLen, &stmt, &rLine);

        if (!found || !stmt)
        {
            delete BPinfo;
            continue;
        }

        BPinfo->bp_type = (it->isEnabled) ? BP_DISABLED : BP_ENABLED;
        BPinfo->len = rLen;
        BPinfo->line = rLine;
        BPinfo->mod = module;
        BPinfo->modname = modulename.c_str();
        BPinfo->stmt = stmt;
        BPinfo->offs = rOffs;
        dbg->AddBp (BPinfo);
        dbg->do_BreakPointAt (stmt, BPinfo->key);
    }
}

void RemBreakPoints::GetModuleBPs (RSLMODULE module, std::vector<BPinfo>* bps)
{
    CBPData* bpd = dbg->GetBP();

    for (CBPData::iterator it = bpd->begin(); it != bpd->end(); ++it)
    {
        if ((*it)->mod == module)
        {
            BPinfo bpi;
            bpi.offs = (*it)->offs;
            bpi.reallength = (*it)->len;
            bpi.isEnabled = (*it)->bp_type != 0;
            bps->push_back (bpi);
        }
    }
}

void RemBreakPoints::RestoreModFromFile (RSLMODULE hMod)
{
    RSLMODULE module = 0;
    char modName_c [_MAX_FNAME] = {0};
    int mtype;

    dbg->do_EnumModule (module, modName_c, _MAX_FNAME, &mtype, &module);

    do
    {
        if (hMod == module)
        {
            std::vector<BPinfo> bps;
            LoadModuleBP (modName_c, &bps);
            AddModuleBPs (modName_c, module,bps);
            modHandleToName [module] = modName_c;
            break;//break do while cycle;
        }
        dbg->do_EnumModule (module, modName_c, _MAX_FNAME, &mtype, &module);
    }
    while (module);
}

void RemBreakPoints::SaveModToFile (RSLMODULE hMod)
{
    std::vector<RSLMODULE>::iterator it;
    it = std::find (newModules.begin(), newModules.end(), hMod);

    if (it != newModules.end())
        newModules.erase(it);
    else
    {
        std::map<RSLMODULE , std::string>::iterator it;
        it = modHandleToName.find (hMod);
        if (it != modHandleToName.end())
        {
            std::string modulename = it->second;
            std::vector<BPinfo> bps;
            GetModuleBPs (hMod, &bps);
            SaveModuleBP (modulename, bps);
            modHandleToName.erase (it);
        }
    }
}

void RemBreakPoints::Command (SaverCommands command, void* params)
{
    switch (command)
    {
    case CMD_MODULE_LOAD:
        newModules.push_back (reinterpret_cast<RSLMODULE>(params));
        break;
    case CMD_MODULE_UNLOAD:
        SaveModToFile (reinterpret_cast<RSLMODULE>(params));
        break;
    }
}

void RemBreakPoints::Save()
{
    for (std::map<RSLMODULE , std::string>::iterator itt = modHandleToName.begin(); itt != modHandleToName.end(); ++itt)
    {
        std::vector<BPinfo> bps;
        GetModuleBPs (itt->first, &bps);
        SaveModuleBP (itt->second, bps);
    }

    modHandleToName.clear();
}
