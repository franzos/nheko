#pragma once

#include <QUrl>
#include <QQmlApplicationEngine>
#include "QmlInterface.h"

namespace PX::GUI::MATRIX{

struct ApplicationOptions {
    enum class AppFeatures : quint8 {
        keybackup
    };
    static QMap<AppFeatures, QString> APP_FEATURES_DICT;

    enum class AppMenuEntries : quint8 {
        profile,settings,my_qr_code,logout,about,
    };
    static QMap<AppMenuEntries, QString> APP_MENU_ENTRIES_DICT;

    template<typename T>
    bool appendFromStr(const QString& str) {
        if (std::is_same<T, AppFeatures>::value) {
            for (const AppFeatures& key : APP_FEATURES_DICT.keys()) {
                if (ApplicationOptions::APP_FEATURES_DICT[key].toLower() == str.toLower()) {
                    hiddenFeatures.append(key);
                    return true;
                }
            }
        } else if (std::is_same<T, AppMenuEntries>::value) {
            for (const AppMenuEntries& key : APP_MENU_ENTRIES_DICT.keys()) {
                if (APP_MENU_ENTRIES_DICT[key].toLower() == str.toLower()) {
                    hiddenMenuEntries.append(key);
                    return true;
                }
            }
        }
        return false;
    }
    QList<AppFeatures> hiddenFeatures;
    QList<AppMenuEntries> hiddenMenuEntries;
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
