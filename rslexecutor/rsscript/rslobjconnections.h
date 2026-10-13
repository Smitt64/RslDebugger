#ifndef RSLOBJCONNECTIONS_H
#define RSLOBJCONNECTIONS_H

#include <QObject>

typedef struct tagR2M
{
    Qt::HANDLE obj;
    int id;

    tagR2M()
    {
        id = 0;
        obj = nullptr;
    }
}R2M;

class QSignalSpy;
class RslObjConnections : public QObject
{
    Q_OBJECT
public:
    RslObjConnections(QObject *parent = nullptr);
    virtual ~RslObjConnections();

    void setMacroFunc(const QString &macro);
    void setMacroFunc(const R2M &R2m);

    void initSignalSpy(const QObject *obj, const char *signal);
    int execIndex() const;

public slots:
    void ExecuteMacroFunc();

private:
    QString m_MacroName;
    R2M m_R2m;
    QScopedPointer<QSignalSpy> m_pSpy;
};

Q_DECLARE_METATYPE(R2M)

#endif // RSLOBJCONNECTIONS_H
