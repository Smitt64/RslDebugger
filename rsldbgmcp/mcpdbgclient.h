#ifndef MCPDBGCLIENT_H
#define MCPDBGCLIENT_H

#include "dbgclient.h"
#include <QJsonArray>
#include <QJsonObject>

class QEventLoop;

// DbgClient для MCP-сервера: накапливает последнее состояние отладчика
// (событие, текст модуля, стек, локальные переменные, брейкпоинты) и даёт
// блокирующие ожидания - MCP поверх stdio является синхронным
// request/response, поэтому tool-вызовы ждут ответ отладчика напрямую.
class McpDbgClient : public DbgClient
{
    Q_OBJECT

public:
    explicit McpDbgClient(QObject *parent = nullptr);

    // Ждёт событие отладчика (останов на брейкпоинте/шаге, завершение,
    // разрыв соединения). Возвращает снапшот состояния.
    QJsonObject waitEvent(int timeoutMs);

    // Текущее состояние без ожидания. frames по умолчанию включены;
    // waitEvent их не отдаёт (для стека есть отдельный rsl_stack),
    // только счётчик frames_count.
    QJsonObject snapshot(bool withSourceText, bool withFrames = true) const;

    // Вызывается при отправке execContinue: текущий останов "отыгран",
    // следующий waitEvent должен ждать НОВОГО события. Без этого повторный
    // waitEvent на уже стоящем отладчике сгорал бы впустую до таймаута.
    void notifyContinueSent() { m_EventPending = false; }

    QJsonObject lastEvent() const { return m_Event; }
    QJsonArray breakpoints() const { return m_Bps; }
    QJsonArray stackFrames() const { return m_Frames; }
    QJsonArray locals() const { return m_Vars; }
    QJsonObject source() const { return m_Text; }

    // expandVariable + ожидание обновления списка переменных.
    bool expandVariableAndWait(int index, qint64 stack, int stackIndex, int timeoutMs);

    // showVarValue + ожидание ответа с именем/типом/значением.
    QJsonObject valueInfoAndWait(qint64 val, qint64 info, int timeoutMs);

protected:
    void onEvent(const QJsonObject &obj) Q_DECL_OVERRIDE;
    void onUpdateText(const QJsonObject &obj) Q_DECL_OVERRIDE;
    void onUpdateBreakpoints(const QJsonObject &obj) Q_DECL_OVERRIDE;
    void onUpdateStack(const QJsonObject &obj) Q_DECL_OVERRIDE;
    void onUpdateLocals(const QJsonObject &obj) Q_DECL_OVERRIDE;
    void onValueInfo(const QJsonObject &obj) Q_DECL_OVERRIDE;

private:
    bool waitFlag(volatile bool &flag, int timeoutMs);
    void wake();

    QEventLoop *m_WaitLoop = nullptr;

    volatile bool m_FlagEvent = false;
    volatile bool m_EventPending = false; // есть неотыгранный останов
    volatile bool m_FlagLocals = false;
    volatile bool m_FlagValue = false;

    QJsonObject m_Event;
    QJsonObject m_Text;
    QJsonObject m_ValueInfo;
    QJsonArray m_Bps;
    QJsonArray m_Frames;
    QJsonArray m_Vars;
};

#endif // MCPDBGCLIENT_H
