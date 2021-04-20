#include "monitordialog.h"
#include "ui_monitordialog.h"

monitorDialog::monitorDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::monitorDialog)
{
    ui->setupUi(this);

    ui->graphicsSourceSelect->addItem("Resistance sensor");
    ui->graphicsSourceSelect->addItem("Camera");
    ui->graphicsSourceSelect->addItem("Picture");
    ui->graphicsSourceSelect->addItem("History log");

    ui->pictureSelect->setDisabled(true);

    ui->refreshRateEdit->setText("100");

    ui->logPane->document()->setMaximumBlockCount(300);

    adcSensor = new sensor();
    adcSensor->openADC();

    sprintf(graphicSource, "Resistance sensor");
    sprintf(pictureName, "");
    refreshingRateMs = 500;

    connect(&resistanceRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshResistance()));
    resistanceRefreshTimer.start(refreshingRateMs);
}

monitorDialog::~monitorDialog()
{
    delete ui;
}

void monitorDialog::on_graphicRefreshButton_clicked()
{
}

void monitorDialog::on_refreshRateSetButton_clicked()
{
    resistanceRefreshTimer.stop();
    int retv = 0;
    bool transSuccess = false;
    retv = ui->refreshRateEdit->text().toInt(&transSuccess);
    if ((!transSuccess) || (retv <= 0))
    {
        QMessageBox::information(this, tr("Error"), tr("Ivalid refreshing rate!"), QMessageBox::Ok);
        return;
    }
    refreshingRateMs = retv;
    resistanceRefreshTimer.start(refreshingRateMs);
    printf("Set new ADC refreshing rate: %d\n", refreshingRateMs);
}

void monitorDialog::on_connectionSettingsButton_clicked()
{
}

void monitorDialog::changeRefreshRate()
{
}

void monitorDialog::refreshResistance()
{
    int r = adcSensor->getResistance();
    if (r == -1)
    {
        return;
    }
    sprintf(buffer, "Resistance = %d", r);
    updateLog(buffer);
}

void monitorDialog::updateLog(char *buffer)
{
    ui->logPane->append(buffer);
}