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

void ComandiBox::on_toolButton_2_clicked()
{
  QString nomeFile=QFileDialog::getOpenFileName(this,"Apre database","C:\\","*.db3");
  if(!nomeFile.isEmpty()) {
      QSqlDatabase db=QSqlDatabase::database();
      if(createConnection(nomeFile)) {
          db.close();
          emit dbCaricato();
        }

    }
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

void ComandiBox::on_toolButton_clicked()
{

  QString testo=toolButton->text();
  if(0==toolButton->text().compare("CASSA")) {
      qApp->setOverrideCursor(Qt::BlankCursor);
      toolButton->setText("GESTIONE");
      emit cambiaModalita(1);
    } else {
      qApp->restoreOverrideCursor();
      toolButton->setText("CASSA");
      emit cambiaModalita(0);
    }

}
