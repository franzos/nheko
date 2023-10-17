#pragma once

#include <QUrl>
#include <QQmlApplicationEngine>
#include "QmlInterface.h"

namespace PX::GUI::MATRIX{

struct ApplicationOptions {
    enum class AppFeatures : quint8 {
        keybackup, menu, settings_interface
    };
    static QMap<AppFeatures, QString> APP_FEATURES_DICT;

    enum class AppMenuEntries : quint8 {
        profile,settings,my_qr_code,logout,about,
    };
    static QMap<AppMenuEntries, QString> APP_MENU_ENTRIES_DICT;

    bool appendHiddenFeature(const QString& str) {
        for (const auto& key : APP_FEATURES_DICT.keys()) {
            if (APP_FEATURES_DICT[key].toLower() == str.toLower()) {
                this->hiddenFeatures.append(key);
                return true;
            }
        }
        return false;
    }

    bool appendVisibleFeature(const QString& str) {
        for (const auto& key: APP_FEATURES_DICT.keys()) {
            if (APP_FEATURES_DICT[key].toLower() == str.toLower()) {
                this->visibleFeatures.append(key);
                return true;
            }
        }
        return false;
    }

    bool appendHiddenMenuEntry(const QString& str) {
        for (const auto& key: APP_MENU_ENTRIES_DICT.keys()) {
            if (APP_MENU_ENTRIES_DICT[key].toLower() == str.toLower()) {
                this->hiddenMenuEntries.append(key);
                return true;
            }
        }
        return false;
    }

    QList<AppFeatures> hiddenFeatures;
    QList<AppMenuEntries> hiddenMenuEntries;
    QList<AppFeatures> visibleFeatures;
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
