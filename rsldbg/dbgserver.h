#ifndef DBGSERVER_H
#define DBGSERVER_H

#include "dbgserverproto.h"
#include "lf/bp_data.h"
#include "server/dbgserverbase.h"
#include <string>
#include <map>

class CDebug;
class DbgServer : public DbgServerBase
{
public:
    DbgServer();
    virtual ~DbgServer();

    CDebug *m_curdbg;

    void UpdateDbgInfo(TBpData *data);

    void setIsNewDebug(const bool &v);

    void sendEventBreakPoint(Qt::HANDLE BpData);

protected:
    virtual bool startApp() override;
    virtual bool serverActionEvent(ServerActionEvent *e) override;
    virtual bool serverEvent(DbgServerEvent *e) override;
    virtual void clientDisconnected() override;

private:
    // namespace процедуры по стеку (THIS Object): сырые байты типа (ANSI)
    std::map<void*, std::string> m_ProcNamespace;

    int RslGetModuleLine(Qt::HANDLE module, int offs, int len);

    // filename в UTF-8, encode - имя кодировки файла ("IBM 866" / пусто=UTF-8).
    std::string ReadTextFileContent(const std::string &filename, const std::string &encode);
    void UpdateText(const int &index);
    void UpdateStack();
    void UpdateWatch();
    void UpdateDbgInfo(const int &index);
    void UpdateVariables(const int &index);
    void ShowVariables(const int &index);
    void ShowVariableValue(const long long &val, const long long &info);
    std::string m_LastError;
    RSLSTACK prevStack;
    bool newDbg;

    Qt::HANDLE m_curModuleInView;

    // Кэш текста модуля: UPDATETEXT идёт на каждом шаге, а файл .mac
    // во время отладки не меняется - поле "text" шлём только при смене
    // модуля (с флагом "textChanged"), клиент держит свой кэш.
    std::string m_TextCacheFile;     // UTF-8
    std::string m_TextCacheContent;  // UTF-8

    // Пакетный шаг (EXECCONTNUE с "skip_same_line"): повторяем команду
    // автоматически, не уведомляя клиента, пока не сменится (модуль,
    // строка) стартовой точки или не кончится лимит шагов. Режим только
    // для step_over/step_into.
    int   m_SkipCmd;        // 0 = неактивно, иначе EXECCONTNUE_*
    int   m_SkipLeft;       // оставшиеся авто-шаги
    void *m_SkipMod;        // модуль точки старта
    int   m_SkipLine;       // строка точки старта
    bool  m_SkipSwallowBp;  // проглотить BreakPointEvent авто-шага

    void *m_ProcHandle; // HANDLE запущенного rsldbgclnt.exe (или nullptr)
};

#endif // DBGSERVER_H
