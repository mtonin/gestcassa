#include "arealavoroarticoli.h"
#include <QPainter>

AreaLavoroArticoli::AreaLavoroArticoli(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  QPainter painter(this);

  painter.drawRect(10,20,200,300);

}
