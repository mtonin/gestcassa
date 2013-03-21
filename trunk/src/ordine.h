#ifndef ORDINE_H
#define ORDINE_H

#include "ui_ordine.h"
#include "ordinemodel.h"
#include "controlliordine.h"
#include <QStandardItemModel>

class Ordine : public QWidget, private Ui::Ordine
{
  Q_OBJECT
  
public:
  explicit Ordine(QWidget *parent = 0);

public slots:
  void nuovoArticolo(const int idArticolo, const QString descrizione, const float prezzo);

private slots:
  void on_pushButton_clicked();
  void on_articoliTab_clicked(const QModelIndex &index);
  void ricalcolaTotale(QModelIndex,QModelIndex);

private:
  OrdineModel modello;
  ControlliOrdine* controlli;

};

#endif // ORDINE_H
