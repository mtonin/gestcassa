#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "commons.h"
#include "dettaglireparto.h"
#include "dettagliarticolo.h"
#include "ordine.h"
#include <QMainWindow>
#include <QList>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  
private slots:
   void repartoSelezionato();
   void articoloSelezionato();
   void modalitaBtnClicked();

   void on_modalitaBtn_2_clicked();

private:
  Ui::MainWindow *ui;
  Ordine* ordineBox;
  dettagliReparto* dettagliRepartoBox;
  DettagliArticolo* dettagliArticoloBox;
  QList<ArticoloBtnWidget*> articoliBtnList;

  modalitaType modalitaCorrente;

  void keyPressEvent(QKeyEvent *);
  void closeEvent(QCloseEvent *);

  void creaRepartiButtons();
  void gestioneModalita(const modalitaType nuovaModalita);
  void creaArticoliPerRepartoButtons(RepartoBtnWidget*);

};

#endif // MAINWINDOW_H
