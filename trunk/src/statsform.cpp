#include "statsform.h"
#include "qcustomplot.h"
#include "textprinter.h"

#include <QtSql>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextDocumentFragment>
#include <QTextDocumentWriter>

StatsForm::StatsForm(QMap<QString, QVariant> *par, QWidget *parent) : configurazione(par), QDialog(parent)
{
  idSessioneCorrente=configurazione->value("sessioneCorrente").toInt();

  setupUi(this);
  setWindowFlags(Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint);
  activateWindow();
  showMaximized();

  fromData->setDate(QDate::currentDate());
  toData->setDate(QDate::currentDate());
  statsModel=new StatsModel;
  statsView->setModel(statsModel);

  statsView->verticalHeader()->hide();
  statsView->horizontalHeader()->setSortIndicatorShown(true);

  statsView->setWordWrap(true);
  statsView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  statsView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  //statsView->horizontalHeader()->setStretchLastSection(true);

  connect(statsView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),this,SLOT(ordinaByColumn(int)));

  caricaStats();
}

StatsForm::~StatsForm()
{
  delete statsModel;
}

void StatsForm::on_filtraBtn_clicked()
{
  caricaStats();
}

void StatsForm::ordinaByColumn(int column)
{
  if(column==0) {
    column=2;
  }
  statsView->sortByColumn(column);
}

void StatsForm::caricaStats()
{
  graficoPlot->removePlottable(0);
  QCPBars* barPlot=new QCPBars(graficoPlot->xAxis,graficoPlot->yAxis);
  graficoPlot->addPlottable(barPlot);
  QVector<double> keyData;
  QVector<double> valueData;
  QVector<QString> tickLabelsData;
  QVector<double> tickData;
  int numero=0;

  QString sql("SELECT descrizione,sum(quantita),tsstampa \
              FROM storicoordini \
              where \
              %1 \
              and tipoArticolo <> ? \
              group by descrizione \
              order by 3 asc");
  QString condizione;
  if(ultimaSessioneBox->isChecked()) {
    condizione="idsessione=?";
  } else {
    condizione="datetime(tsstampa) between ? and ?";
  }
  sql=sql.arg(condizione);
  QString tipoArticolo;
  if(expMenuBox->isChecked()) {
      tipoArticolo="M";
  } else {
      tipoArticolo="C";
  }
  QSqlQuery stmt;
  stmt.prepare(sql);

  if(ultimaSessioneBox->isChecked()) {
    stmt.addBindValue(idSessioneCorrente);
  } else {
    QString from=QString("%1 %2").arg(fromData->date().toString("yyyy-MM-dd")).arg(fromOra->time().toString("hh:mm:ss"));
    QString to=QString("%1 %2").arg(toData->date().toString("yyyy-MM-dd")).arg(toOra->time().toString("hh:mm:ss"));
    stmt.addBindValue(from);
    stmt.addBindValue(to);
  }

  stmt.addBindValue(tipoArticolo);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  statsModel->clear();
  tsInizio.clear();
  tsFine.clear();

  while(stmt.next()) {
    if(tsInizio.isEmpty()) {
      tsInizio=stmt.value(2).toDateTime().toString("dd-MM-yyyy hh:mm:ss");
    }
    tsFine=stmt.value(2).toDateTime().toString("dd-MM-yyyy hh:mm:ss");
    QString nomeArticolo=stmt.value(0).toString();
    int quantita=stmt.value(1).toInt();
    QList<QStandardItem*> riga;
    riga.append(new QStandardItem(nomeArticolo));
    QStandardItem* quantitaItem=new QStandardItem;
    quantitaItem->setData(QVariant(quantita),Qt::EditRole);
    riga.append(quantitaItem);
    riga.append(new QStandardItem(nomeArticolo.toUpper()));
    statsModel->appendRow(riga);

    keyData.append(++numero);
    valueData.append(quantita);
    tickLabelsData.append(nomeArticolo);
    tickData.append(numero);
  }

  statsView->hideColumn(2);

  QFont font=graficoPlot->xAxis->tickLabelFont();
  font.setPointSize(8);
  graficoPlot->xAxis->setTickLabelFont(font);
  graficoPlot->xAxis->setAutoTicks(false);
  graficoPlot->xAxis->setAutoTickLabels(false);
  barPlot->setData(keyData,valueData);
  graficoPlot->xAxis->setTickVector(tickData);
  graficoPlot->xAxis->setTickVectorLabels(tickLabelsData);
  graficoPlot->xAxis->setTickLabelRotation(60);
  graficoPlot->xAxis->setSubTickCount(0);
  graficoPlot->xAxis->setTickLength(0, 4);
  graficoPlot->xAxis->grid()->setVisible(false);
  graficoPlot->xAxis->setRange(0, ++numero);
  graficoPlot->xAxis->setPadding(10);
  graficoPlot->plotLayout()->setAutoMargins(QCP::msAll);
  graficoPlot->rescaleAxes();
  graficoPlot->replot();

  graficoPlot->setInteraction(QCP::iRangeDrag,true);
  graficoPlot->setInteraction(QCP::iRangeZoom,true);
  //graficoPlot->setInteraction(QCP::iSelectItems,true);

  statsView->horizontalHeader()->setSortIndicator(1,Qt::DescendingOrder);

  calcolaTotali();
}

void StatsForm::calcolaTotali()
{

  QString sql("select count(*),sum(importoordine) \
              from (SELECT idsessione||numeroordine,importo as importoordine \
                            FROM storicoordini \
                            where \
                            %1 \
                            group by idsessione,numeroordine)");

  QString condizione;
  if(ultimaSessioneBox->isChecked()) {
    condizione="idsessione=?";
  } else {
    condizione="datetime(tsstampa) between ? and ?";
  }

  sql=sql.arg(condizione);

  QSqlQuery stmt;
  stmt.prepare(sql);

  if(ultimaSessioneBox->isChecked()) {
    stmt.addBindValue(idSessioneCorrente);
  } else {
    QString from=QString("%1 %2").arg(fromData->date().toString("yyyy-MM-dd")).arg(fromOra->time().toString("hh:mm:ss"));
    QString to=QString("%1 %2").arg(toData->date().toString("yyyy-MM-dd")).arg(toOra->time().toString("hh:mm:ss"));
    stmt.addBindValue(from);
    stmt.addBindValue(to);
  }

  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  stmt.next();
  totOrdini=stmt.value(0).toInt();
  totImporto=stmt.value(1).toFloat();

  totaleOrdiniTxt->setText(QString("%L1").arg(totOrdini));
  totaleImportoTxt->setText(QString("%1 %L2").arg(QChar(0x20AC)).arg(totImporto,4,'f',2));
}

void StatsForm::on_stampaBtn_clicked(){

  QTextBlockFormat rightAlignFormat;
  rightAlignFormat.setAlignment(Qt::AlignRight);
  QTextBlockFormat centerAlignFormat;
  centerAlignFormat.setAlignment(Qt::AlignCenter);

  QTextDocument* documento=new QTextDocument(this);
  QTextCursor cursoreDoc(documento);
  QString testo;

  cursoreDoc.setBlockFormat(centerAlignFormat);
  cursoreDoc.insertHtml("<center><h2><u>CONDIZIONI</u></h2></center><br>");
  testo=QString("Nome cassa: %1\n").arg(configurazione->value("nomeCassa").toString());
  cursoreDoc.insertText(testo);
  testo=QString("Primo scontrino: %1\n").arg(tsInizio);
  cursoreDoc.insertText(testo);
  testo=QString("Ultimo scontrino: %1\n").arg(tsFine);
  cursoreDoc.insertText(testo);
  testo=QString("Ordini emessi: %L1\n").arg(totOrdini);
  cursoreDoc.insertText(testo);
  testo=QString("Importo incassato: %1 %L2\n\n").arg(QChar(0x20AC)).arg(totImporto,4,'f',2);
  cursoreDoc.insertText(testo);

  cursoreDoc.setBlockFormat(centerAlignFormat);
  cursoreDoc.insertHtml("<center><h2><u>TOTALIZZATORI</u></h2></center><br>");
  testo=QString("Nome cassa: %1\n").arg(configurazione->value("nomeCassa").toString());

  QTextDocument tableDocument;
  QTextCursor cursore(&tableDocument);
  QTextTable* tabella=cursore.insertTable(1,2);

  foreach(testo,((QString)"ARTICOLO,QUANTITA'").split(",")) {
    putHeader(cursore,testo);
    cursore.movePosition(QTextCursor::NextCell);
  }

  for(int i=0;i<statsModel->rowCount();i++) {
    QString descrizioneArt=statsModel->item(i,0)->data(Qt::DisplayRole).toString();
    QString quantita=statsModel->item(i,1)->data(Qt::DisplayRole).toString();
    tabella->appendRows(1);
    cursore.movePosition(QTextCursor::PreviousCell,QTextCursor::MoveAnchor,1);
    cursore.insertText(descrizioneArt);
    cursore.movePosition(QTextCursor::NextCell);
    cursore.setBlockFormat(rightAlignFormat);
    cursore.insertText(quantita);
  }

  cursoreDoc.insertFragment(QTextDocumentFragment(&tableDocument));
  //QString descrReport=QString("Totali di cassa -- CASSA %1 -- &date;").arg(configurazione->value("nomeCassa").toString());
  QString descrReport=QString("Totali di cassa - &date;");
  stampa(documento,descrReport,false);

}

void StatsForm::stampa(const QTextDocument *doc, const QString descrReport, bool preview)
{

  TextPrinter* tprinter=new TextPrinter(this);
  QString header=QString("<center><b>%1<br>%2</b></center>").arg(configurazione->value("descrManifestazione").toString()).arg(descrReport);
  tprinter->setHeaderText(header);
  tprinter->setFooterText("<center><b>GESTIONE CASSA</b><br>Pag. &page;</center>");
  tprinter->setHeaderSize(5);
  tprinter->setFooterSize(10);
  tprinter->setDateFormat("d MMM yyyy h:mm:ss");

  if(preview) {
    tprinter->preview(doc);
  } else {
    tprinter->exportPdf(doc,"Salva con nome");
  }

}

void StatsForm::putHeader(QTextCursor cursore, const QString testo)
{
  QTextBlockFormat format;
  format.setAlignment(Qt::AlignHCenter);
  QTextCharFormat charFormat=cursore.charFormat();
  QFont font=cursore.document()->defaultFont();
  font.setPointSize(font.pointSize()+2);
  font.setBold(true);
  charFormat.setFont(font);
  cursore.setBlockFormat(format);
  cursore.setCharFormat(charFormat);
  cursore.insertText(testo);
}

void StatsForm::formattaTabella(QTextTable* tabella)
{
  QTextTableFormat formatoTabella;
  formatoTabella.setBorder(1);
  formatoTabella.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
  formatoTabella.setHeaderRowCount(1);
  formatoTabella.setCellSpacing(-1);
  formatoTabella.setCellPadding(3);
  formatoTabella.setWidth(QTextLength(QTextLength::PercentageLength,100));

  tabella->setFormat(formatoTabella);

}

