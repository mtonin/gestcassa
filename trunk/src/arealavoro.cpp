#include <QtSql>
#include <QMessageBox>
#include "arealavoro.h"
#include "repartobtnwidget.h"
#include "elementobtnwidget.h"
#include <QPictureButton.h>

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
  hbox->setSpacing(5);
  QSqlQuery query("select * from reparti");
  query.exec();
  int repartoNum=query.record().indexOf("descrizione");
  int idRepNum=query.record().indexOf("idreparto");
  int idColore=query.record().indexOf("coloresfondo");
  if(query.isActive()) {
      while(query.next()) {
          QString repartoDescr=query.value(repartoNum).toString();
          int idRep=query.value(idRepNum).toInt();
          QString colore=query.value(idColore).toString();
          creaRepartoBtn(idRep,repartoDescr,colore);
        }
    }
  setContextMenuPolicy(Qt::ActionsContextMenu);
  articoliBox->setCurrentIndex(0);
  articoliBox->setCurrentIndex(0);

}

void AreaLavoro::creaRepartoBtn(int id, QString nome,QString colore)
{
  if(nome.isEmpty())
    nome="REPARTO";
  RepartoBtnWidget* newButton=new RepartoBtnWidget(id);
  newButton->SetButtonColorNormal(QColor(colore));

  repartiBox->layout()->addWidget(newButton);

  QWidget* pagina=creaNuovaPagina(nome);
  articoliBox->addWidget(pagina);
  articoliBox->setCurrentWidget(pagina);
  cambiaColore(colore);
  pagineArticoli.insert(id,pagina);

  connect(newButton,SIGNAL(cancellaReparto(RepartoBtnWidget*)),this,SLOT(cancellaReparto(RepartoBtnWidget*)));
  connect(newButton,SIGNAL(clicked()),this,SLOT(attivaReparto()));
  connect(newButton,SIGNAL(cambiaColore(QString)),this,SLOT(cambiaColore(QString)));

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
          QString nomeBtn=QString("%1").arg(riga*6+col+1);
          ElementoBtnWidget* btn=new ElementoBtnWidget(riga*6+col+1,nomeBtn);
          //btn->setText(nomeBtn);
          btn->setText("");
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


  creaRepartoBtn(idReparto,nome,"grey");
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

void AreaLavoro::cambiaColore(QString colore)
{
  QString stile=QString("background-color: %1;").arg(colore);
  //articoliBox->currentWidget()->setStyleSheet(stile);
  QObjectList listaBtn=articoliBox->currentWidget()->children();
  foreach (QObject* btnObj,listaBtn) {
      QPictureButton* btn=qobject_cast<QPictureButton*>(btnObj);
      if(btn) {
        btn->SetButtonColorNormal(QColor(colore));
        btn->SetButtonColorHot(QColor(colore));
        //btn->SetButtonColorPushed(QColor(colore));
        }
    }
}
