#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "repartobutton.h"

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
  void gestioneModalita(const int);
  void repartoSlot(RepartoButton*);

private:
  Ui::MainWindow *ui;
  void keyPressEvent(QKeyEvent *);
};

#endif // MAINWINDOW_H
