#include "mcpdbgclient.h"
#include <QEventLoop>
#include <QElapsedTimer>
#include <QTimer>

McpDbgClient::McpDbgClient(QObject *parent)
    : DbgClient{parent}
{
    // Разрыв соединения тоже будит ожидающие tool-вызовы.
    QObject::connect(this, &DbgClient::disconnected, this, [this]
    {
        m_Event = QJsonObject{{QStringLiteral("event"), QStringLiteral("disconnected")}};
        m_FlagEvent = true;
        wake();
    });
}

void McpDbgClient::wake()
{
    if (m_WaitLoop && m_WaitLoop->isRunning())
        m_WaitLoop->quit();
}

bool McpDbgClient::waitFlag(volatile bool &flag, int timeoutMs)
{
    flag = false;

    QElapsedTimer timer;
    timer.start();

    QEventLoop loop;
    m_WaitLoop = &loop;

    // wake() общий на все флаги: onUpdateLocals и др. гасят цикл, не взводя
    // наш флаг. Перезаходим в цикл, пока не взведён именно наш флаг или
    // не истёк общий таймаут.
    while (!flag && timer.elapsed() < timeoutMs)
    {
        QTimer::singleShot(timeoutMs - (int)timer.elapsed(), &loop, &QEventLoop::quit);
        loop.exec();
    }

    m_WaitLoop = nullptr;

    return flag;
}

QJsonObject McpDbgClient::waitEvent(int timeoutMs)
{
    waitFlag(m_FlagEvent, timeoutMs);
    return snapshot(false);
}

QJsonObject McpDbgClient::snapshot(bool withSourceText) const
{
    QJsonObject src = m_Text;
    if (!withSourceText)
        src.remove(QStringLiteral("text")); // полный исходник - только по запросу

    QJsonObject out;
    out.insert(QStringLiteral("connected"), isConnected());
    out.insert(QStringLiteral("event"), m_Event);
    out.insert(QStringLiteral("source"), src);
    out.insert(QStringLiteral("frames"), m_Frames);
    out.insert(QStringLiteral("vars"), m_Vars);
    return out;
}

bool McpDbgClient::expandVariableAndWait(int index, qint64 stack, int stackIndex, int timeoutMs)
{
    expandVariable(index, stack, stackIndex);
    return waitFlag(m_FlagLocals, timeoutMs);
}

QJsonObject McpDbgClient::valueInfoAndWait(qint64 val, qint64 info, int timeoutMs)
{
    showVarValue(val, info);
    waitFlag(m_FlagValue, timeoutMs);
    return m_ValueInfo;
}

void McpDbgClient::onEvent(const QJsonObject &obj)
{
    m_Event = obj;
    m_FlagEvent = true;
    wake();
}

void McpDbgClient::onUpdateText(const QJsonObject &obj)
{
    m_Text = obj;
    // НЕ будим wait_event: сервер шлёт состояние пачкой
    // (text/locals/stack), а завершает её кадром EVENT (110). Пробуждение
    // по text дало бы неполный снапшот (event=None, frames=0, vars=0).
}

void McpDbgClient::onUpdateBreakpoints(const QJsonObject &obj)
{
    m_Bps = obj.value(QStringLiteral("bps")).toArray();
}

void McpDbgClient::onUpdateStack(const QJsonObject &obj)
{
    m_Frames = obj.value(QStringLiteral("frames")).toArray();
}

void McpDbgClient::onUpdateLocals(const QJsonObject &obj)
{
    m_Vars = obj.value(QStringLiteral("vars")).toArray();
    m_FlagLocals = true;
    wake();
}

void McpDbgClient::onValueInfo(const QJsonObject &obj)
{
    m_ValueInfo = obj;
    m_FlagValue = true;
    wake();
}
