#include "view/mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "controller/pyutil.h"
#include "controller/util.h"
#include "controller/controller.h"


int main(int argc, char *argv[])
{
    printf("starting Datanoor %s...\n", APP_VERSION);
    // c++ is needed to get the app path and set lib path before QApplication instance
    QString app_path = QString::fromLatin1(normalize_path(get_dir(get_fullpath(std::string(argv[0])))).c_str());

    QApplication app(argc, argv);

    // init main controller
    Controller::instance()->init();

    // python
    wchar_t *program = 0;
    python_init(argv[0], app_path, program);


    MainWindow w(QCoreApplication::arguments());
    w.show();

    int exit_code = app.exec();

    //python_finalize();
    if (Py_FinalizeEx() < 0) {
       exit_code = 120;
    }
    PyMem_RawFree(program);
    qDebug()<< "exit code:" << exit_code;
    return exit_code;
}
