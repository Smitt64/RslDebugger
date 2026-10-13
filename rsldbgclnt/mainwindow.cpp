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
#include <QTextCodec>
#include <QBuffer>
#include <QItemSelectionModel>
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
    oem866 = QTextCodec::codecForName("IBM 866");

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

    m_pSocket->waitForReadyRead();
    QByteArray headerdata = m_pSocket->read(sizeof(DBGHEADER));

    if (headerdata.size() != sizeof(DBGHEADER))
    {
        m_LastError = QString("Invalid header size: expected %1, received %2")
            .arg(sizeof(headerdata))
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
    DBGHEADER hdr;
    DbgMakeHeader(&hdr, sizeof(DBG_EXECCONTNUE), DBG_REQUEST_EXECCONTNUE);

    DBG_EXECCONTNUE exec;
    exec.trace_log = trace_log;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.writeRawData((char*)&hdr, sizeof(DBGHEADER));
    stream.writeRawData((char*)&exec, sizeof(DBG_EXECCONTNUE));

    m_pSocket->write(data);
    m_pSocket->waitForBytesWritten();
}

void MainWindow::expandVariable(const int &index, const qint64 &stack)
{
    DBG_EXPANDVARIABLE var;
    var.index = index;
    var.st = stack;

    QModelIndex Selected = m_StackDockWidget->view()->selectionModel()->currentIndex();

    if (Selected.isValid())
        var.stackindex = Selected.row();
    else
        var.stackindex = 0;

    write(DBG_REQUEST_EXPANDVARIABLE, &var, sizeof(DBG_EXPANDVARIABLE));
}

void MainWindow::showVarValue(const qint64 &val, const qint64 &info)
{
    DBG_GETVALUENFO value;
    value.hash_id = 0;
    value.val = val;
    value.info = info;

    write(DBG_REQUEST_GETVALUENFO, &value, sizeof(DBG_EXPANDVARIABLE));
}

void MainWindow::readyRead()
{
    qint64 bytesAvailable = m_pSocket->bytesAvailable();

    if (m_LastHeader.isEmpty())
    {
        if (bytesAvailable < sizeof(DBGHEADER))
            return;
    }
    else
    {
        if (bytesAvailable < m_LastHeader.top().size)
            return;
    }

    DBGHEADER header;
    m_pSocket->read((char*)&header, sizeof(DBGHEADER));

    if (!DbgCheckHeader(&header))
    {
        m_LastError = "Invalid header magic";
        qCritical(dbg()) << m_LastError;
        return;
    }

    bytesAvailable = m_pSocket->bytesAvailable();
    if (header.action == DBG_REQUEST_EVENT)
    {
        DBGEVENT event;
        m_pSocket->read((char*)&event, sizeof(DBGEVENT));
        if (event.event == MSG_BREAKPOINT)
        {
            qDebug(dbg()) << "Handle breakpoint event. Is setted:" << (event.data.bp_event.BpSetted == 1 ? true : false);
        }
        else
            qDebug(dbg()) << "Handle unknown event:" << event.event;
    }
    else if (header.action == DBG_REQUEST_UPDATETEXT)
    {
        DBG_UPDATETEXT updatetext;
        m_pSocket->read((char*)&updatetext, sizeof(DBG_UPDATETEXT));
        bytesAvailable = m_pSocket->bytesAvailable();

        if (updatetext.size)
        {
            char *buffer = new char[updatetext.size + 1];
            memset(buffer, 0, updatetext.size + 1);
            m_pSocket->read(buffer, updatetext.size);
            QString text = QString::fromLocal8Bit(buffer, updatetext.size);
            delete[] buffer;

            m_pCodeEditor->setPlainText(text);
            setWindowFilePath(updatetext.filename);
        }

        m_CurModuleInView = updatetext.curModuleInView;
        m_pCodeEditor->setCurrentHighlightLine(updatetext.line);
        m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconCurrentLine, updatetext.line);

        applyCurrentStatement(updatetext.offs, updatetext.len, updatetext.line);
        qInfo(dbg()) << "Update source requested:" << updatetext.filename;
        qInfo(dbg()) << "Offset:" << updatetext.offs << "Length:" << updatetext.len << "Line:" << updatetext.line;
    }
    else if (header.action == DBG_REQUEST_UPDATEBP)
    {
        qInfo(dbg()) << "Update breakpoints requested";

        DBG_UPDATEBP upd;
        m_pSocket->read((char*)&upd, sizeof(DBG_UPDATEBP));

        QByteArray bpdata = m_pSocket->read(sizeof(DBGBPDATA) * upd.count);

        DBGBPDATA *bp = (DBGBPDATA*)bpdata.data();
        for (int i = 0; i < upd.count; i++, bp++)
        {
            if (bp->bp_type == DBGBP_ENABLED)
                m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconBreakpoint, bp->line);
            else if (bp->bp_type == DBGBP_DISABLED)
                m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconBreakpointDisable, bp->line);
        }
    }
    else if (header.action == DBG_REQUEST_UPDATESTACK)
    {
        m_CallStackModel->clear();
        m_pCodeEditorProvider->clearEnterFunction();
        QByteArray data = m_pSocket->read(header.size - sizeof(DBGHEADER));

        int count = data.size() / sizeof(DBG_UPDATSTACK);

        DBG_UPDATSTACK *stack = (DBG_UPDATSTACK*)data.data();
        for (int i = 0; i < count; i++, stack++)
        {
            m_CallStackModel->append(stack);

            if (i != 0 && m_CurModuleInView == stack->module)
                m_pCodeEditorProvider->addEnterFunction(stack->line);
        }
    }
    else if (header.action == DBG_REQUEST_UPDATELOCALS)
    {
        QTextCodec *codec = QTextCodec::codecForName("Windows-1251"); // Windows-1250   IBM 866
        QByteArray data = m_pSocket->read(header.size - sizeof(DBGHEADER));
        QBuffer stream(&data);
        stream.open(QIODevice::ReadOnly);

        DBG_LOCALS locals;
        stream.read((char*)&locals, sizeof(DBG_LOCALS));

        //m_LocalsModel->clear();
        m_LocalsModel->startResetVarables();
        for (int i = 0; i < locals.var_count; i++)
        {
            DBG_VARIABLEDATA valdata;
            stream.read((char*)&valdata, sizeof(DBG_VARIABLEDATA));

            qstrcpy(valdata.str_name, codec->toUnicode(valdata.str_name).toStdString().c_str());
            qstrcpy(valdata.str_type, codec->toUnicode(valdata.str_type).toStdString().c_str());

            QString val;
            if (valdata.value_size)
            {
                char *value = new char[valdata.value_size];
                memset(value, 0, valdata.value_size);
                //QByteArray tmpval = stream.read(valdata.value_size);
                stream.read(value, valdata.value_size);
                //OemToCharA(tmpval.data(), value);
                val = codec->toUnicode(value);
                //qDebug() << "val" << val << value;
                delete[] value;
            }

            m_LocalsModel->append(&valdata, val);
        }
        m_LocalsModel->finishResetVarables();
    }
    else if (header.action == DBG_REQUEST_GETVALUENFO)
    {
        QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
        QByteArray data = m_pSocket->read(header.size - sizeof(DBGHEADER));
        QBuffer stream(&data);
        stream.open(QIODevice::ReadOnly);

        DBG_GETVALUENFO_RESULT resfata;
        stream.read((char*)&resfata, sizeof(DBG_GETVALUENFO_RESULT));

        QByteArray value = stream.read(resfata.size);
        VarWatchDlg dlg(this);
        dlg.setValue(codec->toUnicode(value));
        dlg.setVarName(codec->toUnicode(resfata.str_name));
        dlg.setVarType(codec->toUnicode(resfata.str_type));
        dlg.exec();
        //QInputDialog::getMultiLineText(this, "", "", codec->toUnicode(value));
    }

    bytesAvailable = m_pSocket->bytesAvailable();

    if (!m_LastHeader.isEmpty())
        m_LastHeader.pop();

    if (bytesAvailable)
        readyRead();
}
