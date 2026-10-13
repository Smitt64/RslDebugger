// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef RSLSTATICMODULE_H
#define RSLSTATICMODULE_H

#include <QObject>
#include "ToolsRuntime_global.h"

typedef struct RslStaticModuleCallerBase
{
    using FuncPtr = void(*)(void);

    FuncPtr Init = nullptr;
    FuncPtr Proc = nullptr;
    FuncPtr Close = nullptr;
}RslStaticModuleCallerBase;

class RslStaticModulePrivate;
class TOOLSRUNTIME_EXPORT RslStaticModule
{
public:
    RslStaticModule();
    void setCaller(RslStaticModuleCallerBase *caller);
    RslStaticModuleCallerBase *caller();

    void addConstant(const QString &name, const QVariant &value);
    void addGlobal(const QString &name, const QVariant &value);
    //RslStaticModuleCaller *caller();

    virtual void Init();
    virtual void Proc() = 0;
    virtual void Close();


private:
    RslStaticModulePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(RslStaticModule);
};

#endif // RSLSTATICMODULE_H
