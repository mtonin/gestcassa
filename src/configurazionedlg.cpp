#include "configurazionedlg.h"
#include "destinazionidlg.h"
#include "confermadlg.h"
#include "dbmanager.h"
#include "parametriavanzati.h"
#include "messaggiodlg.h"

#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QtSql>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QSettings>

const QStringList chiaviConfLocale=QStringList()
                                    << "VISUALIZZAZIONEPREZZO"
                                    << "ABILITARESTO"
                                    << "DURATARESTO"
                                    << "STAMPANTEPDF"
                                    << "STAMPANTE"
                                    << "IDCASSA"
                                    << "INIFILE"
                                    << "DBFILEPATH"
                                    << "SERIERITIRO"
                                    << "NASCONDECURSORE"
                                    << "LARGHEZZAFOGLIO"
                                    << "LUNGHEZZAFOGLIO"
                                    << "MARGINEDX"
                                    << "MARGINESX"
                                    << "PRINTERFONT"
                                    << "PRINTERRESOLUTION"
                                    << "PRINTERWINWIDTH";

const QStringList chiaviConfRemote=QStringList()
                                    << "DESCRMANIFESTAZIONE"
                                    << "PRINTINTESTAZIONE"
                                    << "INTESTAZIONE"
                                    << "PRINTFONDO"
                                    << "FONDO"
                                    << "PRINTLOGO"
                                    << "LOGOPIXMAP"
                                    << "PRINTLOGOFONDO"
                                    << "LOGOFONDOPIXMAP"
                                    << "SCONTO"
                                    << "PERCENTUALESCONTO"
                                    << "SCONTOMULTIPLO"
                                    << "DESCRIZIONESCONTO";

ConfigurazioneDlg::ConfigurazioneDlg(QMap<QString, QVariant>* par, QWidget *parent) : configurazioneAttuale(par), QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    isChanged=false;
    nuovaConfigurazione = new QMap<QString, QVariant>;
    nuovaConfigurazione->unite(*configurazioneAttuale);

    char serie = 'A';
    while (serie <= 'Z') {
        serieRitiroTxt->addItem(QChar(serie++));
    }

    cifratore = new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));

    cartellaPdfTxt->setText(nuovaConfigurazione->value("CARTELLAPDF", "").toString());
    if (nuovaConfigurazione->value("STAMPANTEPDF", true).toBool()) {
        pdfChk->setChecked(true);
    } else {
        stampanteChk->setChecked(true);
    }

    QString stampanteSelezionata = nuovaConfigurazione->value("STAMPANTE").toString();
    QList<QPrinterInfo> listaStampanti = QPrinterInfo::availablePrinters();
    int idx = 0;
    foreach(QPrinterInfo stampante, listaStampanti) {
        stampanteBox->addItem(stampante.printerName());
        if (stampante.printerName() == stampanteSelezionata) {
            stampanteBox->setCurrentIndex(idx);
        }
        idx++;
    }
    intestazioneScontrinoTxt->setPlainText(nuovaConfigurazione->value("INTESTAZIONE").toString());
    fondoScontrinoTxt->setPlainText(nuovaConfigurazione->value("FONDO").toString());
    durataRestoTxt->setText(nuovaConfigurazione->value("DURATARESTO", 5).toString());
    attivaRestoCheck->setChecked(nuovaConfigurazione->value("ABILITARESTO", false).toBool());
    nomeCassaTxt->setText(nuovaConfigurazione->value("NOMECASSA").toString());
    descrManifestazioneTxt->setText(nuovaConfigurazione->value("DESCRMANIFESTAZIONE").toString());
    visualizzaPrezzoBox->setChecked(nuovaConfigurazione->value("VISUALIZZAZIONEPREZZO",false).toBool());
    nascondeCursoreBox->setChecked(nuovaConfigurazione->value("NASCONDECURSORE",false).toBool());
    scontoBox->setChecked(nuovaConfigurazione->value("SCONTO",false).toBool());
    percentualeScontoTxt->setValue(nuovaConfigurazione->value("PERCENTUALESCONTO",0).toDouble());
    scontiMultipliBox->setChecked(nuovaConfigurazione->value("SCONTOMULTIPLO",false).toBool());
    scontoDescrizioneTxt->setText(nuovaConfigurazione->value("DESCRIZIONESCONTO","SCONTO E ARROT.").toString());

    QString pwdCifrata = nuovaConfigurazione->value("ADMINPASSWORD").toString();
    pwdInChiaro = pwdCifrata.isEmpty()?"12345":cifratore->decryptToString(pwdCifrata);
    adminPasswordTxt->setText(pwdInChiaro);

    dbPathTxt->setPlainText(nuovaConfigurazione->value("DBFILEPATH").toString());
    //QString ser=configurazione->value("SERIERITIRO","Z").toString();
    //int num=ser.at(0).unicode();
    serieRitiroTxt->setCurrentIndex(nuovaConfigurazione->value("SERIERITIRO", "A").toString().at(0).unicode() - QChar('A').unicode());

    logoCheckBox->setChecked(nuovaConfigurazione->value("PRINTLOGO", false).toBool());
    logoIntestazioneBtn->setEnabled(logoCheckBox->isChecked());
    cancellaLogoBtn->setEnabled(logoCheckBox->isChecked());
    logoFondoCheckBox->setChecked(nuovaConfigurazione->value("PRINTLOGOFONDO", false).toBool());
    logoFondoBtn->setEnabled(logoFondoCheckBox->isChecked());
    cancellaLogoFondoBtn->setEnabled(logoFondoCheckBox->isChecked());
    QPixmap logo;
    logo.loadFromData(nuovaConfigurazione->value("LOGOPIXMAP").toByteArray());
    logoPreview->setPixmap(logo);
    QPixmap logoFondo;
    logoFondo.loadFromData(nuovaConfigurazione->value("LOGOFONDOPIXMAP").toByteArray());
    logoFondoPreview->setPixmap(logoFondo);

    intestazioneCheckBox->setChecked(nuovaConfigurazione->value("PRINTINTESTAZIONE", false).toBool());
    intestazioneScontrinoTxt->setEnabled(intestazioneCheckBox->isChecked());
    fondoCheckBox->setChecked(nuovaConfigurazione->value("PRINTFONDO", false).toBool());
    fondoScontrinoTxt->setEnabled(fondoCheckBox->isChecked());

    tabWidget->setCurrentIndex(0);
    descrManifestazioneTxt->setFocus();

    setCaratteriRimanenti();
}

ConfigurazioneDlg::~ConfigurazioneDlg()
{
    delete cifratore;
    delete nuovaConfigurazione;
}

void ConfigurazioneDlg::on_printerSelectBtn_clicked()
{

    /*
    QPrintDialog printDlg(&printer);
    printDlg.setOption(QPrintDialog::PrintSelection,false);
    printDlg.setOption(QPrintDialog::PrintCollateCopies,false);
    printDlg.setOption(QPrintDialog::PrintPageRange,false);
    printDlg.setOption(QPrintDialog::PrintToFile,false);

    if(QDialog::Accepted==printDlg.exec()) {
    }
    */
    QPageSetupDialog pageSetupDlg(&printer);
    if (QDialog::Accepted == pageSetupDlg.exec()) {
    }

    //nuovaConfigurazione->insert("STAMPANTE",printer.printerName());

}

void ConfigurazioneDlg::on_buttonBox_accepted()
{
    QSettings confLocale(configurazioneAttuale->value("INIFILE").toString(),QSettings::IniFormat);
    confLocale.beginGroup("CONFIGURAZIONE");
    QSqlQuery stmt;
    foreach(QString key, nuovaConfigurazione->keys()) {
        if(key.startsWith('%'))
            continue;
        QVariant nuovoValore=nuovaConfigurazione->value(key);
        if(chiaviConfLocale.contains(key,Qt::CaseInsensitive)) {
            confLocale.setValue(key,nuovoValore.toString());
            configurazioneAttuale->insert(key,nuovoValore.toString());
            continue;
        }
        stmt.clear();
        if (0 == QString::compare(key,"LOGOPIXMAP", Qt::CaseInsensitive) ||
            0 == QString::compare(key,"LOGOFONDOPIXMAP",Qt::CaseInsensitive)) {
            if(nuovoValore.toByteArray()==configurazioneAttuale->value(key).toByteArray())
                continue;
            if(!stmt.prepare("update or insert into risorse (id,oggetto) values (?,?)")) {
              QSqlError errore=stmt.lastError();
              QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
              MessaggioDlg msgBox("Database Error", msg,this);
              msgBox.visualizza();
              return;
            }
            configurazioneAttuale->insert(key,nuovoValore.toByteArray());
            stmt.addBindValue(key);
            stmt.addBindValue(nuovoValore.toByteArray());
        } else if(0==QString::compare(key,"NOMECASSA",Qt::CaseInsensitive)) {
            if(0==nuovoValore.toString().compare(configurazioneAttuale->value(key).toString()))
                continue;
            configurazioneAttuale->insert(key,nuovoValore.toString());
            if(!stmt.prepare("update postazioni set nome=? where id=?")) {
                QSqlError errore=stmt.lastError();
                QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                MessaggioDlg msgBox("Database Error", msg,this);
                msgBox.visualizza();
                return;
            }
            stmt.addBindValue(nuovoValore.toString());
            stmt.addBindValue(configurazioneAttuale->value("IDCASSA").toString());
        } else {
            if(0==nuovoValore.toString().compare(configurazioneAttuale->value(key).toString()))
                continue;
            configurazioneAttuale->insert(key,nuovoValore.toString());
            if(!stmt.prepare("update or insert into configurazione (chiave,valore) values (?,?)")) {
              QSqlError errore=stmt.lastError();
              QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
              MessaggioDlg msgBox("Database Error", msg,this);
              msgBox.visualizza();
              return;
            }
            stmt.addBindValue(key);
            stmt.addBindValue(nuovoValore.toString());
        }
        if (!stmt.exec()) {
            MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
            msgBox.visualizza();
        }
        isChanged=true;
    }

    confLocale.sync();

    if(isChanged) {
        emit configurazioneCambiata();
    }

    return accept();
}

void ConfigurazioneDlg::on_buttonBox_rejected()
{
    return reject();
}

void ConfigurazioneDlg::on_attivaRestoCheck_toggled(bool checked)
{
    durataRestoLbl->setEnabled(checked);
    durataRestoTxt->setEnabled(checked);
    nuovaConfigurazione->insert("ABILITARESTO", checked);
}

void ConfigurazioneDlg::on_stampanteChk_toggled(bool checked)
{
    stampanteBox->setEnabled(checked);
    printerSelectBtn->setEnabled(checked);
    nuovaConfigurazione->insert("STAMPANTEPDF", !checked);
}

void ConfigurazioneDlg::on_pdfChk_toggled(bool checked)
{
    cartellaPdfBtn->setEnabled(checked);
    cartellaPdfTxt->setEnabled(checked);
    nuovaConfigurazione->insert("STAMPANTEPDF", checked);
}

void ConfigurazioneDlg::on_cartellaPdfBtn_clicked()
{
    QString cartella;
    cartella = QFileDialog::getExistingDirectory(0, "Selezionare una cartella", "c:\\");
    if (!cartella.isEmpty()) {
        cartellaPdfTxt->setText(cartella);
    }
    nuovaConfigurazione->insert("CARTELLAPDF", cartella);

}

void ConfigurazioneDlg::on_cartellaPdfTxt_editingFinished()
{
    nuovaConfigurazione->insert("CARTELLAPDF", cartellaPdfTxt->text());
}

void ConfigurazioneDlg::on_nomeCassaTxt_editingFinished()
{
    nuovaConfigurazione->insert("NOMECASSA", nomeCassaTxt->text());
}

void ConfigurazioneDlg::on_durataRestoTxt_editingFinished()
{
    nuovaConfigurazione->insert("DURATARESTO", durataRestoTxt->text());
}

void ConfigurazioneDlg::on_descrManifestazioneTxt_editingFinished()
{
    nuovaConfigurazione->insert("DESCRMANIFESTAZIONE", descrManifestazioneTxt->text());
}

void ConfigurazioneDlg::on_intestazioneScontrinoTxt_textChanged()
{
    nuovaConfigurazione->insert("INTESTAZIONE", intestazioneScontrinoTxt->toPlainText());
}

void ConfigurazioneDlg::on_fondoScontrinoTxt_textChanged()
{
    nuovaConfigurazione->insert("FONDO", fondoScontrinoTxt->toPlainText());
}

void ConfigurazioneDlg::on_visualizzaPrezzoBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("VISUALIZZAZIONEPREZZO", checked);
    emit cambiaVisualizzaPrezzo(checked);
}

void ConfigurazioneDlg::on_cancellaOrdiniBtn_clicked()
{
    ConfermaDlg* dlg = new ConfermaDlg("Questa operazione azzera il contatore degli ordini.\nProseguire?", "", false);
    if (QDialog::Accepted != dlg->visualizza()) return;

    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection,false);
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from ordinirighe") &&
        !stmt.exec("delete from ordini")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        db.rollback();
        return;
    }

    int idSessioneCorrente = nuovaConfigurazione->value("SESSIONECORRENTE").toInt();
    idSessioneCorrente++;

    if(!stmt.prepare("insert into sessione (idsessione,tsinizio) values (?,'now')")) {
      QSqlError errore=stmt.lastError();
      QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      MessaggioDlg msgBox("Database Error", msg,this);
      msgBox.visualizza();
      return;
    }
    stmt.addBindValue(idSessioneCorrente);

    if (!stmt.exec()) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        db.rollback();
        return;
    }

    nuovaConfigurazione->insert("SESSIONECORRENTE", idSessioneCorrente);

    db.commit();
    emit resetOrdini(idSessioneCorrente);

}

void ConfigurazioneDlg::on_stampanteBox_activated(const QString &arg1)
{
    nuovaConfigurazione->insert("STAMPANTE", arg1);
}

void ConfigurazioneDlg::on_exportOrdiniBtn_clicked()
{

    QString separatoreRighe = "#§EOL#§";
    QStringList listaSql;

    QSqlQuery stmt("select nomecassa,numeroordine, tsstampa, importo, descrizione, quantita, destinazione, prezzo, tipoArticolo from storicoordini");
    if (!stmt.isActive()) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    while (stmt.next()) {
        QString nomeCassaOrdine = stmt.value(0).toString();
        QString numeroOrdine = stmt.value(1).toString();
        QString tsStampaOrdine = stmt.value(2).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString importoOrdine = stmt.value(3).toString();
        QString descrizioneArticoloOrdine = stmt.value(4).toString();
        QString quantitaArticoloOrdine = stmt.value(5).toString();
        QString destinazioneArticoloOrdine = stmt.value(6).toString();
        QString prezzoArticoloOrdine = stmt.value(7).toString();
        QString tipoArticoloOrdine = stmt.value(8).toString();
        QString riga = QString("ORDINI#§%1#§%2#§%3#§%4#§%5#§%6#§%7#§%8#§%9")
                       .arg(nomeCassaOrdine)
                       .arg(numeroOrdine)
                       .arg(tsStampaOrdine)
                       .arg(importoOrdine)
                       .arg(descrizioneArticoloOrdine)
                       .arg(quantitaArticoloOrdine)
                       .arg(destinazioneArticoloOrdine)
                       .arg(prezzoArticoloOrdine)
                       .arg(tipoArticoloOrdine);
        listaSql.append(riga);
    }

    esportaInFile(listaSql.join(separatoreRighe));

}

void ConfigurazioneDlg::on_exportArticoliBtn_clicked()
{
    QString separatoreRighe = "#§EOL#§";
    QStringList exportLista;
    QSqlQuery stmt;

    if (!stmt.exec("select nome,coalesce(intestazione,'NULL'),stampaflag,stampanumeroritiroflag from destinazionistampa")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    while (stmt.next()) {
        QString nomeDest = stmt.value(0).toString();
        QString intestazioneDest = stmt.value(1).toString();
        bool stampaFlag = stmt.value(2).toBool();
        bool stampaNumeroRitiroFlag = stmt.value(3).toBool();
        QString riga = QString("DESTINAZIONISTAMPA#§%1#§%2#§%3#§%4")
                       .arg(nomeDest)
                       .arg(intestazioneDest)
                       .arg(stampaFlag)
                       .arg(stampaNumeroRitiroFlag);
        exportLista.append(riga);
    }

    if (!stmt.exec("select idreparto,descrizione,coalesce(font,'NULL'),coalesce(coloresfondo,'NULL'),coalesce(colorecarattere,'NULL'),abilitato,adattafont from reparti")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    while (stmt.next()) {
        QString idreparto = stmt.value(0).toString();
        QString descrizioneReparto = stmt.value(1).toString();
        QString font = stmt.value(2).toString();
        QString coloreSfondo = stmt.value(3).toString();
        QString coloreCarattere = stmt.value(4).toString();
        bool abilitato = stmt.value(5).toBool();
        bool adattaFlag = stmt.value(6).toBool();
        QString riga = QString("REPARTI#§%1#§%2#§%3#§%4#§%5#§%6#§%7")
                       .arg(idreparto)
                       .arg(descrizioneReparto)
                       .arg(font)
                       .arg(coloreSfondo)
                       .arg(coloreCarattere)
                       .arg(abilitato)
                       .arg(adattaFlag);
        exportLista.append(riga);
    }

    if (!stmt.exec("select idarticolo,descrizione,prezzo,coalesce(destinazione,'NULL'),gestioneMenu from articoli")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    while (stmt.next()) {
        QString idArticolo = stmt.value(0).toString();
        QString descrizioneArticolo = stmt.value(1).toString();
        QString prezzo = stmt.value(2).toString();
        QString destinazioneStampa = stmt.value(3).toString();
        QString gestioneMenu = stmt.value(4).toString();
        QString riga = QString("ARTICOLI#§%1#§%2#§%3#§%4#§%5")
                       .arg(idArticolo)
                       .arg(descrizioneArticolo)
                       .arg(prezzo)
                       .arg(destinazioneStampa)
                       .arg(gestioneMenu);
        exportLista.append(riga);
    }

    if (!stmt.exec("select idarticolo,idarticolomenu from articolimenu")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    while (stmt.next()) {
        QString idArticolo = stmt.value(0).toString();
        QString idArticoloMenu = stmt.value(1).toString();
        QString riga = QString("ARTICOLIMENU#§%1#§%2")
                       .arg(idArticolo)
                       .arg(idArticoloMenu);
        exportLista.append(riga);
    }

    if (!stmt.exec("select idreparto,riga,colonna,idarticolo,abilitato from pulsanti")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    while (stmt.next()) {
        QString idReparto = stmt.value(0).toString();
        QString rigaPulsante = stmt.value(1).toString();
        QString colonnaPulsante = stmt.value(2).toString();
        QString idArticolo = stmt.value(3).toString();
        QString flagAbilitato = stmt.value(4).toString();
        QString riga = QString("PULSANTI#§%1#§%2#§%3#§%4#§%5")
                       .arg(idReparto)
                       .arg(rigaPulsante)
                       .arg(colonnaPulsante)
                       .arg(idArticolo)
                       .arg(flagAbilitato);
        exportLista.append(riga);
    }

    QString str = nuovaConfigurazione->value("DESCRMANIFESTAZIONE").toString();
    QString riga = QString("CONFIGURAZIONE#§DESCRMANIFESTAZIONE#§%1")
                   .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = nuovaConfigurazione->value("PRINTINTESTAZIONE").toString();
    riga = QString("CONFIGURAZIONE#§PRINTINTESTAZIONE#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = nuovaConfigurazione->value("INTESTAZIONE").toString();
    riga = QString("CONFIGURAZIONE#§INTESTAZIONE#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = nuovaConfigurazione->value("PRINTFONDO").toString();
    riga = QString("CONFIGURAZIONE#§PRINTFONDO#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = nuovaConfigurazione->value("FONDO").toString();
    riga = QString("CONFIGURAZIONE#§FONDO#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = nuovaConfigurazione->value("PRINTLOGO").toString();
    riga = QString("CONFIGURAZIONE#§PRINTLOGO#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = nuovaConfigurazione->value("PRINTLOGOFONDO").toString();
    riga = QString("CONFIGURAZIONE#§PRINTLOGOFONDO#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    if (!stmt.exec("select id,oggetto from risorse")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    while (stmt.next()) {
        QString idRisorsa = stmt.value(0).toString();
        QString objRisorsa = stmt.value(1).toByteArray().toBase64();
        QString riga = QString("RISORSE#§%1#§%2")
                       .arg(idRisorsa)
                       .arg(objRisorsa);
        exportLista.append(riga);
    }

    esportaInFile(exportLista.join(separatoreRighe));
}

void ConfigurazioneDlg::esportaInFile(const QString &testo)
{
    QString nomeFile;
    nomeFile = QFileDialog::getSaveFileName(0, "Esporta in");
    if (nomeFile.isEmpty()) {
        return;
    }
    QFile exportFile(nomeFile);
    exportFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    QTextStream exportStream(&exportFile);

    exportStream << testo;
    exportFile.close();
}

QVariant ConfigurazioneDlg::valutaStringa(const QString &str)
{
    if (0 == str.compare("null", Qt::CaseInsensitive)) {
        return QVariant(QVariant::String);
    } else {
        if (0 == str.compare("true", Qt::CaseInsensitive))
            return QVariant(1);
        else if (0 == str.compare("false", Qt::CaseInsensitive))
            return QVariant(0);
        else
            return QVariant(str);
    }
}

void ConfigurazioneDlg::keyPressEvent(QKeyEvent *evt)
{
  switch (evt->key()) {
    case Qt::Key_F1: {
        if(evt->modifiers() & Qt::ControlModifier) {
          qDebug("CTRL+F1 pressed");
          evt->accept();
          execParametriAvanzati();
        }
    }
  }

}

void ConfigurazioneDlg::execParametriAvanzati()
{
  ParametriAvanzati dlg(nuovaConfigurazione);
  dlg.exec();
}

void ConfigurazioneDlg::setCaratteriRimanenti(){

  int carRimanenti=descrManifestazioneTxt->maxLength()-descrManifestazioneTxt->text().size();
  QString label=QString("%1 caratteri rimanenti").arg(carRimanenti);
  rimanentiLbl->setText(label);

}

void ConfigurazioneDlg::on_importArticoliBtn_clicked()
{
    QString nomeFile;
    nomeFile = QFileDialog::getOpenFileName(0, "Importa da");
    if (nomeFile.isEmpty()) {
        return;
    }
    QFile importFile(nomeFile);
    importFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection,false);
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from pulsanti") ||
        !stmt.exec("delete from articolimenu") ||
        !stmt.exec("delete from articoli") ||
        !stmt.exec("delete from destinazionistampa") ||
        !stmt.exec("delete from reparti") ||
        !stmt.exec("delete from risorse")
    ) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        db.rollback();
        return;
    }

    QString inputText =QString::fromLatin1(importFile.readAll());
    QString separatoreRighe = "#§EOL#§";
    if (-1 == inputText.indexOf(separatoreRighe)) {
        MessaggioDlg msgBox("Import Error", "Il formato del file è obsoleto.\nImpossibile eseguire l'importazione",this);
        msgBox.visualizza();
        db.rollback();
        return;
    }
    QString separatoreCampi = "#§";
    if (inputText.indexOf("Â§") > 0) {
        separatoreCampi = "Â§";
    }
    QStringList righeInput = inputText.split(separatoreRighe);
    foreach(QString rigaSingola, righeInput) {
        QStringList campiInput = rigaSingola.split(separatoreCampi);
        int idx = 0;
        while (idx < campiInput.size()) {
            QString tabella = campiInput.at(idx);
            QString sql;
            if (0 == tabella.compare("destinazionistampa", Qt::CaseInsensitive)) {
                sql="INSERT INTO DESTINAZIONISTAMPA (nome,intestazione,stampaflag,stampanumeroritiroflag) VALUES(?,?,?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  MessaggioDlg msgBox("Database Error", msg,this);
                  msgBox.visualizza();
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                if (idx >= campiInput.size() - 2)
                    stmt.addBindValue(valutaStringa("true"));
                else
                    stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                if (idx >= campiInput.size() - 1)
                    stmt.addBindValue(valutaStringa("false"));
                else
                    stmt.addBindValue(valutaStringa(campiInput.at(++idx)));

            }
            if (0 == tabella.compare("pulsanti", Qt::CaseInsensitive)) {
                sql="INSERT INTO PULSANTI (idReparto,riga,colonna,idArticolo,abilitato) VALUES(?,?,?,?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  MessaggioDlg msgBox("Database Error", msg,this);
                  msgBox.visualizza();
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            }
            if (0 == tabella.compare("reparti", Qt::CaseInsensitive)) {
                sql="INSERT INTO REPARTI (idReparto,descrizione,font,coloreSfondo,coloreCarattere,abilitato,adattafont) VALUES(?,?,?,?,?,?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  MessaggioDlg msgBox("Database Error", msg,this);
                  msgBox.visualizza();
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                if (idx >= campiInput.size() - 1)
                    stmt.addBindValue(valutaStringa("true"));
                else
                    stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                if (idx >= campiInput.size() - 1)
                    stmt.addBindValue(valutaStringa("false"));
                else
                    stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            }
            if (0 == tabella.compare("articoli", Qt::CaseInsensitive)) {
                sql="INSERT INTO ARTICOLI (idArticolo,descrizione,prezzo,destinazione,gestioneMenu) VALUES(?,?,?,?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  MessaggioDlg msgBox("Database Error", msg,this);
                  msgBox.visualizza();
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            }
            if (0 == tabella.compare("articolimenu", Qt::CaseInsensitive)) {
                sql="INSERT INTO ARTICOLIMENU (idArticolo,idArticoloMenu) VALUES(?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  MessaggioDlg msgBox("Database Error", msg,this);
                  msgBox.visualizza();
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            }
            if (0 == tabella.compare("configurazione", Qt::CaseInsensitive)) {
                sql="UPDATE OR INSERT INTO CONFIGURAZIONE VALUES(?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  MessaggioDlg msgBox("Database Error", msg,this);
                  msgBox.visualizza();
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)).toString().toUpper());
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            }
            if (0 == tabella.compare("risorse", Qt::CaseInsensitive)) {
                sql="UPDATE OR INSERT INTO RISORSE VALUES(?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  MessaggioDlg msgBox("Database Error", msg,this);
                  msgBox.visualizza();
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)).toString().toUpper());
                stmt.addBindValue(QByteArray::fromBase64(valutaStringa(campiInput.at(++idx)).toByteArray()));
            }

            qDebug(campiInput.join("#").toStdString().c_str());
            if (!stmt.exec()) {
                MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
                msgBox.visualizza();
                db.rollback();
                return;
            }
            idx++;
        }

    }

    if (!stmt.exec("select max(idarticolo) from articoli")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        db.rollback();
        return;
    }
    if (stmt.next()) {
        int maxIdArticolo = stmt.value(0).toInt();
        if (!stmt.exec(QString("alter sequence seqarticoli restart with %1").arg(maxIdArticolo))) {
            MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
            msgBox.visualizza();
            db.rollback();
            return;
        }
    }

    db.commit();
    emit resetArticoli();

    foreach (QString nomePar,chiaviConfRemote) {
      if(!aggiornaConfigurazioneDaDB(nomePar)) {
        db.rollback();
        return;
      }

    }

    on_buttonBox_accepted();
}

void ConfigurazioneDlg::on_resetDbBtn_clicked()
{
    ConfermaDlg* dlg = new ConfermaDlg("Questa operazione cancella tutti i reparti e gli articoli censiti.\nProseguire?", "", false);
    if (QDialog::Accepted != dlg->visualizza()) return;

    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection,false);
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from pulsanti") ||
        !stmt.exec("delete from reparti") ||
        !stmt.exec("delete from articolimenu") ||
        !stmt.exec("delete from articoli") ||
        !stmt.exec("delete from destinazionistampa") ||
        !stmt.exec("delete from risorse")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        db.rollback();
        return;
    }

    int idSessioneCorrente = nuovaConfigurazione->value("SESSIONECORRENTE").toInt();
    idSessioneCorrente++;

    if(!stmt.prepare("insert into sessione (idsessione,tsinizio) values (?,'now')")) {
      QSqlError errore=stmt.lastError();
      QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      MessaggioDlg msgBox("Database Error", msg,this);
      msgBox.visualizza();
      db.rollback();
      return;
    }
    stmt.addBindValue(idSessioneCorrente);

    if (!stmt.exec()) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        db.rollback();
        return;
    }

    nuovaConfigurazione->insert("SESSIONECORRENTE", idSessioneCorrente);

    db.commit();
    emit resetOrdini(idSessioneCorrente);
    emit resetArticoli();

    nuovaConfigurazione->insert("INTESTAZIONE", "");
    nuovaConfigurazione->insert("FONDO", "");
    nuovaConfigurazione->insert("DURATARESTO", "5");
    nuovaConfigurazione->insert("ABILITARESTO", "");
    nuovaConfigurazione->insert("STAMPANTE", "");
    nuovaConfigurazione->insert("CARTELLAPDF", "");
    nuovaConfigurazione->insert("STAMPANTEPDF", "");
    nuovaConfigurazione->insert("NOMECASSA", "");
    nuovaConfigurazione->insert("DESCRMANIFESTAZIONE", "");
    nuovaConfigurazione->insert("VISUALIZZAZIONEPREZZO", "");
    nuovaConfigurazione->insert("NASCONDECURSORE", "");
    nuovaConfigurazione->insert("SERIERITIRO", "A");
    nuovaConfigurazione->insert("PRINTLOGO", "");
    nuovaConfigurazione->insert("PRINTINTESTAZIONE", "");
    nuovaConfigurazione->insert("LOGOPIXMAP", "");
    nuovaConfigurazione->insert("PRINTLOGOFONDO", "");
    nuovaConfigurazione->insert("PRINTFONDO", "");
    nuovaConfigurazione->insert("LOGOFONDOPIXMAP", "");

    on_buttonBox_accepted();
}

void ConfigurazioneDlg::on_resetBuoniBtn_clicked()
{
    ConfermaDlg* dlg = new ConfermaDlg("Questa operazione cancella tutti i buoni emessi.\nProseguire?", "", false);
    if (QDialog::Accepted != dlg->visualizza()) return;

    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection,false);
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from buoni")) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        db.rollback();
        return;
    }

    db.commit();
}

void ConfigurazioneDlg::selezionaLogo(const QString nomePar) {
  QString logoFileName;
  logoFileName = QFileDialog::getOpenFileName();
  if (logoFileName.isEmpty()) {
      return;
  }

  QPixmap logo;
  logo.load(logoFileName);
  if(nomePar.compare("LOGOPIXMAP")==0)
    logoPreview->setPixmap(logo);
  if(nomePar.compare("LOGOFONDOPIXMAP")==0)
    logoFondoPreview->setPixmap(logo);
  QByteArray logoData;
  QBuffer logoBuffer(&logoData);
  logoBuffer.open(QIODevice::WriteOnly);
  bool rcSave = logo.save(&logoBuffer, "PNG");
  nuovaConfigurazione->insert(nomePar, logoData);
}

bool ConfigurazioneDlg::aggiornaConfigurazioneDaDB(const QString nomePar) {

  QString sql;
  if(nomePar.contains("pixmap",Qt::CaseInsensitive)) {
    sql="select oggetto from risorse where id=?";
  } else {
    sql="select valore from configurazione where chiave=?";
  }
  QSqlQuery stmt;
  if (!stmt.prepare(sql)) {
      MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
      msgBox.visualizza();
      return false;
  }
  stmt.addBindValue(nomePar);
  if (!stmt.exec()) {
      MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
      msgBox.visualizza();
    return false;
  }

  if(stmt.next()) {
    if(nomePar.contains("pixmap",Qt::CaseInsensitive)) {
      nuovaConfigurazione->insert(nomePar, stmt.value(0).toByteArray());
    } else {
      nuovaConfigurazione->insert(nomePar, stmt.value(0).toString());
    }
  } else {
      nuovaConfigurazione->insert(nomePar,NULL);
  }

  return true;
}

void ConfigurazioneDlg::on_logoIntestazioneBtn_clicked(){
  selezionaLogo("LOGOPIXMAP");
}

void ConfigurazioneDlg::on_logoFondoBtn_clicked(){
  selezionaLogo("LOGOFONDOPIXMAP");
}

void ConfigurazioneDlg::on_logoCheckBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("PRINTLOGO", checked);
}

void ConfigurazioneDlg::on_logoFondoCheckBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("PRINTLOGOFONDO", checked);
}

void ConfigurazioneDlg::on_intestazioneCheckBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("PRINTINTESTAZIONE", checked);
}

void ConfigurazioneDlg::on_fondoCheckBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("PRINTFONDO", checked);
}

void ConfigurazioneDlg::on_cancellaLogoBtn_clicked()
{
    logoPreview->setPixmap(QPixmap());
    nuovaConfigurazione->insert("LOGOPIXMAP",NULL);
}

void ConfigurazioneDlg::on_cancellaLogoFondoBtn_clicked()
{
  logoFondoPreview->setPixmap(QPixmap());
  nuovaConfigurazione->insert("LOGOFONDOPIXMAP",NULL);
}

void ConfigurazioneDlg::on_nascondeCursoreBox_clicked(bool checked)
{
  nuovaConfigurazione->insert("NASCONDECURSORE", checked);
}

void ConfigurazioneDlg::on_adminPasswordTxt_editingFinished()
{
    if (pwdInChiaro != adminPasswordTxt->text()) {
        QString pwdCifrata = cifratore->encryptToString(adminPasswordTxt->text());
        nuovaConfigurazione->insert("ADMINPASSWORD", pwdCifrata);
    }
}

void ConfigurazioneDlg::on_serieRitiroTxt_currentIndexChanged(int index)
{
    nuovaConfigurazione->insert("SERIERITIRO", QChar('A' + index));
}

void ConfigurazioneDlg::on_descrManifestazioneTxt_textEdited(const QString &arg1)
{
    setCaratteriRimanenti();
}

void ConfigurazioneDlg::on_scontoBox_toggled(bool checked)
{
    nuovaConfigurazione->insert("SCONTO",checked);
    emit(cambiaStatoSconto(checked));
}

void ConfigurazioneDlg::on_percentualeScontoTxt_editingFinished()
{
    nuovaConfigurazione->insert("PERCENTUALESCONTO",percentualeScontoTxt->value());
}

void ConfigurazioneDlg::on_scontiMultipliBox_toggled(bool checked)
{
    nuovaConfigurazione->insert("SCONTOMULTIPLO",checked);
}

void ConfigurazioneDlg::on_scontoDescrizioneTxt_editingFinished()
{
    nuovaConfigurazione->insert("DESCRIZIONESCONTO",scontoDescrizioneTxt->text());
}
