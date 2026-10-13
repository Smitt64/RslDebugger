#ifndef SAVER_H
#define SAVER_H

#include <QObject>

class CDebug;
enum SaverCommands
{
    CMD_MODULE_LOAD,
    CMD_MODULE_UNLOAD
};

class SaverBase
{
public:
    virtual ~SaverBase() = default;
    virtual void Save() = 0;
    virtual void Restore() = 0;
    virtual void Command (SaverCommands command, void* params) = 0;
};

class Saver
{
public:
    typedef QVector<SaverBase*>::iterator iterator;
    Saver(CDebug* dbg_);
    ~Saver();

    void Load();
    void Save();
    void Command(SaverCommands command, void* params);

    template <class SaverClassName>
    void AddInstance()
    {
        saveInstances.push_back(new SaverClassName(dbg));
    }
private:
    CDebug* dbg;
    QVector<SaverBase*> saveInstances;
};

#endif // SAVER_H
