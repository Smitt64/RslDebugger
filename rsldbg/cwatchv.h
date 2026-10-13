#ifndef CWATCHV_H
#define CWATCHV_H

//#include "cdebug.h"
#include "displayvar.h"
#include <loki/SmartPtr.h>
#include <vector>

typedef Loki::SmartPtr<DisplayVar, Loki::RefLinked> SpVarPtrType;
#define SpIter std::vector<SpVarPtrType>::iterator

class CDebug;
class CWatchV : private std::vector<SpVarPtrType>
{
    friend class CWatch;
    friend class CDebug;
public:
    using std::vector<SpVarPtrType>::iterator;
    using std::vector<SpVarPtrType>::begin;
    using std::vector<SpVarPtrType>::end;
    using std::vector<SpVarPtrType>::size;
    using std::vector<SpVarPtrType>::push_back;
    using std::vector<SpVarPtrType>::clear;
    using std::vector<SpVarPtrType>::erase;
    using std::vector<SpVarPtrType>::insert;
    using std::vector<SpVarPtrType>::at;
    using std::vector<SpVarPtrType>::empty;

    CWatchV(CDebug*	pParent);
    ~CWatchV();
    void operator = (std::vector<DisplayVar*>* arg);

    bool AddWatchV (const QByteArray &str);
    bool DelWatchV(int index);
    bool ExpandV(int index, RSLSTACK _st = nullptr);
    bool CollapseV(int);

protected:
    DisplayVar* CreateFakeChildrensItem (RSLVALUE value, int curDepth);
    virtual void NotifyCollapsing (iterator it) {};

    CDebug*	m_parent;
    using std::vector<SpVarPtrType>::swap;

private:
    SpIter FindWatchV (const QString &str);
};

typedef CWatchV::iterator iter_watch;

#endif // CWATCHV_H
