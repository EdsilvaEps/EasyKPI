#ifndef TESTMANAGER_H
#define TESTMANAGER_H

#include <adbmanager.h>
#include <QDebug>
#include <QTimer>
#include <QObject>
#include <QList>
#include <QThread>
#include <cstdlib>
#include <iostream>
#include <fstream>

class TestManager : public QObject
{
    Q_OBJECT

public:
    //TestManager(int samples=0, int delay=0, AdbManager* adb=NULL);
    TestManager(AdbManager *adb=NULL);
    // getters and setters for test variables
    //void setTests(int tests);
    void setSamples(int samples);
    void setDelay(int delay);
    // start a KPI test either with arguments or with previously set variables
    void startTest();
    void startTest(int samples, int delay);

    const QString &saveDir() const;
    void setSaveDir(const QString &newSaveDir);
    void stopTest(); // stops an ongoing test routine
    void saveTest(QString testData); // function for saving test data to file.

signals:
    void step_finished(int samples); // reported when a sample has been collected
    void test_finished(); // reported when a test has finished
    void test_failed(QString why); // reported when something happened halfway through the test
    void start_collect(const QString &device, const int &logCount, const int &duration);
    void test_results_available(const QString &res);
    void testing_status_changed(const bool isTesting); // reported when testing should start or end
    void test_saved(); // reported when test is successfully saved
    void no_save_path(QString testData); // reported when there's no save path - send testdata so user can decide what to do with it
    void error(const QString &err); // reported when an error is caught

 private slots:
    void test_step(); //this triggers a snapshot using the AdbManager obj
    void handleLogResults(const QString &);

 private:
    int _samples;
    int _delay;
    int _currentSample;
    double _bufferIncrease;
    bool _testing = false;
    QString _saveDir;
    AdbManager* _adb;



};



#endif // TESTMANAGER_H

