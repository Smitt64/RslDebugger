#include "varwatchdlg.h"
#include "ui_varwatchdlg.h"
#include "codeeditor/codeeditor.h"

VarWatchDlg::VarWatchDlg(QWidget *parent)
    : QDialog(parent)
    , m_pEditor(new CodeEditor()),
    ui(new Ui::VarWatchDlg)
{
    ui->setupUi(this);
    setWindowIcon(QIcon::fromTheme("Variable"));
    ui->verticalLayout->insertWidget(2, m_pEditor.data());

    m_pEditor->setReadOnly(true);
    m_pEditor->setWordWrapMode(QTextOption::NoWrap);

    connect(ui->checkBox, &QCheckBox::stateChanged, [=](const int &state)
    {
        if (state == Qt::Checked)
            m_pEditor->setWordWrapMode(QTextOption::WordWrap);
        else
            m_pEditor->setWordWrapMode(QTextOption::NoWrap);
    });
}

VarWatchDlg::~VarWatchDlg()
{
    delete ui;
}

void VarWatchDlg::setValue(const QString &value)
{
    m_pEditor->setPlainText(value);
}

void VarWatchDlg::setVarName(const QString &value)
{
    ui->variable->setText(value);
}

void VarWatchDlg::setVarType(const QString &value)
{
    ui->type->setText(value);
}
