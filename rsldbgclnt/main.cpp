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
