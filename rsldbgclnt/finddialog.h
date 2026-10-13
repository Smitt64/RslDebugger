#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QRegularExpression>

class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class FindDialog : public QDialog
{
    Q_OBJECT
public:
    FindDialog(QWidget *parent = nullptr);
    virtual ~FindDialog();

    void setEditor(QPlainTextEdit *editor);
    QPlainTextEdit *getEditor();

public slots:
    void find();
    void regexMode();

private:
    bool lastMatch = false;
    QPlainTextEdit *myEditor;

    QLabel *label;
    QLineEdit *lineEdit;
    QCheckBox *regexCheckBox;
    QCheckBox *caseCheckBox;
    QCheckBox *fromStartCheckBox;
    QCheckBox *wholeWordsCheckBox;
    QCheckBox *searchSelectionCheckBox;
    QCheckBox *backwardCheckBox;
    QDialogButtonBox *buttonBox;
    QPushButton *findButton;
    QPushButton *moreButton;
    QWidget *extension;

    QTextCursor cursor;
};

#endif // FINDDIALOG_H
