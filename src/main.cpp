#include "Windows.h"
#include "vld/vld.h"

#include <QtSingleApplication>
#include <QTranslator>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QVariant>
#include <QCommandLineParser>

#include "commons.h"
#include "mainwindow.h"
#include "dbmanager.h"
#include "RollingFileAppender.h"
#include "ConsoleAppender.h"

int main(int argc, char *argv[])
{
    // Permette ad altre applicazioni di tornare in primo piano
    AllowSetForegroundWindow(ASFW_ANY);

    QtSingleApplication a(argc, argv);
    if(a.sendMessage("activate")) {
      return 0;
    }

    QCommandLineParser argsParser;
    argsParser.setApplicationDescription("Gestione Cassa");
    argsParser.addHelpOption();
    argsParser.addVersionOption();
    QCommandLineOption noLogOpt("no-log","Disabilita il registro dell'applicazione");
    argsParser.addOption(noLogOpt);
    QCommandLineOption logFileNameOpt("logFilename","Filename del registro dell'applicazione","logFilename","GestCassa.log");
    argsParser.addOption(logFileNameOpt);
    QCommandLineOption logFolderOpt("logFolder","Cartella del registro dell'applicazione","logFolder",a.applicationDirPath().append("/LOG"));
    argsParser.addOption(logFolderOpt);
    QCommandLineOption logLevelOpt("logLevel","Livello di dettaglio del registro dell'applicazione","logLevel","INFO");
    argsParser.addOption(logLevelOpt);
    argsParser.process(a);

    if(!argsParser.isSet("NoLog")) {
        QString logLevel=argsParser.value("logLevel").toUpper();
        if("TRACE" != logLevel &&
                "DEBUG" != logLevel &&
                "INFO" != logLevel &&
                "WARNING" != logLevel &&
                "ERROR" != logLevel &&
                "FATAL" != logLevel) {
            logLevel="INFO";
        }
        QDir logDir(argsParser.value("logFolder"));
        logDir.mkpath(".");
        const QString logPath(logDir.filePath(argsParser.value("logFilename")));
        RollingFileAppender* logfileAppender=new RollingFileAppender(logPath);
        logfileAppender->setDetailsLevel(logLevel);
        logfileAppender->setLogFilesLimit(10);
        logfileAppender->setDatePattern("'-'yyyy-MM-dd'.log'");
        cuteLogger->registerAppender(logfileAppender);
    }

#ifdef _DEBUG
    ConsoleAppender* consoleAppender=new ConsoleAppender();
    consoleAppender->setDetailsLevel("TRACE");
    cuteLogger->registerAppender(consoleAppender);
#endif

    LOG_INFO() << "Avvio applicazione";
    LOG_INFO() << "Inizializzazione";

    QPixmap splashPixmap(":/GestCassa/splash");
    QSplashScreen splash(splashPixmap);
    splash.show();
    splash.showMessage("Inizializzazione...");
    QCoreApplication::processEvents();

    QDesktopWidget* desktop = QApplication::desktop();
    if (desktop->size().width() < 1024 || desktop->size().height() < 768) {
        if (QMessageBox::No == QMessageBox::question(0,
                QObject::tr("Risoluzione minima"), "E' consigliabile usare una risoluzione minima di 1024x768. Continuare?", QMessageBox::Ok, QMessageBox::No))
            LOG_INFO() << "Uscita dall'applicazione per risoluzione video minore del minimo";
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

    LOG_INFO() << "Caricamento configurazione";
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
    LOG_INFO() << "Collegamento al database";
    splash.showMessage("Collegamento al database...");
    QCoreApplication::processEvents();
    if (dbman.init(QCoreApplication::applicationDirPath())) {
       splash.showMessage("Caricamento articoli...");
       QCoreApplication::processEvents();

       LOG_INFO() << "Creazione Main Window";
       MainWindow w(configurazione,splash);
       a.setActivationWindow(&w);
       w.show();
       splash.finish(&w);

    //a.setStartDragDistance(50);
    //a.setStartDragTime(1000);

       LOG_INFO() << "Avvio message loop";
       QApplication::exec();
    }

    delete configurazione;

    //ExitWindowsEx(EWX_LOGOFF,0);
    LOG_INFO() << "Uscita applicazione";

}
