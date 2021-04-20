#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QObject>
#include <QMessageBox>
#include "sensor.h"

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

    char graphicSource[128];
    char pictureName[128];
    char buffer[128];
    int refreshingRateMs;
    sensor *adcSensor;

private slots:
    void on_graphicRefreshButton_clicked();
    void on_refreshRateSetButton_clicked();
    void on_connectionSettingsButton_clicked();

    void refreshResistance();
    void changeRefreshRate();
    void updateLog(char *buffer);

private:
    Ui::monitorDialog *ui;
    QTimer resistanceRefreshTimer;
};
#endif // MONITORDIALOG_H
