#include "comPortItem.h"

comPortItem::comPortItem(QObject *parent) : listItem(parent) {
}

comPortItem::comPortItem(const QString &name, QObject *parent) {
    m_name = name;
}

QVariant comPortItem::data(int role) const {
    switch(role) {
    case NameRole: return m_name;
    default: return QVariant();
    }
}

QHash<int, QByteArray> comPortItem::roleNames() const {
    QHash<int, QByteArray> roleNames;
    roleNames[NameRole] = "name";
    return roleNames;
}
