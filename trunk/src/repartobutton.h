#ifndef REPARTOBUTTON_H
#define REPARTOBUTTON_H

#include "ui_repartobutton.h"

class RepartoButton : public QWidget, private Ui::RepartoButton
{
  Q_OBJECT

signals:
  void cancellaReparto(RepartoButton*);
  void creaReparto(RepartoButton*);
  void onClickReparto(RepartoButton*);

public:
  explicit RepartoButton(int id, QString nome, QWidget *parent = 0);
  int getId() {return idReparto;}
  QString getNomeReparto() {return nomeReparto;}

private:
  int idReparto;
  QString nomeReparto;
  QString coloreSfondo;

private slots:
  void dettagliShow();
  void cancella();
  void onClick();
  void crea();
};

#endif // REPARTOBUTTON_H
