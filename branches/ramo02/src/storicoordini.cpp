#include "storicoordini.h"
#include "storicoordinimodel.h"

#include <QtSql>
#include <QDataWidgetMapper>

StoricoOrdini::StoricoOrdini(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::Tool);
  activateWindow();

  ordiniModel=new storicoOrdiniModel(this);
  ordiniModel->setQuery("select idsessione,numeroordine,tsstampa,importo,flagstorno \
                               from storicoordinitot");
  ordiniTable->setModel(ordiniModel);
  ordiniTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  ordiniTable->verticalHeader()->setVisible(false);

  articoliOrdineModel=new QSqlQueryModel(this);
  articoliOrdineTbl->setModel(articoliOrdineModel);

  QDataWidgetMapper* mapper=new QDataWidgetMapper(this);
  mapper->setModel(ordiniModel);
  mapper->addMapping(sessioneOrdineTxt,0);
  mapper->addMapping(numeroOrdineTxt,1);
  mapper->addMapping(importoOrdineTxt,3);

  connect(ordiniTable->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),mapper,SLOT(setCurrentModelIndex(QModelIndex)));
  connect(ordiniTable->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(caricaArticoliOrdine()));

}

void StoricoOrdini::caricaArticoliOrdine()
{
  QString sql=QString("select quantita,descrizione \
                                from storicoordinidett \
                                where idsessione=%1 and numeroordine=%2 and tipoArticolo <> 'C'").arg(sessioneOrdineTxt->text()).arg(numeroOrdineTxt->text());
  articoliOrdineModel->setQuery(sql);
  articoliOrdineTbl->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);

}
