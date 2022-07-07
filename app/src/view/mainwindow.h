#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "controller/worker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QStringList &args, QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void editRequested();
    void testRequested();
    void trainRequested();
    void tradeRequested();
    void resultRequested();

private slots:
    void on_pbEdit_released();
    void on_pbTest_clicked();
    void on_pbTrain_released();
    void on_pbTrade_released();
    void on_pbResult_released();
    void onEditDone();
    void onTestDone();
    void onTrainDone();
    void onTradeDone();
    void onResultDone();



private:
    Ui::MainWindow *ui;
    Worker* mWorker;
    QThread mThread;
};
#endif // MAINWINDOW_H
