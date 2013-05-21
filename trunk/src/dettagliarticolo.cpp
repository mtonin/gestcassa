#include "dettagliarticolo.h"
#include "dettagliarticolomenu.h"
#include <QtSql>
#include <QLocale>
#include <QInputDialog>
#include <QComboBox>

DettagliArticolo::DettagliArticolo(QWidget *parent) :
  QWidget(parent),articoloBtn(NULL)
{
  setupUi(this);
  QDoubleValidator* validator=new QDoubleValidator(0,99.99,2,prezzoArticolo);
  validator->setNotation(QDoubleValidator::StandardNotation);
  prezzoArticolo->setValidator(validator);

  modello=new QStandardItemModel;

  articoliList->setModel(modello);
  articoliList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  articoliList->setContextMenuPolicy(Qt::CustomContextMenu);
  //menu=new DettagliArticoloMenu;
  //layout()->addWidget(menu);

  articoliMenuModello=new QStandardItemModel;
  articoliBox->setModel(articoliMenuModello);

}

void DettagliArticolo::setCurrentArticolo(const ArticoloBtnWidget *currentArticoloBtn){

  reset();
  articoloBtn=(ArticoloBtnWidget*)currentArticoloBtn;
  testoArticolo->setText(articoloBtn->getNomeArticolo());
  /*
  QLocale locale;
  QString str=locale.toCurrencyString(articoloBtn->getPrezzo(),QString(" "));
  prezzoArticolo->setText(str);
  //prezzoArticolo->setText(QString::number(articoloBtn->getPrezzo(),'g',2));
  */

  prezzoArticolo->setText(QString("%1").arg(articoloBtn->getPrezzo(),4,'f',2));
  disattivaFlag->setChecked(!articoloBtn->getAbilitato());

  destinazioneBox->clear();
  QSqlQuery stmt("select nome from destinazioniStampa");
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  int i=0;
  while(stmt.next()) {
    QString nome=stmt.value(0).toString();
    destinazioneBox->addItem(nome);
    if(nome==articoloBtn->getRepartoStampa()) {
      destinazioneBox->setCurrentIndex(i);
    }
    i++;
  }

  if(articoloBtn->getRepartoStampa().isEmpty()) {
    articoloBtn->setRepartoStampa(destinazioneBox->currentText());
  }

  menuBox->setChecked(articoloBtn->isGestioneMenu());
  destinazioneBox->setEnabled(!articoloBtn->isGestioneMenu());
  stmt.prepare("select a.idarticolo,b.descrizione from articolimenu a, articoli b where a.idarticolo=b.idarticolo and a.idpulsante=?");
  stmt.addBindValue(articoloBtn->getId());
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  int numRighe=0;
  while(stmt.next()) {
    modello->insertRow(numRighe);
    QString nome=stmt.value(1).toString();
    QString idarticolo=stmt.value(0).toString();
    i++;
    QStandardItem* item=new QStandardItem(nome);
    modello->setItem(numRighe,0,item);
    item=new QStandardItem(idarticolo);
    modello->setItem(numRighe,1,item);
    numRighe++;
  }
  articoliList->hideColumn(1);

  testoArticolo->setFocus();
}

void DettagliArticolo::aggiornaArticolo()
{

  QSqlQuery query;
  if(0==articoloBtn->getId()) {
    query.prepare("insert into articoli (descrizione,prezzo,idreparto,riga,colonna,abilitato,destinazione,gestioneMenu) values(?,?,?,?,?,?,?,?)");
  } else {
    query.prepare("update articoli set descrizione=?,prezzo=?,idreparto=?,riga=?,colonna=?,abilitato=?,destinazione=?,gestioneMenu=? where idarticolo=?");
  }

  //query.addBindValue(articoloBtn->getId());
  query.addBindValue(articoloBtn->getNomeArticolo());
  query.addBindValue(articoloBtn->getPrezzo());
  query.addBindValue(articoloBtn->getIdReparto());
  query.addBindValue(articoloBtn->getRiga());
  query.addBindValue(articoloBtn->getColonna());
  query.addBindValue(!disattivaFlag->isChecked());
  query.addBindValue(articoloBtn->getRepartoStampa());
  query.addBindValue(menuBox->isChecked());
  if(articoloBtn->getId()>0) {
    query.addBindValue(articoloBtn->getId());
  }
  query.exec();
  if(!query.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          query.lastError().text());
    return;
  }
  if(0==articoloBtn->getId()) {
    query.clear();
    query.prepare("select idarticolo from articoli where idreparto=? and riga=? and colonna=?");
    query.addBindValue(articoloBtn->getIdReparto());
    query.addBindValue(articoloBtn->getRiga());
    query.addBindValue(articoloBtn->getColonna());
    query.exec();
    if(!query.isActive()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            query.lastError().text());
      return;
    }
    if(query.next())
      articoloBtn->setId(query.value(0).toInt());
  }

  if(articoloBtn->isGestioneMenu()) {
    query.prepare("delete from articolimenu where idpulsante=?");
    query.addBindValue(articoloBtn->getId());
    query.exec();
    if(!query.isActive()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            query.lastError().text());
      return;
    }
    for(int i=0;i<modello->rowCount();i++) {
      query.prepare("insert into articolimenu (idpulsante,idarticolo) values(?,?)");
      int idarticolo=modello->index(i,1).data().toInt();
      query.addBindValue(articoloBtn->getId());
      query.addBindValue(idarticolo);
      query.exec();
      if(!query.isActive()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              query.lastError().text());
        return;
      }
    }
  }
}

void DettagliArticolo::reset()
{
  modello->clear();
  creaDestinazioneBox(articoliBox);
}

void DettagliArticolo::on_testoArticolo_textEdited(const QString &testo)
{
  articoloBtn->setNomeArticolo(testo);
  aggiornaArticolo();

}

void DettagliArticolo::on_prezzoArticolo_textEdited(const QString &prezzo)
{
  /*
  QLocale locale;
  float val=locale.toFloat(prezzo);
  */
  float val=prezzo.toFloat();
  articoloBtn->setPrezzo(val);
  aggiornaArticolo();
}

void DettagliArticolo::on_disattivaFlag_toggled(bool checked)
{
  articoloBtn->setAbilitato(!checked);
  aggiornaArticolo();
}

void DettagliArticolo::on_destinazioneBox_activated(const QString &arg1)
{
  articoloBtn->setRepartoStampa(arg1);
  aggiornaArticolo();
}

void DettagliArticolo::on_menuBox_clicked(bool checked)
{
  articoloBtn->setGestioneMenu(checked);
  destinazioneBox->setEnabled(!checked);
  aggiornaArticolo();

}

void DettagliArticolo::on_nuovoBtn_clicked()
{
  int numRighe=articoliList->model()->rowCount();
  modello->insertRow(numRighe);
  QString nome=articoliMenuModello->index(articoliBox->currentIndex(),0).data().toString();
  QString idarticolo=articoliMenuModello->index(articoliBox->currentIndex(),1).data().toString();
  QStandardItem* item=new QStandardItem(nome);
  modello->setItem(numRighe,0,item);
  item=new QStandardItem(idarticolo);
  modello->setItem(numRighe,1,item);
  //modello->setItem(numRighe,1,new QStandardItem);

  /*
  QModelIndex idx=modello->index(numRighe,0);
  articoliList->setIndexWidget(idx,creaDestinazioneBox());
  QModelIndex idx=modello->index(numRighe,1);
  QPushButton* cancBtn=new QPushButton("Rimuove");
  connect(cancBtn,SIGNAL(clicked()),this,SLOT(on_cancellaBtn_clicked()));

  articoliList->setIndexWidget(idx,cancBtn);
  */
  articoliList->setCurrentIndex(modello->index(numRighe,0));
  articoliList->hideColumn(1);
  aggiornaArticolo();
  articoliList->setFocus();

}

void DettagliArticolo::on_cancellaBtn_clicked()
{
  int currentRow=articoliList->currentIndex().row();
  articoliList->model()->removeRow(currentRow);
  articoliList->setCurrentIndex(modello->index(currentRow-1,0));
  aggiornaArticolo();
}


void DettagliArticolo::creaDestinazioneBox(QComboBox* cb)
{
  articoliMenuModello->clear();
  QSqlQuery stmt("select idarticolo,descrizione from articoli where gestioneMenu='false' order by descrizione asc");
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }

  while(stmt.next()) {
    QString id=stmt.value(0).toString();
    QString nome=stmt.value(1).toString();
    int numRighe=articoliMenuModello->rowCount();
    articoliMenuModello->insertRow(numRighe);
    QStandardItem* item=new QStandardItem(id);
    articoliMenuModello->setItem(numRighe,1,item);
    item=new QStandardItem(nome);
    articoliMenuModello->setItem(numRighe,0,item);
  }

  articoliBox->setCurrentIndex(0);
}

void DettagliArticolo::on_articoliList_customContextMenuRequested(const QPoint &pos)
{
  if(!articoliList->selectionModel()->currentIndex().isValid()) {
    return;
  }
  QMenu* ctxMenu=new QMenu;
  QAction* cancellaAction=new QAction("Cancella riga",this);
  connect(cancellaAction,SIGNAL(triggered()),this,SLOT(on_cancellaBtn_clicked()));
  ctxMenu->addAction(cancellaAction);
  ctxMenu->move(articoliList->mapToGlobal(pos));
  ctxMenu->exec();
}
