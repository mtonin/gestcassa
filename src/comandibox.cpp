#include <QFileDialog>
#include <QtSql>
#include <QMessageBox>
#include "comandibox.h"

ComandiBox::ComandiBox(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  creaComandi();
}

void ComandiBox::creaComandi()
{

}

QString ComandiBox::on_dbLoadBtn_clicked()
{
  QString nomeFile=QFileDialog::getOpenFileName(this,"Apre database","C:\\","*.db3");
  if(!nomeFile.isEmpty()) {
      QSqlDatabase db=QSqlDatabase::database();
      if(createConnection(nomeFile)) {
          db.close();
          emit dbCaricato();
          return nomeFile;
        }
    }
  return "";
}

bool ComandiBox::createConnection(const QString& nomeFile)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(nomeFile);
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              db.lastError().text());

        return false;
    }

    return true;
}

void ComandiBox::on_modalitaBtn_clicked(){

  QString testo=modalitaBtn->text();
  if(0==testo.compare("CASSA")) {
      qApp->setOverrideCursor(Qt::BlankCursor);
      modalitaBtn->setText("GESTIONE");
      emit cambiaModalita(1);
    } else {
      qApp->restoreOverrideCursor();
      modalitaBtn->setText("CASSA");
      emit cambiaModalita(0);
    }

}

