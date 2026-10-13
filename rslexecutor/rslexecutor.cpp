// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <QApplication>
#include "playrep.h"
#include "qglobal.h"
#include "rsl/isymbol.h"
#include "rsl/krnlintf.h"
#include "rsscript/TRsbRSLInstTmpl.hpp"
#include "rslexecutor.h"
#include "erlist.hpp"
#include "rsscript/registerobjlist.hpp"
#include "rsscript/rslstaticmodule.h"
#include "toolsruntime.h"
#include "toolver.h"
#include <QUuid>
#include <QDebug>
#include <QTextCodec>
#include <QMessageBox>
#include <QSettings>
#include <QTemporaryFile>
#include <QMap>
#include <QGlobalStatic>

static char ErrorString[1024];

typedef QMap<Qt::HANDLE, RslExecutor*> RslInstanceExecutorsMap;
Q_GLOBAL_STATIC(RslInstanceExecutorsMap, rslInstanceExecutors);

static QString FormatErrorMsg(ERRINFO *error)
{
    //Ошибка party.mac строка 20[1]: неопределенный идентификатор dfg
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");

    return QString::fromUtf8("Ошибка %5 строка %1[%2]: %3 %4")
            .arg(error->line)
            .arg(error->pos)
            .arg(codec->toUnicode(GetErrMesEx(error->code, 0)))
            .arg(error->mes)
            .arg(error->file);
}

int Executor_MsgProc (IMESSAGE mes, void *ptr,void *UserData);

typedef TRsbRSLInstTmpl<TRSLConObjInstIntf> TRsbRSLConInstIntf;
typedef TConInstPtr<TRsbRSLConInstIntf, TRSLConObjMsgHandler> TRsbRSLConInst;

class TMacroUserData : public STD_USERDATA
{
public:
  TMacroUserData() :
      STD_USERDATA()
  {
  }

  RslExecutor *m_pExecutor;
  RslExecutorPrivate *m_pEcecPrivate;

  HRSLINST hrsl;
};

class RslExecutorPrivate
{
    Q_DECLARE_PUBLIC(RslExecutor)
public:
    RslExecutorPrivate(RslExecutor *obj) :
        q_ptr(obj)
    {
        Output = nullptr;

        qstrcpy(UserData.nameSpace, QUuid::createUuid().toString(QUuid::WithoutBraces).toLocal8Bit().data());
        UserData.m_pExecutor = obj;
        UserData.m_pEcecPrivate = this;
    }

    ~RslExecutorPrivate()
    {
        if (Output)
            delete[] Output;
    }

    HRSLINST RslGetCurrentInstEx(const char *nameSpace)
    {
        TRSLCacheData  data;
        TRSLMsgHandler  *host = RslGetMainHost();

        data.cache = NULL;
        host->msgProc(host, NULL, IM_GETCACHE, &data);

        if(data.cache != NULL)
            return HRSLCACHE_FUN(data.cache)->SearchInstance(data.cache, nameSpace);

        return NULL;
    }

    QScopedPointer<QTemporaryFile> m_pOutput;

    char *Output;

    TMacroUserData UserData;
    TRSLErrorsList m_ErrList;
    RslExecutor *q_ptr;
    RslExecutorProc PlayRepProc;
};

RslExecutor::RslExecutor(QObject *parent) :
    QObject(parent),
    d_ptr(new RslExecutorPrivate(this))
{
    rslObjList()->applyMacroDirs();
}

RslExecutor::~RslExecutor()
{
    delete d_ptr;
}

QString RslExecutor::outputFileName() const
{
    Q_D(const RslExecutor);
    return d->Output;
}

QStringList RslExecutor::errors()
{
    Q_D(RslExecutor);
    QStringList list;

    TRSLErrorsList err(d->m_ErrList);
    while (err.ErrorNext())
    {
        ERRLISTELEM *elem = err.GetCurrentErr();
        list.append(FormatErrorMsg(&elem->info));
    }

    return list;
}

QString RslExecutor::getSymbolName(Qt::HANDLE sym)
{
    ISYMBOL *symbol = (ISYMBOL*)sym;
    return QString(symbol->name);
}

void RslExecutor::globalSet(Qt::HANDLE sym, const QVariant &value)
{
    ISYMBOL *symbol = (ISYMBOL*)sym;

    auto SetterFunc = [=](int type, void *ptr) -> void
    {
        SymGlobalSet (symbol, type, ptr);
    };

    SetValueFromVariant(SetterFunc, value);
}
void RslExecutor::setDebugMacroFlag(const bool &Eanble)
{
    if (Eanble)
        SetDebugMacroFlag(1);
    else
        SetDebugMacroFlag(0);
}

void RslExecutor::onInspectModuleSymbol(Qt::HANDLE sym)
{

}

void RslExecutor::onWriteOut(const QString &msg)
{
    emit WriteOut(msg);
}

void WriteOutCaller(const QString &msg, void *userData)
{
    TMacroUserData *UserData = (TMacroUserData*)userData;
    UserData->m_pExecutor->onWriteOut(msg);
}

int ShowVarsCaller(Qt::HANDLE sym, Qt::HANDLE mod, void *userData)
{
    TMacroUserData *UserData = (TMacroUserData*)userData;
    UserData->m_pExecutor->onInspectModuleSymbol(sym);
    return 0;
}

static int ShowVars (ISYMBOL *sym, SYMMODULE *mod, void *userData)
{
    return ShowVarsCaller(sym, mod, userData);
}

void RslExecutor::onBeginExec(const QString &modname)
{
    InspectModule (ShowVars, SF_SPECIAL | SF_VAR | SF_MACRO | SF_FILE |
                   SF_STRUC | SF_ARRAY | SF_ALLMODULE, modname.toLocal8Bit().data());
}

int Executor_MsgProcCaller(int mes, void *ptr, void *userData)
{
    TMacroUserData *UserData = (TMacroUserData*)userData;

    switch(mes)
    {
    case IM_GETHOSTINFO:
    {
        THostInfoData *data = (THostInfoData *)ptr;
        data->useFdecimal  = true;
        data->isGUI        = false;
        data->toolsVersion = RSL_TOOLS_VERSION;
    }
        break;

    case IM_BEGIN_PARSE:
        if (UserData->m_pExecutor)
        {
            UserData->hrsl = RslGetCurrentInst();
            rslInstanceExecutors->insert(UserData->hrsl, UserData->m_pExecutor);
        }
        break;

    case IM_DONE_INSTANCE:
        if (UserData->m_pExecutor)
            (void)rslInstanceExecutors->take(UserData->hrsl);
        break;

    case IM_DYNAMIC_MODULE:
    {
        TRslDynModule *DynModule = (TRslDynModule *)ptr;
        if(!DynModule->proc)
        {
            RslStaticModule *mod = RegisterObjList::inst()->staticModule(DynModule->name);

            if (mod)
            {
                DynModule->init = mod->caller()->Init;
                DynModule->proc = mod->caller()->Proc;
                DynModule->done = mod->caller()->Close;
                //func(DynModule);
                if(DynModule->proc)
                    return 1;
            }
        }
    }
        break;
    case IM_SET_STMODULE_ADD:
        AddSystemModule();
        RegisterStringList();
        AddStdProc(V_GENOBJ, "StringList", RslStringList, 0);
        break;
    case IM_ERROR:
    {
        if (UserData->m_pExecutor)
        {
            ERRINFO *error = (ERRINFO*)ptr;
            QString mes = FormatErrorMsg(error);
            UserData->m_pExecutor->onError(error->code, {mes});
        }
    }
        break;
    case IM_MSGBOX:
        break;
    case IM_BEGIN_EXEC:
        if (UserData->m_pExecutor)
            UserData->m_pExecutor->onBeginExec((char*)ptr);
        break;
    case IM_MODULE_OPEN:
        RslOnlyDeclMode();
        break;
    case IM_OUTPUT:
    {
        QTextCodec *codec = QTextCodec::codecForName("IBM 866");
        WriteOutCaller(codec->toUnicode((char*)ptr), userData);
    }
        break;
    }
    return Std_MsgProc ( mes, ptr, UserData );
}

int Executor_MsgProc (IMESSAGE mes, void *ptr, void *userData)
{
    return Executor_MsgProcCaller((IMESSAGE)mes, ptr, userData);
}

void RslExecutor::PlayRepProc()
{
    Q_D(RslExecutor);

    if (d->PlayRepProc)
        d->PlayRepProc();
}

static bool RslPlayRepActionProc(void *UserData)
{
    TMacroUserData *dt = (TMacroUserData*)RslGetDataPassedToPlayRep(UserData);
    dt->m_pExecutor->PlayRepProc();

    return true;
}

void RslExecutor::playRep(const QString &filename, const QString &output, RslExecutorProc proc)
{
    Q_D(RslExecutor);

    TMacroUserData prm;
    prm.m_pExecutor = this;

    d->PlayRepProc = proc;

    if (output.isEmpty())
    {
        d->m_pOutput.reset(new QTemporaryFile());
        d->m_pOutput->open();

        d->Output = new char[1024];
        qstrcpy(d->Output, d->m_pOutput->fileName().toLocal8Bit().data());
    }
    else
    {
        d->Output = new char[output.length() + 1];
        qstrcpy(d->Output, output.toLocal8Bit().data());
    }

    qInfo(logRsl()) << "Begin execute macro file:" << filename;
    qInfo(logRsl()) << "Report filename:" << output;

    QScopedPointer<QSettings> m_RslSettings(new QSettings(toolFullFileNameFromDir("rsl.ini"), QSettings::IniFormat));

    m_RslSettings->beginGroup(QApplication::applicationName());
    bool debugbreak = m_RslSettings->value("debugbreak", false).toBool();
    setDebugMacroFlag(debugbreak);
    m_RslSettings->endGroup();

    if (debugbreak)
        qInfo(logRsl()) << "Debugbreak enabled...";
    else
        qInfo(logRsl()) << "Debugbreak disabled...";

    bool isErrors = true;
    RunRSLEx(filename.toLocal8Bit().data(),
             d->Output,
             d->UserData.nameSpace,
             Executor_MsgProc,
             RslPlayRepActionProc,
             &prm,
             false,
             0,
             &isErrors,
             &d->m_ErrList,
             d->Output);

    if (isErrors)
    {
        QStringList err = errors();
        qCritical(logRsl()) << "Execution finished with errors:";
        std::for_each(err.begin(), err.end(), [=](const QString &text)
                      {
                          qCritical(logRsl()) << text;
                      });
    }
    else
        qInfo(logRsl()) << "Execution success finished" << output;

    d->PlayRepProc = RslExecutorProc();
    emit PlayRepFinished();
}

QVariant RslExecutor::call(const QString &name, const QVariantList &params)
{
    Q_D(RslExecutor);
    TRsbParmForRsl RslParm(params.size());
    ISYMBOL* pSym = RslGetInstSymbol(name.toLocal8Bit().data());

    if (!pSym)
        return QVariant();

    auto SetterFunc = [&RslParm](int id, int type, void *ptr) -> void
    {
        ValueSet(RslParm[id].getValue(), type, ptr);
    };

    for (int i = 0; i < params.size(); i++)
    {
        SetValueFromVariant(std::bind(SetterFunc, i,
                                      std::placeholders::_1,
                                      std::placeholders::_2),
                            params[i]);
    }

    RslCallInstSymbol(pSym, RSL_DISP_RUN,
                      params.size(),
                      RslParm.getParmArray(),
                      RslParm.retVal().getValue());

    VALUE *retval = RslParm.retVal().getValue();
    return SetFromRslValue(retval);
}

void RslExecutor::onError(const int &code, const QString &mes)
{
    emit ErrorMessage(code, mes);
}

void AddFunctionToRsl(const QString &name, ToolRslStdProc proc)
{
    AddStdProc(V_UNDEF, name.toLocal8Bit().data(), proc, 0);
}

QVariant GetFuncParam(const int &id, bool isStringListProp)
{
    VALUE *val;
    GetParm(id, &val);

    return SetFromRslValue(val, isStringListProp);
}

bool IsFuncParamQtObject(const int &id)
{
    VALUE *val;
    GetParm(id, &val);

    Qt::HANDLE handle = (Qt::HANDLE)RSCLSID(val->value.obj);
    RegisterInfoBase *info = RegisterObjList::inst()->info(handle);

    return info;
}

int GetFuncParamObjOwner(const int &id)
{
    if (!IsFuncParamQtObject(id))
        return -1;

    VALUE *val;
    GetParm(id, &val);

    QObjectRsl *obj = (QObjectRsl*)val->value.obj;
    return obj->owner;
}

void SetObjectOwnerProp(QObject *obj, const int &owner)
{
    obj->setProperty(OBJECT_PROP_OWNER, owner);
}

void SetFuncParamObjOwner(const int &id, const int &owner)
{
    if (!IsFuncParamQtObject(id))
        return;

    VALUE *val;
    GetParm(id, &val);

    QObjectRsl *obj = (QObjectRsl*)val->value.obj;
    obj->owner = (RegisterInfoBase::QObjectRslOwner)owner;
}

int GetFuncParamCount()
{
    return GetNumParm();
}

void SetReturnVal(const QVariant &value)
{
    auto SetterFunc = [=](int type, void *ptr) -> void
    {
        ReturnVal(type, ptr);
        //SetParm(id, type, ptr);
    };

    SetValueFromVariant(SetterFunc, value);
}

void SetFuncParam(const int &id, const QVariant &value)
{
    auto SetterFunc = [=](int type, void *ptr) -> void
    {
        SetParm(id, type, ptr);
    };

    SetValueFromVariant(SetterFunc, value);
}

void ThrowParamTypeError(const int &id, const QString &needtype)
{
    if (needtype.isEmpty())
        sprintf(ErrorString, "Param %d type missmatch", id);
    else
    {
        sprintf(ErrorString, "Param %d type missmatch, required %s",
                id, needtype.toLocal8Bit().data());
    }

    iError(IER_RUNTIME, ErrorString);
}

void ThrowMethodNotFoundError(const QString &needtype)
{
    sprintf(ErrorString, "Method \"%s\" not found", needtype.toLocal8Bit().data());

    iError(IER_RUNTIME, ErrorString);
}

RslExecutor *rslExecutorForRslInstance(Qt::HANDLE hrslinst)
{
    if (!rslInstanceExecutors->contains(hrslinst))
        return nullptr;

    return rslInstanceExecutors->value(hrslinst);
}

bool IsMacroExistsFunction(const QString &filename, const QString &name)
{
    bool result = false;
    TMacroUserData user;
    memset(&user, 0, sizeof(TMacroUserData));

    QScopedPointer<QTemporaryFile> report(new QTemporaryFile());
    report->open();

    char *Output = new char[1024];
    qstrcpy(Output, report->fileName().toLocal8Bit().data());

    QScopedPointer<TRSLConInstance> ptr(new TRSLConInstance());
    ptr->Init(filename.toLocal8Bit().data(), Output,
              QUuid::createUuid().toByteArray().data(), NULL,
              Executor_MsgProc, &user);

    if (ptr->AddModule(filename.toLocal8Bit().data()))
    {
        HRSLSYM hInst = ptr->RslGetInstSymbol(name.toLocal8Bit().data());

        if (hInst)
            result = true;
    }

    delete[] Output;
    return result;
}
