#include "configurazionedlg.h"
#include "destinazionidlg.h"
#include "confermadlg.h"
#include "dbmanager.h"
#include "parametriavanzati.h"

#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

ConfigurazioneDlg::ConfigurazioneDlg(QMap<QString, QVariant>* par, QWidget *parent) : configurazione(par), QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    nuovaConfigurazione = new QMap<QString, QVariant>;

    char serie = 'A';
    while (serie <= 'Z') {
        serieRitiroTxt->addItem(QChar(serie++));
    }

    cifratore = new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));

    cartellaPdfTxt->setText(configurazione->value("cartellaPdf", "").toString());
    if (configurazione->value("stampantePdf", true).toBool()) {
        pdfChk->setChecked(true);
    } else {
        stampanteChk->setChecked(true);
    }

    QString stampanteSelezionata = configurazione->value("stampante").toString();
    QList<QPrinterInfo> listaStampanti = QPrinterInfo::availablePrinters();
    int idx = 0;
    foreach(QPrinterInfo stampante, listaStampanti) {
        stampanteBox->addItem(stampante.printerName());
        if (stampante.printerName() == stampanteSelezionata) {
            stampanteBox->setCurrentIndex(idx);
        }
        idx++;
    }
    intestazioneScontrinoTxt->setPlainText(configurazione->value("intestazione").toString());
    fondoScontrinoTxt->setPlainText(configurazione->value("fondo").toString());
    durataRestoTxt->setText(configurazione->value("durataResto", 5).toString());
    if (configurazione->value("abilitaResto", false).toBool()) {
        attivaRestoCheck->setChecked(true);
    }
    nomeCassaTxt->setText(configurazione->value("nomeCassa").toString());
    descrManifestazioneTxt->setText(configurazione->value("descrManifestazione").toString());
    visualizzaPrezzoBox->setChecked(configurazione->value("visualizzazionePrezzo").toBool());

    QString pwdCifrata = configurazione->value("adminPassword").toString();
    pwdInChiaro = pwdCifrata.isEmpty()?"12345":cifratore->decryptToString(pwdCifrata);
    adminPasswordTxt->setText(pwdInChiaro);

    dbPathTxt->setPlainText(configurazione->value("dbFilePath").toString());
    //QString ser=configurazione->value("serieRitiro","Z").toString();
    //int num=ser.at(0).unicode();
    serieRitiroTxt->setCurrentIndex(configurazione->value("serieRitiro", "A").toString().at(0).unicode() - QChar('A').unicode());

    logoCheckBox->setChecked(configurazione->value("printLogo", false).toBool());
    logoIntestazioneBtn->setEnabled(logoCheckBox->isChecked());
    logoFondoCheckBox->setChecked(configurazione->value("printLogoFondo", false).toBool());
    logoFondoBtn->setEnabled(logoFondoCheckBox->isChecked());
    intestazioneCheckBox->setChecked(configurazione->value("printIntestazione", false).toBool());
    intestazioneScontrinoTxt->setEnabled(intestazioneCheckBox->isChecked());
    fondoCheckBox->setChecked(configurazione->value("printFondo", false).toBool());
    fondoScontrinoTxt->setEnabled(fondoCheckBox->isChecked());

    tabWidget->setCurrentIndex(0);
    descrManifestazioneTxt->setFocus();

    connect(serieRitiroTxt, SIGNAL(currentIndexChanged(int)), this, SLOT(cambiaSerieRitiro(int)));

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

    //nuovaConfigurazione->insert("stampante",printer.printerName());

}

void ConfigurazioneDlg::on_buttonBox_accepted()
{
    if (pwdInChiaro != adminPasswordTxt->text()) {
        QString pwdCifrata = cifratore->encryptToString(adminPasswordTxt->text());
        configurazione->insert("adminPassword", pwdCifrata);
        emit passwordCambiata();
    }

    foreach(QString key, nuovaConfigurazione->keys()) {
        configurazione->insert(key, nuovaConfigurazione->value(key));
    }

    QSqlQuery stmt;
    foreach(QString key, configurazione->keys()) {
        stmt.clear();
        if (0 == QString::compare(key, "logoPixmap", Qt::CaseInsensitive) ||
            0 == QString::compare(key,"logoFondoPixmap",Qt::CaseInsensitive)) {
            if(!stmt.prepare("update or insert into risorse (id,oggetto) values (?,?)")) {
              QSqlError errore=stmt.lastError();
              QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
              QMessageBox::critical(this,"Errore",msg);
              return;
            }
            stmt.addBindValue(key);
            stmt.addBindValue(configurazione->value(key).toByteArray());
        } else {
            if(!stmt.prepare("update or insert into configurazione (chiave,valore) values (?,?)")) {
              QSqlError errore=stmt.lastError();
              QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
              QMessageBox::critical(this,"Errore",msg);
              return;
            }
            stmt.addBindValue(key);
            stmt.addBindValue(configurazione->value(key).toString());
        }
        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"),
                                  stmt.lastError().text());
        }
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
    nuovaConfigurazione->insert("abilitaResto", checked);
}

void ConfigurazioneDlg::on_stampanteChk_toggled(bool checked)
{
    stampanteBox->setEnabled(checked);
    printerSelectBtn->setEnabled(checked);
    nuovaConfigurazione->insert("stampantePdf", !checked);
}

void ConfigurazioneDlg::on_pdfChk_toggled(bool checked)
{
    cartellaPdfBtn->setEnabled(checked);
    cartellaPdfTxt->setEnabled(checked);
    nuovaConfigurazione->insert("stampantePdf", checked);
}

void ConfigurazioneDlg::on_cartellaPdfBtn_clicked()
{
    QString cartella;
    cartella = QFileDialog::getExistingDirectory(0, "Selezionare una cartella", "c:\\");
    if (!cartella.isEmpty()) {
        cartellaPdfTxt->setText(cartella);
    }
    nuovaConfigurazione->insert("cartellaPdf", cartella);

}

void ConfigurazioneDlg::on_cartellaPdfTxt_textEdited(const QString &arg1)
{
    nuovaConfigurazione->insert("cartellaPdf", arg1);
}

void ConfigurazioneDlg::on_nomeCassaTxt_textEdited(const QString &arg1)
{
    nuovaConfigurazione->insert("nomeCassa", arg1);
}

void ConfigurazioneDlg::on_durataRestoTxt_textEdited(const QString &arg1)
{
    nuovaConfigurazione->insert("durataResto", arg1);
}

void ConfigurazioneDlg::on_descrManifestazioneTxt_textEdited(const QString &arg1)
{
    nuovaConfigurazione->insert("descrManifestazione", arg1);
    setCaratteriRimanenti();
}

void ConfigurazioneDlg::on_intestazioneScontrinoTxt_textChanged()
{
    nuovaConfigurazione->insert("intestazione", intestazioneScontrinoTxt->toPlainText());
}

void ConfigurazioneDlg::on_fondoScontrinoTxt_textChanged()
{
    nuovaConfigurazione->insert("fondo", fondoScontrinoTxt->toPlainText());
}

void ConfigurazioneDlg::on_visualizzaPrezzoBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("visualizzazionePrezzo", checked);
    emit cambiaVisualizzaPrezzo(checked);
}

void ConfigurazioneDlg::on_cancellaOrdiniBtn_clicked()
{
    ConfermaDlg* dlg = new ConfermaDlg("Questa operazione azzera il contatore degli ordini.\nProseguire?", "", false, this);
    if (QDialog::Accepted != dlg->visualizza()) return;

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from ordinirighe") &&
        !stmt.exec("delete from ordini")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }

    int idSessioneCorrente = configurazione->value("sessioneCorrente").toInt();
    idSessioneCorrente++;

    if(!stmt.prepare("insert into sessione (idsessione,tsinizio) values (?,?)")) {
      QSqlError errore=stmt.lastError();
      QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      QMessageBox::critical(this,"Errore",msg);
      return;
    }
    stmt.addBindValue(idSessioneCorrente);
    stmt.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }

    configurazione->insert("sessioneCorrente", idSessioneCorrente);

    db.commit();
    emit resetOrdini(idSessioneCorrente);

}

void ConfigurazioneDlg::on_stampanteBox_activated(const QString &arg1)
{
    nuovaConfigurazione->insert("stampante", arg1);
}

void ConfigurazioneDlg::on_exportOrdiniBtn_clicked()
{

    QString separatoreRighe = "#§EOL#§";
    QStringList listaSql;

    QSqlQuery stmt("select numeroordine, tsstampa, importo, descrizione, quantita, destinazione, prezzo, tipoArticolo from storicoordini");
    if (!stmt.isActive()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    while (stmt.next()) {
        QString numeroOrdine = stmt.value(0).toString();
        QString tsStampaOrdine = stmt.value(1).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString importoOrdine = stmt.value(2).toString();
        QString descrizioneArticoloOrdine = stmt.value(3).toString();
        QString quantitaArticoloOrdine = stmt.value(4).toString();
        QString destinazioneArticoloOrdine = stmt.value(5).toString();
        QString prezzoArticoloOrdine = stmt.value(6).toString();
        QString tipoArticoloOrdine = stmt.value(7).toString();
        QString riga = QString("ORDINI#§%1#§%2#§%3#§%4#§%5#§%6#§%7#§%8")
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
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
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

    if (!stmt.exec("select idreparto,descrizione,coalesce(font,'NULL'),coalesce(coloresfondo,'NULL'),coalesce(colorecarattere,'NULL'),abilitato from reparti")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        return;
    }
    while (stmt.next()) {
        QString idreparto = stmt.value(0).toString();
        QString descrizioneReparto = stmt.value(1).toString();
        QString font = stmt.value(2).toString();
        QString coloreSfondo = stmt.value(3).toString();
        QString coloreCarattere = stmt.value(4).toString();
        bool abilitato = stmt.value(5).toBool();
        QString riga = QString("REPARTI#§%1#§%2#§%3#§%4#§%5#§%6")
                       .arg(idreparto)
                       .arg(descrizioneReparto)
                       .arg(font)
                       .arg(coloreSfondo)
                       .arg(coloreCarattere)
                       .arg(abilitato);
        exportLista.append(riga);
    }

    if (!stmt.exec("select idarticolo,descrizione,prezzo,coalesce(destinazione,'NULL'),gestioneMenu from articoli")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
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
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
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
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
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

    QString str = configurazione->value("descrManifestazione").toString();
    QString riga = QString("CONFIGURAZIONE#§descrManifestazione#§%1")
                   .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = configurazione->value("printIntestazione").toString();
    riga = QString("CONFIGURAZIONE#§printIntestazione#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = configurazione->value("intestazione").toString();
    riga = QString("CONFIGURAZIONE#§intestazione#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = configurazione->value("printFondo").toString();
    riga = QString("CONFIGURAZIONE#§printFondo#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = configurazione->value("fondo").toString();
    riga = QString("CONFIGURAZIONE#§fondo#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = configurazione->value("printLogo").toString();
    riga = QString("CONFIGURAZIONE#§printLogo#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    str = configurazione->value("printLogoFondo").toString();
    riga = QString("CONFIGURAZIONE#§printLogoFondo#§%1")
           .arg(str.isEmpty() ? "NULL" : str);
    exportLista.append(riga);

    if (!stmt.exec("select id,oggetto from risorse")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
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
  ParametriAvanzati dlg;
  dlg.setRisoluzione(configurazione->value("printerResolution",200).toInt());
  dlg.setAmpiezzaStampa(configurazione->value("printerWinWidth",300).toInt());
  dlg.setDimensioneFontStampa(configurazione->value("printerFontSize",5).toInt());
  if(QDialog::Accepted==dlg.exec()) {
    configurazione->insert("printerResolution",dlg.getRisoluzione());
    configurazione->insert("printerWinWidth",dlg.getAmpiezzaStampa());
    configurazione->insert("printerFontSize",dlg.getDimensioneFontStampa());
  }
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

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from pulsanti") ||
        !stmt.exec("delete from articolimenu") ||
        !stmt.exec("delete from articoli") ||
        !stmt.exec("delete from destinazionistampa") ||
        !stmt.exec("delete from reparti") ||
        !stmt.exec("delete from risorse")
    ) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }

    QString inputText = importFile.readAll();
    QString separatoreRighe = "#§EOL#§";
    if (-1 == inputText.indexOf(separatoreRighe)) {
        QMessageBox::critical(0, QObject::tr("Import Error"), "Il formato del file è obsoleto.\nImpossibile eseguire l'importazione");
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
                  QMessageBox::critical(this,"Errore",msg);
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
                  QMessageBox::critical(this,"Errore",msg);
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
                sql="INSERT INTO REPARTI (idReparto,descrizione,font,coloreSfondo,coloreCarattere,abilitato) VALUES(?,?,?,?,?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  QMessageBox::critical(this,"Errore",msg);
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
            }
            if (0 == tabella.compare("articoli", Qt::CaseInsensitive)) {
                sql="INSERT INTO ARTICOLI (idArticolo,descrizione,prezzo,destinazione,gestioneMenu) VALUES(?,?,?,?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  QMessageBox::critical(this,"Errore",msg);
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
                  QMessageBox::critical(this,"Errore",msg);
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
                  QMessageBox::critical(this,"Errore",msg);
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            }
            if (0 == tabella.compare("risorse", Qt::CaseInsensitive)) {
                sql="UPDATE OR INSERT INTO RISORSE VALUES(?,?)";
                if(!stmt.prepare(sql)) {
                  QSqlError errore=stmt.lastError();
                  QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
                  QMessageBox::critical(this,"Errore",msg);
                  db.rollback();
                  return;
                }
                stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
                stmt.addBindValue(QByteArray::fromBase64(valutaStringa(campiInput.at(++idx)).toByteArray()));
            }

            qDebug(campiInput.join("#").toAscii());
            if (!stmt.exec()) {
                QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
                db.rollback();
                return;
            }
            idx++;
        }

    }

    if (!stmt.exec("select max(idarticolo) from articoli")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        int maxIdArticolo = stmt.value(0).toInt();
        if (!stmt.exec(QString("alter sequence seqarticoli restart with %1").arg(maxIdArticolo))) {
            QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
            db.rollback();
            return;
        }
    }

    db.commit();
    emit resetArticoli();

    if (!stmt.exec("select valore from configurazione where chiave='descrManifestazione'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("descrManifestazione", stmt.value(0).toString());
    }

    if (!stmt.exec("select valore from configurazione where chiave='printIntestazione'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("printIntestazione", valutaStringa(stmt.value(0).toString()));
    }

    if (!stmt.exec("select valore from configurazione where chiave='intestazione'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("intestazione", stmt.value(0).toString());
    }

    if (!stmt.exec("select valore from configurazione where chiave='printFondo'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("printFondo", valutaStringa(stmt.value(0).toString()));
    }

    if (!stmt.exec("select valore from configurazione where chiave='fondo'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("fondo", stmt.value(0).toString());
    }

    if (!stmt.exec("select valore from configurazione where chiave='printLogo'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("printLogo", valutaStringa(stmt.value(0).toString()));
    }

    if (!stmt.exec("select oggetto from risorse where id='logoPixmap'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("logoPixmap", stmt.value(0).toByteArray());
    }

    if (!stmt.exec("select valore from configurazione where chiave='printLogoFondo'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("printLogoFondo", valutaStringa(stmt.value(0).toString()));
    }

    if (!stmt.exec("select oggetto from risorse where id='logoFondoPixmap'")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }
    if (stmt.next()) {
        nuovaConfigurazione->insert("logoFondoPixmap", stmt.value(0).toByteArray());
    }

    on_buttonBox_accepted();
}

void ConfigurazioneDlg::cambiaSerieRitiro(int index)
{
    nuovaConfigurazione->insert("serieRitiro", QChar('A' + index));
}

void ConfigurazioneDlg::on_resetDbBtn_clicked()
{
    ConfermaDlg* dlg = new ConfermaDlg("Questa operazione cancella tutti i reparti e gli articoli censiti.\nProseguire?", "", false, this);
    if (QDialog::Accepted != dlg->visualizza()) return;

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from pulsanti") ||
        !stmt.exec("delete from reparti") ||
        !stmt.exec("delete from articolimenu") ||
        !stmt.exec("delete from articoli") ||
        !stmt.exec("delete from destinazionistampa")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }

    int idSessioneCorrente = configurazione->value("sessioneCorrente").toInt();
    idSessioneCorrente++;

    if(!stmt.prepare("insert into sessione (idsessione,tsinizio) values (?,?)")) {
      QSqlError errore=stmt.lastError();
      QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      QMessageBox::critical(this,"Errore",msg);
      db.rollback();
      return;
    }
    stmt.addBindValue(idSessioneCorrente);
    stmt.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
        db.rollback();
        return;
    }

    configurazione->insert("sessioneCorrente", idSessioneCorrente);

    db.commit();
    emit resetOrdini(idSessioneCorrente);
    emit resetArticoli();

    nuovaConfigurazione->insert("intestazione", "");
    nuovaConfigurazione->insert("fondo", "");
    nuovaConfigurazione->insert("durataResto", "5");
    nuovaConfigurazione->insert("abilitaResto", "");
    nuovaConfigurazione->insert("stampante", "");
    nuovaConfigurazione->insert("cartellaPdf", "");
    nuovaConfigurazione->insert("stampantePdf", "");
    nuovaConfigurazione->insert("nomeCassa", "");
    nuovaConfigurazione->insert("descrManifestazione", "");
    nuovaConfigurazione->insert("visualizzazionePrezzo", "");
    nuovaConfigurazione->insert("serieRitiro", "A");
    nuovaConfigurazione->insert("printLogo", "");
    nuovaConfigurazione->insert("printIntestazione", "");
    nuovaConfigurazione->insert("printFondo", "");

    on_buttonBox_accepted();
}

void ConfigurazioneDlg::on_resetBuoniBtn_clicked()
{
    ConfermaDlg* dlg = new ConfermaDlg("Questa operazione cancella tutti i buoni emessi.\nProseguire?", "", false, this);
    if (QDialog::Accepted != dlg->visualizza()) return;

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from buoni")) {
        QMessageBox::critical(0, QObject::tr("Database Error"), stmt.lastError().text());
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
  QByteArray logoData;
  QBuffer logoBuffer(&logoData);
  logoBuffer.open(QIODevice::WriteOnly);
  bool rcSave = logo.save(&logoBuffer, "PNG");
  nuovaConfigurazione->insert(nomePar, logoData);
}

void ConfigurazioneDlg::on_logoIntestazioneBtn_clicked(){
  selezionaLogo("logoPixmap");
}

void ConfigurazioneDlg::on_logoFondoBtn_clicked(){
  selezionaLogo("logoFondoPixmap");
}

void ConfigurazioneDlg::on_logoCheckBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("printLogo", checked);
}

void ConfigurazioneDlg::on_logoFondoCheckBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("printLogoFondo", checked);
}

void ConfigurazioneDlg::on_intestazioneCheckBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("printIntestazione", checked);
}

void ConfigurazioneDlg::on_fondoCheckBox_clicked(bool checked)
{
    nuovaConfigurazione->insert("printFondo", checked);
}
