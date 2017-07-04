#include "parametriavanzati.h"
#include <QFontDialog>

ParametriAvanzati::ParametriAvanzati(QMap<QString, QVariant> *conf, QWidget *parent) : QDialog(parent)
{
  setupUi(this);
  configurazione=conf;
  risoluzioneBox->setValue(configurazione->value("PRINTERRESOLUTION",200).toInt());
  ampiezzaBox->setValue(configurazione->value("PRINTERWINWIDTH",300).toInt());
  margineSXBox->setValue(configurazione->value("MARGINESX",5).toInt());
  margineDXBox->setValue(configurazione->value("MARGINEDX",5).toInt());
  larghezzaFoglioBox->setValue(configurazione->value("LARGHEZZAFOGLIO",0).toInt());
  lunghezzaFoglioBox->setValue(configurazione->value("LUNGHEZZAFOGLIO",0).toInt());
  QFont fontScontrino;
  fontScontrino.fromString(configurazione->value("PRINTERFONT","lucida console,5").toString());
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
    configurazione->insert("PRINTERRESOLUTION",risoluzioneBox->value());
    configurazione->insert("PRINTERWINWIDTH",ampiezzaBox->value());
    configurazione->insert("MARGINESX",margineSXBox->value());
    configurazione->insert("MARGINEDX",margineDXBox->value());
    configurazione->insert("LARGHEZZAFOGLIO",larghezzaFoglioBox->value());
    configurazione->insert("LUNGHEZZAFOGLIO",lunghezzaFoglioBox->value());
    configurazione->insert("PRINTERFONT",fontBtn->font().toString());

    QDialog::accept();
}

