#include "buonidlg.h"

BuoniDlg::BuoniDlg(QWidget *parent) :QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::Tool);
  activateWindow();

  modello = new buoniModel(this);
  modello->setTable("buoni");
  modello->setSort(1, Qt::AscendingOrder);
  modello->select();
  buoniTable->setModel(modello);
  buoniTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  buoniTable->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
  buoniTable->verticalHeader()->setVisible(false);
  buoniTable->horizontalHeader()->setVisible(true);

}
