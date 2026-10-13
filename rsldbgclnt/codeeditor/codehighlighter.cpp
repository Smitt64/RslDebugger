// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "codehighlighter.h"
#include "highlighterstyle.h"
#include "cppcodehighlighter.h"
#include "rslcodehighlighter.h"
#include "sqlcodehighlighter.h"
#include "codeeditor.h"
#include "xmlcodehighlighter.h"
#include <QPlainTextEdit>
#include <QDebug>

// https://github.com/Waqar144/QSourceHighlite/blob/master/qsourcehighliter.cpp

#define TABSTOP 4

bool IsInOffsets(const QList<qint32> &offsets, const qint32 &value)
{
    bool result = false;

    foreach(const qint32 &offset, offsets) {
        if (value > offset)
            result = true;
    }

    return result;
}

class CodeHighlighterPrivate
{
    Q_DECLARE_PUBLIC(CodeHighlighter)
public:
    CodeHighlighterPrivate(CodeHighlighter *parent)
    {
        q_ptr = parent;
    }

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;

    QSharedPointer<StyleItem> m_Style;
    QString m_StyleName;

    HighlightingRule singleLineCommentRule;

    CodeHighlighter *q_ptr;
};

CodeHighlighter::CodeHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent),
    d_ptr(new CodeHighlighterPrivate(this))
{
    setStyle(QString());
}

CodeHighlighter::~CodeHighlighter()
{
    delete d_ptr;
}

void CodeHighlighter::setStyle(const QString &style)
{
    Q_D(CodeHighlighter);

    if (style.isEmpty())
        d->m_Style = HighlighterStyle::inst()->style(d->m_StyleName);
    else
    {
        d->m_StyleName = style;
        d->m_Style = HighlighterStyle::inst()->style(style);
    }

    reset();
}

QSharedPointer<StyleItem> CodeHighlighter::style()
{
    Q_D(CodeHighlighter);
    return d->m_Style;
}

void CodeHighlighter::addKeyword(const QString &name, const bool &isNotCaseInsensitive)
{
    Q_D(CodeHighlighter);

    HighlightingRule rule;
    rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(name));
    rule.format = d->m_Style->format(FormatKeyword);
    rule.isNotCaseInsensitive = isNotCaseInsensitive;
    d->highlightingRules.append(rule);
}

void CodeHighlighter::addKeyword2(const QString &name, const bool &isNotCaseInsensitive)
{
    Q_D(CodeHighlighter);

    HighlightingRule rule;
    rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(name));
    rule.format = d->m_Style->format(FormatKeyword2);
    rule.isNotCaseInsensitive = isNotCaseInsensitive;
    d->highlightingRules.append(rule);
}

void CodeHighlighter::addType(const QString &name, const bool &isNotCaseInsensitive)
{
    Q_D(CodeHighlighter);

    HighlightingRule rule;
    rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(name));
    rule.format = d->m_Style->format(FormatElemType);
    rule.isNotCaseInsensitive = isNotCaseInsensitive;
    d->highlightingRules.append(rule);
}

void CodeHighlighter::addEnums(const QString &name)
{
    Q_D(CodeHighlighter);

    HighlightingRule rule;
    rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(name));
    rule.format = d->m_Style->format(FormatEnum);
    d->highlightingRules.append(rule);
}

void CodeHighlighter::addDefine(const QString &name)
{
    Q_D(CodeHighlighter);

    HighlightingRule rule;
    rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(name));
    rule.format = d->m_Style->format(FormatDefine);
    rule.isNotCaseInsensitive = true;
    d->highlightingRules.append(rule);
}

void CodeHighlighter::addHighlightingRule(const HighlightingRule &rule)
{
    Q_D(CodeHighlighter);

    d->highlightingRules.prepend(rule);
}

void CodeHighlighter::addHighlightingRules(const GenHighlightingRuleList &ruleList)
{
    Q_D(CodeHighlighter);

    for (auto &rule : ruleList)
        d->highlightingRules.prepend({rule.pattern, d->m_Style->format(rule.alias)});
}

void CodeHighlighter::setSingleLineCommentStr(const QString &value)
{
    Q_D(CodeHighlighter);
    d->singleLineCommentRule.pattern = QRegularExpression(QString("%1[^\\n]*")
                                                          .arg(value));
    d->singleLineCommentRule.format = d->m_Style->format(FormatComment);
}

void CodeHighlighter::reset()
{
    Q_D(CodeHighlighter);

    HighlightingRule rule;
    d->highlightingRules.clear();

    QSharedPointer<StyleItem> style = d->m_Style;
    d->singleLineCommentFormat = style->format(FormatComment);
    d->multiLineCommentFormat = style->format(FormatComment);

    d->commentStartExpression = QRegExp("/\\*");
    d->commentEndExpression = QRegExp("\\*/");

    rule.pattern = QRegularExpression("([a-zA-Z_][a-zA-Z0-9_]+)\\s*(?=\\()");
    rule.format = style->format(FormatFunction);
    d->highlightingRules.append(rule);

    rule.pattern = QRegularExpression("((?<![\\\\])['\"])((?:.(?!(?<![\\\\])\\1))*.?)\\1"); // \\\".*\\\"
    rule.format = style->format(FormatStrig);
    d->highlightingRules.append(rule);

    if (style->hasFormat(FormatNumber))
    {
        rule.pattern = QRegularExpression("\\b[\\-\\+]?([0-9]+(\\.[0-9]+)?)\\b");
        rule.format = style->format(FormatNumber);
        d->highlightingRules.append(rule);
    }

    setSingleLineCommentStr("//");
}

void CodeHighlighter::highlightBlock(const QString &text)
{
    Q_D(CodeHighlighter);

    QList<qint32> offsets;
    foreach (const HighlightingRule &rule, d->highlightingRules)
    {
        QRegularExpression expression(rule.pattern);

        if (!rule.isNotCaseInsensitive)
            expression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match = expression.match(text);
        while (match.hasMatch())
        {
            int offset = match.capturedLength() + match.capturedStart();

            if (!IsInOffsets(offsets, offset))
            {
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
                match = expression.match(text, offset);
            }
        }
    }

    QRegularExpression singleLineCommentPattern = d->singleLineCommentRule.pattern;

    QRegularExpression singleLineCommentExpression(singleLineCommentPattern);
    singleLineCommentExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch singleLineCommentMatch = singleLineCommentExpression.match(text);

    while (singleLineCommentMatch.hasMatch())
    {
        int offset = singleLineCommentMatch.capturedLength() + singleLineCommentMatch.capturedStart();
        setFormat(singleLineCommentMatch.capturedStart(), singleLineCommentMatch.capturedLength(), d->singleLineCommentRule.format);
        singleLineCommentMatch = singleLineCommentExpression.match(text, offset);
        offsets.append(offset);
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = d->commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = d->commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + d->commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, d->multiLineCommentFormat);
        startIndex = d->commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

// -------------------------------------------------------------------------------

void ToolApplyHighlighter(QPlainTextEdit *pEditor,
                          const int &syntax,
                          const QString &style)
{
    CodeHighlighter *pCodeHighlighter = nullptr;

    if (syntax == HighlighterCpp)
        pCodeHighlighter = new CppCodeHighlighter(pEditor);
    else if (syntax == HighlighterRsl)
        pCodeHighlighter = new RslCodeHighlighter(pEditor);
    else if (syntax == HighlighterSql)
        pCodeHighlighter = new SqlCodeHighlighter(pEditor);
    else if (syntax == HighlighterXml)
        pCodeHighlighter = new XmlCodeHighlighter(pEditor);
    else
        pCodeHighlighter = new CodeHighlighter(pEditor);

    if (pCodeHighlighter)
    {
        pCodeHighlighter->setStyle(style);
        pCodeHighlighter->setDocument(pEditor->document());
    }

    CodeEditor *pCodeEditor = qobject_cast<CodeEditor*>(pEditor);
    QSharedPointer<StyleItem> sstyle;

    if (pCodeHighlighter)
        sstyle = pCodeHighlighter->style();
    else
        sstyle = HighlighterStyle::inst()->style();

    QColor background = sstyle->editorBackground();

    if (pCodeEditor)
    {
        if (pCodeEditor->highlighter())
            delete pCodeEditor->highlighter();

        pCodeEditor->setCurrentLineColor(sstyle->editorCurrentLine());
        pCodeEditor->setCurrentWordColor(sstyle->editorCurrentWord());
    }

    QTextCharFormat def =  sstyle->format(FormatDefault);
    QFont defaultFont = def.font();
    defaultFont.setStyleHint(QFont::Courier);
    pEditor->document()->setDefaultFont(defaultFont);

    if (pCodeEditor)
    {
        QFontMetrics metrics(defaultFont);
        int tabWidth = TABSTOP * metrics.width(' ');
        pCodeEditor->setTabStopWidth(tabWidth);
        pCodeEditor->setCodeHighlighter(pCodeHighlighter);
    }

    pEditor->setStyleSheet(QString("QPlainTextEdit { background-color: rgb(%1, %2, %3); color: rgb(%4, %5, %6) }")
                               .arg(background.red())
                               .arg(background.green())
                               .arg(background.blue())
                               .arg(def.foreground().color().red())
                               .arg(def.foreground().color().green())
                               .arg(def.foreground().color().blue()));

    pEditor->setAutoFillBackground(true);
}
