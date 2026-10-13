#ifndef REMBREAKPOINTS_H
#define REMBREAKPOINTS_H

#include "cdebug.h"
#include "saver.h"

class RemBreakPoints : public SaverBase
{
public:
    RemBreakPoints(CDebug* _dbg);
    virtual ~RemBreakPoints();

    void Restore();
    void Save();
    void Command (SaverCommands command, void* params);
    std::string GetModuleFileName(const std::string& modulename);

protected:
    void LoadModuleBP (const std::string& modulename, std::vector<BPinfo>* bps);
    void AddModuleBPs (const std::string& modulename, RSLMODULE module , const std::vector<BPinfo>& bps);
    void SaveModuleBP (const std::string& modulename, const std::vector<BPinfo>& bps);
    void GetModuleBPs (RSLMODULE module, std::vector<BPinfo>* bps);

private:
    CDebug* dbg;
    bool restored;
    bool global;
    std::string fullPath;
    std::vector<RSLMODULE> newModules;
    std::map<RSLMODULE , std::string> modHandleToName;
    void RestoreModFromFile(RSLMODULE module);
    void SaveModToFile(RSLMODULE module);
    std::string getModuleFileName(const std::string& modulename);
};

#endif // REMBREAKPOINTS_H
