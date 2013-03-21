#include "controlliordine.h"
#include "ui_controlliordine.h"
#include <QTimer>

ControlliOrdine::ControlliOrdine(QWidget *parent) :
  idArticolo(0),
  QWidget(parent),
  ui(new Ui::ControlliOrdine)
{
  ui->setupUi(this);
  setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
}

ControlliOrdine::~ControlliOrdine()
{
  delete ui;
}

void ControlliOrdine::show()
{
  connect(&timer,SIGNAL(timeout()),this,SLOT(onTimerTimeout()));
  timer.start(3000);
  QWidget::show();
}

void ControlliOrdine::on_toolButton_clicked()
{
  timer.start();
  emit incrementa(idArticolo);
}

void ControlliOrdine::on_toolButton_2_clicked()
{
  timer.start();
  emit decrementa(idArticolo);
}

void ControlliOrdine::onTimerTimeout()
{
  timer.stop();
  disconnect(&timer,0,0,0);
  close();
}
