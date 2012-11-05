#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>
#include <QtSql>
#include <QMessageBox>
#include "repartibox.h"
#include "repartobutton.h"

RepartiBox::RepartiBox(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  QHBoxLayout* hbox=new QHBoxLayout(box);
  hbox->setSpacing(0);

  creaMenu();

}

void RepartiBox::creaMenu()
{
  QAction* newButtobAction=new QAction("Nuovo",this);
  addAction(newButtobAction);
  connect(newButtobAction,SIGNAL(triggered()),this,SLOT(creaNewReparto()));

}

void RepartiBox::caricaDaDatabase()
{
  QHBoxLayout* hbox=qobject_cast<QHBoxLayout*>(box->layout());
  QLayoutItem* child;
  while((child=hbox->takeAt(0))!=0) {
      child->widget()->hide();
      delete child;
    }
  delete hbox;

  hbox=new QHBoxLayout(box);
  hbox->setSpacing(6);
  QSqlQuery query("select * from reparti");
  query.exec();
  int repartoNum=query.record().indexOf("descrizione");
  int idRepNum=query.record().indexOf("idreparto");
  if(query.isActive()) {
      while(query.next()) {
          QString repartoDescr=query.value(repartoNum).toString();
          int idRep=query.value(idRepNum).toInt();
          creaRepartoBtn(idRep,repartoDescr);
        }
    }
  setContextMenuPolicy(Qt::ActionsContextMenu);

}

void RepartiBox::creaRepartoBtn(int id, QString nome)
{
  if(nome.isEmpty())
    nome="REPARTO";
  RepartoButton* newButton=new RepartoButton(id,nome);

  box->layout()->addWidget(newButton);
  connect(newButton,SIGNAL(cancellaReparto(RepartoButton*)),this,SLOT(cancellaReparto(RepartoButton*)));
  connect(newButton,SIGNAL(onClickReparto(RepartoButton*)),this,SIGNAL(onSelezioneReparto(RepartoButton*)));
  emit onNuovoReparto();

  /*
  QAction* cancellaButtonAction=new QAction("Cancella",this);
  connect(cancellaButtonAction,SIGNAL(triggered()),this,SLOT(cancellaReparto()));
  newButton->addAction(cancellaButtonAction);
  newButton->setContextMenuPolicy(Qt::ActionsContextMenu);
  */

}

void RepartiBox::cancellaReparto(RepartoButton* btn)
{
  QSqlDatabase db=QSqlDatabase::database();
  QSqlQuery stmt;
  stmt.prepare("delete from reparti where idreparto=?");
  stmt.addBindValue(btn->getId());
  if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            db.lastError().text());
      return;
    }
  btn->close();
  emit onCancellazioneReparto();
}

void RepartiBox::creaNewReparto()
{
  QSqlDatabase db=QSqlDatabase::database();
  db.transaction();
  QSqlQuery query("select max(idreparto) from reparti");
  query.exec();
  if(!query.isActive()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            db.lastError().text());
      return;

    }
  query.first();
  int idReparto=query.value(0).toInt();

  QString nome("REPARTO");
  QSqlQuery insertSql;
  insertSql.prepare("insert into reparti (idreparto,descrizione) values(?,?)");
  insertSql.addBindValue(++idReparto);
  insertSql.addBindValue(nome);
  if(insertSql.exec())
    db.commit();
  else {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            db.lastError().text());
      db.rollback();
      return;
    }


  creaRepartoBtn(idReparto,nome);
}
