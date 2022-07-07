#pragma once
#include <qconfig.h>
#include <QString>
#include "controller/export.h"

#ifndef APP_VERSION
#define APP_VERSION             "1.0"
#endif


class EXPORTED Controller
{
public:
    Controller();
    static Controller* mInstance;

    static Controller* instance();
    void init();

    QString settingsPath() const { return mSettingsPath; }
    void setSettingsPath(const QString &path) { mSettingsPath = path;}

private:
    QString mSettingsPath;
};




