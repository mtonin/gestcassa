#include "dettagliarticolo.h"
#include <QtSql>

DettagliArticolo::DettagliArticolo(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  connect(testoArticolo,SIGNAL(textChanged(QString)),this,SLOT(testoChanged(QString)));
}

void DettagliArticolo::setCurrentArticolo(const ArticoloBtnWidget *currentArticoloBtn){
  articoloBtn=(ArticoloBtnWidget*)currentArticoloBtn;
  testoArticolo->setText(articoloBtn->getNomeArticolo());
}

void DettagliArticolo::testoChanged(const QString& testo)
{
  articoloBtn->setNomeArticolo(testo);
  aggiornaArticolo();
}

void DettagliArticolo::aggiornaArticolo()
{
  QSqlQuery query("insert or replace into articoli (idarticolo,descrizione,prezzo,idreparto,riga,colonna,attivo) values(?,?,?,?,?,?,?)");
  query.addBindValue(articoloBtn->getId());
  query.addBindValue(testoArticolo->text());
  query.addBindValue(prezzoArticolo->text());
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
