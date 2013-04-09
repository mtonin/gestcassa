#include <QtGui/QApplication>

#include "mainwindow.h"
#include "dbdialog.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QFont currentFont=a.font();
  currentFont.setPointSize(10);
  a.setFont(currentFont);

  QMap<QString,QVariant>* configurazione=new QMap<QString,QVariant>;
  DBDialog dlg(configurazione);
  if(QDialog::Accepted==dlg.exec()) {
    MainWindow w(configurazione);
    w.show();

    return a.exec();
  } else {
    return 0;
  }

}


