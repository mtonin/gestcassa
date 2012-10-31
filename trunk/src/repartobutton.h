#ifndef REPARTOBUTTON_H
#define REPARTOBUTTON_H

#include "ui_repartobutton.h"

class RepartoButton : public QWidget, private Ui::RepartoButton
{
  Q_OBJECT

signals:
  void cancellaReparto(RepartoButton*);

public:
  explicit RepartoButton(int id, QString nome, QWidget *parent = 0);
  int getId() {return idReparto;}

private:
  int idReparto;
  QString nomeReparto;
  QString coloreSfondo;

private slots:
  void dettagliShow();
  void cancella();
};

#endif // REPARTOBUTTON_H
