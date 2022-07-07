#include "controller.h"
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QMutex>
#include <QFontDatabase>
#include "util.h"

Controller* Controller::mInstance = 0;

Controller::Controller()
{

}

Controller* Controller::instance()
{
    static QMutex mutex;
    if (!mInstance) {
        mutex.lock();
        if (!mInstance)
            mInstance = new Controller();
        mutex.unlock();
    }
    return mInstance;
}

void Controller::init()
{
    // settings
    qApp->setOrganizationName("Algo");
    qApp->setOrganizationDomain("algo.com");
    qApp->setApplicationName("Algo");
    qApp->setApplicationVersion(APP_VERSION);

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;
    QFileInfo fi(settings.fileName());
    mSettingsPath = fi.dir().absolutePath();
    qDebug()<< "temp location:" << mSettingsPath;

    int fontId = QFontDatabase::addApplicationFont(":/fonts/Ubuntu-R.ttf");
    if (fontId != -1)
    {
        QFont font("Ubuntu");
        font.setPointSize(11);
        font.setStyleHint(QFont::SansSerif);
        qApp->setFont(font);
    }

}




