// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef RSLLISTWRAPPER_H
#define RSLLISTWRAPPER_H

#include <QObject>
#include "playrep.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "rsl/objintf.hpp"
#include "rsl/krnlintf.h"
#include "conintf.hpp"
#include "statvars.h"
#include "rslexecutor.h"
#include "registerinfobase.h"

class TRslListWrapperInteface
{
public:
    virtual void clear() = 0;
    virtual int set(const int &index, const QVariant &v) = 0;
    virtual int get(const int &index, QVariant *v) = 0;
    virtual int size() = 0;
    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void setFromPtr(void *) = 0;
};

typedef struct TRslListItemContainer
{
    TGenObject derived;

    unsigned short refCount;

    void *wrp;

    QVector<char*> m_Names;
}TRslListItemContainer;

typedef struct TRslListWrapper
{
    TGenObject derived;
    TGenObject *items;

    unsigned short  generation;     // Used to collect cyclic objects
    unsigned short  marknumber;     // Used to report references

    /*TRslListWrapper(TRslListWrapperInteface *item);
    virtual ~TRslListWrapper();*/

    TRslListWrapperInteface *interface;
    RegisterInfoBase::QObjectRslOwner owner;

    unsigned short refCount;
}TRslListWrapper;

template<class Type>
class TRslTemplateListWrapper : public TRslListWrapperInteface
{
public:
    TRslTemplateListWrapper() :
        TRslListWrapperInteface()
    {
        lst = nullptr;
    }

    void clear() Q_DECL_OVERRIDE
    {
        if (!lst)
            return;

        lst->clear();
    }

    int size() Q_DECL_OVERRIDE
    {
        if (!lst)
            return 0;

        return lst->size();
    }

    int set(const int &index, const QVariant &v) Q_DECL_OVERRIDE
    {
        if (!lst)
            return 1;

        if (!v.isValid())
            return 1;
        //QVariant val = SetFromRslValue(v);
        if (index >= lst->size())
            lst->append(v.value<Type>());
        else
            (*lst)[index] = v.value<Type>();

        return 0;
    }

    int get(const int &index, QVariant *v) Q_DECL_OVERRIDE
    {
        if (!lst)
            return 1;

        if (index < 0 || index >= lst->size())
            return 1;

        *v = QVariant::fromValue<Type>(lst->at(index));

        return 0;
    }

    void init()
    {
        lst = new QList<Type>();
    }

    void deinit()
    {
        delete lst;
    }

    void setFromPtr(void *ptr)
    {
        lst = (QList<Type>*)ptr;
    }

    QList<Type> *lst;
};

void DoneListItemContainer(TGenObject *o);
void DoneRslList(TGenObject *o);
/*class TRslStringListWrapper : public TRslListWrapper
{
public:
    TRslStringListWrapper();

    void clear() Q_DECL_OVERRIDE;
    int size() Q_DECL_OVERRIDE;

    QStringList *lst;
};*/


#endif // RSLLISTWRAPPER_H
