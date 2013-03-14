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

void ArticoloBtnWidget::setPrezzo(float nuovoPrezzo)
{
  prezzo=nuovoPrezzo;
}

void ArticoloBtnWidget::setButtonFont(const QFont &font)
{
  setFont(font);
}

ArticoloBtnWidget::ArticoloBtnWidget(int id,int numRiga, int numColonna,QWidget *parent) :
  idReparto(id),
  riga(numRiga),
  colonna(numColonna),
  QPictureButton(parent)
{
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
  int numColIdArticolo=stmt.record().indexOf("idarticolo");
  int numColDescr=stmt.record().indexOf("descrizione");
  int numColprezzo=stmt.record().indexOf("prezzo");
  int numColAbilitato=stmt.record().indexOf("abilitato");
  if(stmt.next()) {
    idArticolo=stmt.value(numColIdArticolo).toInt();
    nomeArticolo=stmt.value(numColDescr).toString();
    prezzo=stmt.value(numColprezzo).toFloat();
    abilitato=stmt.value(numColAbilitato).toBool();
  } else {
    idArticolo=riga*6+colonna+1;
    nomeArticolo=QString("ARTICOLO %1").arg(riga*6+colonna+1);
    prezzo=0;
    abilitato=true;
  }
  setText(nomeArticolo);

  QSizePolicy buttonSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  buttonSizePolicy.setHorizontalStretch(0);
  buttonSizePolicy.setVerticalStretch(0);
  buttonSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
  setSizePolicy(buttonSizePolicy);

}
