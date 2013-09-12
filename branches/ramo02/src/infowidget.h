#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include "ui_infowidget.h"
#include <QAnimationGroup>

class infoWidget : public QWidget, private Ui::infoWidget
{
  Q_OBJECT
  
private slots:
  void ricominciaEffetto();

public:
  explicit infoWidget(const QStringList lista,QWidget *parent = 0);
  void setListaTesto(const QStringList lista);

private:
  QStringList listaTesto;
  int currentIndex;
  QAnimationGroup* effetto;

};

#endif // INFOWIDGET_H
