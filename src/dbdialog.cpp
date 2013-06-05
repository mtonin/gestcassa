#include "dbdialog.h"

#include <QFile>
#include <QtSql>
#include <QMessageBox>

const QString dbFileName("cassadb.db3");

DBDialog::DBDialog(QMap<QString, QVariant> *configurazione, QWidget *parent):conf(configurazione),QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);

  cifratore=new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));

}

void DBDialog::on_apreBtn_clicked()
{

  if(createConnection(dbFileName,"","")) {
    QSqlQuery stmt("select chiave,valore from configurazione");
    if(!stmt.isActive()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    }
    while(stmt.next()) {
      QString key=stmt.value(0).toString();
      QVariant valore=stmt.value(1).toString();
      conf->insert(key,valore);
    }

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
    accept();
  }
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
  QFile dbFile(dbFileName);
  if(dbFile.exists()) {
    if(QMessageBox::No==QMessageBox::question(this, QObject::tr("Nuovo Database"),QString("Il file %1 esiste già. Lo cancello?").arg(dbFileName),QMessageBox::Yes|QMessageBox::No))
      return;
    else dbFile.remove();
  }
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbFileName);
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
    if(!stmt.exec(sql)) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      return;
    }

  }

}
