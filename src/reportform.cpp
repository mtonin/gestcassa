#include "reportform.h"
#include "textprinter.h"

#include <QPainter>
#include <QtSql>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextDocumentFragment>
#include <QTextDocumentWriter>
#include <QMessageBox>

ReportForm::ReportForm(QMap<QString, QVariant>* par, QWidget *parent) : configurazione(par), QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}

void ReportForm::stampa(bool preview)
{
    QTextDocument* doc;
    if (noSuddivisioneBox->isChecked()) {
        doc = creaDocumentTutto();
        stampa(doc, "Inventario articoli al &date;", preview);
    }
    if (repartiBox->isChecked()) {
        doc = creaDocumentPerReparti();
        stampa(doc, "Inventario articoli per reparti al &date;", preview);
    }
    if (destinazioneBox->isChecked()) {
        doc = creaDocumentPerDestinazione();
        stampa(doc, "Inventario articoli per destinazione stampa al &date;", preview);
    }

}

QTextDocument* ReportForm::creaDocumentTutto()
{
    QTextDocument* documento = new QTextDocument(this);
    QTextCursor cursoreDoc(documento);
    QString testo;

    QTextDocument tableDocument;
    QTextCursor cursore(&tableDocument);
    QTextTable* tabella = cursore.insertTable(1, 5);

    foreach(testo, ((QString)"ARTICOLO,PREZZO,REPARTO,DESTINAZIONE,STATO").split(",")) {
        putHeader(cursore, testo);
        cursore.movePosition(QTextCursor::NextCell);
    }

    QString sql = "select a.descrizione,a.prezzo,a.destinazione,a.gestioneMenu,b.descrizione as reparto,c.abilitato \
              from articoli a,reparti b, pulsanti c \
              where c.idarticolo=a.idarticolo \
              and c.idreparto=b.idreparto";

    if (ordineAlfabeticoBox->isChecked()) {
        sql.append(" order by lower(a.descrizione) asc");
    } else {
        sql.append(" order by a.prezzo asc");
    }

    QSqlQuery stmt(sql);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return NULL;
    }

    int posDescrizione = stmt.record().indexOf("descrizione");
    int posPrezzo = stmt.record().indexOf("prezzo");
    int posDestinazione = stmt.record().indexOf("destinazione");
    int posReparto = stmt.record().indexOf("reparto");
    int posGestioneMenu = stmt.record().indexOf("gestioneMenu");
    int posFlagAbilitato = stmt.record().indexOf("abilitato");
    QTextBlockFormat rightAlignFormat;
    rightAlignFormat.setAlignment(Qt::AlignRight);

    while (stmt.next()) {
        QString descrizione = stmt.value(posDescrizione).toString();
        QString prezzo = QString("%1 %L2").arg(QChar(0x20AC)).arg(stmt.value(posPrezzo).toFloat(), 4, 'f', 2);
        QString destinazione = stmt.value(posDestinazione).toString();
        bool gestioneMenu = stmt.value(posGestioneMenu).toBool();
        if (gestioneMenu)
            destinazione = "**MENU**";
        if (destinazione.isEmpty())
            destinazione = "**NON IMPOSTATA**";

        QString reparto = stmt.value(posReparto).toString();
        QString statoAbilitato = stmt.value(posFlagAbilitato).toBool() == true ? "" : "DISAB.";

        tabella->appendRows(1);
        cursore.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 4);
        cursore.insertText(descrizione);
        cursore.movePosition(QTextCursor::NextCell);

        cursore.setBlockFormat(rightAlignFormat);
        cursore.insertText(prezzo);
        cursore.movePosition(QTextCursor::NextCell);
        cursore.insertText(reparto);
        cursore.movePosition(QTextCursor::NextCell);
        cursore.insertText(destinazione);
        cursore.movePosition(QTextCursor::NextCell);
        cursore.insertText(statoAbilitato);
    }

    formattaTabella(tabella,QString("45,10,20,20,5").split(",").toVector());

    cursoreDoc.insertFragment(QTextDocumentFragment(&tableDocument));
    cursoreDoc.insertText("\n\n\n");

    testo = QString("COMPOSIZIONE MENU'");
    putHeader(cursoreDoc, testo);
    cursoreDoc.movePosition(QTextCursor::NextRow);
    cursoreDoc.insertFragment(QTextDocumentFragment(creaDocumentMenu()));

    return documento;
}

QTextDocument* ReportForm::creaDocumentPerReparti()
{
    QTextDocument* documento = new QTextDocument(this);
    QTextCursor cursore(documento);
    QString testo;

    QString sql = "select idreparto,descrizione from reparti";
    QSqlQuery stmtReparti(sql);
    if (!stmtReparti.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmtReparti.lastError().text());
        return NULL;
    }
    int totReparti = 0;
    while (stmtReparti.next()) {
        if (totReparti > 0) {
            cursore.insertText("\n\n\n\n");
        }
        totReparti++;
        int idReparto = stmtReparti.value(0).toInt();
        QString reparto = stmtReparti.value(1).toString();
        testo = QString("REPARTO %1: %2").arg(totReparti).arg(reparto);
        putHeader(cursore, testo);
        cursore.movePosition(QTextCursor::NextRow);

        sql = "select a.descrizione,a.prezzo,a.destinazione,a.gestioneMenu,c.abilitato \
        from articoli a,reparti b, pulsanti c \
        where c.idarticolo=a.idarticolo \
        and c.idreparto=b.idreparto \
        and c.idreparto=?";
        if (ordineAlfabeticoBox->isChecked()) {
            sql.append(" order by a.descrizione asc");
        } else {
            sql.append(" order by a.prezzo asc");
        }

        QSqlQuery stmt;
        stmt.prepare(sql);
        stmt.addBindValue(idReparto);
        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            return NULL;
        }

        int totArticoli = 0;

        int posDescrizione = stmt.record().indexOf("descrizione");
        int posPrezzo = stmt.record().indexOf("prezzo");
        int posDestinazione = stmt.record().indexOf("destinazione");
        int posGestioneMenu = stmt.record().indexOf("gestioneMenu");
        int posFlagAbilitato = stmt.record().indexOf("abilitato");

        QTextDocument tableDocument;
        QTextCursor tableCursore(&tableDocument);

        QTextTable* tabella = tableCursore.insertTable(1, 4);
        foreach(testo, ((QString)"ARTICOLO,PREZZO,DESTINAZIONE,STATO").split(",")) {
            putHeader(tableCursore, testo);
            tableCursore.movePosition(QTextCursor::NextCell);
        }

        QTextBlockFormat rightAlignFormat;
        rightAlignFormat.setAlignment(Qt::AlignRight);

        while (stmt.next()) {
            totArticoli++;
            QString descrizione = stmt.value(posDescrizione).toString();
            QString prezzo = QString("%1 %L2").arg(QChar(0x20AC)).arg(stmt.value(posPrezzo).toFloat(), 4, 'f', 2);
            QString destinazione = stmt.value(posDestinazione).toString();
            bool gestioneMenu = stmt.value(posGestioneMenu).toBool();
            if (gestioneMenu)
                destinazione = "**MENU**";
            if (destinazione.isEmpty())
                destinazione = "**NON IMPOSTATA**";
            QString statoAbilitato = stmt.value(posFlagAbilitato).toBool() == true ? "" : "DISAB.";

            tabella->appendRows(1);
            tableCursore.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 3);
            tableCursore.insertText(descrizione);
            tableCursore.movePosition(QTextCursor::NextCell);

            tableCursore.setBlockFormat(rightAlignFormat);
            tableCursore.insertText(prezzo);
            tableCursore.movePosition(QTextCursor::NextCell);
            tableCursore.insertText(destinazione);
            tableCursore.movePosition(QTextCursor::NextCell);
            tableCursore.insertText(statoAbilitato);
        }

        formattaTabella(tabella,QString("40,10,30,20").split(",").toVector());

        if (0 == totArticoli) {
            cursore.insertText("\nNessun articolo in questo reparto.");
        } else {
            cursore.insertFragment(QTextDocumentFragment(&tableDocument));

        }
    }

    return documento;

}

QTextDocument* ReportForm::creaDocumentPerDestinazione()
{

    QTextDocument* documento = new QTextDocument(this);
    QTextCursor cursore(documento);
    QString testo;

    QString sql = "select nome from destinazionistampa";
    QSqlQuery stmtDestinazioni(sql);
    if (!stmtDestinazioni.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmtDestinazioni.lastError().text());
        return NULL;
    }
    int totDestinazioni = 0;
    while (stmtDestinazioni.next()) {
        if (totDestinazioni > 0) {
            cursore.insertText("\n\n\n\n");
        }
        totDestinazioni++;
        QString nomeDestinazione = stmtDestinazioni.value(0).toString();
        testo = QString("DESTINAZIONE DI STAMPA: %1").arg(nomeDestinazione);
        putHeader(cursore, testo);
        cursore.movePosition(QTextCursor::NextRow);

        cursore.insertFragment(QTextDocumentFragment(creaDocumentDestinazione(nomeDestinazione)));
    }

    QTextDocument* articoliSenzaRepartiDoc = creaDocumentDestinazione("");
    if (articoliSenzaRepartiDoc != NULL) {
        cursore.insertText("\n\n\n\n");
        testo = QString("ARTICOLI CON DESTINAZIONE DI STAMPA NON IMPOSTATA");
        putHeader(cursore, testo);
        cursore.movePosition(QTextCursor::NextRow);

        cursore.insertFragment(QTextDocumentFragment(articoliSenzaRepartiDoc));
    }

    return documento;
}

QTextDocument *ReportForm::creaDocumentMenu()
{
    QString testo;

    QTextDocument* tableDocument = new QTextDocument(this);
    QTextCursor tableCursore(tableDocument);

    QTextTable* tabella = tableCursore.insertTable(1, 5);
    foreach(testo, ((QString)"ARTICOLO,PREZZO,REPARTO,CONTENUTO,STATO").split(",")) {
        putHeader(tableCursore, testo);
        tableCursore.movePosition(QTextCursor::NextCell);
    }

    QString sql = "select a.idarticolo,a.descrizione,a.prezzo,a.destinazione,b.descrizione as reparto,c.abilitato \
              from articoli a,reparti b, pulsanti c \
              where c.idarticolo=a.idarticolo \
              and c.idreparto=b.idreparto \
              and a.gestionemenu=1";

    if (ordineAlfabeticoBox->isChecked()) {
        sql.append(" order by lower(a.descrizione) asc");
    } else {
        sql.append(" order by a.prezzo asc");
    }

    QSqlQuery stmt(sql);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return NULL;
    }

    int posIdArticolo = stmt.record().indexOf("idarticolo");
    int posDescrizione = stmt.record().indexOf("descrizione");
    int posPrezzo = stmt.record().indexOf("prezzo");
    int posReparto = stmt.record().indexOf("reparto");
    int posFlagAbilitato = stmt.record().indexOf("abilitato");
    QTextBlockFormat rightAlignFormat;
    rightAlignFormat.setAlignment(Qt::AlignRight);

    while (stmt.next()) {
        QString descrizione = stmt.value(posDescrizione).toString();
        QString prezzo = QString("%1 %L2").arg(QChar(0x20AC)).arg(stmt.value(posPrezzo).toFloat(), 4, 'f', 2);
        QString reparto = stmt.value(posReparto).toString();
        int idArticolo = stmt.value(posIdArticolo).toInt();
        QString statoAbilitato = stmt.value(posFlagAbilitato).toBool() == true ? "" : "DISAB.";

        tabella->appendRows(1);
        tableCursore.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 4);
        tableCursore.insertText(descrizione);
        tableCursore.movePosition(QTextCursor::NextCell);

        tableCursore.setBlockFormat(rightAlignFormat);
        tableCursore.insertText(prezzo);
        tableCursore.movePosition(QTextCursor::NextCell);
        tableCursore.insertText(reparto);
        tableCursore.movePosition(QTextCursor::NextCell);

        QSqlQuery stmt1;
        stmt1.prepare("select a.descrizione \
                  from articoli a,articolimenu b \
                  where a.idarticolo=b.idarticolomenu \
                  and b.idarticolo=?");
        stmt1.addBindValue(idArticolo);
        if (!stmt1.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            return NULL;
        }
        QString contenutoMenu;
        while (stmt1.next()) {
            if (!contenutoMenu.isEmpty()) contenutoMenu.append('\n');
            contenutoMenu.append("- ").append(stmt1.value(0).toString());
        }
        tableCursore.insertText(contenutoMenu);
        tableCursore.movePosition(QTextCursor::NextCell);
        tableCursore.movePosition(QTextCursor::NextCell);
        tableCursore.insertText(statoAbilitato);
    }

    formattaTabella(tabella,QString("40,10,20,25,5").split(",").toVector());

    return tableDocument;
}

QTextDocument *ReportForm::creaDocumentDestinazione(const QString& nomeDestinazione)
{

    QTextDocument* tableDocument = new QTextDocument(this);
    QTextCursor tableCursore(tableDocument);

    QString sql = "select a.descrizione,a.prezzo,b.descrizione as reparto,c.abilitato \
        from articoli a,reparti b, pulsanti c \
        where c.idarticolo=a.idarticolo \
        and c.idreparto=b.idreparto \
        and a.gestioneMenu=0\
        and coalesce(a.destinazione,'')=?";
    if (ordineAlfabeticoBox->isChecked()) {
        sql.append(" order by lower(a.descrizione) asc");
    } else {
        sql.append(" order by a.prezzo asc");
    }

    QSqlQuery stmt;
    stmt.prepare(sql);
    stmt.addBindValue(nomeDestinazione);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return NULL;
    }

    int totArticoli = 0;

    int posDescrizione = stmt.record().indexOf("descrizione");
    int posPrezzo = stmt.record().indexOf("prezzo");
    int posReparto = stmt.record().indexOf("reparto");
    int posFlagAbilitato = stmt.record().indexOf("abilitato");

    QTextTable* tabella = tableCursore.insertTable(1, 4);
    foreach(QString testo, ((QString)"ARTICOLO,PREZZO,REPARTO,STATO").split(",")) {
        putHeader(tableCursore, testo);
        tableCursore.movePosition(QTextCursor::NextCell);
    }

    QTextBlockFormat rightAlignFormat;
    rightAlignFormat.setAlignment(Qt::AlignRight);

    while (stmt.next()) {
        totArticoli++;
        QString descrizione = stmt.value(posDescrizione).toString();
        QString prezzo = QString("%1 %L2").arg(QChar(0x20AC)).arg(stmt.value(posPrezzo).toFloat(), 4, 'f', 2);
        QString reparto = stmt.value(posReparto).toString();
        QString statoAbilitato = stmt.value(posFlagAbilitato).toBool() == true ? "" : "DISAB.";

        tabella->appendRows(1);
        tableCursore.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 3);
        tableCursore.insertText(descrizione);
        tableCursore.movePosition(QTextCursor::NextCell);

        tableCursore.setBlockFormat(rightAlignFormat);
        tableCursore.insertText(prezzo);
        tableCursore.movePosition(QTextCursor::NextCell);
        tableCursore.insertText(reparto);
        tableCursore.movePosition(QTextCursor::NextCell);
        tableCursore.insertText(statoAbilitato);
    }

    formattaTabella(tabella,QString("40,20,30,10").split(",").toVector());

    if (0 == totArticoli) {
        return NULL;
    }

    return tableDocument;
}

void ReportForm::stampa(const QTextDocument *doc, const QString descrReport, bool preview)
{

    TextPrinter* tprinter = new TextPrinter(this);
    QString header = QString("<center><b>%1<br>%2</b></center>").arg(configurazione->value("descrManifestazione").toString()).arg(descrReport);
    tprinter->setHeaderText(header);
    tprinter->setFooterText("<center><b>GESTIONE CASSA</b><br>Pag. &page;</center>");
    tprinter->setHeaderSize(5);
    tprinter->setFooterSize(10);
    tprinter->setDateFormat("d MMM yyyy - h:mm:ss");

    if (preview) {
        tprinter->preview(doc);
    } else {
        tprinter->exportPdf(doc, "Salva con nome");
    }

}

void ReportForm::putHeader(QTextCursor cursore, const QString testo)
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

void ReportForm::formattaTabella(QTextTable* tabella,QVector<QString> colWidth)
{
    QTextTableFormat formatoTabella;
    formatoTabella.setBorder(1);
    formatoTabella.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    formatoTabella.setHeaderRowCount(2);
    formatoTabella.setCellSpacing(-1);
    formatoTabella.setCellPadding(3);
    formatoTabella.setWidth(QTextLength(QTextLength::PercentageLength, 100));

    QVector<QTextLength> colLen;
    foreach (QString width,colWidth) {
      colLen.append(QTextLength(QTextLength::PercentageLength,width.toInt()));
    }
    formatoTabella.setColumnWidthConstraints(colLen);

    tabella->setFormat(formatoTabella);

}

void ReportForm::on_anteprimaBtn_clicked()
{
    stampa(true);
}

void ReportForm::on_esportaBtn_clicked()
{
    stampa(false);
}

void ReportForm::on_prenotazioniBtn_clicked()
{
    QTextDocument* doc;
    doc = creaFoglioPrenotazioni();
    /*
    QTextDocumentWriter* docWriter=new QTextDocumentWriter;
    foreach (QByteArray docFormat,docWriter->supportedDocumentFormats()) {
      qDebug(docFormat);
    }
    docWriter->setFileName("c:\\temp\\pronotazioni.odf");
    docWriter->write(QTextDocumentFragment(doc));
    */
    stampa(doc, "PRENOTAZIONI", true);
}

QTextDocument *ReportForm::creaFoglioPrenotazioni()
{
    QTextDocument* documento = new QTextDocument(this);

    QFont font=documento->defaultFont();
    font.setPointSize(14);
    documento->setDefaultFont(font);

    QTextCursor cursore(documento);
    QString testo;

    QString sql = "select idreparto,descrizione from reparti order by lower(descrizione)";
    QSqlQuery stmtReparti(sql);
    if (!stmtReparti.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmtReparti.lastError().text());
        return NULL;
    }
    int totReparti = 0;
    while (stmtReparti.next()) {
        if (totReparti > 0) {
            cursore.insertText("\n\n\n\n");
        }
        int idReparto = stmtReparti.value(0).toInt();
        QString reparto = stmtReparti.value(1).toString();

        sql = "select a.descrizione,a.prezzo \
        from articoli a,reparti b, pulsanti c \
        where c.idarticolo=a.idarticolo \
        and c.idreparto=b.idreparto \
        and c.idreparto=? \
        and c.abilitato = 1 \
        order by lower(a.descrizione) asc";

        QSqlQuery stmt;
        stmt.prepare(sql);
        stmt.addBindValue(idReparto);
        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            return NULL;
        }

        int totArticoli = 0;

        int posDescrizione = stmt.record().indexOf("descrizione");
        int posPrezzo = stmt.record().indexOf("prezzo");

        QTextDocument tableDocument;
        QTextCursor tableCursore(&tableDocument);

        QTextTable* tabella = tableCursore.insertTable(2, 3);
        testo = QString("%1").arg(reparto);
        putHeader(tableCursore,testo);
        tabella->mergeCells(0,0,1,3);
        tableCursore.movePosition(QTextCursor::NextRow);
        foreach(testo, ((QString)"ARTICOLO,PREZZO,Q.TA'").split(",")) {
            putHeader(tableCursore, testo);
            tableCursore.movePosition(QTextCursor::NextCell);
        }

        QTextBlockFormat rightAlignFormat;
        rightAlignFormat.setAlignment(Qt::AlignRight);

        while (stmt.next()) {
            totArticoli++;
            QString descrizione = stmt.value(posDescrizione).toString().simplified();
            QString prezzo = QString("%1 %L2").arg(QChar(0x20AC)).arg(stmt.value(posPrezzo).toFloat(), 4, 'f', 2);

            tabella->appendRows(1);
            tableCursore.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 2);
            tableCursore.insertText(descrizione);
            tableCursore.movePosition(QTextCursor::NextCell);
            tableCursore.setBlockFormat(rightAlignFormat);
            tableCursore.insertText(prezzo);
            tableCursore.movePosition(QTextCursor::NextCell);

        }

        if (0 != totArticoli) {
            formattaTabella(tabella,QString("70,20,10").split(",").toVector());
            cursore.insertFragment(QTextDocumentFragment(&tableDocument));
            totReparti++;
        }
    }

    return documento;
}

