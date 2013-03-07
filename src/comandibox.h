#ifndef COMANDIBOX_H
#define COMANDIBOX_H

#include "commons.h"
#include "ui_comandibox.h"

class ComandiBox : public QWidget, private Ui::ComandiBox
{
  Q_OBJECT

signals:
  void dbCaricato();
  void cambiaModalita(const modalitaType);

public:
  explicit ComandiBox(QWidget *parent = 0);

public slots:
  QString on_dbLoadBtn_clicked();

private slots:
  void on_modalitaBtn_clicked();

private:
  void creaComandi();
  bool createConnection(const QString& nomeFile);
};

#endif // COMANDIBOX_H
