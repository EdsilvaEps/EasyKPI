#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <string>
#include <QTimer>
#include <QMessageBox>
#include <prefsdialog.h>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->terminalOutput->setReadOnly(true);
    this->setWindowTitle("KPI Helper");
    this->timer = new QTimer(this);

    adb = new AdbManager("~/Android/Sdk/platform-tools/adb");
    testMan = new TestManager(0,0,4000,adb);

    // connecting testMans signals to widget slots
    connect(testMan, &TestManager::step_finished, this, &MainWindow::on_test_step);
    connect(testMan, &TestManager::test_finished, this, &MainWindow::on_test_finished);
    connect(testMan, &TestManager::test_failed, this, &MainWindow::on_test_failed);
    connect(testMan, &TestManager::test_results_available, this, &MainWindow::on_test_results_available);
    connect(testMan, &TestManager::testing_status_changed, this, &MainWindow::on_testing_status_changed);
    connect(testMan, &TestManager::no_save_path, this, &MainWindow::on_save_path_missing);
    connect(testMan, &TestManager::error, this, &MainWindow::on_error);

    // connecting adbManager signals to widget slots
    connect(adb, &AdbManager::foundDevice, this, &MainWindow::on_device_found);

    // connecting timer with countdown function
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::countdown));

    adb->getConnectedDevices();
    ui->stopTestBtn->setEnabled(false);
    //ui->terminalOutput->append(deviceLog);

    //ui->startTestBtn->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_refreshBtn_clicked()
{
    ui->terminalOutput->append("searching for devices... ");
    adb->getConnectedDevices();
    if(!adb->hasFoundDevices()){
        ui->searchDeviceBox->clear();
        ui->terminalOutput->append("No devices found...");
        ui->startTestBtn->setEnabled(false);

    }

}


void MainWindow::on_browseFilesBtn_clicked()
{
    openFileDialog();

}


void MainWindow::on_startTestBtn_clicked()
{
    int tests = ui->testNumberBox->value();
    int samples = ui->sampleNumberBox->value();
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


void MainWindow::on_stopTestBtn_clicked()
{
  
    ui->terminalOutput->append("test stopped.");
    this->testMan->stopTest();

}

void MainWindow::on_test_finished(int test)
{
    qDebug() << "test finished: ";
    this->printOnScreen("test " + QString::number(test) + " finished.");
}

void MainWindow::on_test_failed(QString why)
{
    qDebug() << "test failed: " << why;

}

void MainWindow::on_test_step(int tests, int samples)
{
    ui->terminalOutput->append("collecting sample " + QString::number(samples) +
                               " of test " + QString::number(tests));

}

void MainWindow::countdown()
{

    this->printOnScreen("test starts in " + QString::number(this->countdownAcc) + "...");
    this->countdownAcc--;
    if(this->countdownAcc == 0){

        this->timer->stop();
        this->testMan->startTest();
        this->countdownAcc = 3;
    }


}

void MainWindow::printOnScreen(QString text)
{
    ui->terminalOutput->append(text);

}

void MainWindow::on_device_found(QString device)
{
    ui->terminalOutput->append("found device " + device);
    if(adb->getSelectedDevice().isEmpty()){
        adb->selectDevice(device);
        ui->terminalOutput->append("selecting device " + device);

        if(ui->searchDeviceBox->findText(device) == -1) ui->searchDeviceBox->addItem(device);

        // TODO: add condition for enabling btn
        ui->startTestBtn->setEnabled(true);
        adb->setBufferSize(64);
    }
}

void MainWindow::on_test_results_available(const QString &res)
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

void MainWindow::on_testing_status_changed(const bool isTesting)
{
    if(isTesting){
        ui->startTestBtn->setDisabled(true);
        ui->stopTestBtn->setDisabled(false);
    } else{
        ui->startTestBtn->setDisabled(false);
        ui->stopTestBtn->setDisabled(true);
    }


}

void MainWindow::warning_message(const QString &msg)
{
    QMessageBox::warning(this, "Warning", msg, QMessageBox::Ok);

}

void MainWindow::on_error(const QString &msg)
{
    QMessageBox::critical(this, "Error", msg, QMessageBox::Ok);

}

void MainWindow::on_save_path_missing(const QString &testData)
{
    int ret = QMessageBox::question(this, "Save Test Files", "Do you wish to save your test data?",
                                                   QMessageBox::Ok | QMessageBox::Cancel);

    if (ret == QMessageBox::Ok){
        qDebug() << "User clicked on OK";
        if(openFileDialog()){
           testMan->saveTest(testData);
           printOnScreen("data saved on " + ui->selectedPathOutput->text());
        }

    }

    if(ret == QMessageBox::Cancel){
        qDebug() << "Usr clicked on cancel";
    }


}

int MainWindow::openFileDialog()
{
    // TODO: make this path more dynamic
    // TODO: add exception for bad path
    QString chosenDir = QFileDialog::getSaveFileName(this, tr("Save test file"),
                                                "/home/edson/untitled.txt",
                                                tr("Text files (*.txt)"));

    if(!chosenDir.isEmpty()){
        ui->selectedPathOutput->setText(chosenDir);
        testMan->setSaveDir(chosenDir);
        printOnScreen(chosenDir + " selected as output path");
        return 1;
    }
    return 0;
}







void MainWindow::on_action_setup_triggered()
{
    PrefsDialog * dialog = new PrefsDialog(this);

    dialog->exec();

}

