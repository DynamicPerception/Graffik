#include "listModel.h"
#include <algorithm>

listModel::listModel(listItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
    m_roles = m_prototype->roleNames();
}

int listModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant listModel::data(const QModelIndex &index, int role) const {
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

listModel::~listModel() {
    delete m_prototype;
}

void listModel::appendRow(listItem *item) {
    appendRows(QList<listItem*>() << item);
}

void listModel::prependRow(listItem *item) {
    beginInsertRows(QModelIndex(), 0, 0);
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.prepend(item);
    endInsertRows();
}

void listModel::appendRows(const QList<listItem *> &items) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount() + items.size() - 1);
    foreach(listItem *item, items) {
        connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
        m_list.append(item);
    }
    endInsertRows();
}

void listModel::insertRow(int row, listItem *item) {
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    m_list.insert(row, item);
    endInsertRows();
}

void listModel::handleItemChange() {
    listItem* item = static_cast<listItem*>(sender());
    QModelIndex index = indexFromItem(item);
    if(index.isValid())
        emit dataChanged(index, index);
}

listItem *listModel::find(const QString &id) const {
    foreach(listItem* item, m_list)
        if(item->id() == id) return item;
    return 0;
}

listItem *listModel::getItem(int row) const {
    if(row >= m_list.size() || row < 0)
        return NULL;
    return m_list[row];
}

QModelIndex listModel::indexFromItem(const listItem *item) const {
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row)
        if(m_list.at(row) == item) return index(row);
    return QModelIndex();
}

void listModel::clear(bool deleteItems) {
    if(!m_list.size())
        return;

    beginRemoveRows(QModelIndex(), 0, m_list.size() - 1);
    if(deleteItems) {
        for(int i = 0; i < m_list.size(); ++i)
            delete m_list[i];
    }

    m_list.clear();
    endRemoveRows();
}

QList<listItem *> *listModel::items() {
    return &m_list;
}

void listModel::sort(cmpFunction cmp) {
    beginResetModel();
    std::sort(m_list.begin(), m_list.end(), cmp);
    endResetModel();
}

bool listModel::removeRow(int row, const QModelIndex &parent) {
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool listModel::removeRow(const QString &id) {
    for(int i = 0; i < m_list.size(); ++i)
        if(m_list[i]->id() == id)
            return removeRow(i);
    return false;
}

bool listModel::removeRows(int row, int count, const QModelIndex &parent) {
    Q_UNUSED(parent);
    if(row < 0 || (row+count) >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i)
        delete m_list.takeAt(row);

    endRemoveRows();
    return true;
}

listItem *listModel::takeRow(int row) {
    beginRemoveRows(QModelIndex(), row, row);
    listItem* item = m_list.takeAt(row);
    endRemoveRows();
    return item;
}

QHash<int, QByteArray> listModel::roleNames() const {
    return m_roles;
}
