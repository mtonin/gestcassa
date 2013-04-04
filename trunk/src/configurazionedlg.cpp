#include "configurazionedlg.h"
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrinter>

ConfigurazioneDlg::ConfigurazioneDlg(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);
}

void ConfigurazioneDlg::on_printerSelectBtn_clicked()
{
  QPrinter printer;
  QPageSetupDialog pageSetupDlg(&printer);
  if(QDialog::Accepted==pageSetupDlg.exec()) {
    stampanteSelezionataTxt->setText(printer.printerName());
  }

  /*
  QPrintDialog selezionaStampanteDlg(&printer);
  if(QDialog::Accepted==selezionaStampanteDlg.exec()) {
    stampanteSelezionataTxt->setText(printer.printerName());
  }
  */
}
