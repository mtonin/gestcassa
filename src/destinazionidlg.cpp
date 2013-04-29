#include "destinazionidlg.h"
#include "destinazionestampa.h"

#include <QMessageBox>

DestinazioniDlg::DestinazioniDlg(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  modello=new QSqlTableModel;
  modello->setTable("destinazioniStampa");
  modello->setEditStrategy(QSqlTableModel::OnFieldChange);
  modello->select();
  destinazioniList->setModel(modello);
  destinazioniList->hideColumn(1);

  mapper=new QDataWidgetMapper;
  mapper->setModel(modello);
  mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
  mapper->addMapping(intestazione1Txt,1);

  connect(destinazioniList->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),mapper,SLOT(setCurrentModelIndex(QModelIndex)));

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

void DestinazioniDlg::on_nuovoBtn_clicked()
{
    DestinazioneStampa* dlg=new DestinazioneStampa;
    if(QDialog::Accepted==dlg->exec()) {
      /*
      QList<QStandardItem*> lista;
      lista.append(new QStandardItem(dlg->getNome()));
      lista.append(new QStandardItem(dlg->getIntestazione()));
      modello->appendRow(lista);
      */
    }
}
