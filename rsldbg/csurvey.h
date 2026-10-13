#ifndef CSURVEY_H
#define CSURVEY_H

#include "cwatchv.h"
#include <set>

class CSurvey: public CWatchV
{
public:
    CSurvey(CDebug* pParent);
    virtual ~CSurvey();

    void Update(int m_index);

private:
    void RefreshValue(SpVarPtrType value);
    bool ExecuteExpression (RSLEXPCTX expr);
    bool ParseExpression (SpVarPtrType value);
    void ApplyCurInfoToElem(SpVarPtrType value);
    bool GetFirstChildItem(SpVarPtrType expElem);
    bool GetNextChildItem(SpVarPtrType expElem);
    void UpdateExpandInfo();
    void Expand();
    iterator ExpandElem(iterator it);
    void NotifyCollapsing (iterator it);
    void BuildNameList (iterator it, std::vector<std::string>* dstNameList);

    QByteArray unknownStr;
    QByteArray errorStr;
    QByteArray cannotevalStr;

    int isObject;
    bool isExp;
    char nameStr [MAX_NAME];
    char typeStr [MAX_TYPENAME];
    char valueStr[MAX_VALUE];
    RSLVINFO info;
    RSLVALUE val;
    RSLEXPCTX exp;

    std::set<std::vector<std::string>> ExpandInfo;
};

#endif // CSURVEY_H
