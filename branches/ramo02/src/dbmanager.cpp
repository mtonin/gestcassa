#include "dbmanager.h"
#include "confermadlg.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QtSql>
#include <QDesktopServices>
#include <QMessageBox>

const QString dbFileName("GestioneCassa/cassadb.db3");

DBManager::DBManager(QObject *parent) :QObject(parent) {
}

DBManager::DBManager(QMap<QString, QVariant> *configurazione):conf(configurazione) {

  QFileInfo dbFileInfo(QString("%1/%2")
             .arg(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
             .arg(dbFileName));
  dbFilePath=dbFileInfo.absoluteFilePath();
  QDir dbParentDir(dbFileInfo.absolutePath());
  dbParentDir.mkpath(dbFileInfo.absolutePath());
  leggeConfigurazione();
}

void DBManager::leggeConfigurazione() {

  if(!createConnection(dbFilePath,"","")) {
    return;
  }

  QSqlQuery stmt("select chiave,valore from configurazione");
  if(!stmt.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
  }
  while(stmt.next()) {
    QString key=stmt.value(0).toString();
    QVariant valore=stmt.value(1).toString();
    conf->insert(key,valore);
  }

  if(!stmt.exec("select max(idsessione) from sessione")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
  }
  while(stmt.next()) {
    QVariant valore=stmt.value(0).toString();
    conf->insert("sessioneCorrente",valore);
  }

  conf->insert("dbFilePath",dbFilePath);

  int versioneDB=conf->value("versione",1).toInt();
  int nuovaVersioneDB=versioneDB;

  QSqlDatabase db=QSqlDatabase::database();
  db.transaction();
  if(versioneDB<2) {
    if(!stmt.exec("alter table destinazionistampa add column stampaflag   BOOLEAN NOT NULL DEFAULT ( 'true' ) ")) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }
    nuovaVersioneDB=2;
  }
  if(versioneDB<3) {
    if(!stmt.exec("alter table ordinirighe rename to ordinirighe_old") ||
       !stmt.exec("CREATE TABLE ordinirighe (numeroordine INTEGER REFERENCES ordini ( numero ), \
                  idarticolo   INTEGER,   \
                  quantita     INTEGER );") ||
       !stmt.exec("insert into ordinirighe select numeroordine,idarticolo,quantita from ordinirighe_old") ||
       !stmt.exec("drop table ordinirighe_old")) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }
    nuovaVersioneDB=3;
  }

  if(versioneDB<4) {
    if(!stmt.exec("alter table reparti add column abilitato   BOOLEAN NOT NULL DEFAULT ( 'true' ) ")) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }
    nuovaVersioneDB=4;
  }

  if(versioneDB<5) {
    if(!stmt.exec("alter table ordinicontenuto rename to storicoordini")) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }
    nuovaVersioneDB=5;
  }

  if(versioneDB<6) {
    if(!stmt.exec("alter table destinazionistampa add column stampanumeroritiroflag BOOLEAN NOT NULL DEFAULT ( 'false' ) ")) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }
    nuovaVersioneDB=6;
  }

  if(versioneDB<7) {
    if(!stmt.exec("CREATE TABLE storicoordinitot (  \
                  idsessione   INTEGER, \
                  numeroordine INTEGER, \
                  tsstampa     DATETIME, \
                  importo      REAL, \
                  flagstorno   BOOLEAN NOT NULL DEFAULT ('false'))")   ||
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
                  AND tot.flagstorno = 'false'")  ||
       !stmt.exec("insert into storicoordinitot (idsessione,numeroordine,tsstampa,importo) \
                  select a.idsessione,a.numeroordine,a.tsstampa,a.importo \
                  from storicoordini_old a \
                  group by a.idsessione,a.numeroordine,a.tsstampa,a.importo")  ||
       !stmt.exec("insert into storicoordinidett (idsessione,numeroordine,descrizione,quantita,destinazione,prezzo,tipoarticolo) \
                  select a.idsessione,a.numeroordine,a.descrizione,a.quantita,a.destinazione,a.prezzo,a.tipoArticolo \
                  from storicoordini_old a") ||
       !stmt.exec("drop table storicoordini_old")     ) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }
    nuovaVersioneDB=7;
  }

  if(versioneDB!=nuovaVersioneDB) {
    versioneDB=nuovaVersioneDB;
    conf->insert("versione",versioneDB);
    stmt.prepare("replace into configurazione (chiave,valore) values('versione',?)");
    stmt.addBindValue(nuovaVersioneDB);
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }

  }

  db.commit();
}

bool DBManager::createConnection(const QString &nomeFile, const QString &utente, const QString &password)
{
  if(nomeFile.isEmpty())
    return false;
  QFile dbFile(nomeFile);
  if(!dbFile.exists()) {
    creaDb();
  }
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(nomeFile);
  db.setUserName(utente);
  db.setPassword(password);
  if (!db.open()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),db.lastError().text());
    return false;
  }
  QSqlQuery query("select 1 from articoli");
  if(!query.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),"Database inesistente o inutilizzabile");
    return false;
  }

  query.exec("pragma foreign_keys=ON;");
  if(!query.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),query.lastError().text());
    return false;
  }

  return true;
}

void DBManager::creaDb()
{
  QFile dbFile(dbFilePath);
  if(dbFile.exists()) {
    QString descrizione=QString("Il file %1 esiste già. Lo cancello?").arg(dbFilePath);
    ConfermaDlg* dlg=new ConfermaDlg(descrizione,"",false,0);
    if(QDialog::Accepted!=dlg->visualizza()) return;
    dbFile.remove();
  }
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbFilePath);
  //db.setUserName(utente);
  //db.setPassword(password);
  if (!db.open()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),db.lastError().text());
    return;
  }

  QFile sqlFile(":/GestCassa/creadb");
  if(!sqlFile.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(0,QObject::tr("Database Error"),tr("Errore nella lettura della risorsa creadb"));
    return;
  }
  QSqlQuery stmt;
  QString sqlString=sqlFile.readAll();
  foreach (QString sql,sqlString.split(";")) {
    if(sql.isEmpty()) continue;
    qDebug(sql.toUtf8());
    if(!stmt.exec(sql)) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      return;
    }

  }

  if(!stmt.exec("insert into sessione (idsessione,tsinizio) values ('1',datetime('now'))")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }

}
