#ifndef CDEBUG_H
#define CDEBUG_H

#include "cdebugroot.h"
#include "lf/bp_data.h"
#include "lf/types.h"
#include "cwatchv.h"
#include <QEvent>

#define	MSG_BREAKPOINT (QEvent::User + 1)
#define MSG_FINISH     (QEvent::User + 2)
#define MSG_TRACE      (QEvent::User + 3)
#define MSG_EDITWATCH  (QEvent::User + 4)

typedef struct
{
    int offs;
    int reallength;
    bool isEnabled;
}BPinfo;

bool operator== (const BPinfo& inf1, const BPinfo& inf2);
std::ostream& operator<< (std::ostream& oc, const BPinfo& BPi);
std::istream& operator>> (std::istream& ic, BPinfo& BPi);

// ------------------------------------------------------------------

typedef struct
{
    std::string expStr;
    std::string contextStr;
    std::string typeStr;
    unsigned int depth;
    bool isObject;
    bool isExpanded;
}ExpInfo;

bool operator== (const ExpInfo& inf1, const ExpInfo& inf2);
std::ostream& operator<< (std::ostream& oc, const ExpInfo& inf);
std::istream& operator>> (std::istream& ic, ExpInfo& inf);

// ------------------------------------------------------------------

class CSurvey;
class CQSurvey;
class DisplayVar;
class CLocals;
class Saver;
class CDebug : public CDebugRoot
{
    Q_OBJECT
public:
    CDebug(HRD inst, TDbgIntf*  dbgftable, QObject *parent = nullptr);
    virtual ~CDebug();

    void GetRemoteVersion();
    bool GetDebugState();
    void SetDebugState(bool state);

    int m_nVerHi;
    short int m_nVerLo;
    static const QString na;
    static const QString expanding;
    bool inInteractiveExp;

    iter_bpdata FindBP(TBpData*);
    TBpData* FindBPbyKey (unsigned long key);
    bool AddBp(TBpData*);
    bool DelBp(TBpData*);
    bool DisBp(TBpData*);
    CBPData* GetBP();

    RSLPROC GetCurProc(int index = -1);
    RSLSTACK GetCurStack(int index = -1);
    CStackInfo* GetStackInfo(void);
    int GetStackSize();
    CSurvey* GetSurvey(void);
    CLocals* GetLocals();
    CQSurvey* GetQSurvey();
    Saver *GetSaver();

    int FindStackIndex(RSLSTACK st);
    int GetStackCount();

    iter_watch FindSWatch(const QString &str, RSLSTACK st, RSLPROC proc);
    iter_watch FindQWatch (const QString &str, RSLSTACK st, RSLPROC proc);
    iter_watch FindLocal(const QString &str, RSLSTACK st, RSLPROC proc);
    DisplayVar* FindRootItem(DisplayVar* var, BOOL surv);
    RSLMODULE GetCurModule(int index = -1);
    RSLSTMT GetCurStatement(int* offs, int* len, int index = -1);

    int GetWatchVarPos(DisplayVar* var);
    int GetLocalVarPos(DisplayVar* var);
    QString GetProcName (RSLSTACK st);
    QString GetCurModuleName(int index = -1);
    RSLSTACK GetStackAt(int index);
    int GetIndex();
    void SetIndex (int index = 0);
    void UpdateDbgInfo();
    void UpdateVariables();
    void UpdateSurvey();
    void UpdateQSurvey();
    void ClearSurvey();
    void EraseQSurvey();
    void ClearQSurvey();

    void SetCurModule(RSLMODULE mod);
    RSLMODULE GetActiveModule(void);

    bool AddSWatch(QByteArray str, RSLSTACK st, RSLPROC proc, QByteArray curprocname, RSLMODULE curmodule, bool isLvalue);
    bool DelSwatch(DisplayVar *var);
    bool AddQWatch(QByteArray str, RSLSTACK st, RSLPROC proc, QByteArray curprocname, RSLMODULE curmodule);
    bool DelQWatch(QString);

    BP_TYPE GetBPType(int index);
    int GetBPOffs(int index);
    int GetBPLen(int index);
    int GetBPCount();
    TBpData* GetBP(int i);
    iter_bpdata GetBPEnd();

    void SetInst(HRD inst);
    TDbgIntf* GetDbgFTable();
    HRD GetInst();
    QString GetError();
    int GetStatementLine(int offs, int len, RSLMODULE mod);
    //int GetRealIndex(int _virtual, WNDENUM wnd);

private:
    CStackInfo m_stackinfo;
    QScopedPointer<CSurvey> m_survey;
    QScopedPointer<CQSurvey> m_qsurvey;
    QScopedPointer<CLocals> m_locals;
    QScopedPointer<Saver> m_dataSaver;
    int m_index;
    CBPData m_bpdata;
    unsigned long bpKey;

    RSLMODULE m_curmod;
    RSLMODULE m_actmod; //active module, which is being debugged
    bool m_isDebugging;
};

#endif // CDEBUG_H
