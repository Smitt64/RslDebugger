#ifndef XMLCODEHIGHLIGHTER_H
#define XMLCODEHIGHLIGHTER_H

#include "codehighlighter.h"

class XmlCodeHighlighter : public CodeHighlighter
{
    Q_OBJECT
public:
    XmlCodeHighlighter(QObject *parent = nullptr);
    virtual ~XmlCodeHighlighter();

protected:
    virtual void reset() Q_DECL_OVERRIDE;
    virtual void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    void highlightByRegex(const QTextCharFormat & format,
                          const QRegExp & regex, const QString & text);

    QList<QRegExp> m_xmlKeywordRegexes;
    QRegExp m_xmlElementRegex;
    QRegExp m_xmlAttributeRegex;
    QRegExp m_xmlValueRegex;
    QRegExp m_xmlCommentRegex;
};

#endif // XMLCODEHIGHLIGHTER_H
