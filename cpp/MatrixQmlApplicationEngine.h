#pragma once

#include <QUrl>
#include <QQmlApplicationEngine>
#include "QmlInterface.h"

namespace PX::GUI::MATRIX{

enum class ApplicationFeatures
{
    KeyBackup,
};

struct ApplicationOptions {
    QList<ApplicationFeatures> hiddenFeatures;

    static QMap<QString, ApplicationFeatures> FeatureStrings;
};

class MatrixQmlApplicationEngine : public QmlInterface ,public QQmlApplicationEngine{
public:
    MatrixQmlApplicationEngine(QObject *parent = nullptr, ApplicationOptions options = ApplicationOptions());
    void load(bool callAutoAccept = false);
public slots:
    void setVideoCallItem() override;

protected:
    ApplicationOptions m_options;
};
}