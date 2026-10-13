#ifndef CALLSTACKMODEL_H
#define CALLSTACKMODEL_H

#include <QAbstractTableModel>
#include <QObject>

typedef struct
{
    QString func, file, fullfilename, fnamespace;
    int offs;
    int len;
    int line;
}StackItem;

class CallStackModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum
    {
        ColumnLevel = 0,
        ColumnFunction,
        ColumnFile,
        ColumnLine,
        ColumnCount,
    };
    CallStackModel(QObject *parent = nullptr);
    virtual ~CallStackModel();

    void clear();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void append(Qt::HANDLE stack);

    QString file(const QModelIndex &index) const;
    QString filename(const QModelIndex &index) const;
    QString func(const QModelIndex &index) const;

private:
    QVector<StackItem> m_Items;
};

#endif // CALLSTACKMODEL_H
