#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLoggingCategory>
#include <QStack>
//#define IGNORE_INCLUDE
#include "dbgserverproto.h"
#include "dbgclient.h"

Q_DECLARE_LOGGING_CATEGORY(dbg)

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class CodeEditor;
class QJsonObject;
class LogEventModel;
class StdViewDockWidget;
class CallStackModel;
class VarWatchModel;
class VarWatchDockWidget;
class FindDialog;
class DbgEditorLineWidgetProvider;
class WindowTitleMenuBar;
class SARibbonSystemButtonBar;
class GuiDbgClient;
class MainWindow : public QMainWindow
{
    Q_OBJECT

    // GuiDbgClient (наследник DbgClient) вызывает handle*-методы
    // и обращается к GUI-членам напрямую
    friend class GuiDbgClient;

public:
    enum DBG_EVENTS
    {
        //EventBreakPoint = MSG_BREAKPOINT,
    };
    Q_ENUM(DBG_EVENTS);

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool connectToHost(const QString &hostName, quint16 port, const QString &authToken = QString());
    QString lastError();

public slots:

private slots:
    void expandVariable(const int &index, const qint64 &stack);
    void showVarValue(const qint64 &val, const qint64 &info);
    void onFind();
    void onFindNext();

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual bool event(QEvent *event) Q_DECL_OVERRIDE;
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:

    void InitTitleBar();
    void updateSystemButtonsGeometry();
    void updateMaxButtonIcon();
    void InitDebugToolBar();
    void InitOtherToolBars();
    void applyCurrentStatement(const int &offs, const int &len, const int &line);

    // Обработчики сообщений сервера - вызываются из GuiDbgClient
    void handleEvent(const QJsonObject &obj);
    void handleUpdateText(const QJsonObject &obj);
    void handleUpdateBreakpoints(const QJsonObject &obj);
    void handleUpdateStack(const QJsonObject &obj);
    void handleUpdateLocals(const QJsonObject &obj);
    void handleValueInfo(const QJsonObject &obj);

    Ui::MainWindow *ui;
    qint64 m_CurModuleInView;
    GuiDbgClient *m_Client = nullptr;
    QScopedPointer<LogEventModel> m_LogModel;
    QScopedPointer<CallStackModel> m_CallStackModel;

    QScopedPointer<VarWatchModel> m_LocalsModel;

    QScopedPointer<StdViewDockWidget> m_LogDockWidget;
    QScopedPointer<StdViewDockWidget> m_StackDockWidget;
    QScopedPointer<VarWatchDockWidget> m_LocalsDockWidget;

    CodeEditor *m_pCodeEditor;
    DbgEditorLineWidgetProvider *m_pCodeEditorProvider;

    FindDialog *m_pFindDialog;

    WindowTitleMenuBar *m_TitleMenuBar = nullptr;
    SARibbonSystemButtonBar *m_SysButtons = nullptr;
};
#endif // MAINWINDOW_H
