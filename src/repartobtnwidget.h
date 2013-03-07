#ifndef REPARTOBTNWIDGET_H
#define REPARTOBTNWIDGET_H

#include <QPictureButton.h>

class RepartoBtnWidget : public QPictureButton
{
  Q_OBJECT
public:
  explicit RepartoBtnWidget(int id,QString nome,QWidget *parent=0);
  explicit RepartoBtnWidget(QWidget *parent=0);
  int getId() {return idReparto;}
  QString getNomeReparto() {return nomeReparto;}
  const QColor& getColore() {return coloreSfondo;}

signals:
  void cancellaReparto(RepartoBtnWidget*);
  void cambiaColore(const QColor& colore);
  
public slots:
  void setColore(const QColor& colore);

private:
  int idReparto;
  QString nomeReparto;
  QColor coloreSfondo;

private slots:
  void on_dettagliAction_triggered();
  void on_cancellaAction_triggered();

};

#endif // REPARTOBTNWIDGET_H
