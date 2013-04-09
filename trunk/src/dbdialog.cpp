#include "dbdialog.h"

#include <QFileDialog>
#include <QtSql>
#include <QMessageBox>

DBDialog::DBDialog(QMap<QString, QVariant> *configurazione, QWidget *parent):conf(configurazione),QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
  dbfile->setFocus();
}

void DBDialog::on_toolButton_clicked()
{
  QString dbFileName=QFileDialog::getOpenFileName(0,"Apre database esistente","c:\\temp","*.db3");
  if(!dbFileName.isEmpty()) {
    dbfile->setText(dbFileName);
  }
}

void DBDialog::on_apreBtn_clicked()
{
  if(createConnection(dbfile->text(),utente->text(),password->text())) {
    QSqlQuery stmt("select chiave,valore from configurazione");
    if(!stmt.isActive()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    }
    while(stmt.next()) {
      QString key=stmt.value(0).toString();
      QVariant valore=stmt.value(1).toString();
      conf->insert(key,valore);
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

  return true;
}
