#include "dettagliarticolo.h"
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

  prezzoArticolo->setText(QString("%L1").arg(articoloBtn->getPrezzo(),4,'f',2));
  disattivaFlag->setChecked(!articoloBtn->getAbilitato());

  destinazioneBox->clear();
  QSqlQuery stmt("select nome from destinazioniStampa order by nome");
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
    destinazioneBox->setCurrentIndex(-1);
    articoloBtn->setRepartoStampa(NULL);
  }

  menuBox->setChecked(articoloBtn->isGestioneMenu());
  destinazioneBox->setEnabled(!articoloBtn->isGestioneMenu());
  stmt.prepare("select a.idarticolomenu,b.descrizione from articolimenu a, articoli b where a.idarticolomenu=b.idarticolo and a.idarticolo=?");
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

  testoArticolo->selectAll();
  testoArticolo->setFocus();
}

void DettagliArticolo::aggiornaArticolo()
{

  QSqlQuery stmt1;
  if(0==articoloBtn->getId()) {
    stmt1.prepare("insert into articoli (descrizione,prezzo,destinazione,gestioneMenu) values(?,?,?,?)");
  } else {
    stmt1.prepare("update articoli set descrizione=?,prezzo=?,destinazione=?,gestioneMenu=? where idarticolo=?");
  }
  stmt1.addBindValue(articoloBtn->getNomeArticolo());
  stmt1.addBindValue(articoloBtn->getPrezzo());
  stmt1.addBindValue(articoloBtn->getRepartoStampa());
  stmt1.addBindValue(menuBox->isChecked());
  if(articoloBtn->getId()>0) {
    stmt1.addBindValue(articoloBtn->getId());
  }
  stmt1.exec();
  if(!stmt1.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt1.lastError().text());
    return;
  }
  if(0==articoloBtn->getId()) {
    stmt1.exec("select last_insert_rowid()");
    stmt1.exec();
    if(!stmt1.isActive()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt1.lastError().text());
      return;
    }
    if(stmt1.next())
      articoloBtn->setId(stmt1.value(0).toInt());
  }

  stmt1.prepare("insert or replace into pulsanti (idreparto,riga,colonna,abilitato,idarticolo) values(?,?,?,?,?)");
  stmt1.addBindValue(articoloBtn->getIdReparto());
  stmt1.addBindValue(articoloBtn->getRiga());
  stmt1.addBindValue(articoloBtn->getColonna());
  stmt1.addBindValue(!disattivaFlag->isChecked());
  stmt1.addBindValue(articoloBtn->getId());
  stmt1.exec();
  if(!stmt1.isActive()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt1.lastError().text());
    return;
  }

  if(articoloBtn->isGestioneMenu()) {
    stmt1.prepare("delete from articolimenu where idarticolo=?");
    stmt1.addBindValue(articoloBtn->getId());
    stmt1.exec();
    if(!stmt1.isActive()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt1.lastError().text());
      return;
    }
    for(int i=0;i<modello->rowCount();i++) {
      stmt1.prepare("insert into articolimenu (idarticolo,idarticolomenu) values(?,?)");
      int idarticolo=modello->index(i,1).data().toInt();
      stmt1.addBindValue(articoloBtn->getId());
      stmt1.addBindValue(idarticolo);
      stmt1.exec();
      if(!stmt1.isActive()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              stmt1.lastError().text());
        return;
      }
    }
  }

}

void DettagliArticolo::reset()
{
  modello->clear();
  creaSelezioneArticoloBox();
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
  if(checked) {
      destinazioneBox->setCurrentIndex(-1);
      articoloBtn->setRepartoStampa(NULL);
  }
  aggiornaArticolo();

}

void DettagliArticolo::on_nuovoBtn_clicked()
{
  //int numRighe=articoliList->model()->rowCount();
  //modello->insertRow(numRighe);
  QString nome=articoliMenuModello->index(articoliBox->currentIndex(),0).data().toString();
  QString idarticolo=articoliMenuModello->index(articoliBox->currentIndex(),1).data().toString();

  QList<QStandardItem*> listaItems=modello->findItems(idarticolo,Qt::MatchExactly,1);
  QStandardItem* item;
  if(0==listaItems.size()) {
    item=new QStandardItem(nome);
    listaItems.append(item);
    item=new QStandardItem(idarticolo);
    listaItems.append(item);
    modello->appendRow(listaItems);
    articoliList->hideColumn(1);
    aggiornaArticolo();
  } else {
    item=listaItems.at(0);
  }
  articoliList->setCurrentIndex(modello->indexFromItem(item));
  articoliList->setFocus();

}

void DettagliArticolo::creaSelezioneArticoloBox()
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

void DettagliArticolo::on_eliminaBtn_clicked()
{
  QSqlDatabase db=QSqlDatabase::database();
  db.transaction();

  if(articoloBtn->getId()>0) {
    QSqlQuery stmt("delete from articolimenu where idarticolo=?");
    stmt.addBindValue(articoloBtn->getId());
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }

    stmt.prepare("delete from pulsanti where idreparto=? and riga=? and colonna=?");
    stmt.addBindValue(articoloBtn->getIdReparto());
    stmt.addBindValue(articoloBtn->getRiga());
    stmt.addBindValue(articoloBtn->getColonna());
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
      db.rollback();
      return;
    }

    stmt.prepare("delete from articoli where idarticolo=?");
    stmt.addBindValue(articoloBtn->getId());
    if(!stmt.exec()) {
      QMessageBox::critical(this,"Errore","Impossibile cancellare l'articolo. Verificare se è inserito in un menù.");
      db.rollback();
      return;
    }

    db.commit();

    articoloBtn->setNomeArticolo("");
    articoloBtn->setPrezzo(0);
    articoloBtn->setAbilitato(true);
    articoloBtn->setRepartoStampa("");
    articoloBtn->setGestioneMenu(false);
    articoloBtn->setId(0);
    setCurrentArticolo(articoloBtn);
    //emit eliminaPulsanteCorrente(articoloBtn);

  }
}

void DettagliArticolo::rimuoveArticolo()
{
  int currentRow=articoliList->currentIndex().row();
  articoliList->model()->removeRow(currentRow);
  articoliList->setCurrentIndex(modello->index(currentRow-1,0));
  aggiornaArticolo();
}

void DettagliArticolo::on_articoliList_clicked(const QModelIndex &index)
{
  QMenu* ctxMenu=new QMenu;
  QAction* cancellaAction=new QAction("Rimuove articolo",this);
  connect(cancellaAction,SIGNAL(triggered()),this,SLOT(rimuoveArticolo()));
  ctxMenu->addAction(cancellaAction);
  QPoint pos=QCursor::pos();
  ctxMenu->move(pos);
  ctxMenu->exec();

}

void DettagliArticolo::on_prezzoArticolo_editingFinished()
{
    prezzoArticolo->setText(QString("%L1").arg(articoloBtn->getPrezzo(),4,'f',2));
}
