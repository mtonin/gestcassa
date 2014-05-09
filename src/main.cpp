#include "Windows.h"
#include "vld/vld.h"

#include <QtGui/QApplication>
#include <QTranslator>

#include "mainwindow.h"
#include "dbmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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

    MainWindow w(configurazione);
    w.show();

    //a.setStartDragDistance(50);
    //a.setStartDragTime(1000);

    a.exec();
    delete configurazione;

    //ExitWindowsEx(EWX_LOGOFF,0);

}
