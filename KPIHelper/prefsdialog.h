#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class PrefsDialog;
}

class PrefsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrefsDialog(QWidget *parent = nullptr);
    ~PrefsDialog();

private slots:
    void on_browseBtn_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::PrefsDialog *ui;
    QString adbPath;

    void saveSettings();
    void loadSettings();

};

#endif // PREFSDIALOG_H
