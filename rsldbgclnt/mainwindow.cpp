#include <Windows.h>
#include "mainwindow.h"
#include "models/varwatchmodel.h"
#include "stdviewdockwidget.h"
#include "logevent.h"
#include "models/logeventmodel.h"
#include "models/callstackmodel.h"
#include "ui_mainwindow.h"
#include "dbgeditorlinewidgetprovider.h"
#include "varwatchdockwidget.h"
#include "callstackdockwidget.h"
#include <finddialog.h>
#include <dbgserverproto.h>
#include <codeeditor/codeeditor.h>
#include <codeeditor/cppcodehighlighter.h>
#include <QAction>
#include <QTreeView>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <varwatchdlg.h>
//#include <QIODevice>

Q_LOGGING_CATEGORY(dbg, "rsldbg")
// русский комментарий
static QString strippedActionText(QString s)
{
    s.remove(QString::fromLatin1("..."));
    for (int i = 0; i < s.size(); ++i)
    {
        if (s.at(i) == QLatin1Char('&'))
            s.remove(i, 1);
    }

    return s.trimmed();
}

template<class T>void AddShortcutToToolTip(T *action)
{
    if (!action->shortcut().isEmpty())
    {
        QString tooltip = action->property("tooltipBackup").toString();
        if (tooltip.isEmpty())
        {
            tooltip = action->toolTip();
            if (tooltip != strippedActionText(action->text()))
                action->setProperty("tooltipBackup", action->toolTip());  // action uses a custom tooltip. Backup so that we can restore it later.
        }

        QColor shortcutTextColor = QApplication::palette().color(QPalette::ToolTipText);
        QString shortCutTextColorName;
        if (shortcutTextColor.value() == 0)
            shortCutTextColorName = "gray";  // special handling for black because lighter() does not work there [QTBUG-9343].
        else
        {
            int factor = (shortcutTextColor.value() < 128) ? 150 : 50;
            shortCutTextColorName = shortcutTextColor.lighter(factor).name();
        }
        action->setToolTip(QString("<p style='white-space:pre'>%1&nbsp;&nbsp;<code style='color:%2; font-size:small'>%3</code></p>")
                               .arg(tooltip, shortCutTextColorName, action->shortcut().toString(QKeySequence::NativeText)));
    }
}

void AddShortcutToToolTip(QAction *action)
{
    AddShortcutToToolTip<QAction>(action);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_CurModuleInView(0),
    m_pSocket(new QTcpSocket()),
    m_pFindDialog(nullptr)
{

    ui->setupUi(this);
    setIconSize(QSize(16, 16));

    setDockNestingEnabled(true);
    setDockOptions(GroupedDragging | AnimatedDocks | AllowTabbedDocks);

    m_LogModel.reset(new LogEventModel());
    m_CallStackModel.reset(new CallStackModel());

    m_LogDockWidget.reset(new StdViewDockWidget());
    m_LogDockWidget->setModel(m_LogModel.data());

    m_StackDockWidget.reset(new CallStackDockWidget());
    m_StackDockWidget->setModel(m_CallStackModel.data());
    m_StackDockWidget->setWindowTitle(tr("Call stack"));

    m_LocalsModel.reset(new VarWatchModel());
    m_LocalsDockWidget.reset(new VarWatchDockWidget());
    m_LocalsDockWidget->setWindowTitle(tr("Locals"));
    m_LocalsDockWidget->setModel(m_LocalsModel.data());
    //m_LocalsDockWidget->view()->setRootIsDecorated(true);
    //m_LocalsDockWidget->view()->resetIndentation();

    m_pCodeEditor = new CodeEditor(this);
    m_pCodeEditor->setReadOnly(true);
    m_pCodeEditor->setAutoHighlightCurrentLine(false);
    setCentralWidget(m_pCodeEditor);

    m_pCodeEditorProvider = new DbgEditorLineWidgetProvider();
    m_pCodeEditor->setCodeEditorLineWidgetProvider(m_pCodeEditorProvider);

    ToolApplyHighlighter(m_pCodeEditor, HighlighterRsl);

    m_LogDockWidget->setAutoScrollToBottom(true);
    m_LogDockWidget->setWindowTitle(tr("Debugger log"));

    m_LogDockWidget->view()->header()->resizeSection(LogEventModel::ColumnTime, 135);
    m_LogDockWidget->view()->header()->resizeSection(LogEventModel::ColumnType, 30);
    m_LogDockWidget->view()->header()->resizeSection(LogEventModel::ColumnContext, 50);

    //m_StackDockWidget->view()->header()->resizeSection(CallStackModel::ColumnLevel, 50);

    addDockWidget(Qt::BottomDockWidgetArea, m_LogDockWidget.data());
    addDockWidget(Qt::BottomDockWidgetArea, m_StackDockWidget.data());
    tabifyDockWidget(m_LogDockWidget.data(), m_LocalsDockWidget.data());

    m_StackDockWidget->view()->header()->setStretchLastSection(false);
    m_StackDockWidget->view()->header()->setSectionResizeMode(CallStackModel::ColumnLevel, QHeaderView::ResizeToContents);
    m_StackDockWidget->view()->header()->setSectionResizeMode(CallStackModel::ColumnFile, QHeaderView::ResizeToContents);
    m_StackDockWidget->view()->header()->setSectionResizeMode(CallStackModel::ColumnLine, QHeaderView::ResizeToContents);
    m_StackDockWidget->view()->header()->setSectionResizeMode(CallStackModel::ColumnFunction, QHeaderView::Stretch);
    m_StackDockWidget->view()->header()->resizeSections(QHeaderView::Stretch);

    InitDebugToolBar();
    InitOtherToolBars();

    connect(m_pSocket.data(), &QTcpSocket::connected, this, &MainWindow::dbgConnected);
    connect(m_pSocket.data(), &QTcpSocket::disconnected, this, &MainWindow::dbgDisconnected);
    connect(m_LocalsDockWidget.data(), &VarWatchDockWidget::expandVariable, this, &MainWindow::expandVariable);
    connect(m_LocalsDockWidget.data(), &VarWatchDockWidget::showVarValue, this, &MainWindow::showVarValue);
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug(dbg()) << "~MainWindow()";
}

QString MainWindow::lastError()
{
    return m_LastError;
}

void MainWindow::dbgConnected()
{
    qInfo(dbg()) << "Connected...";
}

void MainWindow::dbgDisconnected()
{
    qInfo(dbg()) << "Disconnected...";
    close();
}

void MainWindow::InitOtherToolBars()
{
    ui->actionFind->setIcon(QIcon::fromTheme("Find"));
    ui->actionFind->setShortcut(QKeySequence::Find);

    ui->actionFindNext->setShortcut(QKeySequence::FindNext);

    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::onFind);
    connect(ui->actionFindNext, &QAction::triggered, this, &MainWindow::onFindNext);
}

void MainWindow::InitDebugToolBar()
{
    ui->actionExit->setIcon(QIcon::fromTheme("ResumeExec"));

    ui->actionResume->setIcon(QIcon::fromTheme("Resume"));
    ui->actionResume->setShortcut(QKeySequence(tr("F5")));

    ui->actionStop->setIcon(QIcon::fromTheme("Stop"));

    ui->actionStepInto->setIcon(QIcon::fromTheme("StepInto"));
    ui->actionStepInto->setShortcut(QKeySequence(tr("F11")));

    ui->actionStepOver->setIcon(QIcon::fromTheme("StepOver"));
    ui->actionStepOver->setShortcut(QKeySequence(tr("F10")));

    ui->actionStepOut->setIcon(QIcon::fromTheme("StepOut"));
    ui->actionStepOut->setShortcut(QKeySequence(tr("Shift+F11")));

    ui->debugToolBar->addAction(ui->actionResume);
    ui->debugToolBar->addAction(ui->actionStop);
    ui->debugToolBar->addSeparator();
    ui->debugToolBar->addAction(ui->actionStepInto);
    ui->debugToolBar->addAction(ui->actionStepOver);
    ui->debugToolBar->addAction(ui->actionStepOut);

    AddShortcutToToolTip(ui->actionResume);
    AddShortcutToToolTip(ui->actionStepInto);
    AddShortcutToToolTip(ui->actionStepOver);
    AddShortcutToToolTip(ui->actionStepOut);

    connect(ui->actionStepOver, &QAction::triggered, [=]()
    {
        exec_continue(EXECCONTNUE_STEP_OVER);
    });

    connect(ui->actionStepInto, &QAction::triggered, [=]()
    {
        exec_continue(EXECCONTNUE_STEP_INTO);
    });

    connect(ui->actionStepOut, &QAction::triggered, [=]()
    {
        exec_continue(EXECCONTNUE_STEP_OUT);
    });

    connect(ui->actionResume, &QAction::triggered, [=]()
    {
        exec_continue(EXECCONTNUE_DBG_RUN);
    });

    connect(ui->actionStop, &QAction::triggered, [=]()
    {
        exec_continue(EXECCONTNUE_DBG_ABORT);
    });
}

bool MainWindow::connectToHost(const QString &hostName, quint16 port)
{
    bool hr = true;
    m_pSocket->connectToHost(hostName, port);

    qInfo(dbg()) << "Try connect to" << hostName << ":" << port;
    m_pSocket->waitForConnected();
    qInfo(dbg()) << "State" << m_pSocket->state();

    if (m_pSocket->state() != QAbstractSocket::ConnectedState)
    {
        m_LastError = QString("Can't connecto to debug host %1:%2")
                        .arg(hostName)
                        .arg(port);
        qCritical(dbg()) << m_LastError;
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
        m_LastError = QString("Invalid header size: expected %1, received %2")
            .arg(sizeof(DBGHEADER))
            .arg(headerdata.size());

        qCritical(dbg()) << m_LastError << headerdata;

        m_pSocket->disconnectFromHost();
        return false;
    }

    DBGHEADER *hdr = (DBGHEADER*)headerdata.data();
    if (!DbgCheckHeader(hdr))
    {
        m_LastError = "Invalid header magic";
        qCritical(dbg()) << m_LastError;

        m_pSocket->disconnectFromHost();
        return false;
    }

    if (hdr->action != DBG_REQUEST_HANDSHAKE)
    {
        m_LastError = "Invalid header action";
        qCritical(dbg()) << m_LastError;

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
        m_LastError = QStringLiteral("Устаревший rsldbg.dll: протокол v%1, требуется v%2. Пересоберите сервер.")
                          .arg(m_ProtocolVersion).arg(RSLDBG_PROTOCOL_VERSION);
        qCritical(dbg()) << m_LastError;
        m_pSocket->disconnectFromHost();
        return false;
    }

    qInfo(dbg()) << "connectToHost" << hr;
    connect(m_pSocket.data(), &QTcpSocket::readyRead, this, &MainWindow::readyRead);

    if (m_pSocket->bytesAvailable())
        readyRead();

    return hr;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_pSocket->disconnectFromHost();
    QMainWindow::closeEvent(event);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == MSG_LOG)
    {
        LogEvent *log = (LogEvent*)event;
        m_LogModel->append(log);
        //event->accept();
    }

    return QMainWindow::event(event);
}

void MainWindow::onFind()
{
    if (m_pFindDialog)
    {
        if (m_pFindDialog->isVisible())
            return;
    }

    if (!m_pFindDialog)
    {
        m_pFindDialog = new FindDialog(this);
        m_pFindDialog->setEditor(m_pCodeEditor);
        m_pFindDialog->setWindowIcon(QIcon::fromTheme("Find"));
    }

    m_pFindDialog->show();
}

void MainWindow::onFindNext()
{
    if (m_pFindDialog)
    {
        if (!m_pFindDialog->isVisible())
            return;
    }

    m_pFindDialog->find();
}

void MainWindow::applyCurrentStatement(const int &offs, const int &len, const int &line)
{
    QTextDocument *doc = m_pCodeEditor->document();
    QTextBlock blok = doc->firstBlock();

    if (blok.isValid())
    {
        QTextCursor cursor(blok);
        cursor.setPosition(offs - line + 1, QTextCursor::MoveAnchor);
        cursor.setPosition(offs + len - line + 1, QTextCursor::KeepAnchor);

        QTextCharFormat format;
        format.setBackground(QColor("#fb000d"));

        m_pCodeEditor->clearUserSelection();
        m_pCodeEditor->appendUserSelection(cursor, format);
    }
}

void MainWindow::write(const quint16 &acton, void *data, const int &len)
{
    QByteArray tmp;
    tmp.append((char*)data, len);
    write(acton, tmp);
}

void MainWindow::write(const quint16 &acton, const QByteArray &data)
{
    DBGHEADER hdr;
    DbgMakeHeader(&hdr, data.size(), acton);

    QByteArray tmp;
    QDataStream stream(&tmp, QIODevice::WriteOnly);
    stream.writeRawData((char*)&hdr, sizeof(DBGHEADER));
    tmp.append(data);

    m_pSocket->write(tmp);
    m_pSocket->waitForBytesWritten();
}

void MainWindow::exec_continue(int trace_log)
{
    QJsonObject obj;
    obj["traceLog"] = trace_log;

    write(DBG_REQUEST_EXECCONTNUE_JSON, QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void MainWindow::expandVariable(const int &index, const qint64 &stack)
{
    QModelIndex Selected = m_StackDockWidget->view()->selectionModel()->currentIndex();

    QJsonObject obj;
    obj["index"] = index;
    obj["stackindex"] = Selected.isValid() ? Selected.row() : 0;
    obj["st"] = QString::number(stack);

    write(DBG_REQUEST_EXPANDVARIABLE_JSON, QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void MainWindow::showVarValue(const qint64 &val, const qint64 &info)
{
    QJsonObject obj;
    obj["val"] = QString::number(val);
    obj["info"] = QString::number(info);
    obj["hash"] = "0";

    write(DBG_REQUEST_GETVALUENFO_JSON, QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void MainWindow::readyRead()
{
    // Заголовок НЕ извлекаем из буфера (peek), пока не убедимся, что
    // сообщение пришло целиком: иначе при фрагментации TCP-потока остаток
    // тела будет прочитан как "заголовок" и поток рассинхронизируется.
    if (m_pSocket->bytesAvailable() < (qint64)sizeof(DBGHEADER))
        return;

    DBGHEADER header;
    m_pSocket->peek((char*)&header, sizeof(DBGHEADER));

    if (!DbgCheckHeader(&header))
    {
        m_LastError = "Invalid header magic";
        qCritical(dbg()) << m_LastError;
        m_pSocket->disconnectFromHost();
        return;
    }

    // Защита от мусорного размера: без неё read(size - sizeof(DBGHEADER))
    // при size < sizeof(DBGHEADER) уходит в беззнаковое переполнение
    // и пытается выделить гигабайты (bad_alloc, падение клиента).
    if (header.size < (qint32)sizeof(DBGHEADER) || header.size > 64 * 1024 * 1024)
    {
        m_LastError = QString("Invalid message size: %1").arg(header.size);
        qCritical(dbg()) << m_LastError;
        m_pSocket->disconnectFromHost();
        return;
    }

    if (m_pSocket->bytesAvailable() < header.size)
        return; // тело ещё не пришло целиком - дождёмся следующего readyRead

    m_pSocket->read(sizeof(DBGHEADER)); // теперь извлекаем заголовок
    const qint64 payloadSize = (qint64)header.size - (qint64)sizeof(DBGHEADER);
    if (header.action == DBG_REQUEST_EVENT_JSON)
    {
        QByteArray payload = m_pSocket->read(payloadSize);
        QJsonObject obj = QJsonDocument::fromJson(payload).object();

        if (obj.value("event").toString() == QLatin1String("breakpoint"))
            qDebug(dbg()) << "Handle breakpoint event. Is setted:" << obj.value("set").toBool();
        else
            qDebug(dbg()) << "Handle unknown event:" << obj.value("event").toString();
    }
    else if (header.action == DBG_REQUEST_UPDATETEXT_JSON)
    {
        // Протокол v2: текст и имя файла уже в Unicode (UTF-8 JSON).
        QByteArray payload = m_pSocket->read(payloadSize);
        QJsonObject obj = QJsonDocument::fromJson(payload).object();

        if (obj.contains("text"))
        {
            m_pCodeEditor->setPlainText(obj.value("text").toString());
            setWindowFilePath(obj.value("filename").toString());
        }

        int offs = obj.value("offs").toInt();
        int len = obj.value("len").toInt();
        int line = obj.value("line").toInt();

        m_CurModuleInView = obj.value("curModuleInView").toString().toLongLong();
        m_pCodeEditor->setCurrentHighlightLine(line);
        m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconCurrentLine, line);

        applyCurrentStatement(offs, len, line);
        qInfo(dbg()) << "Update source requested:" << obj.value("filename").toString();
        qInfo(dbg()) << "Offset:" << offs << "Length:" << len << "Line:" << line;
    }
    else if (header.action == DBG_REQUEST_UPDATEBP_JSON)
    {
        qInfo(dbg()) << "Update breakpoints requested";

        QByteArray payload = m_pSocket->read(payloadSize);
        QJsonObject obj = QJsonDocument::fromJson(payload).object();

        const QJsonArray bps = obj.value("bps").toArray();
        for (const QJsonValue &entry : bps)
        {
            const QJsonObject bp = entry.toObject();
            int type = bp.value("type").toInt();
            int line = bp.value("line").toInt();

            if (type == DBGBP_ENABLED)
                m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconBreakpoint, line);
            else if (type == DBGBP_DISABLED)
                m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconBreakpointDisable, line);
        }
    }
    else if (header.action == DBG_REQUEST_UPDATESTACK_JSON)
    {
        m_CallStackModel->clear();
        m_pCodeEditorProvider->clearEnterFunction();

        QByteArray payload = m_pSocket->read(payloadSize);
        QJsonObject obj = QJsonDocument::fromJson(payload).object();

        const QJsonArray frames = obj.value("frames").toArray();
        int i = 0;
        for (const QJsonValue &entry : frames)
        {
            const QJsonObject frame = entry.toObject();

            StackItem item;
            item.func = frame.value("func").toString();
            item.fnamespace = frame.value("namespace").toString();
            item.fullfilename = frame.value("file").toString();
            item.offs = frame.value("offs").toInt();
            item.len = frame.value("len").toInt();
            item.line = frame.value("line").toInt();

            m_CallStackModel->append(item);

            qint64 module = frame.value("module").toString().toLongLong();
            if (i != 0 && m_CurModuleInView == module)
                m_pCodeEditorProvider->addEnterFunction(item.line);

            i++;
        }
    }
    else if (header.action == DBG_REQUEST_UPDATELOCALS_JSON)
    {
        // Протокол v2: переменные в JSON (UTF-8), хендлы - строками,
        // чтобы не терять точность в double.
        QByteArray payload = m_pSocket->read(payloadSize);

        QJsonParseError parseError = {};
        QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        {
            qWarning(dbg()) << "Invalid UPDATELOCALS_JSON payload:"
                            << parseError.errorString() << "at" << parseError.offset;
        }
        else
        {
            const QJsonArray vars = doc.object().value("vars").toArray();

            m_LocalsModel->startResetVarables();

            for (const QJsonValue &entry : vars)
            {
                const QJsonObject obj = entry.toObject();

                VarWatchData var;
                var.index = obj.value("index").toInt();
                var.name = obj.value("name").toString();
                var.type = obj.value("type").toString();
                var.proc = obj.value("proc").toString();
                var.depth = obj.value("depth").toInt(1);
                var.isObject = obj.value("object").toBool();
                var.isExpanded = obj.value("expanded").toBool();
                var.isLvalue = obj.value("lvalue").toBool();
                var.isFakeChildren = obj.value("fakeChildren").toBool();

                var.val  = obj.value("val").toString().toLongLong();
                var.exp  = obj.value("exp").toString().toLongLong();
                var.st   = obj.value("st").toString().toLongLong();
                var.procHandle = obj.value("procHandle").toString().toLongLong();
                var.info = obj.value("info").toString().toLongLong();
                var.mod  = obj.value("mod").toString().toLongLong();

                m_LocalsModel->append(var, obj.value("value").toString());
            }

            m_LocalsModel->finishResetVarables();
        }
    }
    else if (header.action == DBG_REQUEST_GETVALUENFO_JSON)
    {
        // Протокол v2: name/type/value уже в Unicode.
        QByteArray payload = m_pSocket->read(payloadSize);
        QJsonObject obj = QJsonDocument::fromJson(payload).object();

        VarWatchDlg dlg(this);
        dlg.setValue(obj.value("value").toString());
        dlg.setVarName(obj.value("name").toString());
        dlg.setVarType(obj.value("type").toString());
        dlg.exec();
    }

    // Если в буфере уже лежит следующее сообщение - обрабатываем сразу.
    if (m_pSocket->bytesAvailable() >= (qint64)sizeof(DBGHEADER))
        readyRead();
}
