#include "sqlcodehighlighter.h"
#include "highlighterstyle.h"
#include <QFile>
#include <QTextStream>

SqlCodeHighlighter::SqlCodeHighlighter(QObject *parent)
    : CodeHighlighter{parent}
{

}

SqlCodeHighlighter::~SqlCodeHighlighter()
{

}

void SqlCodeHighlighter::reset()
{
    CodeHighlighter::reset();

    HighlightingRule rule;
    QStringList keywordPatterns, keywordPatterns2;

    QSharedPointer<StyleItem> style = CodeHighlighter::style();
    rule.pattern = QRegularExpression("([a-zA-Z_][a-zA-Z0-9_]+)\\s*(?=\\()");
    rule.format = style->format(FormatFunction);
    addHighlightingRule(rule);

    keywordPatterns << "\\bVARCHAR2\\b"
                    << "\\bNVARCHAR2\\b"
                    << "\\bVARCHAR\\b"
                    << "\\bNUMBER\\b"
                    << "\\bLONG\\b"
                    << "\\bDATE\\b"
                    << "\\bCHAR\\b";

    QFile f(":/highlighter/sql_keys.txt");
    f.open(QIODevice::ReadOnly);
    QTextStream stream(&f);

    QString line;
    do
    {
        line = stream.readLine();
        QStringList l = line.remove(" ").split(",");

        foreach (const QString &pattern, l)
        {
            if (!pattern.isEmpty())
                keywordPatterns.append(QString("\\b%1\\b").arg(pattern));
        }
    } while (!line.isNull());

    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = style->format(FormatKeyword);
        rule.isNotCaseInsensitive = false;

        addHighlightingRule(rule);
    }

    rule.pattern = QRegularExpression("((?<![\\\\])['\"])((?:.(?!(?<![\\\\])\\1))*.?)\\1"); // \\\".*\\\"
    rule.format = style->format(FormatStrig);
    addHighlightingRule(rule);

    if (style->hasFormat(FormatNumber))
    {
        rule.pattern = QRegularExpression("\\b[\\-\\+]?([0-9]+(\\.[0-9]+)?)\\b");
        rule.format = style->format(FormatNumber);
        addHighlightingRule(rule);
    }

    setSingleLineCommentStr("--");
}
