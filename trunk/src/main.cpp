#include "Windows.h"
#include "vld/vld.h"

#include <QtSingleApplication>
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
    // Permette ad altre applicazioni di tornare in primo piano
    AllowSetForegroundWindow(ASFW_ANY);

    QtSingleApplication a(argc, argv);
    if(a.sendMessage("activate")) {
      return 0;
    }


    QPixmap splashPixmap(":/GestCassa/splash");
    QSplashScreen splash(splashPixmap);
    splash.show();
    splash.showMessage("Inizializzazione...");
    a.processEvents();

    QDesktopWidget* desktop = QApplication::desktop();
    if (desktop->size().width() < 1024 || desktop->size().height() < 768) {
        if (QMessageBox::No == QMessageBox::question(0,
                QObject::tr("Risoluzione minima"), "E' consigliabile usare una risoluzione minima di 1024x768. Continuare?", QMessageBox::Ok, QMessageBox::No))
            return 0;
    }

    QTranslator translator;
    if (translator.load(QString("qt_it"))) {
        a.installTranslator(&translator);
    }

    /*
    int idFont=QFontDatabase::addApplicationFont(":/GestCassa/adventix");
    qDebug("idFont=%d",idFont);
    QFontDatabase::applicationFontFamilies(idFont)
    */

    QFont currentFont = a.font();
    currentFont.setPointSize(10);
    //currentFont.setFamily("adventix");
    a.setFont(currentFont);

    QMap<QString, QVariant>* configurazione = new QMap<QString, QVariant>;
    DBManager dbman(configurazione);

    /*
    QFileInfo dbFileInfo(QString("%1/%2")
                       .arg(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
                       .arg(dbFileName));
    QDir dbParentDir(dbFileInfo.absolutePath());
    dbParentDir.mkpath(dbFileInfo.absolutePath());
    QFileInfo dbFileModello(QString("%1/gcas.fdb").arg(a.applicationDirPath()));

    QString nomeFile;
    nomeFile = dbFileInfo.absoluteFilePath();
    /*
    nomeFile=QFileDialog::getOpenFileName(0,"Scegliere il database");
    */
    splash.showMessage("Collegamento al database...");
    a.processEvents();
    if (dbman.init(a.applicationDirPath())) {
       splash.showMessage("Caricamento articoli...");
       a.processEvents();

       MainWindow w(configurazione,splash);
       a.setActivationWindow(&w);
       w.show();
       splash.finish(&w);

    //a.setStartDragDistance(50);
    //a.setStartDragTime(1000);

       a.exec();
}

    delete configurazione;

    //ExitWindowsEx(EWX_LOGOFF,0);

}
