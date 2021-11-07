#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <adbmanager.h>
#include <testmanager.h>
#include <QObject>
#include <QFileDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:

    void on_startTestBtn_clicked();

    void on_stopTestBtn_clicked();

    void on_test_finished();

    void on_test_step(int samples);

    void on_test_failed(QString why);

    void on_refreshBtn_clicked();

    void on_device_found(QString device);

    void on_browseFilesBtn_clicked();

    void on_test_results_available(const QString &res);

    void on_testing_status_changed(const bool isTesting);

    void warning_message(const QString &msg);

    void on_error(const QString &msg);

    void on_save_path_missing(const QString &testData);

    void on_action_setup_triggered(); // opens the setup menu

    void on_actionTerminal_text_triggered(); // opens a dialog for changing terminal text font

    void on_terminal_message(const QString &message); // prints a message on the terminal screen

private:
    Ui::MainWindow *ui;
    AdbManager *adb;
    TestManager *testMan;
    QTimer *timer;
    QFont terminalFont;
    int countdownAcc;

    void countdown();

    void printOnScreen(QString text);

    void updateUI(); // update test variables on screen

    int openFileDialog();

    void openSetupMenu();
};
#endif // MAINWINDOW_H
