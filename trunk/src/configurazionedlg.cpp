#include "configurazionedlg.h"
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrinter>

ConfigurazioneDlg::ConfigurazioneDlg(QMap<QString,QVariant>* par,QWidget *parent) : configurazione(par),QDialog(parent)
{
  setupUi(this);
  nuovaConfigurazione=new QMap<QString,QVariant>;
  stampanteSelezionataTxt->setText(configurazione->value("stampante","PDF").toString());
}

void ConfigurazioneDlg::on_printerSelectBtn_clicked()
{
  QPrinter printer;
  QPageSetupDialog pageSetupDlg(&printer);
  if(QDialog::Accepted==pageSetupDlg.exec()) {
    stampanteSelezionataTxt->setText(printer.printerName());
    nuovaConfigurazione->insert("stampante",printer.printerName());
  }

}

void ConfigurazioneDlg::on_buttonBox_accepted()
{
  foreach (QString key, nuovaConfigurazione->keys()) {
    configurazione->insert(key,nuovaConfigurazione->value(key));
  }
  configurazione->insert("intestazione1",intestazione1Txt->text());
  configurazione->insert("intestazione2",intestazione2Txt->text());
  configurazione->insert("intestazione3",intestazione3Txt->text());
  return accept();
}

void ConfigurazioneDlg::on_buttonBox_rejected()
{
  return reject();
}
