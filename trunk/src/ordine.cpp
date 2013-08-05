#include "ordine.h"
#include "restodlg.h"
#include "controlliordine.h"
#include "confermadlg.h"

#include <QMessageBox>
#include <QTimer>
#include <QtSql>
#include <QPrinter>
#include <QPainter>

Ordine::Ordine(QMap<QString, QVariant> *par, QWidget *parent) : configurazione(par), QWidget(parent)
{
  setupUi(this);

  articoliTab->setModel(&modello);
  articoliTab->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
  articoliTab->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
  articoliTab->hideColumn(0);
  connect(&modello,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(ricalcolaTotale(QModelIndex,QModelIndex)));
  controlli=new ControlliOrdine(this);

  importoUltimoOrdine=0;
  importoOrdineCorrente=0;
  idSessioneCorrente=configurazione->value("sessioneCorrente").toInt();
  nuovoOrdine(idSessioneCorrente);

  stampaBtn->SetIconSpace(20);
  stampaBtn->setIcon(QIcon(":/GestCassa/stampante"));
  stampaBtn->setIconSize(QSize(48,48));
  stampaBtn->SetIconPosition(QPictureButton::PositionTop);
  annullaBtn->setIcon(QIcon(":/GestCassa/annulla"));
  annullaBtn->setIconSize(QSize(32,32));
  annullaBtn->SetIconPosition(QPictureButton::PositionTop);
  duplicaBtn->setIcon(QIcon(":/GestCassa/duplica"));
  duplicaBtn->setIconSize(QSize(32,32));
  duplicaBtn->SetIconPosition(QPictureButton::PositionTop);
  ultimoRestoBtn->setIcon(QIcon(":/GestCassa/money"));
  ultimoRestoBtn->setIconSize(QSize(32,32));
  ultimoRestoBtn->SetIconPosition(QPictureButton::PositionTop);
  pagPrevBtn->setIcon(QIcon(":/GestCassa/freccia_su"));
  pagPrevBtn->setIconSize(QSize(32,32));
  pagPrevBtn->SetIconPosition(QPictureButton::PositionTop);
  pagNextBtn->setIcon(QIcon(":/GestCassa/freccia_giu"));
  pagNextBtn->setIconSize(QSize(32,32));
  pagNextBtn->SetIconPosition(QPictureButton::PositionTop);
  ristampaBtn->setIcon(QIcon(":/GestCassa/printer-1"));
  ristampaBtn->setIconSize(QSize(32,32));
  ristampaBtn->SetIconPosition(QPictureButton::PositionTop);

  stampaBtn->SetButtonColorNormal(Qt::green);
  annullaBtn->SetButtonColorNormal(Qt::yellow);
  duplicaBtn->SetButtonColorNormal(Qt::yellow);
  ultimoRestoBtn->SetButtonColorNormal(Qt::yellow);
  pagPrevBtn->SetButtonColorNormal(Qt::yellow);
  pagNextBtn->SetButtonColorNormal(Qt::yellow);
  ristampaBtn->SetButtonColorNormal(Qt::yellow);
  stampaBtn->SetButtonColorHot(Qt::magenta);
  annullaBtn->SetButtonColorHot(Qt::magenta);
  duplicaBtn->SetButtonColorHot(Qt::magenta);
  ultimoRestoBtn->SetButtonColorHot(Qt::magenta);
  pagPrevBtn->SetButtonColorHot(Qt::magenta);
  pagNextBtn->SetButtonColorHot(Qt::magenta);
  ristampaBtn->SetButtonColorHot(Qt::magenta);

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
  QDesktopWidget* desktop=QApplication::desktop();
  int screenWidth=desktop->screenGeometry().width();
  if(pos.x()+controlli->width()>screenWidth) {
    pos.setX(screenWidth-controlli->width());
  }
  controlli->move(pos);
  controlli->show();
}

void Ordine::ricalcolaTotale(QModelIndex, QModelIndex)
{
  importoOrdineCorrente=0;
  for(int i=0;i<modello.rowCount(QModelIndex());i++) {
    importoOrdineCorrente+=modello.index(i,3).data(Qt::UserRole).toFloat();
  }
  totaleLine->setText(QString("%L1").arg(importoOrdineCorrente,4,'f',2));
}

void Ordine::on_annullaBtn_clicked()
{
  if(!isInComposizione()) return;
  ConfermaDlg* dlg=new ConfermaDlg("Confermi l'annullamento dell'ordine corrente?","",false,this);
  if(QDialog::Accepted!=dlg->visualizza()) return;
  modello.clear();
}

void Ordine::on_ristampaBtn_clicked()
{
  int numeroOrdine=numOrdineCorrente-1;
  QSqlQuery stmt;
  stmt.prepare("select 1 from storicoordini where idsessione=? and numeroordine=?");
  stmt.addBindValue(idSessioneCorrente);
  stmt.addBindValue(numeroOrdine);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  if(stmt.next()) {
      stampaScontrino(numeroOrdine);
  }
}

void Ordine::on_stampaBtn_clicked()
{
  if(0==modello.rowCount()) {
    return;
  }

  if(modello.completaOrdine(numOrdineCorrente,importoOrdineCorrente,idSessioneCorrente)) {

      stampaScontrino(numOrdineCorrente);

      if(configurazione->value("abilitaResto").toBool()) {
        int durataSecondi=configurazione->value("durataResto",5).toInt();
        RestoDlg restoDlg(importoOrdineCorrente,durataSecondi,this);
        restoDlg.exec();
      }
      nuovoOrdine(idSessioneCorrente);
  }
}

void Ordine::nuovoOrdine(const int idSessione)
{
  idSessioneCorrente=idSessione;
  importoUltimoOrdine=importoOrdineCorrente;
  modello.clear();
  QSqlQuery query("select max(numero) from ordini");
  if(!query.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                        query.lastError().text());
    return;
  }
  numOrdineCorrente=0;
  if(query.next()) {
    numOrdineCorrente=query.value(0).toInt();
  }
  numOrdineCorrente++;
  numeroOrdineTxt->setText(QString("%L1").arg(numOrdineCorrente));
  totaleLine->setText(QString("%L1").arg(importoOrdineCorrente,4,'f',2));
  if(0==importoUltimoOrdine) {
    importoUltimoOrdineText->setText("--");
  } else {
    importoUltimoOrdineText->setText(QString("%L1").arg(importoUltimoOrdine,4,'f',2));
  }
}

void Ordine::stampaScontrino(const int numeroOrdine)
{
  QString intest=configurazione->value("intestazione").toString();
  QString fondo=configurazione->value("fondo").toString();
  QString nomeCassa=configurazione->value("nomeCassa","000").toString();
  QString descrManifestazione=configurazione->value("descrManifestazione","NOME MANIFESTAZIONE").toString();

  QString intestazione;
  intestazione.append(descrManifestazione).append("\n");
  if(!intest.isEmpty()) {
    intestazione.append(intest).append("\n");
  }

  QPrinter printer;
  bool stampantePdf=configurazione->value("stampantePdf",true).toBool();
  QString stampanteSelezionata=configurazione->value("stampante","PDF").toString();
  if(stampantePdf) {
    QDir cartellaPdf(configurazione->value("cartellaPdf","ticket").toString());
    cartellaPdf.mkpath(cartellaPdf.absolutePath());
    QString pdfString=QString("Cartella pdf: %1").arg(cartellaPdf.absolutePath());
    qDebug(pdfString.toAscii());
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
  fontNormale.setPointSize(10);
  QFont fontGrassetto("lucida console");
  fontGrassetto.setPointSize(14);
  fontGrassetto.setBold(true);
  QFont fontGrassettoCorsivo("lucida console");
  fontGrassettoCorsivo.setPointSize(14);
  fontGrassettoCorsivo.setBold(true);
  fontGrassettoCorsivo.setItalic(true);
  fontGrassettoCorsivo.setUnderline(true);
  QFont fontMini("lucida console");
  fontMini.setPointSize(1);

  painter.begin(&printer);
  painter.setWindow(0,0,pageWidth,pageWidth/rapportoFoglio);

  // stampa scontrini per destinazione

  QSqlQuery stmt;
  stmt.prepare("select distinct(destinazione) from storicoordini where idsessione=? and numeroordine=? and tipoArticolo <> 'M'");
  stmt.addBindValue(idSessioneCorrente);
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

    stmt.prepare("select coalesce(intestazione,nome),stampaflag from destinazionistampa where nome=?");
    stmt.addBindValue(reparto);
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt.lastError().text());
      return;
    }

    QString intestReparto=reparto;
    if(stmt.next()) {
      bool stampaFlag=stmt.value(1).toBool();
      if(!stampaFlag) continue;
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
    painter.drawText(x+pageWidth/2,y,pageWidth/2,100,Qt::AlignRight,QString("ORDINE N. %L1").arg(numeroOrdine),&textRect);
    painter.drawLine(x,y+textRect.height()+5,pageWidth,y+textRect.height()+5);
    y+=10;

    stmt.prepare("SELECT descrizione,sum(quantita) \
                 FROM storicoordini \
                 where idsessione=? \
                 and numeroordine=? \
                 and destinazione=? \
                 group by numeroordine,descrizione");
    stmt.addBindValue(idSessioneCorrente);
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
      QString descrizione=stmt.value(0).toString();
      int quantita=stmt.value(1).toInt();
      numArticoli+=quantita;
      QString quantitaString=QString("%1").arg(quantita,3,10);

      QRect tmpRect;
      painter.setFont(fontNormale);
      painter.drawText(x,y,(pageWidth/10)*2-5,100,Qt::AlignLeft|Qt::AlignTop,quantitaString,&tmpRect);
      painter.drawText(x+(pageWidth/10)*2,y,(pageWidth/10)*7-5,1000,Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,descrizione,&textRect);
    }

    y+=textRect.height()+5;
    painter.drawLine(x,y,pageWidth,y);

    y+=5;
    QString totaleString=QString("TOTALE: %L1 ARTICOLI").arg(numArticoli);
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

  y+=5;
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
  painter.drawText(x,y,pageWidth,100,Qt::AlignHCenter,QDateTime::currentDateTime().toLocalTime().toString("dd-MM-yyyy   hh:mm:ss"),&textRect);
  y+=textRect.height()+5;

  painter.drawText(x,y,200,100,Qt::AlignLeft,QString("CASSA %1").arg(nomeCassa),&textRect);
  painter.drawText(x+pageWidth/2,y,pageWidth/2,100,Qt::AlignRight,QString("ORDINE N. %L1").arg(numeroOrdine),&textRect);
  painter.drawLine(x,y+textRect.height()+5,pageWidth,y+textRect.height()+5);
  y+=10;

  stmt.prepare("select descrizione,quantita,prezzo*quantita from storicoordini where idsessione=? and numeroordine=? and tipoArticolo <> 'C'");
  stmt.addBindValue(idSessioneCorrente);
  stmt.addBindValue(numeroOrdine);

  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  float totale=0;
  while(stmt.next()) {
    y+=textRect.height();
    QString descrizione=stmt.value(0).toString();
    int quantita=stmt.value(1).toInt();
    QString quantitaString=QString("%1").arg(quantita,3,10);
    float prezzo=stmt.value(2).toFloat();
    QString prezzoString=QString("%1 %L2").arg(QChar(0x20AC)).arg(prezzo,5,'f',2);

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
  QString totaleString=QString("TOTALE: %1 %L2").arg(QChar(0x20AC)).arg(totale,5,'f',2);
  painter.setFont(fontGrassetto);
  painter.drawText(x,y,pageWidth,100,Qt::AlignRight,totaleString,&textRect);

  if(!fondo.isEmpty()) {
    y+=textRect.height()+20;
    painter.setFont(fontNormale);
    painter.drawText(x,y,pageWidth,100,Qt::AlignHCenter|Qt::TextWordWrap,fondo,&textRect);
  }

  y+=textRect.height()+35;
  painter.setFont(fontMini);
  painter.drawText(x,y,".");
  painter.end();

}

void Ordine::on_ultimoRestoBtn_clicked()
{
  if(importoUltimoOrdine!=0) {
    RestoDlg restoDlg(importoUltimoOrdine,0,this);
    restoDlg.exec();
  }

}

void Ordine::on_pagPrevBtn_clicked()
{
  int primaRiga=articoliTab->rowAt(1);
  QModelIndex idx=modello.index(primaRiga-1,1);
  articoliTab->scrollTo(idx,QAbstractItemView::PositionAtTop);
}

void Ordine::on_pagNextBtn_clicked()
{
  int primaRiga=articoliTab->rowAt(1);
  QModelIndex idx=modello.index(primaRiga+1,1);
  articoliTab->scrollTo(idx,QAbstractItemView::PositionAtTop);
}

void Ordine::on_duplicaBtn_clicked()
{
  if(isInComposizione()) {
    ConfermaDlg* dlg=new ConfermaDlg("Confermi l'annullamento dell'ordine corrente?","",false,this);
    if(QDialog::Accepted!=dlg->visualizza()) return;
    modello.clear();
  }
  int numeroOrdine=numOrdineCorrente-1;
  QSqlQuery stmt;
  stmt.prepare("select a.idarticolo,a.quantita,b.descrizione,b.prezzo from ordinirighe a,articoli b where a.numeroordine=? and a.idarticolo=b.idarticolo");
  stmt.addBindValue(numeroOrdine);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    int idArticolo=stmt.value(0).toInt();
    int quantita=stmt.value(1).toInt();
    QString descrizione=stmt.value(2).toString();
    float prezzo=stmt.value(3).toFloat();
    while(quantita-->0)
      nuovoArticolo(idArticolo,descrizione,prezzo);
  }
}
