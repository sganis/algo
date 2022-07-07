#pragma once

#include <cmath>
#include <QString>
#include <QList>
#include <QDateTime>

#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")

#ifdef Py_DEBUG
#undef Py_DEBUG
#endif

void pyCall(PyObject *instance, const char *method,
          PyObject* args, PyObject *kwargs, PyObject*& result);
QString pyCallString(PyObject *instance, const char* method,
                         PyObject *args, PyObject *kwargs);
QVector<int> pyCallIntList(PyObject *instance, const char* method,
                         PyObject *args, PyObject *kwargs);
QString pyGetAttrString(PyObject* obj, const char* attr);


inline int python_init(const char* prog, const QString& app_path, wchar_t *program)
{
    // create python interpreter, add plugins folder to sys.path

    // Py_NoSiteFlag=1;
    program = Py_DecodeLocale(prog, NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);  /* optional but recommended */
    Py_InitializeEx(0);
    if( !Py_IsInitialized() ) {
        qDebug("Unable to initialize Python interpreter.");
        return 1;
    }
    printf("program path from python: %ls\n", Py_GetProgramFullPath());
    PyRun_SimpleString("from time import time,ctime\n"
                           "print('Today is', ctime(time()))\n");


    // set python path
#if defined(Q_OS_WIN)
    //PyObject* sys_path = PySys_GetObject((char*)"path");
    // clear sys.path../
    PySys_SetObject((char*)"path", PyList_New(0));
    PyObject* sys_path = PySys_GetObject((char*)"path");
    PyList_Insert(sys_path, 0, PyUnicode_FromString((char*)QString(app_path +QStringLiteral("/plugins")).toStdString().c_str()));
    PyList_Insert(sys_path, 0, PyUnicode_FromString((char*)QString(app_path +QStringLiteral("/python36.zip")).toStdString().c_str()));
    PyList_Insert(sys_path, 0, PyUnicode_FromString((char*)app_path.toStdString().c_str()));
#elif defined(Q_OS_LINUX)
    // clear sys.path
    //PySys_SetObject((char*)"path", PyList_New(0));
    PyObject* sys_path = PySys_GetObject("path");
    //PyList_Insert(sys_path, 0, PyString_FromString((char*)QString(path +"/lib/python27.zip").toStdString().c_str()));
    PyRun_SimpleString("import sys\n"
                       "print('python path:')\n"
                       "print('\\n\'.join(sys.path))\n"
                       "sys.stdout.flush()\n");
    PyList_Insert(sys_path, 0, PyUnicode_FromString((char*)QString(app_path + QStringLiteral("/plugins")).toStdString().c_str()));
    //PyList_Insert(sys_path, 0, PyString_FromString((char*)QString(path +"/lib").toStdString().c_str()));
#elif defined(Q_OS_MAC)
    // clear sys.path
    PyObject* sys_path = PySys_GetObject((char*)"path");
    PyRun_SimpleString("import sys; print ('python path:'); print('\\n'.join(sys.path)); sys.stdout.flush()");
    PyList_Insert(sys_path, 0, PyUnicode_FromString((char*)QString(app_path +"/strategies").toStdString().c_str()));
#endif

    // test
    PyRun_SimpleString("import sys\n"
                       "print('python path after update:')\n"
                       "print('\\n'.join(sys.path))\n"
                       "sys.stdout.flush()\n");
    qDebug("loading textimport plugin...");
    PyRun_SimpleString("import textimport");
    PyRun_SimpleString("print(textimport.__file__)");

    return 0;
}

