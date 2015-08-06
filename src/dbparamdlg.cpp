#include "dbparamdlg.h"
#include "simplecrypt.h"

#include <QtSql>
#include <QTcpSocket>
#include <QMessageBox>
#include <QSettings>

DBParamDlg::DBParamDlg(QWidget *parent) :
QDialog(parent)
{
  setWindowFlags(Qt::Tool);
  activateWindow();
  setupUi(this);
}

void DBParamDlg::on_dbLocaleFlg_clicked()
{
    dbStackedWidget->setCurrentIndex(0);
}

void DBParamDlg::on_dbReteFlg_clicked()
{
  dbStackedWidget->setCurrentIndex(1);
}

void DBParamDlg::accept()
{
  if(dbReteFlg->isChecked()) {
    if(!openDBRemoto())
      return;
  }

  QSettings iniSettings(nomeFile,QSettings::IniFormat);
  if(dbReteFlg->isChecked()) {
    iniSettings.setValue("DATABASE/DBLOCALE","0");
  } else {
    iniSettings.setValue("DATABASE/DBLOCALE","1");
  }
  iniSettings.setValue("DATABASE/DBSERVER",dbServerTxt->text());
  iniSettings.setValue("DATABASE/DBPORT",dbPortaTxt->text());
  iniSettings.setValue("DATABASE/DBLOCALEPATH",dbLocalePathTxt->text());
  iniSettings.setValue("DATABASE/DBNOME",dbNomeTxt->text());
  iniSettings.setValue("DATABASE/DBUTENTE",dbUtenteTxt->text());

  SimpleCrypt* cifratore=new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));
  iniSettings.setValue("DATABASE/DBPASSWORD",cifratore->encryptToString(dbPasswordTxt->text()));
  delete cifratore;

  iniSettings.sync();

  QDialog::accept();
}

bool DBParamDlg::openDBRemoto()
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE");
  db.setHostName(dbServerTxt->text());
  db.setPort(dbPortaTxt->text().toInt());
  db.setDatabaseName(dbNomeTxt->text());
  db.setUserName(dbUtenteTxt->text());
  db.setPassword(dbPasswordTxt->text());

  // testa se il server è attivo
  QTcpSocket testsock;
  testsock.connectToHost(db.hostName(),db.port());
  if(!testsock.waitForConnected(3000)) {
    QMessageBox::critical(0, QObject::tr("Database Error"),testsock.errorString());
    return false;
  }

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

      /*
            query.exec("pragma foreign_keys=ON;");
            if(!query.isActive()) {
              QMessageBox::critical(0, QObject::tr("Database Error"),query.lastError().text());
              return false;
            }
      */
  }

  return true;
}
