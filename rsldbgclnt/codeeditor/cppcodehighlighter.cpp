#include "cppcodehighlighter.h"
#include "highlighterstyle.h"
#include <QFile>

CppCodeHighlighter::CppCodeHighlighter(QObject *parent) :
    CodeHighlighter(parent)
{

}

CppCodeHighlighter::~CppCodeHighlighter()
{

}

void CppCodeHighlighter::reset()
{
    CodeHighlighter::reset();

    QStringList keywordPatterns, keywordPatterns2;
    HighlightingRule rule;

    keywordPatterns2
        << "\\bbreak\\b" << "\\bcase\\b" << "\\bcatch\\b" << "\\bcontinue\\b"
        << "\\bdefault\\b"<< "\\bdo\\b"<< "\\belse\\b" << "\\bfor\\b" << "\\bgoto\\b"
        << "\\bif\\b" << "\\breturn\\b" << "\\btry\\b" << "\\bwhile\\b";

    keywordPatterns << "\\balignas\\b" << "\\balignof\\b" << "\\b__asm\\b" << "\\bauto\\b"
                    << "\\bbool\\b"<< "\\bchar\\b" << "\\bchar16_t\\b" << "\\bchar32_t\\b"
                    << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bconstexpr\\b" << "\\bconst_cast\\b"  << "\\bdecltype\\b"
                    << "\\bdelete\\b"  << "\\bdouble\\b" << "\\bdynamic_cast\\b"  << "\\benum\\b"
                    << "\\bexplicit\\b" << "\\bexport\\b" << "\\bextern\\b" << "\\bfalse\\b" << "\\bfloat\\b"
                    << "\\bfriend\\b"  << "\\binline\\b" << "\\bint\\b" << "\\blong\\b"
                    << "\\bmutable\\b" << "\\bnamespace\\b" << "\\bnew\\b" << "\\bnoexcept\\b" << "\\bnot\\b" << "\\bnot_eq\\b"
                    << "\\bnullptr\\b" << "\\boperator\\b" << "\\bprivate\\b" << "\\bprotected\\b"
                    << "\\bpublic\\b" << "\\bregister\\b" << "\\breinterpret_cast\\b"  << "\\bshort\\b"
                    << "\\bsigned\\b" << "\\bsizeof\\b" << "\\bstatic\\b" << "\\bstatic_assert\\b" << "\\bstatic_cast\\b"
                    << "\\bstruct\\b" << "\\bswitch\\b" << "\\btemplate\\b" << "\\bthis\\b" << "\\bthread_local\\b" << "\\bthrow\\b"
                    << "\\btrue\\b"  << "\\btypedef\\b" << "\\btypeid\\b" << "\\btypename\\b" << "\\bunion\\b"
                    << "\\bunsigned\\b" << "\\busing\\b" << "\\bvirtual\\b" << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bwchar_t\\b";

    QFile cpp_rstypes(":/highlighter/cpp_rstypes.txt");
    cpp_rstypes.open(QIODevice::ReadOnly);

    while(!cpp_rstypes.atEnd())
    {
        QString pattern = cpp_rstypes.readLine().trimmed();
        addType(pattern);
    }
    cpp_rstypes.close();

    QFile cpp_rsenums(":/highlighter/cpp_rsenums.txt");
    cpp_rsenums.open(QIODevice::ReadOnly);

    while(!cpp_rsenums.atEnd())
    {
        QString pattern = cpp_rsenums.readLine().trimmed();

        addEnums(pattern);
    }
    cpp_rsenums.close();

    QSharedPointer<StyleItem> style = CodeHighlighter::style();
    rule.pattern = QRegularExpression("([a-zA-Z_][a-zA-Z0-9_]+)\\s*(?=\\()");
    rule.format = style->format(FormatFunction);
    addHighlightingRule(rule);

    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = style->format(FormatKeyword);
        rule.isNotCaseInsensitive = true;

        addHighlightingRule(rule);
    }

    foreach (const QString &pattern, keywordPatterns2)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = style->format(FormatKeyword2);
        rule.isNotCaseInsensitive = true;
        addHighlightingRule(rule);
    }

    QFile cpp_rsdefines(":/highlighter/cpp_rsdefines.txt");
    cpp_rsdefines.open(QIODevice::ReadOnly);

    while(!cpp_rsdefines.atEnd())
    {
        QString pattern = cpp_rsdefines.readLine().trimmed();

        addDefine(pattern);
    }

    cpp_rsdefines.close();

    rule.pattern = QRegularExpression("((?<![\\\\])['\"])((?:.(?!(?<![\\\\])\\1))*.?)\\1"); // \\\".*\\\"
    rule.format = style->format(FormatStrig);
    addHighlightingRule(rule);

    if (style->hasFormat(FormatNumber))
    {
        rule.pattern = QRegularExpression("\\b[\\-\\+]?([0-9]+(\\.[0-9]+)?)\\b");
        rule.format = style->format(FormatNumber);
        addHighlightingRule(rule);
    }
}
