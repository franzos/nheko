#ifndef TIME_LINE_ITEM_H
#define TIME_LINE_ITEM_H

#include <QObject>
#include <matrix-client-library/Client.h>

class TimelineItem {
public: 
    TimelineItem(const QString &id, const QString &senderId, 
                 const QString &body, const QString &descriptiveTime, 
                 qlonglong timestamp, bool isLocal);
    TimelineItem();

    QString id() const;
    void setId(const QString &id);

    QString senderId() const;
    void setSenderId(const QString &id);

    QString body() const;
    void setBody(const QString &body);

    QString descriptiveTime() const;
    void setDescriptiveTime(const QString &descriptiveTime);

    qlonglong timestamp() const;
    void setTimestamp(qlonglong timestamp);

    bool isLocal() const;
    void setLocal(bool isLocal);

    QString toString();

private:
    QString _id;
    QString _senderId;
    QString _body;
    QString _descriptiveTime;
    qlonglong _timestamp;
    bool    _isLocal;
};
#endif // TIME_LINE_ITEM_H
