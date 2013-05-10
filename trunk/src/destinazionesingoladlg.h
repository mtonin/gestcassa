#ifndef DESTINAZIONESINGOLADLG_H
#define DESTINAZIONESINGOLADLG_H

#include "ui_destinazionesingoladlg.h"

class DestinazioneSingolaDlg : public QDialog, private Ui::DestinazioneSingolaDlg
{
  Q_OBJECT
  
public:
  explicit DestinazioneSingolaDlg(QWidget *parent = 0);
  void setParametri(const QString nome, const QString intestazione);
};

#endif // DESTINAZIONESINGOLADLG_H
