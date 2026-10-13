#include "finddialog.h"
#include <QtWidgets>

// https://github.com/Yet-Zio/QFindDialogs/blob/master/src/finddialog.cpp

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent)
{
    label = new QLabel(tr("Find &what: "));
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);

    caseCheckBox = new QCheckBox(tr("Match &case"));
    caseCheckBox->setChecked(true);
    fromStartCheckBox = new QCheckBox(tr("Search from &start"));
    fromStartCheckBox->setChecked(true);
    regexCheckBox = new QCheckBox(tr("Regex"));
    regexCheckBox->setChecked(false);

    findButton = new QPushButton(tr("&Find"));
    connect(findButton, SIGNAL(clicked()), this, SLOT(find()));
    findButton->setDefault(true);

    moreButton = new QPushButton(tr("&More"));
    moreButton->setCheckable(true);
    moreButton->setAutoDefault(false);

    extension = new QWidget;
    wholeWordsCheckBox = new QCheckBox(tr("&Whole words"));
    backwardCheckBox = new QCheckBox(tr("Search &backward"));
    searchSelectionCheckBox = new QCheckBox(tr("Search se&lection"));

    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(findButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(moreButton, QDialogButtonBox::ActionRole);

    connect(regexCheckBox, SIGNAL(toggled(bool)), this, SLOT(regexMode()));
    connect(moreButton, &QAbstractButton::toggled, extension, &QWidget::setVisible);

    QVBoxLayout *extensionLayout = new QVBoxLayout;
    extensionLayout->setContentsMargins(QMargins());
    extensionLayout->addWidget(wholeWordsCheckBox);
    extensionLayout->addWidget(backwardCheckBox);
    extensionLayout->addWidget(searchSelectionCheckBox);
    extension->setLayout(extensionLayout);

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label);
    topLeftLayout->addWidget(lineEdit);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addWidget(caseCheckBox);
    leftLayout->addWidget(regexCheckBox);
    leftLayout->addWidget(fromStartCheckBox);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout(leftLayout, 0, 0);
    mainLayout->addWidget(buttonBox, 0, 1);
    mainLayout->addWidget(extension, 1, 0, 1, 2);
    mainLayout->setRowStretch(2, 1);

    setLayout(mainLayout);
    setWindowTitle(tr("Find"));
    extension->hide();
}

FindDialog::~FindDialog()
{

}

void FindDialog::setEditor(QPlainTextEdit *editor)
{
    myEditor = editor;
}

QPlainTextEdit *FindDialog::getEditor()
{
    return myEditor;
}

void FindDialog::find()
{
    QString query = lineEdit->text();
    QRegularExpression re;
    QPlainTextEdit *Editor = getEditor();

    if(fromStartCheckBox->isChecked())
    {
        cursor = Editor->textCursor();
        cursor.movePosition(QTextCursor::Start);
        Editor->setTextCursor(cursor);
    }

    fromStartCheckBox->setChecked(false);

    if(regexCheckBox->isChecked())
    {
        re = QRegularExpression(query);
        if(backwardCheckBox->isChecked())
            lastMatch = Editor->find(re, QTextDocument::FindBackward);
        else
            lastMatch = Editor->find(re);
    }

    if(searchSelectionCheckBox->isChecked())
    {
        query = Editor->textCursor().selectedText();
        Editor->find(query);
    }
    else
    {
        if(backwardCheckBox->isChecked())
        {
            if(wholeWordsCheckBox->isChecked() && caseCheckBox->isChecked())
                lastMatch = Editor->find(query, QTextDocument::FindWholeWords | QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);
            else if(wholeWordsCheckBox->isChecked() && caseCheckBox->isChecked() != true)
                lastMatch = Editor->find(query, QTextDocument::FindWholeWords | QTextDocument::FindBackward);
            else if(caseCheckBox->isChecked())
                lastMatch = Editor->find(query, QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);
            else
                lastMatch = Editor->find(query, QTextDocument::FindBackward);
        }
        else
        {
            if(wholeWordsCheckBox->isChecked() && caseCheckBox->isChecked())
                lastMatch = Editor->find(query, QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively);
            else if(wholeWordsCheckBox->isChecked() && caseCheckBox->isChecked() != true)
                lastMatch = Editor->find(query, QTextDocument::FindWholeWords);
            else if(caseCheckBox->isChecked())
                lastMatch = Editor->find(query, QTextDocument::FindCaseSensitively);
            else
                lastMatch = Editor->find(query);
        }
    }
}

void FindDialog::regexMode()
{
    caseCheckBox->setChecked(false);
    wholeWordsCheckBox->setChecked(false);

    caseCheckBox->setEnabled(false);
    wholeWordsCheckBox->setEnabled(false);
}
