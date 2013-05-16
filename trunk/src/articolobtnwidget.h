#ifndef ARTICOLOBTNWIDGET_H
#define ARTICOLOBTNWIDGET_H

#include <QPictureButton.h>

class ArticoloBtnWidget : public QPictureButton
{
  Q_OBJECT
public:
  explicit ArticoloBtnWidget(int id,int numRiga, int numColonna,QWidget *parent=0);

  int getId() {return idArticolo;}
  void setId(int id) {idArticolo=id;}
  QString getNomeArticolo() {return nomeArticolo;}
  void setNomeArticolo(const QString nome);
  int getIdReparto() {return idReparto;}
  int getRiga() {return riga;}
  int getColonna() {return colonna;}
  float getPrezzo() {return prezzo;}
  void setPrezzo(float nuovoPrezzo);
  bool getAbilitato() {return abilitato;}
  void setAbilitato(bool val) {abilitato=val;}
  QString getRepartoStampa() {return repartoStampa;}
  void setRepartoStampa(const QString nome);
  void setVisualizzaPrezzo(const bool valore) {visualizzaPrezzo=valore;}

signals:

public slots:
  void setButtonFont(const QFont& font);

private:
  int idArticolo;
  QString nomeArticolo;
  float prezzo;
  int riga,colonna;
  int idReparto;
  bool abilitato;
  QString repartoStampa;
  bool visualizzaPrezzo;

  void PaintPrezzo();
  void paintEvent(QPaintEvent *);
};

#endif // ARTICOLOBTNWIDGET_H
