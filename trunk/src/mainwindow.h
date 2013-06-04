#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "commons.h"
#include "dettaglireparto.h"
#include "dettagliarticolo.h"
#include "ordine.h"
#include "infowidget.h"
#include <QMainWindow>
#include <QList>
#include <QMap>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

signals:
  void aggiungeArticolo(const int idArticolo, const QString& nomeArticolo, float prezzo);
  
public:
  explicit MainWindow(QMap<QString,QVariant>* configurazione,QWidget *parent = 0);
  ~MainWindow();

  
private slots:
   void repartoSelezionato();
   void articoloSelezionato();
   void on_configurazioneBtn_clicked();

   void on_closeBtn_clicked();

   void on_reportBtn_clicked();

   void on_statsBtn_clicked();

   void on_cassaBtn_clicked();

   void on_gestioneBtn_clicked();

private:
  Ui::MainWindow *ui;
  Ordine* ordineBox;
  DettagliReparto* dettagliRepartoBox;
  DettagliArticolo* dettagliArticoloBox;
  infoWidget* info;

  QList<ArticoloBtnWidget*> articoliBtnList;
  QList<QStackedWidget*> stackedList;
  modalitaType modalitaCorrente;
  QMap<QString,QVariant>* confMap;

  void keyPressEvent(QKeyEvent *);
  void closeEvent(QCloseEvent *);

  void creaRepartiButtons();
  void gestioneModalita(const modalitaType nuovaModalita);
  void creaArticoliPerRepartoButtons(RepartoBtnWidget*);
  void creaInfoMessaggi();

};

#endif // MAINWINDOW_H
