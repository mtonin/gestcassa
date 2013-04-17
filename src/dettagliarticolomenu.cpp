#include "dettagliarticolomenu.h"
#include "destinazionestampadlg.h"
#include <QDataWidgetMapper>

DettagliArticoloMenu::DettagliArticoloMenu(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  modello=new QStandardItemModel;

  articoliList->setModel(modello);
  articoliList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  connect(articoliList,SIGNAL(),)

}

void DettagliArticoloMenu::on_toolButton_2_clicked()
{
  articoliList->model()->removeRow(articoliList->currentIndex().row());
}

void DettagliArticoloMenu::on_toolButton_3_clicked()
{
  int numRighe=articoliList->model()->rowCount();
  modello->insertRow(numRighe);
  modello->setItem(numRighe,0,new QStandardItem("NUOVO ARTICOLO"));
  modello->setItem(numRighe,1,new QStandardItem("BAR"));

  articoliList->setCurrentIndex(modello->index(numRighe,0));
}
