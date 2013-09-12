#ifndef CONTROLLIORDINE_H
#define CONTROLLIORDINE_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QModelIndex>
namespace Ui {
  class ControlliOrdine;
}

class ControlliOrdine : public QWidget
{
  Q_OBJECT
  
signals:
  void incrementa(const int);
  void decrementa(const int,const QModelIndex&);

public:
  explicit ControlliOrdine(QWidget *parent = 0);
  ~ControlliOrdine();
  void show();
  void setIdArticolo(const int id) {idArticolo=id;}
  void setModelIndex(QModelIndex idx) {index=idx;}

public slots:

private slots:
  void on_toolButton_clicked();
  void on_toolButton_2_clicked();

private:
  int idArticolo;
  QModelIndex index;
  Ui::ControlliOrdine *ui;
  QPropertyAnimation* effetto;
};

#endif // CONTROLLIORDINE_H
