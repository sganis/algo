#ifndef GOOGLEMP_H
#define GOOGLEMP_H

#include <QObject>
#include <QLocale>
#include <QSettings>
#include <QMutex>
#include "controller/export.h"

#if QT_VERSION >= 0x050000
#define sl(x) QStringLiteral(x)
#else
#define sl(x) x
#endif

class EXPORTED GoogleMP : public QObject
{
    Q_OBJECT
    Q_PROPERTY (uint startTime READ startTime)
    Q_PROPERTY (QString appName READ appName WRITE setAppName)
    Q_PROPERTY (QString appVersion READ appVersion WRITE setAppVersion)
    //Q_PROPERTY (QString appId READ appId WRITE setAppId)

public:
    static void startSession(qint64 sTime, QVariantMap extSessionParams = QVariantMap());
    static qint64 startTime() { return m_startTime; }
    static QString divineUserAgent();
    static GoogleMP* m_Instance;

    explicit GoogleMP(QObject *parent = 0) ;
    ~GoogleMP() ;

    QString appVersion() { return m_appVersion; }
    QString appName() { return m_appName; }
    //QString appId() { return m_appId; }
    void setAppVersion(QString appVersion) { m_appVersion = appVersion; }
    void setAppName(QString appName) { m_appName = appName; }
    //void setAppId(QString appId) { m_appId = appId; }

    Q_INVOKABLE void showScreen(const QString& screenName) ;
    Q_INVOKABLE void startTiming(const QString& eventName, bool overwrite) ;
    Q_INVOKABLE void reportTiming(const QString& eventName, bool reset) ;
    Q_INVOKABLE void reportRawTiming(const QString& eventName, int msecs) ;
    Q_INVOKABLE void reportEvent(const QString& category, const QString& action, QString value = sl("")) ;
    Q_INVOKABLE void sendRaw(const QString& rawstring) ;

    GoogleMP(const GoogleMP &); // hide copy constructor
    GoogleMP& operator=(const GoogleMP &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static GoogleMP* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
                m_Instance = new GoogleMP();

            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }
signals:

public slots:
    void networkreply(QNetworkReply::NetworkError error=QNetworkReply::NoError) ;

private:
    static qint64 m_startTime;
    static QVariantMap m_extParams;
    static QVariantMap m_extSessionParams;

    static bool isConnectedToNetwork();

    QNetworkReply *m_reply;
    QNetworkAccessManager *m_manager;
    QString m_uuid;

    QMap<QString, qint64> m_timers;
    QLocale m_locale;
    QString m_appName;
    QString m_appVersion;
    //QString m_appId;
    int m_width;
    int m_height;
    QString m_ua;
    bool m_enabled;
    bool m_bufferfull;
    QSettings* m_settings;

    QByteArray vmapToPostData(QVariantMap top = QVariantMap());
    void sendRequest(QByteArray postData);
    void appendCommonData(QVariantMap *base);
};


class GA // after Google Analytics
{
public:
    static void event(const char* category, const char* action, const char* value="")
    {
        GoogleMP::instance()->reportEvent(
                    QString::fromLatin1(category),
                    QString::fromLatin1(action),
                    QString::fromLatin1(value)
                    );
    }
};
#endif // GOOGLEMP_H
