#ifndef TIME_LINE_MODE_H
#define TIME_LINE_MODE_H

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>
#include <matrix-client-library/Client.h>

#include "TimelineItem.h"

class TimelineModel : public QAbstractListModel{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
public:
    enum TimelineItemRoles {
        idRole = Qt::UserRole + 1,
        senderIdRole,
        bodyRole,
        descriptiveTimeRole,
        timestampRole,
        isLocalRole
    };

    TimelineModel(const QString roomID = "", QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int  timelineIdToIndex(const QString &roomid);

protected:
    QHash<int, QByteArray> roleNames() const;

public slots:
    void add(TimelineItem &item);
    void add(QList<TimelineItem> &items);
    void remove(const QStringList &ids);
    void send(const QString &message);

signals:
    void rowCountChanged();
    void typingUsersChanged(const QString &text);

private:
    void add(const QVector<DescInfo> &items);
    QList<TimelineItem> _timelineItems;
    QStringList _messageIds;
    Timeline *_timeline = nullptr;
};
#endif
