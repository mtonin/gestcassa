#ifndef AREALAVORO_H
#define AREALAVORO_H

#include "ui_arealavoro.h"
#include "repartobtnwidget.h"
#include <QMap>

class AreaLavoro : public QWidget, private Ui::AreaLavoro
{
  Q_OBJECT
  
public:
  explicit AreaLavoro(QWidget *parent = 0);

signals:

private slots:
  void creaNewReparto();
  void cancellaReparto(RepartoBtnWidget*);
  void attivaReparto();
  void cambiaColore(QString colore);

public slots:
  void caricaDaDatabase();

private:
  void creaMenu();
  void creaRepartoBtn(int id,QString nome,QString colore);
  QWidget* creaNuovaPagina(QString nome);
  QMap<int,QWidget*> pagineArticoli;
};

#endif // AREALAVORO_H
