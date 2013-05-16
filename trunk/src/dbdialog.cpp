#include "dbdialog.h"

#include <QFileDialog>
#include <QtSql>
#include <QMessageBox>

DBDialog::DBDialog(QMap<QString, QVariant> *configurazione, QWidget *parent):conf(configurazione),QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
  dbfile->setFocus();

  cifratore=new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));

}

void DBDialog::on_toolButton_clicked()
{
  QString dbFileName=QFileDialog::getOpenFileName(0,"Apre database esistente","c:\\","*.db3");
  if(!dbFileName.isEmpty()) {
    dbfile->setText(dbFileName);
  }
}

void DBDialog::on_apreBtn_clicked()
{

  if(createConnection(dbfile->text(),"","")) {
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

void DBDialog::on_nuovoBtn_clicked()
{
  QString dbFileName=QFileDialog::getSaveFileName(0,"Crea nuovo database","c:\\temp","*.db3");
  if(!dbFileName.isEmpty()) {
    dbfile->setText(dbFileName);
    accept();
  }

}

bool DBDialog::createConnection(const QString &nomeFile, const QString &utente, const QString &password)
{
  if(nomeFile.isEmpty())
    return false;
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
    QMessageBox::critical(0, QObject::tr("Database Error"),query.lastError().text());
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
