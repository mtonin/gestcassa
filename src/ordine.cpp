#include "ordine.h"
#include "restodlg.h"
#include "controlliordine.h"
#include <QMessageBox>
#include <QTimer>
#include <QtSql>

Ordine::Ordine(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  articoliTab->setModel(&modello);
  articoliTab->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
  articoliTab->hideColumn(0);
  articoliTab->verticalHeader()->setDefaultSectionSize(30);
  connect(&modello,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(ricalcolaTotale(QModelIndex,QModelIndex)));
  controlli=new ControlliOrdine(this);
  nuovoOrdine();
}

void Ordine::nuovoArticolo(const int idArticolo, const QString descrizione, const float prezzo)
{
  controlli->hide();
  modello.incrementa(idArticolo,descrizione,prezzo);
  articoliTab->scrollToBottom();

}

void Ordine::hide()
{
  controlli->hide();
  QWidget::hide();
}

void Ordine::on_articoliTab_clicked(const QModelIndex &index)
{
  int id=modello.index(index.row(),0).data().toInt();
  controlli->setIdArticolo(id);
  controlli->setModelIndex(index);
  connect(&modello,SIGNAL(rigaCancellata()),controlli,SLOT(close()));
  disconnect(controlli,0,0,0);
  connect(controlli,SIGNAL(incrementa(int)),&modello,SLOT(incrementa(int)));
  connect(controlli,SIGNAL(decrementa(int,QModelIndex)),&modello,SLOT(decrementa(int,QModelIndex)));

  QPoint pos=QCursor::pos();
  pos.setY(pos.y()+20);
  controlli->move(pos);
  controlli->show();
}

void Ordine::ricalcolaTotale(QModelIndex, QModelIndex)
{
  float totale=0;
  for(int i=0;i<modello.rowCount(QModelIndex());i++) {
    totale+=modello.index(i,3).data().toFloat();
  }
  totaleLine->setText(QString("%1").arg(totale,4,'f',2));
}

void Ordine::on_annullaBtn_clicked()
{
  if(QMessageBox::Ok==QMessageBox::question(0,"Annulla ordine","Confermi l'annullamento dell'ordine?",QMessageBox::Ok|QMessageBox::No)) {
    modello.clear();
  }
}

void Ordine::on_stampaBtn_clicked()
{
  if(0==modello.rowCount()) {
    return;
  }

  int numeroOrdine=numeroLbl->text().toInt();

  QSqlQuery stmt;
  stmt.prepare("insert into ordini(numero,tsstampa) values(?,?)");
  stmt.addBindValue(numeroOrdine);
  QDateTime ts=QDateTime::currentDateTime();
  stmt.addBindValue(ts);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }

  modello.completaOrdine(numeroOrdine);

  importoUltimoOrdine=totaleLine->text().toFloat();
  RestoDlg restoDlg(importoUltimoOrdine,this);
  restoDlg.exec();
  nuovoOrdine();
}

void Ordine::nuovoOrdine()
{
  modello.clear();
  QSqlQuery query("select max(numero) from ordini");
  if(!query.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                        query.lastError().text());
    return;
  }
  int numeroOrdine=0;
  if(query.next()) {
    numeroOrdine=query.value(0).toInt();
  }
  numeroOrdine++;
  numeroLbl->setText(QString("%1").arg(numeroOrdine));
}

void Ordine::on_ultimoRestoBtn_clicked()
{
  RestoDlg restoDlg(importoUltimoOrdine,this);
  restoDlg.exec();

}
