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
    void setTests(int tests);
    void setSamples(int samples);
    void setDelay(int delay);
    void startTest();
    void startTest(int tests, int samples, int delay);

    //static void msleep(unsigned long msecs){QThread::msleep(msecs);}

 signals:
    void step_finished(int tests, int samples);
    void test_finished(int test);
    void test_failed(QString why);

 private slots:
    void test_step();

 private:
    int _tests;
    int _samples;
    int _delay;
    int _currentTest;
    int _currentSample;
    AdbManager* _adb;


};


#endif // TESTMANAGER_H
