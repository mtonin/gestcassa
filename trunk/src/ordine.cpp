#include "ordine.h"
#include "restodlg.h"
#include "controlliordine.h"
#include <QMessageBox>
#include <QTimer>
#include <QtSql>
#include <QPrinter>
#include <QPainter>

Ordine::Ordine(QMap<QString, QVariant> *par, QWidget *parent) : configurazione(par), QWidget(parent)
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
  QString stampanteSelezionata=configurazione->value("stampante","PDF").toString();
  if("PDF"==stampanteSelezionata) {
    printer.setOutputFileName(QString("c:\\temp\\%1.pdf").arg(numeroOrdine,5,10,QChar('0')));
  } else {
    printer.setPrinterName(stampanteSelezionata);
  }

  int pageWidth=printer.pageRect().width();
  QRect textRect;

  QPainter painter(&printer);
  painter.setFont(QFont("lucida console"));
  painter.setWindow(0,0,1000,2000);
  painter.setViewport(0,0,1000,2000);

  int x=0;
  int y=5;

  QString intest1=configurazione->value("intestazione1").toString();
  QString intest2=configurazione->value("intestazione2").toString();
  QString intest3=configurazione->value("intestazione3").toString();
  if(!intest1.isEmpty()) {
    painter.drawText(x,y,200,100,Qt::AlignHCenter,intest1,&textRect);
    y+=textRect.height();
  }
  if(!intest2.isEmpty()) {
    painter.drawText(x,y,200,100,Qt::AlignHCenter,intest2,&textRect);
    y+=textRect.height();
  }
  if(!intest3.isEmpty()) {
    painter.drawText(x,y,200,100,Qt::AlignHCenter,intest3,&textRect);
    y+=textRect.height();
  }
  y+=5;
  QPen pen(Qt::black,2);
  painter.setPen(pen);
  painter.drawRect(0,0,200,y);
  pen.setWidth(1);
  painter.setPen(pen);
  y+=5;
  painter.drawText(x,y,100,100,Qt::AlignLeft,QString("CASSA %1").arg("01"),&textRect);
  painter.drawText(x+110,y,100,100,Qt::AlignLeft,QString("ORDINE N. %1").arg(numeroOrdine),&textRect);
  y+=textRect.height();
  painter.drawText(x,y,200,100,Qt::AlignLeft,QDateTime::currentDateTime().toLocalTime().toString("dd-MM-yyyy   hh:mm:ss"),&textRect);
  painter.drawLine(x,y+textRect.height()+5,pageWidth,y+textRect.height()+5);
  y+=5;

  QSqlQuery stmt;
  stmt.prepare("select quantita,prezzo,descrizione from righeordine a,articoli b on a.idarticolo=b.idarticolo where numeroordine=?");
  stmt.addBindValue(numeroOrdine);

  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  float totale=0;
  while(stmt.next()) {
    y+=textRect.height()+5;
    QString descrizione=stmt.value(2).toString();
    int quantita=stmt.value(0).toInt();
    QString quantitaString=QString("%1").arg(quantita,3,10);
    float prezzo=stmt.value(1).toFloat()*quantita;
    QString prezzoString=QString("%1 %2").arg(QChar(0x20AC)).arg(prezzo,5,'f',2);

    totale+=prezzo;
    QRect tmpRect;
    painter.drawText(x,y,40,1000,Qt::AlignLeft|Qt::AlignTop,quantitaString,&tmpRect);
    painter.drawText(x+45,y,110,1000,Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,descrizione,&textRect);
    painter.drawText(x+160,y,150,1000,Qt::AlignLeft|Qt::AlignTop,prezzoString,&tmpRect);
  }
  y+=textRect.height()+5;
  painter.drawLine(x,y,pageWidth,y);

  y+=5;
  QString totaleString=QString("%1 %2").arg(QChar(0x20AC)).arg(totale,5,'f',2);
  painter.drawText(x,y,150,100,Qt::AlignLeft,"TOTALE:",&textRect);
  painter.drawText(x+160,y,150,100,Qt::AlignLeft,totaleString,&textRect);

  painter.end();

}

void Ordine::on_ultimoRestoBtn_clicked()
{
  RestoDlg restoDlg(importoUltimoOrdine,this);
  restoDlg.exec();

}
