#include "storicoordini.h"
#include "storicoordinimodel.h"

#include <QtSql>
#include <QDataWidgetMapper>

StoricoOrdini::StoricoOrdini(const int idSessione, QWidget *parent) : QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::Tool);
  activateWindow();

  fromData->setDate(QDate::currentDate());
  toData->setDate(QDate::currentDate());

  ordiniModel=new storicoOrdiniModel(this);
  ordiniModel->setTable("storicoordinitot");
  condizione=QString("idsessione=%1").arg(idSessione);
  ordiniModel->setFilter(condizione);
  ordiniModel->select();
  ordiniTable->setModel(ordiniModel);
  ordiniTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  ordiniTable->verticalHeader()->setVisible(false);
  //ordiniTable->selectRow(0);

  articoliOrdineModel=new QSqlQueryModel(this);
  articoliOrdineTbl->setModel(articoliOrdineModel);

  QDataWidgetMapper* mapper=new QDataWidgetMapper(this);
  mapper->setModel(ordiniModel);
  mapper->addMapping(sessioneOrdineTxt,0);
  mapper->addMapping(numeroOrdineTxt,1);
  mapper->addMapping(importoOrdineTxt,3);

  connect(ordiniModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),ordiniTable,SLOT(setCurrentIndex(QModelIndex)));
  connect(ordiniTable->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),mapper,SLOT(setCurrentModelIndex(QModelIndex)));
  connect(ordiniTable->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(caricaArticoliOrdine()));

}

void StoricoOrdini::caricaArticoliOrdine()
{
  QString sql=QString("select quantita,descrizione \
                                from storicoordinidett \
                                where idsessione=%1 and numeroordine=%2 and tipoArticolo <> 'C'").arg(sessioneOrdineTxt->text()).arg(numeroOrdineTxt->text());
  articoliOrdineModel->setQuery(sql);
  articoliOrdineModel->setHeaderData(0,Qt::Horizontal,"QUANTITA'",Qt::DisplayRole);
  articoliOrdineModel->setHeaderData(1,Qt::Horizontal,"DESCRIZIONE",Qt::DisplayRole);
  articoliOrdineTbl->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);

}

void StoricoOrdini::on_filtraBtn_5_clicked()
{
  if(ultimaSessioneBox->isChecked()) {
    ordiniModel->setFilter(condizione);
  } else {
    QString condDataOra=QString("datetime(tsstampa) between datetime(\"%1 %2\") and datetime(\"%3 %4\")")
                        .arg(fromData->date().toString("yyyy-MM-dd")).arg(fromOra->time().toString("hh:mm:ss"))
                        .arg(toData->date().toString("yyyy-MM-dd")).arg(toOra->time().toString("hh:mm:ss"));
    ordiniModel->setFilter(condDataOra);
  }
  ordiniTable->scrollToTop();

}

void StoricoOrdini::on_filtroDateBox_toggled(bool checked)
{
  fromData->setEnabled(checked);
  fromOra->setEnabled(checked);
  toData->setEnabled(checked);
  toOra->setEnabled(checked);
}
