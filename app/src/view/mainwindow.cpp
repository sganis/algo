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
    connect(mWorker, SIGNAL(editDone()), this, SLOT(onEditDone()));
    connect(mWorker, SIGNAL(testDone()), this, SLOT(onTestDone()));
    connect(mWorker, SIGNAL(trainDone()), this, SLOT(onTrainDone()));
    connect(mWorker, SIGNAL(tradeDone()), this, SLOT(onTradeDone()));
    connect(mWorker, SIGNAL(resultDone()), this, SLOT(onResultDone()));
    connect(this, SIGNAL(editRequested()), mWorker, SLOT(edit()));
    connect(this, SIGNAL(testRequested()), mWorker, SLOT(test()));
    connect(this, SIGNAL(trainRequested()), mWorker, SLOT(train()));
    connect(this, SIGNAL(tradeRequested()), mWorker, SLOT(trade()));
    connect(this, SIGNAL(resultRequested()), mWorker, SLOT(result()));

    // start worker
    mThread.start();

    ui->pbTest->setFocus();

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

void MainWindow::on_pbTest_clicked()
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

