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
  return accept();
}

void ConfigurazioneDlg::on_buttonBox_rejected()
{
  return reject();
}
