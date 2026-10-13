// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef CODEHIGHLIGHTER_H
#define CODEHIGHLIGHTER_H

#include "ToolsRuntime_global.h"
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

typedef struct
{
    QRegularExpression pattern;
    QString alias;
}GenHighlightingRule;
typedef QList<GenHighlightingRule> GenHighlightingRuleList;

typedef struct HighlightingRule
{
    QRegularExpression pattern;
    QTextCharFormat format;
    bool isNotCaseInsensitive;
}HighlightingRule;
typedef QList<HighlightingRule> HighlightingRuleList;

class StyleItem;
class QPlainTextEdit;
class CodeHighlighterPrivate;
class TOOLSRUNTIME_EXPORT CodeHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    friend TOOLSRUNTIME_EXPORT void ToolApplyHighlighter(QPlainTextEdit *pEditor, const int &syntax, const QString &style);
public:
    CodeHighlighter(QObject *parent = Q_NULLPTR);
    virtual ~CodeHighlighter();

    void setStyle(const QString &style);
    QSharedPointer<StyleItem> style();

    void addKeyword(const QString &name, const bool &isNotCaseInsensitive = true);
    void addKeyword2(const QString &name, const bool &isNotCaseInsensitive = true);
    void addType(const QString &name, const bool &isNotCaseInsensitive = true);
    void addEnums(const QString &name);
    void addDefine(const QString &name);
    void addHighlightingRule(const HighlightingRule &name);
    void addHighlightingRules(const GenHighlightingRuleList &ruleList);

    void setSingleLineCommentStr(const QString &value);

protected:
    virtual void reset();
    virtual void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    CodeHighlighterPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CodeHighlighter);
};

enum HighlighterSyntax
{
    HighlighterPlain = 0,
    HighlighterCpp,
    HighlighterRsl,
    HighlighterSql,
    HighlighterXml,
};

TOOLSRUNTIME_EXPORT void ToolApplyHighlighter(QPlainTextEdit *pEditor,
                                              const int &syntax,
                                              const QString &style = QString());

#endif // CODEHIGHLIGHTER_H
