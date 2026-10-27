#include "mainwindow.h"
#include "macrorunner.h"

#include "codeeditor/codeeditor.h"
#include "codeeditor/codehighlighter.h"

#include <QCloseEvent>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QTextCodec>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : MainWindow(QString(), parent)
{
}

MainWindow::MainWindow(const QString &startFile, QWidget *parent)
    : QMainWindow(parent),
      m_running(false)
{
    m_pEditor = new CodeEditor(this);
    setCentralWidget(m_pEditor);

    ToolApplyHighlighter(m_pEditor, HighlighterRsl);

    // Output panel for macro run logs.
    m_pOutput = new QPlainTextEdit(this);
    m_pOutput->setReadOnly(true);
    m_pOutput->setMaximumBlockCount(1000);

    QDockWidget *outputDock = new QDockWidget(tr("Вывод"), this);
    outputDock->setObjectName(QStringLiteral("outputDock"));
    outputDock->setWidget(m_pOutput);
    addDockWidget(Qt::BottomDockWidgetArea, outputDock);

    createActions();
    createMenus();

    statusBar()->addWidget(new QLabel(tr("Кодировка: IBM 866 (DOS)"), this));

    connect(m_pEditor->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);

    setCurrentFile(QString());
    resize(900, 700);

    // Для удобства тестирования: при старте сразу открываем файл
    // (по умолчанию main.cpp подставляет bin/test/hello.mac)
    if (!startFile.isEmpty())
        loadFile(startFile);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

void MainWindow::createActions()
{
    // Actions are created in createMenus() directly.
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("Файл"));

    QAction *openAct = fileMenu->addAction(tr("Открыть..."), this, &MainWindow::open);
    openAct->setShortcut(QKeySequence::Open);

    QAction *saveAct = fileMenu->addAction(tr("Сохранить"), this, &MainWindow::save);
    saveAct->setShortcut(QKeySequence::Save);

    QAction *saveAsAct = fileMenu->addAction(tr("Сохранить как..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcut(QKeySequence::SaveAs);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("Выход"), this, &QWidget::close);
    exitAct->setShortcut(QKeySequence::Quit);

    QMenu *runMenu = menuBar()->addMenu(tr("Выполнить"));

    QAction *runAct = runMenu->addAction(tr("Выполнить (с отладкой)"), this, &MainWindow::run);
    runAct->setShortcut(QKeySequence(Qt::Key_F5));

    QAction *runNoDbgAct = runMenu->addAction(tr("Выполнить без отладки"), this, &MainWindow::runWithoutDebug);
    runNoDbgAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F5));
}

bool MainWindow::maybeSave()
{
    if (!m_pEditor->document()->isModified())
        return true;

    QMessageBox::StandardButton ret = QMessageBox::warning(
        this, tr("RSL Editor"),
        tr("Документ был изменен.\nСохранить изменения?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return save();
    else if (ret == QMessageBox::Cancel)
        return false;

    return true;
}

void MainWindow::open()
{
    if (!maybeSave())
        return;

    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Открыть макрос"), QString(), tr("RSL макросы (*.mac);;Все файлы (*)"));

    if (!fileName.isEmpty())
        loadFile(fileName);
}

bool MainWindow::save()
{
    if (m_CurrentFile.isEmpty())
        return saveAs();

    return saveFile(m_CurrentFile);
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Сохранить макрос"), m_CurrentFile, tr("RSL макросы (*.mac);;Все файлы (*)"));

    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("RSL Editor"),
                             tr("Не удалось открыть файл %1:\n%2")
                                 .arg(fileName, file.errorString()));
        return false;
    }

    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    QByteArray data = file.readAll();
    QString text = codec->toUnicode(data);

    m_pEditor->setPlainText(text);
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Файл загружен"), 2000);
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("RSL Editor"),
                             tr("Не удалось сохранить файл %1:\n%2")
                                 .arg(fileName, file.errorString()));
        return false;
    }

    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    QByteArray data = codec->fromUnicode(m_pEditor->toPlainText());

    qint64 written = file.write(data);
    file.close();

    if (written != data.size())
    {
        QMessageBox::warning(this, tr("RSL Editor"),
                             tr("Ошибка записи в файл %1").arg(fileName));
        return false;
    }

    m_pEditor->document()->setModified(false);
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Файл сохранен"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_CurrentFile = fileName;
    m_pEditor->document()->setModified(false);

    setWindowModified(false);

    if (m_CurrentFile.isEmpty())
    {
        setWindowTitle(tr("RSL Editor[*]"));
    }
    else
    {
        setWindowTitle(tr("%1[*] - RSL Editor").arg(QFileInfo(m_CurrentFile).fileName()));
    }
}

void MainWindow::documentWasModified()
{
    setWindowModified(m_pEditor->document()->isModified());
}

void MainWindow::run()
{
    runMacro(true);
}

void MainWindow::runWithoutDebug()
{
    runMacro(false);
}

void MainWindow::runMacro(bool debug)
{
    if (m_CurrentFile.isEmpty() || !save())
    {
        QMessageBox::information(this, tr("RSL Editor"),
                                 tr("Сохраните макрос перед запуском."));
        return;
    }

    if (m_running)
    {
        QMessageBox::information(this, tr("RSL Editor"),
                                 tr("Макрос уже выполняется."));
        return;
    }

    m_pOutput->clear();

    // The macro runs on the GUI thread inside this process: RunRSL is a
    // blocking call (same as in RslTest), the runner's action proc pumps
    // Qt events so the window stays responsive. In debug mode rsldbg.dll
    // is installed into the runtime and spawns rsldbgclnt itself.
    m_running = true;

    MacroRunner runner(m_CurrentFile, debug, this);
    connect(&runner, &MacroRunner::logMessage,
            this, &MainWindow::appendRunLog);

    statusBar()->showMessage(debug ? tr("Макрос запущен с отладкой")
                                   : tr("Макрос запущен"), 3000);

    int exitCode = runner.exec();

    m_running = false;

    statusBar()->showMessage(exitCode == 0 ? tr("Макрос завершен успешно")
                                           : tr("Макрос завершен с ошибкой"), 5000);
}

void MainWindow::appendRunLog(const QString &text)
{
    m_pOutput->appendPlainText(text);
}
