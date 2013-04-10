#ifndef DESTINAZIONESTAMPADLG_H
#define DESTINAZIONESTAMPADLG_H

#include "ui_destinazionestampadlg.h"

class DestinazioneStampaDlg : public QDialog, private Ui::DestinazioneStampaDlg
{
  Q_OBJECT
  
public:
  explicit DestinazioneStampaDlg(QWidget *parent = 0);
  QString getDestinazione() {return box->currentText();}

private slots:
  void on_buttonBox_accepted();
  void on_buttonBox_rejected();
};

#endif // DESTINAZIONESTAMPADLG_H
