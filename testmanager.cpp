#include "testmanager.h"


TestManager::TestManager(int tests, int samples, int delay, AdbManager *adb):
    _tests(tests), _samples(samples), _delay(delay)
{
    this->_adb = adb;
    this->_currentSample = 0;
    this->_currentTest = 0;

    LogCollector *collector = new LogCollector(this->_adb->getAbsPath());
    collector->moveToThread(&workerThread);
    // connect signals and slots between LogCollector and TestManager
    connect(&workerThread, &QThread::finished, collector, &QObject::deleteLater);
    connect(this, &TestManager::start_collect, collector, &LogCollector::startCollecting);
    connect(collector, &LogCollector::resultReady, this, &TestManager::handleLogResults);
    workerThread.start();

}

TestManager::~TestManager()
{
    this->workerThread.quit();
    this->workerThread.wait();

}

void TestManager::setTests(int tests){
    this->_tests = tests;
}

void TestManager::setSamples(int samples){
    this->_samples = samples;
}

void TestManager::setDelay(int delay){
    this->_delay = delay;
}

void TestManager::startTest(){
    if(this->_adb == NULL){ throw logic_error("no adb instance provided"); }
    if(this->_tests <= 0 || this->_samples <= 0 || this->_delay <= 0){ throw logic_error("invalid test parameters"); }

    emit start_collect(this->_adb->getSelectedDevice(), this->_samples, this->_delay*2);

    try{

        int timer = 10; // inicial interval
        for(int tst = 0; tst < this->_tests; tst++){
            for(int smp = 0; smp < this->_samples; smp++){
                qDebug() << "Test " << tst+1 << " Sample " << smp+1 ;
                QTimer::singleShot(timer, this ,SLOT(test_step()));
                timer += this->_delay;

            }
        }


    } catch(exception &ex){
        qDebug() << "exception " << ex.what() << " in TestManager::startTest()";
        emit test_failed(QString::fromStdString(ex.what()));
    }



}

void TestManager::startTest(int tests, int samples, int delay)
{
    this->setTests(tests);
    this->setSamples(samples);
    this->setDelay(delay);
    this->startTest();

}

void TestManager::test_step()
{
    qDebug() << "shutter btn";
    this->_adb->clickShutterBtn();
    emit step_finished(this->_currentTest+1, ++this->_currentSample);

    if(this->_currentSample == this->_samples){
        emit test_finished(this->_currentTest+1);
        //this->_adb->getLogResult();
        this->_currentTest++;
        this->_currentSample = 0;
    }

    // test is over
    if(this->_currentTest > this->_tests){
        this->_currentSample = 0;
        this->_currentTest = 0;
    }




}

void TestManager::handleLogResults(const QString &res)
{
    qDebug() << res;
    emit test_results_available(res);
}

const QString &TestManager::saveDir() const
{
    return _saveDir;
}

void TestManager::setSaveDir(const QString &newSaveDir)
{
    _saveDir = newSaveDir;
}





LogCollector::LogCollector(QString adb_path):
    _adb_path(adb_path)
{}

void LogCollector::startCollecting(const QString &device, const int &logsCount, const int &time)
{
    // TODO: end this task after the end of 'time' constraint
    int t = time;
    QString res = AdbManager::getLogResult(this->_adb_path, device, logsCount);
    emit resultReady(res);
}
