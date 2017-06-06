#include "commons.h"
#include "dbmanager.h"
#include "confermadlg.h"
#include "dbparamdlg.h"
#include "simplecrypt.h"

#include <QFile>
#include <QtSql>
#include <QMessageBox>
#include <QTcpSocket>
#include <QSettings>
#include <QHostAddress>

DBManager::DBManager(QObject *parent) : QObject(parent)
{
}

DBManager::DBManager(QMap<QString, QVariant> *configurazione): conf(configurazione)
{

}

bool DBManager::init(const QString percorso)
{

  if (percorso.isEmpty())
      return false;

  dbFilePath=QString("%1/GCAS.fdb").arg(percorso);
  QString dbFileModelloName=QString("%1/model.fdb").arg(percorso);
  QString iniFileName=QString("%1/GCAS.ini").arg(percorso);
  conf->insert("iniFile",iniFileName);

  QString ipAddress;
  DBParamDlg dlg;
  dlg.setNomeFile(iniFileName);
  while(!createConnection(iniFileName,dbFileModelloName,ipAddress)) {
    dlg.activateWindow();
    if(!dlg.exec())
      return false;
  }

  leggeConfigurazione();
  leggeConfigurazioneLocale(iniFileName);

  aggiornaPostazione(ipAddress);

  return true;
}

bool DBManager::leggeConfigurazione()
{
    QSqlDatabase db =QSqlDatabase::database(QSqlDatabase::defaultConnection,false);

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

    if(versioneDB > 9) {
      if(!stmt.exec("select id,oggetto from risorse")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return false;
      }
      while(stmt.next()) {
        QString key=stmt.value(0).toString();
        QByteArray logoData=stmt.value(1).toByteArray();
        conf->insert(key,logoData);
      }
    }

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
        nuovaVersioneDB = 9;
    }

    if (versioneDB < 10) {
        if (!stmt.exec("CREATE TABLE RISORSE ( \
                              ID       VARCHAR(100)      NOT NULL primary key,   \
                              OGGETTO  BLOB)")) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return false;
        }
        nuovaVersioneDB = 10;
    }

    if (versioneDB < 11) {
            if (!stmt.exec("CREATE TABLE buoni ( \
                         cognome     VARCHAR(100), \
                         nome     VARCHAR(100), \
                         tsemissione     TIMESTAMP, \
                         flagannullato   BOOLEAN DEFAULT  0 NOT NULL, \
                           PRIMARY KEY (cognome,nome))"))  {
                QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
                db.rollback();
                return false;
            }
            nuovaVersioneDB = 11;
    }

    if (versioneDB < 12) {
       if (!stmt.exec("alter table reparti add adattafont BOOLEAN DEFAULT 0 NOT NULL"))  {
          QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
          db.rollback();
          return false;
       }
       nuovaVersioneDB = 12;
    }

    if (versioneDB != nuovaVersioneDB) {
        versioneDB = nuovaVersioneDB;
        conf->insert("versione", versioneDB);
        if(!stmt.prepare("update or insert into configurazione (chiave,valore) values('versione',?)")) {
          QSqlError errore=stmt.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(0,"Errore",msg);
          db.rollback();
          return false;
        }
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

bool DBManager::createConnection(const QString &nomeFile,const QString& modello, QString& localIpAddress)
{
    if (nomeFile.isEmpty())
        return false;
    QFile iniFile(nomeFile);
    if (!iniFile.exists())
        return false;

    QSettings iniSettings(nomeFile,QSettings::IniFormat);
    QString utente=iniSettings.value("DATABASE/DBUTENTE").toString();
    SimpleCrypt* cifratore=new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));
    QString password=cifratore->decryptToString(iniSettings.value("DATABASE/DBPASSWORD").toString());
    delete cifratore;

    bool dbLocaleFlag=iniSettings.value("DATABASE/DBLOCALE").toBool();
    if(dbLocaleFlag) {
        dbFilePath=iniSettings.value("DATABASE/DBLOCALEPATH").toString();
        QFile dbFile(dbFilePath);
         if (!dbFile.exists()) {
           creaDb(utente,password,modello);
        }
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE");

    if(dbLocaleFlag) {
        db.setDatabaseName(dbFilePath);
    } else {
        db.setHostName(iniSettings.value("DATABASE/DBSERVER").toString());
        db.setPort(iniSettings.value("DATABASE/DBPORT").toInt());
        db.setDatabaseName(iniSettings.value("DATABASE/DBNOME").toString());
        dbFilePath=QString("%1:%2/%3").arg(db.hostName()).arg(db.port()).arg(db.databaseName());
        // testa se il server è attivo
        QTcpSocket testsock;
        testsock.connectToHost(db.hostName(),db.port());
        if(!testsock.waitForConnected(3000)) {
          QMessageBox::critical(0, QObject::tr("Database Error"),"Impossibile connettersi al database");
          return false;
        }
        localIpAddress=testsock.localAddress().toString();
    }
    db.setUserName(utente);
    db.setPassword(password);
    db.setConnectOptions("ISC_DPB_LC_CTYPE=UTF8");
    //db.setConnectOptions("ISC_DPB_SQL_ROLE_NAME=gcas_user");

    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().text());
        return false;
    }
    {
        QSqlQuery query("select 1 from articoli");
        if (!query.isActive()) {
            QMessageBox::critical(0, QObject::tr("Database Error"), "Database inesistente o inutilizzabile");
            return false;
        }
    }
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
    db.setConnectOptions("ISC_DPB_LC_CTYPE=UTF8");
    //db.setConnectOptions("ISC_DPB_SQL_ROLE_NAME=gcas_user");

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

    //db.close();

}

void DBManager::leggeConfigurazioneLocale(const QString &nomeFile)
{
        conf->insert("iniFile",nomeFile);
        QSettings iniSettings(nomeFile,QSettings::IniFormat);
        iniSettings.beginGroup("CONFIGURAZIONE");
        QStringList chiavi=iniSettings.childKeys();
        foreach (QString chiave,chiavi) {
           conf->insert(chiave,iniSettings.value(chiave).toString());
        }
}

void DBManager::aggiornaPostazione(const QString& ipAddress) {

    QString idCassa=conf->value("IDCASSA").toString();
    QSqlQuery stmt;
    if (!stmt.prepare("update or insert into postazioni (id,ipaddress) values (?,?) returning nome")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    stmt.addBindValue(idCassa);
    stmt.addBindValue(ipAddress);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    if(stmt.next()) {
        QString nomeCassa=stmt.value(0).toString();
        conf->insert("nomeCassa",nomeCassa);
    }

}
