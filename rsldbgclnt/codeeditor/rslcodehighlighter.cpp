#include "rslcodehighlighter.h"

RslCodeHighlighter::RslCodeHighlighter(QObject *parent) :
    CodeHighlighter(parent)
{

}

RslCodeHighlighter::~RslCodeHighlighter()
{

}

void RslCodeHighlighter::reset()
{
    CodeHighlighter::reset();

    QFile rsl_keywords(":/highlighter/rsl_keywords.txt");
    rsl_keywords.open(QIODevice::ReadOnly);

    while(!rsl_keywords.atEnd())
    {
        QString pattern = rsl_keywords.readLine().trimmed();
        addKeyword(pattern);
    }
    rsl_keywords.close();

    addKeyword2("debugbreak");
}
