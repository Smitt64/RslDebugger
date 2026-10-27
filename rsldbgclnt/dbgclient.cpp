#include "dbgclient.h"
#include "dbgserverproto.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Q_LOGGING_CATEGORY(dbg, "rsldbg")

DbgClient::DbgClient(QObject *parent)
    : QObject(parent),
      m_pSocket(new QTcpSocket(this))
{
    connect(m_pSocket, &QTcpSocket::connected, this, &DbgClient::connected);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &DbgClient::disconnected);
}

DbgClient::~DbgClient()
{
}

void DbgClient::setError(const QString &message)
{
    m_LastError = message;
    qCritical(dbg()) << m_LastError;
}

bool DbgClient::isConnected() const
{
    return m_pSocket->state() == QAbstractSocket::ConnectedState;
}

void DbgClient::disconnectFromHost()
{
    m_pSocket->disconnectFromHost();
}

bool DbgClient::connectToHost(const QString &hostName, quint16 port)
{
    m_pSocket->connectToHost(hostName, port);

    qInfo(dbg()) << "Try connect to" << hostName << ":" << port;
    m_pSocket->waitForConnected();
    qInfo(dbg()) << "State" << m_pSocket->state();

    if (m_pSocket->state() != QAbstractSocket::ConnectedState)
    {
        setError(QString("Can't connect to debug host %1:%2")
                     .arg(hostName)
                     .arg(port));
        return false;
    }

    // Заголовок handshake может прийти по частям - дочитываем в цикле.
    QByteArray headerdata;
    while (headerdata.size() < (int)sizeof(DBGHEADER))
    {
        if (!m_pSocket->bytesAvailable() && !m_pSocket->waitForReadyRead())
            break;

        headerdata += m_pSocket->read(sizeof(DBGHEADER) - headerdata.size());
    }

    if (headerdata.size() != sizeof(DBGHEADER))
    {
        setError(QString("Invalid header size: expected %1, received %2")
                     .arg(sizeof(DBGHEADER))
                     .arg(headerdata.size()));

        m_pSocket->disconnectFromHost();
        return false;
    }

    DBGHEADER *hdr = (DBGHEADER*)headerdata.data();
    if (!DbgCheckHeader(hdr))
    {
        setError("Invalid header magic");
        m_pSocket->disconnectFromHost();
        return false;
    }

    if (hdr->action != DBG_REQUEST_HANDSHAKE)
    {
        setError("Invalid header action");
        m_pSocket->disconnectFromHost();
        return false;
    }

    // Протокол v2: за заголовком handshake идёт DBG_HANDSHAKE с версией.
    // Старый сервер шлёт size == 0 - остаёмся на версии 1.
    m_ProtocolVersion = 1;

    if (hdr->size > (qint32)sizeof(DBGHEADER))
    {
        const int payloadSize = hdr->size - (int)sizeof(DBGHEADER);
        QByteArray payload;

        while (payload.size() < payloadSize)
        {
            if (!m_pSocket->bytesAvailable())
                m_pSocket->waitForReadyRead();

            payload += m_pSocket->read(payloadSize - payload.size());
        }

        if (payload.size() >= (int)sizeof(DBG_HANDSHAKE))
        {
            const DBG_HANDSHAKE *hs = reinterpret_cast<const DBG_HANDSHAKE*>(payload.constData());
            m_ProtocolVersion = hs->version;
        }
    }

    qInfo(dbg()) << "Server protocol version:" << m_ProtocolVersion;

    if (m_ProtocolVersion < RSLDBG_PROTOCOL_VERSION)
    {
        setError(QStringLiteral("Устаревший rsldbg.dll: протокол v%1, требуется v%2. Пересоберите сервер.")
                     .arg(m_ProtocolVersion).arg(RSLDBG_PROTOCOL_VERSION));
        m_pSocket->disconnectFromHost();
        return false;
    }

    // Протокол v3: после handshake сервер ждёт кадр авторизации.
    if (m_ProtocolVersion >= 3 && !authenticate())
    {
        m_pSocket->disconnectFromHost();
        return false;
    }

    connect(m_pSocket, &QTcpSocket::readyRead, this, &DbgClient::readyRead);

    if (m_pSocket->bytesAvailable())
        readyRead();

    return true;
}

void DbgClient::stop()
{
    if (!isConnected())
        return;

    execContinue(EXECCONTNUE_DBG_ABORT);

    // Даём серверу короткое время обработать abort до разрыва TCP
    m_pSocket->flush();
    m_pSocket->waitForDisconnected(500);
}

bool DbgClient::authenticate()
{
    // Токен нужен, только если он настроен на сервере (rsldbg.ini / URI) и
    // задан явно через setAuthToken/--token. Иначе кадр DBG_REQUEST_AUTH сам
    // по себе является приветствием "я клиент отладчика".
    QJsonObject obj;
    obj.insert(QStringLiteral("client"), QStringLiteral("rsldbgclnt"));
    if (!m_AuthToken.isEmpty())
        obj.insert(QStringLiteral("token"), m_AuthToken);

    write(DBG_REQUEST_AUTH, QJsonDocument(obj).toJson(QJsonDocument::Compact));

    // Ответ сервера: DBG_REQUEST_AUTH {"ok":bool}. Сервер держит сокет
    // блокирующим с таймаутом 10с, так что ответ придёт быстро либо никак.
    QByteArray headerdata;
    while (headerdata.size() < (int)sizeof(DBGHEADER))
    {
        if (!m_pSocket->bytesAvailable() && !m_pSocket->waitForReadyRead(15000))
            break;

        headerdata += m_pSocket->read(sizeof(DBGHEADER) - headerdata.size());
    }

    if (headerdata.size() != sizeof(DBGHEADER))
    {
        setError(QStringLiteral("Нет ответа авторизации от сервера"));
        return false;
    }

    DBGHEADER *hdr = (DBGHEADER*)headerdata.data();
    const int payloadSize = hdr->size - (int)sizeof(DBGHEADER);
    if (!DbgCheckHeader(hdr) || hdr->action != DBG_REQUEST_AUTH ||
        payloadSize <= 0 || payloadSize > 4096)
    {
        setError(QStringLiteral("Некорректный ответ авторизации от сервера"));
        return false;
    }

    QByteArray payload;
    while (payload.size() < payloadSize)
    {
        if (!m_pSocket->bytesAvailable() && !m_pSocket->waitForReadyRead(15000))
            break;

        payload += m_pSocket->read(payloadSize - payload.size());
    }

    const QJsonObject reply = QJsonDocument::fromJson(payload).object();
    if (!reply.value(QStringLiteral("ok")).toBool())
    {
        setError(reply.value(QStringLiteral("error")).toString(
            QStringLiteral("Сервер отклонил токен авторизации")));
        return false;
    }

    qInfo(dbg()) << "Auth OK";
    return true;
}

void DbgClient::write(const quint16 &action, void *data, const int &len)
{
    QByteArray tmp;
    tmp.append((char*)data, len);
    write(action, tmp);
}

void DbgClient::write(const quint16 &action, const QByteArray &data)
{
    DBGHEADER hdr;
    DbgMakeHeader(&hdr, data.size(), action);

    QByteArray tmp;
    QDataStream stream(&tmp, QIODevice::WriteOnly);
    stream.writeRawData((char*)&hdr, sizeof(DBGHEADER));
    tmp.append(data);

    m_pSocket->write(tmp);
    m_pSocket->waitForBytesWritten();
}

void DbgClient::execContinue(int traceLog)
{
    QJsonObject obj;
    obj["traceLog"] = traceLog;

    write(DBG_REQUEST_EXECCONTNUE_JSON, QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void DbgClient::expandVariable(int index, qint64 stack, int stackIndex)
{
    QJsonObject obj;
    obj["index"] = index;
    obj["stackindex"] = stackIndex;
    obj["st"] = QString::number(stack);

    write(DBG_REQUEST_EXPANDVARIABLE_JSON, QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void DbgClient::showVarValue(qint64 val, qint64 info)
{
    QJsonObject obj;
    obj["val"] = QString::number(val);
    obj["info"] = QString::number(info);
    obj["hash"] = "0";

    write(DBG_REQUEST_GETVALUENFO_JSON, QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void DbgClient::readyRead()
{
    // Заголовок НЕ извлекаем из буфера (peek), пока не убедимся, что
    // сообщение пришло целиком: иначе при фрагментации TCP-потока остаток
    // тела будет прочитан как "заголовок" и поток рассинхронизируется.
    while (m_pSocket->bytesAvailable() >= (qint64)sizeof(DBGHEADER))
    {
        DBGHEADER header;
        m_pSocket->peek((char*)&header, sizeof(DBGHEADER));

        if (!DbgCheckHeader(&header))
        {
            setError("Invalid header magic");
            m_pSocket->disconnectFromHost();
            return;
        }

        // Защита от мусорного размера: без неё read(size - sizeof(DBGHEADER))
        // при size < sizeof(DBGHEADER) уходит в беззнаковое переполнение
        // и пытается выделить гигабайты (bad_alloc, падение клиента).
        if (header.size < (qint32)sizeof(DBGHEADER) || header.size > 64 * 1024 * 1024)
        {
            setError(QString("Invalid message size: %1").arg(header.size));
            m_pSocket->disconnectFromHost();
            return;
        }

        if (m_pSocket->bytesAvailable() < header.size)
            return; // тело ещё не пришло целиком - дождёмся следующего readyRead

        m_pSocket->read(sizeof(DBGHEADER)); // теперь извлекаем заголовок
        const qint64 payloadSize = (qint64)header.size - (qint64)sizeof(DBGHEADER);
        QByteArray payload = m_pSocket->read(payloadSize);

        QJsonParseError parseError = {};
        QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        {
            qWarning(dbg()) << "Invalid JSON payload for action" << header.action
                            << ":" << parseError.errorString() << "at" << parseError.offset;
            continue;
        }

        const QJsonObject obj = doc.object();

        switch (header.action)
        {
        case DBG_REQUEST_EVENT_JSON:        onEvent(obj);            break;
        case DBG_REQUEST_UPDATETEXT_JSON:   onUpdateText(obj);       break;
        case DBG_REQUEST_UPDATEBP_JSON:     onUpdateBreakpoints(obj); break;
        case DBG_REQUEST_UPDATESTACK_JSON:  onUpdateStack(obj);      break;
        case DBG_REQUEST_UPDATELOCALS_JSON: onUpdateLocals(obj);     break;
        case DBG_REQUEST_GETVALUENFO_JSON:  onValueInfo(obj);        break;
        default:                            onUnknownMessage(header.action, payload); break;
        }

        // Обработчик мог разорвать соединение (или сервер закрыл его) - выходим
        if (m_pSocket->state() != QAbstractSocket::ConnectedState)
            break;
    }
}

// Базовые обработчики - только логирование; содержательная реакция в наследниках

void DbgClient::onEvent(const QJsonObject &obj)
{
    qDebug(dbg()) << "Event:" << obj.value("event").toString();
}

void DbgClient::onUpdateText(const QJsonObject &obj)
{
    qDebug(dbg()) << "UpdateText:" << obj.value("filename").toString();
}

void DbgClient::onUpdateBreakpoints(const QJsonObject &obj)
{
    qDebug(dbg()) << "UpdateBreakpoints:" << obj.value("bps").toArray().size();
}

void DbgClient::onUpdateStack(const QJsonObject &obj)
{
    qDebug(dbg()) << "UpdateStack:" << obj.value("frames").toArray().size();
}

void DbgClient::onUpdateLocals(const QJsonObject &obj)
{
    qDebug(dbg()) << "UpdateLocals:" << obj.value("vars").toArray().size();
}

void DbgClient::onValueInfo(const QJsonObject &obj)
{
    qDebug(dbg()) << "ValueInfo:" << obj.value("name").toString();
}

void DbgClient::onUnknownMessage(quint16 action, const QByteArray &payload)
{
    qWarning(dbg()) << "Unknown message action:" << action << "payload size:" << payload.size();
}
