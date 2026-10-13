#ifndef RSLCODEHIGHLIGHTER_H
#define RSLCODEHIGHLIGHTER_H

#include "codehighlighter.h"

class RslCodeHighlighter : public CodeHighlighter
{
    Q_OBJECT
public:
    RslCodeHighlighter(QObject *parent = Q_NULLPTR);
    virtual ~RslCodeHighlighter();

protected:
    virtual void reset() Q_DECL_OVERRIDE;
};

#endif // RSLCODEHIGHLIGHTER_H
