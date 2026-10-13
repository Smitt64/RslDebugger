#ifndef CDEBUGROOT_H
#define CDEBUGROOT_H

#include <Windows.h>
#include <QObject>
#define _INC_WINDOWS
#include "rsl/dbgintf.h"

const unsigned ERROR_TEXT_MAX_LENGTH = 4096 * 10;

class CDebugRoot : public QObject
{
    Q_OBJECT
public:
    CDebugRoot(QObject *parent = nullptr);
    virtual ~CDebugRoot();

    QString toUnicode(const char *str);

    void SetChildsLimit (DWORD childsLimit_);
    void SetExternalMode(int mode);
    BOOL NeedConvert();
    bool  CheckVersion(int needVersion, bool showDlg = true);

    bool do_EnumStack(RSLSTACK, RSLPROC*, RSLMODULE*, RSLSTMT*,int*, int*, char*, int, char*, int, RSLSTACK*);
    bool do_EnumModule(RSLMODULE, char*, int, int*, RSLMODULE*);
    bool do_GetStatementOfPos(RSLMODULE, int, int, int*, int*, RSLSTMT*, int* line);
    bool do_BreakPointAt(RSLSTMT, uint32);
    bool do_ClrBreakPoint(RSLSTMT);
    bool do_ParseExp(RSLPROC, const char*, int*, RSLEXPCTX*);
    bool do_RemExp(RSLEXPCTX);
    bool do_ExecExpAt(RSLEXPCTX, RSLSTACK, int*, char*, int, char*, int, RSLVALUE*);
    bool do_GetFirstLocalInfo(RSLSTACK, RSLVALUE*, int*, char*, int, char*, int, char*, int, RSLVINFO*);
    bool do_GetFirstPropInfo(RSLVALUE, RSLVALUE*, int*, char*, int, char*, int, char*, int, RSLVINFO*);
    bool do_GetNextInfo(RSLVINFO, RSLVALUE*, int*, char*, int, char*, int, char*, int, RSLVINFO*);
    bool do_RefreshInfo(RSLVINFO, RSLVALUE*, int*, char*, int, char*, int, char*, int);
    bool do_DoneInfo(RSLVINFO);
    bool do_SetNewInfo(RSLVINFO, RSLSTACK, const char*, bool*);
    bool do_OpenSrc(RSLMODULE, RSLSRC*, int* encode);
    bool do_ReadSrc(RSLSRC, char*, int, int*);
    bool do_GetSizeSrc(RSLSRC, int*);
    bool do_CloseSrc(RSLSRC);
    bool do_Interrupt();
    bool do_ExecContinue(int);
    bool do_GetAppWnd(HWND*);
    bool do_SetDbgFlag(int, int*);
    bool do_GetVersion(short int*, int*);
    bool do_SetExpValue(RSLEXPCTX, RSLSTACK, const char*, bool*);
    bool do_GetValueSize (RSLVINFO hinfo, int* size);
    bool do_GetValueData (RSLVINFO hinfo, char* valBuff, int size, int offs, int* outSz);
    bool do_SetValueData (RSLVINFO hinfo, int phase, const char* valBuff, int size);
    bool do_GetExpSize (RSLEXPCTX hexp, int* size);
    bool do_GetExpData (RSLEXPCTX hexp, char* valBuff, int size, int offs, int* outSz);
    bool do_SetExpData (RSLEXPCTX hexp, int phase, const char* valBuff, int size);
    RSLVINFO do_GetFirstArrayInfo (RSLVALUE hv, RSLVALUE *val,int *isObject, char *name, int szName, char *tpName, int szTpName, char *value, int szValue);
    bool do_GetLastErrorText (char* errBuff, int szBuff);
    bool do_ExecExpInteractive (const char *txt);
    unsigned short do_GetExpInteractiveStatus ();

protected:
    TDbgIntf*	m_dbgftable;
    int         m_dbgintfMajorVer;
    QString		m_error;
    HRD			m_inst;

private:
    QTextCodec *oem866;
    int m_mode;
    DWORD childsLimit;
    std::map<RSLVINFO, unsigned> curChilds;
};

void GetLastErrorText (CDebugRoot* dbgRoot, QByteArray* str, const QByteArray& def);

#endif // CDEBUGROOT_H
