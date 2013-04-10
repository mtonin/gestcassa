#include "destinazionestampadlg.h"
#include <QtSql>
#include <QMessageBox>

DestinazioneStampaDlg::DestinazioneStampaDlg(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);

  QSqlQuery stmt("select nome from destinazioniStampa");
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString nome=stmt.value(0).toString();
    box->addItem(nome);
  }
}

void DestinazioneStampaDlg::on_buttonBox_accepted()
{
    return accept();
}

void DestinazioneStampaDlg::on_buttonBox_rejected()
{
    return reject();
}
