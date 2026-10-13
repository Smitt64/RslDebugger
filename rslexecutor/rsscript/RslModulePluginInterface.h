// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef RSLMODULEPLUGININTERFACE_H
#define RSLMODULEPLUGININTERFACE_H

#include <QtPlugin>

class RslStaticModule;

class RslStaticModuleInterface
{
public:
    virtual ~RslStaticModuleInterface() = default;

    virtual QStringList staticModules() const = 0;
    virtual void registerStaticModules() = 0;
};

#define RslStaticModuleInterface_iid "org.toolsruntime.rsl.modules.RslStaticModuleInterface/1.0"
Q_DECLARE_INTERFACE(RslStaticModuleInterface, RslStaticModuleInterface_iid)

#endif // RSLMODULEPLUGININTERFACE_H
