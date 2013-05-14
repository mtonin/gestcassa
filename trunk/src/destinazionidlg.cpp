#include "destinazionidlg.h"

#include <QMessageBox>
#include <QSqlRecord>

DestinazioniDlg::DestinazioniDlg(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  modello=new QSqlTableModel;
  modello->setTable("destinazioniStampa");
  modello->setEditStrategy(QSqlTableModel::OnFieldChange);
  modello->select();
  destinazioniList->setModel(modello);
  //destinazioniList->hideColumn(1);

  //destinazioniList->selectionModel()->setCurrentIndex(modello->index(0,0),QItemSelectionModel::SelectCurrent);

  /*
  QSqlQuery stmt("select nome,intestazione from destinazioniStampa");
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  while(stmt.next()) {
    QString nome=stmt.value(0).toString();
    QString intestazione=stmt.value(1).toString();
    QList<QStandardItem*> lista;
    lista.append(new QStandardItem(nome));
    lista.append(new QStandardItem(intestazione));
    modello->appendRow(lista);
  }
  */

}

void DestinazioniDlg::on_cancellaBtn_clicked()
{
  int currRiga=destinazioniList->currentIndex().row();
    if(!modello->removeRow(currRiga)) {
      QMessageBox::critical(this,"Errore","Impossibile cancellare la destinazione. Controllare gli articoli in cui è impostata.");
      return;
    }
    destinazioniList->selectionModel()->select(modello->index(currRiga-1,0),QItemSelectionModel::SelectCurrent);
}

void DestinazioniDlg::on_nuovoBtn_clicked()
{
  /*
  QSqlRecord riga;
  riga.setValue("nome","DESTINAZIONE");
  riga.setValue("intestazione","intestazione");
  modello->insertRecord(-1,riga);
  */

  int numRighe=modello->rowCount();
  modello->insertRow(numRighe);
  modello->setData(modello->index(numRighe,0),"NOME DESTINAZIONE");
  modello->setData(modello->index(numRighe,1),"INTESTAZIONE SCONTRINO");
  modello->submitAll();
  destinazioniList->setCurrentIndex(modello->index(numRighe,0));
  destinazioniList->edit(modello->index(numRighe,0));
}
