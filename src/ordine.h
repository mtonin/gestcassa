#ifndef ORDINE_H
#define ORDINE_H

#include "ui_ordine.h"
#include "ordinemodel.h"
#include "controlliordine.h"
#include <QStandardItemModel>
#include <QMap>
#include <QTimer>

class Ordine : public QWidget, private Ui::Ordine
{
  Q_OBJECT
  
public:
  explicit Ordine(QMap<QString,QVariant>* par,QWidget *parent = 0);
  void enterTest();
  void exitTest();

public slots:
  void nuovoArticolo(const int idArticolo, const QString descrizione, const float prezzo);
  void hide();
  bool isInComposizione() {return modello.rowCount()>0?true:false;}
  void nuovoOrdine(const int);
  void clearSelezione();

private slots:
  void seleziona(const QModelIndex &indexNew);
  void ricalcolaTotale(QModelIndex,QModelIndex);
  void lampeggia();

  void on_annullaBtn_clicked();

  void on_ristampaBtn_clicked();

  void on_stampaBtn_clicked();

  void on_ultimoRestoBtn_clicked();

  void on_pagPrevBtn_clicked();

  void on_pagNextBtn_clicked();

  void on_duplicaBtn_clicked();

private:
  float importoUltimoOrdine;
  QMap<QString,QVariant>* configurazione;
  int idSessioneCorrente;
  int numOrdineCorrente;
  float importoOrdineCorrente;
  QTimer* blinkTimer;
  OrdineModel modello;
  ControlliOrdine* controlli;
  QString colore;
  void stampaScontrino(const int);
};

#endif // ORDINE_H
