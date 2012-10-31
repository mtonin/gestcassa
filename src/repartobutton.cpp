#include <QtSql>
#include <QMessageBox>
#include "repartobutton.h"
#include "dettagliorepartodlg.h"

RepartoButton::RepartoButton(int id, QString nome, QWidget *parent) :
  idReparto(id),
  nomeReparto(nome),
  QWidget(parent)
{
  setupUi(this);

  QSqlDatabase db=QSqlDatabase::database();
  QSqlQuery stmt;
  stmt.prepare("select * from reparti where idreparto=?");
  stmt.addBindValue(idReparto);
  if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            db.lastError().text());
      return;
    }
  int numColDescr=stmt.record().indexOf("descrizione");
  int numColFont=stmt.record().indexOf("font");
  int numColColoreSfondo=stmt.record().indexOf("coloresfondo");
  if(stmt.next()) {
      pushButton->setText(stmt.value(numColDescr).toString());
      QFont currentFont;
      currentFont.fromString(stmt.value(numColFont).toString());
      pushButton->setFont(currentFont);
      coloreSfondo=stmt.value(numColColoreSfondo).toString();
      QString stile=QString("background-color: %1;").arg(coloreSfondo);
      pushButton->setStyleSheet(stile);
    }
  else {
      pushButton->setText(nomeReparto);
    }

  QAction* dettagliAction=new QAction("Modifica",this);
  QAction* cancellaAction=new QAction("Cancella",this);
  addAction(dettagliAction);
  addAction(cancellaAction);
  setContextMenuPolicy(Qt::ActionsContextMenu);

  connect(dettagliAction,SIGNAL(triggered()),this,SLOT(dettagliShow()));
  connect(cancellaAction,SIGNAL(triggered()),this,SLOT(cancella()));

}

void RepartoButton::dettagliShow()
{

  DettaglioRepartoDlg dlg;
  dlg.setCurrentFont(pushButton->font());
  dlg.setTesto(nomeReparto);
  dlg.setCurrentColor(coloreSfondo);
  if(dlg.showAtMousePosition()) {

    QString coloreSfondo=dlg.getCurrentColor();
    QFont currentFont=dlg.getCurrentFont();

    QSqlDatabase db=QSqlDatabase::database();
    QSqlQuery stmt;
    stmt.prepare("update reparti set descrizione=?,font=?,coloresfondo=? where idreparto=?");
    stmt.addBindValue(dlg.getDescrizione());
    stmt.addBindValue(currentFont.toString());
    stmt.addBindValue(coloreSfondo);
    stmt.addBindValue(idReparto);
    if(!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              db.lastError().text());
        return;
      }

    pushButton->setText(dlg.getDescrizione());
    pushButton->setFont(currentFont);

    QString stile=QString("background-color: %1;").arg(coloreSfondo);
    pushButton->setStyleSheet(stile);
  }
}

void RepartoButton::cancella()
{
  emit cancellaReparto(this);
}

