#include "dbdialog.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QtSql>
#include <QMessageBox>
#include <QDesktopServices>

const QString dbFileName("GestioneCassa/cassadb.db3");

DBDialog::DBDialog(QMap<QString, QVariant> *configurazione, QWidget *parent):conf(configurazione),QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);

  cifratore=new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));
  QFileInfo dbFileInfo(QString("%1/%2")
             .arg(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
             .arg(dbFileName));
  dbFilePath=dbFileInfo.absoluteFilePath();
  QDir dbParentDir(dbFileInfo.absolutePath());
  dbParentDir.mkpath(dbFileInfo.absolutePath());
}

void DBDialog::on_apreBtn_clicked()
{

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

  QString pwdDB=conf->value("adminPassword").toString();
  if(pwdDB.isEmpty()) {
    pwdDB="12345";
  } else {
    pwdDB=cifratore->decryptToString(pwdDB);
  }
  if(adminBox->isChecked()) {
    if(pwdDB==password->text()) {
      conf->insert("ruolo","amministratore");
    } else {
      QMessageBox::critical(this,"Accesso","Password errata");
      password->selectAll();
      password->setFocus();
      return;
    }
  } else {
    conf->insert("ruolo","operatore");
  }

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

  if(versioneDB!=nuovaVersioneDB) {
    versioneDB=nuovaVersioneDB;
    stmt.prepare("insert or replace into configurazione (chiave,valore) values('versione',?)");
    stmt.addBindValue(nuovaVersioneDB);
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }

  }

  db.commit();
  accept();
}

void DBDialog::on_esceBtn_clicked()
{
  reject();
}

bool DBDialog::createConnection(const QString &nomeFile, const QString &utente, const QString &password)
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

void DBDialog::on_adminBox_clicked(bool checked)
{
    passwordLbl->setEnabled(checked);
    password->setEnabled(checked);
    password->setFocus();
}

void DBDialog::creaDb()
{
  QFile dbFile(dbFilePath);
  if(dbFile.exists()) {
    if(QMessageBox::No==QMessageBox::question(this, QObject::tr("Nuovo Database"),QString("Il file %1 esiste già. Lo cancello?")
                                              .arg(dbFilePath),QMessageBox::Yes|QMessageBox::No))
      return;
    else dbFile.remove();
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
