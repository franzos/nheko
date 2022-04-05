#pragma once 

#include <QUrl>
#include <QQuickView>
#include <QQuickItem>
#include <QWindow>
#include "QmlInterface.h"

namespace PX::GUI::MATRIX{
class MatrixQuickView : public QmlInterface, public QQuickView{
public:
    MatrixQuickView(QWindow *parent = nullptr);
    QQuickView *videoCallPage();
    
public slots:
    void setVideoCallItem() override;

private: 
    QQuickView *_videoCallQuickView = nullptr;
};
}