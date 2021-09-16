#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <string>
#include <QTimer>
#include <QMessageBox>

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->terminalOutput->setReadOnly(true);
    this->setWindowTitle("KPI Helper");
    this->timer = new QTimer(this);

    adb = new AdbManager("~/Android/Sdk/platform-tools/adb");
    testMan = new TestManager(0,0,4000,adb);

    adb->setBufferSize(64);

    // connecting testMans signals to widget slots
    connect(testMan, &TestManager::step_finished, this, &Widget::on_test_step);
    connect(testMan, &TestManager::test_finished, this, &Widget::on_test_finished);
    connect(testMan, &TestManager::test_failed, this, &Widget::on_test_failed);
    connect(testMan, &TestManager::test_results_available, this, &Widget::on_test_results_available);
    connect(testMan, &TestManager::testing_status_changed, this, &Widget::on_testing_status_changed);

    // connecting adbManager signals to widget slots
    connect(adb, &AdbManager::foundDevice, this, &Widget::on_device_found);

    // connecting timer with countdown function
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::countdown));

    adb->getConnectedDevices();
    ui->stopTestBtn->setEnabled(false);
    //ui->terminalOutput->append(deviceLog);

    //ui->startTestBtn->setEnabled(false);



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
        printOnScreen("choose a valid amount of tests/samples and/or a valid interval time.");
        return;
    }

    if(this->adb->getSelectedDevice().isEmpty()){
        printOnScreen("connect a device to the usb port");
        return;
    }

    testMan->setTests(tests);
    testMan->setSamples(samples);
    testMan->setDelay(delay);

    printOnScreen("starting test with " + QString::number(tests) + " tests and " +
                  QString::number(samples) + " samples...");

    this->countdownAcc = 3;
    timer->start(1000);


}


void Widget::on_stopTestBtn_clicked()
{

    ui->terminalOutput->append("test stopped.");
    this->testMan->stopTest();

}

void Widget::on_test_finished(int test)
{
    qDebug() << "test finished: ";
    this->printOnScreen("test " + QString::number(test) + " finished.");
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

    this->printOnScreen("test starts in " + QString::number(this->countdownAcc) + "...");
    this->countdownAcc--;
    if(this->countdownAcc == 0){

        this->timer->stop();
        this->testMan->startTest();
        this->countdownAcc = 3;
    }


}

void Widget::printOnScreen(QString text)
{
    ui->terminalOutput->append(text);

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


void Widget::on_browseFilesBtn_clicked()
{
    QStringList chosenDir;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);

    if(dialog.exec()){
        chosenDir = dialog.selectedFiles();
        qDebug() << "chosen: " << chosenDir;

        if(!chosenDir.isEmpty()){
            ui->selectedPathOutput->setText(chosenDir[0]);
            testMan->setSaveDir(chosenDir[0]);
            this->printOnScreen("'" + chosenDir[0] + "'" + " selected as output path.");

        }
    }

}

void Widget::on_test_results_available(const QString &res)
{
    printOnScreen("\n----RAW TEST RESULTS----");
    printOnScreen(res);

    // example: 09-13 12:30:10.693 11667 12893 I CameraKpiTag: SHOT_TO_SHOT_O : 294 ms
    // TODO: maybe this processing does not belong to the widget, change that later.
    QRegularExpression re("SHOT_TO_SHOT_O : ([0-9]+)", QRegularExpression::MultilineOption);

    QRegularExpressionMatchIterator match = re.globalMatch(res);
    qDebug() << "text to process";
    qDebug() << res;
    if(!match.hasNext()){
        // TODO: put this on a rss file
        QString warning_msg = "Your test returned no results, make sure you "
"have the MotCamera app open and the device is running on an user build.";

        this->warning_message(warning_msg);
        qDebug() << "found nothing on regex";
    }
    printOnScreen("----TREATED RESULTS----");
    while(match.hasNext()){
        QRegularExpressionMatch m = match.next();
        qDebug() << m.captured(1);
        printOnScreen(m.captured(1));
    }

}

void Widget::on_testing_status_changed(const bool isTesting)
{
    if(isTesting){
        ui->startTestBtn->setDisabled(true);
        ui->stopTestBtn->setDisabled(false);
    } else{
        ui->startTestBtn->setDisabled(false);
        ui->stopTestBtn->setDisabled(true);
    }


}

void Widget::warning_message(const QString &msg)
{
    QMessageBox *msgBox = new QMessageBox;
    msgBox->setText(msg);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->exec();

}

