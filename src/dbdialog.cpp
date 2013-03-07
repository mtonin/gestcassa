#include "dbdialog.h"

#include <QFileDialog>

DBDialog::DBDialog(QWidget *parent) :
   QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
  dbfile->setFocus();
}

void DBDialog::on_toolButton_clicked()
{
  QString dbFileName=QFileDialog::getOpenFileName(0,"","c:\\temp","*.db3");
   if(!dbFileName.isEmpty()) {
       dbfile->setText(dbFileName);
     }
}
