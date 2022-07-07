#pragma once
#include <QColor>
#include <QDebug>
#include <QLabel>
#include <QString>
#include <QThread>
#include <sstream>
#include "controller.h"
#include "controller/export.h"

bool isNumeric(const QString& str);
inline unsigned int randomInt(unsigned int min, unsigned int max) { return (qrand() % (max+1-min)) + min;}
QString get_temp_filename(const QString &path);
void str_replace(std::string& str, const std::string& from, const std::string& to);
EXPORTED std::string	get_fullpath(const std::string& relative_path);
EXPORTED std::string	normalize_path(const std::string& path);
EXPORTED std::string	get_dir(const std::string& fullpath);

inline int to_int(const std::string& s){
    return atoi(s.c_str());
}

inline double to_double(const std::string& s){
    return atof(s.c_str());
}

inline std::string 	to_str(int n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}
inline std::string 	to_str(size_t n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

inline std::string 	to_str(double n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

inline bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
inline QString get_os()
{
    QString os="Unknown";
    #if defined(Q_OS_WIN)
    os = "Windows";
    #elif defined(Q_OS_MAC)
    os = "Mac";
    #elif defined(Q_OS_LINUX)
    os = "Linux";
    #endif
    return os;
}


void zip (QString filename , QString zipfilename);
void unzip (QString zipfilename , QString filename);
//QString getClientID();
void setupApplicationIcon(const QString& app_path);



class SleeperThread : public QThread
{
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};
