#ifndef CLOCALS_H
#define CLOCALS_H

#include "cwatchv.h"

class CDebug;
class CLocals : public CWatchV
{
public:
    CLocals(CDebug* pParent);
    virtual ~CLocals();

    bool CollectLocals (const RSLSTACK _st, RSLSTACK* prevSt, bool instChanged);
};

#endif // CLOCALS_H
