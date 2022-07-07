#include "worker.h"



void Worker::test()
{
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
