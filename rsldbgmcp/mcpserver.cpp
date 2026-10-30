#include "mcpserver.h"
#include "mcpdbgclient.h"
#include "discovery.h"
#include "dbgserverproto.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QHash>
#include <QUrl>
#include <QUrlQuery>
#include <cstdio>

// ---------------------------------------------------------------------------
// StdinReader
// ---------------------------------------------------------------------------

void StdinReader::run()
{
    QFile in;
    in.open(stdin, QIODevice::ReadOnly);

    while (!in.atEnd())
    {
        const QByteArray line = in.readLine();
        if (line.isEmpty())
        {
            if (in.atEnd())
                break;
            continue;
        }
        emit lineReady(QString::fromUtf8(line));
    }

    emit eof();
}

// ---------------------------------------------------------------------------
// McpServer
// ---------------------------------------------------------------------------

McpServer::McpServer(QObject *parent)
    : QObject{parent}
    , m_Dbg(new McpDbgClient(this))
{
    m_Out.open(stdout, QIODevice::WriteOnly);

    m_Reader = new StdinReader(this);
    connect(m_Reader, &StdinReader::lineReady, this, &McpServer::onLine);
    connect(m_Reader, &StdinReader::eof, this, &McpServer::onEof);
    m_Reader->start();
}

McpServer::~McpServer()
{
    m_Reader->terminate(); // поток висит на блокирующем readLine
    m_Reader->wait(3000);
}

void McpServer::send(const QJsonObject &msg)
{
    m_Out.write(QJsonDocument(msg).toJson(QJsonDocument::Compact));
    m_Out.write("\n");
    m_Out.flush();
}

void McpServer::sendError(const QJsonValue &id, int code, const QString &message)
{
    send(QJsonObject{
        {QStringLiteral("jsonrpc"), QStringLiteral("2.0")},
        {QStringLiteral("id"), id},
        {QStringLiteral("error"), QJsonObject{
            {QStringLiteral("code"), code},
            {QStringLiteral("message"), message}}}
    });
}

void McpServer::onLine(const QString &line)
{
    // wait_event и другие долгие команды крутят вложенный QEventLoop,
    // в котором доставляются следующие queued lineReady. Чтобы запросы не
    // выполнялись реентерабельно (например, disconnect посреди wait_event),
    // обрабатываем строго по одному, остальное копим в очереди.
    if (m_Busy)
    {
        m_Pending.append(line);
        return;
    }

    m_Busy = true;
    handleLine(line);

    while (!m_Pending.isEmpty())
        handleLine(m_Pending.takeFirst());

    m_Busy = false;

    if (m_Eof)
        qApp->quit();
}

void McpServer::onEof()
{
    // См. комментарий к m_Eof: quit() прервал бы вложенный QEventLoop
    // работающего сейчас wait_event - выходим только после всех запросов.
    if (m_Busy)
        m_Eof = true;
    else
        qApp->quit();
}

void McpServer::handleLine(const QString &line)
{
    if (line.trimmed().isEmpty())
        return;

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
    {
        sendError(QJsonValue(), -32700, QStringLiteral("Parse error"));
        return;
    }

    const QJsonObject req = doc.object();
    const QJsonValue id = req.value(QStringLiteral("id"));
    const QString method = req.value(QStringLiteral("method")).toString();

    // Уведомления (без id) ответа не требуют.
    if (id.isUndefined())
        return;

    QJsonObject result, error;
    process(method, req.value(QStringLiteral("params")).toObject(), &result, &error);

    QJsonObject resp;
    resp.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    resp.insert(QStringLiteral("id"), id);
    if (error.isEmpty())
        resp.insert(QStringLiteral("result"), result);
    else
        resp.insert(QStringLiteral("error"), error);
    send(resp);
}

void McpServer::process(const QString &method, const QJsonObject &params,
                        QJsonObject *result, QJsonObject *error)
{
    if (method == QLatin1String("initialize"))
    {
        *result = QJsonObject{
            {QStringLiteral("protocolVersion"), QStringLiteral("2024-11-05")},
            {QStringLiteral("capabilities"), QJsonObject{
                {QStringLiteral("tools"), QJsonObject{}}}},
            {QStringLiteral("serverInfo"), QJsonObject{
                {QStringLiteral("name"), QStringLiteral("rsldbgmcp")},
                {QStringLiteral("version"), QStringLiteral("0.1.0")}}}
        };
    }
    else if (method == QLatin1String("ping"))
    {
        *result = QJsonObject{};
    }
    else if (method == QLatin1String("tools/list"))
    {
        *result = QJsonObject{{QStringLiteral("tools"), toolsList()}};
    }
    else if (method == QLatin1String("tools/call"))
    {
        *result = callTool(params.value(QStringLiteral("name")).toString(),
                           params.value(QStringLiteral("arguments")).toObject());
    }
    else
    {
        *error = QJsonObject{
            {QStringLiteral("code"), -32601},
            {QStringLiteral("message"), QStringLiteral("Method not found: %1").arg(method)}};
    }
}

// ---------------------------------------------------------------------------
// Tools
// ---------------------------------------------------------------------------

QJsonObject McpServer::toolText(const QJsonDocument &doc, bool isError)
{
    return QJsonObject{
        {QStringLiteral("content"), QJsonArray{QJsonObject{
            {QStringLiteral("type"), QStringLiteral("text")},
            {QStringLiteral("text"), QString::fromUtf8(doc.toJson(QJsonDocument::Compact))}}}},
        {QStringLiteral("isError"), isError}
    };
}

QJsonObject McpServer::toolError(const QString &message)
{
    return QJsonObject{
        {QStringLiteral("content"), QJsonArray{QJsonObject{
            {QStringLiteral("type"), QStringLiteral("text")},
            {QStringLiteral("text"), message}}}},
        {QStringLiteral("isError"), true}
    };
}

static QJsonObject schemaProp(const QString &type, const QString &desc)
{
    return QJsonObject{
        {QStringLiteral("type"), type},
        {QStringLiteral("description"), desc}};
}

QJsonArray McpServer::toolsList() const
{
    QJsonArray tools;

    auto tool = [](const QString &name, const QString &desc, const QJsonObject &schema)
    {
        return QJsonObject{
            {QStringLiteral("name"), name},
            {QStringLiteral("description"), desc},
            {QStringLiteral("inputSchema"), schema}};
    };

    auto objectSchema = [](const QJsonObject &props, const QJsonArray &required = QJsonArray())
    {
        QJsonObject s{
            {QStringLiteral("type"), QStringLiteral("object")},
            {QStringLiteral("properties"), props}};
        if (!required.isEmpty())
            s.insert(QStringLiteral("required"), required);
        return s;
    };

    tools.append(tool(QStringLiteral("rsl_sessions"),
        QStringLiteral("Find running RSL debug sessions (UDP discovery). "
                       "Returns an array of sessions: pid, port, exe, title, user, host."),
        objectSchema({{QStringLiteral("timeout_ms"),
                       schemaProp(QStringLiteral("integer"),
                                  QStringLiteral("Announce collection time, ms (default 1500)"))}})));

    tools.append(tool(QStringLiteral("rsl_connect"),
        QStringLiteral("Connect to a debug session via rsldbg:// URI or host/port/token."),
        objectSchema({{QStringLiteral("uri"),
                       schemaProp(QStringLiteral("string"),
                                  QStringLiteral("rsldbg://host:port?token=... (from the log/clipboard)"))},
                      {QStringLiteral("host"), schemaProp(QStringLiteral("string"), QStringLiteral("Host (default 127.0.0.1)"))},
                      {QStringLiteral("port"), schemaProp(QStringLiteral("integer"), QStringLiteral("Session port"))},
                      {QStringLiteral("token"), schemaProp(QStringLiteral("string"), QStringLiteral("Token, if configured on the server"))}})));

    tools.append(tool(QStringLiteral("rsl_disconnect"),
        QStringLiteral("Disconnect from the current debug session."),
        objectSchema({})));

    tools.append(tool(QStringLiteral("rsl_continue"),
        QStringLiteral("Execution control: run, step_over, step_into, step_out, abort. "
                       "With wait=true (recommended) also waits for the next stop and returns "
                       "the event snapshot in one call - saves a separate rsl_wait_event round trip."),
        objectSchema({{QStringLiteral("mode"),
                       schemaProp(QStringLiteral("string"),
                                  QStringLiteral("run | step_over | step_into | step_out | abort"))},
                      {QStringLiteral("wait"),
                       schemaProp(QStringLiteral("boolean"),
                                  QStringLiteral("Wait for the next stop and return its snapshot (default false)"))},
                      {QStringLiteral("timeout_ms"),
                       schemaProp(QStringLiteral("integer"),
                                  QStringLiteral("Wait timeout when wait=true, ms (default 30000)"))},
                      {QStringLiteral("skip_same_line"),
                       schemaProp(QStringLiteral("boolean"),
                                  QStringLiteral("step_over/step_into only: the server repeats the step "
                                                 "automatically until the (module, line) position changes, "
                                                 "without round trips - fast skipping of multi-line statements"))},
                      {QStringLiteral("max_steps"),
                       schemaProp(QStringLiteral("integer"),
                                  QStringLiteral("Safety cap for skip_same_line (default 500)"))}},
                     QJsonArray{QStringLiteral("mode")})));

    tools.append(tool(QStringLiteral("rsl_wait_event"),
        QStringLiteral("Wait for a debugger event (stop, line change, termination) "
                       "and return a snapshot: event, source (without full text), vars, "
                       "frames_count/vars_count (call rsl_stack for the full call stack). "
                       "If the debugger is already stopped and no rsl_continue was sent since, "
                       "returns the current snapshot immediately - call it once after each rsl_continue."),
        objectSchema({{QStringLiteral("timeout_ms"),
                       schemaProp(QStringLiteral("integer"),
                                  QStringLiteral("Wait timeout, ms (default 30000)"))}})));

    tools.append(tool(QStringLiteral("rsl_locals"),
        QStringLiteral("Local variables of the current frame (latest debugger update)."),
        objectSchema({})));

    tools.append(tool(QStringLiteral("rsl_stack"),
        QStringLiteral("Call stack (frames: func, namespace, file, line)."),
        objectSchema({})));

    tools.append(tool(QStringLiteral("rsl_breakpoints"),
        QStringLiteral("Breakpoints (line, type)."),
        objectSchema({})));

    tools.append(tool(QStringLiteral("rsl_source"),
        QStringLiteral("Current source module: filename, line, offs, len, text. "
                       "By default returns the full text; pass context to get "
                       "only +/- N lines around the current line."),
        objectSchema({{QStringLiteral("context"),
                       schemaProp(QStringLiteral("integer"),
                                  QStringLiteral("If set, return only +/- N lines around the current line"))}})));

    tools.append(tool(QStringLiteral("rsl_expand_variable"),
        QStringLiteral("Expand an object variable (child fields). Returns "
                       "the updated variable list."),
        objectSchema({{QStringLiteral("index"), schemaProp(QStringLiteral("integer"), QStringLiteral("Variable index"))},
                      {QStringLiteral("stack"), schemaProp(QStringLiteral("string"), QStringLiteral("Variable stack handle (st) as a string"))},
                      {QStringLiteral("stackindex"), schemaProp(QStringLiteral("integer"), QStringLiteral("Stack frame index"))}},
                     QJsonArray{QStringLiteral("index"), QStringLiteral("stack")})));

    tools.append(tool(QStringLiteral("rsl_var_value"),
        QStringLiteral("Get variable name/type/value by val and info handles."),
        objectSchema({{QStringLiteral("val"), schemaProp(QStringLiteral("string"), QStringLiteral("Value handle (val) as a string"))},
                      {QStringLiteral("info"), schemaProp(QStringLiteral("string"), QStringLiteral("Description handle (info) as a string"))}},
                     QJsonArray{QStringLiteral("val"), QStringLiteral("info")})));

    return tools;
}

QJsonObject McpServer::callTool(const QString &name, const QJsonObject &args)
{
    auto requireConnected = [this]() -> QString
    {
        if (!m_Dbg->isConnected())
            return QStringLiteral("Not connected to a debugger. Call rsl_connect first.");
        return QString();
    };

    if (name == QLatin1String("rsl_sessions"))
    {
        const int timeout = args.value(QStringLiteral("timeout_ms")).toInt(1500);
        return toolText(QJsonDocument(discoverDebugSessions(timeout)));
    }

    if (name == QLatin1String("rsl_connect"))
    {
        QString host = args.value(QStringLiteral("host")).toString();
        int port = args.value(QStringLiteral("port")).toInt();
        QString token = args.value(QStringLiteral("token")).toString();

        const QString uri = args.value(QStringLiteral("uri")).toString();
        if (!uri.isEmpty())
        {
            const QUrl u(uri);
            if (u.scheme() != QLatin1String("rsldbg"))
                return toolError(QStringLiteral("Invalid URI (expected rsldbg://host:port?token=...): %1").arg(uri));
            host = u.host();
            port = u.port();
            token = QUrlQuery(u).queryItemValue(QStringLiteral("token"));
        }

        if (host.isEmpty())
            host = QStringLiteral("127.0.0.1");
        if (port <= 0 || port > 65535)
            return toolError(QStringLiteral("Session port not specified (uri or port)."));

        m_Dbg->disconnectFromHost();
        m_Dbg->setAuthToken(token);

        if (!m_Dbg->connectToHost(host, quint16(port)))
            return toolError(QStringLiteral("Connect failed: %1").arg(m_Dbg->lastError()));

        return toolText(QJsonDocument(QJsonObject{
            {QStringLiteral("connected"), true},
            {QStringLiteral("host"), host},
            {QStringLiteral("port"), port},
            {QStringLiteral("protocol"), m_Dbg->protocolVersion()}
        }));
    }

    if (name == QLatin1String("rsl_disconnect"))
    {
        m_Dbg->disconnectFromHost();
        return toolText(QJsonDocument(QJsonObject{{QStringLiteral("connected"), false}}));
    }

    if (name == QLatin1String("rsl_continue"))
    {
        const QString err = requireConnected();
        if (!err.isEmpty())
            return toolError(err);

        static const QHash<QString, int> modes = {
            {QStringLiteral("step_over"),  EXECCONTNUE_STEP_OVER},
            {QStringLiteral("step_into"),  EXECCONTNUE_STEP_INTO},
            {QStringLiteral("run"),        EXECCONTNUE_DBG_RUN},
            {QStringLiteral("abort"),      EXECCONTNUE_DBG_ABORT},
            {QStringLiteral("step_out"),   EXECCONTNUE_STEP_OUT},
            {QStringLiteral("run_nextmod"), EXECCONTNUE_RUN_NEXTMOD},
            {QStringLiteral("run_thismod"), EXECCONTNUE_RUN_THISMOD},
        };

        const QString mode = args.value(QStringLiteral("mode")).toString();
        if (!modes.contains(mode))
            return toolError(QStringLiteral("Unknown mode: %1 (run, step_over, step_into, step_out, abort)").arg(mode));

        m_Dbg->notifyContinueSent();

        // Пакетный шаг: сервер сам повторяет step_over/step_into, пока не
        // сменится (модуль, строка) - экономим MCP-раундтрипы на шагах
        // внутри одной строки/многострочного оператора.
        const bool skipSameLine = args.value(QStringLiteral("skip_same_line")).toBool();
        const int maxSteps = args.value(QStringLiteral("max_steps")).toInt(500);
        m_Dbg->execContinue(modes.value(mode), skipSameLine, maxSteps);

        // wait=true: один tool call вместо пары rsl_continue + rsl_wait_event -
        // экономим целый MCP-раундтрип (модельное время дороже сетевого).
        if (args.value(QStringLiteral("wait")).toBool())
        {
            const int timeout = args.value(QStringLiteral("timeout_ms")).toInt(30000);
            return toolText(QJsonDocument(m_Dbg->waitEvent(timeout)));
        }

        return toolText(QJsonDocument(QJsonObject{{QStringLiteral("sent"), mode}}));
    }

    if (name == QLatin1String("rsl_wait_event"))
    {
        const QString err = requireConnected();
        if (!err.isEmpty())
            return toolError(err);

        const int timeout = args.value(QStringLiteral("timeout_ms")).toInt(30000);
        return toolText(QJsonDocument(m_Dbg->waitEvent(timeout)));
    }

    if (name == QLatin1String("rsl_locals"))
    {
        const QString err = requireConnected();
        if (!err.isEmpty())
            return toolError(err);
        return toolText(QJsonDocument(m_Dbg->locals()));
    }

    if (name == QLatin1String("rsl_stack"))
    {
        const QString err = requireConnected();
        if (!err.isEmpty())
            return toolError(err);
        return toolText(QJsonDocument(m_Dbg->stackFrames()));
    }

    if (name == QLatin1String("rsl_breakpoints"))
    {
        const QString err = requireConnected();
        if (!err.isEmpty())
            return toolError(err);
        return toolText(QJsonDocument(m_Dbg->breakpoints()));
    }

    if (name == QLatin1String("rsl_source"))
    {
        const QString err = requireConnected();
        if (!err.isEmpty())
            return toolError(err);

        QJsonObject src = m_Dbg->source();

        // context: отдать только окрестность текущей строки, а не весь модуль
        const QJsonValue ctx = args.value(QStringLiteral("context"));
        if (ctx.isDouble() && src.contains(QStringLiteral("text")))
        {
            const int context = ctx.toInt();
            QString text = src.value(QStringLiteral("text")).toString();

            QStringList lines = text.split(QStringLiteral("\r\n"));
            if (lines.size() <= 1)
                lines = text.split(QLatin1Char('\n'));

            const int cur = src.value(QStringLiteral("line")).toInt(); // 1-based
            const int from = qMax(0, cur - 1 - context);
            const int to = qMin(lines.size(), cur + context); // exclusive

            src.insert(QStringLiteral("from_line"), from + 1);
            src.insert(QStringLiteral("to_line"), to);
            src.insert(QStringLiteral("text"),
                       lines.mid(from, to - from).join(QStringLiteral("\r\n")));
        }

        return toolText(QJsonDocument(src));
    }

    if (name == QLatin1String("rsl_expand_variable"))
    {
        const QString err = requireConnected();
        if (!err.isEmpty())
            return toolError(err);

        const int index = args.value(QStringLiteral("index")).toInt();
        const qint64 stack = args.value(QStringLiteral("stack")).toString().toLongLong();
        const int stackIndex = args.value(QStringLiteral("stackindex")).toInt();

        if (!m_Dbg->expandVariableAndWait(index, stack, stackIndex, 5000))
            return toolError(QStringLiteral("Debugger did not send a variable update (timeout)."));

        return toolText(QJsonDocument(m_Dbg->locals()));
    }

    if (name == QLatin1String("rsl_var_value"))
    {
        const QString err = requireConnected();
        if (!err.isEmpty())
            return toolError(err);

        const qint64 val = args.value(QStringLiteral("val")).toString().toLongLong();
        const qint64 info = args.value(QStringLiteral("info")).toString().toLongLong();

        const QJsonObject reply = m_Dbg->valueInfoAndWait(val, info, 5000);
        if (reply.isEmpty())
            return toolError(QStringLiteral("Debugger did not send a value (timeout)."));

        return toolText(QJsonDocument(reply));
    }

    return toolError(QStringLiteral("Unknown tool: %1").arg(name));
}
