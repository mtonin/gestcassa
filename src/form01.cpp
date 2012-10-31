#include "form01.h"
#include "ui_form01.h"

Form01::Form01(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Form01)
{
  ui->setupUi(this);
}

Form01::~Form01()
{
  delete ui;
}

