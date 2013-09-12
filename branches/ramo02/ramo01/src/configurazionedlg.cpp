#include "configurazionedlg.h"
#include "destinazionidlg.h"
#include "confermadlg.h"

#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

ConfigurazioneDlg::ConfigurazioneDlg(QMap<QString,QVariant>* par,QWidget *parent) : configurazione(par),QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);

  cifratore=new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));

  nuovaConfigurazione=new QMap<QString,QVariant>;
  cartellaPdfTxt->setText(configurazione->value("cartellaPdf","").toString());
  if(configurazione->value("stampantePdf",true).toBool()) {
    pdfChk->setChecked(true);
  } else {
    stampanteChk->setChecked(true);
  }

  QString stampanteSelezionata=configurazione->value("stampante").toString();
  QList<QPrinterInfo> listaStampanti=QPrinterInfo::availablePrinters();
  int idx=0;
  foreach (QPrinterInfo stampante, listaStampanti) {
    stampanteBox->addItem(stampante.printerName());
    if(stampante.printerName()==stampanteSelezionata) {
      stampanteBox->setCurrentIndex(idx);
    }
    idx++;
  }
  intestazioneScontrinoTxt->setPlainText(configurazione->value("intestazione").toString());
  fondoScontrinoTxt->setPlainText(configurazione->value("fondo").toString());
  durataRestoTxt->setText(configurazione->value("durataResto",5).toString());
  if(configurazione->value("abilitaResto",false).toBool()) {
    attivaRestoCheck->setChecked(true);
  }
  nomeCassaTxt->setText(configurazione->value("nomeCassa").toString());
  descrManifestazioneTxt->setText(configurazione->value("descrManifestazione").toString());
  visualizzaPrezzoBox->setChecked(configurazione->value("visualizzazionePrezzo").toBool());
  adminPasswordTxt->setText(configurazione->value("adminPassword").toString());
  dbPathTxt->setPlainText(configurazione->value("dbFilePath").toString());

  tabWidget->setCurrentIndex(0);
  descrManifestazioneTxt->setFocus();
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
  if(QDialog::Accepted==pageSetupDlg.exec()) {
  }

  //nuovaConfigurazione->insert("stampante",printer.printerName());

}

void ConfigurazioneDlg::on_buttonBox_accepted()
{
  if(configurazione->value("adminPassword").toString()!=adminPasswordTxt->text()) {
    QString pwd=cifratore->encryptToString(adminPasswordTxt->text());
    configurazione->insert("adminPassword",pwd);
    emit passwordCambiata();
  }

  foreach (QString key, nuovaConfigurazione->keys()) {
    configurazione->insert(key,nuovaConfigurazione->value(key));
  }

  QSqlQuery stmt;
  foreach (QString key, configurazione->keys()) {
    stmt.clear();
    stmt.prepare("insert or replace into configurazione (chiave,valore) values (?,?)");
    stmt.addBindValue(key);
    stmt.addBindValue(configurazione->value(key).toString());
    if(!stmt.exec()) {
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
    nuovaConfigurazione->insert("abilitaResto",checked);
}

void ConfigurazioneDlg::on_stampanteChk_toggled(bool checked)
{
  stampanteBox->setEnabled(checked);
  printerSelectBtn->setEnabled(checked);
  nuovaConfigurazione->insert("stampantePdf",!checked);
}

void ConfigurazioneDlg::on_pdfChk_toggled(bool checked)
{
    cartellaPdfBtn->setEnabled(checked);
    cartellaPdfTxt->setEnabled(checked);
    nuovaConfigurazione->insert("stampantePdf",checked);
}

void ConfigurazioneDlg::on_cartellaPdfBtn_clicked()
{
  QString cartella;
  cartella=QFileDialog::getExistingDirectory(0,"Selezionare una cartella","c:\\");
  if(!cartella.isEmpty()) {
    cartellaPdfTxt->setText(cartella);
  }
  nuovaConfigurazione->insert("cartellaPdf",cartella);

}

void ConfigurazioneDlg::on_cartellaPdfTxt_textEdited(const QString &arg1)
{
  nuovaConfigurazione->insert("cartellaPdf",arg1);
}

void ConfigurazioneDlg::on_nomeCassaTxt_textEdited(const QString &arg1)
{
  nuovaConfigurazione->insert("nomeCassa",arg1);
}

void ConfigurazioneDlg::on_durataRestoTxt_textEdited(const QString &arg1)
{
  nuovaConfigurazione->insert("durataResto",arg1);
}

void ConfigurazioneDlg::on_descrManifestazioneTxt_textEdited(const QString &arg1)
{
  nuovaConfigurazione->insert("descrManifestazione",arg1);
}

void ConfigurazioneDlg::on_adminPasswordTxt_textEdited(const QString &arg1)
{
}

void ConfigurazioneDlg::on_intestazioneScontrinoTxt_textChanged()
{
  nuovaConfigurazione->insert("intestazione",intestazioneScontrinoTxt->toPlainText());
}

void ConfigurazioneDlg::on_fondoScontrinoTxt_textChanged()
{
  nuovaConfigurazione->insert("fondo",fondoScontrinoTxt->toPlainText());
}

void ConfigurazioneDlg::on_visualizzaPrezzoBox_clicked(bool checked)
{
  nuovaConfigurazione->insert("visualizzazionePrezzo",checked);
  emit cambiaVisualizzaPrezzo(checked);
}

void ConfigurazioneDlg::on_cancellaOrdiniBtn_clicked()
{
  ConfermaDlg* dlg=new ConfermaDlg("Questa operazione azzera il contatore degli ordini.\nProseguire?","",false,this);
  if(QDialog::Accepted!=dlg->visualizza()) return;

  QSqlDatabase db=QSqlDatabase::database();
  db.transaction();

  QSqlQuery stmt;
  if(!stmt.exec("delete from ordinirighe")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    db.rollback();
    return;
  }
  if(!stmt.exec("delete from ordini")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    db.rollback();
    return;
  }

  int idSessioneCorrente=configurazione->value("sessioneCorrente").toInt();
  idSessioneCorrente++;

  stmt.prepare("insert into sessione (idsessione,tsinizio) values (?,datetime('now'))");
  stmt.addBindValue(idSessioneCorrente);

  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    db.rollback();
    return;
  }

  configurazione->insert("sessioneCorrente",idSessioneCorrente);

  db.commit();
  emit resetOrdini(idSessioneCorrente);

}

void ConfigurazioneDlg::on_stampanteBox_activated(const QString &arg1)
{
  nuovaConfigurazione->insert("stampante",arg1);
}

void ConfigurazioneDlg::on_exportOrdiniBtn_clicked()
{

  QString separatoreRighe="#§EOL#§";
  QStringList listaSql;

  QSqlQuery stmt("select numeroordine, tsstampa, importo, descrizione, quantita, destinazione, prezzo, tipoArticolo from storicoordini");
  if(!stmt.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString numeroOrdine=stmt.value(0).toString();
    QString tsStampaOrdine=stmt.value(1).toString();
    QString importoOrdine=stmt.value(2).toString();
    QString descrizioneArticoloOrdine=stmt.value(3).toString();
    QString quantitaArticoloOrdine=stmt.value(4).toString();
    QString destinazioneArticoloOrdine=stmt.value(5).toString();
    QString prezzoArticoloOrdine=stmt.value(6).toString();
    QString tipoArticoloOrdine=stmt.value(7).toString();
    QString riga=QString("ORDINI#§%1#§%2#§%3#§%4#§%5#§%6#§%7#§%8")
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
  QString separatoreRighe="#§EOL#§";
  QStringList exportLista;
  QSqlQuery stmt;

  if(!stmt.exec("select nome,coalesce(intestazione,'NULL') from destinazionistampa")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString nomeDest=stmt.value(0).toString();
    QString intestazioneDest=stmt.value(1).toString();
    QString riga=QString("DESTINAZIONISTAMPA#§%1#§%2")
                 .arg(nomeDest)
                 .arg(intestazioneDest);
    exportLista.append(riga);
  }

  if(!stmt.exec("select idreparto,descrizione,coalesce(font,'NULL'),coalesce(coloresfondo,'NULL'),coalesce(colorecarattere,'NULL'),abilitato from reparti")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString idreparto=stmt.value(0).toString();
    QString descrizioneReparto=stmt.value(1).toString();
    QString font=stmt.value(2).toString();
    QString coloreSfondo=stmt.value(3).toString();
    QString coloreCarattere=stmt.value(4).toString();
    bool abilitato=stmt.value(5).toBool();
    QString riga=QString("REPARTI#§%1#§%2#§%3#§%4#§%5#§%6")
                 .arg(idreparto)
                 .arg(descrizioneReparto)
                 .arg(font)
                 .arg(coloreSfondo)
                 .arg(coloreCarattere)
                 .arg(abilitato);
    exportLista.append(riga);
  }

  if(!stmt.exec("select idarticolo,descrizione,prezzo,coalesce(destinazione,'NULL'),gestioneMenu from articoli")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString idArticolo=stmt.value(0).toString();
    QString descrizioneArticolo=stmt.value(1).toString();
    QString prezzo=stmt.value(2).toString();
    QString destinazioneStampa=stmt.value(3).toString();
    QString gestioneMenu=stmt.value(4).toString();
    QString riga=QString("ARTICOLI#§%1#§%2#§%3#§%4#§%5")
                 .arg(idArticolo)
                 .arg(descrizioneArticolo)
                 .arg(prezzo)
                 .arg(destinazioneStampa)
                 .arg(gestioneMenu);
    exportLista.append(riga);
  }

  if(!stmt.exec("select idarticolo,idarticolomenu from articolimenu")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString idArticolo=stmt.value(0).toString();
    QString idArticoloMenu=stmt.value(1).toString();
    QString riga=QString("ARTICOLIMENU#§%1#§%2")
                 .arg(idArticolo)
                 .arg(idArticoloMenu);
    exportLista.append(riga);
  }

  if(!stmt.exec("select idreparto,riga,colonna,idarticolo,abilitato from pulsanti")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString idReparto=stmt.value(0).toString();
    QString rigaPulsante=stmt.value(1).toString();
    QString colonnaPulsante=stmt.value(2).toString();
    QString idArticolo=stmt.value(3).toString();
    QString flagAbilitato=stmt.value(4).toString();
    QString riga=QString("PULSANTI#§%1#§%2#§%3#§%4#§%5")
                 .arg(idReparto)
                 .arg(rigaPulsante)
                 .arg(colonnaPulsante)
                 .arg(idArticolo)
                 .arg(flagAbilitato);
    exportLista.append(riga);
  }

  QString str=configurazione->value("descrManifestazione").toString();
  QString riga=QString("CONFIGURAZIONE#§descrManifestazione#§%1")
          .arg(str.isEmpty()?"NULL":str);
  exportLista.append(riga);

  str=configurazione->value("intestazione").toString();
  riga=QString("CONFIGURAZIONE#§intestazione#§%1")
          .arg(str.isEmpty()?"NULL":str);
  exportLista.append(riga);

  str=configurazione->value("fondo").toString();
  riga=QString("CONFIGURAZIONE#§fondo#§%1")
          .arg(str.isEmpty()?"NULL":str);
  exportLista.append(riga);

  esportaInFile(exportLista.join(separatoreRighe));
}

void ConfigurazioneDlg::esportaInFile(const QString &testo)
{
  QString nomeFile;
  nomeFile=QFileDialog::getSaveFileName(0,"Esporta in");
  if(nomeFile.isEmpty()) {
    return;
  }
  QFile exportFile(nomeFile);
  exportFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate);
  QTextStream exportStream(&exportFile);

  exportStream << testo;
  exportFile.close();
}

QVariant ConfigurazioneDlg::valutaStringa(const QString &str)
{
    if(0==str.compare("null",Qt::CaseInsensitive)) {
        return QVariant(QVariant::String);
    } else {
        return QVariant(str);
    }
}

void ConfigurazioneDlg::on_importArticoliBtn_clicked()
{
  QString nomeFile;
  nomeFile=QFileDialog::getOpenFileName(0,"Importa da");
  if(nomeFile.isEmpty()) {
    return;
  }
  QFile importFile(nomeFile);
  importFile.open(QIODevice::ReadOnly|QIODevice::Text);

  QSqlDatabase db=QSqlDatabase::database();
  db.transaction();

  QSqlQuery stmt;
  if(!stmt.exec("delete from pulsanti") ||
     !stmt.exec("delete from articolimenu") ||
     !stmt.exec("delete from articoli") ||
     !stmt.exec("delete from destinazionistampa") ||
     !stmt.exec("delete from reparti")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    db.rollback();
    return;
  }

  QString inputText=importFile.readAll();
  QString separatoreRighe="#§EOL#§";
  if(-1==inputText.indexOf(separatoreRighe)) {
    QMessageBox::critical(0, QObject::tr("Import Error"),"Il formato del file è obsoleto.\nImpossibile eseguire l'importazione");
    db.rollback();
    return;
  }
  QString separatoreCampi="#§";
  if(inputText.indexOf("Â§")>0) {
    separatoreCampi="Â§";
  }
  QStringList righeInput=inputText.split(separatoreRighe);
  foreach (QString rigaSingola,righeInput) {
    QStringList campiInput=rigaSingola.split(separatoreCampi);
    int idx=0;
    while(idx<campiInput.size()) {
        QString tabella=campiInput.at(idx);
        QString sql;
        if(0==tabella.compare("destinazionistampa",Qt::CaseInsensitive)) {
            stmt.prepare("INSERT INTO DESTINAZIONISTAMPA (nome,intestazione) VALUES(?,?)");
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
        }
        if(0==tabella.compare("pulsanti",Qt::CaseInsensitive)) {
            stmt.prepare("INSERT INTO PULSANTI (idReparto,riga,colonna,idArticolo,abilitato) VALUES(?,?,?,?,?)");
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
        }
        if(0==tabella.compare("reparti",Qt::CaseInsensitive)) {
            stmt.prepare("INSERT INTO REPARTI (idReparto,descrizione,font,coloreSfondo,coloreCarattere,abilitato) VALUES(?,?,?,?,?,?)");
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            if(idx>=campiInput.size()-1)
              stmt.addBindValue(valutaStringa("true"));
            else
              stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
        }
        if(0==tabella.compare("articoli",Qt::CaseInsensitive)) {
            stmt.prepare("INSERT INTO ARTICOLI (idArticolo,descrizione,prezzo,destinazione,gestioneMenu) VALUES(?,?,?,?,?)");
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
        }
        if(0==tabella.compare("articolimenu",Qt::CaseInsensitive)) {
            stmt.prepare("INSERT INTO ARTICOLIMENU (idArticolo,idArticoloMenu) VALUES(?,?)");
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
        }
        if(0==tabella.compare("configurazione",Qt::CaseInsensitive)) {
            stmt.prepare("INSERT OR REPLACE INTO CONFIGURAZIONE VALUES(?,?)");
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
            stmt.addBindValue(valutaStringa(campiInput.at(++idx)));
        }

        if(!stmt.exec()) {
          QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
          db.rollback();
          return;
        }
        idx++;
    }

  }

  db.commit();
  emit resetArticoli();

  if(!stmt.exec("select valore from configurazione where chiave='descrManifestazione'")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    db.rollback();
    return;
  }
  if(stmt.next()) {
    nuovaConfigurazione->insert("descrManifestazione",stmt.value(0).toString());
  }

  if(!stmt.exec("select valore from configurazione where chiave='intestazione'")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    db.rollback();
    return;
  }
  if(stmt.next()) {
    nuovaConfigurazione->insert("intestazione",stmt.value(0).toString());
  }

  if(!stmt.exec("select valore from configurazione where chiave='fondo'")) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    db.rollback();
    return;
  }
  if(stmt.next()) {
    nuovaConfigurazione->insert("fondo",stmt.value(0).toString());
  }

  on_buttonBox_accepted();
}
