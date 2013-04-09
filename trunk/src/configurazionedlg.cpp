#include "configurazionedlg.h"
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrinter>
#include <QtSql>
#include <QMessageBox>

ConfigurazioneDlg::ConfigurazioneDlg(QMap<QString,QVariant>* par,QWidget *parent) : configurazione(par),QDialog(parent)
{
  setupUi(this);
  nuovaConfigurazione=new QMap<QString,QVariant>;
  stampanteSelezionataTxt->setText(configurazione->value("stampante","PDF").toString());
  intestazione1Txt->setText(configurazione->value("intestazione1").toString());
  intestazione2Txt->setText(configurazione->value("intestazione2").toString());
  intestazione3Txt->setText(configurazione->value("intestazione3").toString());
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

  QSqlQuery stmt;
  foreach (QString key, configurazione->keys()) {
    stmt.clear();
    stmt.prepare("insert or replace into configurazione (chiave,valore) values (?,?)");
    stmt.addBindValue(key);
    stmt.addBindValue(configurazione->value(key).toString());
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt.lastError().text());
      return accept();
    }
  }

  return accept();
}

void ConfigurazioneDlg::on_buttonBox_rejected()
{
  return reject();
}
