#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(const QStringList &args, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    mWorker = new Worker();
    mWorker->moveToThread(&mThread);
    connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));
    connect(this, SIGNAL(testRequested()), mWorker, SLOT(test()));
    connect(mWorker, SIGNAL(testDone()), this, SLOT(onTestDone()));
    connect(this, SIGNAL(trainRequested()), mWorker, SLOT(train()));
    connect(mWorker, SIGNAL(trainDone()), this, SLOT(onTrainDone()));
    connect(this, SIGNAL(tradeRequested()), mWorker, SLOT(trade()));
    connect(mWorker, SIGNAL(tradeDone()), this, SLOT(onTradeDone()));

    // start worker
    mThread.start();


}

MainWindow::~MainWindow()
{
    mThread.quit();
    mThread.wait();
    delete ui;
}

void MainWindow::on_pbEdit_released()
{
    ui->pbEdit->setEnabled(false);
    emit this->editRequested();
}

void MainWindow::on_pbTest_released()
{
    ui->pbTest->setEnabled(false);
    emit this->testRequested();
}

void MainWindow::onTestDone()
{
    ui->pbTest->setEnabled(true);
}
void MainWindow::onEditDone()
{
    ui->pbEdit->setEnabled(true);
}

void MainWindow::on_pbTrain_released()
{
    ui->pbTrain->setEnabled(false);
    emit this->trainRequested();
}
void MainWindow::onTrainDone()
{
    ui->pbTrain->setEnabled(true);
}
void MainWindow::on_pbTrade_released()
{
    ui->pbTrade->setEnabled(false);
    emit this->tradeRequested();
}
void MainWindow::onTradeDone()
{
    ui->pbTrade->setEnabled(true);
}
void MainWindow::on_pbResult_released()
{
    ui->pbResult->setEnabled(false);
    emit this->resultRequested();
}
void MainWindow::onResultDone()
{
    ui->pbResult->setEnabled(true);
}
