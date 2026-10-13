#include "xmlcodehighlighter.h"
#include "highlighterstyle.h"

XmlCodeHighlighter::XmlCodeHighlighter(QObject *parent)
    : CodeHighlighter{parent}
{
    m_xmlElementRegex.setPattern("<[?\\s]*[/]?[\\s]*([^\\n][^>]*)(?=[\\s/>])");
    m_xmlAttributeRegex.setPattern("\\w+(?=\\=)");
    m_xmlValueRegex.setPattern("\"[^\\n\"]+\"(?=[?\\s/>])");
    m_xmlCommentRegex.setPattern("<!--[^\\n]*-->");

    m_xmlKeywordRegexes = QList<QRegExp>() << QRegExp("<\\?") << QRegExp("/>")
                                           << QRegExp(">") << QRegExp("<") << QRegExp("</")
                                           << QRegExp("\\?>");
}

XmlCodeHighlighter::~XmlCodeHighlighter()
{

}

void XmlCodeHighlighter::reset()
{
    CodeHighlighter::reset();

    /*HighlightingRule rule;
    rule.pattern = QRegularExpression("<[?\\s]*[/]?[\\s]*([^\\n][^>]*)(?=[\\s/>])");
    rule.format = style()->format(FormatKeyword);
    rule.isNotCaseInsensitive = false;

    addHighlightingRule(rule);

    rule.pattern = QRegularExpression("<[?\\s]*[/]?[\\s]*([^\\n][^>]*)(?=[\\s/>])");
    rule.format = style()->format(FormatKeyword2);
    addHighlightingRule(rule);*/
}

void XmlCodeHighlighter::highlightBlock(const QString &text)
{
    int xmlElementIndex = m_xmlElementRegex.indexIn(text);
    while(xmlElementIndex >= 0)
    {
        int matchedPos = m_xmlElementRegex.pos(1);
        int matchedLength = m_xmlElementRegex.cap(1).length();
        setFormat(matchedPos, matchedLength, style()->format(FormatKeyword));

        xmlElementIndex = m_xmlElementRegex.indexIn(text, matchedPos + matchedLength);
    }

    typedef QList<QRegExp>::const_iterator Iter;
    Iter xmlKeywordRegexesEnd = m_xmlKeywordRegexes.end();
    for(Iter it = m_xmlKeywordRegexes.begin(); it != xmlKeywordRegexesEnd; ++it)
    {
        const QRegExp & regex = *it;
        highlightByRegex(style()->format(FormatKeyword), regex, text);
    }

    highlightByRegex(style()->format(FormatKeyword2), m_xmlAttributeRegex, text);
    highlightByRegex(style()->format(FormatComment), m_xmlCommentRegex, text);
    highlightByRegex(style()->format(FormatStrig), m_xmlValueRegex, text);
}

void XmlCodeHighlighter::highlightByRegex(const QTextCharFormat &format,
                                          const QRegExp &regex, const QString &text)
{
    int index = regex.indexIn(text);

    while(index >= 0)
    {
        int matchedLength = regex.matchedLength();
        setFormat(index, matchedLength, format);

        index = regex.indexIn(text, index + matchedLength);
    }
}
