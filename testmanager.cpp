#include "testmanager.h"


TestManager::TestManager(int tests, int samples, int delay, AdbManager *adb):
    _tests(tests), _samples(samples), _delay(delay)
{
    this->_adb = adb;
    this->_currentSample = 0;
    this->_currentTest = 0;

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

    this->_testing = true;
    emit testing_status_changed(this->_testing);
    this->_adb->clearDeviceLog();

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
    if(!_testing) return;
    qDebug() << "shutter btn";
    this->_adb->clickShutterBtn();
    emit step_finished(this->_currentTest+1, ++this->_currentSample);

    if(this->_currentSample == this->_samples){
        emit test_finished(this->_currentTest+1);
        QString res = this->_adb->getLogResult();
        emit test_results_available(res);
        this->_adb->clearDeviceLog();
        this->_currentTest++;
        this->_currentSample = 0;
        this->_testing = false;
        emit testing_status_changed(_testing);
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

void TestManager::stopTest()
{
    this->_testing = false;
    emit testing_status_changed(this->_testing);
    qDebug() << "testing variable set to false";
}





/*LogCollector::LogCollector(QString adb_path):
    _adb_path(adb_path)
{}

LogCollector::~LogCollector()
{
    qDebug() << "exitting Log collector";
    exit(0);
}

void LogCollector::startCollecting(const QString &device, const int &logsCount)
{
    //QString res = AdbManager::getLogResult(t);
    //emit resultReady(res);
}*/
