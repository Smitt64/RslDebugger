#include "cdebug.h"
#include "callstackmodel.h"
#include "rsl/dbgintf.h"
#include "rsldbg.h"
#include <QFileInfo>
#include <QIcon>

CallStackModel::CallStackModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

CallStackModel::~CallStackModel()
{

}

void CallStackModel::clear()
{
    beginResetModel();
    m_Items.clear();
    endResetModel();
}

int CallStackModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_Items.size();
}

int CallStackModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant CallStackModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == ColumnItem)
        {
            return tr("%1!%2. Line: %3")
                .arg(m_Items[index.row()].file, m_Items[index.row()].func)
                .arg(m_Items[index.row()].line);
        }
    }
    else if (role == Qt::DecorationRole && index.row() == 0)
        return QIcon::fromTheme("CurrentLine");

    return QVariant();
}

void CallStackModel::append(Qt::HANDLE stack)
{
    beginInsertRows(QModelIndex(), m_Items.size(), m_Items.size());

    int isBtrStream = 0;
    Qt::HANDLE module = RslGetModuleFromStack(stack);
    Qt::HANDLE hst = RslGetStatementFromStack(stack);

    StackItem item;
    item.fullfilename = RslGetModuleFile(module, &isBtrStream);
    item.func = RslGetProcNameFromStack(stack);

    RslGetStatementPos(hst, &item.offs, &item.len);
    //item.line = RslGetModuleLine(module, item.offs, item.len);

    QFileInfo fi(item.fullfilename);
    item.file = fi.fileName();

    m_Items.append(item);
    endInsertRows();
}

QVariant CallStackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QVector<QString> Headers = {
        //"",
        tr("Name")
    };

    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
            return Headers[section];
    }

    return QVariant();
}
