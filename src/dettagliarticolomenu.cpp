#include "dettagliarticolomenu.h"
#include "destinazionestampadlg.h"
#include <QDataWidgetMapper>

DettagliArticoloMenu::DettagliArticoloMenu(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  modello=new QStandardItemModel;

  articoliList->setModel(modello);

  QDataWidgetMapper* mapper=new QDataWidgetMapper;
  mapper->setModel(modello);
  mapper->addMapping(articoloTxt,0);
  mapper->addMapping(destinazioneTxt,1);

  connect(articoliList->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),mapper,SLOT(setCurrentModelIndex(QModelIndex)));
}

void DettagliArticoloMenu::on_toolButton_2_clicked()
{
  articoliList->model()->removeRow(articoliList->currentIndex().row());
}

void DettagliArticoloMenu::on_toolButton_3_clicked()
{
  int numRighe=articoliList->model()->rowCount();
  modello->insertRow(numRighe);
  modello->setItem(numRighe,0,new QStandardItem);
  modello->setItem(numRighe,1,new QStandardItem);

  articoliList->setCurrentIndex(modello->index(numRighe,0));
}

void DettagliArticoloMenu::on_toolButton_clicked()
{
  DestinazioneStampaDlg dlg;
  dlg.setWindowFlags(Qt::Tool);
  //dlg->move(QCursor::pos());
  if(QDialog::Accepted==dlg.exec()) {
    destinazioneTxt->setText(dlg.getDestinazione());
  }

}
