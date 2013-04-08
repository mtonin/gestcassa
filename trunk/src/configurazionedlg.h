#ifndef CONFIGURAZIONEDLG_H
#define CONFIGURAZIONEDLG_H

#include "ui_configurazionedlg.h"
#include <QMap>

class ConfigurazioneDlg : public QDialog, private Ui::ConfigurazioneDlg
{
  Q_OBJECT
  
public:
  explicit ConfigurazioneDlg(QMap<QString,QVariant>* par,QWidget *parent = 0);
  QMap<QString,QVariant> getConfigurazione() {return *configurazione;}

private slots:
  void on_printerSelectBtn_clicked();

  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
  QMap<QString,QVariant>* configurazione;
  QMap<QString,QVariant>* nuovaConfigurazione;
};

#endif // CONFIGURAZIONEDLG_H
