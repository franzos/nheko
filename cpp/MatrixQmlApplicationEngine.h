#pragma once

#include <QUrl>
#include <QQmlApplicationEngine>
#include "QmlInterface.h"

namespace PX::GUI::MATRIX{
class MatrixQmlApplicationEngine : public QmlInterface ,public QQmlApplicationEngine{
public:
    MatrixQmlApplicationEngine(QObject *parent = nullptr);
    void load();
};
}