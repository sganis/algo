#include <QRegExp>
#include <QDate>
#include <QVector>
#include "util.h"
#include <functional>
#include <QDebug>
#include <QDir>
#include <QMovie>
#include <QTimer>
#include <QUuid>
#include <QSettings>

#if !defined(Q_OS_WIN)
#include <wordexp.h>
#endif

bool isNumeric(const QString& str)
{
    //QRegExp re("\\d*");
    //return re.exactMatch(str);
    bool ok;
    //double d;
    str.toDouble(&ok);
    return ok;
}



#if defined(Q_OS_WIN)

#elif defined(Q_OS_LINUX)

#elif defined(Q_OS_MAC)

#endif


void setupApplicationIcon(const QString& app_path)
{
    #if defined(Q_OS_WIN)
    Q_UNUSED(app_path)
    #elif defined(Q_OS_MAC)
    Q_UNUSED(app_path)
    #elif defined(Q_OS_LINUX)
    // check if Exec path is the same, not need to recreate file
    QString home_path = QDir::homePath();
    QString app = app_path + sl("/Datanoor");
    QString icon_path = home_path + sl("/.local/share/icons/hicolor");
    QString icon128apps = icon_path +  sl("/128x128/apps/Datanoor.png");
    QFileInfo app_info(app);
    QFileInfo icon128apps_info(icon128apps);

    bool exists = icon128apps_info.exists();
    bool older = app_info.lastModified() > icon128apps_info.lastModified();
    if(!exists || older) {
        qDebug()<< "setting up applicatoin icons...";
        QFile::copy(sl(":/icons/icon_16.png"), icon_path + sl("/16x16/apps/Datanoor.png"));
        QFile::copy(sl(":/icons/icon_32.png"), icon_path + sl("/32x32/apps/Datanoor.png"));
        QFile::copy(sl(":/icons/icon_48.png"), icon_path + sl("/48x48/apps/Datanoor.png"));
        QFile::copy(sl(":/icons/icon_128.png"), icon_path + sl("/128x128/apps/Datanoor.png"));
        QFile::copy(sl(":/icons/icon_256.png"), icon_path + sl("/256x256/apps/Datanoor.png"));
    }

    // also move icons
    QString desktopEntry = QString(
                sl("[Desktop Entry]\n"
                "Version=1.0\n"
                "Type=Application\n"
                "Name=Datanoor\n"
                "GenericName=Data mining tool\n"
                "Comment=Interactive data visualization tool\n"
                "Path=%1\n"
                "Exec=%2/Datanoor %f\n"
                "Terminal=false\n"
                "MimeType=text/plain;text/csv;\n"
                "Icon=Datanoor\n"
                "Categories=Science;Graphics;DataVisualization;"))
            .arg(home_path)
            .arg(app_path);
    QString filename = home_path + sl("/.local/share/applications/Datanoor.desktop");
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        // file.write(desktopEntry.toLatin1(),desktopEntry.length());
        QTextStream stream(&file);
        qDebug() << "application icon set:" << desktopEntry;
        stream << desktopEntry;
    }
    #endif

}

#ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
        // Unsupported platform
    #endif
#elif __linux
    // linux
#elif __unix // all unices not caught above
    // Unix
#elif __posix
    // POSIX
#endif

void str_replace(std::string& str,
                 const std::string& from,
                 const std::string& to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

QString get_temp_filename(const QString& path)
{
    QString name = path;
    #ifdef _WIN32
    name = name.split(sl(":"))[1];
    #endif
    name = name.replace("/","-");
    while(name.startsWith("-"))
        name = name.right(name.length()-1);
    return name;
}

std::string get_dir(const std::string& fullpath)
{
    #if defined(Q_OS_WIN)
    size_t last = fullpath.find_last_of('\\');
    #else
    size_t last = fullpath.find_last_of('/');
    #endif
    std::string path = fullpath.substr(0,last);
    return path;
}

std::string get_fullpath(const std::string& relative_path)
{
    std::string fullpath = "";
    #if defined(Q_OS_WIN)
    char full[_MAX_PATH];
    if( _fullpath( full, relative_path.c_str(), _MAX_PATH ) != NULL )
        fullpath = std::string(full);
    #else
    //Use wordexp to expand for the tilde ~
    wordexp_t we;
    wordexp(relative_path.c_str(), &we, 0);
    char resolvedname[PATH_MAX];
    char* full = realpath(we.we_wordv[0], resolvedname);
    if(full != NULL)
        fullpath = std::string(resolvedname);
    wordfree(&we);
    #endif
    return fullpath;
}

std::string normalize_path(const std::string& path)
{
    std::string fullpath = path;
    #if _MSC_VER
    if(fullpath.find('/'))
        str_replace(fullpath,"/","\\");
    #else
    if(fullpath.find('\\'))
        str_replace(fullpath,"\\","/");
    #endif
    return fullpath;
}

void zip(QString filename, QString zipfilename)
{
    QFile infile(filename);
    QFile outfile(zipfilename);
    infile.open(QIODevice::ReadOnly);
    outfile.open(QIODevice::WriteOnly);
    QByteArray uncompressedData = infile.readAll();
    QByteArray compressedData = qCompress(uncompressedData,9);
    outfile.write(compressedData);
    infile.close();
    outfile.close();
}

void unzip(QString zipfilename, QString filename)
{
    QFile infile(zipfilename);
    QFile outfile(filename);
    infile.open(QIODevice::ReadOnly);
    outfile.open(QIODevice::WriteOnly);
    QByteArray uncompressedData = infile.readAll();
    QByteArray compressedData = qUncompress(uncompressedData);
    outfile.write(compressedData);
    infile.close();
    outfile.close();
}

