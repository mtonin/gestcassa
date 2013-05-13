#include "reportform.h"
#include "textprinter.h"

#include <QPainter>
#include <QtSql>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextDocumentFragment>
#include <QMessageBox>

ReportForm::ReportForm(QMap<QString,QVariant>* par,QWidget *parent) : configurazione(par), QDialog(parent)
{
  setupUi(this);
}

void ReportForm::stampa(bool preview)
{
  QTextDocument* doc;
  if(noSuddivisioneBox->isChecked()) {
    doc=creaDocumentTutto();
    stampa(doc,"Inventario articoli al &date;",preview);
  }
  if(repartiBox->isChecked()) {
    doc=creaDocumentPerReparti();
    stampa(doc,"Inventario articoli per reparti al &date;",preview);
  }
  if(destinazioneBox->isChecked()) {
    doc=creaDocumentPerDestinazione();
    stampa(doc,"Inventario articoli per destinazione stampa al &date;",preview);
  }

}

QTextDocument* ReportForm::creaDocumentTutto()
{
  QTextDocument* documento=new QTextDocument(this);
  QTextCursor cursore(documento);
  QTextTable* tabella=cursore.insertTable(1,4);
  QTextTableFormat formatoTabella;
  QString testo;

  foreach(testo,((QString)"ARTICOLO,PREZZO,REPARTO,DESTINAZIONE").split(",")) {
    putHeader(cursore,testo);
    cursore.movePosition(QTextCursor::NextCell);
  }

  QString sql="select a.descrizione,a.prezzo,a.destinazione,b.descrizione as reparto from articoli a,reparti b on a.idreparto=b.idreparto";

  if(ordineAlfabeticoBox->isChecked()) {
    sql.append(" order by a.descrizione asc");
  } else {
    sql.append(" order by a.prezzo asc");
  }

  QSqlQuery stmt(sql);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return NULL;
  }

  int posDescrizione=stmt.record().indexOf("descrizione");
  int posPrezzo=stmt.record().indexOf("prezzo");
  int posDestinazione=stmt.record().indexOf("destinazione");
  int posReparto=stmt.record().indexOf("reparto");
  QTextBlockFormat rightAlignFormat;
  rightAlignFormat.setAlignment(Qt::AlignRight);

  while(stmt.next()) {
    QString descrizione=stmt.value(posDescrizione).toString();
    QString prezzo=QString("%1 %2").arg(QChar(0x20AC)).arg(stmt.value(posPrezzo).toFloat(),4,'f',2);
    QString destinazione=stmt.value(posDestinazione).toString();
    QString reparto=stmt.value(posReparto).toString();

    tabella->appendRows(1);
    cursore.movePosition(QTextCursor::PreviousCell,QTextCursor::MoveAnchor,3);
    cursore.insertText(descrizione);
    cursore.movePosition(QTextCursor::NextCell);

    cursore.setBlockFormat(rightAlignFormat);
    cursore.insertText(prezzo);
    cursore.movePosition(QTextCursor::NextCell);
    cursore.insertText(reparto);
    cursore.movePosition(QTextCursor::NextCell);
    cursore.insertText(destinazione);
  }


  formatoTabella.setBorder(1);
  formatoTabella.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
  formatoTabella.setHeaderRowCount(1);
  formatoTabella.setCellSpacing(-1);
  formatoTabella.setCellPadding(3);
  formatoTabella.setWidth(QTextLength(QTextLength::PercentageLength,100));

  tabella->setFormat(formatoTabella);

  return documento;

}

QTextDocument* ReportForm::creaDocumentPerReparti()
{
  QTextDocument* documento=new QTextDocument(this);
  QTextCursor cursore(documento);
  QTextTableFormat formatoTabella;
  QString testo;

  QString sql="select idreparto,descrizione from reparti";
  QSqlQuery stmtReparti(sql);
  if(!stmtReparti.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmtReparti.lastError().text());
    return NULL;
  }
  int totReparti=0;
  while(stmtReparti.next()) {
    if(totReparti>0) {
      cursore.insertText("\n\n\n\n\n");
    }
    totReparti++;
    int idReparto=stmtReparti.value(0).toInt();
    QString reparto=stmtReparti.value(1).toString();
    testo=QString("REPARTO: %1").arg(reparto);
    putHeader(cursore,testo);
    cursore.movePosition(QTextCursor::NextRow);

    sql="select a.descrizione,a.prezzo,a.destinazione from articoli a,reparti b on a.idreparto=b.idreparto and a.idreparto=?";
    if(ordineAlfabeticoBox->isChecked()) {
      sql.append(" order by a.descrizione asc");
    } else {
      sql.append(" order by a.prezzo asc");
    }

    QSqlQuery stmt;
    stmt.prepare(sql);
    stmt.addBindValue(idReparto);
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      return NULL;
    }

    int totArticoli=0;

    int posDescrizione=stmt.record().indexOf("descrizione");
    int posPrezzo=stmt.record().indexOf("prezzo");
    int posDestinazione=stmt.record().indexOf("destinazione");

    QTextDocument tableDocument;
    QTextCursor tableCursore(&tableDocument);

    QTextTable* tabella=tableCursore.insertTable(1,3);
    foreach(testo,((QString)"ARTICOLO,PREZZO,DESTINAZIONE").split(",")) {
      putHeader(tableCursore,testo);
      tableCursore.movePosition(QTextCursor::NextCell);
    }

    QTextBlockFormat rightAlignFormat;
    rightAlignFormat.setAlignment(Qt::AlignRight);

    while(stmt.next()) {
      totArticoli++;
      QString descrizione=stmt.value(posDescrizione).toString();
      QString prezzo=QString("%1 %2").arg(QChar(0x20AC)).arg(stmt.value(posPrezzo).toFloat(),4,'f',2);
      QString destinazione=stmt.value(posDestinazione).toString();

      tabella->appendRows(1);
      tableCursore.movePosition(QTextCursor::PreviousCell,QTextCursor::MoveAnchor,2);
      tableCursore.insertText(descrizione);
      tableCursore.movePosition(QTextCursor::NextCell);

      tableCursore.setBlockFormat(rightAlignFormat);
      tableCursore.insertText(prezzo);
      tableCursore.movePosition(QTextCursor::NextCell);
      tableCursore.insertText(destinazione);
    }

    formatoTabella.setBorder(1);
    formatoTabella.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    formatoTabella.setHeaderRowCount(1);
    formatoTabella.setCellSpacing(-1);
    formatoTabella.setCellPadding(3);
    formatoTabella.setWidth(QTextLength(QTextLength::PercentageLength,100));

    tabella->setFormat(formatoTabella);

    if(0==totArticoli){
      cursore.insertText("\nNessun articolo in questo reparto.");
    } else {
      cursore.insertFragment(QTextDocumentFragment(&tableDocument));

    }
  }

  return documento;

}

QTextDocument* ReportForm::creaDocumentPerDestinazione()
{
  QTextDocument* documento=new QTextDocument(this);
  QTextCursor cursore(documento);
  QTextTableFormat formatoTabella;
  QString testo;

  QString sql="select nome from destinazionistampa";
  QSqlQuery stmtDestinazioni(sql);
  if(!stmtDestinazioni.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmtDestinazioni.lastError().text());
    return NULL;
  }
  int totDestinazioni=0;
  while(stmtDestinazioni.next()) {
    if(totDestinazioni>0) {
      cursore.insertText("\n\n\n\n\n");
    }
    totDestinazioni++;
    QString nomeDestinazione=stmtDestinazioni.value(0).toString();
    testo=QString("DESTINAZIONE DI STAMPA: %1").arg(nomeDestinazione);
    putHeader(cursore,testo);
    cursore.movePosition(QTextCursor::NextRow);

    sql="select a.descrizione,a.prezzo,b.descrizione as reparto from articoli a,reparti b on a.idreparto=b.idreparto and a.destinazione=?";
    if(ordineAlfabeticoBox->isChecked()) {
      sql.append(" order by a.descrizione asc");
    } else {
      sql.append(" order by a.prezzo asc");
    }

    QSqlQuery stmt;
    stmt.prepare(sql);
    stmt.addBindValue(nomeDestinazione);
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      return NULL;
    }

    int totArticoli=0;

    int posDescrizione=stmt.record().indexOf("descrizione");
    int posPrezzo=stmt.record().indexOf("prezzo");
    int posReparto=stmt.record().indexOf("reparto");

    QTextDocument tableDocument;
    QTextCursor tableCursore(&tableDocument);

    QTextTable* tabella=tableCursore.insertTable(1,3);
    foreach(testo,((QString)"ARTICOLO,PREZZO,REPARTO").split(",")) {
      putHeader(tableCursore,testo);
      tableCursore.movePosition(QTextCursor::NextCell);
    }

    QTextBlockFormat rightAlignFormat;
    rightAlignFormat.setAlignment(Qt::AlignRight);

    while(stmt.next()) {
      totArticoli++;
      QString descrizione=stmt.value(posDescrizione).toString();
      QString prezzo=QString("%1 %2").arg(QChar(0x20AC)).arg(stmt.value(posPrezzo).toFloat(),4,'f',2);
      QString reparto=stmt.value(posReparto).toString();

      tabella->appendRows(1);
      tableCursore.movePosition(QTextCursor::PreviousCell,QTextCursor::MoveAnchor,2);
      tableCursore.insertText(descrizione);
      tableCursore.movePosition(QTextCursor::NextCell);

      tableCursore.setBlockFormat(rightAlignFormat);
      tableCursore.insertText(prezzo);
      tableCursore.movePosition(QTextCursor::NextCell);
      tableCursore.insertText(reparto);
    }

    formatoTabella.setBorder(1);
    formatoTabella.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    formatoTabella.setHeaderRowCount(1);
    formatoTabella.setCellSpacing(-1);
    formatoTabella.setCellPadding(3);
    formatoTabella.setWidth(QTextLength(QTextLength::PercentageLength,100));

    tabella->setFormat(formatoTabella);

    if(0==totArticoli){
      cursore.insertText("\nNessun articolo in questa destinazione.");
    } else {
      cursore.insertFragment(QTextDocumentFragment(&tableDocument));

    }
  }

  return documento;
}

void ReportForm::stampa(const QTextDocument *doc, const QString descrReport, bool preview)
{

  TextPrinter* tprinter=new TextPrinter(this);
  QString header=QString("<center><b>%1 - %2</b></center>").arg(configurazione->value("descrManifestazione").toString()).arg(descrReport);
  tprinter->setHeaderText(header);
  tprinter->setFooterText("<center><b>GESTIONE CASSA</b><br>Pag. &page;</center>");
  tprinter->setHeaderSize(5);
  tprinter->setFooterSize(10);
  tprinter->setDateFormat("d MMM yyyy");

  if(preview) {
    tprinter->preview(doc);
  } else {
    tprinter->exportPdf(doc,"Salva con nome");
  }

}

void ReportForm::putHeader(QTextCursor cursore, const QString testo)
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

void ReportForm::on_anteprimaBtn_clicked()
{
  stampa(true);
}

void ReportForm::on_esportaBtn_clicked()
{
  stampa(false);
}
