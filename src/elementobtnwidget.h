#ifndef ELEMENTOBTNWIDGET_H
#define ELEMENTOBTNWIDGET_H

#include <QPictureButton.h>

class ElementoBtnWidget : public QPictureButton
{
  Q_OBJECT
public:
  explicit ElementoBtnWidget(int id,QString nome,QWidget *parent=0);
  int getId() {return idElemento;}
  QString getNomeElemento() {return nomeElemento;}

signals:
  void cancellaReparto(ElementoBtnWidget*);
  void cambiaColore(QString colore);
  
public slots:
  
private:
  int idElemento;
  QString nomeElemento;
  QString coloreSfondo;

private slots:
  void on_dettagliAction_triggered();
  void on_cancellaAction_triggered();

};

#endif // ELEMENTOBTNWIDGET_H
