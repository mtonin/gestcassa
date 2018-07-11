#include "Windows.h"
#include "vld/vld.h"

#include <QtSingleApplication>
#include <QTranslator>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QVariant>

#include "QsLogDest.h"

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

    QsLogging::Logger& logger=QsLogging::Logger::instance();
    QDir applDir(a.applicationDirPath());
    applDir.mkpath(applDir.path().append("/LOG"));
    const QString sLogPath(QDir(a.applicationDirPath().append("/LOG")).filePath("GestCassa.log"));

    QsLogging::DestinationPtrU fileDestination(QsLogging::DestinationFactory::MakeFileDestination(
      sLogPath, QsLogging::LogRotationOption::EnableLogRotation, QsLogging::MaxSizeBytes(1024*1024), QsLogging::MaxOldLogCount(10)));
    logger.addDestination(std::move(fileDestination));

    QLOG_INFO() << "Program started";
    QLOG_INFO() << "Inizializzazione";

    QPixmap splashPixmap(":/GestCassa/splash");
    QSplashScreen splash(splashPixmap);
    splash.show();
    splash.showMessage("Inizializzazione...");
    QCoreApplication::processEvents();

    QDesktopWidget* desktop = QApplication::desktop();
    if (desktop->size().width() < 1024 || desktop->size().height() < 768) {
        if (QMessageBox::No == QMessageBox::question(0,
                QObject::tr("Risoluzione minima"), "E' consigliabile usare una risoluzione minima di 1024x768. Continuare?", QMessageBox::Ok, QMessageBox::No))
            return 0;
    }

    QTranslator translator;
    if (translator.load(QString("qtbase_it"))) {
        QtSingleApplication::installTranslator(&translator);
    }

    /*
    int idFont=QFontDatabase::addApplicationFont(":/GestCassa/adventix");
    qDebug("idFont=%d",idFont);
    QFontDatabase::applicationFontFamilies(idFont)
    */

    QFont currentFont = QApplication::font();
    currentFont.setPointSize(10);
    //currentFont.setFamily("adventix");
    QApplication::setFont(currentFont);

    QLOG_INFO() << "Caricamento configurazione";
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
    QLOG_INFO() << "Collegamento al database";
    splash.showMessage("Collegamento al database...");
    QCoreApplication::processEvents();
    if (dbman.init(QCoreApplication::applicationDirPath())) {
       splash.showMessage("Caricamento articoli...");
       QCoreApplication::processEvents();

       QLOG_INFO() << "Creazione Main Window";
       MainWindow w(configurazione,splash);
       a.setActivationWindow(&w);
       w.show();
       splash.finish(&w);

    //a.setStartDragDistance(50);
    //a.setStartDragTime(1000);

       QLOG_INFO() << "Avvio message loop";
       QApplication::exec();
    }

    delete configurazione;

    //ExitWindowsEx(EWX_LOGOFF,0);
    QLOG_INFO() << "Fine applicazione";
#ifdef Q_OS_WIN
    logger.shutDownLoggerThread();
#endif

}
