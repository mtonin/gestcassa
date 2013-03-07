#ifndef ARTICOLOBTNWIDGET_H
#define ARTICOLOBTNWIDGET_H

#include <QPictureButton.h>

class ArticoloBtnWidget : public QPictureButton
   {
   Q_OBJECT
public:
   explicit ArticoloBtnWidget(QWidget *parent = 0);
   int getId() {return idArticolo;}
   QString getNomeArticolo() {return nomeArticolo;}
   void setNomeArticolo(const QString nome);

signals:
   
public slots:
   
private:
  int idArticolo;
  QString nomeArticolo;
  float prezzo;

   };

#endif // ARTICOLOBTNWIDGET_H
