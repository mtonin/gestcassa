#ifndef REPARTOBTNWIDGET_H
#define REPARTOBTNWIDGET_H

#include <QPictureButton.h>

class RepartoBtnWidget : public QPictureButton
{
  Q_OBJECT
public:
  explicit RepartoBtnWidget(int id,QWidget *parent=0);
  explicit RepartoBtnWidget(QWidget *parent=0);

  const int getId() {return idReparto;}
  const QString getNomeReparto() {return nomeReparto;}
  const QColor& getColore() {return buttonColorNormal();}
  const QFont& getFont() {return font();}

signals:
  void cambiaColore(const QColor& colore);
  void cambiaFont(const QFont& font);
  
public slots:
  void setColore(const QColor& colore);
  void setButtonFont(const QFont& font);

private:
  int idReparto;
  QString nomeReparto;

private slots:
  void on_dettagliAction_triggered();

};

#endif // REPARTOBTNWIDGET_H
