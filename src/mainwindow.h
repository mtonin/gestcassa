#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
  void gestioneModalita(const int);

private:
  Ui::MainWindow *ui;
  void keyPressEvent(QKeyEvent *);
  Ordine* ordineBox;

};

#endif // MAINWINDOW_H
