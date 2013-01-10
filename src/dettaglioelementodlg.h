#ifndef DETTAGLIOELEMENTODLG_H
#define DETTAGLIOELEMENTODLG_H

#include "ui_dettaglioelementodlg.h"
#include <QDialog>

class dettaglioElementoDlg : public QDialog,private Ui::dettaglioElementoDlg
{
  Q_OBJECT
  
public:
  explicit dettaglioElementoDlg(QWidget *parent = 0);
  ~dettaglioElementoDlg();
  int showAtMousePosition();
  QString getDescrizione() {return descrizioneBox->text();}

private slots:
  void on_buttonBox_accepted();
  void on_buttonBox_rejected();
};

#endif // DETTAGLIOELEMENTODLG_H
