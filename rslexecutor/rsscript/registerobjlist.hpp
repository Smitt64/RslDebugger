// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef REGISTEROBJLIST_HPP
#define REGISTEROBJLIST_HPP

#include <QObject>
#include "registerinfobase.h"
#include "rslstaticmodule.h"
#include "ToolsRuntime_global.h"

#define OBJECT_PROP_OWNER "RSL_OWNER"

typedef void (*RslFuncProc)(void);

class RslStaticModule;
TOOLSRUNTIME_EXPORT RegisterInfoBase *findInfo(const QString &name);

template<class Obj>
class RegisterObjInfo : public RegisterInfoBase
{
public:
    RegisterObjInfo(const qint32 &flags = 0) :
        RegisterInfoBase()
    {
        const QMetaObject meta = Obj::staticMetaObject;
        FillFromMetaObject(flags, meta,
                           RegisterObjInfo<Obj>::GenObjTypeName,
                           RegisterObjInfo<Obj>::GenObjFindMember);

        SetFunctions(RegisterObjInfo<Obj>::GenObjGetUniqID,
                     RegisterObjInfo<Obj>::GenObjEnumProps);

        AddObjectFuncs(RegisterObjInfo<Obj>::InitProvider,
                       RegisterObjInfo<Obj>::DoneProvider,
                       RegisterObjInfo<Obj>::CreateObject,
                       RegisterObjInfo<Obj>::GetTypeInfo,
                       RslConstructorFunc);
    }

    static const char *GenObjTypeName(void *o)
    {
        const QMetaObject meta = Obj::staticMetaObject;
        return meta.className();
    }

    static int GenObjFindMember(const char *name, long *id)
    {
        const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());

        return info->findMember(name, id);
    }

    static int GenObjEnumProps(void *obj, int cmd, void *data)
    {
        const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());

        return EnumPropsCaller(info, obj, cmd, data);
    }

    static int GenObjGetUniqID(void *pObj, const char *name)
    {
        const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());

        long  id = -1;
        info->findMember(name, &id);
        return id;
    }

    static int InitProvider(void *clntId)
    {
        const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());
        return InitProviderCaller(info, clntId);
    }

    static void DoneProvider(void *clntId)
    {
        const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());
        return DoneProviderCaller(info, clntId);
    }

    static void *CreateObject(void *clntId, const char *typeName)
    {
        const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());

        if(qstricmp(typeName, meta.className()) != 0)
            return NULL;

        void *GenObject = nullptr;
        info->Create(&GenObject, nullptr);
        return GenObject;
    }

    static void RslConstructorFunc()
    {
        const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());

        ConsrtuctorCaller(info);
    }

    static void *GetTypeInfo(void *clntId, const char *typeName)
    {
        const QMetaObject meta = Obj::staticMetaObject;
        RegisterInfoBase *info = findInfo(meta.className());

        return GetTypeInfoCaller(info, clntId, typeName);
    }

};

class RegisterObjListPrivate;
class TOOLSRUNTIME_EXPORT RegisterObjList
{
public:
    static RegisterObjList *inst();

    RegisterInfoBase *info(const QString &name);
    RegisterInfoBase *info(const Qt::HANDLE &rslID);
    bool isExists(const QString &name) const;

    template<class Module, const char *name>
    void addStaticModule(RslStaticModule *module)
    {
        struct tagRslStaticModuleCaller : public RslStaticModuleCallerBase
        {
        }RslStaticModuleCaller;

        tagRslStaticModuleCaller *caller = new tagRslStaticModuleCaller();
        if (addStaticModulePrivate(name, module, caller))
        {
            caller->Init = [](void)
            {
                RslStaticModule *mod = RegisterObjList::inst()->staticModule(name);
                mod->Init();
            };

            caller->Proc = [](void)
            {
                RslStaticModule *mod = RegisterObjList::inst()->staticModule(name);
                mod->Proc();
            };

            caller->Close = [](void)
            {
                RslStaticModule *mod = RegisterObjList::inst()->staticModule(name);
                mod->Close();
            };
        }
    }

    template<class Obj>
    void RegisterRslObject(const qint32 &flags = 0)
    {
        static_assert(std::is_base_of_v<QObject, Obj>, "QObject is not base of class");
        const QMetaObject meta = Obj::staticMetaObject;

        if (!isExists(meta.className()))
        {
            RegisterInfoBase *_info = new RegisterObjInfo<Obj>(flags);
            InsertInfo(meta.className(), _info);
        }
    }

    template<class Obj>
    bool AddObject(const bool &canCreate = true)
    {
        static_assert(std::is_base_of_v<QObject, Obj>, "QObject is not base of class");
        const QMetaObject meta = Obj::staticMetaObject;

        RegisterInfoBase *_info = info(meta.className());

        if (!_info)
            return false;

        _info->importObject(canCreate);

        return true;
    }

    bool AddObject(const QString &name, const bool &canCreate = true);
    void AddStdProc (const char *name, RslFuncProc proc);

    RslStaticModule *staticModule(const QString &name);

    void setIncDir(const QString &path);
    void setIncDir(const QStringList &path);

    static void setTextDir(const QString &path);

    void addStaticMacroDir(const QString &dir);
    const QStringList &staticMacroDir() const;

    void addMacroDir(const QString &dir);
    const QStringList &macroDir() const;

    void applyMacroDirs();

    QString incDirs() const;

private:
    RegisterObjList();
    void loadStaticPlugins();
    void InsertInfo(const QString &name, RegisterInfoBase *info);
    bool addStaticModulePrivate(const QString &name, RslStaticModule *module, RslStaticModuleCallerBase *caller);
    static RegisterObjList *m_inst;

    RegisterObjListPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(RegisterObjList);
};

template<class Module, const char *name>
void addStaticModule(RslStaticModule *module)
{
    RegisterObjList::inst()->addStaticModule<Module, name>(module);
}

template<class Module>
void addStaticModule(RslStaticModule *module)
{
    RegisterObjList::inst()->addStaticModule<Module, Module::name>(module);
}

TOOLSRUNTIME_EXPORT RegisterObjList *rslObjList();
TOOLSRUNTIME_EXPORT void rslAddMacroDir(const QString &dir);
TOOLSRUNTIME_EXPORT void rslAddStaticMacroDir(const QString &dir);
TOOLSRUNTIME_EXPORT QString rslFindMacroFile(const QString &macro);
TOOLSRUNTIME_EXPORT QMap<QString,QString> rslGetMacroInfo(const QString &macro);

void RegisterStringList(void);
void RslStringList();

#endif // REGISTEROBJLIST_HPP
