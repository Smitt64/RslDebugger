#ifndef DBGEDITORLINEWIDGETPROVIDER_H
#define DBGEDITORLINEWIDGETPROVIDER_H

#include <codeeditor/codeeditor.h>
#include <QMap>

class DbgEditorLineWidgetProvider : public CodeEditorLineWidgetProvider
{
public:
    enum
    {
        IconBreakpoint = 1 << 1,
        IconBreakpointDisable = 1 << 2,
        IconCurrentLine = 1 << 3,
        IconEnterFunction = 1 << 4,
    };
    DbgEditorLineWidgetProvider();
    virtual ~DbgEditorLineWidgetProvider();

    void addItemId(const int &id, const int &line) Q_DECL_OVERRIDE;
    void removeItemId(const int &id, const int &line) Q_DECL_OVERRIDE;

    int width() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const int &line, const QRect &rect) Q_DECL_OVERRIDE;
    void lineClick(const int &line, const Qt::MouseButton &button) Q_DECL_OVERRIDE;

    void clearEnterFunction();
    void addEnterFunction(const int &line);

private:
    QMap<int, quint32> m_IconInfo;
};

#endif // DBGEDITORLINEWIDGETPROVIDER_H
