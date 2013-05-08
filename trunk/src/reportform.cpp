#include "reportform.h"

#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QtSql>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextDocumentFragment>

ReportForm::ReportForm(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
}

void ReportForm::on_stampaBtn_clicked()
{
  QString pdfFileName=QFileDialog::getSaveFileName(0,"Crea nuovo report","c:\\","*.pdf");
  if(pdfFileName.isEmpty()) {
    return;
  }
  if(noSuddivisioneBox->isChecked()) {
    stampaTutto(pdfFileName);
  }
  if(repartiBox->isChecked()) {
    stampaPerReparti(pdfFileName);
  }
  if(destinazioneBox->isChecked()) {
    stampaPerDestinazione(pdfFileName);
  }

}

void ReportForm::stampaTutto(const QString& nomeFile)
{
  QFile f(nomeFile);
  QTextDocument documento;
  QTextCursor cursore(&documento);
  cursore.setPosition(QTextCursor::Start);
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
    return;
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

  QPrinter pdfPrinter;
  pdfPrinter.setOutputFileName(nomeFile);
  documento.print(&pdfPrinter);

}

void ReportForm::stampaPerReparti(const QString &nomeFile)
{
  QFile f(nomeFile);
  QTextDocument documento;
  QTextCursor cursore(&documento);
  cursore.setPosition(QTextCursor::Start);
  QTextTableFormat formatoTabella;
  QString testo;

  QString sql="select idreparto,descrizione from reparti";
  QSqlQuery stmtReparti(sql);
  if(!stmtReparti.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmtReparti.lastError().text());
    return;
  }
  while(stmtReparti.next()) {
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
      return;
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
    cursore.insertText("\n\n\n\n\n");
  }

  QPrinter pdfPrinter;
  pdfPrinter.setOutputFileName(nomeFile);
  documento.print(&pdfPrinter);

}

void ReportForm::stampaPerDestinazione(const QString &nomeFile)
{
  QFile f(nomeFile);
  QTextDocument documento;
  QTextCursor cursore(&documento);
  cursore.setPosition(QTextCursor::Start);
  QTextTableFormat formatoTabella;
  QString testo;

  QString sql="select nome from destinazionistampa";
  QSqlQuery stmtDestinazioni(sql);
  if(!stmtDestinazioni.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmtDestinazioni.lastError().text());
    return;
  }
  while(stmtDestinazioni.next()) {
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
      return;
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
    cursore.insertText("\n\n\n\n\n");
  }

  QPrinter pdfPrinter;
  pdfPrinter.setOutputFileName(nomeFile);
  documento.print(&pdfPrinter);
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
