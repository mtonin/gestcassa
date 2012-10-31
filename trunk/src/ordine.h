#ifndef ORDINE_H
#define ORDINE_H

#include "ui_ordine.h"

class Ordine : public QWidget, private Ui::Ordine
{
  Q_OBJECT
  
public:
  explicit Ordine(QWidget *parent = 0);
private slots:
  void on_pushButton_clicked();
};

#endif // ORDINE_H
