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

  if(configurazione->value("abilitaResto").toBool()) {
    int durataSecondi=configurazione->value("durataResto",5).toInt();
    RestoDlg restoDlg(importoUltimoOrdine,durataSecondi,this);
    restoDlg.exec();
  }
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
  QString intest1=configurazione->value("intestazione1").toString();
  QString intest2=configurazione->value("intestazione2").toString();
  QString intest3=configurazione->value("intestazione3").toString();
  QString nomeCassa=configurazione->value("nomeCassa","000").toString();
  QString descrManifestazione=configurazione->value("descrManifestazione","NOME MANIFESTAZIONE").toString();

  QString intestazione;
  intestazione.append(descrManifestazione).append("\n");
  if(!intest1.isEmpty()) {
    intestazione.append(intest1).append("\n");
  }
  if(!intest2.isEmpty()) {
    intestazione.append(intest2).append("\n");
  }
  if(!intest3.isEmpty()) {
    intestazione.append(intest3).append("\n");
  }

  QPrinter printer;
  bool stampantePdf=configurazione->value("stampantePdf",true).toBool();
  QString stampanteSelezionata=configurazione->value("stampante","PDF").toString();
  if(stampantePdf) {
    QDir cartellaPdf(configurazione->value("cartellaPdf","c:/temp").toString());
    cartellaPdf.mkpath(cartellaPdf.absolutePath());
    printer.setOutputFileName(QString("%1/%2.pdf").arg(cartellaPdf.absolutePath()).arg(numeroOrdine,5,10,QChar('0')));
  } else {
    printer.setPrinterName(stampanteSelezionata);
  }

  printer.setResolution(100);
  QSizeF foglioSize(80,200);
  float rapportoFoglio=foglioSize.width()/foglioSize.height();
  printer.setPaperSize(foglioSize,QPrinter::Millimeter);
  printer.setPageMargins(5,0,5,0,QPrinter::Millimeter);

  QRect textRect;

  QPainter painter;

  int pageWidth=300;

  QFont fontNormale("lucida console");
  fontNormale.setPointSize(12);
  QFont fontGrassetto("lucida console");
  fontGrassetto.setPointSize(14);
  fontGrassetto.setBold(true);
  QFont fontGrassettoCorsivo("lucida console");
  fontGrassettoCorsivo.setPointSize(14);
  fontGrassettoCorsivo.setBold(true);
  fontGrassettoCorsivo.setItalic(true);
  QFont fontMini("lucida console");
  fontMini.setPointSize(1);

  painter.begin(&printer);
  painter.setWindow(0,0,pageWidth,pageWidth/rapportoFoglio);

  // stampa scontrini per destinazione

  QSqlQuery stmt;
  stmt.prepare("select distinct(destinazione) from righeordine a,articoli b on a.idarticolo=b.idarticolo where numeroordine=?");
  stmt.addBindValue(numeroOrdine);

  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  QStringList repartiStampaList;
  while(stmt.next()) {
    repartiStampaList.append(stmt.value(0).toString());
  }
  foreach(QString reparto,repartiStampaList) {

    stmt.prepare("select coalesce(intestazione,nome) from destinazionistampa where nome=?");
    stmt.addBindValue(reparto);
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt.lastError().text());
      return;
    }

    QString intestReparto=reparto;
    if(stmt.next()) {
      intestReparto=stmt.value(0).toString();
    }

    int x=0;
    int y=0;
    painter.setFont(fontGrassettoCorsivo);
    painter.drawText(x,y,pageWidth,100,Qt::AlignHCenter|Qt::TextWordWrap,intestReparto,&textRect);
    y+=textRect.height()+10;
    painter.setFont(fontGrassetto);
    painter.drawText(x,y,pageWidth,100,Qt::AlignHCenter|Qt::TextWordWrap,descrManifestazione,&textRect);
    y+=textRect.height()+10;

    painter.setFont(fontNormale);
    painter.drawText(x,y,pageWidth,100,Qt::AlignHCenter,QDateTime::currentDateTime().toLocalTime().toString("dd-MM-yyyy   hh:mm:ss"),&textRect);
    y+=textRect.height()+10;
    painter.drawText(x,y,200,100,Qt::AlignLeft,QString("CASSA %1").arg(nomeCassa),&textRect);
    painter.drawText(x+pageWidth/2,y,pageWidth/2,100,Qt::AlignRight,QString("ORDINE N. %1").arg(numeroOrdine),&textRect);
    painter.drawLine(x,y+textRect.height()+5,pageWidth,y+textRect.height()+5);
    y+=10;

    stmt.prepare("select quantita,prezzo,descrizione from righeordine a,articoli b on a.idarticolo=b.idarticolo where numeroordine=? and destinazione=?");
    stmt.addBindValue(numeroOrdine);
    stmt.addBindValue(reparto);

    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt.lastError().text());
      return;
    }
    int numArticoli=0;
    while(stmt.next()) {
      y+=textRect.height();
      QString descrizione=stmt.value(2).toString();
      int quantita=stmt.value(0).toInt();
      numArticoli+=quantita;
      QString quantitaString=QString("%1").arg(quantita,3,10);
      float prezzo=stmt.value(1).toFloat()*quantita;
      QString prezzoString=QString("%1 %2").arg(QChar(0x20AC)).arg(prezzo,5,'f',2);

      QRect tmpRect;
      painter.setFont(fontNormale);
      painter.drawText(x,y,(pageWidth/10)*2-5,100,Qt::AlignLeft|Qt::AlignTop,quantitaString,&tmpRect);
      painter.drawText(x+(pageWidth/10)*2,y,(pageWidth/10)*5-5,1000,Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,descrizione,&textRect);
    }

    y+=textRect.height()+5;
    painter.drawLine(x,y,pageWidth,y);

    y+=5;
    QString totaleString=QString("TOTALE: %1 ARTICOLI").arg(numArticoli);
    painter.setFont(fontGrassetto);
    painter.drawText(x,y,pageWidth,100,Qt::AlignRight,totaleString,&textRect);

    y+=textRect.height()+35;
    painter.setFont(fontMini);
    painter.drawText(x,y,".");
    painter.setFont(fontNormale);

    if(stampantePdf) {
      printer.newPage();
    } else {
      painter.end();
      painter.begin(&printer);
      painter.setWindow(0,0,pageWidth,pageWidth/rapportoFoglio);
    }
  }

  // stampa scontrino del totale

  int x=0;
  int y=0;

  painter.drawText(x,y,pageWidth,100,Qt::AlignHCenter,QDateTime::currentDateTime().toLocalTime().toString("dd-MM-yyyy   hh:mm:ss"),&textRect);
  y+=textRect.height()+10;

  painter.setFont(fontGrassetto);
  painter.drawText(x,y,pageWidth,100,Qt::AlignHCenter|Qt::TextWordWrap,intestazione,&textRect);
  y+=textRect.height();
  y+=5;
  QPen pen(Qt::black,2);
  painter.setPen(pen);
  painter.drawRect(textRect.x()-5,textRect.y()-5,textRect.width()+10,textRect.height());
  pen.setWidth(1);
  painter.setPen(pen);
  y+=5;
  painter.setFont(fontNormale);
  painter.drawText(x,y,200,100,Qt::AlignLeft,QString("CASSA %1").arg(nomeCassa),&textRect);
  painter.drawText(x+pageWidth/2,y,pageWidth/2,100,Qt::AlignRight,QString("ORDINE N. %1").arg(numeroOrdine),&textRect);
  painter.drawLine(x,y+textRect.height()+5,pageWidth,y+textRect.height()+5);
  y+=10;

  stmt.prepare("select quantita,prezzo,descrizione from righeordine a,articoli b on a.idarticolo=b.idarticolo where numeroordine=?");
  stmt.addBindValue(numeroOrdine);

  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  float totale=0;
  while(stmt.next()) {
    y+=textRect.height();
    QString descrizione=stmt.value(2).toString();
    int quantita=stmt.value(0).toInt();
    QString quantitaString=QString("%1").arg(quantita,3,10);
    float prezzo=stmt.value(1).toFloat()*quantita;
    QString prezzoString=QString("%1 %2").arg(QChar(0x20AC)).arg(prezzo,5,'f',2);

    totale+=prezzo;
    QRect tmpRect;
    painter.setFont(fontNormale);
    painter.drawText(x,y,(pageWidth/10)*2-5,100,Qt::AlignLeft|Qt::AlignTop,quantitaString,&tmpRect);
    painter.drawText(x+(pageWidth/10)*2,y,(pageWidth/10)*5-5,1000,Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,descrizione,&textRect);
    painter.drawText(x+(pageWidth/10)*7,y,(pageWidth/10)*3,100,Qt::AlignRight|Qt::AlignTop,prezzoString,&tmpRect);
  }
  y+=textRect.height()+5;
  painter.drawLine(x,y,pageWidth,y);

  y+=5;
  QString totaleString=QString("TOTALE: %1 %2").arg(QChar(0x20AC)).arg(totale,5,'f',2);
  painter.setFont(fontGrassetto);
  painter.drawText(x,y,pageWidth,100,Qt::AlignRight,totaleString,&textRect);

  y+=textRect.height()+35;
  painter.setFont(fontMini);
  painter.drawText(x,y,".");
  painter.end();

}

void Ordine::on_ultimoRestoBtn_clicked()
{
  RestoDlg restoDlg(importoUltimoOrdine,0,this);
  restoDlg.exec();

}
