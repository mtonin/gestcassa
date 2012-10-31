#ifndef COMANDIBOX_H
#define COMANDIBOX_H

#include "ui_comandibox.h"

class ComandiBox : public QWidget, private Ui::ComandiBox
{
  Q_OBJECT

signals:
  void dbCaricato();
  void cambiaModalita(const int);

public:
  explicit ComandiBox(QWidget *parent = 0);

private slots:
  void on_toolButton_2_clicked();
  void on_toolButton_clicked();

private:
  void creaComandi();
  bool createConnection(const QString& nomeFile);
};

#endif // COMANDIBOX_H
