#include "prefsdialog.h"
#include "ui_prefsdialog.h"
#include "QSettings"
#include "QFileDialog"
#include "QMessageBox"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
    loadSettings();
}

PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::saveSettings()
{

    int samples = ui->samplesAmount->text().toInt();
    double interval = ui->intervalTime->text().toDouble();
    double bufferIncrease = ui->bufferExpansion->text().toDouble() ;

    QSettings settings("IPE", "KPIHelper");
    settings.beginGroup("settings");
    settings.setValue("samples", samples);
    settings.setValue("interval", interval);
    settings.setValue("bufferIncrease", bufferIncrease);
    settings.setValue("adbPath", adbPath);
    settings.endGroup();



}

void PrefsDialog::loadSettings()
{
    QSettings settings("IPE", "KPIHelper");
    settings.beginGroup("settings");
    int samples = settings.value("samples", QVariant(0)).toInt();
    double interval = settings.value("interval", QVariant(0)).toDouble();
    double bufferIncrease =  settings.value("bufferIncrease", QVariant(0)).toDouble();
    adbPath = settings.value("adbPath", QVariant("")).toString();
    settings.endGroup();

    ui->samplesAmount->setValue(samples);
    ui->intervalTime->setValue(interval);
    ui->bufferExpansion->setValue(bufferIncrease);
    ui->adbPathLineEdit->setText(adbPath);

}

void PrefsDialog::on_browseBtn_clicked()
{
    // open the file explorer
    QString filename = QFileDialog::getOpenFileName(this, tr("Open FIle", "/home"));

    if(!filename.isNull()){
        adbPath = filename;
        ui->adbPathLineEdit->setText(adbPath);

    }
}


void PrefsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton stdButton = ui->buttonBox->standardButton(button);

    if(stdButton == QDialogButtonBox::Apply){
        int ret = QMessageBox::question(this, "Save configuration", "Are you sure you want to save the new settings?",
                                        QMessageBox::Ok | QMessageBox::Cancel);

        if(ret == QMessageBox::Ok){
            saveSettings();
        }
    }

    if(stdButton == QDialogButtonBox::Cancel) reject();


    if(stdButton == QDialogButtonBox::Ok){
        int ret = QMessageBox::question(this, "Save configuration", "Are you sure you want to save the new settings?",
                                        QMessageBox::Ok | QMessageBox::Cancel);

        if(ret == QMessageBox::Ok){
            saveSettings();
            accept();
        } else reject();
    }
}

