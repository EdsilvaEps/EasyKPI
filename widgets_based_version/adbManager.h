#ifndef ADBMANAGER_H
#define ADBMANAGER_H

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

using namespace std;

/* Author: Edson Silva
 * class meant for handling adb commands and results */
class AdbManager{

private:
    string absAdbPath;
    string selectedDevice;
    int connectedDevices;

public:

    AdbManager(string absPath=""): absAdbPath(""), selectedDevice(""), connectedDevices(0) {
        absAdbPath = absPath;
    }

    string* getConnectedDevices(); // returns a list of connected devices

    void selectDevice();

    void clickShutterBtn();

    string getLogResult();


};

#endif // ADBMANAGER_H
