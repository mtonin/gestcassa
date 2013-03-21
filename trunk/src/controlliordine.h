#ifndef CONTROLLIORDINE_H
#define CONTROLLIORDINE_H

#include <QWidget>
#include <QTimer>

namespace Ui {
  class ControlliOrdine;
}

class ControlliOrdine : public QWidget
{
  Q_OBJECT
  
signals:
  void incrementa(const int);
  void decrementa(const int);

public:
  explicit ControlliOrdine(QWidget *parent = 0);
  ~ControlliOrdine();
  void show();
  void setIdArticolo(const int id) {idArticolo=id;}

public slots:

private slots:
  void on_toolButton_clicked();
  void on_toolButton_2_clicked();
  void onTimerTimeout();

private:
  int idArticolo;
  Ui::ControlliOrdine *ui;
  QTimer timer;
};

#endif // CONTROLLIORDINE_H