#include "controlliordine.h"
#include "ui_controlliordine.h"
#include <QTimer>

ControlliOrdine::ControlliOrdine(const int id,QWidget *parent) :
  idArticolo(id),
  QWidget(parent),
  ui(new Ui::ControlliOrdine)
{
  ui->setupUi(this);
  setWindowFlags(Qt::FramelessWindowHint);
}

ControlliOrdine::~ControlliOrdine()
{
  delete ui;
}

void ControlliOrdine::show()
{
  timer.singleShot(3000,this,SLOT(close()));
  QWidget::show();
}

void ControlliOrdine::on_toolButton_clicked()
{
    emit incrementa(idArticolo);
}

void ControlliOrdine::on_toolButton_2_clicked()
{
    emit decrementa(idArticolo);
}
