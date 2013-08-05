#include <QtGui/QApplication>
#include <QTranslator>

#include "mainwindow.h"
#include "dbmanager.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QTranslator translator;
  if(translator.load(QString("qt_it"))) {
    a.installTranslator(&translator);
  }

  QFont currentFont=a.font();
  currentFont.setPointSize(10);
  a.setFont(currentFont);

  QMap<QString,QVariant>* configurazione=new QMap<QString,QVariant>;
  DBManager dbman(configurazione);

  MainWindow w(configurazione);
  w.show();

  return a.exec();

}


