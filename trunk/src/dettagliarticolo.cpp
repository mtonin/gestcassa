#include "dettagliarticolo.h"
#include <QtSql>
#include <QLocale>
#include <QInputDialog>

DettagliArticolo::DettagliArticolo(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  QDoubleValidator* validator=new QDoubleValidator(0,99.99,2,prezzoArticolo);
  validator->setNotation(QDoubleValidator::StandardNotation);
  prezzoArticolo->setValidator(validator);

  destModel=new QSqlTableModel;
  destModel->setTable("destinazioniStampa");
  destModel->removeColumn(0);
  destModel->select();
  destStampaCombo->setModel(destModel);
}

void DettagliArticolo::setCurrentArticolo(const ArticoloBtnWidget *currentArticoloBtn){
  articoloBtn=(ArticoloBtnWidget*)currentArticoloBtn;
  testoArticolo->setText(articoloBtn->getNomeArticolo());
  /*
  QLocale locale;
  QString str=locale.toCurrencyString(articoloBtn->getPrezzo(),QString(" "));
  prezzoArticolo->setText(str);
  //prezzoArticolo->setText(QString::number(articoloBtn->getPrezzo(),'g',2));
  */

  prezzoArticolo->setText(QString("%1").arg(articoloBtn->getPrezzo(),4,'f',2));
  disattivaFlag->setChecked(!articoloBtn->getAbilitato());
  testoArticolo->setFocus();
}

void DettagliArticolo::aggiornaArticolo()
{

  QSqlQuery query;
  if(0==articoloBtn->getId()) {
    query.prepare("insert into articoli (descrizione,prezzo,idreparto,riga,colonna,abilitato) values(?,?,?,?,?,?)");
  } else {
    query.prepare("update articoli set descrizione=?,prezzo=?,idreparto=?,riga=?,colonna=?,abilitato=? where idarticolo=?");
  }

  //query.addBindValue(articoloBtn->getId());
  query.addBindValue(articoloBtn->getNomeArticolo());
  query.addBindValue(articoloBtn->getPrezzo());
  query.addBindValue(articoloBtn->getIdReparto());
  query.addBindValue(articoloBtn->getRiga());
  query.addBindValue(articoloBtn->getColonna());
  query.addBindValue(!disattivaFlag->isChecked());
  if(articoloBtn->getId()>0) {
    query.addBindValue(articoloBtn->getId());
  }
  query.exec();
  if(!query.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          query.lastError().text());
    return;
  }
  if(0==articoloBtn->getId()) {
    query.clear();
    query.prepare("select idarticolo from articoli where idreparto=? and riga=? and colonna=?");
    query.addBindValue(articoloBtn->getIdReparto());
    query.addBindValue(articoloBtn->getRiga());
    query.addBindValue(articoloBtn->getColonna());
    query.exec();
    if(!query.isActive()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            query.lastError().text());
      return;
    }
    if(query.next())
      articoloBtn->setId(query.value(0).toInt());
  }
}

void DettagliArticolo::on_testoArticolo_textEdited(const QString &testo)
{
  articoloBtn->setNomeArticolo(testo);
  aggiornaArticolo();

}

void DettagliArticolo::on_prezzoArticolo_textEdited(const QString &prezzo)
{
  /*
  QLocale locale;
  float val=locale.toFloat(prezzo);
  */
  float val=prezzo.toFloat();
  articoloBtn->setPrezzo(val);
  aggiornaArticolo();
}

void DettagliArticolo::on_disattivaFlag_toggled(bool checked)
{
  articoloBtn->setAbilitato(!checked);
  aggiornaArticolo();
}

void DettagliArticolo::on_nuovaDestinazioneBtn_clicked()
{
  QString nuovaDest=QInputDialog::getText(this,"Nuova Destinazione di stampa","Inserire il nome della nuova destinazione di stampa");
  if(nuovaDest.isNull()) return;

  QSqlQuery stmt;
  stmt.prepare("insert into destinazionistampa (nome) values(?)");
  stmt.addBindValue(nuovaDest);
  stmt.exec();
  if(!stmt.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }

  destModel->select();
}
