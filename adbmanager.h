#ifndef ADBMANAGER_H
#define ADBMANAGER_H

#define MAX_DEVICES 5

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <QDebug>
#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QObject>

using namespace std;

/* Author: Edson Silva
 * class meant for handling adb commands and results */

class AdbManager : public QObject
{
    Q_OBJECT

private:
    QString _absAdbPath;
    QString _foundDevices[MAX_DEVICES];
    QString _selectedDevice;
    int _foundDevIndex;

    void addDevice(QString device);


public:

    //AdbManager();

    AdbManager(QString path="");

    //~AdbManager();

    QString getConnectedDevices(); // returns a list of connected devices

    void setAbsPath(QString path);

    void selectDevice(QString device);

    void clearDeviceList();

    bool hasFoundDevices();

    int getDevFoundIndex();

    QString getSelectedDevice();

    QString getDeviceByIndex(int index);


signals:
    void foundDevice(QString device);

public slots:
    void clickShutterBtn();

    void getLogResult();

};

#endif // ADBMANAGER_H
