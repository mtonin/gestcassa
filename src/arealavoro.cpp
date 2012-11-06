#include <QtSql>
#include <QMessageBox>
#include "arealavoro.h"
#include "repartobtnwidget.h"

AreaLavoro::AreaLavoro(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  QHBoxLayout* hbox=new QHBoxLayout(repartiBox);
  hbox->setSpacing(0);

  creaMenu();
}

void AreaLavoro::caricaDaDatabase()
{
  QHBoxLayout* hbox=qobject_cast<QHBoxLayout*>(repartiBox->layout());
  QLayoutItem* child;
  while((child=hbox->takeAt(0))!=0) {
      child->widget()->hide();
      delete child;
    }
  delete hbox;

  hbox=new QHBoxLayout(repartiBox);
  hbox->setSpacing(0);
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

void AreaLavoro::creaRepartoBtn(int id, QString nome)
{
  if(nome.isEmpty())
    nome="REPARTO";
  RepartoBtnWidget* newButton=new RepartoBtnWidget(id,nome);

  repartiBox->layout()->addWidget(newButton);

  QWidget* pagina=creaNuovaPagina(nome);
  articoliBox->addWidget(pagina);
  pagineArticoli.insert(id,pagina);
  connect(newButton,SIGNAL(cancellaReparto(RepartoBtnWidget*)),this,SLOT(cancellaReparto(RepartoBtnWidget*)));
  connect(newButton,SIGNAL(clicked()),this,SLOT(attivaReparto()));

  //connect(newButton,SIGNAL(onClickReparto(RepartoButton*)),this,SIGNAL(onSelezioneReparto(RepartoButton*)));
  //emit onNuovoReparto();

  /*
  QAction* cancellaButtonAction=new QAction("Cancella",this);
  connect(cancellaButtonAction,SIGNAL(triggered()),this,SLOT(cancellaReparto()));
  newButton->addAction(cancellaButtonAction);
  newButton->setContextMenuPolicy(Qt::ActionsContextMenu);
  */

}

QWidget *AreaLavoro::creaNuovaPagina(QString nome)
{
  QGridLayout* griglia=new QGridLayout;
  griglia->setSpacing(1);
  for(int riga=0;riga<5;riga++) {
      for(int col=0;col<6;col++) {
          QString nomeBtn=QString("%1%2").arg(nome).arg(riga*6+col);
          QPushButton* btn=new QPushButton(nomeBtn);
          btn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
          griglia->addWidget(btn,riga,col);
        }
    }
  QFrame* pagina=new QFrame;
  pagina->setLayout(griglia);
  return pagina;
}

void AreaLavoro::creaMenu()
{
  QAction* newButtobAction=new QAction("Nuovo",this);
  addAction(newButtobAction);
  connect(newButtobAction,SIGNAL(triggered()),this,SLOT(creaNewReparto()));

}

void AreaLavoro::creaNewReparto()
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

void AreaLavoro::cancellaReparto(RepartoBtnWidget* reparto)
{
  QSqlDatabase db=QSqlDatabase::database();
  QSqlQuery stmt;
  stmt.prepare("delete from reparti where idreparto=?");
  stmt.addBindValue(reparto->getId());
  if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            db.lastError().text());
      return;
    }

  reparto->deleteLater();

}

void AreaLavoro::attivaReparto()
{
  RepartoBtnWidget* btn=qobject_cast<RepartoBtnWidget*>(sender());
  articoliBox->setCurrentWidget(pagineArticoli.value(btn->getId()));
}
