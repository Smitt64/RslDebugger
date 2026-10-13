#include "cdebugroot.h"
#include <QMessageBox>
#include <QApplication>
#include "dbg.h"
#include "convert.h"
#include <QTextCodec>
#include <QDebug>

void GetLastErrorText (CDebugRoot* dbgRoot, QByteArray* str, const QByteArray& def)
{
    char buff [ERROR_TEXT_MAX_LENGTH] = {0};

    if (dbgRoot->do_GetLastErrorText (buff, ERROR_TEXT_MAX_LENGTH))
        *str = buff;
    else
        *str = def;
}

CDebugRoot::CDebugRoot(QObject *parent)
    : QObject{parent},
    m_dbgftable(nullptr),
    m_dbgintfMajorVer(0),
    m_mode(TRUE),
    childsLimit(0)
{
    oem866 = QTextCodec::codecForName("IBM 866");
}

CDebugRoot::~CDebugRoot()
{

}

QString CDebugRoot::toUnicode(const char *str)
{
    if (NeedConvert())
        return oem866->toUnicode(str);

    return QString::fromLocal8Bit(str);
}

void CDebugRoot::SetChildsLimit (DWORD childsLimit_)
{
    childsLimit = childsLimit_;
}

void CDebugRoot::SetExternalMode(int mode)
{
    m_mode = mode;
}

BOOL CDebugRoot::NeedConvert()
{
    return m_mode;
}

bool CDebugRoot::CheckVersion (int needVersion, bool showDlg)
{
    if (m_dbgintfMajorVer < needVersion)
    {
        if (showDlg)
        {
            QString str = QString("Requaed function needs RSL DBG interface version %1").arg(needVersion);
            QMessageBox::critical(QApplication::activeWindow(), "Error", str);
        }
        return false;
    }
    return true;
}

bool CDebugRoot::do_EnumStack(	RSLSTACK st, RSLPROC *prc, RSLMODULE *mod, RSLSTMT *stmt,
                              int *offs,int *len, char *procName, int szProcName, char *modName, int szModName,
                              RSLSTACK* rv){
    CHECK_FTABLE(EnumStack);

    try
    {
        *rv = m_dbgftable->EnumStack(m_inst, st, prc, mod, stmt, offs, len, procName, szProcName, modName,szModName);
        if(NeedConvert())
        {
            Dos2Win(procName);
            Dos2Win(modName);
        }
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_EnumModule(RSLMODULE hmod, char *modName, int szModName, int* modtype, RSLMODULE* rv)
{
    CHECK_FTABLE(EnumModule);

    try
    {
        *rv = m_dbgftable->EnumModule(m_inst, hmod, modName, szModName, modtype);
        if(NeedConvert())
            Dos2Win(modName);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_GetStatementOfPos(RSLMODULE hmod, int offs,int len, int *realOffs,int *realLen, RSLSTMT* rv, int* line)
{
    CHECK_FTABLE(GetStatementOfPos);

    try
    {
        *rv = m_dbgftable->GetStatementOfPos(m_inst, hmod, offs, len, realOffs, realLen, line);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_BreakPointAt(RSLSTMT stmt,uint32 data)
{
    CHECK_FTABLE(BreakPointAt);

    try
    {
        m_dbgftable->BreakPointAt(m_inst, stmt, data);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_ClrBreakPoint(RSLSTMT stmt)
{
    CHECK_FTABLE(ClrBreakPoint);

    try
    {
        m_dbgftable->ClrBreakPoint(m_inst, stmt);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_ParseExp(RSLPROC proc, const char *txt, int* isLval, RSLEXPCTX* rv)
{
    CHECK_FTABLE(ParseExp);
    // !!!!!!!!!! CString text (txt);
    try
    {
        /*if(NeedConvert()){
            Win2Dos(&text);
        }
        *rv = m_dbgftable->ParseExp(m_inst, proc, text, isLval);
        */
        *rv = m_dbgftable->ParseExp(m_inst, proc, txt, isLval);

        // *********** Test code *************
        //      char buff [512];
        //      m_dbgftable->GetLastErrorText (m_inst, buff, sizeof buff);
        //	     if (NeedConvert ())
        //         Dos2Win (buff);
        // *********** Test code *************
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_RemExp(RSLEXPCTX exp)
{
    CHECK_FTABLE(RemExp);

    try
    {
        m_dbgftable->RemExp(m_inst, exp);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_ExecExpAt(RSLEXPCTX exp,RSLSTACK st,int *isObject,
                              char *tpName, int szTpName, char *value, int szValue,
                              RSLVALUE* rv)
{
    CHECK_FTABLE(ExecExpAt);

    try
    {
        *rv = m_dbgftable->ExecExpAt(m_inst, exp, st, isObject, tpName, szTpName, value, szValue);

        if(NeedConvert())
        {
            Dos2Win(tpName);
            Dos2Win(value);
        }
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_GetFirstLocalInfo(RSLSTACK hst,RSLVALUE *val,int *isObject,
                                      char *name, int szName, char *tpName, int szTpName,
                                      char *value, int szValue,  RSLVINFO* rv)
{
    CHECK_FTABLE(GetFirstLocalInfo);

    try
    {
        *rv = m_dbgftable->GetFirstLocalInfo(m_inst, hst, val, isObject, name, szName, tpName, szTpName, value, szValue);

        if(NeedConvert())
        {
            Dos2Win(name);
            Dos2Win(tpName);
            Dos2Win(value);
        }
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_GetFirstPropInfo(RSLVALUE hv,RSLVALUE *val,int *isObject,
                                     char *name, int szName, char *tpName, int szTpName,
                                     char *value, int szValue, RSLVINFO* rv)
{
    CHECK_FTABLE(GetFirstPropInfo);

    try
    {
        *rv = m_dbgftable->GetFirstPropInfo(m_inst,hv,val,isObject,name,szName,tpName,szTpName,value,szValue);

        if(NeedConvert())
        {
            Dos2Win(name);
            Dos2Win(tpName);
            Dos2Win(value);
        }
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_GetNextInfo (RSLVINFO hst, RSLVALUE *val, int *isObject,
                                char *name, int szName, char *tpName, int szTpName,
                                char *value, int szValue, RSLVINFO* rv)
{
    CHECK_FTABLE(GetNextInfo);

    try
    {
        RSLVINFO ret = m_dbgftable->GetNextInfo(m_inst, hst, val, isObject,
                                                name, szName, tpName, szTpName,
                                                value, szValue);

        std::map<RSLVINFO, unsigned>::iterator it = curChilds.find (hst);
        if (it != curChilds.end ())
        {
            if (it->second == childsLimit)
            {
                do_DoneInfo (ret);
                ret = 0;
            }
            else
                curChilds.insert (std::make_pair (ret, it->second + 1));
        }

        *rv = ret;
        if(NeedConvert ())
        {
            Dos2Win (name);
            Dos2Win (tpName);
            Dos2Win (value);
        }
    }
    catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_RefreshInfo(RSLVINFO hinfo,RSLVALUE *val,int *isObject,
                                char *name, int szName, char *tpName, int szTpName, char *value, int szValue)
{
    CHECK_FTABLE(RefreshInfo);

    try
    {
        m_dbgftable->RefreshInfo(m_inst, hinfo, val, isObject, name, szName, tpName, szTpName, value, szValue);

        if(NeedConvert())
        {
            Dos2Win(name);
            Dos2Win(tpName);
            Dos2Win(value);
        }
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_DoneInfo(RSLVINFO hinfo)
{
    static int count = 1;
    CHECK_FTABLE(DoneInfo);

    try
    {
        qDebug() << "do_DoneInfo" << count++;
        curChilds.erase (hinfo);
        m_dbgftable->DoneInfo(m_inst, hinfo);
    }
    catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_SetNewInfo(RSLVINFO hinfo,RSLSTACK st, const char *txt, bool* rv)
{
    CHECK_FTABLE(SetNewInfo);

    //CString text (txt); !!!!!!!!!!!!!!!!!!!!!!!!
    try
    {
        /*if(NeedConvert())
        {
            Win2Dos(&text);
        }*/
        *rv = m_dbgftable->SetNewInfo(m_inst, hinfo, st, txt);
    } catch(...){
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_OpenSrc(RSLMODULE hmod, RSLSRC* rv, int* encode)
{
    CHECK_FTABLE(OpenSrc);

    try
    {
        if (m_dbgftable->verHi > 3)
            *rv = m_dbgftable->OpenSrcEncode (m_inst, hmod, NULL, NULL, encode);
        else
            *rv = m_dbgftable->OpenSrc (m_inst, hmod, NULL, NULL);
    }
    catch (...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_ReadSrc(RSLSRC hsrc,char *buff, int size, int* rv)
{
    CHECK_FTABLE(ReadSrc);

    try
    {
        *rv = m_dbgftable->ReadSrc(m_inst, hsrc, buff, size);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_GetSizeSrc(RSLSRC hsrc, int* rv)
{
    CHECK_FTABLE(GetSizeSrc);

    try
    {
        *rv = m_dbgftable->GetSizeSrc(m_inst, hsrc);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_CloseSrc(RSLSRC hsrc)
{
    CHECK_FTABLE(CloseSrc);

    try
    {
        m_dbgftable->CloseSrc(m_inst, hsrc);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_Interrupt()
{
    CHECK_FTABLE(Interrupt);

    try
    {
        m_dbgftable->Interrupt(m_inst);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_ExecContinue(int traceFlag)
{
    CHECK_FTABLE(ExecContinue);

    try
    {
        m_dbgftable->ExecContinue(m_inst, traceFlag);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_GetAppWnd(HWND* rv)
{
    CHECK_FTABLE(GetAppWnd);

    try
    {
        m_dbgftable->GetAppWnd(m_inst, rv);

    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_SetDbgFlag(int newFlag, int* rv)
{
    CHECK_FTABLE(SetDbgFlag);

    try
    {
        *rv = m_dbgftable->SetDbgFlag(m_inst, newFlag);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_GetVersion(short int *verLo, int* rv)
{
    CHECK_FTABLE(GetVersion);

    try
    {
        *rv = m_dbgftable->GetVersion(m_inst, verLo);
    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_SetExpValue(RSLEXPCTX exp,RSLSTACK st,	const char* txt, bool* rv)
{
    CHECK_FTABLE(SetExpValue);

    // CString text (txt); !!!!!!!!!!!!!!!
    try
    {
        /*if(NeedConvert()){
            Win2Dos(&text);
        }*/
        *rv = m_dbgftable->SetExpValue(m_inst, exp, st, txt);

    } catch(...)
    {
        m_error = ERR_UNKNOWN;
        return false;
    }

    return true;
}

bool CDebugRoot::do_GetValueSize (RSLVINFO hinfo, int* size)
{
    bool retval = true;
    if (!CheckVersion (5, false))
        return false;

    try
    {
        m_dbgftable->GetValueSize (m_inst, hinfo, size);
    }
    catch (...)
    {
        retval = false;
    }

    return retval;
}

bool CDebugRoot::do_GetValueData (RSLVINFO hinfo, char* valBuff, int size, int offs, int* outSz)
{
    bool retval = true;
    if (!CheckVersion (5, false))
        return false;

    try
    {
        m_dbgftable->GetValueData (m_inst, hinfo, valBuff, size, offs, outSz);

        if (NeedConvert ())
            Dos2Win (valBuff);
    }
    catch (...)
    {
        retval = false;
    }

    return retval;
}

bool CDebugRoot::do_SetValueData (RSLVINFO hinfo, int phase, const char* valBuff, int size)
{
    bool retval = true;
    if (!CheckVersion (5, false))
        return false;

    try
    {
        //CString text (valBuff); !!!!!!!!

        //if (NeedConvert ())
        //    Win2Dos (&text);

        m_dbgftable->SetValueData (m_inst, hinfo, phase, valBuff, size);
    }
    catch (...)
    {
        retval = false;
    }

    return retval;
}

bool CDebugRoot::do_GetExpSize (RSLEXPCTX hexp, int* size)
{
    bool retval = true;
    if (!CheckVersion (5, false))
        return false;

    try
    {
        m_dbgftable->GetExpSize (m_inst, hexp, size);
    }
    catch (...)
    {
        retval = false;
    }

    return retval;
}

bool CDebugRoot::do_GetExpData (RSLEXPCTX hexp, char* valBuff, int size, int offs, int* outSz)
{
    bool retval = true;
    if (!CheckVersion (5, false))
        return false;

    try
    {
        m_dbgftable->GetExpData (m_inst, hexp, valBuff, size, offs, outSz);

        if (NeedConvert ())
            Dos2Win (valBuff);
    }
    catch (...)
    {
        retval = false;
    }

    return retval;
}

bool CDebugRoot::do_SetExpData (RSLEXPCTX hexp, int phase, const char* valBuff, int size)
{
    bool retval = true;
    if (!CheckVersion (5, false))
        return false;

    try
    {
        // CString text (valBuff); !!!!!!!

        //if (NeedConvert ())
        //    Win2Dos (&text);

        m_dbgftable->SetExpData (m_inst, hexp, phase, valBuff, size);
    }
    catch (...)
    {
        retval = false;
    }

    return retval;
}

RSLVINFO CDebugRoot::do_GetFirstArrayInfo (RSLVALUE hv, RSLVALUE *val,int *isObject,
                                          char *name, int szName, char *tpName, int szTpName,
                                          char *value, int szValue)
{
    if (!CheckVersion (6, false) || !childsLimit)
        return 0;

    RSLVINFO ret = 0;

    try
    {
        ret = m_dbgftable->GetFirstArrayInfo (m_inst, hv, val, isObject, name, szName,
                                             tpName, szTpName, value, szValue);
        if (ret)
            curChilds.insert (std::make_pair (ret, 1));

        if(NeedConvert ())
        {
            Dos2Win (name);
            Dos2Win (tpName);
            Dos2Win (value);
        }
    }
    catch (...)
    {
    }

    return ret;
}

bool CDebugRoot::do_GetLastErrorText (char* errBuff, int szBuff)
{
    bool retval = false;
    if (!CheckVersion (7, false))
        return retval;

    try
    {
        m_dbgftable->GetLastErrorText (m_inst, errBuff, szBuff);

        if (*errBuff)
        {
            retval = true;

            if (NeedConvert())
                Dos2Win (errBuff);
        }
    }
    catch (...)
    {
    }

    return retval;
}

bool CDebugRoot::do_ExecExpInteractive (const char *txt)
{
    bool retval = false;

    if (!CheckVersion (8, false))
        return retval;

    try
    {
        //CString text (txt); !!!!!!

        //if (NeedConvert())
            //Win2Dos (&text);

        retval = m_dbgftable->ExecExpInteractive (m_inst, txt);
    }
    catch (...)
    {
        retval = false;
    }

    return retval;
}

unsigned short CDebugRoot::do_GetExpInteractiveStatus()
{
    unsigned short retval = INTR_EXP_NOEXP;
    if (!CheckVersion (8))
        return INTR_EXP_NOEXP;

    try
    {
        retval = m_dbgftable->GetExpInteractiveStatus (m_inst);
    }
    catch (...)
    {
    }

    return retval;
}
