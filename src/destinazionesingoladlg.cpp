#include "destinazionesingoladlg.h"

DestinazioneSingolaDlg::DestinazioneSingolaDlg(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);
}

void DestinazioneSingolaDlg::setParametri(const QString nome, const QString intestazione)
{
  nomeDestTxt->setText(nome);
  intestazioneDestTxt->setPlainText(intestazione);
}
