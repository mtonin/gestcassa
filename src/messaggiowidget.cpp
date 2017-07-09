#include "messaggiowidget.h"
#include "ui_messaggiowidget.h"

MessaggioWidget::MessaggioWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessaggioWidget)
{
    ui->setupUi(this);
}

MessaggioWidget::~MessaggioWidget()
{
    delete ui;
}
