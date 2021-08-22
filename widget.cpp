#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <string>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->terminalOutput->setReadOnly(true);
    this->setWindowTitle("KPI Helper");
    this->timer = new QTimer(this);
    //ui->startTestBtn->setEnabled(false);
    //system("~/Android/Sdk/platform-tools/adb devices");

    adb = new AdbManager("~/Android/Sdk/platform-tools/adb");
    testMan = new TestManager(0,0,4000,adb);

    // connecting testMans signals to widget slots
    connect(testMan, &TestManager::step_finished, this, &Widget::on_test_step);
    connect(testMan, &TestManager::test_finished, this, &Widget::on_test_finished);
    connect(testMan, &TestManager::test_failed, this, &Widget::on_test_failed);

    // connecting adbManager signals to widget slots
    connect(adb, &AdbManager::foundDevice, this, &Widget::on_device_found);

    adb->getConnectedDevices();
    //ui->terminalOutput->append(deviceLog);

    ui->startTestBtn->setEnabled(false);



}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_startTestBtn_clicked()
{
    int tests = ui->testNumberBox->value();
    int samples = ui->samplesNumberBox->value();
    int delay = ui->delayTime->value();

    if(tests <= 0 || samples <= 0 || delay <= 0){
        ui->terminalOutput->append("choose a valid amount of tests/samples and/or a valid interval time.");
        return;
    }

    testMan->setTests(tests);
    testMan->setSamples(samples);
    testMan->setDelay(delay);

    ui->terminalOutput->append("starting test with " + QString::number(tests) + " tests and " +
                               QString::number(samples) + " samples...");


    this->countdownAcc = 3;

    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::countdown));

    timer->start(1000);



}


void Widget::on_stopTestBtn_clicked()
{

    ui->terminalOutput->append("test stopped.");

}

void Widget::on_test_finished(int test)
{
    qDebug() << "test finished: ";
    ui->terminalOutput->append("test " + QString::number(test) + " finished.");



}

void Widget::on_test_failed(QString why)
{
    qDebug() << "test failed: " << why;

}

void Widget::on_test_step(int tests, int samples)
{
    ui->terminalOutput->append("collecting sample " + QString::number(samples) +
                               " of test " + QString::number(tests));

}

void Widget::countdown()
{

    ui->terminalOutput->append("test starts in " + QString::number(this->countdownAcc) + "...");
    --this->countdownAcc;
    if(this->countdownAcc == 0){

        this->timer->stop();
        this->testMan->startTest();
    }


}


void Widget::on_refreshBtn_clicked()
{
    ui->terminalOutput->append("searching for devices... ");
    adb->getConnectedDevices();
    if(!adb->hasFoundDevices()){
        ui->searchDeviceBox->clear();
        ui->terminalOutput->append("No devices found...");
        ui->startTestBtn->setEnabled(false);

    }




}

void Widget::on_device_found(QString device)
{
    ui->terminalOutput->append("found device " + device);
    if(adb->getSelectedDevice().isEmpty()){
        adb->selectDevice(device);
        ui->terminalOutput->append("selecting device " + device);

        if(ui->searchDeviceBox->findText(device) == -1) ui->searchDeviceBox->addItem(device);

        // TODO: add condition for enabling btn
        ui->startTestBtn->setEnabled(true);
    }
}

