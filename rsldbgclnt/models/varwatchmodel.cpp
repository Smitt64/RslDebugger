#include "varwatchmodel.h"
#include "mainwindow.h"
#include <QDebug>
#include <QStyle>
#include <QApplication>

class TreeItem
{
public:
    explicit TreeItem(VarWatchModel *model, TreeItem *parentItem = nullptr) :
        pModel(model),
        m_parentItem(parentItem)
    {
        m_VarInfo.depth = 0;
    }

    void appendChild(std::unique_ptr<TreeItem> &&child)
    {
        m_childItems.push_back(std::move(child));
    }

    TreeItem *child(int row)
    {
        return row >= 0 && row < childCount() ? m_childItems.at(row).get() : nullptr;
    }

    TreeItem *lastChild()
    {
        return !m_childItems.empty() ? m_childItems.back().get() : nullptr;
    }

    int childCount() const
    {
        return int(m_childItems.size());
    }

    QVariant data(int column, int role) const
    {
        if (role == Qt::DisplayRole)
        {
            if (column == VarWatchModel::ColumnName)
                return m_VarInfo.name;
            else if (column == VarWatchModel::ColumnType)
                return m_VarInfo.type;
            else if (column == VarWatchModel::ColumnValue)
                return VarValueCache;
        }
        else if (role == Qt::DecorationRole)
        {
            if (column == VarWatchModel::ColumnName)
            {
                if (m_VarInfo.isObject)
                {
                    if (!m_VarInfo.isFakeChildren)
                    {
                        if (m_VarInfo.type == QLatin1String("TArray"))
                            return QIcon::fromTheme("Array");

                        return QIcon::fromTheme("Class");
                    }
                    else
                        return QIcon::fromTheme("Childs");
                }
                else
                {
                    if (m_VarInfo.type == QLatin1String("UNDEFINED"))
                        return QIcon::fromTheme("Undefined");

                    return QIcon::fromTheme("Variable");
                }
            }
        }
        else if (role == VarWatchModel::IndexRole)
            return m_VarInfo.index;
        else if (role == VarWatchModel::StackRole)
            return m_VarInfo.st;
        else if (role == VarWatchModel::RealHasChild)
            return !m_childItems.empty();
        else if (role == VarWatchModel::ShowWatchButtonRole)
            return !m_VarInfo.type.compare("STRING", Qt::CaseInsensitive) && !m_VarInfo.isFakeChildren;
        else if (role == VarWatchModel::ValueRole)
            return QVariant::fromValue<qint64>(m_VarInfo.val);
        else if (role == VarWatchModel::ValueInfoRole)
            return QVariant::fromValue<qint64>(m_VarInfo.info);

        return QVariant();
    }

    int row() const
    {
        if (m_parentItem == nullptr)
            return 0;
        const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(),
                                     [this](const std::unique_ptr<TreeItem> &treeItem) {
                                         return treeItem.get() == this;
                                     });

        if (it != m_parentItem->m_childItems.cend())
            return std::distance(m_parentItem->m_childItems.cbegin(), it);

        Q_ASSERT(false); // should not happen
        return -1;
    }

    TreeItem *parentItem()
    {
        return m_parentItem;
    }

    VarWatchData &var()
    {
        return m_VarInfo;
    }

    const VarWatchData &var() const
    {
        return m_VarInfo;
    }

    void setValue(const QString &val)
    {
        VarValueCache = val;
    }

private:
    VarWatchModel *pModel;
    std::vector<std::unique_ptr<TreeItem>> m_childItems;
    //QVariantList m_itemData;
    TreeItem *m_parentItem;

    QVariant VarValueCache;

    VarWatchData m_VarInfo;
};

//-------------------------------------------------------------

VarWatchModel::VarWatchModel(QObject *parent)
    : QAbstractItemModel{parent},
    rootItem(std::make_unique<TreeItem>(this))
{
}

VarWatchModel::~VarWatchModel()
{

}

void VarWatchModel::clear()
{
    //beginResetModel();
    rootItem.reset(new TreeItem(this));
    //m_LastItem.push(rootItem.get());
    //endResetModel();
}

void VarWatchModel::WalkExpandedItems(TreeItem *item, QModelIndexList &lst)
{
    for (int i = 0; i < item->childCount(); i++)
    {
        if (item->child(i)->var().isExpanded)
        {
            TreeItem *parentItem = item->child(i);//item->parentItem();
            QModelIndex index = createIndex(parentItem->row(), 0, parentItem);
            WalkExpandedItems(parentItem, lst);
            lst.append(index);
        }
    }
}

QModelIndexList VarWatchModel::getExpanded()
{
    QModelIndexList list;
    WalkExpandedItems(rootItem.get(), list);
    return list;
}

void VarWatchModel::startResetVarables()
{
    beginResetModel();
    clear();

    m_LastItem.clear();
    m_LastItem.push(rootItem.get());
}

void VarWatchModel::finishResetVarables()
{
    endResetModel();

    QModelIndexList list = getExpanded();
    for (int i = 0; i < list.size(); i++)
        emit expand(list[i]);
}

void VarWatchModel::append(const VarWatchData &var, const QString &val)
{
    const VarWatchData &lastvar = m_LastItem.top()->var();

    if (var.depth > lastvar.depth)
    {
        TreeItem *topItem = m_LastItem.top();
        //TreeItem *parentItem = topItem->parentItem() ? topItem->parentItem() : rootItem.get();
        TreeItem *parentItem = topItem ? topItem : rootItem.get();
        parentItem->appendChild(std::make_unique<TreeItem>(this, parentItem));

        TreeItem *item = parentItem->lastChild();
        item->var() = var;
        item->setValue(val);

        m_LastItem.push(item);
    }
    else if (var.depth == lastvar.depth)
    {
        TreeItem *topItem = m_LastItem.top();
        TreeItem *parentItem = topItem->parentItem() ? topItem->parentItem() : rootItem.get();
        parentItem->appendChild(std::make_unique<TreeItem>(this, parentItem));

        TreeItem *item = parentItem->lastChild();
        item->var() = var;
        item->setValue(val);

        m_LastItem.pop();
        m_LastItem.push(item);
    }
    else
    {
        m_LastItem.pop();
        append(var, val);
    }
}

int VarWatchModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

QVariant VarWatchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    const auto *item = static_cast<const TreeItem*>(index.internalPointer());
    return item->data(index.column(), role);
}

QModelIndex VarWatchModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    TreeItem *parentItem = parent.isValid() ? static_cast<TreeItem*>(parent.internalPointer()) : rootItem.get();
    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);

    return {};
}

QModelIndex VarWatchModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    auto *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    return parentItem != rootItem.get() ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}

int VarWatchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    const TreeItem *parentItem = parent.isValid()
                                     ? static_cast<const TreeItem*>(parent.internalPointer())
                                     : rootItem.get();

    return parentItem->childCount();
}

Qt::ItemFlags VarWatchModel::flags(const QModelIndex &index) const
{
    return index.isValid() ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

bool VarWatchModel::hasChildren(const QModelIndex &parent) const
{
    // QTreeViewPrivate::layout() опрашивает строки только для индексов,
    // у которых hasChildren() == true (см. qtreeview.cpp:3319). Для КОРНЯ
    // раньше это случайно работало: неинициализированный DBG_VARIABLEDATA
    // в debug-сборке заполнен 0xCD, и is_object корня был "true". После
    // перехода на VarWatchData с инициализаторами корень стал честным
    // false -> дерево перестало показывать строки. Отвечаем по факту.
    if (!parent.isValid())
        return rootItem->childCount() > 0;

    TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
    return parentItem->var().isObject;
}

QVariant VarWatchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QStringList Headers =
    {
        tr("Variable"),
        tr("Type"),
        tr("Value")
    };

    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
            return Headers[section];
    }

    return QVariant();
}
