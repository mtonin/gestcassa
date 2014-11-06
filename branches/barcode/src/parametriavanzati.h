#ifndef PARAMETRIAVANZATI_H
#define PARAMETRIAVANZATI_H

#include "ui_parametriavanzati.h"

class ParametriAvanzati : public QDialog, private Ui::ParametriAvanzati
{
  Q_OBJECT

public:
  explicit ParametriAvanzati(QWidget *parent = 0);
  void setRisoluzione(int arg) {risoluzioneBox->setValue(arg);}
  int getRisoluzione() {return risoluzioneBox->value();}
  void setAmpiezzaStampa(int arg) {ampiezzaBox->setValue(arg);}
  int getAmpiezzaStampa() {return ampiezzaBox->value();}
  void setDimensioneFontStampa(int arg) {dimFontBox->setValue(arg);}
  int getDimensioneFontStampa() {return dimFontBox->value();}
};

#endif // PARAMETRIAVANZATI_H
