#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QObject>
#include <QMessageBox>
#include <qfiledialog.h>
#include <QTcpSocket>
#include <sys/timeb.h>
#include <dirent.h>
#include "sensor.h"
#include "camera.h"
#include "yuv2rgb.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class monitorDialog;
}
QT_END_NAMESPACE

class monitorDialog : public QDialog
{
    Q_OBJECT

public:
    monitorDialog(QWidget *parent = 0);
    ~monitorDialog();

    char buffer[128];
    struct timeb startingTime, currentTime;
    unsigned char *RGBBuffer, *UVYBuffer;
    int ADCRefreshRateMs;
    int camRefreshRateMs;
    int width, height;
    bool connected;
    bool capturing;
    QImage *graphics;
    sensor *adcSensor;
    Camera *myCamera;
    QTcpSocket *tcpSocket;

    QString picturePath;
    QStringList pictureNameList;
    // connectionSettingsDialog connectionSettings;

private slots:
    void on_graphicRefreshButton_clicked();
    void on_connectButton_clicked();
    void on_ADCRefreshRateSetButton_clicked();
    void on_CamRefreshRateSetButton_clicked();
    // void on_refreshRateSetButton_clicked();

    void refreshResistance();
    void refreshGraphics();
    void startCapturing();
    void stopCapturing();
    void changeRefreshRate();
    void updateLog(char *buffer);

    void on_graphicsSourceSelect_currentTextChanged(const QString &arg1);

    void on_graphicsSourceSelect_currentIndexChanged(const QString &arg1);

    void on_pictureRefreshButton_clicked();

    void on_pictureSelect_currentIndexChanged(const QString &arg1);

private:
    Ui::monitorDialog *ui;
    QTimer resistanceRefreshTimer;
    QTimer camRefreshTimer;
};
#endif // MONITORDIALOG_H
