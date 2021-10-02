#include "adbmanager.h"


AdbManager::AdbManager(QString path):
    _absAdbPath(path),
    _selectedDevice(""),
    _foundDevIndex(0)
{

}

void AdbManager::setAbsPath(QString path) { this->_absAdbPath = path; }


QString AdbManager::getSelectedDevice(){
    return this->_selectedDevice;
}


QString AdbManager::getDeviceByIndex(int index){
    return this->_foundDevices[index];
}

 QString AdbManager::getLogResult()
{
    if(this->_absAdbPath.isEmpty()) throw logic_error("no adb tools found");
    if(this->_selectedDevice.isEmpty()) throw logic_error("no device found");

    char buffer[128];
    qDebug() << " getLogResult()";

    // https://developer.android.com/studio/command-line/logcat
    // the line below opens a stream of logs and waits for logCount logs, unreliable against user errors
    // where we might get less logs than expected.
    //QString cmd = adb_path + " -s " + device  +
    //        " logcat " + "-m " + QString::number(logCount) + " --regex=\"SHOT_TO_SHOT_O :\"";

    // dumps the entire log buffer filtering by the given tag
    // TODO: make a menu so the user can setup the preferred buffer size
    QString cmd = this->_absAdbPath + " -s " + this->_selectedDevice  + " logcat -d --regex=\"SHOT_TO_SHOT_O :\"";
    qDebug() << "issuing command " << cmd;
    QString res = "";


    try {

        // open pipe to file
        FILE *pipe = popen(cmd.toStdString().c_str(), "r");
        if(!pipe){
            //return "adb couldn't get devices";
        }

        // read till end of process:
        while (!feof(pipe)) {

            // use buffer to read and add to result
            if(fgets(buffer, 128, pipe) != NULL){
                res += buffer;
                //qDebug() << res;

            }

        }

        pclose(pipe);

    }  catch (...) {
        cout << "something happened";

    }

    return res;


}

void AdbManager::clickShutterBtn()
{
    if(this->_selectedDevice.isEmpty()) throw logic_error("no device selected");
    if(this->_absAdbPath == "") throw logic_error("no adb path provided");

    QString device = this->_selectedDevice;
    QString adb = this->_absAdbPath;
    QString cmd = adb + " -s " + device + " shell input keyevent 27";
    int res = system(cmd.toStdString().c_str());
    qDebug() << "issued command " << cmd << " with result " << res;
}


int AdbManager::getDevFoundIndex(){
    return this->_foundDevIndex;
}

QString AdbManager::getAbsPath()
{
    return this->_absAdbPath;
}

void AdbManager::clearDeviceList(){
    this->_foundDevices->clear();
    this->_foundDevIndex = 0;
    qDebug() << "Device list cleared";
}

void AdbManager::clearDeviceLog()
{
    QString cmd = this->_absAdbPath + " -s " + this->_selectedDevice + " logcat -c";
    int res = system(cmd.toStdString().c_str());
    qDebug() << "issued command " << cmd << " with result " << res;
}

void AdbManager::setBufferSize(int size)
{
    QString cmd = this->_absAdbPath + " -s " + this->_selectedDevice + " logcat -G " + QString::number(size) + "M";
    int res = system(cmd.toStdString().c_str());
    qDebug() << "issued command " << cmd << " with result " << res;
}

void AdbManager::addDevice(QString device) {

    if(this->_foundDevIndex == MAX_DEVICES) throw overflow_error("Can't connect more devices");
    this->_foundDevices[this->_foundDevIndex++] = device;
    qDebug() << "device " << device << " added";
}

bool AdbManager::hasFoundDevices(){
    return this->_foundDevIndex > 0;
}

void AdbManager::selectDevice(QString device){
    this->_selectedDevice = device;
    qDebug() << "Selected device " << device;
}

QString AdbManager::getConnectedDevices()
{
    //string *devices = new
    char buffer[128];
    string result = "";
    string command = this->_absAdbPath.toStdString() + " devices";
    QString res = "";

    try {

        // open pipe to file
        FILE *pipe = popen(command.c_str(), "r");
        if(!pipe){
            return "adb couldn't get devices";
        }

        // read till end of process:
        while (!feof(pipe)) {

            // use buffer to read and add to result
            if(fgets(buffer, 128, pipe) != NULL){
                res += buffer;

            }

        }

        pclose(pipe);

    }  catch (...) {
        cout << "something happened";

    }

   // Finding the names of the devices in the response message
    QRegularExpression re("(N[A-Z0-9]+)", QRegularExpression::MultilineOption);
    qDebug() << res;
    QRegularExpressionMatchIterator match = re.globalMatch(res);

    for(int i=0; i < res.length(); i++) qDebug() << res[i];

    while(match.hasNext()){
        QRegularExpressionMatch m = match.next();
        qDebug() << m.captured(1);

        if(this->_foundDevices->count() == MAX_DEVICES) throw overflow_error("device list full");
        this->addDevice(m.captured(1));
        emit foundDevice(m.captured(1));

    }

    if(this->_foundDevIndex > 0) this->selectDevice(_foundDevices[0]); // set the first found device to be selected by default

    //else qDebug() << "no matches";


    return res;

}
