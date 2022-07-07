#include "googlemp.h"

#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkReply>
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras>
#include <QtAndroidExtras/QAndroidJniObject>
#endif
#include <QUuid>
#include <QSysInfo>
#include <QDateTime>
#include <QScreen>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#else
#include <QtGui/QApplication>
#include <QDesktopWidget>
#endif

#define GMP_MAXBUFFERSIZE 200

qint64 GoogleMP::m_startTime = 0;
QVariantMap GoogleMP::m_extParams;
QVariantMap GoogleMP::m_extSessionParams;
GoogleMP* GoogleMP::m_Instance = 0;

void GoogleMP::startSession(qint64 sTime, QVariantMap extSessionParams)
{
    m_startTime = sTime;
    m_extSessionParams = extSessionParams;
}

GoogleMP::GoogleMP(QObject *parent) : QObject(parent)
{
    //m_appId = qApp->applicationName() + sl(".") + qApp->organizationDomain();
    m_appVersion = qApp->applicationVersion();
    m_appName = qApp->applicationName();

    m_reply = 0;
    m_manager = 0;
    m_uuid = sl("");
    m_ua = sl("");
    m_width = 0;
    m_height= 0;
    m_enabled = false;
    m_bufferfull = false;

    m_settings = new QSettings();
    m_uuid = m_settings->value(sl("cid"),sl("")).toString();
    if (m_uuid.isEmpty()) {
        // no CID yet
        m_uuid = QUuid::createUuid().toString();
        m_uuid.chop(1);
        m_uuid.remove(0,1);
        m_settings->setValue(sl("cid"), m_uuid);
    }

    qDebug() << "Tracking initialized";
    qDebug() << m_uuid;

    m_enabled = isConnectedToNetwork();
#ifndef GMP_ID
    qWarning(sl("GMP_ID missing from .pro file, GMP functionality disabled!"));
    m_enabled = false;
    return;
#endif
    m_manager = new QNetworkAccessManager;
#if QT_VERSION >= 0x050000
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
#else
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
#endif
    m_width = screenGeometry.width();
    m_height = screenGeometry.height();
    m_ua = divineUserAgent();


    if (m_enabled) {
        QVariantMap top;
        top.insert(sl("sc"), sl("start") );
        top.insert(sl("t"), sl("screenview"));      // Screenview hit type.
        top.insert(sl("cd"), sl("splash"));         // We will claim splash is the first screen
        if (m_startTime == 0) {
            m_startTime = QDateTime::currentMSecsSinceEpoch();

        } else { // if timing has already started, report time spent in waiting/queue
            top.insert(sl("qt"), QString::number(QDateTime::currentMSecsSinceEpoch() - m_startTime));
        }
        foreach (QString key, m_extSessionParams.keys()) {
            top.insert(key, m_extSessionParams.value(key));
        }

        appendCommonData(&top);
        QByteArray postData = vmapToPostData(top);
        if (m_enabled) { // online logging
            sendRequest(postData);
            QStringList qsl = m_settings->value(sl("reqbacklog"), QStringList()).toStringList();
            if (qsl.size() > 1) {
                m_settings->setValue(sl("reqbacklog"), QStringList());
                foreach (QString postData, qsl) {
                    sendRequest(postData.toLatin1());
                }
                m_bufferfull = false;
                qDebug() << "Analytics backlog flushed";
                reportEvent(sl("app"), sl("OfflineEvents"), QString::number(qsl.size()));
            }
        } else {
            QStringList qsl = m_settings->value(sl("reqbacklog"), QStringList()).toStringList();
            qsl.append(QString::fromLatin1(postData.data()));
            m_settings->setValue(sl("reqbacklog"), qsl);
            qDebug() << "Net off, added to backlog";
        }
    }
}

QByteArray GoogleMP::vmapToPostData(QVariantMap top) {
    QString postData = QString(sl("v=1&cid=%0&tid=%1")).arg(m_uuid).arg(sl(GMP_ID));
    foreach (QString key, top.keys()) {
        postData += QString(sl("&%0=%1")).arg(key).arg(top[key].toString());
    }
    return postData.toLatin1();
}

void GoogleMP::sendRequest(QByteArray postData) {
    if (m_enabled) {
        QNetworkRequest req;
        req.setUrl(QUrl(sl("http://www.google-analytics.com/collect")));
        req.setHeader(QNetworkRequest::ContentTypeHeader,sl("application/json"));

        if (m_manager != NULL)
            m_reply = m_manager->post(req, postData);
        else
            qDebug() << "Uh-oh, no QNAManager!";

        qDebug() << QString::fromLatin1(postData);
        //connect(m_reply,  SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkreply(QNetworkReply::NetworkError)));
        connect(m_reply, SIGNAL(finished()), this, SLOT(networkreply()));
    } else if (!m_bufferfull) {
        QStringList qsl = m_settings->value(sl("reqbacklog"), QStringList()).toStringList();
        qsl.append(QString::fromLatin1(postData));
        m_settings->setValue(sl("reqbacklog"), qsl);
        qDebug() << QString(sl("Net off, %0 added to backlog")).arg(QString::fromLatin1(postData));
        if (qsl.size() > GMP_MAXBUFFERSIZE) m_bufferfull = true;
    }
}

void GoogleMP::appendCommonData(QVariantMap* base)
{
    if (base) {
        base->insert(sl("an"), m_appName );
        //base->insert(sl("aid"), m_appId );
        base->insert( sl("av"), m_appVersion );
        base->insert( sl("ds"), QString(sl("app")) );
        base->insert( sl("ul"), m_locale.uiLanguages().at(0));
        if (m_width && m_height)
            base->insert( sl("sr"), QString(sl("%0x%1")).arg(m_width).arg(m_height));
        if (!m_ua.isEmpty())
            base->insert( sl("ua"), m_ua);
    }
}

void GoogleMP::showScreen(const QString& screenName)
{
    qDebug() << "showScreen " << screenName;
    QVariantMap top;

    top.insert(sl("t"), sl("screenview"));      // Pageview hit type.
    top.insert(sl("cd"), screenName);      // Pageview hit type.
    appendCommonData(&top);
    sendRequest(vmapToPostData(top));
}

void GoogleMP::startTiming(const QString& eventName, bool overwrite = true)
{
    qDebug() << "startTiming " << eventName << overwrite;
    if (overwrite || !m_timers.contains(eventName)) {
        m_timers.remove(eventName);
        m_timers.insert(eventName, QDateTime::currentMSecsSinceEpoch());
    }
}

void GoogleMP::reportTiming(const QString& eventName, bool reset = true)
{
    qDebug() << "reportTiming " << eventName << reset;
    if (m_timers.contains(eventName) || eventName.startsWith(sl("startup"))) {
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch();
        if (!eventName.startsWith(sl("startup")))
            elapsed -= m_timers.value(eventName);               // Timing
        else
            elapsed -= m_startTime;               // Timing

        reportRawTiming(eventName, elapsed);               // Timing
        qDebug() << "Timing " << eventName;
    } else {
        qDebug() << sl("Report request for non-existent timer ") + eventName + sl(" asked");
    }

    if (reset)
        m_timers.remove(eventName);

}

void GoogleMP::reportRawTiming(const QString& eventName, int msecs)
{
    QVariantMap top;
    top.insert(sl("t"), sl("timing"));      // Pageview hit type.
    top.insert( sl("utc"), eventName);            // Category.
    top.insert( sl("utv"), eventName);            // Variable.
    top.insert( sl("utl"), eventName);            // Label.
    top.insert( sl("utt"), msecs);
    appendCommonData(&top);
    sendRequest(vmapToPostData(top));
}

void GoogleMP::reportEvent(const QString& category, const QString& action, QString value)
{
    qDebug() << "reportEvent " << category << " " << action << " " << value;
    QVariantMap top;

    top.insert( sl("t"), sl("event"));          // Event hit type.
    top.insert( sl("ec"), category);             // Category.
    top.insert( sl("ea"), action);               // Action.
    top.insert( sl("el"), action);               // Label.
    if (!value.isEmpty())
        top.insert( sl("ev"), value);            // Value.

    appendCommonData(&top);

    sendRequest(vmapToPostData(top));
}

void GoogleMP::sendRaw(const QString& rawstring)
{
    qDebug() << "sendRaw";

    if (!rawstring.contains(sl("&t=")))
        qDebug() << "Warning - sendRaw without GMP type specified!";

    sendRequest(rawstring.toLatin1() + vmapToPostData());
}

GoogleMP::~GoogleMP()
{

}

void GoogleMP::networkreply(QNetworkReply::NetworkError error)
{
    if(error != QNetworkReply::NoError) {
        qDebug() << "networkreply";
        if (m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
            qDebug() << m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    //        qDebug() << m_reply->isFinished();
            qDebug() << m_reply->error() << m_reply->errorString();
            qDebug() << m_reply->request().url();
            qDebug() << m_reply->readAll();
            qDebug() << "Nuff said";
        }
    }
}


QString GoogleMP::divineUserAgent()
{
    QString ua=sl("");
    #ifdef Q_OS_ANDROID
    QAndroidJniObject qaj= QAndroidJniObject::callStaticObjectMethod(sl("java/lang/System"), "getProperty"), "(Ljava/lang/String;)Ljava/lang/String;"), QAndroidJniObject::fromString(sl("http.agent")).object());
    ua = qaj.toString();
    if (ua.isEmpty()) {
        ua = QString(sl("Mozilla/5.0 (Linux; Android %0; %1 Build/%2) Qt/%3"))
                                           .arg(QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build$VERSION"), "RELEASE")).toString())
                                           .arg(QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "MODEL")).toString())
                                           .arg(QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "DISPLAY")).toString())
                                           .arg(qVersion());
    }

    /* extra potential info from anroid/os/Build
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "MANUFACTURER")).toString(); // LGE
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "MODEL")).toString();        // Nexus 5
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "PRODUCT")).toString();      // hammerhead
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "HARDWARE")).toString();     // hammerhead
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "DEVICE")).toString();       // hammerhead
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "CPU_ABI")).toString();      // armeabi-v7a
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "BRAND")).toString();        // google
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "SERIAL")).toString();       // 02e05272...
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "DISPLAY")).toString();      // KOT49H
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "BOARD")).toString();        // hammerhead
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build"), "TAGS")).toString();         // release-keys
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build$VERSION"), "RELEASE")).toString(); // 4.4.2
        qDebug() << QAndroidJniObject::getStaticObjectField<jstring>(sl("android/os/Build$VERSION"), "INCREMENTAL")).toString(); // 937116
        qDebug() << QString::number(QAndroidJniObject::getStaticField<jint>(sl("android/os/Build$VERSION"), "SDK_INT")));    // 19

    **/

    #elif defined(Q_OS_IOS)
    ua = QString(sl("Mozilla/5.0 (iPhone; U; CPU iPhone OS %0 like Mac OS X; en)")).arg(QSysInfo::productVersion().replace('.',"_")));
    //    Mozilla/5.0 (iPhone; CPU iPhone OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A5376e Safari/8536.25
    #elif defined(Q_OS_LINUX)
    #if QT5
    ua = QString(sl("Mozilla/5.0 (X11; %0; Linux %1) Qt/%2"))
            .arg(QSysInfo::productType(), QSysInfo::currentCpuArchitecture(), QString::fromLatin1(qVersion()));
    #endif
    //ua = QString(sl("Mozilla/5.0 (X11; Linux x86_64; rv:24.0) Gecko/20100101 Firefox/24.0"));
    //#elif defined(Q_OS_WIN)
    //    ua = QString(sl("Mozilla/5.0 (Windows %0) Qt/%1")).arg(QSysInfo::productType(), qVersion());
    //    ua = QString(sl("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36"));
    #else
    // ... TODO Implement meaningful User-Agent strings for real platforms
    qDebug() << QSysInfo::prettyProductName();
    qDebug() << QSysInfo::productType();
    qDebug() << QSysInfo::productVersion();
    qDebug() << QSysInfo::kernelType();
    qDebug() << QSysInfo::kernelVersion();
    qDebug() << QSysInfo::currentCpuArchitecture();
    #endif

    return ua;
}

bool GoogleMP::isConnectedToNetwork(){

    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for (int i = 0; i < ifaces.count(); i++) {

        QNetworkInterface iface = ifaces.at(i);
        if ( iface.flags().testFlag(QNetworkInterface::IsUp)
             && !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

//            #ifdef QT_DEBUG
//            // details of connection
//            qDebug() << "name:" << iface.name() << endl
//                     << "ip addresses:" << endl
//                     << "mac:" << iface.hardwareAddress() << endl;
//            #endif


            for (int j=0; j<iface.addressEntries().count(); j++) {
//            #ifdef QT_DEBUG
//                qDebug() << iface.addressEntries().at(j).ip().toString()
//                         << " / " << iface.addressEntries().at(j).netmask().toString() << endl;
//            #endif

                // got an interface which is up, and has an ip address
                if (result == false)
                    result = true;
            }
        }

    }

    return result;
}
