#ifndef CATEGORYITEM_H
#define CATEGORYITEM_H

#include "listModel.h"

class categoryItem : public listItem
{
    Q_OBJECT
public:
    enum Roles {
        IdentifierRole = Qt::UserRole + 1,
        NameRole
    };

    explicit categoryItem(QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    void setId(const QString &cid);
    void setName(const QString &name);

    QString id() const { return m_cid; }
    QString name() const { return m_name; }

private:
    QString m_cid;
    QString m_name;
};

#endif // CATEGORYITEM_H
