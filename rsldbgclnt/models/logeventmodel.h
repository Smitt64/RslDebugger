#ifndef LOGEVENTMODEL_H
#define LOGEVENTMODEL_H

#include <QDateTime>
#include <QAbstractTableModel>

typedef struct
{
    QString msg, context;
    QDateTime time;
    QtMsgType type;
}LogModelItem;

class LogEvent;
class LogEventModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum
    {
        //ColumnIcon = 0,
        ColumnTime,
        ColumnType,
        ColumnContext,
        ColumnMessage,

        ColumnCount
    };
    LogEventModel(QObject *parent = nullptr);
    virtual ~LogEventModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void append(LogEvent *log);
private:
    QVector<LogModelItem> m_Items;
};

#endif // LOGEVENTMODEL_H
