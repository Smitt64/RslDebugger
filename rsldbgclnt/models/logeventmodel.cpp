#include "logeventmodel.h"
#include "../logevent.h"

LogEventModel::LogEventModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

LogEventModel::~LogEventModel()
{

}

int LogEventModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_Items.size();
}

int LogEventModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant LogEventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QVector<QString> Headers = {
        tr("Time"),
        tr("Type"),
        tr("Context"),
        tr("Message")
    };

    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
            return Headers[section];
    }

    return QVariant();
}

QVariant LogEventModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == ColumnTime)
            return m_Items[index.row()].time.toString("yyyy-MM-dd hh:mm:ss.zzz");
        else if (index.column() == ColumnType)
        {
            QString out;
            switch (m_Items[index.row()].type)
            {
            case QtInfoMsg:     out = "INF"; break;
            case QtDebugMsg:    out = "DBG"; break;
            case QtWarningMsg:  out = "WRN"; break;
            case QtCriticalMsg: out = "CRT"; break;
            case QtFatalMsg:    out = "FTL"; break;
            }

            return out;
        }
        else if (index.column() == ColumnContext)
            return m_Items[index.row()].context;
        else if (index.column() == ColumnMessage)
            return m_Items[index.row()].msg;
    }

    return QVariant();
}

void LogEventModel::append(LogEvent *log)
{
    beginInsertRows(QModelIndex(), m_Items.size(), m_Items.size());

    LogModelItem item;
    item.msg = log->msg();
    item.context = log->context();
    item.time = log->dateTime();
    item.type = log->type();

    m_Items.append(item);
    endInsertRows();
}
