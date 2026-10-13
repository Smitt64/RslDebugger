// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef REGISTERINFOBASE_H
#define REGISTERINFOBASE_H

#include <QObject>
#include "ToolsRuntime_global.h"

#define OBJ_RSL_METHOD_OFFSET 1001
#define OBJ_RSL_ENUM_OFFSET 2001

typedef const char* (*ObjTypeName)(void*);
typedef int (*ObjFindMember)(const char*,long *);
typedef int (*ObjGetUniqID)(void*,const char*);
typedef int (*ObjMemberFromID)(void*,long, long*);
typedef int (*ObjEnumProps)(void *obj, int cmd, void *data);

typedef int(*ObjInitProvider)(void*);
typedef void(*ObjDoneProvider)(void*);
typedef void*(*ObjCreateObject)(void*, const char*);
typedef void*(*ObjClassProc)(void*, const char*);
typedef void(*ObjConstructorProc)(void);

enum GenInfoFlags
{
    GenInfoUseParentProps = 1 << 1,
    GenInfoUseParentMeths = 1 << 2,
};

class RegisterInfoBasePrivate;
class TOOLSRUNTIME_EXPORT RegisterInfoBase
{
    friend TOOLSRUNTIME_EXPORT void ConsrtuctorCaller(RegisterInfoBase *info);
    friend TOOLSRUNTIME_EXPORT int EnumPropsCaller(RegisterInfoBase *info, void *obj, int cmd, void *data);
    friend TOOLSRUNTIME_EXPORT int InitProviderCaller(RegisterInfoBase *info, void *clntId);
    friend TOOLSRUNTIME_EXPORT void DoneProviderCaller(RegisterInfoBase *info, void *clntId);
    friend TOOLSRUNTIME_EXPORT void *GetTypeInfoCaller(RegisterInfoBase *info, void *clntId, const char *typeName);
public:
    enum QObjectRslOwner
    {
        RslOwner = 0,
        CppOwner
    };

    RegisterInfoBase();
    virtual ~RegisterInfoBase();

    int findMember(const char *name, long *id);
    void importObject(const bool &canCreate = true);

    int enumValue(int id) const;

    Qt::HANDLE rslID() const;
    Qt::HANDLE object(Qt::HANDLE GenObject);

    virtual void Create(void **GenObject, QObject *cls, const QObjectRslOwner &owner = CppOwner);

protected:
    void FillFromMetaObject(const qint32 &flags,
                            const QMetaObject &meta,
                            ObjTypeName ObjTypeNameFunc,
                            ObjFindMember ObjFindMemberFunc);

    void SetFunctions(ObjGetUniqID ObjGetUniqIDFunc, ObjEnumProps ObjEnumProps);
    void AddObjectFuncs(ObjInitProvider _InitProvider,
                        ObjDoneProvider _DoneProvider,
                        ObjCreateObject _CreateObject,
                        ObjClassProc _ClassProc,
                        ObjConstructorProc _ObjConstructorProc);

    void CallCansrtuctor();
    virtual int EnumProps(void *obj, int cmd, void *data);

    virtual int InitProvider(void *clntId);
    virtual void DoneProvider(void *clntId);
    virtual void *GetTypeInfo(void *clntId, const char *typeName);

private:
    RegisterInfoBasePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(RegisterInfoBase);
};

extern int FindMethod(const QMetaObject *meta, const QString &name, int NumParams, bool NeedConstructor = false);
extern void *CallMethod(const QMetaObject *meta,
                const QMetaMethod &method,
                const QMetaObject::Call &type,
                const long &id, QObject *Instance);

TOOLSRUNTIME_EXPORT void ConsrtuctorCaller(RegisterInfoBase *info);
TOOLSRUNTIME_EXPORT int EnumPropsCaller(RegisterInfoBase *info, void *obj, int cmd, void *data);

TOOLSRUNTIME_EXPORT int InitProviderCaller(RegisterInfoBase *info, void *clntId);
TOOLSRUNTIME_EXPORT void DoneProviderCaller(RegisterInfoBase *info, void *clntId);
TOOLSRUNTIME_EXPORT void *GetTypeInfoCaller(RegisterInfoBase *info, void *clntId, const char *typeName);

#endif // REGISTERINFOBASE_H
