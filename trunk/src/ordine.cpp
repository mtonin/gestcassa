#include "ordine.h"
#include <QMessageBox>

Ordine::Ordine(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  articoliTab->setModel(&modello);
  articoliTab->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
  articoliTab->hideColumn(0);
  articoliTab->verticalHeader()->setDefaultSectionSize(40);
}

void Ordine::nuovoArticolo(const int idArticolo, const QString descrizione, const float prezzo)
{
  /*
  QModelIndex idx;
  int riga=0;
  for(riga=0;riga<modello.rowCount();riga++) {
      idx=modello.index(riga,1);
      if(idx.data()==descrizione) {
          break;
        }
    }
  if(riga<modello.rowCount()) {
      idx=modello.index(riga,2);
      int valore=idx.data().toInt();
      modello.setData(idx,++valore);
    } else {
      QStandardItem* item=new QStandardItem();
      modello.appendRow(item);
      idx=modello.index(riga,1);
      modello.setData(idx,descrizione,Qt::DisplayRole);
      QVariant flag=Qt::AlignRight|Qt::AlignVCenter;
      idx=modello.index(riga,2);
      modello.setData(idx,1,Qt::DisplayRole);
      modello.setData(idx,flag,Qt::TextAlignmentRole);
      idx=modello.index(riga,3);
      modello.setData(idx,prezzo,Qt::DisplayRole);
      modello.setData(idx,flag,Qt::TextAlignmentRole);
    }

  */
  modello.aggiunge(idArticolo,descrizione,prezzo);
  articoliTab->scrollToBottom();
  float totale=totaleLine->text().toFloat();
  totale+=prezzo;
  totaleLine->setText(QString("%1").arg(totale,4,'f',2));
}

void Ordine::on_pushButton_clicked()
{
  QMessageBox::information(this,"MSG","click()");
}
