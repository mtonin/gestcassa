#include "dettaglireparto.h"

#include <QColorDialog>
#include <QtSql>

DettagliReparto::DettagliReparto(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
}

void DettagliReparto::setCurrentReparto(RepartoBtnWidget *currentRepartoBtn){

  repartoBtn=currentRepartoBtn;
  testoReparto->setText(repartoBtn->getNomeReparto());
  QColor colore=repartoBtn->getColore();
  coloreBtn->SetButtonColorNormal(colore);
  fontBtn->setFont(repartoBtn->getFont());
}

void DettagliReparto::aggiornaReparto()
{
  QSqlQuery query("insert or replace into reparti (idreparto,descrizione,font,coloresfondo) values(?,?,?,?)");
  query.addBindValue(repartoBtn->getId());
  query.addBindValue(testoReparto->text());
  query.addBindValue(repartoBtn->getFont());
  query.addBindValue(repartoBtn->getColore().toRgb());
  query.exec();
  if(!query.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          query.lastError().text());
    return;
  }
}

void DettagliReparto::on_coloreBtn_clicked()
{
  QColorDialog dlg;
  QColor colore=repartoBtn->getColore();
  dlg.setCurrentColor(colore);
  if(dlg.exec()) {
    colore=dlg.currentColor();
    repartoBtn->setColore(colore);
    coloreBtn->SetButtonColorNormal(colore);
  }
  aggiornaReparto();

}

void DettagliReparto::on_fontBtn_clicked()
{
  QFontDialog dlg;
  dlg.setCurrentFont(repartoBtn->getFont());
  if(dlg.exec()) {
    QFont currentFont=dlg.currentFont();
    fontBtn->setFont(currentFont);
    repartoBtn->setButtonFont(currentFont);
  }
  aggiornaReparto();
}

void DettagliReparto::on_testoReparto_textEdited(const QString &arg1)
{
    repartoBtn->setText(arg1);
    aggiornaReparto();
}
