#ifndef WORKER_H
#define WORKER_H

#include <QObject>


class Worker : public QObject
{
    Q_OBJECT
public:
    Worker() {}
public Q_SLOTS:
    void test();
    void train();
    void trade();
signals:
    void testDone();
    void trainDone();
    void tradeDone();
private:

};

#endif // WORKER_H
