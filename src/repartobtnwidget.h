#ifndef REPARTOBTNWIDGET_H
#define REPARTOBTNWIDGET_H

#include <QPictureButton.h>

class RepartoBtnWidget : public QPictureButton
{
  Q_OBJECT
public:
  explicit RepartoBtnWidget(int id,QString nome,QWidget *parent=0);
  int getId() {return idReparto;}
  QString getNomeReparto() {return nomeReparto;}

signals:
  void cancellaReparto(RepartoBtnWidget*);
  void cambiaColore(QString colore);
  
public slots:
  
private:
  int idReparto;
  QString nomeReparto;
  QString coloreSfondo;

private slots:
  void on_dettagliAction_triggered();
  void on_cancellaAction_triggered();

};

#endif // REPARTOBTNWIDGET_H
