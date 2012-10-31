#include "ordine.h"
#include <QMessageBox>

Ordine::Ordine(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
}

void Ordine::on_pushButton_clicked()
{
  QMessageBox::information(this,"MSG","click()");
}
