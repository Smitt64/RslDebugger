#ifndef REMEXPRESSIONS_H
#define REMEXPRESSIONS_H

#include "cdebug.h"

class RemExpressions
{
public:
    RemExpressions(CDebug* _dbg);
    void Restore();

private:
    CDebug* dbg;
    std::string procName;
    bool restored;
};

#endif // REMEXPRESSIONS_H
