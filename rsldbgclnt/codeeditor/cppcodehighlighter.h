#ifndef CPPCODEHIGHLIGHTER_H
#define CPPCODEHIGHLIGHTER_H

#include <QObject>
#include "codehighlighter.h"

class CppCodeHighlighter : public CodeHighlighter
{
    Q_OBJECT
public:
    CppCodeHighlighter(QObject *parent = Q_NULLPTR);
    virtual ~CppCodeHighlighter();

protected:
    virtual void reset() Q_DECL_OVERRIDE;
};

#endif // CPPCODEHIGHLIGHTER_H
