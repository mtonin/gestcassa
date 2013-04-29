#include "dettagliarticolomenu.h"
#include <QDataWidgetMapper>
#include <QtSql>
#include <QMessageBox>
#include <QMenu>

DettagliArticoloMenu::DettagliArticoloMenu(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  modello=new QStandardItemModel;

  articoliList->setModel(modello);
  articoliList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  articoliList->setContextMenuPolicy(Qt::CustomContextMenu);
}

void DettagliArticoloMenu::cancella()
{
  articoliList->model()->removeRow(articoliList->currentIndex().row());
}

void DettagliArticoloMenu::on_toolButton_3_clicked()
{
  int numRighe=articoliList->model()->rowCount();
  modello->insertRow(numRighe);
  modello->setItem(numRighe,0,new QStandardItem("NUOVO ARTICOLO"));
  modello->setItem(numRighe,1,new QStandardItem("BAR"));

  QModelIndex idx=modello->index(numRighe,1);
  articoliList->setIndexWidget(idx,creaDestinazioneBox());
  articoliList->setCurrentIndex(modello->index(numRighe,0));
  //articoliList->selectionModel()->setCurrentIndex(modello->index(numRighe,0),QItemSelectionModel::Current);

  articoliList->setFocus();
}

QComboBox *DettagliArticoloMenu::creaDestinazioneBox()
{
  QComboBox* box=new QComboBox;
  QSqlQuery stmt("select nome from destinazioniStampa");
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return NULL;
  }
  while(stmt.next()) {
    QString nome=stmt.value(0).toString();
    box->addItem(nome);
  }

  return box;
}

void DettagliArticoloMenu::on_articoliList_customContextMenuRequested(const QPoint &pos)
{
  if(!articoliList->selectionModel()->currentIndex().isValid()) {
    return;
  }
  QMenu* ctxMenu=new QMenu;
  QAction* cancellaAction=new QAction("Cancella riga",this);
  connect(cancellaAction,SIGNAL(triggered()),this,SLOT(cancella()));
  ctxMenu->addAction(cancellaAction);
  ctxMenu->move(articoliList->mapToGlobal(pos));
  ctxMenu->exec();
}
