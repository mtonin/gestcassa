#ifndef DETTAGLIARTICOLO_H
#define DETTAGLIARTICOLO_H

#include "ui_dettagliarticolo.h"
#include "articolobtnwidget.h"

#include <QStandardItemModel>

class DettagliArticolo : public QWidget, private Ui::DettagliArticolo
   {
   Q_OBJECT

signals:
  void eliminaPulsanteCorrente(ArticoloBtnWidget*);

public:
   explicit DettagliArticolo(QWidget *parent = 0);
   void setCurrentArticolo(const ArticoloBtnWidget*);

private slots:
   void on_testoArticolo_textEdited(const QString &arg1);

   void on_prezzoArticolo_textEdited(const QString &arg1);

   void on_disattivaFlag_toggled(bool checked);

   void on_destinazioneBox_activated(const QString &arg1);

   void on_menuBox_clicked(bool checked);

   void on_nuovoBtn_clicked();

   void on_cancellaBtn_clicked();

   void on_articoliList_customContextMenuRequested(const QPoint &pos);

   void on_eliminaBtn_clicked();

  private:
   void aggiornaArticolo();
   ArticoloBtnWidget* articoloBtn;
   QStandardItemModel* modello;
   QStandardItemModel* articoliMenuModello;

   void reset();
   void creaSelezioneArticoloBox();

};

#endif // DETTAGLIARTICOLO_H
