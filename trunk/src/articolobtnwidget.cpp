#include "articolobtnwidget.h"
#include <QtSql>

ArticoloBtnWidget::ArticoloBtnWidget(QWidget *parent) :
  QPictureButton(parent)
{
}

void ArticoloBtnWidget::setNomeArticolo(const QString nome){
  nomeArticolo=nome;
  setText(nomeArticolo);
}

ArticoloBtnWidget::ArticoloBtnWidget(int id,int numRiga, int numColonna,QString nome,QWidget *parent) :
  idReparto(id),
  riga(numRiga),
  colonna(numColonna),
  nomeArticolo(nome),
  QPictureButton(parent)
{
  setText(nome);
  QSqlQuery stmt;
  stmt.prepare("select * from articoli where idreparto=? and riga=? and colonna=?");
  stmt.addBindValue(idReparto);
  stmt.addBindValue(riga);
  stmt.addBindValue(colonna);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  int numColDescr=stmt.record().indexOf("descrizione");
  int numColprezzo=stmt.record().indexOf("prezzo");
  if(stmt.next()) {
    nomeArticolo=stmt.value(numColDescr).toString();
  }
  setText(nomeArticolo);

  QAction* dettagliAction=new QAction("Modifica",this);
  dettagliAction->setObjectName("dettagliAction");
  addAction(dettagliAction);

  QAction* cancellaAction=new QAction("Cancella",this);
  cancellaAction->setObjectName("cancellaAction");
  addAction(cancellaAction);
  setContextMenuPolicy(Qt::ActionsContextMenu);

  QSizePolicy buttonSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  buttonSizePolicy.setHorizontalStretch(0);
  buttonSizePolicy.setVerticalStretch(0);
  buttonSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
  setSizePolicy(buttonSizePolicy);

  QMetaObject::connectSlotsByName(this);
}
