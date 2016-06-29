#include "backgroundcontroller.h"

#include <QtSql>
#include <QTimer>

BackgroundController::BackgroundController()
{

}

void BackgroundController::run()
{
    stato=Reset;
    while(true) {
        controllaDatabase();
        QThread::sleep(2);
    }
}

void BackgroundController::controllaDatabase(){
    enum statoDatabase statoCorrente=stato;
    QString sql;
    QString testo;
    QSqlDatabase db=QSqlDatabase::database(QSqlDatabase::defaultConnection,false);
    if(Disconnesso==stato) {
        if(!db.open()) {
            statoCorrente=Disconnesso;
            testo=db.lastError().text();
            qDebug("DB Open Error=%d,text=%s",db.lastError().number(),testo.toStdString().c_str());
        }
    }
    if(db.isOpen()){
        QSqlQuery stmt(db);
        QDateTime ultimoAggiornamentoDB;
        sql="select valore from configurazione where chiave='ultimoaggiornamento'";
        if (!stmt.exec(sql)) {
            statoCorrente=Disconnesso;
            //db.close();
            testo=stmt.lastError().text();
            qDebug("DBError=%d,text=%s",stmt.lastError().number(),testo.toStdString().c_str());
        } else {
            if (stmt.next()) {
                ultimoAggiornamentoDB=stmt.value(0).toDateTime();
            }
            if(ultimoAggiornamentoDB > ultimaLetturaDB) {
                statoCorrente=DaAggiornare;
                testo="Ricaricare l'archivio";
            } else {
                statoCorrente=Connesso;
                testo=QString("Database: %1/%2, stato: OK").arg(db.hostName()).arg(db.databaseName());
            }
        }
    }

    if(stato!=statoCorrente) {
        stato=statoCorrente;
        emit messaggioDB(stato,testo);
    }
}

