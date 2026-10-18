#ifndef VARWATCHMODEL_H
#define VARWATCHMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QStack>
#include <QString>

// Данные одной переменной (раньше - проводная структура DBG_VARIABLEDATA
// из протокола; теперь протокол JSON, а это просто внутренний POD модели).
typedef struct
{
    QString name, type, proc;
    int index = 0;
    int depth = 1;
    bool isObject = false;
    bool isExpanded = false;
    bool isLvalue = false;
    bool isFakeChildren = false;

    qint64 val = 0;        // RSLVALUE
    qint64 exp = 0;        // RSLEXPCTX
    qint64 st = 0;         // RSLSTACK
    qint64 procHandle = 0; // RSLPROC
    qint64 info = 0;       // RSLVINFO
    qint64 mod = 0;        // RSLMODULE
} VarWatchData;

class TreeItem;
class VarWatchModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class TreeItem;
public:
    enum Column
    {
        ColumnName = 0,
        ColumnType,
        ColumnValue,

        ColumnCount
    };

    enum CustomRoles
    {
        IndexRole = Qt::UserRole + 1,
        StackRole,
        ValueRole,
        ValueInfoRole,
        RealHasChild,
        ShowWatchButtonRole
    };

    VarWatchModel(QObject *parent = nullptr);
    virtual ~VarWatchModel();

    void clear();
    void append(const VarWatchData &var, const QString &val);

    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = {}) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void startResetVarables();
    void finishResetVarables();

    QModelIndexList getExpanded();

signals:
    void expand(const QModelIndex &index);

private:
    void WalkExpandedItems(TreeItem *item, QModelIndexList &lst);
    std::unique_ptr<TreeItem> rootItem;

    QStack<TreeItem*> m_LastItem;
};

#endif // VARWATCHMODEL_H
