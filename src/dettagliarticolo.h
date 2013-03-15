#ifndef DETTAGLIARTICOLO_H
#define DETTAGLIARTICOLO_H

#include "ui_dettagliarticolo.h"
#include "articolobtnwidget.h"

class DettagliArticolo : public QWidget, private Ui::DettagliArticolo
   {
   Q_OBJECT
   
public:
   explicit DettagliArticolo(QWidget *parent = 0);
   void setCurrentArticolo(const ArticoloBtnWidget*);

private slots:
   void on_testoArticolo_textEdited(const QString &arg1);

   void on_prezzoArticolo_textEdited(const QString &arg1);

   void on_disattivaFlag_toggled(bool checked);

private:
   void aggiornaArticolo();
   ArticoloBtnWidget* articoloBtn;

   };

#endif // DETTAGLIARTICOLO_H
