#ifndef DESTINAZIONESTAMPADLG_H
#define DESTINAZIONESTAMPADLG_H

#include "ui_destinazionestampadlg.h"

class DestinazioneStampaDlg : public QDialog, private Ui::DestinazioneStampaDlg
{
  Q_OBJECT
  
public:
  explicit DestinazioneStampaDlg(const QString& nome, QWidget *parent = 0);
private slots:
  void on_buttonBox_accepted();
};

#endif // DESTINAZIONESTAMPADLG_H
