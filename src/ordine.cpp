#include "commons.h"
#include "ordine.h"
#include "restodlg.h"
#include "controlliordine.h"
#include "confermadlg.h"
#include "alerttesseredlg.h"

#include <QMessageBox>
#include <QTimer>
#include <QtSql>
#include <QPrinter>
#include <QPainter>

Ordine::Ordine(QMap<QString, QVariant> *par, QWidget *parent) : configurazione(par), QWidget(parent)
{
    setupUi(this);

    articoliTab->setModel(&modello);
    articoliTab->setWordWrap(true);
    articoliTab->setAlternatingRowColors(true);
    articoliTab->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    articoliTab->horizontalHeader()->setDefaultSectionSize(70);
    articoliTab->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    articoliTab->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    articoliTab->hideColumn(0);
    connect(&modello, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(ricalcolaTotale(QModelIndex, QModelIndex)));
    connect(articoliTab, SIGNAL(clicked(QModelIndex)), this, SLOT(seleziona(QModelIndex)));
    controlli = new ControlliOrdine(this);

    importoUltimoOrdine = 0;
    importoUltimoOrdineText->setText("--");

    idSessioneCorrente = configurazione->value("SESSIONECORRENTE").toInt();
    idCassa = configurazione->value("IDCASSA").toString();
    setStatoSconto(configurazione->value("SCONTO",false).toBool());

    nuovoOrdine(idSessioneCorrente);

    stampaBtn->SetIconSpace(20);
    stampaBtn->setIcon(QIcon(":/GestCassa/stampante"));
    stampaBtn->setIconSize(QSize(48, 48));
    stampaBtn->SetIconPosition(QPictureButton::PositionTop);
    annullaBtn->setIcon(QIcon(":/GestCassa/annulla"));
    annullaBtn->setIconSize(QSize(32, 32));
    annullaBtn->SetIconPosition(QPictureButton::PositionTop);
    duplicaBtn->setIcon(QIcon(":/GestCassa/duplica"));
    duplicaBtn->setIconSize(QSize(32, 32));
    duplicaBtn->SetIconPosition(QPictureButton::PositionTop);
    ultimoRestoBtn->setIcon(QIcon(":/GestCassa/money"));
    ultimoRestoBtn->setIconSize(QSize(32, 32));
    ultimoRestoBtn->SetIconPosition(QPictureButton::PositionTop);
    pagPrevBtn->setIcon(QIcon(":/GestCassa/freccia_su"));
    pagPrevBtn->setIconSize(QSize(32, 32));
    pagPrevBtn->SetIconPosition(QPictureButton::PositionTop);
    pagNextBtn->setIcon(QIcon(":/GestCassa/freccia_giu"));
    pagNextBtn->setIconSize(QSize(32, 32));
    pagNextBtn->SetIconPosition(QPictureButton::PositionTop);
    ristampaBtn->setIcon(QIcon(":/GestCassa/printer-1"));
    ristampaBtn->setIconSize(QSize(32, 32));
    ristampaBtn->SetIconPosition(QPictureButton::PositionTop);

    stampaBtn->SetButtonColorNormal(Qt::green);
    annullaBtn->SetButtonColorNormal(Qt::yellow);
    duplicaBtn->SetButtonColorNormal(Qt::yellow);
    ultimoRestoBtn->SetButtonColorNormal(Qt::yellow);
    pagPrevBtn->SetButtonColorNormal(Qt::yellow);
    pagNextBtn->SetButtonColorNormal(Qt::yellow);
    ristampaBtn->SetButtonColorNormal(Qt::yellow);
    stampaBtn->SetButtonColorHot(Qt::green);
    annullaBtn->SetButtonColorHot(Qt::yellow);
    duplicaBtn->SetButtonColorHot(Qt::yellow);
    ultimoRestoBtn->SetButtonColorHot(Qt::yellow);
    pagPrevBtn->SetButtonColorHot(Qt::yellow);
    pagNextBtn->SetButtonColorHot(Qt::yellow);
    ristampaBtn->SetButtonColorHot(Qt::yellow);
    stampaBtn->SetButtonColorPushed(Qt::magenta);
    annullaBtn->SetButtonColorPushed(Qt::magenta);
    duplicaBtn->SetButtonColorPushed(Qt::magenta);
    ultimoRestoBtn->SetButtonColorPushed(Qt::magenta);
    pagPrevBtn->SetButtonColorPushed(Qt::magenta);
    pagNextBtn->SetButtonColorPushed(Qt::magenta);
    ristampaBtn->SetButtonColorPushed(Qt::magenta);
    scontoBtn->SetButtonColorNormal(Qt::cyan);
    scontoBtn->SetButtonColorHot(Qt::cyan);
    scontoBtn->SetButtonColorPushed(Qt::magenta);

    percentualeSconto=0;
}

Ordine::~Ordine()
{
}

void Ordine::nuovoArticolo(const int idArticolo, const QString descrizione, const float prezzo)
{
    clearSelezione();
    modello.incrementa(idArticolo, descrizione, prezzo);
    articoliTab->scrollToBottom();

}

void Ordine::hide()
{
    controlli->hide();
    QWidget::hide();
}

void Ordine::seleziona(const QModelIndex &indexNew)
{
    if (!indexNew.isValid())
        return;

    int id = modello.index(indexNew.row(), 0).data().toInt();
    controlli->setIdArticolo(id);
    controlli->setModelIndex(indexNew);
    connect(&modello, SIGNAL(rigaCancellata()), controlli, SLOT(close()));

    articoliTab->selectionModel()->select(indexNew, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    disconnect(controlli, 0, 0, 0);
    connect(controlli, SIGNAL(incrementa(int)), &modello, SLOT(incrementa(int)));
    connect(controlli, SIGNAL(decrementa(int, QModelIndex)), &modello, SLOT(decrementa(int, QModelIndex)));
    connect(controlli, SIGNAL(effettoTerminato()), this, SLOT(clearSelezione()));

    QPoint pos = QCursor::pos();
    pos.setY(pos.y() + 20);
    QDesktopWidget* desktop = QApplication::desktop();
    int screenWidth = desktop->screenGeometry().width();
    if (pos.x() + controlli->width() > screenWidth) {
        pos.setX(screenWidth - controlli->width());
    }
    controlli->move(pos);
    controlli->show();
}

void Ordine::ricalcolaTotale(QModelIndex, QModelIndex)
{
    importoOrdineCorrente = 0;
    for (int i = 0; i < modello.rowCount(QModelIndex()); i++) {
        importoOrdineCorrente += modello.index(i, 3).data(Qt::UserRole).toFloat();
    }

    importoSconto=(importoOrdineCorrente*percentualeSconto/100);
    //importoSconto=((float)qFloor(importoSconto*10))/10;
    importoSconto=((float)qRound(importoSconto*10))/10;
    importoOrdineCorrente-=importoSconto;
    totaleLine->setText(QString("%L1").arg(importoOrdineCorrente, 4, 'f', 2));
}

void Ordine::on_annullaBtn_clicked()
{
    if (!isInComposizione()) return;
    ConfermaDlg dlg("Confermi l'annullamento dell'ordine corrente?", "", false);
    if (QDialog::Accepted != dlg.visualizza()) return;
    modello.clear();
    setSconto(false);
}

void Ordine::on_ristampaBtn_clicked()
{
    int numeroOrdine = numOrdineCorrente - 1;
    QSqlQuery stmt;
    if(!stmt.prepare("select 1 from storicoordini where idsessione=? and numeroordine=? and idcassa=?")) {
      QSqlError errore=stmt.lastError();
      QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      QMessageBox::critical(this,"Errore",msg);
      return;
    }
    stmt.addBindValue(idSessioneCorrente);
    stmt.addBindValue(numeroOrdine);
    stmt.addBindValue(idCassa);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    if (stmt.next()) {
        stampaScontrino(numeroOrdine);
    }
}

void Ordine::on_stampaBtn_clicked()
{
    if (0 == modello.rowCount()) {
        return;
    }

    nomeCassa = configurazione->value("NOMECASSA", "000").toString();
    if (modello.completaOrdine(numOrdineCorrente, importoOrdineCorrente, idSessioneCorrente, idCassa,nomeCassa,percentualeSconto,importoSconto)) {

        stampaScontrino(numOrdineCorrente);

		/*
        QPoint pos=stampaBtn->pos();
        AlertTessereDlg tessereDlg(configurazione->value("TESSERESERVER").toString(),pos,this);
        pos=QPoint(0,0);
        tessereDlg.exec();
		*/

        if (configurazione->value("ABILITARESTO").toBool()) {
            int durataSecondi = configurazione->value("DURATARESTO", 5).toInt();
            RestoDlg* restoDlg=new RestoDlg(importoOrdineCorrente, durataSecondi, this);
            //restoDlg->setWindowModality(Qt::WindowModal);
            restoDlg->exec();
        }

        importoUltimoOrdine = importoOrdineCorrente;
        if (0 == importoUltimoOrdine) {
            importoUltimoOrdineText->setText("--");
        } else {
            importoUltimoOrdineText->setText(QString("%L1").arg(importoUltimoOrdine, 4, 'f', 2));
        }

        nuovoOrdine(idSessioneCorrente);

    }
}

void Ordine::nuovoOrdine(const int idSessione)
{
    numOrdineCorrente = 0;
    importoOrdineCorrente=0;
    idSessioneCorrente = idSessione;
    modello.clear();
    QSqlQuery query;
    if(!query.prepare("select max(numeroordine) from storicoordinitot where idsessione=? and idcassa=?")) {
      QSqlError errore=query.lastError();
      QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      QMessageBox::critical(this,"Errore",msg);
      return;
    }
    query.addBindValue(idSessioneCorrente);
    query.addBindValue(idCassa);
    if (!query.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),query.lastError().text());
        return;
    }
    if (query.next()) {
        numOrdineCorrente = query.value(0).toInt();
    }
    numOrdineCorrente++;
    numeroOrdineTxt->setText(QString("%L1").arg(numOrdineCorrente));
    totaleLine->setText(QString("%L1").arg(importoOrdineCorrente, 4, 'f', 2));

    setSconto(false);
}

void Ordine::clearSelezione()
{
    articoliTab->selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
    controlli->close();
}

void Ordine::setStatoSconto(bool flag)
{
    scontoBtn->setVisible(flag);
    scontoLbl->setVisible(flag);
    scontoTxt->setVisible(flag);
}

void Ordine::setSconto(bool flag)
{
    scontoLbl->setEnabled(flag);
    scontoTxt->setEnabled(flag);
    if(flag) {
        percentualeSconto=configurazione->value("PERCENTUALESCONTO").toFloat();
    } else {
        percentualeSconto=0;
    }
    scontoTxt->setText(QString("%1%").arg(percentualeSconto,2,'f',0));
}

void Ordine::stampaScontrino(const int numeroOrdine)
{
    QString intest = configurazione->value("INTESTAZIONE").toString();
    QString fondo = configurazione->value("FONDO").toString();
    QString descrManifestazione = configurazione->value("DESCRMANIFESTAZIONE", "NOME MANIFESTAZIONE").toString();
    bool flagStampaNumeroRitiro = false;
    QChar serieRitiro = configurazione->value("SERIERITIRO", "A").toString().at(0);
    bool logoAbilitato=configurazione->value("PRINTLOGO",false).toBool();
    bool logoFondoAbilitato=configurazione->value("PRINTLOGOFONDO",false).toBool();
    bool intestazioneAbilitata=configurazione->value("PRINTINTESTAZIONE",false).toBool();
    bool footerAbilitata=configurazione->value("PRINTFONDO",false).toBool();

    QString intestazione;
    intestazione.append(descrManifestazione); //.append("\n");
    if (intestazioneAbilitata & !intest.isEmpty()) {
        intestazione.append("\n").append(intest);
    }
    QString rigaTest = "=   SESSIONE DI TEST   =\n= SCONTRINO NON VALIDO =";

    QPixmap logoPixmap;
    if(logoAbilitato) {
      logoPixmap.loadFromData(configurazione->value("LOGOPIXMAP").toByteArray());
      if(logoPixmap.isNull()) logoAbilitato=false;
    }
    QPixmap logoFondoPixmap;
    if(logoFondoAbilitato) {
      logoFondoPixmap.loadFromData(configurazione->value("LOGOFONDOPIXMAP").toByteArray());
      if(logoFondoPixmap.isNull()) logoFondoAbilitato=false;
    }

    QPrinter printer;
    bool stampantePdf = configurazione->value("STAMPANTEPDF", true).toBool();
    QString stampanteSelezionata = configurazione->value("STAMPANTE", "PDF").toString();
    if (stampantePdf) {
        QDir cartellaPdf(configurazione->value("CARTELLAPDF", "ticket").toString());
        cartellaPdf.mkpath(cartellaPdf.absolutePath());
        if (ID_SESSIONE_TEST == idSessioneCorrente)
            printer.setOutputFileName(QString("%1/TEST%2.pdf").arg(cartellaPdf.absolutePath()).arg(numeroOrdine, 5, 10, QChar('0')));
        else
            printer.setOutputFileName(QString("%1/%2.pdf").arg(cartellaPdf.absolutePath()).arg(numeroOrdine, 5, 10, QChar('0')));
    } else {
        printer.setPrinterName(stampanteSelezionata);
    }

    QPageLayout pageLayout=printer.pageLayout();
    QMarginsF margineOriginale=pageLayout.margins();
    QPageSize pageSizeOriginale=pageLayout.pageSize();
    qDebug("pagina originale - width=%f,height=%f",pageSizeOriginale.size(QPageSize::Millimeter).width(),pageSizeOriginale.size(QPageSize::Millimeter).height());

    int risoluzione = configurazione->value("PRINTERRESOLUTION",200).toInt();
    int margineSX=configurazione->value("MARGINESX",5).toInt();
    int margineDX=configurazione->value("MARGINEDX",5).toInt();
    int pageWidth = configurazione->value("PRINTERWINWIDTH",300).toInt();
    int larghezzaFoglio=configurazione->value("LARGHEZZAFOGLIO",0).toInt();
    int lunghezzaFoglio=configurazione->value("LUNGHEZZAFOGLIO",0).toInt();
    QString nomeFontNormale=configurazione->value("PRINTERFONT","lucida console,5").toString();

    printer.setResolution(risoluzione);
    QSizeF foglioSize=pageSizeOriginale.size(QPageSize::Millimeter);
    if(larghezzaFoglio>0 && lunghezzaFoglio > 0) {
        QSizeF nuovaSize(larghezzaFoglio,lunghezzaFoglio);
        QPageSize pageSize(nuovaSize,QPageSize::Millimeter);
        printer.setPageSize(pageSize);
        foglioSize=nuovaSize;
    }
    float rapportoFoglio = foglioSize.width() / foglioSize.height();
    QMargins margineSize(margineSX,0,margineDX,0);
    printer.setPageMargins(margineSize, QPageLayout::Millimeter);

    QRect textRect;
    QPainter painter;

    QFont fontNormale;
    fontNormale.fromString(nomeFontNormale);
    QFont fontGrassetto(nomeFontNormale);
    fontGrassetto.setPointSize(fontNormale.pointSize()+2);
    fontGrassetto.setBold(true);
    QFont fontGrassettoCorsivo(nomeFontNormale);
    fontGrassettoCorsivo.setPointSize(fontNormale.pointSize()+2);
    fontGrassettoCorsivo.setBold(true);
    fontGrassettoCorsivo.setItalic(true);
    //fontGrassettoCorsivo.setUnderline(true);
    QFont fontMini(nomeFontNormale);
    fontMini.setPointSize(1);
    QFont fontMaxi(nomeFontNormale);
    fontMaxi.setPointSize(fontNormale.pointSize()*4);

    QBrush sfondoIntestazione;
    sfondoIntestazione.setColor(Qt::black);
    sfondoIntestazione.setStyle(Qt::SolidPattern);
    QPen pennaIntestazione;
    pennaIntestazione.setColor(Qt::white);
    QPen pennaNormale;
    pennaNormale.setColor(Qt::black);

    painter.begin(&printer);
    QRect vpo=painter.viewport();
    QRect winRect=painter.window();
    painter.setWindow(0, 0, pageWidth, pageWidth / rapportoFoglio);
    winRect=painter.window();
    painter.setRenderHint(QPainter::Antialiasing);

    QSqlQuery stmt;

    // recupera ts dell'ordine

    if(!stmt.prepare("select tsstampa from ordini where idcassa=?")) {
      QSqlError errore=stmt.lastError();
      QString msg=QString("Errore nella lettura dei dati dell'ordine\ncodice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      QMessageBox::critical(this,"Database Error",msg);
      return;
    }
    stmt.addBindValue(idCassa);
    if (!(stmt.exec() && stmt.next())) {
      QSqlError errore=stmt.lastError();
      QString msg=QString("Errore nella lettura dei dati dell'ordine\ncodice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      QMessageBox::critical(this,"Database Error",msg);
      return;
    }
    QDateTime tsOrdine = stmt.record().value(0).toDateTime();

    // stampa scontrini per destinazione

    if(!stmt.prepare("select distinct(destinazione) from dettagliordine where tipoArticolo not in('M','X') and idcassa=?")) {
      QSqlError errore=stmt.lastError();
      QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      QMessageBox::critical(this,"Errore",msg);
      return;
    }
    //stmt.addBindValue(idSessioneCorrente);
    //stmt.addBindValue(numeroOrdine);
    stmt.addBindValue(idCassa);

    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    QStringList repartiStampaList;
    while (stmt.next()) {
        repartiStampaList.append(stmt.value(0).toString());
    }
    foreach(QString destinazioneStampa, repartiStampaList) {

        if(!stmt.prepare("select coalesce(intestazione,nome),stampaflag,stampanumeroritiroflag from destinazionistampa where nome=?")) {
          QSqlError errore=stmt.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(this,"Errore",msg);
          return;
        }
        stmt.addBindValue(destinazioneStampa);
        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            return;
        }

        QString intestDestinazione = "DESTINAZIONE NON IMPOSTATA";
        QString codiceRitiro;
        if (stmt.next()) {
            bool stampaFlag = stmt.value(1).toBool();
            if (!stampaFlag) continue;
            intestDestinazione = stmt.value(0).toString();
            if (stmt.value(2).toBool()) {
                codiceRitiro = QString("CODICE RITIRO: %1/%2").arg(serieRitiro).arg(numeroOrdine);
                flagStampaNumeroRitiro = true;
            }
        }

        int x = 0;
        int y = 0;
        if (ID_SESSIONE_TEST == idSessioneCorrente) {
            painter.setFont(fontGrassetto);
            painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter | Qt::TextWordWrap, rigaTest, &textRect);
            y += textRect.height() + 10;
        }

        painter.setFont(fontGrassetto);
        painter.setPen(pennaIntestazione);
        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.setBackground(sfondoIntestazione);
        painter.drawText(x,y,pageWidth,100,Qt::AlignHCenter | Qt::TextWordWrap, intestDestinazione,&textRect);
        painter.setBackgroundMode(Qt::TransparentMode);
        y += textRect.height() + 10;

        painter.setPen(pennaNormale);

        painter.setFont(fontGrassetto);
        painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter | Qt::TextWordWrap, descrManifestazione, &textRect);
        y += textRect.height() + 10;

        painter.setFont(fontNormale);
        painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter, tsOrdine.toString("dd-MM-yyyy   hh:mm:ss"), &textRect);
        y += textRect.height() + 10;
        painter.drawText(x, y, 200, 100, Qt::AlignLeft, QString("CASSA %1").arg(nomeCassa), &textRect);
        painter.drawText(x + pageWidth / 2, y, pageWidth / 2, 100, Qt::AlignRight, QString("ORDINE N. %L1").arg(numeroOrdine), &textRect);
        painter.drawLine(x, y + textRect.height() + 5, pageWidth, y + textRect.height() + 5);
        y += 10;

        if(!stmt.prepare("SELECT descrizione,sum(quantita) \
                 FROM dettagliordine \
                 where numeroordine=? \
                 and coalesce(destinazione,'')=? \
                 and idcassa=? \
                 group by numeroordine,descrizione")) {
          QSqlError errore=stmt.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(this,"Errore",msg);
          return;
        }
        //stmt.addBindValue(idSessioneCorrente);
        stmt.addBindValue(numeroOrdine);
        stmt.addBindValue(destinazioneStampa.isEmpty() ? "" : destinazioneStampa);
        stmt.addBindValue(idCassa);

        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
            return;
        }
        int numArticoli = 0;
        while (stmt.next()) {
            y += textRect.height();
            QString descrizione = stmt.value(0).toString().simplified();
            int quantita = stmt.value(1).toInt();
            numArticoli += quantita;
            QString quantitaString = QString("%1").arg(quantita, 3, 10);

            QRect tmpRect;
            painter.setFont(fontNormale);
            painter.drawText(x, y, (pageWidth / 10) * 2 - 5, 100, Qt::AlignLeft | Qt::AlignTop, quantitaString, &tmpRect);
            painter.drawText(x + (pageWidth / 10) * 2, y, (pageWidth / 10) * 8 - 5, 1000, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, descrizione, &textRect);
        }

        y += textRect.height() + 5;
        painter.drawLine(x, y, pageWidth, y);

        y += 5;
        QString totaleString = QString("TOTALE: %L1 ARTICOLI").arg(numArticoli);
        painter.setFont(fontGrassetto);
        painter.drawText(x, y, pageWidth, 100, Qt::AlignRight, totaleString, &textRect);
        y += textRect.height() + 20;

        painter.setFont(fontNormale);
        if (!codiceRitiro.isEmpty()) {
            painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter, codiceRitiro, &textRect);
            y += textRect.height() + 10;
        }

        if (ID_SESSIONE_TEST == idSessioneCorrente) {
            painter.setFont(fontGrassetto);
            painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter | Qt::TextWordWrap, rigaTest, &textRect);
            y += textRect.height() + 10;
        }

        painter.setFont(fontMini);
        painter.drawText(x, y, ".");
        painter.setFont(fontNormale);

        if (stampantePdf) {
            printer.newPage();
        } else {
            painter.end();
            painter.begin(&printer);
            painter.setWindow(0, 0, pageWidth, pageWidth / rapportoFoglio);
        }
    }

    // stampa scontrino del totale

    int x = 0;
    int y = 0;

    if (ID_SESSIONE_TEST == idSessioneCorrente) {
        painter.setFont(fontGrassetto);
        painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter | Qt::TextWordWrap, rigaTest, &textRect);
        y += textRect.height() + 10;
    }

    if(logoAbilitato) {
        float rapporto=(float)logoPixmap.width()/pageWidth;
        float logoHeight=logoPixmap.height()/rapporto;
        // stampa il logo grafico
        painter.drawPixmap(x,y,pageWidth,logoHeight,logoPixmap);
        y+=logoHeight+10;
    };

      if(0==y)
        y+=5;
      painter.setFont(fontGrassetto);
      painter.drawText(x,y,pageWidth,200,Qt::AlignHCenter | Qt::TextWordWrap, intestazione,&textRect);
      QPen pen(Qt::black, 2);
      painter.setPen(pen);
      painter.drawRect(textRect.x() - 5, textRect.y() - 5, textRect.width() + 10, textRect.height()+5);
      pen.setWidth(1);
      painter.setPen(pen);
      y += textRect.height()+5;

    y+=5;
    painter.setFont(fontNormale);
    painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter, tsOrdine.toString("dd-MM-yyyy   hh:mm:ss"), &textRect);
    y += textRect.height() + 10;

    painter.drawText(x, y, 200, 100, Qt::AlignLeft, QString("CASSA %1").arg(nomeCassa), &textRect);
    painter.drawText(x + pageWidth / 2, y, pageWidth / 2, 100, Qt::AlignRight, QString("ORDINE N. %L1").arg(numeroOrdine), &textRect);
    painter.drawLine(x, y + textRect.height() + 5, pageWidth, y + textRect.height() + 5);
    y += 10;

    if(!stmt.prepare("select descrizione,quantita,prezzo*quantita from dettagliordine where tipoArticolo not in ('C','X') and idcassa=?")) {
          QSqlError errore=stmt.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(this,"Errore",msg);
          return;
    }
    //stmt.addBindValue(idSessioneCorrente);
    //stmt.addBindValue(numeroOrdine);
    stmt.addBindValue(idCassa);

    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
        return;
    }
    float totale = 0;
    while (stmt.next()) {
        y += textRect.height();
        QString descrizione = stmt.value(0).toString().simplified();
        int quantita = stmt.value(1).toInt();
        QString quantitaString = QString("%1").arg(quantita, 3, 10);
        float prezzo = stmt.value(2).toFloat();
        QString prezzoString = QString("%1 %L2").arg(QChar(0x20AC)).arg(prezzo, 6, 'f', 2);

        totale += prezzo;
        QRect tmpRect;
        painter.setFont(fontNormale);
        painter.drawText(x, y, (pageWidth / 10) * 1 - 5, 100, Qt::AlignLeft | Qt::AlignTop, quantitaString, &tmpRect);
        painter.drawText(x + (pageWidth / 10) * 1+5, y, (pageWidth / 10) * 5 - 5, 1000, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, descrizione, &textRect);
        painter.drawText(x + (pageWidth / 10) * 6, y, (pageWidth / 10) * 4, 100, Qt::AlignRight | Qt::AlignTop, prezzoString, &tmpRect);
    }

    // stampa eventuale sconto
    if(!stmt.prepare("select quantita from dettagliordine where tipoArticolo = 'X' and idcassa=?")) {
        QSqlError errore=stmt.lastError();
        QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
        QMessageBox::critical(this,"Errore",msg);
        return;
    }
    stmt.addBindValue(idCassa);

    if (!stmt.exec()) {
       QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
       return;
    }
    while (stmt.next()) {
        y += textRect.height() + 5;
        painter.drawLine(x, y, pageWidth, y);
        QString totaleString = QString("TOTALE PARZIALE:  %1 %L2").arg(QChar(0x20AC)).arg(totale, 6, 'f', 2);
        y += 5;
        painter.drawText(x, y, pageWidth, 100, Qt::AlignRight, totaleString, &textRect);

        QString descrSconto=configurazione->value("DESCRIZIONESCONTO","SCONTO E ARROT.").toString();
        percentualeSconto=stmt.value(0).toFloat();
        float importoSconto=(totale/100)*percentualeSconto;
        importoSconto=((float)qFloor(importoSconto*10))/10;
        totale-=importoSconto;
        QString scontoString=QString("%1: -%2 %L3").arg(descrSconto).arg(QChar(0x20AC)).arg(importoSconto,6,'f',2);
        y += textRect.height();
        painter.drawText(x, y, pageWidth, 100, Qt::AlignRight, scontoString, &textRect);
    }

    y += textRect.height() + 5;
    painter.drawLine(x, y, pageWidth, y);

    y += 5;
    QString totaleString = QString("TOTALE: %1 %L2").arg(QChar(0x20AC)).arg(totale, 6, 'f', 2);
    painter.setFont(fontGrassetto);
    painter.drawText(x, y, pageWidth, 100, Qt::AlignRight, totaleString, &textRect);


    y += textRect.height() + 20;

    if(footerAbilitata && !fondo.isEmpty()) {
        painter.setFont(fontNormale);
        painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter | Qt::TextWordWrap, fondo, &textRect);
        y += textRect.height() + 15;
    }

    if(logoFondoAbilitato) {
            float rapporto=(float)logoFondoPixmap.width()/pageWidth;
            float logoHeight=logoFondoPixmap.height()/rapporto;
          // stampa il logo grafico
          painter.drawPixmap(x,y,pageWidth,logoHeight,logoFondoPixmap);
          y+=logoHeight+10;
    };

    if (flagStampaNumeroRitiro) {
        painter.drawLine(x, y, pageWidth, y);
        y += 5;
        QString testoRitiro = "CONSERVARE PER IL RITIRO";
        painter.setFont(fontGrassetto);
        painter.drawText(x, y, pageWidth, 20, Qt::AlignCenter, testoRitiro, &textRect);
        y += textRect.height() + 5;
        QString numRitiro = QString("%1/%2").arg(serieRitiro).arg(numeroOrdine);
        painter.setFont(fontMaxi);
        painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter, numRitiro, &textRect);
        y += textRect.height() + 35;
    }

    if (ID_SESSIONE_TEST == idSessioneCorrente) {
        painter.setFont(fontGrassetto);
        painter.drawText(x, y, pageWidth, 100, Qt::AlignHCenter | Qt::TextWordWrap, rigaTest, &textRect);
        y += textRect.height() + 10;
    }

    painter.setFont(fontMini);
    painter.drawText(x, y, ".");
    painter.end();

}

void Ordine::on_ultimoRestoBtn_clicked()
{
    if (importoUltimoOrdine != 0) {
        RestoDlg restoDlg(importoUltimoOrdine, 0, this);
        restoDlg.exec();
    }
}

void Ordine::on_pagPrevBtn_clicked()
{
    int primaRiga = articoliTab->rowAt(0);
    QModelIndex idx = modello.index(primaRiga - 1, 1);
    articoliTab->scrollTo(idx, QAbstractItemView::PositionAtTop);
}

void Ordine::on_pagNextBtn_clicked()
{
    int primaRiga = articoliTab->rowAt(1);
    QModelIndex idx = modello.index(primaRiga + 1, 1);
    articoliTab->scrollTo(idx, QAbstractItemView::PositionAtTop);
}

void Ordine::on_duplicaBtn_clicked()
{
    if (isInComposizione()) {
        ConfermaDlg* dlg = new ConfermaDlg("Confermi l'annullamento dell'ordine corrente?", "", false);
        if (QDialog::Accepted != dlg->visualizza()) return;
        modello.clear();
    }
    QSqlQuery stmt;
    if(!stmt.prepare("select a.idarticolo,a.quantita,b.descrizione,b.prezzo from ordinirighe a,articoli b where a.idcassa=? and a.idarticolo=b.idarticolo")) {
          QSqlError errore=stmt.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(this,"Errore",msg);
          return;
    }
    stmt.addBindValue(idCassa);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    while (stmt.next()) {
        int idArticolo = stmt.value(0).toInt();
        int quantita = stmt.value(1).toInt();
        QString descrizione = stmt.value(2).toString();
        float prezzo = stmt.value(3).toFloat();
        while (quantita-- > 0)
            nuovoArticolo(idArticolo, descrizione, prezzo);
    }

    setSconto(false);
    ricalcolaTotale(QModelIndex(),QModelIndex());
}

void Ordine::on_scontoBtn_clicked() {
    if (!isInComposizione()) return;

    bool flagSconto=!scontoLbl->isEnabled();
    setSconto((flagSconto));
    ricalcolaTotale(QModelIndex(),QModelIndex());
}
