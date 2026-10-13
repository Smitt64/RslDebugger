// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <QApplication>
#include "playrep.h"
#include "registerobjlist.hpp"
#include "registerinfobase.h"
#include "rsldirs.h"
#include "toolsruntime.h"
#include "rsscript/RslModulePluginInterface.h"
#include "rslobjconnections.h"
#include <QMap>
#include <QPluginLoader>
#include <QDir>
#include <QFile>
#include <QRegExp>

RegisterObjList *RegisterObjList::m_inst = nullptr;

class RegisterObjListPrivate
{
    Q_DECLARE_PUBLIC(RegisterObjList)
public:
    RegisterObjListPrivate(RegisterObjList *obj)
    {
        q_ptr = obj;
        m_IncDir = nullptr;
        m_StaticMacroPath.append(".\\mac");
    }

    ~RegisterObjListPrivate()
    {
        if (m_IncDir)
            delete m_IncDir;
    }

    QStringList m_StaticMacroPath, m_MacroPath;
    QMap<QString, RegisterInfoBase*> m_Info;
    QMap<QString, RslStaticModule*> m_StaticModules;
    RegisterObjList *q_ptr;

    //QPluginLoader m_Loader;
    char *m_IncDir;

    QStringList m_PluginPath;
    QList<QPluginLoader*> m_Loaders;
};

// -------------------------------------------------------

RegisterObjList::RegisterObjList():
    d_ptr(new RegisterObjListPrivate(this))
{
    qRegisterMetaType<R2M>("R2M");
}

typedef const char (*qt_plugin_query_metadata_t)();
void RegisterObjList::loadStaticPlugins()
{
    Q_D(RegisterObjList);
    QObjectList lst = QPluginLoader::staticInstances();

    for (auto obj : lst)
    {
        RslStaticModuleInterface *plugin = dynamic_cast<RslStaticModuleInterface*>(obj);

        if (plugin)
            plugin->registerStaticModules();
    }

    QDir appDir(QApplication::applicationDirPath());
    QStringList dlls = appDir.entryList({"*.dll"}, QDir::Files);

    QStringList loaded;
    auto AddDll = [&loaded, &d](const QString &dll)
    {
        QLibrary lib(dll);
        if (!lib.load())
            return;

        qt_plugin_query_metadata_t func = (qt_plugin_query_metadata_t)lib.resolve("qt_plugin_query_metadata");
        if (!func)
            return;

        QPluginLoader *plugin = new QPluginLoader(dll);
        qInfo(logRsl()) << "Check dll as plugin:" << dll;
        qInfo(logRsl()) << dll << "metadata:" << func();

        if (!plugin->isLoaded() && !loaded.contains(QFileInfo(dll).fileName()) && plugin->load())
        {
            RslStaticModuleInterface *plugininterface = dynamic_cast<RslStaticModuleInterface*>(plugin->instance());

            if (plugininterface)
            {
                plugininterface->registerStaticModules();
                d->m_Loaders.append(plugin);
                loaded.append(QFileInfo(dll).fileName());
            }
            else
            {
                qInfo(logRsl()) << "Check result false: not rsl plugin";
                delete plugin;
        }
        }
        else
        {
            qInfo(logRsl()) << "Check result false:" << plugin->errorString();
            delete plugin;
        }
    };

    for (const QString &dll : dlls)
        AddDll(dll);

    appDir = QDir::current();
    dlls = appDir.entryList({"*.dll"}, QDir::Files);

    for (const QString &dll : dlls)
        AddDll(dll);
}

RegisterObjList *RegisterObjList::inst()
{
    if (!RegisterObjList::m_inst)
    {
        RegisterObjList::m_inst = new RegisterObjList();
        RegisterObjList::m_inst->loadStaticPlugins();

        QString ini = toolFullFileNameFromDir("rsl.ini");
        if (ini.isEmpty())
        {
            QDir appDir(qApp->applicationDirPath());
            QFile file(appDir.absoluteFilePath("rsl.ini"));
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                file.write("[General]");
                file.close();
            }
        }
    }

    return RegisterObjList::m_inst;
}

RegisterInfoBase *RegisterObjList::info(const QString &name)
{
    Q_D(RegisterObjList);

    if (!d->m_Info.contains(name))
        return nullptr;

    return d->m_Info[name];
}

RegisterInfoBase *RegisterObjList::info(const Qt::HANDLE &rslID)
{
    Q_D(RegisterObjList);

    RegisterInfoBase *result = nullptr;
    for (auto item : d->m_Info)
    {
        if (item->rslID() == rslID)
        {
            result = item;
            break;
        }
    }

    return result;
}

RegisterInfoBase *findInfo(const QString &name)
{
    return RegisterObjList::inst()->info(name);
}

bool RegisterObjList::isExists(const QString &name) const
{
    Q_D(const RegisterObjList);

    return d->m_Info.contains(name);
}

void RegisterObjList::InsertInfo(const QString &name, RegisterInfoBase *info)
{
    Q_D(RegisterObjList);
    d->m_Info.insert(name, info);
}

bool RegisterObjList::addStaticModulePrivate(const QString &name, RslStaticModule *module, RslStaticModuleCallerBase *caller)
{
    Q_D(RegisterObjList);

    if (!d->m_StaticModules.contains(name.toUpper()))
    {
        d->m_StaticModules.insert(name.toUpper(), module);
        module->setCaller(caller);
        return true;
    }

    return false;
}

RslStaticModule *RegisterObjList::staticModule(const QString &name)
{
    Q_D(RegisterObjList);

    if (d->m_StaticModules.contains(name.toUpper()))
        return d->m_StaticModules[name.toUpper()];

    return nullptr;
}

void RegisterObjList::setIncDir(const QString &path)
{
    Q_D(RegisterObjList);

    if (d->m_IncDir)
        delete d->m_IncDir;

    QStringList lst = path.split(";");
    for (QString &str : lst)
    {
        if (QDir::isRelativePath(str))
        {
            QDir d(qApp->applicationDirPath());
            d = QDir(d.absoluteFilePath(str));
            str = QDir::toNativeSeparators(d.canonicalPath());
        }
    }

    QString newpath = lst.join(";");
    d->m_IncDir = new char[newpath.size() + 1];
    qstrcpy(d->m_IncDir, newpath.toLocal8Bit().data());
    ToolsSetIncDir(d->m_IncDir);
}

void RegisterObjList::setIncDir(const QStringList &path)
{
    setIncDir(path.join(";"));
}

void RegisterObjList::setTextDir(const QString &path)
{
    ToolsSetTextDir(path.toLocal8Bit().data());
}

void RegisterObjList::addStaticMacroDir(const QString &dir)
{
    Q_D(RegisterObjList);
    d->m_StaticMacroPath.append(dir);
}

const QStringList &RegisterObjList::staticMacroDir() const
{
    Q_D(const RegisterObjList);
    return d->m_StaticMacroPath;
}

void RegisterObjList::addMacroDir(const QString &dir)
{
    Q_D(RegisterObjList);
    d->m_MacroPath.append(dir);
}

const QStringList &RegisterObjList::macroDir() const
{
    Q_D(const RegisterObjList);
    return d->m_MacroPath;
}

void RegisterObjList::applyMacroDirs()
{
    QStringList lst = macroDir();
    lst << staticMacroDir();
    setIncDir(lst);
}

bool RegisterObjList::AddObject(const QString &name, const bool &canCreate)
{
    Q_D(RegisterObjList);
    RegisterInfoBase *_info = info(name);

    if (!_info)
        return false;

    _info->importObject(canCreate);

    return true;
}

void RegisterObjList::AddStdProc (const char *name, RslFuncProc proc)
{
    ::AddStdProc(V_UNDEF, name, (RslStdProc)proc, 0);
}

QString RegisterObjList::incDirs() const
{
    return ToolsGetIncDir();
}

RegisterObjList *rslObjList()
{
    return RegisterObjList::inst();
}

void rslAddMacroDir(const QString &dir)
{
    RegisterObjList::inst()->addMacroDir(dir);
}

void rslAddStaticMacroDir(const QString &dir)
{
    RegisterObjList::inst()->addStaticMacroDir(dir);
}

QString rslFindMacroFile(const QString &macro)
{
    char file[_MAX_PATH] = {0};
    rslObjList()->applyMacroDirs();
    ToolsFindMacroFile(file, macro.toLocal8Bit().data());
    return file;
}

QMap<QString,QString> rslGetMacroInfo(const QString &macro)
{
    QMap<QString,QString> values;

    auto ReadValues = [&values](const QString &matched)
    {
        QString content = matched;
        content = content.remove("/*").remove("*/").trimmed();
        QStringList lines = content.split("\n");
        for (const QString &line : lines)
        {
            if (line.isEmpty())
                continue;

            QStringList items = line.split(":");

            QString key = items[0].remove("$").trimmed().simplified();
            values[key] = items[1].trimmed().simplified();
        }
    };

    QFile f(macro);
    if (f.open(QIODevice::ReadOnly))
    {
        QRegularExpression rx("\\/\\*[\\s\\S]*?\\*\\/");

        QTextStream stream(&f);
        stream.setCodec("IBM 866");
        QString content = stream.readAll();

        QRegularExpressionMatch match = rx.match(content);
        if (match.hasMatch())
        {
            QString matched = match.captured();
            ReadValues(matched);
        }
        f.close();
    }
    return values;
}
