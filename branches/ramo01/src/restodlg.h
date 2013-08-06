#ifndef RESTODLG_H
#define RESTODLG_H

#include <QDialog>
#include <QPropertyAnimation>
#include <QKeyEvent>

#include "ui_restodlg.h"

class RestoDlg : public QDialog, private Ui::RestoDlg
{
  Q_OBJECT
  
public:
  explicit RestoDlg(float valore, int durata, QWidget *parent = 0);
  ~RestoDlg();
  
private slots:
  void on_tasto0Btn_clicked();
  void on_tasto1Btn_clicked();
  void on_tasto2Btn_clicked();
  void on_tasto3Btn_clicked();
  void on_tasto4Btn_clicked();
  void on_tasto5Btn_clicked();
  void on_tasto6Btn_clicked();
  void on_tasto7Btn_clicked();
  void on_tasto8Btn_clicked();
  void on_tasto9Btn_clicked();
  void on_tastoVirgolaBtn_clicked();
  void ricalcolaResto();
  void cancellaUltimoCarattere();

private:
  QPropertyAnimation* effetto;
  int _durata;

  float importoOrdine;

  void keyPressEvent(QKeyEvent *);
};

#endif // RESTODLG_H
