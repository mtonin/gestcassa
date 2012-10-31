#include <QtGui/QApplication>
#include <QtSql>
#include <QStyle>
#include <QMessageBox>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QApplication::setStyle("windowsvista");
  QStyle* stile=QApplication::style();

  MainWindow w;
  w.show();
  
  return a.exec();
}
