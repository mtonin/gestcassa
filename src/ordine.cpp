#include "ordine.h"
#include "restodlg.h"
#include "controlliordine.h"
#include <QMessageBox>
#include <QTimer>
#include <QtSql>
#include <QPrinter>
#include <QPainter>

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

  stampaScontrino(numeroOrdine);

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

void Ordine::stampaScontrino(int numeroOrdine)
{
  QPrinter printer;
  printer.setOutputFileName(QString("c:\\temp\\%1.pdf").arg(numeroOrdine,5,10,QChar('0')));

  int pageWidth=printer.pageRect().width();

  QPainter painter(&printer);
  painter.setFont(QFont("lucida console"));
  painter.setWindow(0,0,1000,2000);
  painter.setViewport(0,0,1000,2000);
  painter.drawText(0,0,QString("Cassa %1").arg("01"));
  painter.drawText(0,15,QString("Scontrino N. %1").arg(numeroOrdine));
  painter.drawText(0,30,QDateTime::currentDateTime().toLocalTime().toString("dd-MM-yyyy   hh:mm:ss"));
  painter.drawLine(0,45,pageWidth,45);

  QSqlQuery stmt;
  stmt.prepare("select quantita,prezzo,descrizione from righeordine a,articoli b on a.idarticolo=b.idarticolo where numeroordine=?");
  stmt.addBindValue(numeroOrdine);

  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  int y=60;
  float totale=0;
  QRect textRect;
  while(stmt.next()) {
    y+=textRect.height()+5;
    QString descrizione=stmt.value(2).toString();
    int quantita=stmt.value(0).toInt();
    QString quantitaString=QString("%1").arg(quantita,3,10);
    float prezzo=stmt.value(1).toFloat()*quantita;
    QString prezzoString=QString("%1 %2").arg(QChar(0x20AC)).arg(prezzo,5,'f',2);

    totale+=prezzo;
    painter.drawText(0,y,40,1000,Qt::AlignLeft|Qt::AlignTop,quantitaString);
    painter.drawText(40,y,100,1000,Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,descrizione,&textRect);
    painter.drawText(150,y,100,1000,Qt::AlignLeft|Qt::AlignTop,prezzoString);
  }
  y+=textRect.height()+5;
  painter.drawLine(0,y,pageWidth,y);

  y+=15;
  QString totaleString=QString("%1 %2").arg(QChar(0x20AC)).arg(totale,5,'f',2);
  painter.drawText(0,y,"TOTALE:");
  painter.drawText(150,y,totaleString);

  y+=15;
  painter.drawLine(0,y,pageWidth,y);
  painter.end();

}

void Ordine::on_ultimoRestoBtn_clicked()
{
  RestoDlg restoDlg(importoUltimoOrdine,this);
  restoDlg.exec();

}
