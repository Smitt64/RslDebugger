#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CodeEditor;
class QPlainTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(const QString &startFile, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void open();
    bool save();
    bool saveAs();
    void run();
    void runWithoutDebug();
    void documentWasModified();
    void appendRunLog(const QString &text);

private:
    void createActions();
    void createMenus();
    bool maybeSave();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void runMacro(bool debug);

    CodeEditor *m_pEditor;
    QPlainTextEdit *m_pOutput;
    bool m_running;
    QString m_CurrentFile;
};

#endif // MAINWINDOW_H
