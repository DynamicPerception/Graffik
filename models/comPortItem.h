#ifndef COMPORTITEM_H
#define COMPORTITEM_H

#include "listModel.h"

class comPortItem : public listItem
{
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1
    };

    explicit comPortItem(QObject *parent = 0);
    explicit comPortItem(const QString &name, QObject *parent = 0);

    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;
    QString id() const { return m_name; }

private:
    QString m_name;
};

#endif // COMPORTITEM_H
