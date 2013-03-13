#include <QtGui/QApplication>
#include <QStyle>
#include <QMessageBox>
#include <QtSql>

#include "mainwindow.h"
#include "dbdialog.h"

bool createConnection(const QString& nomeFile, const QString& utente, const QString& password)
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

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QApplication::setStyle("windowsvista");
  QStyle* stile=QApplication::style();

  DBDialog dlg;
  while(true) {
    if(QDialog::Accepted==dlg.exec()) {
      if(createConnection(dlg.getDbFileName(),dlg.getUser(),dlg.getPassword()))
        break;
    } else {
      return 0;
    }
  }

  MainWindow w;
  w.show();
  
  return a.exec();
}


