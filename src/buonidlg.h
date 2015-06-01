#ifndef BUONIDLG_H
#define BUONIDLG_H

#include "ui_buonidlg.h"
#include "buonimodel.h"

class BuoniDlg : public QDialog, private Ui::BuoniDlg
{
  Q_OBJECT

public:
  explicit BuoniDlg(QWidget *parent = 0);

private:
  buoniModel* modello;

};

#endif // BUONIDLG_H
