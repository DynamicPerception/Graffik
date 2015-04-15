#include "categoryItem.h"

categoryItem::categoryItem(QObject *parent) : listItem(parent) {
}

QVariant categoryItem::data(int role) const {
    switch(role) {
    case IdentifierRole: return m_cid;
    case NameRole: return m_name;
    default: return QVariant();
    }
}

QHash<int, QByteArray> categoryItem::roleNames() const {
    QHash<int, QByteArray> roleNames;
    roleNames[IdentifierRole] = "mir_cid";
    roleNames[NameRole] = "mir_name";
    return roleNames;
}

void categoryItem::setId(const QString &cid) {
    m_cid = cid;
}

void categoryItem::setName(const QString &name) {
    m_name = name;
}
