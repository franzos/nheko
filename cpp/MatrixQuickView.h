#pragma once 

#include <QUrl>
#include <QQuickView>
#include <QWindow>
#include "QmlInterface.h"

namespace PX::GUI::MATRIX{
class MatrixQuickView : public QmlInterface, public QQuickView{
public:
    MatrixQuickView(QWindow *parent = nullptr);
};
}