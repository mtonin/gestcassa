#include "Windows.h"
#include "vld/vld.h"

#include <QtGui/QApplication>
#include <QTranslator>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QVariant>

#include "mainwindow.h"
#include "dbmanager.h"
#include "commons.h"

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

  QFileInfo dbFileInfo(QString("%1/%2")
             .arg(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
             .arg(dbFileName));
  QDir dbParentDir(dbFileInfo.absolutePath());
  dbParentDir.mkpath(dbFileInfo.absolutePath());
  if(dbman.init(dbFileInfo.absoluteFilePath())) {

    QString nomeFile;
    nomeFile=dbFileInfo.absoluteFilePath();
    /*
    nomeFile=QFileDialog::getOpenFileName(0,"Scegliere il database");
    */
    if(!nomeFile.isEmpty()) {
        if(dbman.init(nomeFile)) {
          MainWindow w(configurazione);
          w.show();

          //a.setStartDragDistance(50);
          //a.setStartDragTime(1000);

          a.exec();
        }
    }
  }

  delete configurazione;

  //ExitWindowsEx(EWX_LOGOFF,0);

}
