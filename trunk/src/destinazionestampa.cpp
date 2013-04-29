#include "destinazionestampa.h"

DestinazioneStampa::DestinazioneStampa(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);
}

void DestinazioneStampa::on_buttonBox_accepted()
{
    accept();
}

void DestinazioneStampa::on_buttonBox_rejected()
{
    reject();
}
