#ifndef CONFIGURAZIONEDLG_H
#define CONFIGURAZIONEDLG_H

#include "ui_configurazionedlg.h"

class ConfigurazioneDlg : public QDialog, private Ui::ConfigurazioneDlg
{
  Q_OBJECT
  
public:
  explicit ConfigurazioneDlg(QWidget *parent = 0);
private slots:
  void on_printerSelectBtn_clicked();
};

#endif // CONFIGURAZIONEDLG_H
