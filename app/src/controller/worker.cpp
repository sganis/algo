#include "worker.h"
#include "controller/util.h"


void Worker::edit()
{
    emit editDone();
}
void Worker::test()
{
    QThread::msleep(2000);
    emit testDone();
}

void Worker::train()
{
    emit trainDone();
}

void Worker::trade()
{
    emit tradeDone();
}
void Worker::result()
{
    emit resultDone();
}
