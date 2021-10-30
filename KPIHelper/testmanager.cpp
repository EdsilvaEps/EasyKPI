#include "testmanager.h"
#include "qsettings.h"

TestManager::TestManager(AdbManager *adb)
{
    QSettings settings("IPE", "KPIHelper");
    settings.beginGroup("settings");
    int samples = settings.value("samples", QVariant(0)).toInt();
    double interval = settings.value("interval", QVariant(0)).toDouble();
    double bufferIncrease =  settings.value("bufferIncrease", QVariant(0)).toDouble();
    settings.endGroup();

    this->_samples = samples;
    this->_delay = interval;
    this->_bufferIncrease = bufferIncrease;
    this->_adb = adb;
    this->_currentSample = 0;

}


/*void TestManager::setTests(int tests){
    this->_tests = tests;
}*/

void TestManager::setSamples(int samples){
    this->_samples = samples;
}

void TestManager::setDelay(int delay){
    this->_delay = delay;
}

void TestManager::startTest(){
    if(this->_adb == NULL){ throw logic_error("no adb instance provided"); }
    if(this->_samples <= 0 || this->_delay <= 0){ throw logic_error("invalid test parameters"); }

    this->_testing = true;
    emit testing_status_changed(this->_testing);
    this->_adb->clearDeviceLog();

    try{

        int timer = 10; // inicial interval
        for(int smp = 0; smp < this->_samples; smp++){
            qDebug() << " Sample " << smp+1 ;
            QTimer::singleShot(timer, this ,SLOT(test_step()));
            timer += this->_delay;

        }


    } catch(exception &ex){
        qDebug() << "exception " << ex.what() << " in TestManager::startTest()";
        emit test_failed(QString::fromStdString(ex.what()));
    }


}

void TestManager::startTest(int samples, int delay)
{
    this->setSamples(samples);
    this->setDelay(delay);
    this->startTest();

}

void TestManager::test_step()
{
    if(!_testing) return;
    qDebug() << "shutter btn";
    this->_adb->clickShutterBtn();
    emit step_finished(++this->_currentSample);

    if(this->_currentSample == this->_samples){
        emit test_finished();
        QString res = this->_adb->getLogResult();
        emit test_results_available(res);
        this->_adb->clearDeviceLog();
        this->_currentSample = 0;
        this->_testing = false;
        emit testing_status_changed(_testing);

        try {
            saveTest(res);
        }  catch (runtime_error &e) {
            QString ex = e.what();
            emit error(ex);
        }

    }

}

void TestManager::handleLogResults(const QString &res)
{
    qDebug() << res;
    emit test_results_available(res);
}

void TestManager::saveTest(QString testData)
{
    qDebug() << "trying to save test to path " + _saveDir;
    if(_saveDir.isEmpty()){
        qDebug() << "path is empty";
        emit no_save_path(testData);
        return;
    }

    ofstream saveFile(_saveDir.toStdString());
    if(saveFile.is_open()){
        saveFile <<  testData.toStdString();
        saveFile.close();

    }
    else throw runtime_error("could not open file");

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



