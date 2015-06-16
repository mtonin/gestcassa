#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextDocumentFragment>
#include <QTextDocumentWriter>
#include <QMessageBox>
#include <QtSql>
#include "textprinter.h"
#include "buonidlg.h"

const QString QUERYALL("select cognome || ' ' || nome, tsemissione,flagannullato,cognome,nome from buoni order by tsemissione");
const QString QUERYNONANNULLATI("select cognome || ' ' || nome, tsemissione,flagannullato,cognome,nome from buoni where flagannullato=0 order by tsemissione");

BuoniDlg::BuoniDlg(QMap<QString, QVariant> *par, QWidget *parent) :configurazione(par),QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::Tool);
  activateWindow();

  modello = new buoniQueryModel(this);
  modello->setQuery(QUERYALL);
  buoniTable->setModel(modello);

  totaleTxt->setText(QString("%1").arg(modello->rowCount()));
  buoniTable->hideColumn(3);
  buoniTable->hideColumn(4);
  buoniTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  buoniTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);

  connect(modello,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(aggiornaUI()));
}

void BuoniDlg::on_nuovoBuonoBtn_clicked()
{
  QString msg;
  QString cognome=cognomeTxt->text().toUpper();
  QString nome=nomeTxt->text().toUpper();

  if(cognome.isEmpty() || nome.isEmpty()) {
    QMessageBox::critical(this,"Errore","E' obbligatorio indicare nome e cognome");
    return;
  }

  QString query("insert into buoni (cognome, nome, tsemissione,flagannullato) values(?,?,?,0)");
  QSqlQuery sql;
  if(!sql.prepare(query)) {
    QSqlError errore=sql.lastError();
    msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
    QMessageBox::critical(this,"Errore",msg);
    return;
  }
  sql.addBindValue(cognome);
  sql.addBindValue(nome);
  QDateTime ts = QDateTime::currentDateTime();
  sql.addBindValue(ts.toString("yyyy-MM-dd hh:mm:ss"));
  if(!sql.exec()) {
    QSqlError errore=sql.lastError();
    if(-803==errore.number()) {
      msg="Nominativo già presente.";
    } else {
      msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.text());
    }
    QMessageBox::critical(this,"Errore",msg);
    return;
  }
  msg=QString("Emesso il buono intestato a %2 %3").arg(cognome).arg(nome);
  QMessageBox::information(this,"Nuovo buono",msg);

  cognomeTxt->setText("");
  nomeTxt->setText("");

  aggiornaUI();
}

void BuoniDlg::on_tuttiBox_clicked()
{
  aggiornaUI();
}

void BuoniDlg::on_nonAnnullatiBox_clicked()
{
  aggiornaUI();
}

void BuoniDlg::aggiornaUI()
{
  if(tuttiBox->isChecked())
    modello->setQuery(QUERYALL);
  else
    modello->setQuery(QUERYNONANNULLATI);
  totaleTxt->setText(QString("%1").arg(modello->rowCount()));

}

void BuoniDlg::on_stampaBtn_clicked()
{
  QTextBlockFormat rightAlignFormat;
  rightAlignFormat.setAlignment(Qt::AlignRight);
  QTextBlockFormat centerAlignFormat;
  centerAlignFormat.setAlignment(Qt::AlignCenter);

  QTextDocument* documento = new QTextDocument(this);
  QTextCursor cursoreDoc(documento);
  QString testo;

  cursoreDoc.setBlockFormat(centerAlignFormat);
  cursoreDoc.insertHtml("<center><h2><u>ELENCO BUONI</u></h2></center><br>");

  QTextDocument tableDocument;
  QTextCursor cursore(&tableDocument);
  QTextTable* tabella = cursore.insertTable(1, 3);

  foreach(testo, ((QString)"NUMERO,NOMINATIVO,EMESSO IL").split(",")) {
      putHeader(cursore, testo);
      cursore.movePosition(QTextCursor::NextCell);
  }

  int num=0;
  for (int i = 0; i < modello->rowCount(); i++) {
      num++;
      QString nominativo = modello->index(i, 0).data(Qt::DisplayRole).toString();
      QDateTime tsEmissione = modello->index(i, 1).data(Qt::DisplayRole).toDateTime();
      tabella->appendRows(1);
      cursore.movePosition(QTextCursor::PreviousCell,QTextCursor::MoveAnchor,2);
      cursore.setBlockFormat(rightAlignFormat);
      cursore.insertText(QString("%1").arg(num));
      cursore.movePosition(QTextCursor::NextCell);
      cursore.insertText(nominativo);
      cursore.movePosition(QTextCursor::NextCell);
      cursore.insertText(tsEmissione.toString("dd/MM/yyyy hh:mm:ss"));
  }

  formattaTabella(tabella, QString("20,50,30").split(",").toVector());
  cursoreDoc.insertFragment(QTextDocumentFragment(&tableDocument));

  QString descrReport = QString("Buoni emessi - &date;");
  stampa(documento, descrReport, false);

}

void BuoniDlg::stampa(const QTextDocument *doc, const QString descrReport, bool preview)
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

void BuoniDlg::putHeader(QTextCursor cursore, const QString testo)
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

void BuoniDlg::formattaTabella(QTextTable* tabella, QVector<QString> colWidth)
{
    QTextTableFormat formatoTabella;
    formatoTabella.setBorder(1);
    formatoTabella.setBorderBrush(QBrush(Qt::black));
    formatoTabella.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    formatoTabella.setHeaderRowCount(2);
    formatoTabella.setCellSpacing(0);
    formatoTabella.setCellPadding(3);
    formatoTabella.setWidth(QTextLength(QTextLength::PercentageLength, 100));

    QVector<QTextLength> colLen;
    foreach(QString width, colWidth) {
        colLen.append(QTextLength(QTextLength::PercentageLength, width.toInt()));
    }
    formatoTabella.setColumnWidthConstraints(colLen);

    tabella->setFormat(formatoTabella);

}
