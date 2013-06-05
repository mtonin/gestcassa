#include "configurazionedlg.h"
#include "destinazionidlg.h"

#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>

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
  durataRestoTxt->setText(configurazione->value("durataResto",5).toString());
  if(configurazione->value("abilitaResto",false).toBool()) {
    attivaRestoCheck->setChecked(true);
  }
  nomeCassaTxt->setText(configurazione->value("nomeCassa").toString());
  descrManifestazioneTxt->setText(configurazione->value("descrManifestazione").toString());
  visualizzaPrezzoBox->setChecked(configurazione->value("visualizzazionePrezzo").toBool());
  adminPasswordTxt->setText(configurazione->value("adminPassword").toString());

  tabWidget->setCurrentIndex(0);
  descrManifestazioneTxt->setFocus();
}

void ConfigurazioneDlg::on_printerSelectBtn_clicked()
{
  QPrinter printer;

  QPrintDialog printDlg(&printer);
  if(QDialog::Accepted==printDlg.exec()) {
  }
  /*
  QPageSetupDialog pageSetupDlg(&printer);
  if(QDialog::Accepted==pageSetupDlg.exec()) {
  }
  */

  //nuovaConfigurazione->insert("stampante",printer.printerName());

}

void ConfigurazioneDlg::on_buttonBox_accepted()
{
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
    configurazione->insert("abilitaResto",checked);
}

void ConfigurazioneDlg::on_stampanteChk_toggled(bool checked)
{
  stampanteBox->setEnabled(checked);
  printerSelectBtn->setEnabled(checked);
  configurazione->insert("stampantePdf",!checked);
}

void ConfigurazioneDlg::on_pdfChk_toggled(bool checked)
{
    cartellaPdfBtn->setEnabled(checked);
    cartellaPdfTxt->setEnabled(checked);
    configurazione->insert("stampantePdf",checked);
}

void ConfigurazioneDlg::on_cartellaPdfBtn_clicked()
{
  QString cartella;
  cartella=QFileDialog::getExistingDirectory(0,"Selezionare una cartella","c:\\");
  if(!cartella.isEmpty()) {
    cartellaPdfTxt->setText(cartella);
  }
  configurazione->insert("cartellaPdf",cartella);

}

void ConfigurazioneDlg::on_cartellaPdfTxt_textEdited(const QString &arg1)
{
  configurazione->insert("cartellaPdf",arg1);
}

void ConfigurazioneDlg::on_nomeCassaTxt_textEdited(const QString &arg1)
{
  configurazione->insert("nomeCassa",arg1);
}

void ConfigurazioneDlg::on_durataRestoTxt_textEdited(const QString &arg1)
{
  configurazione->insert("durataResto",arg1);
}

void ConfigurazioneDlg::on_descrManifestazioneTxt_textEdited(const QString &arg1)
{
  configurazione->insert("descrManifestazione",arg1);
}

void ConfigurazioneDlg::on_adminPasswordTxt_textEdited(const QString &arg1)
{
  QString pwd=cifratore->encryptToString(arg1);
  configurazione->insert("adminPassword",pwd);
}

void ConfigurazioneDlg::on_intestazioneScontrinoTxt_textChanged()
{
  configurazione->insert("intestazione",intestazioneScontrinoTxt->toPlainText());
}

void ConfigurazioneDlg::on_visualizzaPrezzoBox_clicked(bool checked)
{
  configurazione->insert("visualizzazionePrezzo",checked);
}

void ConfigurazioneDlg::on_cancellaOrdiniBtn_clicked()
{
  if(QMessageBox::Yes!=QMessageBox::question(this,"Cancellazione ordini","Cancellare tutti gli ordini?",QMessageBox::Yes|QMessageBox::No)) {
    return;
  }
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

  db.commit();
  emit resetOrdini();

}

void ConfigurazioneDlg::on_stampanteBox_activated(const QString &arg1)
{
  nuovaConfigurazione->insert("stampante",arg1);
}

void ConfigurazioneDlg::on_exportOrdiniBtn_clicked()
{
  QSqlQuery stmt("select a.numero,a.tsstampa, a.importo,b.quantita,c.descrizione \
               from ordini a,ordinirighe b, articoli c \
               where a.numero=b.numeroordine \
               and b.idarticolo=c.idarticolo");
  if(!stmt.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString numeroOrdine=stmt.value(0).toString();
    QString tsStampaOrdine=stmt.value(1).toString();
    QString importoOrdine=stmt.value(2).toString();
    QString quantitaArticoloOrdine=stmt.value(3).toString();
    QString descrizioneArticoloOrdine=stmt.value(4).toString();
    QString riga=QString("%1,%2,%3,%4,%5")
                 .arg(numeroOrdine)
                 .arg(tsStampaOrdine)
                 .arg(importoOrdine)
                 .arg(quantitaArticoloOrdine)
                 .arg(descrizioneArticoloOrdine);
    qDebug(riga.toUtf8());
  }

}
