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

    ui->ADCRefreshRateEdit->setText("100");
    ui->CamRefreshRateEdit->setText("500");

    ui->logPane->document()->setMaximumBlockCount(300);

    adcSensor = new sensor();
    adcSensor->openADC();

    sprintf(graphicSource, "Resistance sensor");
    sprintf(pictureName, ".");
    refreshingRateMs = 500;

    connect(&resistanceRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshResistance()));
    resistanceRefreshTimer.start(refreshingRateMs);

    tcpSocket = new QTcpSocket();
    connected = false;
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
    if (r > 7000)
    {
        sprintf(buffer, "%s, warning: too HIGH.", buffer);
    }
    if (r < 6000)
    {
        sprintf(buffer, "%s, warning: too LOW.", buffer);
    }
    if (connected)
    {
        tcpSocket->write(buffer);
    }
    updateLog(buffer);
}

void monitorDialog::updateLog(char *buffer)
{
    ui->logPane->append(buffer);
}

void monitorDialog::on_connectButton_clicked()
{
    if (!connected)
    {
        QString IPinput = ui->serverIPEdit->text();
        int port = ui->serverPortEdit->text().toInt();
        printf("Connect button clicked, trying to connect to server: %s:%d\n", IPinput.toLatin1().data(), port);
        tcpSocket->connectToHost(IPinput, port);
        connected = tcpSocket->waitForConnected(3000);
        if (!connected)
        {
            QMessageBox::information(this, tr("Error"), tr("Cannot connect to server after 3 secs!"), QMessageBox::Ok);
            return;
        }
        else
        {
            printf("Successfully connected to server.\n");
        }
        ui->connectButton->setText("Disconnect");
        ui->serverIPEdit->setDisabled(true);
        ui->serverPortEdit->setDisabled(true);
    }
    else
    {
        tcpSocket->disconnectFromHost();
        tcpSocket->waitForDisconnected(3000);
        connected = false;
        ui->connectButton->setText("Connect");
        ui->serverIPEdit->setEnabled(true);
        ui->serverPortEdit->setEnabled(true);
    }
}

void monitorDialog::on_ADCRefreshRateSetButton_clicked()
{
    resistanceRefreshTimer.stop();
    int retv = 0;
    bool transSuccess = false;
    retv = ui->ADCRefreshRateEdit->text().toInt(&transSuccess);
    if (retv == refreshingRateMs)
    {
        retv = random() % 4900 + 100;
        sprintf(buffer, "%d", retv);
        ui->ADCRefreshRateEdit->setText(buffer);
    }
    if ((!transSuccess) || (retv < 100) || (retv > 5000))
    {
        QMessageBox::information(this, tr("Error"), tr("Ivalid refreshing rate!"), QMessageBox::Ok);
        return;
    }
    refreshingRateMs = retv;
    resistanceRefreshTimer.start(refreshingRateMs);
    printf("Set new ADC refreshing rate: %d\n", refreshingRateMs);
}

void monitorDialog::on_CamRefreshRateSetButton_clicked()
{
}
