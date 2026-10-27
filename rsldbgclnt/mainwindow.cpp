#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include "mainwindow.h"
#include "windowtitlebar.h"
#include "SARibbon.h"
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
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <QTreeView>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <varwatchdlg.h>
//#include <QIODevice>

// GUI-реализация DbgClient: переопределяет обработчики сообщений сервера
// и обновляет окно отладчика (MainWindow объявляет его дружественным).
class GuiDbgClient : public DbgClient
{
public:
    explicit GuiDbgClient(MainWindow *wnd)
        : DbgClient(wnd), m_Wnd(wnd)
    {
    }

protected:
    void onEvent(const QJsonObject &obj) Q_DECL_OVERRIDE         { m_Wnd->handleEvent(obj); }
    void onUpdateText(const QJsonObject &obj) Q_DECL_OVERRIDE    { m_Wnd->handleUpdateText(obj); }
    void onUpdateBreakpoints(const QJsonObject &obj) Q_DECL_OVERRIDE { m_Wnd->handleUpdateBreakpoints(obj); }
    void onUpdateStack(const QJsonObject &obj) Q_DECL_OVERRIDE   { m_Wnd->handleUpdateStack(obj); }
    void onUpdateLocals(const QJsonObject &obj) Q_DECL_OVERRIDE  { m_Wnd->handleUpdateLocals(obj); }
    void onValueInfo(const QJsonObject &obj) Q_DECL_OVERRIDE     { m_Wnd->handleValueInfo(obj); }

private:
    MainWindow *m_Wnd;
};

// Фирменная палитра RS-Bank V.6 (по Bank256.bmp): бирюза + оранжевый акцент.
// Макросы-литералы, чтобы конкатенироваться со строками QSS на этапе компиляции.
#define RSB_BRAND_COLOR   "#0E6B65" // основной: заголовок, статусбар, окантовка
#define RSB_BRAND_HOVER   "#178E88" // hover пунктов меню и системных кнопок
#define RSB_BRAND_PRESSED "#093F3B" // нажатое состояние
#define RSB_ACCENT_COLOR  "#FCB332" // оранжевая полоса (как внизу Bank256)
#define RSB_CLOSE_HOVER   "#E81123" // кнопка закрытия — системный красный
#define RSB_CLOSE_PRESSED "#F1707A"

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
    m_pFindDialog(nullptr)
{

    ui->setupUi(this);
    setIconSize(QSize(16, 16));

    // Протокольный клиент: транспорт и handshake живут в DbgClient,
    // GUI-реакция на сообщения - в GuiDbgClient -> handle*-методах
    m_Client = new GuiDbgClient(this);

    // Окно без системной рамки: заголовок рисуем сами
    // (меню в заголовке, системные кнопки SARibbon справа).
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowTitle(tr("RSL Debugger"));
    setWindowIcon(QIcon::fromTheme("AppLogo"));
    setContentsMargins(1, 1, 1, 1); // 1px под окантовку в цвет темы
    InitTitleBar();

    // Статус-бар в цвет заголовка + фирменная оранжевая полоса сверху
    // (как оранжевая полоса внизу Bank256)
    ui->statusbar->setStyleSheet(QString::fromLatin1(
        "QStatusBar { background-color: " RSB_BRAND_COLOR "; color: #FFFFFF;"
        " border-top: 2px solid " RSB_ACCENT_COLOR "; }"
        "QStatusBar::item { border: none; }"));

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

    // Фирменная тема подсветки (регистрируется статически из qrc)
    ToolApplyHighlighter(m_pCodeEditor, HighlighterRsl, QStringLiteral("RS-Bank"));

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

    connect(m_Client, &DbgClient::connected, this, []()
    {
        qInfo(dbg()) << "Connected...";
    });
    connect(m_Client, &DbgClient::disconnected, this, []()
    {
        qInfo(dbg()) << "Disconnected...";
    });
    connect(m_Client, &DbgClient::disconnected, this, &MainWindow::close);
    connect(m_LocalsDockWidget.data(), &VarWatchDockWidget::expandVariable, this, &MainWindow::expandVariable);
    connect(m_LocalsDockWidget.data(), &VarWatchDockWidget::showVarValue, this, &MainWindow::showVarValue);
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug(dbg()) << "~MainWindow()";
}

// Заголовок в стиле Visual Studio: классические меню в строке заголовка,
// системные кнопки SARibbon поверх неё справа.
void MainWindow::InitTitleBar()
{
    m_TitleMenuBar = new WindowTitleMenuBar(this);
    m_TitleMenuBar->setStyleSheet(QString::fromLatin1(
        "QMenuBar { background-color: " RSB_BRAND_COLOR "; color: #FFFFFF; padding-left: 28px; }"
        "QMenuBar::item { background: transparent; padding: 5px 9px; }"
        "QMenuBar::item:selected { background-color: " RSB_BRAND_HOVER "; }"
        "QMenuBar::item:pressed { background-color: " RSB_BRAND_PRESSED "; }"
        // Выпадающие меню: выделенный пункт — оранжевый акцент бренда
        "QMenu { background-color: #FFFFFF; border: 1px solid " RSB_BRAND_COLOR "; }"
        "QMenu::item:selected { background-color: " RSB_ACCENT_COLOR "; color: #1A1A1A; }"));

    QMenu *menuFile = m_TitleMenuBar->addMenu(tr("File"));
    menuFile->addAction(ui->actionExit);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    QMenu *menuEdit = m_TitleMenuBar->addMenu(tr("Edit"));
    menuEdit->addAction(ui->actionFind);
    menuEdit->addAction(ui->actionFindNext);

    QMenu *menuDebug = m_TitleMenuBar->addMenu(tr("Debug"));
    menuDebug->addAction(ui->actionResume);
    menuDebug->addAction(ui->actionStop);
    menuDebug->addSeparator();
    menuDebug->addAction(ui->actionStepInto);
    menuDebug->addAction(ui->actionStepOver);
    menuDebug->addAction(ui->actionStepOut);

    setMenuBar(m_TitleMenuBar);

    // Кнопки свернуть/развернуть/закрыть. Родителем должно быть главное
    // окно — слоты SARibbonSystemButtonBar вызывают parentWidget()->close() и т.п.
    // Флаги задаём явно: у frameless-окна нет Min/Max/CloseButtonHint,
    // и по флагам родителя кнопки бы не создались.
    m_SysButtons = new SARibbonSystemButtonBar(this, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    m_SysButtons->setStyleSheet(QString::fromLatin1(
        "SARibbonSystemButtonBar { background: transparent; }"
        "SARibbonSystemButtonBar QToolButton { background: transparent; border: none; }"
        "SARibbonSystemButtonBar QToolButton:hover { background-color: " RSB_BRAND_HOVER "; }"
        "SARibbonSystemButtonBar QToolButton:pressed { background-color: " RSB_BRAND_PRESSED "; }"
        "SARibbonSystemButtonBar QToolButton#SACloseWindowButton:hover { background-color: " RSB_CLOSE_HOVER "; }"
        "SARibbonSystemButtonBar QToolButton#SACloseWindowButton:pressed { background-color: " RSB_CLOSE_PRESSED "; }"));

    // Иконки SARibbon сам не назначает (рассчитано на QSS `image:`,
    // но SVG без width/height имеет натуральный размер 1024px) —
    // ставим QIcon, он масштабирует SVG под iconSize.
    const QSize iconSize(10, 10); // глифы размером как в заголовке VS

    if (QToolButton *btn = m_SysButtons->findChild<QToolButton*>(QStringLiteral("SAMinimizeWindowButton")))
    {
        btn->setIcon(QIcon(QStringLiteral(":/SARibbon/image/resource/Titlebar_Min.svg")));
        btn->setIconSize(iconSize);
    }

    if (QToolButton *btn = m_SysButtons->findChild<QToolButton*>(QStringLiteral("SACloseWindowButton")))
    {
        btn->setIcon(QIcon(QStringLiteral(":/SARibbon/image/resource/Titlebar_Close.svg")));
        btn->setIconSize(iconSize);
    }

    updateMaxButtonIcon();

    updateSystemButtonsGeometry();
}

void MainWindow::updateMaxButtonIcon()
{
    if (!m_SysButtons)
        return;

    QToolButton *btn = m_SysButtons->findChild<QToolButton*>(QStringLiteral("SAMaximizeWindowButton"));

    if (!btn)
        return;

    const QString res = isMaximized()
        ? QStringLiteral(":/SARibbon/image/resource/Titlebar_Normal.svg")
        : QStringLiteral(":/SARibbon/image/resource/Titlebar_Max.svg");

    btn->setIcon(QIcon(res));
    btn->setIconSize(QSize(10, 10));
}

void MainWindow::updateSystemButtonsGeometry()
{
    if (!m_SysButtons || !m_TitleMenuBar)
        return;

    m_SysButtons->setWindowTitleHeight(m_TitleMenuBar->height());

    const QSize hint = m_SysButtons->sizeHint();
    const int w = qMax(hint.width(), 105); // 3 кнопки по 35px как минимум

    m_SysButtons->setGeometry(width() - w, 0, w, m_TitleMenuBar->height());
    m_SysButtons->raise();
    m_TitleMenuBar->setRightReserved(w);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateSystemButtonsGeometry();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (m_SysButtons)
        {
            m_SysButtons->setWindowStates(windowState());
            updateMaxButtonIcon();
        }

        // Резервируем 1px под окантовку только в нормальном состоянии
        const bool borderless = isMaximized() || isFullScreen();
        setContentsMargins(borderless ? 0 : 1, borderless ? 0 : 1,
                           borderless ? 0 : 1, borderless ? 0 : 1);
        update();
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    static bool framePrepared = false;

    if (!framePrepared)
    {
        // Qt создаёт frameless-окно как чистый WS_POPUP — без WS_THICKFRAME
        // Windows не даёт менять размер и разворачивать окно двойным кликом.
        // Возвращаем стили вручную (визуально рамка не появляется).
        const HWND hwnd = reinterpret_cast<HWND>(winId());
        const LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
        SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

        // Тень у окна с WS_THICKFRAME рисует сам DWM.
        // DwmExtendFrameIntoClientArea здесь НЕ используем: расширенная
        // рамка рисуется DWM поверх крайнего пикселя клиентской области
        // и перекрывает нашу окантовку в цвет темы.

        // Windows 11 скругляет углы всем окнам — отключаем, нужны прямые
        // (на Windows 10 вызов просто вернёт ошибку, игнорируем)
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
        DWORD cornerPref = 1; // DWMWCP_DONOTROUND
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

        framePrepared = true;
    }
}

// Тонкая окантовка в цвет темы (в maximized-состоянии не рисуем —
// там contentsMargins нулевые и рамка всё равно за экраном)
void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);

    if (!isMaximized() && !isFullScreen())
    {
        QPainter p(this);
        p.setPen(QColor(QString::fromLatin1(RSB_BRAND_COLOR)));
        p.drawRect(rect().adjusted(0, 0, -1, -1));
    }
}

// Hit-test заголовка и краёв frameless-окна.
// Края дают изменение размера, полоса заголовка (кроме пунктов меню
// и системных кнопок) — перетаскивание, Aero Snap и разворачивание
// двойным кликом.
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG *msg = static_cast<MSG*>(message);

    // Qt маршрутизирует nativeEvent по hwnd — сюда приходят только сообщения
    // этого окна. Сравнивать msg->hwnd с winId() НЕЛЬЗЯ: winId() форсирует
    // создание платформенного окна, а первое сообщение (WM_SETICON из
    // конструктора QWindowsWindow) приходит, когда оно ещё создаётся —
    // получается бесконечная рекурсия создания окна.
    if (msg->message == WM_NCCALCSIZE)
    {
        // WS_THICKFRAME добавил неклиентскую область, которую DefWindowProc
        // закрашивает белым фоном класса окна (белая полоса сверху).
        // Возврат 0: клиентская область = весь прямоугольник окна.
        if (!msg->wParam) // простой RECT: по документации результат 1
        {
            *result = 1;
            return true;
        }

        if (IsZoomed(msg->hwnd))
        {
            // Развёрнутое окно: ограничить клиент рабочей областью монитора,
            // чтобы не перекрывать панель задач.
            NCCALCSIZE_PARAMS *params = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            HMONITOR monitor = MonitorFromWindow(msg->hwnd, MONITOR_DEFAULTTONULL);

            if (monitor)
            {
                MONITORINFO info{sizeof(info)};

                if (GetMonitorInfoW(monitor, &info))
                    params->rgrc[0] = info.rcWork;
            }
        }

        *result = 0;
        return true;
    }

    if (msg->message == WM_NCHITTEST)
    {
        RECT winRect{};
        GetWindowRect(msg->hwnd, &winRect);

        const qreal dpr = devicePixelRatioF();
        const QPoint local(qRound((GET_X_LPARAM(msg->lParam) - winRect.left) / dpr),
                           qRound((GET_Y_LPARAM(msg->lParam) - winRect.top) / dpr));
        const int w = qRound((winRect.right - winRect.left) / dpr);
        const int h = qRound((winRect.bottom - winRect.top) / dpr);

        if (!isMaximized() && !isFullScreen())
        {
            const int bw = 5;
            const bool onLeft   = local.x() >= 0 && local.x() < bw;
            const bool onRight  = local.x() >= w - bw && local.x() < w;
            const bool onTop    = local.y() >= 0 && local.y() < bw;
            const bool onBottom = local.y() >= h - bw && local.y() < h;

            if (onTop && onLeft)        { *result = HTTOPLEFT;     return true; }
            if (onTop && onRight)       { *result = HTTOPRIGHT;    return true; }
            if (onBottom && onLeft)     { *result = HTBOTTOMLEFT;  return true; }
            if (onBottom && onRight)    { *result = HTBOTTOMRIGHT; return true; }
            if (onLeft)                 { *result = HTLEFT;        return true; }
            if (onRight)                { *result = HTRIGHT;       return true; }
            if (onTop)                  { *result = HTTOP;         return true; }
            if (onBottom)               { *result = HTBOTTOM;      return true; }
        }

        QList<QRect> excluded = m_TitleMenuBar->interactiveRects();

        if (m_SysButtons)
            excluded.append(m_SysButtons->geometry());

        if (SA::isTitleBarDragArea(local, QRect(0, 0, w, h),
                                   m_TitleMenuBar->height(), excluded,
                                   isMaximized() || isFullScreen()))
        {
            *result = HTCAPTION;
            return true;
        }
    }

    return QMainWindow::nativeEvent(eventType, message, result);
}

QString MainWindow::lastError()
{
    return m_Client->lastError();
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
        m_Client->execContinue(EXECCONTNUE_STEP_OVER);
    });

    connect(ui->actionStepInto, &QAction::triggered, [=]()
    {
        m_Client->execContinue(EXECCONTNUE_STEP_INTO);
    });

    connect(ui->actionStepOut, &QAction::triggered, [=]()
    {
        m_Client->execContinue(EXECCONTNUE_STEP_OUT);
    });

    connect(ui->actionResume, &QAction::triggered, [=]()
    {
        m_Client->execContinue(EXECCONTNUE_DBG_RUN);
    });

    connect(ui->actionStop, &QAction::triggered, [=]()
    {
        m_Client->execContinue(EXECCONTNUE_DBG_ABORT);
    });
}

bool MainWindow::connectToHost(const QString &hostName, quint16 port, const QString &authToken)
{
    m_Client->setAuthToken(authToken);
    return m_Client->connectToHost(hostName, port);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Аналог команды Stop с тулбара: перед закрытием шлём отладчику abort,
    // чтобы исполняемый макрос не остался подвешенным вместе с процессом-клиентом
    m_Client->stop();
    m_Client->disconnectFromHost();
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
        // Текущая инструкция — осветлённый фирменный оранжевый (#FCB332 + белый),
        // тёмный текст под ним остаётся читаемым
        format.setBackground(QColor("#FDD78F"));

        m_pCodeEditor->clearUserSelection();
        m_pCodeEditor->appendUserSelection(cursor, format);
    }
}

void MainWindow::expandVariable(const int &index, const qint64 &stack)
{
    QModelIndex Selected = m_StackDockWidget->view()->selectionModel()->currentIndex();

    m_Client->expandVariable(index, stack, Selected.isValid() ? Selected.row() : 0);
}

void MainWindow::showVarValue(const qint64 &val, const qint64 &info)
{
    m_Client->showVarValue(val, info);
}

// Обработчики сообщений сервера - вызываются из GuiDbgClient.
// Разбор кадра (заголовок, размер, JSON) уже выполнен в DbgClient.

void MainWindow::handleEvent(const QJsonObject &obj)
{
    if (obj.value("event").toString() == QLatin1String("breakpoint"))
        qDebug(dbg()) << "Handle breakpoint event. Is setted:" << obj.value("set").toBool();
    else
        qDebug(dbg()) << "Handle unknown event:" << obj.value("event").toString();
}

void MainWindow::handleUpdateText(const QJsonObject &obj)
{
    // Протокол v2: текст и имя файла уже в Unicode (UTF-8 JSON).
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

void MainWindow::handleUpdateBreakpoints(const QJsonObject &obj)
{
    qInfo(dbg()) << "Update breakpoints requested";

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

void MainWindow::handleUpdateStack(const QJsonObject &obj)
{
    m_CallStackModel->clear();
    m_pCodeEditorProvider->clearEnterFunction();

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

void MainWindow::handleUpdateLocals(const QJsonObject &obj)
{
    // Протокол v2: переменные в JSON (UTF-8), хендлы - строками,
    // чтобы не терять точность в double.
    const QJsonArray vars = obj.value("vars").toArray();

    m_LocalsModel->startResetVarables();

    for (const QJsonValue &entry : vars)
    {
        const QJsonObject varObj = entry.toObject();

        VarWatchData var;
        var.index = varObj.value("index").toInt();
        var.name = varObj.value("name").toString();
        var.type = varObj.value("type").toString();
        var.proc = varObj.value("proc").toString();
        var.depth = varObj.value("depth").toInt(1);
        var.isObject = varObj.value("object").toBool();
        var.isExpanded = varObj.value("expanded").toBool();
        var.isLvalue = varObj.value("lvalue").toBool();
        var.isFakeChildren = varObj.value("fakeChildren").toBool();

        var.val  = varObj.value("val").toString().toLongLong();
        var.exp  = varObj.value("exp").toString().toLongLong();
        var.st   = varObj.value("st").toString().toLongLong();
        var.procHandle = varObj.value("procHandle").toString().toLongLong();
        var.info = varObj.value("info").toString().toLongLong();
        var.mod  = varObj.value("mod").toString().toLongLong();

        m_LocalsModel->append(var, varObj.value("value").toString());
    }

    m_LocalsModel->finishResetVarables();
}

void MainWindow::handleValueInfo(const QJsonObject &obj)
{
    // Протокол v2: name/type/value уже в Unicode.
    VarWatchDlg dlg(this);
    dlg.setValue(obj.value("value").toString());
    dlg.setVarName(obj.value("name").toString());
    dlg.setVarType(obj.value("type").toString());
    dlg.exec();
}
