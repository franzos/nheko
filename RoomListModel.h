#ifndef ROOMLISTMODEL_H
#define ROOMLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>

class RoomListItem {
public: 
    RoomListItem(const QString &id, const QString &name, const QString &avatar, bool invite);

    QString id();
    QString name();
    QString avatar();
    bool    invite();
    QString toString();

private:
    QString _id;
    QString _name;
    QString _avatar;
    bool _invite;
};

class RoomListModel : public QAbstractListModel{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
public:
    RoomListModel(QObject *parent = 0)
        : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,int role = Qt::EditRole) override;

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    int  roomidToIndex(const QString &roomid);
public slots:
    void add(RoomListItem &item);
    void add(QVector<RoomListItem> &item);
    void remove(const QStringList &ids);

signals:
    void rowCountChanged();

private:
    QStringList _roomIds;
    QStringList _roomList;
};


#endif
