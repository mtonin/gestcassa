#include "ordine.h"
#include "controlliordine.h"
#include <QMessageBox>
#include <QTimer>

Ordine::Ordine(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  articoliTab->setModel(&modello);
  articoliTab->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
  articoliTab->hideColumn(0);
  articoliTab->verticalHeader()->setDefaultSectionSize(30);
  connect(&modello,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(ricalcolaTotale(QModelIndex,QModelIndex)));
  controlli=new ControlliOrdine(this);

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
  controlli->hide();
  modello.incrementa(idArticolo,descrizione,prezzo);
  articoliTab->scrollToBottom();

  /*
  float totale=totaleLine->text().toFloat();
  totale+=prezzo;
  totaleLine->setText(QString("%1").arg(totale,4,'f',2));
  */
}

void Ordine::hide()
{
  controlli->hide();
  QWidget::hide();
}

void Ordine::on_articoliTab_clicked(const QModelIndex &index)
{
  int id=index.model()->index(index.row(),0).data().toInt();
  controlli->setIdArticolo(id);
  connect(&modello,SIGNAL(rigaCancellata()),controlli,SLOT(close()));
  disconnect(controlli,0,0,0);
  connect(controlli,SIGNAL(incrementa(int)),&modello,SLOT(incrementa(int)));
  connect(controlli,SIGNAL(decrementa(int)),&modello,SLOT(decrementa(int)));

  QPoint pos=QCursor::pos();
  controlli->move(pos);
  controlli->show();
}

void Ordine::ricalcolaTotale(QModelIndex, QModelIndex)
{
  float totale=0;
  for(int i=0;i<modello.rowCount(QModelIndex());i++) {
    totale+=modello.index(i,3).data().toFloat();
  }
  totaleLine->setText(QString("%1").arg(totale,4,'f',2));
}

void Ordine::on_annullaBtn_clicked()
{
  if(QMessageBox::Ok==QMessageBox::question(0,"Annulla ordine","Confermi l'annullamento dell'ordine?",QMessageBox::Ok|QMessageBox::No)) {
    modello.clear();
  }
}

void Ordine::on_stampaBtn_clicked()
{
  float totale=totaleLine->text().toFloat();
  restoDlg=new RestoDlg(totale,this);
  restoDlg->exec();
}
