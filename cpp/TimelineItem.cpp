#include <QDebug>
#include "TimelineItem.h"

TimelineItem::TimelineItem( const QString &id, 
                            const QString &senderId,
                            const QString &body,
                            const QString &descriptiveTime,
                            qlonglong timestamp,
                            bool isLocal):
    _id(id), _senderId(senderId), _body(body), 
    _descriptiveTime(descriptiveTime), _timestamp(timestamp),
    _isLocal(isLocal)
    {}

QString TimelineItem::id() const{
    return _id;
}

void TimelineItem::setId(const QString &id){
    if (id != _id) {
        _id = id;
    }
}

QString TimelineItem::body() const {
    return _body;
}

void TimelineItem::setBody(const QString &body){
    if (body != _body) {
        _body = body;
    }
}

QString TimelineItem::senderId() const {
    return _senderId;
}

void TimelineItem::setSenderId(const QString &id) {
    if (_senderId != id) {
        _senderId = id;
    }
}

QString TimelineItem::descriptiveTime() const {
    return _descriptiveTime;
}

void TimelineItem::setDescriptiveTime(const QString &descriptiveTime){
    if (_descriptiveTime != descriptiveTime) {
        _descriptiveTime = descriptiveTime;
    }
}

qlonglong TimelineItem::timestamp() const {
    return _timestamp;
}

void TimelineItem::setTimestamp(qlonglong timestamp) {
    if (_timestamp != timestamp) {
        _timestamp = timestamp;
    }
}


bool TimelineItem::isLocal() const{
    return _isLocal;
}

void TimelineItem::setLocal(bool isLocal){
    _isLocal = isLocal;
}


QString TimelineItem::toString(){
    return "{\"ID\":\""     + _id   + "\"," +
            "\"Body\":\""   + _body + "\"," +
            "\"Sender ID\":\""   + _senderId + "\"," +
            "\"Descriptive Time\":\""   + _descriptiveTime + "\"," +
            "\"TimeStamp\":\""   + QString::number(_timestamp) + "\"" +
             "}";
}