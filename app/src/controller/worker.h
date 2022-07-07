#ifndef WORKER_H
#define WORKER_H

#include <QObject>


class Worker : public QObject
{
    Q_OBJECT
public:
    Worker() {}
public Q_SLOTS:
    void edit();
    void test();
    void train();
    void trade();
    void result();

signals:
    void editDone();
    void testDone();
    void trainDone();
    void tradeDone();
    void resultDone();

private:

};

#endif // WORKER_H
