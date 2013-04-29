#ifndef DESTINAZIONESTAMPA_H
#define DESTINAZIONESTAMPA_H

#include "ui_destinazionestampa.h"

class DestinazioneStampa : public QDialog, private Ui::DestinazioneStampa
{
  Q_OBJECT
  
public:
  explicit DestinazioneStampa(QWidget *parent = 0);
  void setNome(const QString val) {nomeTxt->setText(val);}
  void setintestazione(const QString val) {intestazione1Txt->setText(val);}
  QString getNome() {return nomeTxt->text();}
  QString getIntestazione() {return intestazione1Txt->text();}

private slots:
  void on_buttonBox_accepted();
  void on_buttonBox_rejected();
};

#endif // DESTINAZIONESTAMPA_H
