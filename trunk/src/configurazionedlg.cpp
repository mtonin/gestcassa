#include "configurazionedlg.h"
#include "destinazionidlg.h"

#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrinter>
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>

ConfigurazioneDlg::ConfigurazioneDlg(QMap<QString,QVariant>* par,QWidget *parent) : configurazione(par),QDialog(parent)
{
  setupUi(this);

  cifratore=new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));

  nuovaConfigurazione=new QMap<QString,QVariant>;
  cartellaPdfTxt->setText(configurazione->value("cartellaPdf","c:\\").toString());
  if(configurazione->value("stampantePdf",true).toBool()) {
    pdfChk->setChecked(true);
  } else {
    stampanteChk->setChecked(true);
  }
  stampanteSelezionataTxt->setText(configurazione->value("stampante").toString());
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
  nomeCassaTxt->setFocus();
}

void ConfigurazioneDlg::on_printerSelectBtn_clicked()
{
  QPrinter printer;
  QPageSetupDialog pageSetupDlg(&printer);
  if(QDialog::Accepted==pageSetupDlg.exec()) {
    stampanteSelezionataTxt->setText(printer.printerName());
  }

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
    stampanteSelezionataTxt->setEnabled(checked);
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

}

void ConfigurazioneDlg::on_cartellaPdfTxt_textEdited(const QString &arg1)
{
  configurazione->insert("cartellaPdf",arg1);
}

void ConfigurazioneDlg::on_stampanteSelezionataTxt_textEdited(const QString &arg1)
{
  nuovaConfigurazione->insert("stampante",arg1);
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

void ConfigurazioneDlg::on_gestioneMenuBox_clicked(bool checked)
{
  configurazione->insert("gestioneMenu",checked);
}
