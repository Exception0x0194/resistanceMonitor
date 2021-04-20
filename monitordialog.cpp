#include "monitordialog.h"
#include "ui_monitordialog.h"

monitorDialog::monitorDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::monitorDialog)
{
    ui->setupUi(this);
}

monitorDialog::~monitorDialog()
{
    delete ui;
}

