// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef RSLEXECUTOR_H
#define RSLEXECUTOR_H

#include <QObject>
#include <QVariant>
#include "rsscript/registerinfobase.h"
#include "ToolsRuntime_global.h"
#include <concepts>
#include <type_traits>

typedef void (*ToolRslStdProc)(void);
typedef std::function<void(void)> RslExecutorProc;

#define QVariantR2M (QVariant::UserType + 1)

class RslExecutorPrivate;
class TOOLSRUNTIME_EXPORT RslExecutor : public QObject
{
    Q_OBJECT
    friend int Executor_MsgProcCaller(int mes, void *ptr, void *userData);
    friend int ShowVarsCaller(Qt::HANDLE sym, Qt::HANDLE mod, void *userData);
    friend void WriteOutCaller(const QString &msg, void *userData);
    friend bool RslPlayRepActionProc(void *UserData);
public:
    RslExecutor(QObject *parent = nullptr);
    virtual ~RslExecutor();

    virtual void playRep(const QString &filename, const QString &output = QString(), RslExecutorProc proc = RslExecutorProc());

    QStringList errors();
    QString outputFileName() const;
    QVariant call(const QString &name, const QVariantList &params);

    static QString getSymbolName(Qt::HANDLE sym);
    static void globalSet(Qt::HANDLE sym, const QVariant &value);
    void setDebugMacroFlag(const bool &Eanble);

signals:
    void WriteOut(QString);
    void ErrorMessage(int, QString);
    void PlayRepFinished();

protected:
    virtual void PlayRepProc();
    virtual void onError(const int &code, const QString &mes);
    virtual void onBeginExec(const QString &modname);
    virtual void onInspectModuleSymbol(Qt::HANDLE sym);
    virtual void onWriteOut(const QString &msg);

private:
    RslExecutorPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(RslExecutor);
};

bool TOOLSRUNTIME_EXPORT IsMacroExistsFunction(const QString &filename, const QString &name);
TOOLSRUNTIME_EXPORT RslExecutor *rslExecutorForRslInstance(Qt::HANDLE hrslinst);
bool TOOLSRUNTIME_EXPORT CompareTypes(const int &MetaType, void *val, bool isOutParam = false);
void TOOLSRUNTIME_EXPORT StdValueSetFunc(void *val, int type, void *ptr);
QVariant TOOLSRUNTIME_EXPORT SetFromRslValue(void *value, bool isStringListProp = false);
int TOOLSRUNTIME_EXPORT SetValueFromVariant(std::function<void(int,void*)> Setter, const QVariant &value);

QVariant TOOLSRUNTIME_EXPORT GetFuncParam(const int &id, bool isStringListProp = false);
bool TOOLSRUNTIME_EXPORT IsFuncParamQtObject(const int &id);
int TOOLSRUNTIME_EXPORT GetFuncParamObjOwner(const int &id);
void TOOLSRUNTIME_EXPORT SetFuncParamObjOwner(const int &id, const int &owner);
void TOOLSRUNTIME_EXPORT SetObjectOwnerProp(QObject *obj, const int &owner);

int TOOLSRUNTIME_EXPORT GetFuncParamCount();
void TOOLSRUNTIME_EXPORT SetFuncParam(const int &id, const QVariant &value);
void TOOLSRUNTIME_EXPORT SetReturnVal(const QVariant &value);
int TOOLSRUNTIME_EXPORT GetFuncParamType(const int &id);

void TOOLSRUNTIME_EXPORT AddFunctionToRsl(const QString &name, ToolRslStdProc proc);
void TOOLSRUNTIME_EXPORT ThrowParamTypeError(const int &id, const QString &needtype = QString());

bool TOOLSRUNTIME_EXPORT IsRectRsl(Qt::HANDLE obj);
QRect TOOLSRUNTIME_EXPORT GetRectRsl(Qt::HANDLE obj);
Qt::HANDLE TOOLSRUNTIME_EXPORT CreateRectRsl(const QRect &rect);

Qt::HANDLE TOOLSRUNTIME_EXPORT CreatePointRsl(const QPoint &pt);
bool TOOLSRUNTIME_EXPORT IsPointRsl(Qt::HANDLE obj);
QPoint TOOLSRUNTIME_EXPORT GetPointRsl(Qt::HANDLE obj);

bool TOOLSRUNTIME_EXPORT IsSizeRsl(Qt::HANDLE obj);
QSize TOOLSRUNTIME_EXPORT GetSizeRsl(Qt::HANDLE obj);
Qt::HANDLE TOOLSRUNTIME_EXPORT CreateSizeRsl(const QSize &sz);

Qt::HANDLE TOOLSRUNTIME_EXPORT CreateByteArrayRsl(const QByteArray &pt);
bool TOOLSRUNTIME_EXPORT IsByteArrayRsl(Qt::HANDLE obj);
QByteArray TOOLSRUNTIME_EXPORT GetByteArrayRsl(Qt::HANDLE obj);

template<class T>
void ThrowParamTypeError(const int &id)
{
    ThrowParamTypeError(id, typeid(T).name());
}

TOOLSRUNTIME_EXPORT void* MakeStringList(QStringList *lst, RegisterInfoBase::QObjectRslOwner owner);

template<class T>
T GetFuncParam(const int &id)
{
    QVariant val = GetFuncParam(id);
    if constexpr (std::is_base_of_v<std::remove_pointer<T>, QObject> &&
                  std::is_convertible_v<const volatile T*, const volatile QObject*>)
    {
        QObject *obj = val.value<QObject*>();

        T result = qobject_cast<T>(obj);
        return result;
    }

    return val.value<T>();
}

#endif // RSLEXECUTOR_H
