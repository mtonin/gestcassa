#ifndef REPARTIBOX_H
#define REPARTIBOX_H

#include "ui_repartibox.h"
#include "repartobutton.h"

class RepartiBox : public QWidget, private Ui::RepartiBox
{
  Q_OBJECT

signals:
  void onNuovoReparto();
  void onSelezioneReparto(RepartoButton*);
  void onCancellazioneReparto();

private slots:
  void cancellaReparto(RepartoButton*);
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
