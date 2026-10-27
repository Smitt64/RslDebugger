#ifndef MCPSERVER_H
#define MCPSERVER_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QStringList>

class McpDbgClient;

// Читает stdin построчно в отдельном потоке: блокирующий readLine
// (QSocketNotifier с консольными хендлами Windows не дружит). Строки
// доставляются в главный поток queued-сигналом.
class StdinReader : public QThread
{
    Q_OBJECT

public:
    using QThread::QThread;

signals:
    void lineReady(const QString &line);
    void eof();

protected:
    void run() Q_DECL_OVERRIDE;
};

// MCP-сервер (JSON-RPC 2.0 поверх stdio, построчный JSON).
// В stdout уходят ТОЛЬКО JSON-RPC сообщения; логирование - в stderr.
class McpServer : public QObject
{
    Q_OBJECT

public:
    explicit McpServer(QObject *parent = nullptr);
    ~McpServer() Q_DECL_OVERRIDE;

private slots:
    void onLine(const QString &line);
    void onEof();

private:
    void handleLine(const QString &line);
    void send(const QJsonObject &msg);
    void sendError(const QJsonValue &id, int code, const QString &message);

    void process(const QString &method, const QJsonObject &params,
                 QJsonObject *result, QJsonObject *error);

    QJsonObject callTool(const QString &name, const QJsonObject &args);
    QJsonArray toolsList() const;

    static QJsonObject toolText(const QJsonDocument &doc, bool isError = false);
    static QJsonObject toolError(const QString &message);

    QFile m_Out;
    StdinReader *m_Reader;
    McpDbgClient *m_Dbg;

    // Запросы обрабатываются строго по одному: wait_event и др. крутят
    // вложенный QEventLoop, и без этой очереди queued lineReady выполнялись
    // бы реентерабельно (disconnect мог сработать посреди wait_event).
    bool m_Busy = false;
    QStringList m_Pending;
    // eof от StdinReader во время обработки запроса: qApp->quit() выставляет
    // quitNow и мгновенно прерывает вложенные QEventLoop'ы (wait_event),
    // поэтому выход откладываем до конца обработки очереди.
    bool m_Eof = false;
};

#endif // MCPSERVER_H
