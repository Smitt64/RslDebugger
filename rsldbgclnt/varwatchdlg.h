#ifndef VARWATCHDLG_H
#define VARWATCHDLG_H

#include <QDialog>

namespace Ui {
class VarWatchDlg;
}

class CodeEditor;
class VarWatchDlg : public QDialog
{
    Q_OBJECT

public:
    explicit VarWatchDlg(QWidget *parent = nullptr);
    ~VarWatchDlg();

    void setValue(const QString &value);
    void setVarName(const QString &value);
    void setVarType(const QString &value);

private:
    QScopedPointer<CodeEditor> m_pEditor;
    Ui::VarWatchDlg *ui;
};

#endif // VARWATCHDLG_H
