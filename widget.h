#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <adbmanager.h>
#include <testmanager.h>
#include <QObject>
#include <QFileDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{

    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_startTestBtn_clicked();

    void on_stopTestBtn_clicked();

    void on_test_finished(int test);

    void on_test_step(int tests, int samples);

    void on_test_failed(QString why);

    void on_refreshBtn_clicked();

    void on_device_found(QString device);

    void on_browseFilesBtn_clicked();

private:
    Ui::Widget *ui;
    AdbManager *adb;
    TestManager *testMan;
    QTimer *timer;
    int countdownAcc;

    void countdown();

    void printOnScreen(QString text);

};
#endif // WIDGET_H
