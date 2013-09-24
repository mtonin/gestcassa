#ifndef STORICOORDINI_H
#define STORICOORDINI_H

#include "storicoordinimodel.h"
#include "ui_storicoordini.h"

#include <QSqlQueryModel>

class StoricoOrdini : public QDialog, private Ui::StoricoOrdini
{
  Q_OBJECT
  
public:
  explicit StoricoOrdini(QWidget *parent = 0);

private slots:
  void caricaArticoliOrdine();

private:
  storicoOrdiniModel* ordiniModel;
  QSqlQueryModel* articoliOrdineModel;
};

#endif // STORICOORDINI_H
