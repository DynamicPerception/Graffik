#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>

class listItem: public QObject
{
    Q_OBJECT
public:
    listItem(QObject* parent = 0) : QObject(parent) {}
    virtual ~listItem() {}
    virtual QString id() const = 0;
    virtual QVariant data(int role) const = 0;
    virtual QHash<int, QByteArray> roleNames() const = 0;

signals:
    void dataChanged();
};

typedef bool (*cmpFunction)(listItem*, listItem*);

class listModel : public QAbstractListModel
{
    Q_OBJECT    
public:
    explicit listModel(listItem* prototype, QObject* parent = 0);
    ~listModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(listItem* item);
    void prependRow(listItem* item);
    void appendRows(const QList<listItem*> &items);
    void insertRow(int row, listItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRow(const QString &id);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    listItem* takeRow(int row);
    listItem* find(const QString &id) const;
    listItem* getItem(int row) const;
    QModelIndex indexFromItem( const listItem* item) const;
    QHash<int, QByteArray> roleNames() const;
    void clear(bool deleteItems = true);
    inline int size() const { return m_list.size(); }
    QList<listItem *> *items();
    void sort(cmpFunction cmp);

private slots:
    void handleItemChange();

private:
    listItem* m_prototype;
    QHash<int, QByteArray> m_roles;
    QList<listItem*> m_list;
};

Q_DECLARE_METATYPE(listModel*)

#endif//LISTMODEL_H
