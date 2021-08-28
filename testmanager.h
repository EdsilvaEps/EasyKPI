#ifndef TESTMANAGER_H
#define TESTMANAGER_H

#include <adbmanager.h>
#include <QDebug>
#include <QTimer>
#include <QObject>
#include <QList>
#include <QThread>

class TestManager : public QObject
{
    Q_OBJECT

public:
    TestManager(int tests=0, int samples=0, int delay=0, AdbManager* adb=NULL);
    ~TestManager();
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
    void start_collect(const QString &device, const int &logCount, const int &duration);
    void test_results_available(const QString &res);

 private slots:
    void test_step(); //this triggers a snapshot using the AdbManager obj
    void handleLogResults(const QString &);

 private:
    int _tests;
    int _samples;
    int _delay;
    int _currentTest;
    int _currentSample;
    QString _saveDir;
    AdbManager* _adb;
    QThread workerThread;



};

/* class for multithreading - will capture the logs while the
   kpi test is happening on the main thread */
class LogCollector : public QObject{
    Q_OBJECT

    QString _adb_path;

public:
    LogCollector(QString adb_path);

public slots:
    // function that collects the logs, receives int logsCount and int time
    // logsCount - number of logs to collect
    // time - duration of test
    void startCollecting(const QString &device, const int &logsCount, const int &time);

signals:
    void resultReady(const QString result);
};


#endif // TESTMANAGER_H
