#include "destinazionestampadlg.h"
#include <QtSql>
#include <QMessageBox>

DestinazioneStampaDlg::DestinazioneStampaDlg(const QString &nome, QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);

  QSqlQuery stmt;

  if(!nome.isEmpty()) {
    stmt.prepare("select intestazione from destinazioniStampa where nome=?");
    stmt.addBindValue(nome);
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt.lastError().text());
    }
    if(stmt.next()) {
      intestazioneTxt->setText(stmt.value(0).toString());
    }
  }
}

void DestinazioneStampaDlg::on_buttonBox_accepted()
{
    QSqlQuery stmt;
    stmt.prepare("insert or replace into destinazioniStampa (nome,intestazione) values(?,?)");
    stmt.addBindValue(repartoTxt->text());
    stmt.addBindValue(intestazioneTxt->text());
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt.lastError().text());
    }
    return accept();

}
