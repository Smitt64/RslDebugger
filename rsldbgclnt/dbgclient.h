#ifndef DBGCLIENT_H
#define DBGCLIENT_H

#include <QObject>
#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dbg)

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QJsonObject;
QT_END_NAMESPACE

// Клиент отладочного протокола rsldbg (протокол v3: DBGHEADER + JSON по TCP,
// авторизация токеном после handshake).
// Содержит транспорт, handshake, авторизацию, разбор кадров и формирование
// запросов. Реакцию на входящие сообщения определяют наследники: GUI-клиент
// (MainWindow через GuiDbgClient) или, в перспективе, MCP-сервер.
class DbgClient : public QObject
{
    Q_OBJECT

public:
    explicit DbgClient(QObject *parent = nullptr);
    ~DbgClient() Q_DECL_OVERRIDE;

    bool connectToHost(const QString &hostName, quint16 port);
    void disconnectFromHost();
    bool isConnected() const;

    QString lastError() const { return m_LastError; }
    qint32 protocolVersion() const { return m_ProtocolVersion; }

    // Токен авторизации (из rsldbg://-URI или --token). Нужен, только если
    // токен настроен на сервере (rsldbg.ini); локально обычно не требуется.
    void setAuthToken(const QString &token) { m_AuthToken = token; }

    // Запросы к серверу
    void execContinue(int traceLog);
    void expandVariable(int index, qint64 stack, int stackIndex);
    void showVarValue(qint64 val, qint64 info);

    // Аналог команды Stop: abort + короткое ожидание, чтобы сервер
    // успел обработать его до разрыва TCP
    void stop();

signals:
    void connected();
    void disconnected();

protected:
    // Обработчики сообщений сервера — переопределяются наследниками.
    // Базовые реализации только логируют.
    virtual void onEvent(const QJsonObject &obj);
    virtual void onUpdateText(const QJsonObject &obj);
    virtual void onUpdateBreakpoints(const QJsonObject &obj);
    virtual void onUpdateStack(const QJsonObject &obj);
    virtual void onUpdateLocals(const QJsonObject &obj);
    virtual void onValueInfo(const QJsonObject &obj);
    virtual void onUnknownMessage(quint16 action, const QByteArray &payload);

private slots:
    void readyRead();

private:
    void write(const quint16 &action, const QByteArray &data);
    void write(const quint16 &action, void *data, const int &len);
    void setError(const QString &message);
    bool authenticate();

    QTcpSocket *m_pSocket;
    QString m_LastError;
    QString m_AuthToken;
    qint32 m_ProtocolVersion = 1; // из handshake сервера; 1 = legacy без версии
};

#endif // DBGCLIENT_H
