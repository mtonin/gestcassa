#include "dettagliarticolo.h"
#include <QtSql>
#include <QLocale>

DettagliArticolo::DettagliArticolo(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  QDoubleValidator* validator=new QDoubleValidator(0,99.99,2,prezzoArticolo);
  validator->setNotation(QDoubleValidator::StandardNotation);
  prezzoArticolo->setValidator(validator);
}

void DettagliArticolo::setCurrentArticolo(const ArticoloBtnWidget *currentArticoloBtn){
  articoloBtn=(ArticoloBtnWidget*)currentArticoloBtn;
  testoArticolo->setText(articoloBtn->getNomeArticolo());
  QLocale locale;
  QString str=locale.toString(articoloBtn->getPrezzo());
  prezzoArticolo->setText(str);
  //prezzoArticolo->setText(QString::number(articoloBtn->getPrezzo(),'g',2));
  disattivaFlag->setChecked(!articoloBtn->getAbilitato());
  testoArticolo->setFocus();
}

void DettagliArticolo::aggiornaArticolo()
{
  QSqlQuery query("insert or replace into articoli (idarticolo,descrizione,prezzo,idreparto,riga,colonna,abilitato) values(?,?,?,?,?,?,?)");
  query.addBindValue(articoloBtn->getId());
  query.addBindValue(articoloBtn->getNomeArticolo());
  query.addBindValue(articoloBtn->getPrezzo());
  query.addBindValue(articoloBtn->getIdReparto());
  query.addBindValue(articoloBtn->getRiga());
  query.addBindValue(articoloBtn->getColonna());
  query.addBindValue(!disattivaFlag->isChecked());
  query.exec();
  if(!query.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          query.lastError().text());
    return;
  }
}

void DettagliArticolo::on_testoArticolo_textEdited(const QString &testo)
{
  articoloBtn->setNomeArticolo(testo);
  aggiornaArticolo();

}

void DettagliArticolo::on_prezzoArticolo_textEdited(const QString &prezzo)
{
  QLocale locale;
  float val=locale.toFloat(prezzo);
  articoloBtn->setPrezzo(val);
  aggiornaArticolo();
}

void DettagliArticolo::on_disattivaFlag_toggled(bool checked)
{
  articoloBtn->setAbilitato(!checked);
  aggiornaArticolo();
}
