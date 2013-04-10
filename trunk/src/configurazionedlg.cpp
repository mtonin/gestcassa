#include "configurazionedlg.h"
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrinter>
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>

ConfigurazioneDlg::ConfigurazioneDlg(QMap<QString,QVariant>* par,QWidget *parent) : configurazione(par),QDialog(parent)
{
  setupUi(this);
  nuovaConfigurazione=new QMap<QString,QVariant>;
  cartellaPdfTxt->setText(configurazione->value("cartellaPdf","c:\\").toString());
  if(configurazione->value("stampantePdf",true).toBool()) {
    pdfChk->setChecked(true);
  } else {
    stampanteChk->setChecked(true);
  }
  stampanteSelezionataTxt->setText(configurazione->value("stampante").toString());
  intestazione1Txt->setText(configurazione->value("intestazione1").toString());
  intestazione2Txt->setText(configurazione->value("intestazione2").toString());
  intestazione3Txt->setText(configurazione->value("intestazione3").toString());
  durataRestoTxt->setText(configurazione->value("durataResto",5).toString());
  if(configurazione->value("abilitaResto",false).toBool()) {
    attivaRestoCheck->setChecked(true);
  }
  nomeCassaTxt->setText(configurazione->value("nomeCassa").toString());

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

void ConfigurazioneDlg::on_cartellaPdfTxt_textChanged(const QString &arg1)
{
  configurazione->insert("cartellaPdf",arg1);
}

void ConfigurazioneDlg::on_stampanteSelezionataTxt_textChanged(const QString &arg1)
{
  nuovaConfigurazione->insert("stampante",arg1);
}

void ConfigurazioneDlg::on_intestazione1Txt_textChanged(const QString &arg1)
{
  configurazione->insert("intestazione1",arg1);
}

void ConfigurazioneDlg::on_intestazione2Txt_textChanged(const QString &arg1)
{
  configurazione->insert("intestazione2",arg1);
}

void ConfigurazioneDlg::on_intestazione3Txt_textChanged(const QString &arg1)
{
  configurazione->insert("intestazione3",arg1);
}

void ConfigurazioneDlg::on_nomeCassaTxt_textChanged(const QString &arg1)
{
  configurazione->insert("nomeCassa",arg1);
}

void ConfigurazioneDlg::on_durataRestoTxt_textChanged(const QString &arg1)
{
  configurazione->insert("durataResto",arg1);
}
