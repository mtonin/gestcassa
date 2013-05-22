#ifndef ORDINE_H
#define ORDINE_H

#include "ui_ordine.h"
#include "ordinemodel.h"
#include "controlliordine.h"
#include <QStandardItemModel>
#include <QMap>

class Ordine : public QWidget, private Ui::Ordine
{
  Q_OBJECT
  
public:
  explicit Ordine(QMap<QString,QVariant>* par,QWidget *parent = 0);

public slots:
  void nuovoArticolo(const int idArticolo, const QString descrizione, const float prezzo);
  void hide();
  bool isInComposizione() {return modello.rowCount()>0?true:false;}

private slots:
  void on_articoliTab_clicked(const QModelIndex &index);
  void ricalcolaTotale(QModelIndex,QModelIndex);

  void on_annullaBtn_clicked();

  void on_stampaBtn_clicked();

  void on_ultimoRestoBtn_clicked();

private:
  float importoUltimoOrdine;
  QMap<QString,QVariant>* configurazione;

  OrdineModel modello;
  ControlliOrdine* controlli;
  void nuovoOrdine();
  void stampaScontrino(int numeroOrdine);
};

#endif // ORDINE_H
