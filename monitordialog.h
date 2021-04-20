#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class monitorDialog; }
QT_END_NAMESPACE

class monitorDialog : public QDialog
{
    Q_OBJECT

public:
    monitorDialog(QWidget *parent = 0);
    ~monitorDialog();

private:
    Ui::monitorDialog *ui;
};
#endif // MONITORDIALOG_H
