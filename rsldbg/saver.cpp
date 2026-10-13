#include "saver.h"

Saver::Saver(CDebug *dbg_)
{
    dbg = dbg_;
}

Saver::~Saver()
{
    for (iterator it = saveInstances.begin(); it != saveInstances.end(); ++it)
        delete *it;
}

void Saver::Load()
{
    std::for_each(saveInstances.begin(), saveInstances.end(), std::mem_fun (&SaverBase::Restore));
}

void Saver::Save()
{
    std::for_each (saveInstances.begin(), saveInstances.end(), std::mem_fun (&SaverBase::Save));
}

void Saver::Command(SaverCommands command, void* params)
{
    for (iterator it = saveInstances.begin(); it != saveInstances.end(); ++it)
        (*it)->Command(command, params);
}
