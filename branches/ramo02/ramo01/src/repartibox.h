#ifndef REPARTIBOX_H
#define REPARTIBOX_H

#include "ui_repartibox.h"

class RepartiBox : public QWidget, private Ui::areaLavoro
{
  Q_OBJECT

signals:
  void onNuovoReparto();
  void onCancellazioneReparto();

private slots:
  void creaNewReparto();

public slots:
  void caricaDaDatabase();

public:
  explicit RepartiBox(QWidget *parent = 0);

private:
  void creaMenu();
  void creaRepartoBtn(int id,QString nome);
};

#endif // REPARTIBOX_H
