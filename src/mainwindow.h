#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "commons.h"
#include "dettaglireparto.h"
#include "dettagliarticolo.h"
#include "dettagliorepartodlg.h"
#include "ordine.h"
#include <QMainWindow>

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

private:
  Ui::MainWindow *ui;
  Ordine* ordineBox;
  DettaglioRepartoDlg* dettaglioBox;
  dettagliReparto* dettagliRepartoBox;
  DettagliArticolo* dettagliArticoloBox;
  modalitaType currentMode;

  void keyPressEvent(QKeyEvent *);
  void creaRepartiButtons();
  void gestioneModalita(const modalitaType);
  void creaArticoliPerRepartoButtons(RepartoBtnWidget*);

};

#endif // MAINWINDOW_H
