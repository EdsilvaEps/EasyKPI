#ifndef TESTMANAGER_H
#define TESTMANAGER_H

#include "adbManager.h"
#include <QDebug>
#include <QTimer>
#include <QObject>

class TestManager : public QObject
{
    Q_OBJECT

public:
    TestManager(int tests=0, int samples=0, int delay=0, AdbManager* adb=NULL);
    // getters and setters for test variables
    void setTests(int tests);
    void setSamples(int samples);
    void setDelay(int delay);
    // start a KPI test either with arguments or with previously set variables
    void startTest();
    void startTest(int tests, int samples, int delay);

    const QString &saveDir() const;
    void setSaveDir(const QString &newSaveDir);

signals:
    void step_finished(int tests, int samples); // reported when a sample has been collected
    void test_finished(int test); // reported when a test has finished
    void test_failed(QString why); // reported when something happened halfway through the test

 private slots:
    void test_step(); //this triggers a snapshot using the AdbManager obj

 private:
    int _tests;
    int _samples;
    int _delay;
    int _currentTest;
    int _currentSample;
    QString _saveDir;
    AdbManager* _adb;


};


#endif // TESTMANAGER_H
