#ifndef PARAMETRIAVANZATI_H
#define PARAMETRIAVANZATI_H

#include "ui_parametriavanzati.h"
#include <QMap>

class ParametriAvanzati : public QDialog, private Ui::ParametriAvanzati
{
  Q_OBJECT

private slots:
    void on_fontBtn_clicked();
    void accept();

public:
  explicit ParametriAvanzati(QMap<QString, QVariant>* conf,QWidget *parent = 0);

private:
  QMap<QString, QVariant>* configurazione;
};

#endif // PARAMETRIAVANZATI_H
