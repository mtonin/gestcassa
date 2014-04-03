#ifndef CONFERMADLG_H
#define CONFERMADLG_H

#include "ui_confermadlg.h"

class ConfermaDlg : public QDialog, private Ui::ConfermaDlg
{
  Q_OBJECT

public:
  explicit ConfermaDlg(const QString& descrizione, const QString& descrizioneCampoInput=0, bool isPassword = false, QWidget *parent = 0);
  QString getValore() {return valoreInputTxt->text();}
  int visualizza();

private:
  explicit ConfermaDlg(QWidget *parent = 0);
};

#endif // CONFERMADLG_H
