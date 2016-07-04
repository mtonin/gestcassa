#include "parametriavanzati.h"
#include <QFontDialog>

ParametriAvanzati::ParametriAvanzati(QMap<QString, QVariant> *conf, QWidget *parent) : QDialog(parent)
{
  setupUi(this);
  configurazione=conf;
  risoluzioneBox->setValue(configurazione->value("printerResolution",200).toInt());
  ampiezzaBox->setValue(configurazione->value("printerWinWidth",300).toInt());
  margineSXBox->setValue(configurazione->value("margineSX",5).toInt());
  margineDXBox->setValue(configurazione->value("margineDX",5).toInt());
  larghezzaFoglioBox->setValue(configurazione->value("larghezzaFoglio",0).toInt());
  lunghezzaFoglioBox->setValue(configurazione->value("lunghezzaFoglio",0).toInt());
  QFont fontScontrino;
  fontScontrino.fromString(configurazione->value("printerFont","lucida console,5").toString());
  fontBtn->setFont(fontScontrino);
}

void ParametriAvanzati::on_fontBtn_clicked(){
    QFontDialog dlg;
    dlg.setCurrentFont(fontBtn->font());
    if (dlg.exec()) {
        QFont currentFont = dlg.currentFont();
        fontBtn->setFont(currentFont);
    }
}

void ParametriAvanzati::accept()
{
    configurazione->insert("printerResolution",risoluzioneBox->value());
    configurazione->insert("printerWinWidth",ampiezzaBox->value());
    configurazione->insert("margineSX",margineSXBox->value());
    configurazione->insert("margineDX",margineDXBox->value());
    configurazione->insert("larghezzaFoglio",larghezzaFoglioBox->value());
    configurazione->insert("lunghezzaFoglio",lunghezzaFoglioBox->value());
    configurazione->insert("printerFont",fontBtn->font().toString());

    QDialog::accept();
}

