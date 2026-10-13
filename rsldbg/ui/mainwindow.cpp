#include "cdebug.h"
#include "saver.h"
#include "mainwindow.h"
#include "ui/dbgeditorlinewidgetprovider.h"
#include "ui_mainwindow.h"
#include "rsl/dbgintf.h"
#include "rsldbg.h"
//#include "rsl/idebug.h"
#include "events/dbgbreakpointevent.h"
#include "ui/callstackdockwidget.h"
#include "models/callstackmodel.h"
#include <QFile>
#include <QTextStream>
#include <codeeditor/codeeditor.h>
#include <codeeditor/cppcodehighlighter.h>
#include <QDebug>
#include <QMessageBox>

#define tracemsg (*((CTraceMsg*)m_tracemsg))

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_curdbg(nullptr),
    ui(new Ui::MainWindow),
    m_pCallStackDock(new CallStackDockWidget())
{
    ui->setupUi(this);
    InitMainToolBar();
    InitDebugToolBar();

    m_pCodeEditor = new CodeEditor(this);
    m_pCodeEditor->setReadOnly(true);
    m_pCodeEditor->setAutoHighlightCurrentLine(false);
    setCentralWidget(m_pCodeEditor);
    m_tracemsg = new CTraceMsg();

    m_pCodeEditorProvider = new DbgEditorLineWidgetProvider();
    m_pCodeEditor->setCodeEditorLineWidgetProvider(m_pCodeEditorProvider);

    ToolApplyHighlighter(m_pCodeEditor, HighlighterRsl);

    m_pStackModel = m_pCallStackDock->model();
    addDockWidget(Qt::LeftDockWidgetArea, m_pCallStackDock.data());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitMainToolBar()
{
    m_pRunAction = ui->toolBar->addAction(QIcon::fromTheme("Run"), tr("Run"));
    m_pStopAction = ui->toolBar->addAction(QIcon::fromTheme("Stop"), tr("Stop debug"));
}

void MainWindow::InitDebugToolBar()
{
    ui->actionStepInto->setIcon(QIcon::fromTheme("StepInto"));
    ui->actionStepInto->setShortcut(QKeySequence(tr("F11")));

    ui->actionStepOver->setIcon(QIcon::fromTheme("StepOver"));
    ui->actionStepOver->setShortcut(QKeySequence(tr("F10")));

    ui->actionStepOut->setIcon(QIcon::fromTheme("StepOut"));
    ui->actionStepOut->setShortcut(QKeySequence(tr("Shift+F11")));

    ui->debugToolBar->addAction(ui->actionStepInto);
    ui->debugToolBar->addAction(ui->actionStepOver);
    ui->debugToolBar->addAction(ui->actionStepOut);

    connect(ui->actionStepOver, &QAction::triggered, [=]()
    {
        qDebug() << "actionStepOver";
        exec_continue(RSL_STEP_OVER);
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
    QMainWindow::closeEvent(event);
}

QString MainWindow::ReadTextFileContent(const QString &filename, const QString &encode)
{
    QString content;
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (encode.isEmpty())
            content = f.readAll();
        else
        {
            QTextStream stream(&f);
            stream.setCodec(encode.toLocal8Bit().data());

            content = stream.readAll();
        }

        f.close();
    }
    return content;
}

void MainWindow::AddTraceMsg(const QString &str, const int &level)
{
    TTraceMsg* msg = new TTraceMsg;
    msg->msg = str;
    msg->level = (MSGLEVEL)level;

    elem_tracemsg msg_aptr (msg);
    tracemsg.push_back(msg_aptr);

    qDebug() << str;
}

void MainWindow::UpdateStack()
{
    m_pStackModel->clear();

    int rv = m_curdbg->GetStackCount();
    for (int i = 0; i < rv; i++)
    {
        //QString proc = m_curdbg->GetProcName(m_curdbg->GetStackAt(i));
        m_pStackModel->append(m_curdbg->GetStackAt(i));
        //m_wndCallStack.m_wndCallStackList.AddString (buf);
    }

    // m_wndCallStack.m_wndCallStackList.SetCurSel (0);
}

void MainWindow::ProcessBreakPoint(DbgBreakpointEvent *ev)
{
    /*
     * KillTimer (1);

   HWND wnd = ::GetForegroundWindow ();

   if (wnd != *this)
      ActiveBeforeDbg = wnd;

   // Принадлежит ли окно верхнего уровня отлаживаемому процессу?
   if (ActiveBeforeDbg && IsWindow (ActiveBeforeDbg))
      {
      DWORD pid;
      GetWindowThreadProcessId  (ActiveBeforeDbg, &pid);
      if (pid != GetCurrentProcessId  ())
         ActiveBeforeDbg = NULL;
      }

   ::ReplyMessage((LRESULT)TRUE);

   SetForegroundWindow();

   if (needRefresh)
      {
      SetWindowPlacement (&wndPlacement);
      needRefresh = false;
      }
     * */
    newDbg = (m_curdbg != ev->dbg());
    m_curdbg = ev->dbg();
    //m_curdbg->SetChildsLimit (m_dbgregistry.GetLimitChilds ());

    TBpData* cur_bpdata = (TBpData*)ev->data();
    m_curdbg->GetRemoteVersion();

    if (m_curdbg->inInteractiveExp)
    {
        char buff [512] = { '\0' };
        m_curdbg->inInteractiveExp = false;
        unsigned short stat = m_curdbg->do_GetExpInteractiveStatus ();

        if (stat == INTR_EXP_ERROR)
        {
            m_curdbg->do_GetLastErrorText(buff, sizeof buff);
            //s.LoadString (IDS_INTEREXPERR);
            //MessageBox (buff, s, MB_OK | MB_ICONERROR);
            QMessageBox::critical(this, "IDS_INTEREXPERR", buff);
        }
        else if (stat == INTR_EXP_OK)
        {
            m_curdbg->do_GetLastErrorText (buff, sizeof buff);
            //s.LoadString (IDS_INTERRESULT);
            //MessageBox (buff, s, MB_OK);
            QMessageBox::critical(this, "IDS_INTERRESULT", buff);
        }
    }

    m_curdbg->UpdateDbgInfo();
    m_curdbg->SetDebugState(true);
    m_curdbg->SetCurModule((RSLMODULE)-1);
    m_curdbg->GetSaver()->Load();

    QString str_trace = "trace";
    m_curdbg->SetIndex(0);

    /*QString framename = frame_name();
    framename += " - ";
    framename += str_trace;
    setWindowTitle(framename);*/

    int offs, len;
    if (ev->dbg() && ev->data() && cur_bpdata->bp_type == BP_INVISIBLE)
    {
        m_curdbg->do_ClrBreakPoint(m_curdbg->GetCurStatement (&offs, &len));
        m_curdbg->DelBp(cur_bpdata);
    }

    m_curdbg->SetIndex(0);
    UpdateDbgInfo(0);
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

        QLinearGradient gradient;
        gradient.setColorAt(0.0, QColor("#fb000d")); //#c34222
        gradient.setColorAt(1.0, QColor("#a30008")); //

        gradient.setStart(1, 100);
        gradient.setFinalStop(1, 200);

        QTextCharFormat format;
        format.setBackground(gradient);
        m_pCodeEditor->appendUserSelection(cursor, format);
    }
}

void MainWindow::UpdateDbgInfo(int index)
{
    UpdateStack();
    UpdateWatch();
    UpdateText(index);
}

void MainWindow::UpdateText(int index)
{
    int offs = 0;
    int len = 0;
    int line;
    RSLSRC hsrc;
    RSLMODULE mod = m_curdbg->GetCurModule(index);
    RSLSTMT statement = m_curdbg->GetCurStatement(&offs, &len, index);
    line = m_curdbg->GetStatementLine(offs, len, mod);

    auto updateBreakPoints = [=]()
    {
        TBpData* bp;
        int nBP = m_curdbg->GetBPCount();
        for (int i = 0; i < nBP; ++i)
        {
            bp = m_curdbg->GetBP (i);
            if (mod == bp->mod)
            {
                if (bp->bp_type == BP_DISABLED)
                    m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconBreakpointDisable, bp->line);
                else if (bp->bp_type == BP_ENABLED)
                    m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconBreakpoint, bp->line);
                /*if (bp->bp_type == BP_DISABLED)
                    color = RGB (0, 0, 255);
                else if (bp->bp_type == BP_ENABLED)
                    color = RGB(0, 255, 0);
                else
                    color = GetSysColor (COLOR_WINDOW);

                SetTextColor (bp->offs, bp->len, color, bp->line);*/
            }
        }
    };

    if (m_curModuleInView == mod)
    {
        if (m_lastStmt)
        {
            updateBreakPoints();
            applyCurrentStatement(offs, len, line);

            m_lastOffs  = offs;
            m_lastLen   = len;
            m_lastLine  = line;
            m_lastStmt  = statement;
            m_curModuleInView = mod;
        }
    }
    else
    {
        int encode = -1;
        int isBtrStream = 0;
        char *filename = RslGetModuleFile((Qt::HANDLE)mod, &isBtrStream);

        QString content = ReadTextFileContent(filename, "IBM 866");
        m_pCodeEditor->setPlainText(content);

        int line = m_curdbg->GetStatementLine(offs, len, mod);
        m_pCodeEditor->setCurrentHighlightLine(line);
        m_pCodeEditorProvider->addItemId(DbgEditorLineWidgetProvider::IconCurrentLine, line);

        QTextBlock block = m_pCodeEditor->document()->findBlockByLineNumber(line);
        if (block.isValid())
        {
            QTextCursor cursor(block);
            m_pCodeEditor->setTextCursor(cursor);
        }

        updateBreakPoints();
        applyCurrentStatement(offs, len, line);

        m_lastOffs  = offs;
        m_lastLen   = len;
        m_lastLine  = line;
        m_lastStmt  = statement;
        m_curModuleInView = mod;

        //bool rv = m_curdbg->do_OpenSrc (mod, &hsrc, &encode);
        //m_curdbg->do_CloseSrc(hsrc);
        /*if (hsrc <= 0)
        {
            TRACE("do_OpenSrc returned bad value\n");
            if (m_curdbg->GetStackSize () == 1)
                exec_continue(RSL_STEP_OUT);
            else if (m_curdbg->GetStackSize () > 1)
                exec_continue(RSL_STEP_OVER);
            return;
        }*/
    }
}

void MainWindow::UpdateText(RSLMODULE mod, bool notActivModule)
{

}

void MainWindow::UpdateWatch()
{
    m_curdbg->UpdateSurvey();
    ShowSurvey();
}

void MainWindow::ShowSurvey()
{

}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == MSG_BREAKPOINT)
    {
        DbgBreakpointEvent *ev = dynamic_cast<DbgBreakpointEvent*>(event);
        ProcessBreakPoint(ev);
    }

    return QMainWindow::event(event);
}

void MainWindow::exec_continue(int trace_log)
{
    /*CString framename = frame_name ();

    if (!framename.IsEmpty ())
    {
        CString str_run;
        if (!str_run.LoadString( IDR_STR_RUN))
            str_run = "run";
        framename += " - ";
        framename += str_run;
    }

    SetWindowText (framename);*/
    //m_curdbg->st
    rslDbgObj()->stoploop();
    m_curdbg->SetDebugState(false);
    m_curdbg->do_ExecContinue(trace_log);

    /*if (ActiveBeforeDbg)
        SetTimer (1, 400, NULL);*/
}
