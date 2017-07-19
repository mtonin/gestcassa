#include "storicoordini.h"
#include "storicoordinimodel.h"
#include "storicoarticoliordinimodel.h"
#include <QtSql>
#include <QDataWidgetMapper>
#include <QMessageBox>

StoricoOrdini::StoricoOrdini(const int idSessione, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::Tool);
    activateWindow();

    caricaSessioni();
    fromData->setDate(QDate::currentDate());
    toData->setDate(QDate::currentDate());

    ordiniModel = new storicoOrdiniModel(this);
    ordiniModel->setTable("storicoordinitot");
    condizione = QString("idsessione=%1").arg(idSessione);
    ordiniModel->setFilter(condizione);
    ordiniModel->select();
    while(ordiniModel->canFetchMore())
      ordiniModel->fetchMore();
    ordiniTable->setModel(ordiniModel);
    ordiniTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ordiniTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ordiniTable->verticalHeader()->setVisible(false);
    ordiniTable->hideColumn(1);
    ordiniTable->hideColumn(6);

    articoliOrdineModel = new storicoArticoliOrdiniModel(this);
    articoliOrdineTbl->setModel(articoliOrdineModel);

    QDataWidgetMapper* mapper = new QDataWidgetMapper(this);
    mapper->setModel(ordiniModel);
    mapper->addMapping(sessioneOrdineTxt, 0);
    mapper->addMapping(idCassaOrdineTxt, 1);
    mapper->addMapping(cassaOrdineTxt, 2);
    mapper->addMapping(numeroOrdineTxt, 3);
    mapper->addMapping(importoOrdineTxt, 5);
    mapper->addMapping(stornoBox,6,"checked");

    connect(ordiniModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), ordiniTable, SLOT(setCurrentIndex(QModelIndex)));
    connect(ordiniTable->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), mapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ordiniTable->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(caricaArticoliOrdine()));
    connect(stornoBox,SIGNAL(clicked(bool)),articoliOrdineTbl,SLOT(setFocus()));

    idCassaOrdineTxt->setVisible(false);

    //ordiniTable->selectRow(0);
    ordiniTable->sortByColumn(4,Qt::SortOrder::AscendingOrder);
}

void StoricoOrdini::caricaArticoliOrdine()
{
    QString sql = QString("select quantita,descrizione \
                                from storicoordinidett \
                                where idsessione=%1 and idcassa='%2' and numeroordine=%3 and tipoArticolo <> 'C'")
                  .arg(sessioneOrdineTxt->text()).arg(idCassaOrdineTxt->text()).arg(numeroOrdineTxt->text());
    articoliOrdineModel->setQuery(sql);
    /*
    articoliOrdineModel->setHeaderData(0,Qt::Horizontal,"Q.TA'",Qt::DisplayRole);
    articoliOrdineModel->setHeaderData(1,Qt::Horizontal,"DESCRIZIONE",Qt::DisplayRole);
    */
    articoliOrdineTbl->setWordWrap(true);
    articoliOrdineTbl->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    articoliOrdineTbl->horizontalHeader()->setDefaultSectionSize(70);
    articoliOrdineTbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    articoliOrdineTbl->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

}

void StoricoOrdini::on_filtraBtn_5_clicked()
{
    ordiniModel->submitAll();

    if (sessioneBox->isChecked()) {
        int sessioneSelezionata=sessioneCombo->model()->index(sessioneCombo->currentIndex(),1).data().toInt();
        condizione = QString("idsessione=%1").arg(sessioneSelezionata);
        ordiniModel->setFilter(condizione);
    } else {
        QString condDataOra = QString("tsstampa between '%1 %2' and '%3 %4'")
                              .arg(fromData->date().toString("yyyy-MM-dd")).arg(fromOra->time().toString("hh:mm:ss"))
                              .arg(toData->date().toString("yyyy-MM-dd")).arg(toOra->time().toString("hh:mm:ss"));
        ordiniModel->setFilter(condDataOra);
    }
    while(ordiniModel->canFetchMore())
      ordiniModel->fetchMore();

    ordiniTable->scrollToTop();
    sessioneOrdineTxt->clear();
    numeroOrdineTxt->clear();
    importoOrdineTxt->clear();
    cassaOrdineTxt->clear();

    //ordiniTable->selectRow(0);
}

void StoricoOrdini::on_filtroDateBox_toggled(bool checked)
{
    fromData->setEnabled(checked);
    fromOra->setEnabled(checked);
    toData->setEnabled(checked);
    toOra->setEnabled(checked);
}
void StoricoOrdini::caricaSessioni()
{

  QSqlQueryModel* sessioniModel=new QSqlQueryModel(this);
  sessioniModel->setQuery("SELECT idsessione || ' (Inizio: ' ||     \
  substring(100+extract(day from tsinizio) from 2 for 2)||'/'||       \
  substring(100+extract(month from tsinizio) from 2 for 2)||'/'||   \
  extract(year from tsinizio) || ' ' ||                                            \
  substring(100+extract(hour from tsinizio) from 2 for 2)||':'||      \
  substring(100+extract(minute from tsinizio) from 2 for 2)||':'||   \
  substring(100+extract(second from tsinizio) from 2 for 2) || ')',  \
  idsessione \
  FROM sessione order by idsessione desc"  );

  QTableView* sessioneView=new QTableView(this);
  sessioneCombo->setView(sessioneView);
  sessioneView->horizontalHeader()->hide();
  sessioneView->verticalHeader()->hide();
  sessioneView->setSelectionBehavior(QAbstractItemView::SelectRows);
  sessioneView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  sessioneView->setShowGrid(false);

  sessioneCombo->setModel(sessioniModel);
  sessioneView->hideColumn(1);

}

void StoricoOrdini::on_sessioneBox_toggled(bool checked){
  sessioneCombo->setEnabled(checked);
}

void StoricoOrdini::on_chiudeBtn_clicked(){
    ordiniModel->submitAll();
    close();
}
