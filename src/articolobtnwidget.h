#ifndef ARTICOLOBTNWIDGET_H
#define ARTICOLOBTNWIDGET_H

#include <QPictureButton.h>

class ArticoloBtnWidget : public QPictureButton
{
  Q_OBJECT
public:
  explicit ArticoloBtnWidget(int id,int idRep,int numRiga, int numColonna,QWidget *parent=0);
  explicit ArticoloBtnWidget(int id,QMap<QString,QVariant>* ,QWidget *parent=0);

  int getId() {return idArticolo;}
  void setId(int id) {idArticolo=id;}
  int getIdPulsante() {return idPulsante;}
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
  bool isGestioneMenu() {return gestioneMenu;}
  void setGestioneMenu(bool val) {gestioneMenu=val;}
  void setPos(int r,int c);
  void setDragAbilitato(bool valore);

signals:
  void swapSignal(int id1, int id2);

public slots:
  void setButtonFont(const QFont& font);
  void setColore(const QColor &colore);
  void setColoreText(const QColor &colore);

private:
  int idArticolo;
  QString nomeArticolo;
  float prezzo;
  int riga,colonna;
  int idReparto;
  bool abilitato;
  QString repartoStampa;
  bool visualizzaPrezzo;
  bool gestioneMenu;
  int idPulsante;
  QPoint dragStartPos;
  bool dragAbilitato;

protected:

  void PaintPrezzo();
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *e);
  void dragEnterEvent(QDragEnterEvent *e);
  void dragLeaveEvent(QDragLeaveEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void dropEvent(QDropEvent *e);
};

#endif // ARTICOLOBTNWIDGET_H
