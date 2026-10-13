#ifndef SQLCODEHIGHLIGHTER_H
#define SQLCODEHIGHLIGHTER_H

#include "codehighlighter.h"

class SqlCodeHighlighter : public CodeHighlighter
{
    Q_OBJECT
public:
    explicit SqlCodeHighlighter(QObject *parent = nullptr);
    virtual ~SqlCodeHighlighter();

protected:
    virtual void reset() Q_DECL_OVERRIDE;
};

#endif // SQLCODEHIGHLIGHTER_H
