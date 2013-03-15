#ifndef ORDINE_H
#define ORDINE_H

#include "ui_ordine.h"
#include "ordinemodel.h"
#include <QStandardItemModel>

class Ordine : public QWidget, private Ui::Ordine
{
  Q_OBJECT
  
public:
  explicit Ordine(QWidget *parent = 0);

public slots:
  void nuovoArticolo(const QString descrizione, const float prezzo);

private slots:
  void on_pushButton_clicked();

private:
  OrdineModel modello;

};

#endif // ORDINE_H
