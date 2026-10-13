#include "mainwindow.h"
#include "itemviewiconstyle.h"
#include "logevent.h"
#include <QCommandLineParser>
#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QIcon>
#include <QPixmap>
#include <QTimer>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPainter>

static QScopedPointer<QFile> m_logFile;
static QScopedPointer<MainWindow> pMainWindow;
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(m_logFile.data());
    QDateTime time = QDateTime::currentDateTime();
    out << time.toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type)
    {
    case QtInfoMsg:     out << "INF "; break;
    case QtDebugMsg:    out << "DBG "; break;
    case QtWarningMsg:  out << "WRN "; break;
    case QtCriticalMsg: out << "CRT "; break;
    case QtFatalMsg:    out << "FTL "; break;
    }

    out << context.category << ": "
        << msg << Qt::endl;

    out.flush();

    LogEvent event(time, type, context.category, msg);
    qApp->sendEvent(pMainWindow.data(), &event);
}

bool InitLogging(const QString &prefix, const QString &rules)
{
    QDir logDir(qApp->applicationDirPath());

    if (!logDir.cd("logs"))
    {
        logDir.mkdir("logs");
        logDir.cd("logs");
    }

    QString baseName = prefix + QString("_") + QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss_zzz");
    QString logFileName = QString("%1.txt")
                              .arg(logDir.absoluteFilePath(baseName))
                              .arg(prefix);

    m_logFile.reset(new QFile(logFileName));

    if (m_logFile->open(QFile::Append | QFile::Text))
    {
        //toolSetLoggingRules("*=true");
        QLoggingCategory::setFilterRules(rules);
        qInstallMessageHandler(messageHandler);
        //m_fLogging = true;
    }
    else
    {
        m_logFile.reset(Q_NULLPTR);
        //m_fLogging = false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(codeeditor);

    QApplication a(argc, argv);

    // Поправка прямоугольника иконок item view под стиль windowsvista:
    // иначе Scalable-иконки темы растягиваются по горизонтали.
    // Базовый стиль создаём заново — QApplication::setStyle удалит прежний,
    // а прокси заберёт владение новым.
    QStyle *baseStyle = QStyleFactory::create(a.style()->objectName());
    if (baseStyle)
        a.setStyle(new ItemViewIconStyle(baseStyle));

    pMainWindow.reset(new MainWindow());

    InitLogging("rsldbg", "rsldbg.*=true");
    QIcon::setThemeName("vswin2019");

    // ==== ДИАГНОСТИКА ИКОНОК (временно, удалить после разбора) ====
    foreach (const QString &name, QStringList() << "Variable" << "Class" << "CurrentLine" << "Breakpoint")
    {
        QIcon ico = QIcon::fromTheme(name);
        QPixmap pm16 = ico.pixmap(QSize(16, 16));
        QPixmap pm24 = ico.pixmap(QSize(24, 24));
        QPixmap pm32x16 = ico.pixmap(QSize(32, 16));
        qInfo() << "ICON-DIAG" << name
                << "null:" << ico.isNull()
                << "actual16:" << ico.actualSize(QSize(16, 16))
                << "pm16:" << pm16.size()
                << "pm24:" << pm24.size()
                << "pm32x16:" << pm32x16.size();
    }
    // ==== КОНЕЦ ДИАГНОСТИКИ ====

    // ==== ДИАГНОСТИКА ИКОНОК 2 (временно, удалить после разбора) ====
    QTimer::singleShot(0, []()
    {
        qInfo() << "ICON-DIAG2 style:" << qApp->style()->objectName()
                << "dpr:" << qApp->devicePixelRatio();

        // 1) QIcon::paint в квадратный и неквадратный rect — поведение Scalable
        {
            QPixmap canvas(64, 20);
            canvas.fill(Qt::white);
            QPainter p(&canvas);
            QIcon ico = QIcon::fromTheme("Variable");
            ico.paint(&p, QRect(0, 2, 16, 16));
            ico.paint(&p, QRect(20, 2, 28, 10));
            p.end();
            canvas.save(qApp->applicationDirPath() + "/diag_paint.png");
        }

        auto makeTree = [](const QString &file, bool withIconSize, bool withQss)
        {
            QTreeView tv;
            if (withIconSize)
                tv.setIconSize(QSize(16, 16));
            if (withQss)
            {
                QFile f(":/img/tree/treebranch.qss");
                if (f.open(QIODevice::ReadOnly))
                    tv.setStyleSheet(QString::fromUtf8(f.readAll()));
            }
            QStandardItemModel m;
            foreach (const QString &n, QStringList() << "Variable" << "Class" << "CurrentLine")
                m.appendRow(new QStandardItem(QIcon::fromTheme(n), n));
            tv.setModel(&m);
            tv.resize(220, 110);
            tv.grab().save(qApp->applicationDirPath() + "/" + file);
            qInfo() << "ICON-DIAG2" << file << "iconSize:" << tv.iconSize();
        };

        makeTree("diag_tree_16.png", true, false);
        makeTree("diag_tree_def.png", false, false);
        makeTree("diag_tree_qss.png", true, true);
    });

    // реальные view из доков — после show()
    QTimer::singleShot(1500, []()
    {
        int i = 0;
        foreach (QTreeView *v, pMainWindow->findChildren<QTreeView*>())
        {
            QWidget *dock = v->parentWidget();
            qInfo() << "ICON-DIAG2 realview" << i << (dock ? dock->metaObject()->className() : "?")
                    << "iconSize:" << v->iconSize()
                    << "qss:" << !v->styleSheet().isEmpty();
            v->grab().save(qApp->applicationDirPath() + QString("/diag_real_%1.png").arg(i));
            i++;
        }
    });
    // ==== КОНЕЦ ДИАГНОСТИКИ 2 ====

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption hostOption(QStringList() << "host",
                                  QCoreApplication::translate("main", "host to connect <port>."),
                                  QCoreApplication::translate("main", "host"));
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                  QCoreApplication::translate("main", "port to connect <port>."),
                                  QCoreApplication::translate("main", "port"));

    parser.addOption(hostOption);
    parser.addOption(portOption);
    parser.process(a);
    //qCritical(dbg()) << "test";

    if (!parser.isSet(hostOption) || !parser.isSet(portOption))
    {
        QMessageBox::critical(nullptr, QCoreApplication::translate("main", "Error"),
                              QCoreApplication::translate("main", "Сonnection parameters not specified"));
        a.quit();
        return -1;
    }

    if (!pMainWindow->connectToHost(parser.value(hostOption), parser.value(portOption).toShort()))
    {
        QMessageBox::critical(nullptr, QCoreApplication::translate("main", "Error"),
                              pMainWindow->lastError());
        a.quit();
    }

    pMainWindow->show();

    return a.exec();
}
