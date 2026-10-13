#ifndef DBG_H
#define DBG_H

#define ERR_OK          "OK"
#define ERR_NOTIMPL     "Func. not implemented"
#define ERR_MEM         "Mem error"
#define ERR_TBLNOTINIT  "Func table not inited, ord pointer was destroyed"
#define ERR_AUXFUNC     "Error while call aux func"
#define ERR_UNKNOWN     "Unknown error"
#define ERR_NULLFPTR    "Function pointer is null"
#define ERR_INST        "Bad instance"

#define CHECK_FTABLE(X) m_error = ERR_OK;\
if ( !m_dbgftable ){\
        m_error = ERR_TBLNOTINIT;\
        return false;\
}\
    if( NULL == m_dbgftable->X){\
        m_error = ERR_NULLFPTR;\
        return false;\
}\
    if( NULL == m_inst ){\
        m_error = ERR_INST;\
}

#endif // DBG_H
