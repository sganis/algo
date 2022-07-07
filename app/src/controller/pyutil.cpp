#include "pyutil.h"
#include <cmath>
#include <Python.h>
#include <QDebug>

void pyCall(PyObject *instance,
            const char* method,
                PyObject *args,
                PyObject *kwargs,
                PyObject *&result)
{
    // delimiter
    PyObject *pmethod;
    pmethod = PyObject_GetAttrString(instance, method); // fetch method
    if (pmethod == NULL)
        qDebug("Can't fetch method %s",method);
    if (args == NULL) {
        args = Py_BuildValue("()");   // convert arg to Python
        if (args == NULL) {
            Py_DECREF(pmethod);
            qDebug("Can't build arguments list for method");
        }
    } else {
        //qDebug("Calling method with arguments...");
    }
    if (kwargs != NULL) {
        //qDebug("Calling method with keywords arguments...");
    }
    result = PyEval_CallObjectWithKeywords(pmethod, args, kwargs);
    if (PyErr_Occurred())
        PyErr_Print();
    Py_DECREF(pmethod);
    Py_DECREF(args);
    if(kwargs != NULL)
        Py_DECREF(kwargs);
    if (result == NULL)
        qDebug("Error calling Importer.%s", method);
}
QString pyCallString(PyObject *instance, const char* method,
                         PyObject *args, PyObject *kwargs)
{
    QString result = QStringLiteral("");
    PyObject* presult;
    pyCall(instance, method, args, kwargs, presult);
    if(PyUnicode_Check(presult))
        result = QString::fromUtf8(PyUnicode_AsUTF8(presult));
    Py_DECREF(presult);
    return result;
}

QString pyGetAttrString(PyObject* obj, const char* attr)
{
    QString attribute = QStringLiteral("");
    PyObject* pattr = PyObject_GetAttrString(obj,attr);
    if(PyUnicode_Check(pattr)) {
        attribute = QString::fromUtf8(PyUnicode_AsUTF8(pattr));
        Py_DECREF(pattr);
    }
    return attribute;
}


QVector<int> pyCallIntList(PyObject *instance, const char* method,
                         PyObject *args, PyObject *kwargs)
{
    PyObject *presult;
    QVector<int> list;
    pyCall(instance, method, args, kwargs, presult);
    if(PySequence_Check(presult)) {
        int size = PySequence_Size(presult);
        for(int i = 0; i < size; i++) {
            PyObject *item = PySequence_GetItem(presult, i);
            if(PyLong_Check(item))
                list.append(PyLong_AsSsize_t(item));
            Py_DECREF(item);
        }
    }
    Py_DECREF(presult);
    return list;
}


