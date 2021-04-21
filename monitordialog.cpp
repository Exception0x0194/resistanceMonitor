#include "monitordialog.h"
#include "ui_monitordialog.h"

monitorDialog::monitorDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::monitorDialog)
{
    ui->setupUi(this);

    ui->graphicsSourceSelect->addItem("ADC");
    ui->graphicsSourceSelect->addItem("Camera");
    ui->graphicsSourceSelect->addItem("Picture");

    ui->pictureSelect->setDisabled(true);

    ui->ADCRefreshRateEdit->setText("100");
    ui->CamRefreshRateEdit->setText("500");

    ui->logPane->document()->setMaximumBlockCount(300);

    adcSensor = new sensor();
    adcSensor->openADC();
    capturing = false;
    width = 640;
    height = 480;
    myCamera = new Camera("/dev/video0", width, height, 0);
    RGBBuffer = new unsigned char[width * height * 3];
    UVYBuffer = new unsigned char[width * height * 2];
    graphics = new QImage(RGBBuffer, width, height, QImage::Format_RGB888);
    InitTable();

    ADCRefreshRateMs = 500;
    camRefreshRateMs = 500;

    connect(&resistanceRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshResistance()));
    resistanceRefreshTimer.start(ADCRefreshRateMs);

    connect(&camRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshGraphics()));
    //camRefreshTimer.start(camRefreshRateMs);

    ftime(&startingTime);
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

void monitorDialog::changeRefreshRate()
{
}

void monitorDialog::refreshResistance()
{
    int r = adcSensor->getResistance();
    if (r == -1)
        return;
    sprintf(buffer, "Resistance = %d", r);
    if (r > 7000)
        sprintf(buffer, "%s, warning: too HIGH.", buffer);
    if (r < 6000)
        sprintf(buffer, "%s, warning: too LOW.", buffer);
    updateLog(buffer);

    ftime(&currentTime);
    sprintf(buffer, "{\"RESISTANCE\":%d,\"TIMESTAMP\":%d}", r, -(startingTime.time * 1000 + startingTime.millitm - currentTime.time * 1000 - currentTime.millitm) / 100);
    if (connected)
        tcpSocket->write(buffer);
    //printf("%s\n", buffer);
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
        connected = false;
        ui->connectButton->setText("Connect");
        ui->serverIPEdit->setEnabled(true);
        ui->serverPortEdit->setEnabled(true);
    }
}

void monitorDialog::on_ADCRefreshRateSetButton_clicked()
{
    int retv = 0;
    bool transSuccess = false;
    retv = ui->ADCRefreshRateEdit->text().toInt(&transSuccess);
    if (retv == ADCRefreshRateMs)
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
    resistanceRefreshTimer.stop();
    ADCRefreshRateMs = retv;
    resistanceRefreshTimer.start(ADCRefreshRateMs);
    printf("Set new ADC refreshing rate: %d\n", ADCRefreshRateMs);
}

void monitorDialog::on_CamRefreshRateSetButton_clicked()
{
    int retv = 0;
    bool transSuccess = false;
    retv = ui->CamRefreshRateEdit->text().toInt(&transSuccess);
    if (retv == camRefreshRateMs)
    {
        retv = random() % 4500 + 500;
        sprintf(buffer, "%d", retv);
        ui->CamRefreshRateEdit->setText(buffer);
    }
    if ((!transSuccess) || (retv < 500) || (retv > 5000))
    {
        QMessageBox::information(this, tr("Error"), tr("Ivalid refreshing rate!"), QMessageBox::Ok);
        return;
    }
    camRefreshTimer.stop();
    camRefreshRateMs = retv;
    printf("Set new camera refreshing rate: %d\n", camRefreshRateMs);
    if (!capturing)
    {
        startCapturing();
    }
    camRefreshTimer.start(camRefreshRateMs);
}

void monitorDialog::startCapturing()
{
    if (capturing)
        return;
    if (myCamera->OpenDevice())
    {
        printf("Camera successfully connected.\n");
        capturing = true;
    }
    else
    {
        printf("Camera connection failed.\n");
        capturing = false;
    }
}

void monitorDialog::stopCapturing()
{
    if (!capturing)
        return;
    capturing = false;
    myCamera->CloseDevice();
    QPixmap p;
    ui->graphicsPane->setPixmap(p);
    camRefreshTimer.stop();
    printf("Camera closed.\n");
}

void monitorDialog::refreshGraphics()
{
    if (!capturing)
        return;
    if (!myCamera->GetBuffer(UVYBuffer))
    {
        printf("Error occurred when getting UVY buffer.\n");
        return;
    }
    myCamera->process_image(UVYBuffer, RGBBuffer);
    if (graphics != NULL)
        delete graphics;
    graphics = NULL;
    graphics = new QImage(RGBBuffer, width, height, QImage::Format_RGB888);
    *graphics = graphics->scaled(250, 130);
    ui->graphicsPane->setPixmap(QPixmap::fromImage(*graphics));
}

void monitorDialog::on_graphicsSourceSelect_currentTextChanged(const QString &arg1)
{
}

void monitorDialog::on_graphicsSourceSelect_currentIndexChanged(const QString &arg1)
{
    printf("Graphic source changed.\n");
    ui->graphicsPane->clear();
    if (!arg1.compare("ADC"))
    {
        stopCapturing();
        ui->grapicSourceLabel->setText("Graphics:");
        ui->CamRefreshRateSetButton->setDisabled(true);
        ui->pictureSelect->setDisabled(true);
        ui->pictureRefreshButton->setDisabled(true);
    }
    if (!arg1.compare("Camera"))
    {
        ui->grapicSourceLabel->setText("Cam:");
        ui->CamRefreshRateSetButton->setEnabled((true));
        ui->pictureSelect->setDisabled(true);
        ui->pictureRefreshButton->setDisabled(true);
    }
    if (!arg1.compare("Picture"))
    {
        stopCapturing();
        ui->grapicSourceLabel->setText("");
        ui->CamRefreshRateSetButton->setDisabled(true);
        ui->pictureSelect->setEnabled(true);
        ui->pictureRefreshButton->setEnabled(true);
    }
}

void monitorDialog::on_pictureRefreshButton_clicked()
{
    printf("Open picture folder.\n");
    picturePath = QFileDialog::getOpenFileName(this,
                                               tr("Open folder"),
                                               "/exp",
                                               tr("Images (*.png *.jpg)"));
    QPixmap qpmap = QPixmap(picturePath);
    qpmap = qpmap.scaled(250, 130);
    int index = picturePath.lastIndexOf("/");
    picturePath = picturePath.mid(0, index);
    printf("Folder selected: %s\n", picturePath.toLatin1().data());
    QDir dir(picturePath);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QString("*.png;*.jpg").split(";"));
    pictureNameList = dir.entryList();
    ui->pictureSelect->clear();
    for (int i = 0; i < pictureNameList.size(); i++)
    {
        ui->pictureSelect->addItem(pictureNameList[i]);
    }
    ui->graphicsPane->setPixmap(qpmap);
}

void monitorDialog::on_pictureSelect_currentIndexChanged(const QString &arg1)
{
    QString filePath = picturePath + "/" + arg1;
    printf("Open picture file %s.\n", filePath.toLatin1().data());
    QPixmap qpmap = QPixmap(filePath);
    qpmap = qpmap.scaled(250, 130);
    ui->graphicsPane->setPixmap(qpmap);
}
