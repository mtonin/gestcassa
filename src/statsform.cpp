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

StatsForm::StatsForm(int idSessione, QMap<QString, QVariant> *par, QWidget *parent) : idSessioneCorrente(idSessione), configurazione(par), QDialog(parent)
{
    setupUi(this);
    //graficoVScrollBar->setVisible(false);
    //graficoHScrollBar->setVisible(false);
    setWindowState(Qt::WindowMaximized);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    activateWindow();
    showFullScreen();

    fromData->setDate(QDate::currentDate());
    toData->setDate(QDate::currentDate());
    statsModel = new StatsModel;
    statsView->setModel(statsModel);

    statsView->verticalHeader()->hide();
    statsView->horizontalHeader()->setSortIndicatorShown(true);

    statsView->setWordWrap(true);
    statsView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    statsView->horizontalHeader()->setStretchLastSection(false);

    statsView->setAlternatingRowColors(true);

    connect(statsView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(ordinaByColumn(int)));
    //connect(graficoPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseClick(QMouseEvent*)));
    connect(graficoPlot->xAxis, SIGNAL(rangeChanged(QCPRange, QCPRange)), this, SLOT(xCheckRange(QCPRange, QCPRange)));
    connect(graficoPlot->yAxis, SIGNAL(rangeChanged(QCPRange, QCPRange)), this, SLOT(yCheckRange(QCPRange, QCPRange)));

    caricaSessioni();
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
    if (column == 0) {
        column = 2;
    }
    statsView->sortByColumn(column);
}

void StatsForm::caricaStats()
{
    QString sql("SELECT descrizione,sum(quantita),max(tsstampa) \
              FROM storicoordini \
              where \
              %1 \
              and lower(descrizione) like ? \
              and tipoArticolo <> ? \
              group by descrizione \
              order by 2 asc");
    QString condizione;
    if (sessioneBox->isChecked()) {
        condizione = "idsessione=?";
    } else {
        condizione = "tsstampa between ? and ?";
    }
    sql = sql.arg(condizione);
    QString tipoArticolo;
    if (expMenuBox->isChecked()) {
        tipoArticolo = "M";
    } else {
        tipoArticolo = "C";
    }
    QSqlQuery stmt;
    if (!stmt.prepare(sql)) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }

    if (sessioneBox->isChecked()) {
        int sessioneSelezionata=sessioneCombo->model()->index(sessioneCombo->currentIndex(),1).data().toInt();
        stmt.addBindValue(sessioneSelezionata);
    } else {
        QString from = QString("%1 %2").arg(fromData->date().toString("yyyy-MM-dd")).arg(fromOra->time().toString("hh:mm:ss"));
        QString to = QString("%1 %2").arg(toData->date().toString("yyyy-MM-dd")).arg(toOra->time().toString("hh:mm:ss"));
        stmt.addBindValue(from);
        stmt.addBindValue(to);
    }

    QString filtroNome=QString("%%%1%%").arg(filtroArticoloLbl->text().toLower());
    stmt.addBindValue(filtroNome);

    stmt.addBindValue(tipoArticolo);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    statsModel->clear();

    graficoPlot->clearItems();
    graficoPlot->clearPlottables();

    QVector<double> keyData1;
    QVector<double> valueData1;
    QVector<double> keyData2;
    QVector<double> valueData2;
    QVector<double> keyData3;
    QVector<double> valueData3;
    QVector<double> keyData4;
    QVector<double> valueData4;
    QVector<double> keyData5;
    QVector<double> valueData5;
    QVector<QString> tickLabelsData;
    QVector<double> tickData;
    int counter = 0;
    int maxValore = 0;
    while (stmt.next()) {
        QString nomeArticolo = stmt.value(0).toString().simplified();
        int quantita = stmt.value(1).toInt();
        if (quantita > maxValore)
            maxValore = quantita;
        QList<QStandardItem*> riga;
        riga.append(new QStandardItem(nomeArticolo));
        QStandardItem* quantitaItem = new QStandardItem;
        quantitaItem->setData(QVariant(quantita), Qt::EditRole);
        riga.append(quantitaItem);
        riga.append(new QStandardItem(nomeArticolo.toUpper()));
        statsModel->appendRow(riga);

        int resto = counter % 5;
        switch (resto) {
        case 0:
            keyData1.append(++counter);
            valueData1.append(quantita);
            break;
        case 1:
            keyData2.append(++counter);
            valueData2.append(quantita);
            break;
        case 2:
            keyData3.append(++counter);
            valueData3.append(quantita);
            break;
        case 3:
            keyData4.append(++counter);
            valueData4.append(quantita);
            break;
        case 4:
            keyData5.append(++counter);
            valueData5.append(quantita);
            break;
        }

        tickLabelsData.append(nomeArticolo);
        tickData.append(counter);
        QString testo = QString("%1").arg(quantita);
        impostaLabel(testo, graficoPlot, counter, quantita);
    }

    statsView->hideColumn(2);
    statsView->horizontalHeader()->setSortIndicator(1, Qt::DescendingOrder);
    if (counter > 0) {
        statsView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    }

    calcolaTotali();

    maxXAxis = maxValore + 20;
    maxYAxis = counter + 2;

    QFont font = graficoPlot->yAxis->tickLabelFont();
    font.setPointSize(8);
    graficoPlot->yAxis->setTickLabelFont(font);
    graficoPlot->yAxis->setAutoTicks(false);
    graficoPlot->yAxis->setAutoTickLabels(false);
    graficoPlot->yAxis->setTickVector(tickData);
    graficoPlot->yAxis->setTickVectorLabels(tickLabelsData);
    graficoPlot->yAxis->setSubTickCount(0);
    graficoPlot->yAxis->setTickLength(0, 4);
    graficoPlot->yAxis->grid()->setVisible(false);
    graficoPlot->yAxis->setRange(0, maxYAxis);
    graficoPlot->xAxis->setRange(0, maxXAxis);
    graficoPlot->axisRect()->setAutoMargins(QCP::msLeft);
    graficoPlot->axisRect()->setMargins(QMargins(100, 5, 5, 50));
    graficoPlot->rescaleAxes();

    graficoPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    creaGrafico(Qt::red, keyData1, valueData1);
    creaGrafico(Qt::yellow, keyData2, valueData2);
    creaGrafico(Qt::blue, keyData3, valueData3);
    creaGrafico(Qt::green, keyData4, valueData4);
    creaGrafico(Qt::magenta, keyData5, valueData5);

    graficoPlot->replot();

    graficoHScrollBar->setRange(0,maxXAxis);
    graficoVScrollBar->setRange(0,maxYAxis);
}

void StatsForm::creaGrafico(const QColor &colore, const QVector<double> chiavi, const QVector<double> valori)
{
    QCPBars* barPlot = new QCPBars(graficoPlot->yAxis, graficoPlot->xAxis);
    graficoPlot->addPlottable(barPlot);
    barPlot->setBrush(colore);
    barPlot->setData(chiavi, valori);

}

void StatsForm::calcolaTotali()
{

    QString sql("select count(*),sum(importoordine),min(minTs),max(maxTs) \
              from (SELECT idcassa||idsessione||numeroordine as keyordine,importo as importoordine, min(tsstampa) as minTs,max(tsstampa) as maxTs \
                            FROM storicoordini \
                            where \
                            %1 \
                            and lower(descrizione) like ? \
                            group by keyordine,importo)");

    QString condizione;
    if (sessioneBox->isChecked()) {
        condizione = "idsessione=?";
    } else {
        condizione = "tsstampa between ? and ?";
    }

    sql = sql.arg(condizione);

    QSqlQuery stmt;
    if (!stmt.prepare(sql)) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }

    if (sessioneBox->isChecked()) {
      int sessioneSelezionata=sessioneCombo->model()->index(sessioneCombo->currentIndex(),1).data().toInt();
      stmt.addBindValue(sessioneSelezionata);
    } else {
        QString from = QString("%1 %2").arg(fromData->date().toString("yyyy-MM-dd")).arg(fromOra->time().toString("hh:mm:ss"));
        QString to = QString("%1 %2").arg(toData->date().toString("yyyy-MM-dd")).arg(toOra->time().toString("hh:mm:ss"));
        stmt.addBindValue(from);
        stmt.addBindValue(to);
    }

    QString filtroNome=QString("%%%1%%").arg(filtroArticoloLbl->text().toLower());
    stmt.addBindValue(filtroNome);

    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    stmt.next();
    totOrdini = stmt.value(0).toInt();
    totImporto = stmt.value(1).toFloat();

    totaleOrdiniTxt->setText(QString("%L1").arg(totOrdini));
    totaleImportoTxt->setText(QString("%1 %L2").arg(QChar(0x20AC)).arg(totImporto, 4, 'f', 2));

    tsInizio = stmt.value(2).toDateTime().toString("dd/MM/yyyy hh:mm:ss");
    tsFine = stmt.value(3).toDateTime().toString("dd/MM/yyyy hh:mm:ss");

    ordineDataFrom->setText(tsInizio);
    ordineDataTo->setText(tsFine);

}

void StatsForm::caricaSessioni()
{

  QSqlQueryModel* sessioniModel=new QSqlQueryModel(this);
  sessioniModel->setQuery("SELECT idsessione || ' (Inizio: ' ||     \
  substring(100+extract(day from tsinizio) from 2 for 2)||'/'||       \
  substring(100+extract(month from tsinizio) from 2 for 2)||'/'||   \
  extract(year from tsinizio) || ' ' ||                                            \
  substring(100+extract(hour from tsinizio) from 2 for 2)||':'||      \
  substring(100+extract(minute from tsinizio) from 2 for 2)||':'||   \
  substring(100+extract(second from tsinizio) from 2 for 2) || ')',  \
  idsessione \
  FROM sessione order by idsessione desc"  );

  QTableView* sessioneView=new QTableView(this);
  sessioneCombo->setView(sessioneView);
  sessioneView->horizontalHeader()->hide();
  sessioneView->verticalHeader()->hide();
  sessioneView->setSelectionBehavior(QAbstractItemView::SelectRows);
  sessioneView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  sessioneView->setShowGrid(false);

  sessioneCombo->setModel(sessioniModel);
  sessioneView->hideColumn(1);

}

void StatsForm::on_stampaBtn_clicked()
{

    QTextBlockFormat rightAlignFormat;
    rightAlignFormat.setAlignment(Qt::AlignRight);
    QTextBlockFormat centerAlignFormat;
    centerAlignFormat.setAlignment(Qt::AlignCenter);

    QTextDocument* documento = new QTextDocument(this);
    QTextCursor cursoreDoc(documento);
    QString testo;

    cursoreDoc.setBlockFormat(centerAlignFormat);
    cursoreDoc.insertHtml("<center><h2><u>CONDIZIONI</u></h2></center><br>");
    testo = QString("Nome cassa: %1\n").arg(configurazione->value("nomeCassa").toString());
    cursoreDoc.insertText(testo);
    testo = QString("Primo scontrino: %1\n").arg(tsInizio);
    cursoreDoc.insertText(testo);
    testo = QString("Ultimo scontrino: %1\n").arg(tsFine);
    cursoreDoc.insertText(testo);
    testo = QString("Ordini emessi: %L1\n").arg(totOrdini);
    cursoreDoc.insertText(testo);
    testo = QString("Importo incassato: %1 %L2\n\n").arg(QChar(0x20AC)).arg(totImporto, 4, 'f', 2);
    cursoreDoc.insertText(testo);

    cursoreDoc.setBlockFormat(centerAlignFormat);
    cursoreDoc.insertHtml("<center><h2><u>TOTALIZZATORI</u></h2></center><br>");
    testo = QString("Nome cassa: %1\n").arg(configurazione->value("nomeCassa").toString());

    QTextDocument tableDocument;
    QTextCursor cursore(&tableDocument);
    QTextTable* tabella = cursore.insertTable(1, 2);

    foreach(testo, ((QString)"ARTICOLO,QUANTITA'").split(",")) {
        putHeader(cursore, testo);
        cursore.movePosition(QTextCursor::NextCell);
    }

    for (int i = 0; i < statsModel->rowCount(); i++) {
        QString descrizioneArt = statsModel->item(i, 0)->data(Qt::DisplayRole).toString();
        QString quantita = statsModel->item(i, 1)->data(Qt::DisplayRole).toString();
        tabella->appendRows(1);
        cursore.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 1);
        cursore.insertText(descrizioneArt);
        cursore.movePosition(QTextCursor::NextCell);
        cursore.setBlockFormat(rightAlignFormat);
        cursore.insertText(quantita);
    }

    cursoreDoc.insertFragment(QTextDocumentFragment(&tableDocument));
    //QString descrReport=QString("Totali di cassa -- CASSA %1 -- &date;").arg(configurazione->value("nomeCassa").toString());
    QString descrReport = QString("Totali di cassa - &date;");
    stampa(documento, descrReport, false);

}

void StatsForm::stampa(const QTextDocument *doc, const QString descrReport, bool preview)
{

    TextPrinter* tprinter = new TextPrinter(this);
    QString header = QString("<center><b>%1<br>%2</b></center>").arg(configurazione->value("descrManifestazione").toString()).arg(descrReport);
    tprinter->setHeaderText(header);
    tprinter->setFooterText("<center><b>GESTIONE CASSA</b><br>Pag. &page;</center>");
    tprinter->setHeaderSize(5);
    tprinter->setFooterSize(10);
    tprinter->setDateFormat("d MMM yyyy h:mm:ss");

    if (preview) {
        tprinter->preview(doc);
    } else {
        tprinter->exportPdf(doc, "Salva con nome");
    }

}

void StatsForm::putHeader(QTextCursor cursore, const QString testo)
{
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignHCenter);
    QTextCharFormat charFormat = cursore.charFormat();
    QFont font = cursore.document()->defaultFont();
    font.setPointSize(font.pointSize() + 2);
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
    formatoTabella.setWidth(QTextLength(QTextLength::PercentageLength, 100));

    tabella->setFormat(formatoTabella);

}

void StatsForm::impostaLabel(QString valore, QCustomPlot* customPlot, qreal x, qreal y)
{

    QCPItemText *textLabel = new QCPItemText(customPlot);
    customPlot->addItem(textLabel);
    textLabel->setPositionAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    textLabel->setPadding(QMargins(3, 0, 0, 0));
    textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    textLabel->position->setCoords(y, x);
    textLabel->setText(valore);
    textLabel->setFont(QFont(font().family(), 10));
}

void StatsForm::xCheckRange(const QCPRange &newRange, const QCPRange& oldRange)
{
    if (newRange.lower < 0 || newRange.upper > maxXAxis)
        graficoPlot->xAxis->setRange(oldRange);
    else
        xAxisChanged(newRange);
}

void StatsForm::yCheckRange(const QCPRange &newRange, const QCPRange &oldRange)
{
    if (newRange.lower < 0 || newRange.upper > maxYAxis)
        graficoPlot->yAxis->setRange(oldRange);
    else
        yAxisChanged(newRange);

}

void StatsForm::mouseClick(QMouseEvent *evt)
{
    QCPRange xRange = graficoPlot->xAxis->range();
    QCPRange yRange = graficoPlot->yAxis->range();
    if (Qt::LeftButton == evt->button()) {
        xRange.upper -= 10;
        yRange.upper -= 10;
    } else {
        xRange.upper += 10;
        yRange.upper += 10;
    }
    graficoPlot->xAxis->setRange(xRange);
    graficoPlot->yAxis->setRange(yRange);
    graficoPlot->replot();
}

void StatsForm::on_sessioneBox_toggled(bool checked)
{
    sessioneCombo->setEnabled(checked);
}

void StatsForm::xAxisChanged(QCPRange range)
{
    graficoHScrollBar->setMaximum(maxXAxis-qRound(range.size()));
    graficoHScrollBar->setValue(qRound(range.lower)); // adjust position of scroll bar slider
    graficoHScrollBar->setPageStep(qRound(range.size())); // adjust size of scroll bar slider
}

void StatsForm::yAxisChanged(QCPRange range)
{
    graficoVScrollBar->setMaximum(maxYAxis-qRound(range.size()));
    graficoVScrollBar->setValue(maxYAxis-qRound(range.lower)-qRound(range.size())); // adjust position of scroll bar slider
    graficoVScrollBar->setPageStep(qRound(range.size())); // adjust size of scroll bar slider
}
