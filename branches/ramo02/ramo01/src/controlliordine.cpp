#include "controlliordine.h"
#include "ui_controlliordine.h"
#include <QPropertyAnimation>

ControlliOrdine::ControlliOrdine(QWidget *parent) :
  idArticolo(0),
  QWidget(parent),
  ui(new Ui::ControlliOrdine)
{
  ui->setupUi(this);
  setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
  effetto=new QPropertyAnimation(this,"windowOpacity");
  effetto->setStartValue("1");
  effetto->setEndValue("0");
  effetto->setDuration(3000);
  effetto->setEasingCurve(QEasingCurve::InCubic);

}

ControlliOrdine::~ControlliOrdine()
{
  delete ui;
  delete effetto;
}

void ControlliOrdine::show()
{
  QWidget::show();
  effetto->stop();
  effetto->start();
}

void ControlliOrdine::on_toolButton_clicked()
{
  emit incrementa(idArticolo);
  effetto->stop();
  effetto->start();
}

void ControlliOrdine::on_toolButton_2_clicked()
{
  emit decrementa(idArticolo,index);
  effetto->stop();
  effetto->start();
}
