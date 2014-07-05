#include "commons.h"
#include "dbmanager.h"
#include "confermadlg.h"

#include <QFile>
#include <QtSql>
#include <QMessageBox>
#include <QTcpSocket>

DBManager::DBManager(QObject *parent) : QObject(parent)
{
}

DBManager::DBManager(QMap<QString, QVariant> *configurazione): conf(configurazione)
{

}

bool DBManager::init(const QString nomeFile, const QString modello)
{

    dbFilePath = nomeFile;
    if (!createConnection(dbFilePath, "gcas", "gcas-pwd", modello)) {
        return false;
    }

    return leggeConfigurazione();

}

bool DBManager::leggeConfigurazione()
{

    QSqlQuery stmt("select chiave,valore from configurazione");
    if (!stmt.isActive()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return false;
    }
    while (stmt.next()) {
        QString key = stmt.value(0).toString();
        QVariant valore = stmt.value(1).toString();
        conf->insert(key, valore);
    }

    if (!stmt.exec("select max(idsessione) from sessione")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return false;
    }
    int sessioneCorrente;
    while (stmt.next()) {
        sessioneCorrente = stmt.value(0).toInt();
    }
    if (0 == sessioneCorrente) {
        sessioneCorrente++;
        if (!stmt.exec("insert into sessione (idsessione,tsinizio) values ('1',current_timestamp)")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            return false;
        }
    }

    conf->insert("sessioneCorrente", sessioneCorrente);
    conf->insert("dbFilePath", dbFilePath);

    int versioneDB = conf->value("versione", 1).toInt();
    int nuovaVersioneDB = versioneDB;

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();
    if (versioneDB < 2) {
        if (!stmt.exec("alter table destinazionistampa add column stampaflag   BOOLEAN NOT NULL DEFAULT ( 1 ) ")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }
        nuovaVersioneDB = 2;
    }
    if (versioneDB < 3) {
        if (!stmt.exec("alter table ordinirighe rename to ordinirighe_old") ||
            !stmt.exec("CREATE TABLE ordinirighe (numeroordine INTEGER REFERENCES ordini ( numero ), \
                  idarticolo   INTEGER,   \
                  quantita     INTEGER );") ||
            !stmt.exec("insert into ordinirighe select numeroordine,idarticolo,quantita from ordinirighe_old") ||
            !stmt.exec("drop table ordinirighe_old")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }
        nuovaVersioneDB = 3;
    }

    if (versioneDB < 4) {
        if (!stmt.exec("alter table reparti add column abilitato   BOOLEAN NOT NULL DEFAULT ( 1 ) ")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }
        nuovaVersioneDB = 4;
    }

    if (versioneDB < 5) {
        if (!stmt.exec("alter table ordinicontenuto rename to storicoordini")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }
        nuovaVersioneDB = 5;
    }

    if (versioneDB < 6) {
        if (!stmt.exec("alter table destinazionistampa add column stampanumeroritiroflag BOOLEAN NOT NULL DEFAULT ( 0 ) ")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }
        nuovaVersioneDB = 6;
    }

    if (versioneDB < 7) {
        if (!stmt.exec("CREATE TABLE storicoordinitot (  \
                  idsessione   INTEGER, \
                  numeroordine INTEGER, \
                  tsstampa     DATETIME, \
                  importo      REAL, \
                  flagstorno   BOOLEAN NOT NULL DEFAULT (0))")   ||
            !stmt.exec("CREATE TABLE storicoordinidett (  \
                      idsessione   INTEGER, \
                      numeroordine INTEGER, \
                      descrizione  VARCHAR, \
                      quantita     INTEGER, \
                      destinazione VARCHAR, \
                      prezzo       REAL, \
                      tipoArticolo CHAR     NOT NULL  )")  ||
            !stmt.exec("alter table storicoordini rename to storicoordini_old")  ||
            !stmt.exec("CREATE VIEW storicoordini AS \
                  SELECT tot.idsessione, tot.numeroordine, tot.tsstampa, tot.importo, dett.descrizione, dett.prezzo, dett.quantita, dett.destinazione, dett.tipoArticolo \
                  FROM storicoordinitot tot, storicoordinidett dett \
                  WHERE tot.idsessione = dett.idsessione \
                  AND tot.numeroordine = dett.numeroordine \
                  AND tot.flagstorno = 0")  ||
            !stmt.exec("insert into storicoordinitot (idsessione,numeroordine,tsstampa,importo) \
                  select a.idsessione,a.numeroordine,a.tsstampa,a.importo \
                  from storicoordini_old a \
                  group by a.idsessione,a.numeroordine,a.tsstampa,a.importo")  ||
            !stmt.exec("insert into storicoordinidett (idsessione,numeroordine,descrizione,quantita,destinazione,prezzo,tipoarticolo) \
                  select a.idsessione,a.numeroordine,a.descrizione,a.quantita,a.destinazione,a.prezzo,a.tipoArticolo \
                  from storicoordini_old a") ||
            !stmt.exec("drop table storicoordini_old")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }
        nuovaVersioneDB = 7;
    }

    if (versioneDB < 8) {
        if (!stmt.exec("DROP VIEW storicoordini") ||
            !stmt.exec("ALTER TABLE storicoordinitot rename to storicoordinitot_old") ||
            !stmt.exec("ALTER TABLE storicoordinidett rename to storicoordinidett_old") ||
            !stmt.exec("CREATE TABLE storicoordinitot (  \
                    idsessione   INTEGER, \
                    idcassa         VARCHAR, \
                    numeroordine INTEGER, \
                    tsstampa     DATETIME, \
                    importo      REAL, \
                    flagstorno   BOOLEAN NOT NULL DEFAULT (0))")   ||
            !stmt.exec("CREATE TABLE storicoordinidett (  \
                        idsessione   INTEGER, \
                        idcassa         VARCHAR, \
                        numeroordine INTEGER, \
                        descrizione  VARCHAR, \
                        quantita     INTEGER, \
                        destinazione VARCHAR, \
                        prezzo       REAL, \
                        tipoArticolo CHAR     NOT NULL  )")  ||
            !stmt.exec("CREATE VIEW storicoordini AS \
                    SELECT tot.idsessione, tot.idcassa, tot.numeroordine, tot.tsstampa, tot.importo, dett.descrizione, dett.prezzo, dett.quantita, dett.destinazione, dett.tipoArticolo \
                    FROM storicoordinitot tot, storicoordinidett dett \
                    WHERE tot.idsessione = dett.idsessione \
                    AND tot.idcassa = dett.idcassa \
                    AND tot.numeroordine = dett.numeroordine \
                    AND tot.flagstorno = 0")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }
        nuovaVersioneDB = 8;
    }

    if (versioneDB < 9) {
       if (!stmt.exec("CREATE SEQUENCE SEQARTICOLI") ||
           !stmt.exec("ALTER SEQUENCE SEQARTICOLI RESTART WITH 0") ||
           !stmt.exec("CREATE TRIGGER articoli_t1 \
                      ACTIVE BEFORE INSERT POSITION 0 \
                      ON articoli \
                      AS BEGIN \
                         if (NEW.IDarticolo is NULL) \
                              then NEW.IDarticolo = next value for seqarticoli; \
                      end")) {
           QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
           db.rollback();
           return false;
       }
       nuovaVersioneDB=9;
    }

    if (versioneDB != nuovaVersioneDB) {
        versioneDB = nuovaVersioneDB;
        conf->insert("versione", versioneDB);
        stmt.prepare("update or insert into configurazione (chiave,valore) values('versione',?)");
        stmt.addBindValue(nuovaVersioneDB);
        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }

    }

    db.commit();
    return true;
}

bool DBManager::createConnection(const QString &nomeFile, const QString &utente, const QString &password, const QString modello)
{
    if (nomeFile.isEmpty())
        return false;
    QFile dbFile(nomeFile);
    if (!dbFile.exists()) {
        creaDb(utente, password, modello);
    }
    //QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE");
    //db.setHostName("10.30.102.157");
    //db.setPort(3051);
    db.setDatabaseName(nomeFile);
    db.setUserName(utente);
    db.setPassword(password);
    //db.setDatabaseName(nomeFile);

    // testa se il server è attivo
    /*
    QTcpSocket testsock;
    testsock.connectToHost(db.hostName(),db.port());
    if(!testsock.waitForConnected(3000)) {
      QMessageBox::critical(0, QObject::tr("Database Error"),testsock.errorString());
      return false;
    }
    */

    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().text());
        return false;
    }
    QSqlQuery query("select 1 from articoli");
    if (!query.isActive()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), "Database inesistente o inutilizzabile");
        return false;
    }

    /*
          query.exec("pragma foreign_keys=ON;");
          if(!query.isActive()) {
            QMessageBox::critical(0, QObject::tr("Database Error"),query.lastError().text());
            return false;
          }
    */

    return true;
}

void DBManager::creaDb(const QString user, const QString password, const QString modello)
{
    QFile dbFile(dbFilePath);
    if (dbFile.exists()) {
        QString descrizione = QString("Il file %1 esiste già. Lo cancello?").arg(dbFilePath);
        ConfermaDlg* dlg = new ConfermaDlg(descrizione, "", false, 0);
        if (QDialog::Accepted != dlg->visualizza()) return;
        dbFile.remove();
    }
    if (!QFile::copy(modello, dbFilePath)) {
        QMessageBox::critical(0, QObject::tr("Database Error"), tr("Errore nella creazione del database"));
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE");
    db.setDatabaseName(dbFilePath);
    db.setUserName(user);
    db.setPassword(password);
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().text());
        return;
    }

    QFile sqlFile(":/GestCassa/creadb-firebird");
    if (!sqlFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, QObject::tr("Database Error"), tr("Errore nella lettura della risorsa creadb"));
        return;
    }
    QSqlQuery stmt;
    QString sqlString = sqlFile.readAll();
    foreach(QString sql, sqlString.split(";")) {
        if (sql.isEmpty()) continue;
        qDebug(sql.toUtf8());
        if (!stmt.exec(sql)) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            return;
        }

    }

    if (!stmt.exec("insert into sessione (idsessione,tsinizio) values ('1',current_timestamp)")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }

}
