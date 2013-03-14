#include <QtGui/QApplication>

#include "mainwindow.h"
#include "dbdialog.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QFont currentFont=a.font();
  currentFont.setPointSize(12);
  a.setFont(currentFont);

  DBDialog dlg;
  if(QDialog::Accepted==dlg.exec()) {
    MainWindow w;
    w.show();

    return a.exec();
  } else {
    return 0;
  }

}


